/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>
#include <asm/addrspace.h>
#include <asm/yamon.h>

/*****************************************************************************
 * Local assembler register definitions
 *****************************************************************************/
#define C0_EBASe     $15,1

#define CFG_BOOTSTRAP_CODE

#define S_EBASeExpBase     12       /* Exception Base */
#define M_EBASeExpBase     (0x3ffff << S_EBASeExpBase)
#define S_EBASeCPUNum      0        /* CPU Number */
#define M_EBASeCPUNum      (0x1ff << S_EBASeCPUNum)

/*
 * Values in the CPU Number field
 */
#define K_EBASeCPUNum_Host 0
#define K_EBASeCPUNum_Voice 1

/* C0_EBASE register encoding */
#define CO_EBASE_EXPBASE_SHF     S_EBASeExpBase
#define CO_EBASE_EXPBASE_MSK     M_EBASeExpBase

#define CO_EBASE_CPUNUM_SHF      S_EBASeCPUNum
#define CO_EBASE_CPUNUM_MSK      M_EBASeCPUNum

#define CO_EBASE_CPU_HOST     K_EBASeCPUNum_Host
#define CO_EBASE_CPU_VOICE    K_EBASeCPUNum_Voice

#define CFG_EBU_BOOTWORD              0x688c688c
/* lcd back light gpio define */
#define BSP_MIPS_GPIO_P1_ALTSEL0     0xBE100B4C
#define BSP_MIPS_GPIO_P1_ALTSEL1     0xBE100B50
#define BSP_MIPS_GPIO_P1_DIR         0xBE100B48
#define BSP_MIPS_GPIO_P1_OUT         0xBE100B40
#define BSP_MIPS_GPIO_P1_OD          0xBE100B54

/* FixMe: Adresses not defined... */
#define RST_STAT   0xbf203014
/* POR,SRST, WDT0, WDT1,HRST*/
#define HARD_RST 0xE8000001
    .set noreorder
	.globl _start
   	.globl __exception_vector_tlb_refill
   	.globl __exception_vector_xtlb_refill
   	.globl __exception_vector_general
   	.globl __exception_vector_int
   	.text

_start:
	/* RESET entry */
   	b     reset
   	nop
   	nop
   	nop
   .org 0x10
   .word CFG_EBU_BOOTWORD     /* EBU init code, fetched during booting */
   .word 0x00000000           /* phase of the flash                    */

   .org 0x18
   .string "1.1.0"            /* version 1.1.0                         */
   .word 0x00000000
   .align 4

reset:
        /* turn off led back light */
	li      t0, 0x000091f8
	sw      t0, BSP_MIPS_GPIO_P1_DIR

	li      t0, 0x000085c0
	sw      t0, BSP_MIPS_GPIO_P1_ALTSEL0

	li      t0, 0x00000000
	sw      t0, BSP_MIPS_GPIO_P1_ALTSEL1

	li      t0, 0x000091f8
	sw      t0, BSP_MIPS_GPIO_P1_OD

	li      t0, 0x00000000
	sw      t0, BSP_MIPS_GPIO_P1_OUT

  mtc0    zero, CP0_WATCHLO
	mtc0    zero, CP0_WATCHHI
	mtc0    zero, CP0_CAUSE

       /* Determine processor */
	mfc0    k1, C0_PRId
	li	t0, M_PRIdImp | M_PRIdCoID
	and     k1, t0

/* Check if it is a MIPS 34K processor */
	li	t0, MIPS_34K
	bne	k1, t0, 2f
	nop

	/* Setup MIPS34K specifics (implementation dependent fields) */
	MFC0(   t0, C0_Config )
	li	t1, CONFIG0_MIPS32_64_MSK
	and	t0, t1
	li	t1, CONFIG0_MIPS34K
	or	t0, t1
	MTC0(	t0, C0_Config )

	MFC0(   t0, C0_Status )
	li	t1, STATUS_MIPS32_64_MSK
	and	t0, t1
	li	t1, STATUS_MIPS34K
	or	t0, t1
	MTC0(	t0, C0_Status )

2:
	/* Check if it is a MIPS 4K/5K family processor */
	li	t0, MIPS_4Kc
	beq	k1, t0, 1f
	nop
	li	t0, MIPS_4KEc
	beq	k1, t0, 1f
	nop

	b	mips32_64
	nop

1:
	/* Setup MIPS 4K/5K specifics (implementation dependent fields) */
	MFC0(   t0, C0_Config )
	li	t1, CONFIG0_MIPS32_64_MSK
	and	t0, t1
	li	t1, CONFIG0_MIPS4K5K
	or	t0, t1
	MTC0(	t0, C0_Config )

	MFC0(   t0, C0_Status )
	li	t1, STATUS_MIPS32_64_MSK
	and	t0, t1
	li	t1, STATUS_MIPS4K5K
	or	t0, t1
	MTC0(	t0, C0_Status )

/************************************************************************
 *  MIPS32/MIPS64 specific cpu initialisation
 ************************************************************************/
