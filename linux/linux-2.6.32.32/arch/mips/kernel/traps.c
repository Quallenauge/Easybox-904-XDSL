/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 1999, 2000, 01, 06 Ralf Baechle
 * Copyright (C) 1995, 1996 Paul M. Antoine
 * Copyright (C) 1998 Ulf Carlsson
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000, 01 MIPS Technologies, Inc.
 * Copyright (C) 2002, 2003, 2004, 2005, 2007  Maciej W. Rozycki
 */
#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/bootmem.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/kgdb.h>
#include <linux/kdebug.h>

#include <asm/bootinfo.h>
#include <asm/branch.h>
#include <asm/break.h>
#include <asm/cpu.h>
#include <asm/dsp.h>
#include <asm/fpu.h>
#include <asm/fpu_emulator.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/module.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>
#include <asm/sections.h>
#include <asm/system.h>
#include <asm/tlbdebug.h>
#include <asm/traps.h>
#include <asm/uaccess.h>
#include <asm/watch.h>
#include <asm/mmu_context.h>
#include <asm/types.h>
#include <asm/stacktrace.h>
#include <asm/irq.h>

extern void check_wait(void);
extern asmlinkage void r4k_wait(void);
extern asmlinkage void rollback_handle_int(void);
extern asmlinkage void handle_int(void);
extern asmlinkage void handle_tlbm(void);
extern asmlinkage void handle_tlbl(void);
extern asmlinkage void handle_tlbs(void);
extern asmlinkage void handle_adel(void);
extern asmlinkage void handle_ades(void);
extern asmlinkage void handle_ibe(void);
extern asmlinkage void handle_dbe(void);
extern asmlinkage void handle_sys(void);
extern asmlinkage void handle_bp(void);
extern asmlinkage void handle_ri(void);
extern asmlinkage void handle_ri_rdhwr_vivt(void);
extern asmlinkage void handle_ri_rdhwr(void);
extern asmlinkage void handle_cpu(void);
extern asmlinkage void handle_ov(void);
extern asmlinkage void handle_tr(void);
extern asmlinkage void handle_fpe(void);
extern asmlinkage void handle_mdmx(void);
extern asmlinkage void handle_watch(void);
extern asmlinkage void handle_mt(void);
extern asmlinkage void handle_dsp(void);
extern asmlinkage void handle_mcheck(void);
extern asmlinkage void handle_reserved(void);

extern int fpu_emulator_cop1Handler(struct pt_regs *xcp,
	struct mips_fpu_struct *ctx, int has_fpu);

#ifdef CONFIG_CPU_CAVIUM_OCTEON
extern asmlinkage void octeon_cop2_restore(struct octeon_cop2_state *task);
#endif

void (*board_be_init)(void);
int (*board_be_handler)(struct pt_regs *regs, int is_fixup);
void (*board_nmi_handler_setup)(void);
void (*board_ejtag_handler_setup)(void);
void (*board_bind_eic_interrupt)(int irq, int regset);


static void show_raw_backtrace(unsigned long reg29)
{
	unsigned long *sp = (unsigned long *)(reg29 & ~3);
	unsigned long addr;

	printk("Call Trace:");
#ifdef CONFIG_KALLSYMS
	printk("\n");
#endif
	while (!kstack_end(sp)) {
		unsigned long __user *p =
			(unsigned long __user *)(unsigned long)sp++;
		if (__get_user(addr, p)) {
			printk(" (Bad stack address)");
			break;
		}
		if (__kernel_text_address(addr))
			print_ip_sym(addr);
	}
	printk("\n");
}

#ifdef CONFIG_KALLSYMS
int raw_show_trace;
static int __init set_raw_show_trace(char *str)
{
	raw_show_trace = 1;
	return 1;
}
__setup("raw_show_trace", set_raw_show_trace);
#endif

static void show_backtrace(struct task_struct *task, const struct pt_regs *regs)
{
	unsigned long sp = regs->regs[29];
	unsigned long ra = regs->regs[31];
	unsigned long pc = regs->cp0_epc;

	if (raw_show_trace || !__kernel_text_address(pc)) {
		show_raw_backtrace(sp);
		return;
	}
	printk("Call Trace:\n");
	do {
		print_ip_sym(pc);
		pc = unwind_stack(task, &sp, pc, &ra);
	} while (pc);
	printk("\n");
}

/*
 * This routine abuses get_user()/put_user() to reference pointers
 * with at least a bit of error checking ...
 */
static void show_stacktrace(struct task_struct *task,
	const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	long stackdata;
	int i;
	unsigned long __user *sp = (unsigned long __user *)regs->regs[29];

	printk("Stack :");
	i = 0;
	while ((unsigned long) sp & (PAGE_SIZE - 1)) {
		if (i && ((i % (64 / field)) == 0))
			printk("\n       ");
		if (i > 39) {
			printk(" ...");
			break;
		}

		if (__get_user(stackdata, sp++)) {
			printk(" (Bad stack address)");
			break;
		}

		printk(" %0*lx", field, stackdata);
		i++;
	}
	printk("\n");
	show_backtrace(task, regs);
}

void show_stack(struct task_struct *task, unsigned long *sp)
{
	struct pt_regs regs;
	if (sp) {
		regs.regs[29] = (unsigned long)sp;
		regs.regs[31] = 0;
		regs.cp0_epc = 0;
	} else {
		if (task && task != current) {
			regs.regs[29] = task->thread.reg29;
			regs.regs[31] = 0;
			regs.cp0_epc = task->thread.reg31;
		} else {
			prepare_frametrace(&regs);
		}
	}
	show_stacktrace(task, &regs);
}

/*
 * The architecture-independent dump_stack generator
 */
void dump_stack(void)
{
	struct pt_regs regs;

	prepare_frametrace(&regs);
	show_backtrace(current, &regs);
}

EXPORT_SYMBOL(dump_stack);

static void show_code(unsigned int __user *pc)
{
	long i;
	unsigned short __user *pc16 = NULL;

	printk("\nCode:");

	if ((unsigned long)pc & 1)
		pc16 = (unsigned short __user *)((unsigned long)pc & ~1);
	for(i = -3 ; i < 6 ; i++) {
		unsigned int insn;
		if (pc16 ? __get_user(insn, pc16 + i) : __get_user(insn, pc + i)) {
			printk(" (Bad address in epc)\n");
			break;
		}
		printk("%c%0*x%c", (i?' ':'<'), pc16 ? 4 : 8, insn, (i?' ':'>'));
	}
}

static void __show_regs(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int cause = regs->cp0_cause;
	int i;

	printk("Cpu %d\n", smp_processor_id());

	/*
	 * Saved main processor registers
	 */
	for (i = 0; i < 32; ) {
		if ((i % 4) == 0)
			printk("$%2d   :", i);
		if (i == 0)
			printk(" %0*lx", field, 0UL);
		else if (i == 26 || i == 27)
			printk(" %*s", field, "");
		else
			printk(" %0*lx", field, regs->regs[i]);

		i++;
		if ((i % 4) == 0)
			printk("\n");
	}

#ifdef CONFIG_CPU_HAS_SMARTMIPS
	printk("Acx    : %0*lx\n", field, regs->acx);
#endif
	printk("Hi    : %0*lx\n", field, regs->hi);
	printk("Lo    : %0*lx\n", field, regs->lo);

	/*
	 * Saved cp0 registers
	 */
	printk("epc   : %0*lx %pS\n", field, regs->cp0_epc,
	       (void *) regs->cp0_epc);
	printk("    %s\n", print_tainted());
	printk("ra    : %0*lx %pS\n", field, regs->regs[31],
	       (void *) regs->regs[31]);

	printk("Status: %08x    ", (uint32_t) regs->cp0_status);

	if (current_cpu_data.isa_level == MIPS_CPU_ISA_I) {
		if (regs->cp0_status & ST0_KUO)
			printk("KUo ");
		if (regs->cp0_status & ST0_IEO)
			printk("IEo ");
		if (regs->cp0_status & ST0_KUP)
			printk("KUp ");
		if (regs->cp0_status & ST0_IEP)
			printk("IEp ");
		if (regs->cp0_status & ST0_KUC)
			printk("KUc ");
		if (regs->cp0_status & ST0_IEC)
			printk("IEc ");
	} else {
		if (regs->cp0_status & ST0_KX)
			printk("KX ");
		if (regs->cp0_status & ST0_SX)
			printk("SX ");
		if (regs->cp0_status & ST0_UX)
			printk("UX ");
		switch (regs->cp0_status & ST0_KSU) {
		case KSU_USER:
			printk("USER ");
			break;
		case KSU_SUPERVISOR:
			printk("SUPERVISOR ");
			break;
		case KSU_KERNEL:
			printk("KERNEL ");
			break;
		default:
			printk("BAD_MODE ");
			break;
		}
		if (regs->cp0_status & ST0_ERL)
			printk("ERL ");
		if (regs->cp0_status & ST0_EXL)
			printk("EXL ");
		if (regs->cp0_status & ST0_IE)
			printk("IE ");
	}
	printk("\n");

	printk("Cause : %08x\n", cause);

	cause = (cause & CAUSEF_EXCCODE) >> CAUSEB_EXCCODE;
	if (1 <= cause && cause <= 5)
		printk("BadVA : %0*lx\n", field, regs->cp0_badvaddr);

	printk("PrId  : %08x (%s)\n", read_c0_prid(),
	       cpu_name_string());
}

