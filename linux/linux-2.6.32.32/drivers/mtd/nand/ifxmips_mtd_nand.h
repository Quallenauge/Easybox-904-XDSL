/******************************************************************************
**
** FILE NAME    : ifxmips_nand.h
** PROJECT      : UEIP
** MODULES      : NAND Flash
**
** DATE         : 05 Aug 2008
** AUTHOR       : Lei Chuanhua 
** DESCRIPTION  : NAND Flash MTD Driver
** COPYRIGHT    :       Copyright (c) 2008
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author      $Version   $Comment
** 05 Aug 2008  Lei Chuanhua  1.0        initial version
*******************************************************************************/
#ifndef IFXMIPS_NAND_H
#define IFXMIPS_NAND_H

#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>

#if defined(CONFIG_DANUBE)
/*pliu:  20081107 +*/
#define ENABLE_PULSE_ALE                1
/*1: ALE toggles between multiple address */
/*0: ALE stays valid across multiple address */
#define CS_LATCH		0
/*1: CS stays valid until it clears explicitly */
/*0: CS toggles between multiple read/write cycles. ENABEL_PULSE_ALE has to 1 in this case */
#define CHIP_DELAY		20
#define RDBY_NOT_USED		1
/*1: ready/busy signal is not used */
/*pliu:  20081107 -*/
#endif


#define NAND_PHYS_BASE              0x14000000
#define NAND_BASE_ADDRESS           (NAND_PHYS_BASE | KSEG1)
#define NAND_CON_CE         (1<<20) 
#define NAND_CON_ALE	    (1<<18)

#define NAND_WRITE(addr, val)     *((volatile u8*)(NAND_BASE_ADDRESS | (addr))) = val; while((IFX_REG_R32(IFX_EBU_NAND_WAIT) & IFX_EBU_NAND_WAIT_WR_C) == 0);
#define NAND_READ(addr, val)      val = *((volatile u8*)(NAND_BASE_ADDRESS | (addr)))
//#define NAND_CE_SET     IFX_REG_W32_MASK(0,NAND_CON_CE,IFX_EBU_NAND_CON);
//#define NAND_CE_CLEAR   IFX_REG_W32_MASK(NAND_CON_CE,0,IFX_EBU_NAND_CON);
#define NAND_READY        ((IFX_REG_R32(IFX_EBU_NAND_WAIT) & IFX_EBU_NAND_WAIT_RD) == IFX_EBU_NAND_WAIT_RD)
#define NAND_READY_CLEAR  IFX_REG_W32(0,IFX_EBU_NAND_WAIT);
//#define NAND_ALE_SET       IFX_REG_W32_MASK(0, NAND_CON_ALE, IFX_EBU_NAND_CON);
//#define NAND_ALE_CLEAR     IFX_REG_W32_MASK(NAND_CON_ALE, 0, IFX_EBU_NAND_CON);

#define NAND_CE_SET         IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) | (1<<20), IFX_EBU_NAND_CON);
#define NAND_CE_CLEAR	    IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) & ~(1<<20), IFX_EBU_NAND_CON);
#define NAND_ALE_SET        IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) | (1<<18), IFX_EBU_NAND_CON);
#define NAND_ALE_CLEAR	    IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) & ~(1<<18), IFX_EBU_NAND_CON);
/*
 * In NAND_CON register, bit18~bit23 is related to the following cmd
 * decoder 
 * Lat_en             cmd decoder      meaning
 * ---------------------------------------------
 * bit18              bit2             ALE
 * bit19              bit3             CLE
 * bit20              bit4             CS#
 * bit21              bit5             SE#
 * bit22              bit6             WP#
 * bit23              bit7             PRE
 */
 
#define NAND_CMD_ALE        (1<<2)
#define NAND_CMD_CLE        (1<<3)
#define NAND_CMD_NCLE       (0<<3)
#define NAND_CMD_CS         (1<<4)

