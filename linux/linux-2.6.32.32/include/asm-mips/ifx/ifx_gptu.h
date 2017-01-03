/******************************************************************************
**
** FILE NAME    : ifx_gptu.h
** PROJECT      : IFX UEIP
** MODULES      : GPTU
**
** DATE         : 28 May 2009
** AUTHOR       : Huang Xiaogang
** DESCRIPTION  : IFX General Purpose Timer Counter driver header file
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
** 28 May 2009  Huang Xiaogang The first UEIP release
*******************************************************************************/
#ifndef IFX_GPTU_H
#define IFX_GPTU_H

/*!
  \defgroup IFX_GPTU UEIP Project - GPTU driver module
  \brief UEIP Project - GPTU driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_GPTU_API APIs
  \ingroup IFX_GPTU
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_GPTU_IOCTL IOCTL Commands
  \ingroup IFX_GPTU
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_GPTU_STRUCT Structures
  \ingroup IFX_GPTU
  \brief Structures used by user application.
 */

/*! \file ifx_gptu.h
    \ingroup IFX_GPTU
    \brief This file contains the interface to GPTU driver.
*/	

/*
 *  Available Timer/Counter Index
 */
#define TIMER(n, X)                     (n * 2 + (X ? 1 : 0))
#define TIMER_ANY                       0x00
#define TIMER1A                         TIMER(1, 0)
#define TIMER1B                         TIMER(1, 1)
#define TIMER2A                         TIMER(2, 0)
#define TIMER2B                         TIMER(2, 1)
#define TIMER3A                         TIMER(3, 0)
#define TIMER3B                         TIMER(3, 1)

/*
 *  Flag of Timer/Counter
 *  These flags specify the way in which timer is configured.
 */
/*  Bit size of timer/counter.                      */
#define TIMER_FLAG_16BIT                0x0000
#define TIMER_FLAG_32BIT                0x0001
/*  Switch between timer and counter.               */
#define TIMER_FLAG_TIMER                0x0000
#define TIMER_FLAG_COUNTER              0x0002
/*  Stop or continue when overflowing/underflowing. */
#define TIMER_FLAG_ONCE                 0x0000
#define TIMER_FLAG_CYCLIC               0x0004
/*  Count up or counter down.                       */
#define TIMER_FLAG_UP                   0x0000
#define TIMER_FLAG_DOWN                 0x0008
/*  Count on specific level or edge.                */
#define TIMER_FLAG_HIGH_LEVEL_SENSITIVE 0x0000
#define TIMER_FLAG_LOW_LEVEL_SENSITIVE  0x0040
#define TIMER_FLAG_RISE_EDGE            0x0010
#define TIMER_FLAG_FALL_EDGE            0x0020
#define TIMER_FLAG_ANY_EDGE             0x0030
/*  Signal is syncronous to module clock or not.    */
#define TIMER_FLAG_UNSYNC               0x0000
#define TIMER_FLAG_SYNC                 0x0080
/*  Different interrupt handle type.                */
#define TIMER_FLAG_NO_HANDLE            0x0000
#if defined(__KERNEL__)
    #define TIMER_FLAG_CALLBACK_IN_IRQ  0x0100
#endif  //  defined(__KERNEL__)
#define TIMER_FLAG_SIGNAL               0x0300
/*  Internal clock source or external clock source  */
#define TIMER_FLAG_INT_SRC              0x0000
#define TIMER_FLAG_EXT_SRC              0x1000

#define IFX_GPTU_VER_MAJOR              0
#define IFX_GPTU_VER_MID                1
#define IFX_GPTU_VER_MINOR              15

/*!
  \addtogroup IFX_GPTU_STRUCT
 */
/*@{*/

/*!
  \struct ifx_gptu_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_gptu_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};

/*!
  \struct 	gptu_ioctl_param
  \brief 	data type used to call ioctl
 */     