/*
 * FIXME: really the generic show_regs should take a const pointer argument.
 */
void show_regs(struct pt_regs *regs)
{
	__show_regs((struct pt_regs *)regs);
}

void show_registers(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);

	__show_regs(regs);
	print_modules();
	printk("Process %s (pid: %d, threadinfo=%p, task=%p, tls=%0*lx)\n",
	       current->comm, current->pid, current_thread_info(), current,
	      field, current_thread_info()->tp_value);
	if(current->real_parent != NULL){
		struct task_struct *parent = current->real_parent;
		printk("Real Parent Process %s (pid: %d, task=%p)\n",
		       parent->comm
		       , parent->pid
		       , parent);
	}else{
		printk("without Real Parent Process\n");
	}
	if(current->parent != NULL){
		struct task_struct *parent = current->parent;
		printk("Parent Process %s (pid: %d, task=%p)\n",
		       parent->comm
		       , parent->pid
		       , parent);
	}else{
		printk("without Parent Process\n");
	}
	if (cpu_has_userlocal) {
		unsigned long tls;

		tls = read_c0_userlocal();
		if (tls != current_thread_info()->tp_value)
			printk("*HwTLS: %0*lx\n", field, tls);
	}

	show_stacktrace(current, regs);
	show_code((unsigned int __user *) regs->cp0_epc);
	printk("\n");
}

static DEFINE_SPINLOCK(die_lock);

void __noreturn die(const char * str, const struct pt_regs * regs)
{
	static int die_counter;
#ifdef CONFIG_MIPS_MT_SMTC
	unsigned long dvpret = dvpe();
#endif /* CONFIG_MIPS_MT_SMTC */

	console_verbose();
	spin_lock_irq(&die_lock);
	bust_spinlocks(1);
#ifdef CONFIG_MIPS_MT_SMTC
	mips_mt_regdump(dvpret);
#endif /* CONFIG_MIPS_MT_SMTC */
	printk("%s[#%d]:\n", str, ++die_counter);
	show_registers(regs);
	add_taint(TAINT_DIE);
	spin_unlock_irq(&die_lock);

	if (in_interrupt())
		panic("Fatal exception in interrupt");

	if (panic_on_oops) {
		printk(KERN_EMERG "Fatal exception: panic in 5 seconds\n");
		ssleep(5);
		panic("Fatal exception");
	}

	do_exit(SIGSEGV);
}

extern struct exception_table_entry __start___dbe_table[];
extern struct exception_table_entry __stop___dbe_table[];

__asm__(
"	.section	__dbe_table, \"a\"\n"
"	.previous			\n");

/* Given an address, look for it in the exception tables. */
static const struct exception_table_entry *search_dbe_tables(unsigned long addr)
{
	const struct exception_table_entry *e;

	e = search_extable(__start___dbe_table, __stop___dbe_table - 1, addr);
	if (!e)
		e = search_module_dbetables(addr);
	return e;
}

asmlinkage void do_be(struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	const struct exception_table_entry *fixup = NULL;
	int data = regs->cp0_cause & 4;
	int action = MIPS_BE_FATAL;

	/* XXX For now.  Fixme, this searches the wrong table ...  */
	if (data && !user_mode(regs))
		fixup = search_dbe_tables(exception_epc(regs));

	if (fixup)
		action = MIPS_BE_FIXUP;

	if (board_be_handler)
		action = board_be_handler(regs, fixup != NULL);

	switch (action) {
	case MIPS_BE_DISCARD:
		return;
	case MIPS_BE_FIXUP:
		if (fixup) {
			regs->cp0_epc = fixup->nextinsn;
			return;
		}
		break;
	default:
		break;
	}

	/*
	 * Assume it would be too dangerous to continue ...
	 */
	printk(KERN_ALERT "%s bus error, epc == %0*lx, ra == %0*lx\n",
	       data ? "Data" : "Instruction",
	       field, regs->cp0_epc, field, regs->regs[31]);
	if (notify_die(DIE_OOPS, "bus error", regs, SIGBUS, 0, 0)
	    == NOTIFY_STOP)
		return;

	die_if_kernel("Oops", regs);
	force_sig(SIGBUS, current);
}

/*
 * ll/sc, rdhwr, sync emulation
 */

#define OPCODE 0xfc000000
#define BASE   0x03e00000
#define RT     0x001f0000
#define OFFSET 0x0000ffff
#define LL     0xc0000000
#define SC     0xe0000000
#define SPEC0  0x00000000
#define SPEC3  0x7c000000
#define RD     0x0000f800
#define FUNC   0x0000003f
#define SYNC   0x0000000f
#define RDHWR  0x0000003b

/*
 * The ll_bit is cleared by r*_switch.S
 */

unsigned int ll_bit;
struct task_struct *ll_task;

static inline int simulate_ll(struct pt_regs *regs, unsigned int opcode)
{
	unsigned long value, __user *vaddr;
	long offset;

	/*
	 * analyse the ll instruction that just caused a ri exception
	 * and put the referenced address to addr.
	 */

	/* sign extend offset */
	offset = opcode & OFFSET;
	offset <<= 16;
	offset >>= 16;

	vaddr = (unsigned long __user *)
	        ((unsigned long)(regs->regs[(opcode & BASE) >> 21]) + offset);

	if ((unsigned long)vaddr & 3)
		return SIGBUS;
	if (get_user(value, vaddr))
		return SIGSEGV;

	preempt_disable();

	if (ll_task == NULL || ll_task == current) {
		ll_bit = 1;
	} else {
		ll_bit = 0;
	}
	ll_task = current;

	preempt_enable();

	regs->regs[(opcode & RT) >> 16] = value;

	return 0;
}

static inline int simulate_sc(struct pt_regs *regs, unsigned int opcode)
{
	unsigned long __user *vaddr;
	unsigned long reg;
	long offset;

	/*
	 * analyse the sc instruction that just caused a ri exception
	 * and put the referenced address to addr.
	 */

	/* sign extend offset */
	offset = opcode & OFFSET;
	offset <<= 16;
	offset >>= 16;

	vaddr = (unsigned long __user *)
	        ((unsigned long)(regs->regs[(opcode & BASE) >> 21]) + offset);
	reg = (opcode & RT) >> 16;

	if ((unsigned long)vaddr & 3)
		return SIGBUS;

	preempt_disable();

	if (ll_bit == 0 || ll_task != current) {
		regs->regs[reg] = 0;
		preempt_enable();
		return 0;
	}

	preempt_enable();

	if (put_user(regs->regs[reg], vaddr))
		return SIGSEGV;

	regs->regs[reg] = 1;

	return 0;
}

/*
 * ll uses the opcode of lwc0 and sc uses the opcode of swc0.  That is both
 * opcodes are supposed to result in coprocessor unusable exceptions if
 * executed on ll/sc-less processors.  That's the theory.  In practice a
 * few processors such as NEC's VR4100 throw reserved instruction exceptions
 * instead, so we're doing the emulation thing in both exception handlers.
 */
static int simulate_llsc(struct pt_regs *regs, unsigned int opcode)
{
	if ((opcode & OPCODE) == LL)
		return simulate_ll(regs, opcode);
	if ((opcode & OPCODE) == SC)
		return simulate_sc(regs, opcode);

	return -1;			/* Must be something else ... */
}

/*
 * Simulate trapping 'rdhwr' instructions to provide user accessible
 * registers not implemented in hardware.
 */
static int simulate_rdhwr(struct pt_regs *regs, unsigned int opcode)
{
	struct thread_info *ti = task_thread_info(current);

	if ((opcode & OPCODE) == SPEC3 && (opcode & FUNC) == RDHWR) {
		int rd = (opcode & RD) >> 11;
		int rt = (opcode & RT) >> 16;
		switch (rd) {
		case 0:		/* CPU number */
			regs->regs[rt] = smp_processor_id();
			return 0;
		case 1:		/* SYNCI length */
			regs->regs[rt] = min(current_cpu_data.dcache.linesz,
					     current_cpu_data.icache.linesz);
			return 0;
		case 2:		/* Read count register */
			regs->regs[rt] = read_c0_count();
			return 0;
		case 3:		/* Count register resolution */
			switch (current_cpu_data.cputype) {
			case CPU_20KC:
			case CPU_25KF:
				regs->regs[rt] = 1;
				break;
			default:
				regs->regs[rt] = 2;
			}
			return 0;
		case 29:
			regs->regs[rt] = ti->tp_value;
			return 0;
		default:
			return -1;
		}
	}

	/* Not ours.  */
	return -1;
}