#define NAND_WRITE_CMD      (NAND_CMD_CS | NAND_CMD_CLE)
#define NAND_WRITE_CMD_LNAND (NAND_CMD_CS | NAND_CMD_NCLE) 
#define NAND_WRITE_ADDR     (NAND_CMD_CS | NAND_CMD_ALE)
#define NAND_WRITE_DATA     (NAND_CMD_CS)
#define NAND_READ_DATA      (NAND_CMD_CS)
#define NAND_WRITE_CMD_RESET    0xff

#if defined(CONFIG_DANUBE)
#if defined(ENABLE_PULSE_ALE) && ENABLE_PULSE_ALE
#define NAND_WRITE_ADDR_OVER         NAND_WRITE_ADDR
#else
#define NAND_WRITE_ADDR_OVER          (NAND_CMD_CS) 
#endif 
#endif //defined(CONFIG_DANUBE)

#define IFX_ATC_NAND        0xc176
#define IFX_BTC_NAND        0xc166
#define ST_512WB2_NAND      0x2076
#define SAMSUNG_512_3ADDR   0xec75
#define HYNIX_MLC_FLASH	    0xaddc

/* GPIO global view */
#define IFX_NAND_ALE    13
#define IFX_NAND_CLE    24
#define IFX_NAND_CS1    23
#define IFX_NAND_RDY    48  /* NFLASH_READY */
#define IFX_NAND_RD     49  /* NFLASH_READ_N */

/* AR10 extra config */ 
#define IFX_NAND_D1	50
#define IFX_NAND_D0	51
#define IFX_NAND_D2_P1  52
#define IFX_NAND_D2_P2  53
#define IFX_NAND_D6	54
#define IFX_NAND_D5_P1	55
#define IFX_NAND_D5_P2	56
#define IFX_NAND_D3	57
#define IFX_NAND_WR	59
#define IFX_NAND_WP	60
#define IFX_NAND_SE	61
#define IFX_NAND_CS0	58 /* Use CS0 */
#define IFX_NAND_CS1	23 /* Use CS1 */

#define BBT_D (struct nand_bbt_descr)
#define ECC_L (struct nand_ecclayout)

/* bad block table descriptor which defines where in the
 * NAND device the bad block indicator is stored. This information
 * can be found in the NAND device data sheet (pattern, location, etc)
 * Be careful not to write over the bad block indicators
 */

u8 ifx_hsnand_2048_bbt_pattern[] = {0xff};
u8 ifx_hsnand_512_bbt_pattern[] = {0xff};

struct nand_bbt_descr ifx_hsnand_main_desc_2048 = {
    .options = NAND_BBT_CREATE | NAND_BBT_WRITE | NAND_BBT_SEARCH
               | NAND_BBT_VERSION,
    .offs = 0,
    .len = 1,
    .veroffs = 1,
    .maxblocks = 4,
    .pattern = ifx_hsnand_2048_bbt_pattern,
};

struct nand_bbt_descr ifx_hsnand_mirror_desc_2048 = {
    .options = NAND_BBT_CREATE | NAND_BBT_WRITE | NAND_BBT_SEARCH
               | NAND_BBT_VERSION,
    .offs = 0,
    .len = 1,
    .veroffs = 1,
    .maxblocks = 4,
    .pattern = ifx_hsnand_2048_bbt_pattern,
};

struct nand_bbt_descr ifx_hsnand_main_desc_512 = {
    .options = NAND_BBT_CREATE | NAND_BBT_WRITE | NAND_BBT_SEARCH
               | NAND_BBT_ABSPAGE | NAND_BBT_VERSION,
    .offs = 15,
    .len = 1,
    .veroffs = 14,
    .maxblocks = 4,
    .pattern = ifx_hsnand_512_bbt_pattern,

};

struct nand_bbt_descr ifx_hsnand_mirror_desc_512 = {
    .options = NAND_BBT_CREATE | NAND_BBT_WRITE | NAND_BBT_SEARCH
               | NAND_BBT_ABSPAGE | NAND_BBT_VERSION,
    .offs = 15,
    .len = 1,
    .veroffs = 14,
    .maxblocks = 4,
    .pattern = ifx_hsnand_512_bbt_pattern,

};


