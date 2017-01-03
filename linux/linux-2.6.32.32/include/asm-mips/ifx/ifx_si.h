/******************************************************************************
**
** FILE NAME    : ifx_si.h
** PROJECT      : UEIP
** MODULES      : Serial In Controller
**
** DATE         : 26 Apr 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global Serial In Controller driver header file
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date          $Author         $Comment
** Apr 26, 2010   Xu Liang        Init Version
*******************************************************************************/



#ifndef IFX_SI_H
#define IFX_SI_H

/*!
  \defgroup IFX_SI UEIP Project - SI Controller driver module
  \brief UEIP Project - Serial In Controller driver module, support AR9, VR9, AR10.
 */

/*!
  \defgroup IFX_SI_API APIs
  \ingroup IFX_SI
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_SI_IOCTL IOCTL Commands
  \ingroup IFX_SI
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_SI_STRUCT Structures
  \ingroup IFX_SI
  \brief Structures used by user application.
 */

/*!
  \defgroup IFX_SI_ENUM Enumerated Types
  \ingroup IFX_SI
  \brief Enumerated types used by user application.
 */

/*!
  \file ifx_si.h
  \ingroup IFX_SI
  \brief SI Controller (Serial In) driver header file
 */



/*!
  \addtogroup IFX_SI_ENUM
 */
/*@{*/

/*!
  \enum IFX_SI_UPD_SRC_t
  \brief enum used to configure update source.
 */
typedef enum {
    IFX_SI_UPD_SRC_SW       = 0,
    IFX_SI_UPD_SRC_GPT3     = 1,
    IFX_SI_UPD_SRC_FPID     = 2,
    IFX_SI_UPD_SRC_ERR      = 3,    // no such thing. HW Error.
} IFX_SI_UPD_SRC_t;

/*!
 \enum IFX_SI_SYNC_MODE_t
 \brief enum used to select either sync mode or async mode.
 */
typedef enum {
    IFX_SI_ASYNC_MODE       = 0,
    IFX_SI_SYNC_MODE        = 1,
} IFX_SI_SYNC_MODE_t;

/*!
  \enum IFX_SI_FPI_SAMPLECLK_DIV_t
  \brief enum used to select between FPI CLK Division for setting read interval,
            applicable only when update source = FPID.
 */
typedef enum {
    // FPID2[26:25] For Sampling Interval Clk
    IFX_SI_FPI_SAMPLECLK_DIV_010 = 1,
    IFX_SI_FPI_SAMPLECLK_DIV_040 = 2,
    IFX_SI_FPI_SAMPLECLK_DIV_080 = 3,
    IFX_SI_FPI_SAMPLECLK_DIV_160 = 0,
} IFX_SI_FPI_SAMPLECLK_DIV_t;

/*!
 \enum IFX_SI_GROUP_t
 \brief enum used to select how many input signals are supported.
 */
typedef enum {
    IFX_SI_GROUP_00         = 0,
    IFX_SI_GROUP_08         = 1,
    IFX_SI_GROUP_16         = 3,
} IFX_SI_GROUP_t;

/*!
  \enum IFX_SI_FPI_SHIFTCLK_DIV_t
  \brief enum used to configure shift-in clock.
 */
typedef enum {
    // FPID[1:0] For Serial Input Clk
    IFX_SI_FPI_SHIFTCLK_DIV_002 = 0,
    IFX_SI_FPI_SHIFTCLK_DIV_004 = 1,
    IFX_SI_FPI_SHIFTCLK_DIV_008 = 2,
    IFX_SI_FPI_SHIFTCLK_DIV_016 = 3,
} IFX_SI_FPI_SHIFTCLK_DIV_t;

/*!
  \enum IFX_SI_CONFIG_OP_MASK_t
  \brief enumerated key for field "operation_mask" of struct "ifx_si_config_param_t".
 */
typedef enum {
    IFX_SI_CFG_OP_UPDATE_SOURCE         = 1 << 0,   /*  for setting update source.                      */
    IFX_SI_CFG_OP_UPDATE_MODE           = 1 << 1,   /*  for setting sync/async mode.                    */
    IFX_SI_CFG_OP_UPDATE_GROUP          = 1 << 2,   /*  for setting number of input signals.            */
    IFX_SI_CFG_OP_UPDATE_SHIFTCLKDIV    = 1 << 3,   /*  for setting Shift-in clock through FPI CLK DIV. */
} IFX_SI_CONFIG_OP_MASK_t;

/*@}*/