static int simulate_sync(struct pt_regs *regs, unsigned int opcode)
{
	if ((opcode & OPCODE) == SPEC0 && (opcode & FUNC) == SYNC)
		return 0;

	return -1;			/* Must be something else ... */
}

asmlinkage void do_ov(struct pt_regs *regs)
{
	siginfo_t info;

	die_if_kernel("Integer overflow", regs);

	info.si_code = FPE_INTOVF;
	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_addr = (void __user *) regs->cp0_epc;
	force_sig_info(SIGFPE, &info, current);
}

/*
 * XXX Delayed fp exceptions when doing a lazy ctx switch XXX
 */
asmlinkage void do_fpe(struct pt_regs *regs, unsigned long fcr31)
{
	siginfo_t info;

	if (notify_die(DIE_FP, "FP exception", regs, SIGFPE, 0, 0)
	    == NOTIFY_STOP)
		return;
	die_if_kernel("FP exception in kernel code", regs);

	if (fcr31 & FPU_CSR_UNI_X) {
		int sig;

		/*
		 * Unimplemented operation exception.  If we've got the full
		 * software emulator on-board, let's use it...
		 *
		 * Force FPU to dump state into task/thread context.  We're
		 * moving a lot of data here for what is probably a single
		 * instruction, but the alternative is to pre-decode the FP
		 * register operands before invoking the emulator, which seems
		 * a bit extreme for what should be an infrequent event.
		 */
		/* Ensure 'resume' not overwrite saved fp context again. */
		lose_fpu(1);

		/* Run the emulator */
		sig = fpu_emulator_cop1Handler(regs, &current->thread.fpu, 1);

		/*
		 * We can't allow the emulated instruction to leave any of
		 * the cause bit set in $fcr31.
		 */
		current->thread.fpu.fcr31 &= ~FPU_CSR_ALL_X;

		/* Restore the hardware register state */
		own_fpu(1);	/* Using the FPU again.  */

		/* If something went wrong, signal */
		if (sig)
			force_sig(sig, current);

		return;
	} else if (fcr31 & FPU_CSR_INV_X)
		info.si_code = FPE_FLTINV;
	else if (fcr31 & FPU_CSR_DIV_X)
		info.si_code = FPE_FLTDIV;
	else if (fcr31 & FPU_CSR_OVF_X)
		info.si_code = FPE_FLTOVF;
	else if (fcr31 & FPU_CSR_UDF_X)
		info.si_code = FPE_FLTUND;
	else if (fcr31 & FPU_CSR_INE_X)
		info.si_code = FPE_FLTRES;
	else
		info.si_code = __SI_FAULT;
	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_addr = (void __user *) regs->cp0_epc;
	force_sig_info(SIGFPE, &info, current);
}

static void do_trap_or_bp(struct pt_regs *regs, unsigned int code,
	const char *str)
{
	siginfo_t info;
	char b[40];

	if (notify_die(DIE_TRAP, str, regs, code, 0, 0) == NOTIFY_STOP)
		return;

	/*
	 * A short test says that IRIX 5.3 sends SIGTRAP for all trap
	 * insns, even for trap and break codes that indicate arithmetic
	 * failures.  Weird ...
	 * But should we continue the brokenness???  --macro
	 */
	switch (code) {
	case BRK_OVERFLOW:
	case BRK_DIVZERO:
		scnprintf(b, sizeof(b), "%s instruction in kernel code", str);
		die_if_kernel(b, regs);
		if (code == BRK_DIVZERO)
			info.si_code = FPE_INTDIV;
		else
			info.si_code = FPE_INTOVF;
		info.si_signo = SIGFPE;
		info.si_errno = 0;
		info.si_addr = (void __user *) regs->cp0_epc;
		force_sig_info(SIGFPE, &info, current);
		break;
	case BRK_BUG:
		die_if_kernel("Kernel bug detected", regs);
		force_sig(SIGTRAP, current);
		break;
	case BRK_MEMU:
		/*
		 * Address errors may be deliberately induced by the FPU
		 * emulator to retake control of the CPU after executing the
		 * instruction in the delay slot of an emulated branch.
		 *
		 * Terminate if exception was recognized as a delay slot return
		 * otherwise handle as normal.
		 */
		if (do_dsemulret(regs))
			return;

		die_if_kernel("Math emu break/trap", regs);
		force_sig(SIGTRAP, current);
		break;
	default:
		scnprintf(b, sizeof(b), "%s instruction in kernel code", str);
		die_if_kernel(b, regs);
		force_sig(SIGTRAP, current);
	}
}

asmlinkage void do_bp(struct pt_regs *regs)
{
	unsigned int opcode, bcode;

	if (__get_user(opcode, (unsigned int __user *) exception_epc(regs)))
		goto out_sigsegv;

	/*
	 * There is the ancient bug in the MIPS assemblers that the break
	 * code starts left to bit 16 instead to bit 6 in the opcode.
	 * Gas is bug-compatible, but not always, grrr...
	 * We handle both cases with a simple heuristics.  --macro
	 */
	bcode = ((opcode >> 6) & ((1 << 20) - 1));
	if (bcode >= (1 << 10))
		bcode >>= 10;

	do_trap_or_bp(regs, bcode, "Break");
	return;

out_sigsegv:
	force_sig(SIGSEGV, current);
}

asmlinkage void do_tr(struct pt_regs *regs)
{
	unsigned int opcode, tcode = 0;

	if (__get_user(opcode, (unsigned int __user *) exception_epc(regs)))
		goto out_sigsegv;

	/* Immediate versions don't provide a code.  */
	if (!(opcode & OPCODE))
		tcode = ((opcode >> 6) & ((1 << 10) - 1));

	do_trap_or_bp(regs, tcode, "Trap");
	return;

out_sigsegv:
	force_sig(SIGSEGV, current);
}

asmlinkage void do_ri(struct pt_regs *regs)
{
	unsigned int __user *epc = (unsigned int __user *)exception_epc(regs);
	unsigned long old_epc = regs->cp0_epc;
	unsigned int opcode = 0;
	int status = -1;

	if (notify_die(DIE_RI, "RI Fault", regs, SIGSEGV, 0, 0)
	    == NOTIFY_STOP)
		return;

	die_if_kernel("Reserved instruction in kernel code", regs);

	if (unlikely(compute_return_epc(regs) < 0))
		return;

	if (unlikely(get_user(opcode, epc) < 0))
		status = SIGSEGV;

	if (!cpu_has_llsc && status < 0)
		status = simulate_llsc(regs, opcode);

	if (status < 0)
		status = simulate_rdhwr(regs, opcode);

	if (status < 0)
		status = simulate_sync(regs, opcode);

	if (status < 0)
		status = SIGILL;

	if (unlikely(status > 0)) {
		regs->cp0_epc = old_epc;		/* Undo skip-over.  */
		force_sig(status, current);
	}
}

/*
 * MIPS MT processors may have fewer FPU contexts than CPU threads. If we've
 * emulated more than some threshold number of instructions, force migration to
 * a "CPU" that has FP support.
 */
static void mt_ase_fp_affinity(void)
{
#ifdef CONFIG_MIPS_MT_FPAFF
	if (mt_fpemul_threshold > 0 &&
	     ((current->thread.emulated_fp++ > mt_fpemul_threshold))) {
		/*
		 * If there's no FPU present, or if the application has already
		 * restricted the allowed set to exclude any CPUs with FPUs,
		 * we'll skip the procedure.
		 */
		if (cpus_intersects(current->cpus_allowed, mt_fpu_cpumask)) {
			cpumask_t tmask;

			current->thread.user_cpus_allowed
				= current->cpus_allowed;
			cpus_and(tmask, current->cpus_allowed,
				mt_fpu_cpumask);
			set_cpus_allowed(current, tmask);
			set_thread_flag(TIF_FPUBOUND);
		}
	}
#endif /* CONFIG_MIPS_MT_FPAFF */
}

