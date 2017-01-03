#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>
#include <asm/thread_info.h>

	.text

	.align 5

#if 0
LEAF(ifx_fill_icache)
	.set	noreorder
	.set	push
	li	t1, 0x9FFFFFE0
	and	a0, t1
	li	t0, 0x1F
	addu	t0, t0, a1
	addu	t0, t0, a0
	and	t0, t1
1:
	cache	0x14, 0(a0)
	bne	a0, t0, 1b
	addi	a0, 0x20
	.set	pop
	jr	ra
	END(ifx_fill_icache)
#endif

LEAF(ifx_fill_lock_icache)
	.set	noreorder
	.set	push
	li	t1, 0x9FFFFFE0
	and	a0, t1
	li	t0, 0x1F
	addu	t0, t0, a1
	addu	t0, t0, a0
	and	t0, t1
1:
	cache	0x1c, 0(a0)
	bne	a0, t0, 1b
	addi	a0, 0x20
	.set	pop
	jr	ra
	END(ifx_fill_lock_icache)

LEAF(ifx_unlock_icache)
	.set	noreorder
	.set	push
	li	t1, 0x9FFFFFE0
	and	a0, t1
	li	t0, 0x1F
	addu	t0, t0, a1
	addu	t0, t0, a0
	and	t0, t1
1:
	cache	0x10, 0(a0)
	bne	a0, t0, 1b
	addi	a0, 0x20
	.set	pop
	jr	ra
	END(ifx_unlock_icache)

