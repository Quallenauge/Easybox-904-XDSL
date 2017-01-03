/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_pmon_vr9.h
  \ingroup IFX_PMON
  \brief ifx pmon vr9 specific definition
*/
#ifndef IFXMIPS_PMON_VR9_H
#define IFXMIPS_PMON_VR9_H
#define IFX_PMON_PHY_BASE    0x1F700400
#define IFX_PMON_VIRT_BASE  (IFX_PMON_PHY_BASE | KSEG1)
#define IFX_PMON_SIZE        256

/* Quick access for specified TC and Counter 0/1*/
#define IFX_PMON_PCEESCR(TC, CNT)  ((volatile u32 *)(IFX_PMON_VIRT_BASE + ((TC) << 4) + ((CNT) << 3)))


#define IFX_MIPS_MAX_TC      IFX_MIPS_TC1
#define IFX_PMON_EVENT_MASK  0x1F

#define IFX_PMON_IS_DDR_EVENT(_x) ((_x) >= IFX_PMON_EVENT_DDR_READ && (_x) <= IFX_PMON_EVENT_DDR_EIGHT_WORD_64BIT_WIRTE)

#define IFX_PMON_IS_AHB_EVENT(_x) ((_x) >= IFX_PMON_EVENT_AHB_READ_CYCLES && (_x) <= IFX_PMON_EVENT_AHB_WRITE_CPT)

#define IFX_PMON_IS_DMA_EVENT(_x) ((_x) >= IFX_PMON_EVENT_DMA_RX_BLOCK_CNT && (_x) <= IFX_PMON_EVENT_DMA_TX_BLOCK_CNT)

    
/* Performance Monitoring Interface */
#define IFX_PMON_PMICR       ((volatile u32 *)(IFX_PMON_VIRT_BASE + 0x30))

#define IFX_PMON_DDR_EN      0x00000001
#define IFX_PMON_DMA_EN      0x00000002
#define IFX_PMON_AHB_EN      0x00000004

#define IFX_PMON_PMI_ALL     (IFX_PMON_DDR_EN | IFX_PMON_DMA_EN | IFX_PMON_AHB_EN)


static inline void pmon_platform_init(void)
{
    IFX_REG_W32(0x0, IFX_PMON_PMICR);
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC0, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC0, 1));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC1, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC1, 1));
}

static inline void pmon_platform_event_set(int event, int tc, int counter)
{
    u32 reg;
    u32 pmon_if;
    
   /* Enable the related monitoring interface */
    if (IFX_PMON_IS_DDR_EVENT(event)) {
        pmon_if = IFX_PMON_DDR_EN;
    }
    else if (IFX_PMON_IS_DMA_EVENT(event)){
        pmon_if = IFX_PMON_DMA_EN;
    }
    else if(IFX_PMON_IS_AHB_EVENT(event)){
        pmon_if = IFX_PMON_AHB_EN;
    }
    else {
        pmon_if = 0; /* Disable all PMI interfaces */
    }
    
    reg = IFX_REG_R32(IFX_PMON_PMICR);

    reg |= pmon_if;

    /* 
     * HW WAR, only two counters available. Don't allow three PMI at the same time.
     * If hit, fall back to one PMI.
     */
    if ((reg & IFX_PMON_PMI_ALL) == IFX_PMON_PMI_ALL) {
        reg &= ~ IFX_PMON_PMI_ALL;
        reg = pmon_if;
    }
    IFX_REG_W32(reg, IFX_PMON_PMICR);

    if (pmon_if > 0) { 
        reg = IFX_REG_R32(IFX_PMON_PCEESCR(tc, counter));
        reg &= ~IFX_PMON_EVENT_MASK;
        reg |= event;
        IFX_REG_W32(reg, IFX_PMON_PCEESCR(tc, counter));
    }
}

#endif /* IFXMIPS_PMON_VR9_H */