asmlinkage void do_cpu(struct pt_regs *regs)
{
	unsigned int __user *epc;
	unsigned long old_epc;
	unsigned int opcode;
	unsigned int cpid;
	int status;
	unsigned long __maybe_unused flags;

	die_if_kernel("do_cpu invoked from kernel context!", regs);

	cpid = (regs->cp0_cause >> CAUSEB_CE) & 3;

	switch (cpid) {
	case 0:
		epc = (unsigned int __user *)exception_epc(regs);
		old_epc = regs->cp0_epc;
		opcode = 0;
		status = -1;

		if (unlikely(compute_return_epc(regs) < 0))
			return;

		if (unlikely(get_user(opcode, epc) < 0))
			status = SIGSEGV;

		if (!cpu_has_llsc && status < 0)
			status = simulate_llsc(regs, opcode);

		if (status < 0)
			status = simulate_rdhwr(regs, opcode);

		if (status < 0)
			status = SIGILL;

		if (unlikely(status > 0)) {
			regs->cp0_epc = old_epc;	/* Undo skip-over.  */
			force_sig(status, current);
		}

		return;

	case 1:
		if (used_math())	/* Using the FPU again.  */
			own_fpu(1);
		else {			/* First time FPU user.  */
			init_fpu();
			set_used_math();
		}

		if (!raw_cpu_has_fpu) {
			int sig;
			sig = fpu_emulator_cop1Handler(regs,
						&current->thread.fpu, 0);
			if (sig)
				force_sig(sig, current);
			else
				mt_ase_fp_affinity();
		}

		return;

	case 2:
#ifdef CONFIG_CPU_CAVIUM_OCTEON
		prefetch(&current->thread.cp2);
		local_irq_save(flags);
		KSTK_STATUS(current) |= ST0_CU2;
		status = read_c0_status();
		write_c0_status(status | ST0_CU2);
		octeon_cop2_restore(&(current->thread.cp2));
		write_c0_status(status & ~ST0_CU2);
		local_irq_restore(flags);
		return;
#endif
	case 3:
		break;
	}

	force_sig(SIGILL, current);
}

asmlinkage void do_mdmx(struct pt_regs *regs)
{
	force_sig(SIGILL, current);
}

/*
 * Called with interrupts disabled.
 */
asmlinkage void do_watch(struct pt_regs *regs)
{
	u32 cause;

	/*
	 * Clear WP (bit 22) bit of cause register so we don't loop
	 * forever.
	 */
	cause = read_c0_cause();
	cause &= ~(1 << 22);
	write_c0_cause(cause);

	/*
	 * If the current thread has the watch registers loaded, save
	 * their values and send SIGTRAP.  Otherwise another thread
	 * left the registers set, clear them and continue.
	 */
	if (test_tsk_thread_flag(current, TIF_LOAD_WATCH)) {
		mips_read_watch_registers();
		local_irq_enable();
		force_sig(SIGTRAP, current);
	} else {
		mips_clear_watch_registers();
		local_irq_enable();
	}
}

asmlinkage void do_mcheck(struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	int multi_match = regs->cp0_status & ST0_TS;

	show_regs(regs);

	if (multi_match) {
		printk("Index   : %0x\n", read_c0_index());
		printk("Pagemask: %0x\n", read_c0_pagemask());
		printk("EntryHi : %0*lx\n", field, read_c0_entryhi());
		printk("EntryLo0: %0*lx\n", field, read_c0_entrylo0());
		printk("EntryLo1: %0*lx\n", field, read_c0_entrylo1());
		printk("\n");
		dump_tlb_all();
	}

	show_code((unsigned int __user *) regs->cp0_epc);

	/*
	 * Some chips may have other causes of machine check (e.g. SB1
	 * graduation timer)
	 */
	panic("Caught Machine Check exception - %scaused by multiple "
	      "matching entries in the TLB.",
	      (multi_match) ? "" : "not ");
}

asmlinkage void do_mt(struct pt_regs *regs)
{
	int subcode;

	subcode = (read_vpe_c0_vpecontrol() & VPECONTROL_EXCPT)
			>> VPECONTROL_EXCPT_SHIFT;
	switch (subcode) {
	case 0:
		printk(KERN_DEBUG "Thread Underflow\n");
		break;
	case 1:
		printk(KERN_DEBUG "Thread Overflow\n");
		break;
	case 2:
		printk(KERN_DEBUG "Invalid YIELD Qualifier\n");
		break;
	case 3:
		printk(KERN_DEBUG "Gating Storage Exception\n");
		break;
	case 4:
		printk(KERN_DEBUG "YIELD Scheduler Exception\n");
		break;
	case 5:
		printk(KERN_DEBUG "Gating Storage Schedulier Exception\n");
		break;
	default:
		printk(KERN_DEBUG "*** UNKNOWN THREAD EXCEPTION %d ***\n",
			subcode);
		break;
	}
	die_if_kernel("MIPS MT Thread exception in kernel", regs);

	force_sig(SIGILL, current);
}


asmlinkage void do_dsp(struct pt_regs *regs)
{
	if (cpu_has_dsp)
		panic("Unexpected DSP exception\n");

	force_sig(SIGILL, current);
}

asmlinkage void do_reserved(struct pt_regs *regs)
{
	/*
	 * Game over - no way to handle this if it ever occurs.  Most probably
	 * caused by a new unknown cpu type or after another deadly
	 * hard/software error.
	 */
	show_regs(regs);
	panic("Caught reserved exception %ld - should not happen.",
	      (regs->cp0_cause & 0x7f) >> 2);
}

static int __initdata l1parity = 1;
static int __init nol1parity(char *s)
{
	l1parity = 0;
	return 1;
}
__setup("nol1par", nol1parity);
static int __initdata l2parity = 1;
static int __init nol2parity(char *s)
{
	l2parity = 0;
	return 1;
}
__setup("nol2par", nol2parity);

/*
 * Some MIPS CPUs can enable/disable for cache parity detection, but do
 * it different ways.
 */
static inline void parity_protection_init(void)
{
	switch (current_cpu_type()) {
	case CPU_24K:
	case CPU_34K:
	case CPU_74K:
	case CPU_1004K:
		{
#define ERRCTL_PE	0x80000000
#define ERRCTL_L2P	0x00800000
			unsigned long errctl;
			unsigned int l1parity_present, l2parity_present;

			errctl = read_c0_ecc();
			errctl &= ~(ERRCTL_PE|ERRCTL_L2P);

			/* probe L1 parity support */
			write_c0_ecc(errctl | ERRCTL_PE);
			back_to_back_c0_hazard();
			l1parity_present = (read_c0_ecc() & ERRCTL_PE);

			/* probe L2 parity support */
			write_c0_ecc(errctl|ERRCTL_L2P);
			back_to_back_c0_hazard();
			l2parity_present = (read_c0_ecc() & ERRCTL_L2P);

			if (l1parity_present && l2parity_present) {
				if (l1parity)
					errctl |= ERRCTL_PE;
				if (l1parity ^ l2parity)
					errctl |= ERRCTL_L2P;
			} else if (l1parity_present) {
				if (l1parity)
					errctl |= ERRCTL_PE;
			} else if (l2parity_present) {
				if (l2parity)
					errctl |= ERRCTL_L2P;
			} else {
				/* No parity available */
			}

			printk(KERN_INFO "Writing ErrCtl register=%08lx\n", errctl);

			write_c0_ecc(errctl);
			back_to_back_c0_hazard();
			errctl = read_c0_ecc();
			printk(KERN_INFO "Readback ErrCtl register=%08lx\n", errctl);

			if (l1parity_present)
				printk(KERN_INFO "Cache parity protection %sabled\n",
				       (errctl & ERRCTL_PE) ? "en" : "dis");

			if (l2parity_present) {
				if (l1parity_present && l1parity)
					errctl ^= ERRCTL_L2P;
				printk(KERN_INFO "L2 cache parity protection %sabled\n",
				       (errctl & ERRCTL_L2P) ? "en" : "dis");
			}
		}
		break;

	case CPU_5KC:
		write_c0_ecc(0x80000000);
		back_to_back_c0_hazard();
		/* Set the PE bit (bit 31) in the c0_errctl register. */
		printk(KERN_INFO "Cache parity protection %sabled\n",
		       (read_c0_ecc() & 0x80000000) ? "en" : "dis");
		break;
	case CPU_20KC:
	case CPU_25KF:
		/* Clear the DE bit (bit 16) in the c0_status register. */
		printk(KERN_INFO "Enable cache parity protection for "
		       "MIPS 20KC/25KF CPUs.\n");
		clear_c0_status(ST0_DE);
		break;
	default:
		break;
	}
}