/* Generic flash bbt decriptors. BBT layout that are predefined 
 * in chips can be defined here. 
*/

#if 1 /*ctc*/
 uint8_t generic_bbt_pattern[] = {'A', 'R', 'C', 'A' };
 uint8_t generic_mirror_pattern[] = {'a', 'c', 'r', 'a' };
#else
 uint8_t generic_bbt_pattern[] = {'B', 'b', 't', '0' };
 uint8_t generic_mirror_pattern[] = {'1', 't', 'b', 'B' };
#endif

struct nand_bbt_descr generic_bbt_main_descr = {
        .options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
                | NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
        .offs = 8,
        .len = 4,
        .veroffs = 12,
        .maxblocks = 4,
        .pattern = generic_bbt_pattern
};

struct nand_bbt_descr generic_bbt_mirror_descr = {
        .options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
                | NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
        .offs = 8,
        .len = 4,
        .veroffs = 12,
        .maxblocks = 4,
        .pattern = generic_mirror_pattern
};

/* --> 512 page size for Samsung chips, bad block info
 * is located @ the last byte of the oob (16th byte)
 * area of the 1st two pages of every block.
 * --> 2048 page size for Samsung chips, bad block info
 * is located @ the first byte of the oob area of
 * the first two pages of every block
 */

struct nand_ecclayout ifx_oobinfo_512 = {
    .eccbytes = 3,
    .eccpos = {0, 1, 2},
    .oobavail = 9,
    .oobfree = {{4, 13}}
};

struct nand_ecclayout ifx_oobinfo_2048 = {
    .eccbytes = 12,
    .eccpos = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
    .oobavail = 50,
    .oobfree = {{13, 63}}
};

/* OOB info for generic chips. For specific chips with
 * predefined factory default OOB info, create a structure 
 * with the specifics of the chip's OOB layout
 */


struct nand_ecclayout oobinfo_512_generic  = {
        .eccbytes = 6,
        .eccpos = {0, 1, 2, 3, 6, 7},
        .oobfree = {
                {.offset = 8,
                 . length = 8}}
};

struct nand_ecclayout oobinfo_2048_generic = {
        .eccbytes = 24,
        .eccpos = {
                   40, 41, 42, 43, 44, 45, 46, 47,
                   48, 49, 50, 51, 52, 53, 54, 55,
                   56, 57, 58, 59, 60, 61, 62, 63},
		.oobavail = 38,
        .oobfree = {
                {.offset = 2,
                 .length = 38}}
};

struct nand_extra_info {
    u32 chip_id;
    int addr_cycle;
    struct nand_ecclayout *chip_ecclayout;
    struct nand_bbt_descr *chip_bbt_main_descr;
    struct nand_bbt_descr *chip_bbt_mirror_descr;
};

struct nand_extra_info chip_extra_info[] = {
    { 0xec75, 3, &ifx_oobinfo_512, &ifx_hsnand_main_desc_512, &ifx_hsnand_mirror_desc_512 },
    { 0xecf1, 5, &ifx_oobinfo_2048, &ifx_hsnand_main_desc_2048, &ifx_hsnand_mirror_desc_2048 },
    { 0xecdc, 5, &oobinfo_2048_generic, &ifx_hsnand_main_desc_2048, &ifx_hsnand_mirror_desc_2048 },
    { 0xc176, 4, NULL, NULL, NULL },  // IFX nand
    { 0xecd3, 5, NULL, NULL, NULL }, //samsung mlc
    { 0xaddc, 5, NULL, NULL, NULL }, // hynix
    { 0x2c68, 5, NULL, NULL, NULL }, // micron
    { 0x2c88, 5, NULL, NULL, NULL }, // micron
    { 0xFFFF, 3, &oobinfo_512_generic, &generic_bbt_main_descr, &generic_bbt_mirror_descr },
    { 0xFFFF, 3, &oobinfo_2048_generic, &generic_bbt_main_descr, &generic_bbt_mirror_descr },


};

#endif /* IFXMIPS_NAND_H */