mips32_64:

	/* Setup generic MIPS32/MIPS64 fields of STATUS register */
	MFC0(   t0, C0_Status )
	li	t1, ~STATUS_MIPS32_64_MSK
	and	t0, t1
	li	t1, STATUS_MIPS32_64
	or	t0, t1
	MTC0(	t0, C0_Status )

	/* Setup generic MIPS32 fields of CONFIG0 register */
	MFC0(   t0, C0_Config )
	li	t1, ~CONFIG0_MIPS32_64_MSK
	and	t0, t1
	li	t1, CONFIG0_MIPS32_64
	or	t0, t1
	MTC0(	t0, C0_Config )


	/* Initialize GOT pointer.
	*/
	bal     1f
	nop
	.word   _GLOBAL_OFFSET_TABLE_
1:
	move    gp, ra
	lw      t1, 0(ra)
	move	gp, t1


   	/* Initialize any external memory.
	 */
	la      t9, lowlevel_init
	jalr    t9
	nop
lowlevel_init_done:

sys_init_cache:

#define RA                      t4
#define icache_size             t3
#define icache_linesize         t2
#define dcache_size             t1
#define dcache_linesize         t0
        move    RA, ra
        move    a0, k1
        bal     sys_determine_icache_linesize_flash
        nop
	move    icache_linesize, v0 /*icache_linesize=0x20 for amazon_s*/

        bal     sys_determine_icache_lines_flash /*icache lines=0x400 for amazon_s*/
        nop
        multu   icache_linesize, v0
        mflo    icache_size /*icache_size=0x8000 for amazon_s*/

        bal             sys_determine_dcache_linesize_flash
        nop
        move    dcache_linesize, v0 /*dcache_linesize=0x20 for amazon_s*/

        bal         sys_determine_dcache_lines_flash /*dcache lines=0x200 for amazon_s*/
        nop
        multu   dcache_linesize, v0
        mflo    dcache_size  /*dcache_size=0x4000 for amazon_s*/

        /* Initialise instruction cache */
        move    a0, icache_size
        move    a1, icache_linesize
        bal             sys_init_icache
        nop

        /* Initialise data cache */
        move    a0, dcache_size
        move    a1, dcache_linesize
        bal         sys_init_dcache
        move    a2, k1

#if 1
change_got_table:
        bal     1f
	      nop
        .word   got_size
1:
        move    t1, ra
	      lw      t1, 0(t1)

        move    t0, gp
        add     t1, t0, t1
        addi    t0, 8 /*skip first 2 entries*/
2:
        lw      t2, 0(t0)
        subu    t2, 0x20000000
        sw      t2, 0(t0)
        bne     t0, t1, 2b
        addi    t0, t0, 4

      	subu    gp, 0x20000000
#endif
    	li	t0, 0xBE22FF00
	    la	sp, 0(t0)
	    //la	t9, nand_boot
	    la  t9, nand_spl_init
		j	t9
	    nop


	/* Exception handlers.
	 */
romReserved:
	b romReserved

romExcHandle:
	b romExcHandle




/* icache init */

sys_init_icache:
	li	 a3, MIPS_34K
	beq	 a3, a2, 2f
	nop

1:
	/* 20Kc/25Kf : Clear ITagLo/ITagHi */
	/* Note: Use the default case because ITagLo==TagLo and ITagHI=TagHi */

	/* default: Clear TagLo/TagHi */
	MTC0( zero, C0_TagLo )
	MTC0( zero, C0_TagHi )
	b	0f
	 nop

2:
	/* 24K/24KE/34K : Clear ITagLo */
	MTC0_SEL_OPCODE( R_zero, R_C0_ITagLo, R_C0_SelITagLo )

0:
	/* Calc an address that will correspond to the first cache line */
	// li	a2, KSEG0BASE
	li 		a2, 0x9fc00000

	/* Calc an address that will correspond to the last cache line  */
	addu	a3, a2, a0
	subu    a3, a1

	/* Loop through all lines, invalidating each of them */
1:
	cache	ICACHE_INDEX_STORE_TAG, 0(a2)	/* clear tag */
	bne	a2, a3, 1b
	addu	a2, a1

9:
	jr	ra
	nop


/* dcache init */

sys_init_dcache:
	li	 a3, MIPS_34K
	beq	 a3, a2, 2f
	nop

	/* default: Clear TagLo/TagHi */
	MTC0( zero, C0_TagLo )
	MTC0( zero, C0_TagHi )
	b	0f
	 nop

1:
	/* 20Kc/25Kf : Clear DTagLo and DTagHi */
	MTC0_SEL_OPCODE( R_zero, R_C0_DTagLo, R_C0_SelDTagLo )
	MTC0_SEL_OPCODE( R_zero, R_C0_DTagHi, R_C0_SelDTagHi )
	b	0f
	 nop
2:
	/* 24K/24KE/34K : Clear DTagLo */
	MTC0_SEL_OPCODE( R_zero, R_C0_DTagLo, R_C0_SelDTagLo )


/************************************************************************
 *  invalidating all dcache lines
 ************************************************************************/
0:
	/* Calc an address that will correspond to the first cache line */
//	li	a2, KSEG0BASE
	li	a2, 0x9fc00000

	/* Calc an address that will correspond to the last cache line  */
	addu	a3, a2, a0
	subu    a3, a1

	/* Loop through all lines, invalidating each of them */