asmlinkage void cache_parity_error(void)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int reg_val;

	/* For the moment, report the problem and hang. */
	printk("Cache error exception:\n");
	printk("cp0_errorepc == %0*lx\n", field, read_c0_errorepc());
	reg_val = read_c0_cacheerr();
	printk("c0_cacheerr == %08x\n", reg_val);

	printk("Decoded c0_cacheerr: %s cache fault in %s reference.\n",
	       reg_val & (1<<30) ? "secondary" : "primary",
	       reg_val & (1<<31) ? "data" : "insn");
	printk("Error bits: %s%s%s%s%s%s%s\n",
	       reg_val & (1<<29) ? "ED " : "",
	       reg_val & (1<<28) ? "ET " : "",
	       reg_val & (1<<26) ? "EE " : "",
	       reg_val & (1<<25) ? "EB " : "",
	       reg_val & (1<<24) ? "EI " : "",
	       reg_val & (1<<23) ? "E1 " : "",
	       reg_val & (1<<22) ? "E0 " : "");
	printk("IDX: 0x%08x\n", reg_val & ((1<<22)-1));

#if defined(CONFIG_CPU_MIPS32) || defined(CONFIG_CPU_MIPS64)
	if (reg_val & (1<<22))
		printk("DErrAddr0: 0x%0*lx\n", field, read_c0_derraddr0());

	if (reg_val & (1<<23))
		printk("DErrAddr1: 0x%0*lx\n", field, read_c0_derraddr1());
#endif

	panic("Can't handle the cache error!");
}

/*
 * SDBBP EJTAG debug exception handler.
 * We skip the instruction and return to the next instruction.
 */
void ejtag_exception_handler(struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned long depc, old_epc;
	unsigned int debug;

	printk(KERN_DEBUG "SDBBP EJTAG debug exception - not handled yet, just ignored!\n");
	depc = read_c0_depc();
	debug = read_c0_debug();
	printk(KERN_DEBUG "c0_depc = %0*lx, DEBUG = %08x\n", field, depc, debug);
	if (debug & 0x80000000) {
		/*
		 * In branch delay slot.
		 * We cheat a little bit here and use EPC to calculate the
		 * debug return address (DEPC). EPC is restored after the
		 * calculation.
		 */
		old_epc = regs->cp0_epc;
		regs->cp0_epc = depc;
		__compute_return_epc(regs);
		depc = regs->cp0_epc;
		regs->cp0_epc = old_epc;
	} else
		depc += 4;
	write_c0_depc(depc);

#if 0
	printk(KERN_DEBUG "\n\n----- Enable EJTAG single stepping ----\n\n");
	write_c0_debug(debug | 0x100);
#endif
}

/*
 * NMI exception handler.
 */
NORET_TYPE void ATTRIB_NORET nmi_exception_handler(struct pt_regs *regs)
{
	bust_spinlocks(1);
	printk("NMI taken!!!!\n");
	die("NMI", regs);
}

#if defined(CONFIG_AR9) || defined(CONFIG_VR9)
#define VECTORSPACING 0x200     /* for EI/VI mode */
#else
#define VECTORSPACING 0x100	/* for EI/VI mode */
#endif

unsigned long ebase;
unsigned long exception_handlers[32];
unsigned long vi_handlers[64];

/*
 * As a side effect of the way this is implemented we're limited
 * to interrupt handlers in the address range from
 * KSEG0 <= x < KSEG0 + 256mb on the Nevada.  Oh well ...
 */
void *set_except_vector(int n, void *addr)
{
	unsigned long handler = (unsigned long) addr;
	unsigned long old_handler = exception_handlers[n];

	exception_handlers[n] = handler;
	if (n == 0 && cpu_has_divec) {
		*(u32 *)(ebase + 0x200) = 0x08000000 |
					  (0x03ffffff & (handler >> 2));
		local_flush_icache_range(ebase + 0x200, ebase + 0x204);
	}
	return (void *)old_handler;
}

static asmlinkage void do_default_vi(void)
{
	show_regs(get_irq_regs());
	panic("Caught unexpected vectored interrupt.");
}

static void *set_vi_srs_handler(int n, vi_handler_t addr, int srs)
{
	unsigned long handler;
	unsigned long old_handler = vi_handlers[n];
	int srssets = current_cpu_data.srsets;
	u32 *w;
	unsigned char *b;

	BUG_ON(!cpu_has_veic && !cpu_has_vint);

	if (addr == NULL) {
		handler = (unsigned long) do_default_vi;
		srs = 0;
	} else
		handler = (unsigned long) addr;
	vi_handlers[n] = (unsigned long) addr;

	b = (unsigned char *)(ebase + 0x200 + n*VECTORSPACING);

	if (srs >= srssets)
		panic("Shadow register set %d not supported", srs);

	if (cpu_has_veic) {
		if (board_bind_eic_interrupt)
			board_bind_eic_interrupt(n, srs);
	} else if (cpu_has_vint) {
		/* SRSMap is only defined if shadow sets are implemented */
		if (srssets > 1)
			change_c0_srsmap(0xf << n*4, srs << n*4);
	}

	if (srs == 0) {
		/*
		 * If no shadow set is selected then use the default handler
		 * that does normal register saving and a standard interrupt exit
		 */

		extern char except_vec_vi, except_vec_vi_lui;
		extern char except_vec_vi_ori, except_vec_vi_end;
		extern char rollback_except_vec_vi;
		char *vec_start = (cpu_wait == r4k_wait) ?
			&rollback_except_vec_vi : &except_vec_vi;
#ifdef CONFIG_MIPS_MT_SMTC
		/*
		 * We need to provide the SMTC vectored interrupt handler
		 * not only with the address of the handler, but with the
		 * Status.IM bit to be masked before going there.
		 */
		extern char except_vec_vi_mori;
		const int mori_offset = &except_vec_vi_mori - vec_start;
#endif /* CONFIG_MIPS_MT_SMTC */
		const int handler_len = &except_vec_vi_end - vec_start;
		const int lui_offset = &except_vec_vi_lui - vec_start;
		const int ori_offset = &except_vec_vi_ori - vec_start;

		if (handler_len > VECTORSPACING) {
			/*
			 * Sigh... panicing won't help as the console
			 * is probably not configured :(
			 */
			panic("VECTORSPACING too small");
		}

		memcpy(b, vec_start, handler_len);
#ifdef CONFIG_MIPS_MT_SMTC
		BUG_ON(n > 7);	/* Vector index %d exceeds SMTC maximum. */

		w = (u32 *)(b + mori_offset);
		*w = (*w & 0xffff0000) | (0x100 << n);
#endif /* CONFIG_MIPS_MT_SMTC */
		w = (u32 *)(b + lui_offset);
		*w = (*w & 0xffff0000) | (((u32)handler >> 16) & 0xffff);
		w = (u32 *)(b + ori_offset);
		*w = (*w & 0xffff0000) | ((u32)handler & 0xffff);
		local_flush_icache_range((unsigned long)b,
					 (unsigned long)(b+handler_len));
	}
	else {
		/*
		 * In other cases jump directly to the interrupt handler
		 *
		 * It is the handlers responsibility to save registers if required
		 * (eg hi/lo) and return from the exception using "eret"
		 */
		w = (u32 *)b;
		*w++ = 0x08000000 | (((u32)handler >> 2) & 0x03fffff); /* j handler */
		*w = 0;
		local_flush_icache_range((unsigned long)b,
					 (unsigned long)(b+8));
	}

	return (void *)old_handler;
}

void *set_vi_handler(int n, vi_handler_t addr)
{
	return set_vi_srs_handler(n, addr, 0);
}

/*
 * This is used by native signal handling
 */
asmlinkage int (*save_fp_context)(struct sigcontext __user *sc);
asmlinkage int (*restore_fp_context)(struct sigcontext __user *sc);

extern asmlinkage int _save_fp_context(struct sigcontext __user *sc);
extern asmlinkage int _restore_fp_context(struct sigcontext __user *sc);

extern asmlinkage int fpu_emulator_save_context(struct sigcontext __user *sc);
extern asmlinkage int fpu_emulator_restore_context(struct sigcontext __user *sc);

#ifdef CONFIG_SMP
static int smp_save_fp_context(struct sigcontext __user *sc)
{
	return raw_cpu_has_fpu
	       ? _save_fp_context(sc)
	       : fpu_emulator_save_context(sc);
}

static int smp_restore_fp_context(struct sigcontext __user *sc)
{
	return raw_cpu_has_fpu
	       ? _restore_fp_context(sc)
	       : fpu_emulator_restore_context(sc);
}
#endif

static inline void signal_init(void)
{
#ifdef CONFIG_SMP
	/* For now just do the cpu_has_fpu check when the functions are invoked */
	save_fp_context = smp_save_fp_context;
	restore_fp_context = smp_restore_fp_context;
#else
	if (cpu_has_fpu) {
		save_fp_context = _save_fp_context;
		restore_fp_context = _restore_fp_context;
	} else {
		save_fp_context = fpu_emulator_save_context;
		restore_fp_context = fpu_emulator_restore_context;
	}
#endif
}

#ifdef CONFIG_MIPS32_COMPAT

/*
 * This is used by 32-bit signal stuff on the 64-bit kernel
 */
