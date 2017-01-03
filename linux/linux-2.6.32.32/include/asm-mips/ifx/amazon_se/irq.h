/******************************************************************************
**
** FILE NAME    : irq.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : header file for Amazon-SE
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#ifndef AMAZON_SE_IRQ
#define AMAZON_SE_IRQ



/****** Interrupt Assigments ***********/

#define IFX_ASC1_TIR                    INT_NUM_IM2_IRL0    //  INT_NUM_IM0_IRL0
#define IFX_ASC1_TBIR                   INT_NUM_IM2_IRL1    //  INT_NUM_IM0_IRL1
#define IFX_ASC1_RIR                    INT_NUM_IM2_IRL2    //  INT_NUM_IM0_IRL2
#define IFX_ASC1_EIR                    INT_NUM_IM2_IRL3    //  INT_NUM_IM0_IRL3
#define IFX_ASC1_ABSTIR                 INT_NUM_IM2_IRL4    //  INT_NUM_IM0_IRL4
#define IFX_ASC1_ABDETIR                INT_NUM_IM2_IRL5    //  INT_NUM_IM0_IRL5
#define IFX_ASC1_SFCIR                  INT_NUM_IM2_IRL6    //  INT_NUM_IM0_IRL6

#define IFX_FPI_SLAVE_BCU0_IR           INT_NUM_IM2_IRL7    //  INT_NUM_IM0_IRL7
#define IFX_FPI_MASTER_COSBCU_IR        INT_NUM_IM2_IRL8    //  INT_NUM_IM0_IRL8
#define IFX_FPI_SLAVE_BCU_IRQ           IFX_FPI_SLAVE_BCU0_IR
#define IFX_FPI_MASTER_BCU_IRQ          IFX_FPI_MASTER_COSBCU_IR

#define IFX_DSL_DFE_IR                  INT_NUM_IM2_IRL9    //  INT_NUM_IM0_IRL9
#define IFX_DSL_AFEOVL_IR               INT_NUM_IM2_IRL10   //  INT_NUM_IM0_IRL10
#define IFX_DSL_DYING_GASP_INT          INT_NUM_IM2_IRL11   //  INT_NUM_IM0_IRL11
#define IFX_DSL_DFE_INT0IR              INT_NUM_IM1_IRL16   //  INT_NUM_IM3_IRL16
#define IFX_DSL_DFE_INT1IR              INT_NUM_IM1_IRL17   //  INT_NUM_IM3_IRL17
#define IFX_DSL_DFE_INT2IR              INT_NUM_IM1_IRL18   //  INT_NUM_IM3_IRL18
#define IFX_DSL_DFE_INT3IR              INT_NUM_IM1_IRL19   //  INT_NUM_IM3_IRL19
#define IFX_MEI_INT                     IFX_DSL_DFE_IR
#define IFX_MEI_DYING_GASP_INT          IFX_DSL_DYING_GASP_INT
#define IFX_DSL_DFE_TXIR                IFX_DSL_DFE_INT0IR
#define IFX_DSL_DFE_RXIR                IFX_DSL_DFE_INT1IR

#define IFX_DMA_CH0_INT                 INT_NUM_IM3_IRL0    //  INT_NUM_IM1_IRL0
#define IFX_DMA_CH1_INT                 INT_NUM_IM3_IRL1    //  INT_NUM_IM1_IRL1
#define IFX_DMA_CH2_INT                 INT_NUM_IM3_IRL2    //  INT_NUM_IM1_IRL2
#define IFX_DMA_CH3_INT                 INT_NUM_IM3_IRL3    //  INT_NUM_IM1_IRL3
#define IFX_DMA_CH4_INT                 INT_NUM_IM3_IRL4    //  INT_NUM_IM1_IRL4
#define IFX_DMA_CH5_INT                 INT_NUM_IM3_IRL5    //  INT_NUM_IM1_IRL5
#define IFX_DMA_CH6_INT                 INT_NUM_IM3_IRL6    //  INT_NUM_IM1_IRL6
#define IFX_DMA_CH7_INT                 INT_NUM_IM3_IRL7    //  INT_NUM_IM1_IRL7
#define IFX_DMA_CH8_INT                 INT_NUM_IM3_IRL8    //  INT_NUM_IM1_IRL8
#define IFX_DMA_CH9_INT                 INT_NUM_IM3_IRL9    //  INT_NUM_IM1_IRL9