1:
	cache	DCACHE_INDEX_STORE_TAG, 0(a2)	/* clear tag */
	bne		a2, a3, 1b
	addu	a2, a1
#if 0
/************************************************************************
 *  fetch and lock all dcache lines
 ************************************************************************/
	/* Calc an address that will correspond to the first cache line */
//	li	a2, KSEG0BASE
	li	a2, 0x9fc00000

	/* Calc an address that will correspond to the last cache line  */
	addu	a3, a2, a0
	subu    a3, a1

	/* Loop through all lines, invalidating each of them */
1:
	cache	DCACHE_ADDR_FETCH_LOCK, 0(a2)	/* clear tag */
        //cache   DCACHE_ADDR_HIT_WRITEBACK_INVALIDATE, 0(a2)  /* invalidate */
	bne		a2, a3, 1b
	addu	a2, a1

9:

#endif
	jr	ra
	 nop





/* Subroutine : Determine icache line size */

sys_determine_icache_linesize_flash:
icache_linesize_mips32:

	/* Read CONFIG1 register, which holds implementation data */
	MFC0_SEL_OPCODE( R_t9, R_C0_Config1, R_C0_SelConfig1 )

#define config1	t9

	/* I-cache line size */
	li	t8, M_Config1IL
	and	t8, config1
	//beq	t8, zero, icache_linesize_zero
	li	t7, S_Config1IL
	srl	t8, t7
	li	t7, 0x2
	sll	v0, t7, t8

	jr	ra
	nop

/* Subroutine : Determine icache line */

sys_determine_icache_lines_flash:
icache_lines_mips32:

	/* Read CONFIG1 register, which holds implementation data */
	MFC0_SEL_OPCODE( R_t9, R_C0_Config1, R_C0_SelConfig1 )

#define config1	t9

	/* I-cache lines
	 * Calculated as associativity * sets per way
	 */
	li	t8, M_Config1IA
	and	t8, config1
	li	t7, S_Config1IA
	srl	t8, t7
	addiu	t8,1				/* t8 = associativity	*/

	li	t7, M_Config1IS
	and	t7, config1
	li	t9, S_Config1IS
	srl	t7, t9
	li	t9, 0x40
	sll	t7, t9, t7			/* t7 = sets per way	*/

	multu	t8, t7
	mflo    v0

	jr	ra
	nop


/* Subroutine : Determine dcache line size */

sys_determine_dcache_linesize_flash:
dcache_linesize_mips32:
.globl   dcache_linesize_mips32
.ent     dcache_linesize_mips32
	/* Read CONFIG1 register, which holds implementation data */
	MFC0_SEL_OPCODE( R_t9, R_C0_Config1, R_C0_SelConfig1 )

#define config1	t9

	/* D-cache line size */
	li	t8, M_Config1DL
	and	t8, config1
	//beq	t8, zero, dcache_linesize_zero
	li	t7, S_Config1DL
	srl	t8, t7
	li	t7, 0x2
	sll	v0, t7, t8

	jr	ra
	nop
.end    dcache_linesize_mips32
/* Subroutine : Determine dcache line */

sys_determine_dcache_lines_flash:
dcache_lines_mips32:

	/* Read CONFIG1 register, which holds implementation data */
	MFC0_SEL_OPCODE( R_t9, R_C0_Config1, R_C0_SelConfig1 )

#define config1	t9

	/* D-cache lines
	 * Calculated as associativity * sets per way
	 */
	li	t8, M_Config1DA
	and	t8, config1
	li	t7, S_Config1DA
	srl	t8, t7
	addiu	t8,1				/* t8 = associativity	*/

	li	t7, M_Config1DS
	and	t7, config1
	li	t9, S_Config1DS
	srl	t7, t9
	li	t9, 0x40
	sll	t7, t9, t7			/* t7 = sets per way	*/

	multu	t8, t7
	mflo    v0

	jr	ra
	nop

dcache_writeback_invalidate:
//    a0 : size
//    a1 : dcache_line_size
//    a2 : start addr
.globl   dcache_writeback_invalidate
.ent     dcache_writeback_invalidate

	addu	t8, a2, a0
	subu  t8, a1
	/* Loop through all address */
1:
	cache	DCACHE_ADDR_HIT_WRITEBACK_INVALIDATE, 0(a2)	/* clear tag */
//	bne		a2, t8, 1b
	sub	t9, a2, t8
	blez	t9, 1b
	addu	a2, a1

9:
	jr	ra
	nop
        .end    dcache_writeback_invalidate

dcache_hit_invalidate:
//    a0 : size
//    a1 : dcache_line_size
//    a2 : start addr
.globl   dcache_hit_invalidate
.ent     dcache_hit_invalidate

	addu	t8, a2, a0
	subu  t8, a1
	/* Loop through all address */
1:
	cache	DCACHE_ADDR_HIT_INVALIDATE, 0(a2)	/* clear tag */
	bne		a2, t8, 1b
	addu	a2, a1

9:
	jr	ra
	nop
        .end dcache_hit_invalidate