asmlinkage int (*save_fp_context32)(struct sigcontext32 __user *sc);
asmlinkage int (*restore_fp_context32)(struct sigcontext32 __user *sc);

extern asmlinkage int _save_fp_context32(struct sigcontext32 __user *sc);
extern asmlinkage int _restore_fp_context32(struct sigcontext32 __user *sc);

extern asmlinkage int fpu_emulator_save_context32(struct sigcontext32 __user *sc);
extern asmlinkage int fpu_emulator_restore_context32(struct sigcontext32 __user *sc);

static inline void signal32_init(void)
{
	if (cpu_has_fpu) {
		save_fp_context32 = _save_fp_context32;
		restore_fp_context32 = _restore_fp_context32;
	} else {
		save_fp_context32 = fpu_emulator_save_context32;
		restore_fp_context32 = fpu_emulator_restore_context32;
	}
}
#endif

extern void cpu_cache_init(void);
extern void tlb_init(void);
extern void flush_tlb_handlers(void);

/*
 * Timer interrupt
 */
int cp0_compare_irq;

/*
 * Performance counter IRQ or -1 if shared with timer
 */
int cp0_perfcount_irq;
EXPORT_SYMBOL_GPL(cp0_perfcount_irq);

static int __cpuinitdata noulri;

static int __init ulri_disable(char *s)
{
	pr_info("Disabling ulri\n");
	noulri = 1;

	return 1;
}
__setup("noulri", ulri_disable);

void __cpuinit per_cpu_trap_init(void)
{
	unsigned int cpu = smp_processor_id();
	unsigned int status_set = ST0_CU0;
#ifdef CONFIG_MIPS_MT_SMTC
	int secondaryTC = 0;
	int bootTC = (cpu == 0);

	/*
	 * Only do per_cpu_trap_init() for first TC of Each VPE.
	 * Note that this hack assumes that the SMTC init code
	 * assigns TCs consecutively and in ascending order.
	 */

	if (((read_c0_tcbind() & TCBIND_CURTC) != 0) &&
	    ((read_c0_tcbind() & TCBIND_CURVPE) == cpu_data[cpu - 1].vpe_id))
		secondaryTC = 1;
#endif /* CONFIG_MIPS_MT_SMTC */

	/*
	 * Disable coprocessors and select 32-bit or 64-bit addressing
	 * and the 16/32 or 32/32 FPR register model.  Reset the BEV
	 * flag that some firmware may have left set and the TS bit (for
	 * IP27).  Set XX for ISA IV code to work.
	 */
#ifdef CONFIG_64BIT
	status_set |= ST0_FR|ST0_KX|ST0_SX|ST0_UX;
#endif
	if (current_cpu_data.isa_level == MIPS_CPU_ISA_IV)
		status_set |= ST0_XX;
	if (cpu_has_dsp)
		status_set |= ST0_MX;

	change_c0_status(ST0_CU|ST0_MX|ST0_RE|ST0_FR|ST0_BEV|ST0_TS|ST0_KX|ST0_SX|ST0_UX,
			 status_set);

	if (cpu_has_mips_r2) {
		unsigned int enable = 0x0000000f | cpu_hwrena_impl_bits;

		if (!noulri && cpu_has_userlocal)
			enable |= (1 << 29);

		write_c0_hwrena(enable);
	}

#ifdef CONFIG_MIPS_MT_SMTC
	if (!secondaryTC) {
#endif /* CONFIG_MIPS_MT_SMTC */

	if (cpu_has_veic || cpu_has_vint) {
		unsigned long sr = set_c0_status(ST0_BEV);
		write_c0_ebase(ebase);
		write_c0_status(sr);
		/* Setting vector spacing enables EI/VI mode  */
		change_c0_intctl(0x3e0, VECTORSPACING);
	}
	if (cpu_has_divec) {
		if (cpu_has_mipsmt) {
			unsigned int vpflags = dvpe();
			set_c0_cause(CAUSEF_IV);
			evpe(vpflags);
		} else
			set_c0_cause(CAUSEF_IV);
	}

	/*
	 * Before R2 both interrupt numbers were fixed to 7, so on R2 only:
	 *
	 *  o read IntCtl.IPTI to determine the timer interrupt
	 *  o read IntCtl.IPPCI to determine the performance counter interrupt
	 */
	if (cpu_has_mips_r2) {
		cp0_compare_irq = (read_c0_intctl() >> 29) & 7;
		cp0_perfcount_irq = (read_c0_intctl() >> 26) & 7;
		if (cp0_perfcount_irq == cp0_compare_irq)
			cp0_perfcount_irq = -1;
	} else {
		cp0_compare_irq = CP0_LEGACY_COMPARE_IRQ;
		cp0_perfcount_irq = -1;
	}

#ifdef CONFIG_MIPS_MT_SMTC
	}
#endif /* CONFIG_MIPS_MT_SMTC */

	cpu_data[cpu].asid_cache = ASID_FIRST_VERSION;
	TLBMISS_HANDLER_SETUP();

	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
	BUG_ON(current->mm);
	enter_lazy_tlb(&init_mm, current);

#ifdef CONFIG_MIPS_MT_SMTC
	if (bootTC) {
#endif /* CONFIG_MIPS_MT_SMTC */
		cpu_cache_init();
		tlb_init();
#ifdef CONFIG_MIPS_MT_SMTC
	} else if (!secondaryTC) {
		/*
		 * First TC in non-boot VPE must do subset of tlb_init()
		 * for MMU countrol registers.
		 */
		write_c0_pagemask(PM_DEFAULT_MASK);
		write_c0_wired(0);
	}
#endif /* CONFIG_MIPS_MT_SMTC */
}

/* Install CPU exception handler */
void __init set_handler(unsigned long offset, void *addr, unsigned long size)
{
	memcpy((void *)(ebase + offset), addr, size);
	local_flush_icache_range(ebase + offset, ebase + offset + size);
}

static char panic_null_cerr[] __cpuinitdata =
	"Trying to set NULL cache error exception handler";

/*
 * Install uncached CPU exception handler.
 * This is suitable only for the cache error exception which is the only
 * exception handler that is being run uncached.
 */
void __cpuinit set_uncached_handler(unsigned long offset, void *addr,
	unsigned long size)
{
#ifdef CONFIG_32BIT
	unsigned long uncached_ebase = KSEG1ADDR(ebase);
#endif
#ifdef CONFIG_64BIT
	unsigned long uncached_ebase = TO_UNCAC(ebase);
#endif

	if (!addr)
		panic(panic_null_cerr);

	memcpy((void *)(uncached_ebase + offset), addr, size);
}

static int __initdata rdhwr_noopt;
static int __init set_rdhwr_noopt(char *str)
{
	rdhwr_noopt = 1;
	return 1;
}

__setup("rdhwr_noopt", set_rdhwr_noopt);