#define IFX_PPE_MBOX_INT0               INT_NUM_IM2_IRL12   //  INT_NUM_IM0_IRL12
#define IFX_PPE_MBOX_INT1               INT_NUM_IM2_IRL13   //  INT_NUM_IM0_IRL13
#define IFX_PPE_MBOX_INT2               INT_NUM_IM2_IRL14   //  INT_NUM_IM0_IRL14
#define IFX_PPE_QSB_INT                 INT_NUM_IM2_IRL15   //  INT_NUM_IM0_IRL15

#define IFX_EPHY_INT                    INT_NUM_IM3_IRL29   //  INT_NUM_IM1_IRL29

#define IFX_EIU_IR0                     INT_NUM_IM2_IRL21   //  INT_NUM_IM0_IRL21
#define IFX_EIU_IR1                     INT_NUM_IM0_IRL22   //  INT_NUM_IM2_IRL22
#define IFX_EIU_IR2                     INT_NUM_IM2_IRL23   //  INT_NUM_IM0_IRL23

#define IFX_GPTU_TC1A                   INT_NUM_IM2_IRL25   //  INT_NUM_IM0_IRL25
#define IFX_GPTU_TC1B                   INT_NUM_IM2_IRL26   //  INT_NUM_IM0_IRL26
#define IFX_GPTU_TC2A                   INT_NUM_IM2_IRL27   //  INT_NUM_IM0_IRL27
#define IFX_GPTU_TC2B                   INT_NUM_IM2_IRL28   //  INT_NUM_IM0_IRL28
#define IFX_GPTU_TC3A                   INT_NUM_IM2_IRL29   //  INT_NUM_IM0_IRL29
#define IFX_GPTU_TC3B                   INT_NUM_IM2_IRL30   //  INT_NUM_IM0_IRL30

#define IFX_MC_IR                       INT_NUM_IM3_IRL24   //  INT_NUM_IM1_IRL24

#define IFX_EBU_IR                      INT_NUM_IM2_IRL24   //  INT_NUM_IM0_IRL24

#define IFX_PMC_HIT_IR                  INT_NUM_IM3_IRL25   //  INT_NUM_IM1_IRL25
#define IFX_PMC_MISS_IR                 INT_NUM_IM3_IRL23   //  INT_NUM_IM1_IRL23
#define IFX_PMCIR                       IFX_PMC_HIT_IR

#define IFX_SBIU_ERRIR                  INT_NUM_IM3_IRL26   //  INT_NUM_IM1_IRL26

#define IFX_SSC_RIR                     INT_NUM_IM2_IRL16   //  INT_NUM_IM0_IRL16
#define IFX_SSC_TIR                     INT_NUM_IM2_IRL17   //  INT_NUM_IM0_IRL17
#define IFX_SSC_EIR                     INT_NUM_IM2_IRL18   //  INT_NUM_IM0_IRL18
#define IFX_SSC_FIR                     INT_NUM_IM2_IRL19   //  INT_NUM_IM0_IRL19

#define IFX_MMC_CONTROLLER_INTR0_IRQ    INT_NUM_IM1_IRL20   //  INT_NUM_IM3_IRL20
#define IFX_MMC_CONTROLLER_INTR1_IRQ    INT_NUM_IM1_IRL21   //  INT_NUM_IM3_IRL21
#define IFX_MMC_CONTROLLER_SDIO_I_IRQ   INT_NUM_IM1_IRL22   //  INT_NUM_IM3_IRL22

#define IFX_USB0_IR                     INT_NUM_IM2_IRL31   //  INT_NUM_IM0_IRL31
#define IFX_USB0_OCIR                   INT_NUM_IM2_IRL20   //  INT_NUM_IM0_IRL20
#define IFX_USB_INT                     IFX_USB0_IR
#define IFX_USB_OC_INT                  IFX_USB0_OCIR

#define IFX_WDT_PIR                     INT_NUM_IM3_IRL27   //  INT_NUM_IM1_IRL27
#define IFX_WDT_AEIR                    INT_NUM_IM3_IRL28   //  INT_NUM_IM1_IRL28



#endif  //  AMAZON_SE_IRQ