/*!
  \addtogroup IFX_SI_STRUCT
 */
/*@{*/

/*!
  \struct ifx_si_ioctl_version_t
  \brief Structure used for query of driver version.
 */
typedef struct {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver   */
    unsigned int    minor;  /*!< output, minor number of driver */
} ifx_si_ioctl_version_t;

/*!
  \struct ifx_si_config_param_t
  \brief Structure used for configure SI Controller (Serial In).
 */
typedef struct {
    unsigned int                operation_mask;         /*!< input, Select operations to be performed */
    IFX_SI_UPD_SRC_t            update_source_type;     /*!< input, Corresponding update source (SW, GPT3 or FPID) */
    IFX_SI_FPI_SAMPLECLK_DIV_t  sampling_clk_div;       /*!< input, If FPID is chosen as update source type, set the divider; */
                                                        /*!< input, if GPT is chosen as update source type, set the frequency;*/
                                                        /*!< input, otherwise, it is ignored. */
    IFX_SI_SYNC_MODE_t          sync_mode;              /*!< input, 0: async mode (74x165), 1: sync mode (74x166) */
    IFX_SI_GROUP_t              input_group_type;       /*!< input, 0: only one is connected and hence only inputs [7:0] are enabled; */
                                                        /*!< input, 1: two is connected and both inputs [15:8] and [7:0] are enabled. */
    IFX_SI_FPI_SHIFTCLK_DIV_t   shift_in_clk_div;       /*!< input, Shift-in Clock Div setting */
} ifx_si_config_param_t;

/*!
  \struct ifx_si_eiu_config
  \brief Structure used for interrupt extension with SI Controller (Serial In).
  */
struct ifx_si_eiu_config {
    int             irq;            //  irq triggered by serial input, negative value means no SI EIU support
    unsigned int    intsync;        //  1: sync mode (74x166), 0: async mode (74x165)
    unsigned int    sampling_clk;   //  maximum frequency of sampling clock
    unsigned int    shift_clk;      //  maximum frequency of shift in clock
    unsigned int    group;          //  0: disabled, 1: 8 inputs, 2: 16 inputs
    unsigned int    active_high;    //  bit0-15: 0 - active low, 1 - active high
};

/*@}*/



/*!
  \addtogroup IFX_SI_IOCTL
 */
/*@{*/

#define IFX_SI_IOC_MAGIC            0xfd
/*!
  \def IFX_SI_IOC_VERSION
  \brief SI Controller IOCTL Command - Get driver version number.

   This command uses struct "ifx_si_ioctl_version" as parameter to SI Controller driver version number.
 */
#define IFX_SI_IOC_VERSION          _IOR(IFX_SI_IOC_MAGIC, 1, ifx_si_ioctl_version_t)

/*!
  \def IFX_SI_IOC_SET_CONFIG
  \brief SI Controller IOCTL Command - Config SI Controller (Serial In).

   This command uses struct "ifx_si_config_param_t" as parameter to configure SI Controller (Serial In).
 */
#define IFX_SI_IOC_SET_CONFIG       _IOW(IFX_SI_IOC_MAGIC, 2, ifx_si_config_param_t)

/*!
  \def IFX_SI_IOC_GET_SHIFTIN_DATA
  \brief SI Controller IOCTL Command - Read back shift-in data value (32-bit).

   This command uses "uint32_t" as parameter to store read-back shift-in data value.
 */
#define IFX_SI_IOC_GET_SHIFTIN_DATA _IOR(IFX_SI_IOC_MAGIC, 3, uint32_t)

/*!
  \def IFX_SI_IOC_TEST
  \brief SI Controller IOCTL Command - Run basic driver sanity check (debug).

   No parameter is needed for this command. Only for internal sanity check purpose.
 */
#define IFX_SI_IOC_TEST              _IO(IFX_SI_IOC_MAGIC, 0)

/* For checking endpoint */
#define IFX_SI_IOC_MAXNR               4

/*@}*/



#ifdef __KERNEL__
  extern unsigned int ifx_si_get_data(void);
  extern unsigned int ifx_si_get_bit(unsigned int bit);
  extern int ifx_si_config(ifx_si_config_param_t *param);
  extern int ifx_si_irq_active_high(unsigned int bit, int active_high);
  extern int ifx_si_irq_enable(unsigned int bit, int enable);
  extern unsigned int ifx_si_irq_ier(void);
  extern unsigned int ifx_si_irq_isr(void);
#endif



#endif  //  IFX_SI_H