void __init trap_init(void)
{
	extern char except_vec3_generic, except_vec3_r4000;
	extern char except_vec4;
	unsigned long i;
	int rollback;

	check_wait();
	rollback = (cpu_wait == r4k_wait);

#if defined(CONFIG_KGDB)
	if (kgdb_early_setup)
		return;	/* Already done */
#endif

	if (cpu_has_veic || cpu_has_vint) {
		unsigned long size = 0x200 + VECTORSPACING*64;
		ebase = (unsigned long)
			__alloc_bootmem(size, 1 << fls(size), 0);
	} else {
		ebase = CAC_BASE;
		if (cpu_has_mips_r2)
			ebase += (read_c0_ebase() & 0x3ffff000);
	}

	per_cpu_trap_init();

	/*
	 * Copy the generic exception handlers to their final destination.
	 * This will be overriden later as suitable for a particular
	 * configuration.
	 */
	set_handler(0x180, &except_vec3_generic, 0x80);

	/*
	 * Setup default vectors
	 */
	for (i = 0; i <= 31; i++)
		set_except_vector(i, handle_reserved);

	/*
	 * Copy the EJTAG debug exception vector handler code to it's final
	 * destination.
	 */
	if (cpu_has_ejtag && board_ejtag_handler_setup)
		board_ejtag_handler_setup();

	/*
	 * Only some CPUs have the watch exceptions.
	 */
	if (cpu_has_watch)
		set_except_vector(23, handle_watch);

	/*
	 * Initialise interrupt handlers
	 */
	if (cpu_has_veic || cpu_has_vint) {
		int nvec = cpu_has_veic ? 64 : 8;
		for (i = 0; i < nvec; i++)
			set_vi_handler(i, NULL);
	}
	else if (cpu_has_divec)
		set_handler(0x200, &except_vec4, 0x8);

	/*
	 * Some CPUs can enable/disable for cache parity detection, but does
	 * it different ways.
	 */
	parity_protection_init();

	/*
	 * The Data Bus Errors / Instruction Bus Errors are signaled
	 * by external hardware.  Therefore these two exceptions
	 * may have board specific handlers.
	 */
	if (board_be_init)
		board_be_init();

	set_except_vector(0, rollback ? rollback_handle_int : handle_int);
	set_except_vector(1, handle_tlbm);
	set_except_vector(2, handle_tlbl);
	set_except_vector(3, handle_tlbs);

	set_except_vector(4, handle_adel);
	set_except_vector(5, handle_ades);

	set_except_vector(6, handle_ibe);
	set_except_vector(7, handle_dbe);

	set_except_vector(8, handle_sys);
	set_except_vector(9, handle_bp);
	set_except_vector(10, rdhwr_noopt ? handle_ri :
			  (cpu_has_vtag_icache ?
			   handle_ri_rdhwr_vivt : handle_ri_rdhwr));
	set_except_vector(11, handle_cpu);
	set_except_vector(12, handle_ov);
	set_except_vector(13, handle_tr);

	if (current_cpu_type() == CPU_R6000 ||
	    current_cpu_type() == CPU_R6000A) {
		/*
		 * The R6000 is the only R-series CPU that features a machine
		 * check exception (similar to the R4000 cache error) and
		 * unaligned ldc1/sdc1 exception.  The handlers have not been
		 * written yet.  Well, anyway there is no R6000 machine on the
		 * current list of targets for Linux/MIPS.
		 * (Duh, crap, there is someone with a triple R6k machine)
		 */
		//set_except_vector(14, handle_mc);
		//set_except_vector(15, handle_ndc);
	}


	if (board_nmi_handler_setup)
		board_nmi_handler_setup();

	if (cpu_has_fpu && !cpu_has_nofpuex)
		set_except_vector(15, handle_fpe);

	set_except_vector(22, handle_mdmx);

	if (cpu_has_mcheck)
		set_except_vector(24, handle_mcheck);

	if (cpu_has_mipsmt)
		set_except_vector(25, handle_mt);

	set_except_vector(26, handle_dsp);

	if (cpu_has_vce)
		/* Special exception: R4[04]00 uses also the divec space. */
		memcpy((void *)(ebase + 0x180), &except_vec3_r4000, 0x100);
	else if (cpu_has_4kex)
		memcpy((void *)(ebase + 0x180), &except_vec3_generic, 0x80);
	else
		memcpy((void *)(ebase + 0x080), &except_vec3_generic, 0x80);

	signal_init();
#ifdef CONFIG_MIPS32_COMPAT
	signal32_init();
#endif

	local_flush_icache_range(ebase, ebase + 0x400);
	flush_tlb_handlers();

	sort_extable(__start___dbe_table, __stop___dbe_table);
}

// bitonic added, ctc merged
int enable_backtrace_debug = 0;

int show_backtrace_from_signal_context(int size, struct pt_regs * regs)
{
 unsigned long *addr;
 unsigned long *pc,*ra,*sp;
 unsigned long  ra_offset=0, stack_size;
 int depth = 0;

    if(size==0) return 0;

    printk("\n");
    pc = (unsigned long *)(unsigned long)regs->cp0_epc;
    ra = (unsigned long *)(unsigned long)regs->regs[31];
    sp = (unsigned long *)(unsigned long)regs->regs[29];
    printk("[Debug_backtrace] *** fn(%d)=0x%p\n", depth, pc);

    if(size==1) return 1;

    // scanning to find the size of the current stack-frame
    ra_offset = stack_size = 0;

    for(addr=pc; !ra_offset || !stack_size; --addr)
    {
    	switch(*addr & 0xFFFF0000)
	{
	    case 0x27BD0000:
	    	stack_size = abs((short)(*addr & 0xFFFF));
		break;
	    case 0xAFBF0000:
	    	ra_offset = (short)(*addr & 0xFFFF);
		break;
	    case 0x3c1c0000:
	    	goto __out_of_loop;
	    default:
	    	break;
	}
    }

__out_of_loop:

    if(ra_offset)
    	ra = *(unsigned long **)((unsigned long)sp | ra_offset);
    if(stack_size)
    	sp = (unsigned long *)((unsigned long)sp + stack_size);

    // repeat backward scanning
    for(depth = 1; depth < size && ra; ++depth)
    {
        printk("[Debug_backtrace] *** fn(%d)=0x%p\n", depth, ra);

	ra_offset = stack_size = 0;

	for(addr=ra; !ra_offset || !stack_size; --addr)
	{
	    switch(*addr & 0xFFFF0000)
	    {
	        case 0x27BD0000:
		    stack_size = abs((short)(*addr & 0xFFFF));
		    break;
		case 0xAFBF0000:
		    ra_offset = (short)(*addr & 0xFFFF);
		    break;
		case 0x3c1c0000:
		    return depth + 1;
		default:
		    break;
	    }
	}
	ra = *(unsigned long **)((unsigned long)sp + ra_offset);
	sp = (unsigned long*)((unsigned long)sp + stack_size);
    }
    printk("\n");
    return depth;
}

static void show_function_stack(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	long stackdata;
	int i;
	unsigned long __user *sp = (unsigned long __user *)regs->regs[29];

	printk("Stack :");
	i = 0;
	while ((unsigned long) sp & (PAGE_SIZE - 1)) {
		if (i && ((i % (64 / field)) == 0))
			printk("\n       ");
		if (i > 39) {
			printk(" ...");
			break;
		}

		if (__get_user(stackdata, sp++)) {
			printk(" (Bad stack address)");
			break;
		}

		printk(" %0*lx", field, stackdata);
		i++;
	}
	printk("\n");
}

void show_stacktrace_from_signal_context(struct pt_regs *regs, unsigned long write, unsigned long bad_address, struct task_struct *tsk)
{
	const int field = 2 * sizeof(unsigned long);

	printk("\n");
	printk("[Debug_userspace_show_registers] sending SIGSEGV to %s for "
		       "invalid %s\n%0*lx (epc == %0*lx, ra == %0*lx)\n",
		       tsk->comm,
		       write ? "write access to" : "read access from",
		       field, bad_address,
		       field, (unsigned long) regs->cp0_epc,
		       field, (unsigned long) regs->regs[31]);

	show_regs(regs);
	printk("Process %s (pid: %d, threadinfo=%p, task=%p, tls=%0*lx)\n",
	       current->comm, current->pid, current_thread_info(), current,
	      field, current_thread_info()->tp_value);
	if(current->real_parent != NULL){
		struct task_struct *parent = current->real_parent;
		printk("Real Parent Process %s (pid: %d, task=%p)\n",
		       parent->comm
		       , parent->pid
		       , parent);
	}else{
		printk("without Real Parent Process\n");
	}
	if(current->parent != NULL){
		struct task_struct *parent = current->parent;
		printk("Parent Process %s (pid: %d, task=%p)\n",
		       parent->comm
		       , parent->pid
		       , parent);
	}else{
		printk("without Parent Process\n");
	}
	if (cpu_has_userlocal) {
		unsigned long tls;

		tls = read_c0_userlocal();
		if (tls != current_thread_info()->tp_value)
			printk("*HwTLS: %0*lx\n", field, tls);
	}

	printk("\n");
	show_function_stack(regs);
	show_backtrace_from_signal_context(10, regs);
	show_code((unsigned int __user *) regs->cp0_epc);
	printk("\n");
}

#define ADDIU_SP	0x27BD0000
#define SW_RA	0xAFBF0000
#define LUI_PC 0x3c1c0000
#define STACK_MASK		0xFFFF
#define INST_MASK		0xFFFF0000