struct gptu_ioctl_param {
    unsigned int	timer;	/*!< In command GPTU_REQUEST_TIMER, GPTU_SET_TIMER, and
                        		GPTU_SET_COUNTER, this field is ID of expected
                        		timer/counter. If it's zero, a timer/counter would
                        		be dynamically allocated and ID would be stored in
                        		this field.
                        		In command GPTU_GET_COUNT_VALUE, this field is
                        		ignored.
                        		In other command, this field is ID of timer/counter
                        		allocated.*/
    unsigned int    	flag;	/*!< In command GPTU_REQUEST_TIMER, GPTU_SET_TIMER, and
					GPTU_SET_COUNTER, this field contains flags to
					specify how to configure timer/counter.
 			                In command GPTU_START_TIMER, zero indicate start
                       			and non-zero indicate resume timer/counter.
                       			In other command, this field is ignored.*/
    unsigned long   	value; /*!<  In command GPTU_REQUEST_TIMER, this field contains
 					init/reload value.
					 In command GPTU_SET_TIMER, this field contains
 					frequency (0.001Hz) of timer.
 					In command GPTU_GET_COUNT_VALUE, current count
 					value would be stored in this field.
 					In command GPTU_CALCULATE_DIVIDER, this field
 					contains frequency wanted, and after calculation,
 					divider would be stored in this field to overwrite
 					the frequency.*/
	int		pid; /*!<  In command GPTU_REQUEST_TIMER and GPTU_SET_TIMER,
					if signal is required, this field contains process
					ID to which signal would be sent.
					In other command, this field is ignored.*/
	int		sig; /*!< In command GPTU_REQUEST_TIMER and GPTU_SET_TIMER,
					if signal is required, this field contains signal
					number which would be sent.
					In other command, this field is ignored.*/
};
/*@}*/

/*!
  \addtogroup IFX_GPTU_IOCTL
 */
/*@{*/

/*
 *  ioctl Command
 */
#define IFX_GPTU_IOC_MAGIC               'g'

/*!
  \def 	 IFX_GPTU_REQUEST_TIMER
  \brief General method to setup timer/counter.
 */
#define IFX_GPTU_REQUEST_TIMER              _IOWR(IFX_GPTU_IOC_MAGIC, 1, struct gptu_ioctl_param) 

/*!
  \def   IFX_GPTU_FREE_TIMER
  \brief Free timer/counter
 */
#define IFX_GPTU_FREE_TIMER                 _IOW(IFX_GPTU_IOC_MAGIC, 2, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_START_TIMER
  \brief Start or resume timer/counter
 */
#define IFX_GPTU_START_TIMER                _IOW(IFX_GPTU_IOC_MAGIC, 3, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_STOP_TIMER
  \brief Suspend timer/counter
 */
#define IFX_GPTU_STOP_TIMER                 _IOW(IFX_GPTU_IOC_MAGIC, 4, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_GET_COUNT_VALUE
  \brief Get current count value
 */
#define IFX_GPTU_GET_COUNT_VALUE            _IOWR(IFX_GPTU_IOC_MAGIC, 5, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_CALCULATE_DIVIDER
  \brief Calculate timer divider from given freq
 */
#define IFX_GPTU_CALCULATE_DIVIDER          _IOWR(IFX_GPTU_IOC_MAGIC, 6, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_SET_TIMER
  \brief Simplified method to setup timer
 */
#define IFX_GPTU_SET_TIMER                  _IOWR(IFX_GPTU_IOC_MAGIC, 7, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_SET_COUNTER
  \brief Simplified method to setup counter
 */
#define IFX_GPTU_SET_COUNTER                _IOWR(IFX_GPTU_IOC_MAGIC, 8, struct gptu_ioctl_param)

/*!
  \def   IFX_GPTU_VERSION
  \brief Get GPTU driver version number
 */
#define IFX_GPTU_VERSION                    _IOR(IFX_GPTU_IOC_MAGIC, 9, struct ifx_gptu_ioctl_version)
/*@}*/

typedef void (*timer_callback)(unsigned long arg);

extern int ifx_gptu_timer_request(unsigned int, unsigned int, unsigned long, unsigned long, unsigned long);
extern int ifx_gptu_timer_free(unsigned int);
extern int ifx_gptu_timer_start(unsigned int, int);
extern int ifx_gptu_timer_stop(unsigned int);
extern int ifx_gptu_counter_flags_reset(u32 timer, u32 flags);
extern int ifx_gptu_countvalue_get(unsigned int, unsigned long *);
extern u32 ifx_gptu_divider_cal(unsigned long);
extern int ifx_gptu_timer_set(unsigned int, unsigned int, int, int, unsigned int, unsigned long, unsigned long);
extern int ifx_gptu_counter_set(unsigned int timer, unsigned int flag,
	u32 reload, unsigned long arg1, unsigned long arg2);

#endif /* IFX_GPTU_H */