#if 0
use ipv4_conntrack_local() as example:
80300c08 <ipv4_conntrack_local>:
80300c08:	27bdffe0 	addiu	sp,sp,-32         <---------------- start address of this function
80300c0c:	afb10014 	sw	s1,20(sp)
80300c10:	afb00010 	sw	s0,16(sp)
80300c14:	afbf0018 	sw	ra,24(sp)			  <---------------- return address of this function
80300c18:	8ca30000 	lw	v1,0(a1)
80300c1c:	00808821 	move	s1,a0
80300c20:	8c640060 	lw	a0,96(v1)
80300c24:	2c820014 	sltiu	v0,a0,20
80300c28:	14400008 	bnez	v0,80300c4c <ipv4_conntrack_local+0x44>
80300c2c:	00a08021 	move	s0,a1
80300c30:	8c680020 	lw	t0,32(v1)
80300c34:	91070000 	lbu	a3,0(t0)
80300c38:	30e6000f 	andi	a2,a3,0xf
80300c3c:	00062880 	sll	a1,a2,0x2
80300c40:	2ca30014 	sltiu	v1,a1,20
80300c44:	1060000c 	beqz	v1,80300c78 <ipv4_conntrack_local+0x70>
80300c48:	3c0c8044 	lui	t4,0x8044
80300c4c:	0c09f9dc 	jal	8027e770 <net_ratelimit>
80300c50:	00000000 	nop
80300c54:	3c09803a 	lui	t1,0x803a
80300c58:	14400013 	bnez	v0,80300ca8 <ipv4_conntrack_local+0xa0>
80300c5c:	25240aa4 	addiu	a0,t1,2724
80300c60:	8fbf0018 	lw	ra,24(sp)
80300c64:	8fb10014 	lw	s1,20(sp)
80300c68:	8fb00010 	lw	s0,16(sp)
80300c6c:	24020001 	li	v0,1
80300c70:	03e00008 	jr	ra
80300c74:	27bd0020 	addiu	sp,sp,32		   <---------------- we will meet the incorrect start address here
80300c78:	8d8ab3c0 	lw	t2,-19520(t4)
80300c7c:	3c0b803a 	lui	t3,0x803a
80300c80:	15400011 	bnez	t2,80300cc8 <ipv4_conntrack_local+0xc0>
80300c84:	25640ac0 	addiu	a0,t3,2752
80300c88:	02202821 	move	a1,s1
80300c8c:	02003021 	move	a2,s0
80300c90:	8fbf0018 	lw	ra,24(sp)
80300c94:	8fb10014 	lw	s1,20(sp)
80300c98:	8fb00010 	lw	s0,16(sp)
80300c9c:	24040002 	li	a0,2
80300ca0:	080a8155 	j	802a0554 <nf_conntrack_in>
80300ca4:	27bd0020 	addiu	sp,sp,32
80300ca8:	0c00bf8a 	jal	8002fe28 <printk>
80300cac:	00000000 	nop
80300cb0:	8fbf0018 	lw	ra,24(sp)
80300cb4:	8fb10014 	lw	s1,20(sp)
80300cb8:	8fb00010 	lw	s0,16(sp)
80300cbc:	24020001 	li	v0,1
80300cc0:	03e00008 	jr	ra
80300cc4:	27bd0020 	addiu	sp,sp,32
80300cc8:	0c00bf8a 	jal	8002fe28 <printk>
80300ccc:	00000000 	nop
80300cd0:	0c0045c6 	jal	80011718 <Debug_backtrace>
80300cd4:	00000000 	nop
80300cd8:	080c0323 	j	80300c8c <ipv4_conntrack_local+0x84>	<---------------- search start address and return address within above codes of this function
80300cdc:	02202821 	move	a1,s1
#endif

#define DEBUG_BACKTRACE_MIPS32 0
int backtrace_mips32(int kernel_space, void **buffer, void **func, int size)
{
 	unsigned long *addr;
 	unsigned long *ra, *sp, *pc, *start_addr=NULL;
 	unsigned long  ra_offset=0, stack_size;
 	int bFindRA = 0, bFindStack = 0, bFindSymbolName = 0;
	int depth = 0;
	extern void ret_from_irq(void);
	extern void ret_from_exception(void);

	// search function start address
	char *modname;
	const char *name;
	unsigned long offset, sysbol_size;
	char namebuf[KSYM_NAME_LEN+1];

	if(!size)
		return 0;

	if(!buffer || size < 0)
		return -1;

	// get current $ra and $sp
	__asm__ __volatile__ (
	"	move	%0, $ra\n"
	"	move 	%1, $sp\n"
	: "=r"(ra), "=r"(sp)
	);

	//printk("[backtrace_mips32] ra=0x%x, sp=0x%x\n", ra, sp);

	// scanning to find the size of the current stack-frame
	stack_size = 0;

    for(addr=(unsigned long*)backtrace_mips32; !stack_size; ++addr)
    {
    	if((*addr & INST_MASK/*0xFFFF0000*/) == ADDIU_SP/*0x27BD0000*/)
    		stack_size = abs((short)(*addr & STACK_MASK/*0xFFFF*/));
    	else if(*addr == 0x03e00008)
    		break;
    }

	sp = (unsigned long*)((unsigned long)sp + stack_size);

	if(kernel_space)
		if(__kernel_text_address((unsigned long)ra) == 0) return 0;

	buffer[depth] = ra;
	func[depth] = (void*)backtrace_mips32;
	depth++;

    // repeat backward scanning
	for(; depth < size && ra; ++depth)
	{
		if(__kernel_text_address((unsigned long)ra) == 0) break;

		if ( ((unsigned long)ra == (unsigned long)ret_from_irq) ||
			((unsigned long)ra == (unsigned long)ret_from_exception) )
			break;

		// reset all local variables
		ra_offset = stack_size = 0;
		bFindRA = bFindStack = bFindSymbolName = 0;
		start_addr = NULL;
		pc = NULL;

		for(addr=ra; !bFindRA || !bFindStack; --addr)
		{
	   		switch(*addr & INST_MASK/*0xFFFF0000*/)
	    	{
	        	case ADDIU_SP/*0x27BD0000*/:
					name = kallsyms_lookup( (unsigned long)addr, &sysbol_size, &offset, &modname, namebuf);
#if (DEBUG_BACKTRACE_MIPS32 == 1)
					if(name==NULL)
						printk("[backtrace_mips32] ADDIU_SP, can not find symbol name\n");
					else
						printk("[backtrace_mips32] ADDIU_SP, symbol name=%s, offset=%d\n", name, offset);
#endif
					// If we can find the symbol name and offset value, we can get the start address of this function.(do once)
					if( (name != NULL) && (bFindSymbolName==0) )
					{
						bFindSymbolName = 1;
						start_addr = (unsigned long*)((unsigned long)addr - offset);
#if (DEBUG_BACKTRACE_MIPS32 == 1)
						printk("[backtrace_mips32] ADDIU_SP, addr=0x%x, offset=%d, start_addr=0x%x\n", addr, offset, start_addr);
#endif
					}

					// If we have found symbol name, we must check whether addr is smaller than start_addr.
					if(bFindSymbolName)
					{
						if(addr < start_addr)
						{
#if (DEBUG_BACKTRACE_MIPS32 == 1)
							printk("[backtrace_mips32] ADDIU_SP, depth=%d, *addr=0x%x, addr=0x%x, before start_addr:0x%x\n", depth, *addr, addr, start_addr);
#endif
							if(kernel_space)
		    					pc=start_addr;
		    				bFindRA = 1;
		    				bFindStack = 1;
							break;
						}
					}

					// There may be more ADDIU_SP commands within one kernel function.
					if(name)
					{	// find symbol name
						if(offset != 0)
							bFindStack = 0; // search again
						else
			    			if(bFindRA) bFindStack = 1;
					}
					else
		    			if(bFindRA) bFindStack = 1;

					if(kernel_space)
		    			pc=addr;
		    		stack_size = abs((short)(*addr & STACK_MASK/*0xFFFF*/));
#if (DEBUG_BACKTRACE_MIPS32 == 1)
					printk("[backtrace_mips32] ADDIU_SP, depth=%d, *addr=0x%x, addr=0x%x\n", depth, *addr, addr);
#endif
		   		break;

				case SW_RA/*0xAFBF0000*/:
#if (DEBUG_BACKTRACE_MIPS32 == 1)
					printk("[backtrace_mips32] SW_RA, depth=%d, *addr=0x%x, addr=0x%x\n", depth, *addr, addr);
#endif
		    		ra_offset = (short)(*addr & STACK_MASK/*0xFFFF*/);
		    		bFindRA = 1;
		    	break;

				case LUI_PC/*0x3c1c0000*/:
					if(pc != NULL) return depth;
#if (DEBUG_BACKTRACE_MIPS32 == 1)
					printk("[backtrace_mips32] LUI_PC, depth=%d, *addr=0x%x, addr=0x%x\n", depth, *addr, addr);
#endif
		    		pc=addr;
	    		break;

				default:
	    		break;
	    	}
		}

		ra = *(unsigned long **)((unsigned long)sp + ra_offset);
		sp = (unsigned long*)((unsigned long)sp + stack_size);
		buffer[depth] = ra;
		func[depth] = pc;
		pc = ra;
    }
    return depth;
}

void Debug_backtrace( void )
{
	unsigned long ra[40];
	unsigned long func[40]; // ra[] is the return address of func[].
	int i, count;
	count = backtrace_mips32(1, (void**)&ra, (void**)&func, 30); // from kernel space
	for(i=0; i<count; i++)
	{
		printk("[Debug_backtrace] i=%d, ra=0x%lx, func=0x%lx => symbol ", i, ra[i], func[i]);
		print_symbol("%s\n", (unsigned long)func[i]);
	}
	printk("\n");
}
EXPORT_SYMBOL(Debug_backtrace); // backtrace for kernel

EXPORT_SYMBOL(enable_backtrace_debug); // enable to dump backtrace debug message
