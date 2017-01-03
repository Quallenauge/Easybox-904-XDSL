/******************************************************************************
**
** FILE NAME    : ifxmips_asc.c
** PROJECT      : IFX UEIP
** MODULES      : ASC (UART)
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global IFX ASC (UART) driver source file
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



#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/serial_core.h>
#include <linux/spinlock.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_pmcu.h>
#include "ifxmips_asc.h"

#define IFX_ASC_VER_MAJOR               1
#define IFX_ASC_VER_MIDDLE              0
#define IFX_ASC_VER_MINOR               9

#ifndef PORT_IFX_ASC
  #define PORT_IFX_ASC                  43
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
  typedef struct uart_state             UART_PORT_STATE;
  #define UART_PORT_TO_STATE(port)      ((port)->state)
  #define UART_PORT_TO_TTY(port)        ((port)->state->port.tty)
  #define UART_PORT_TO_XMIT(port)       (&((port)->state->xmit))
  #define UART_PORT_TO_MSR(port)        ((port)->state->port.delta_msr_wait)
#else
  typedef struct uart_info              UART_PORT_STATE;
  #define UART_PORT_TO_STATE(port)      ((port)->info)
  #define UART_PORT_TO_TTY(port)        ((port)->info->tty)
  #define UART_PORT_TO_XMIT(port)       (&((port)->info->xmit))
  #define UART_PORT_TO_MSR(port)        ((port)->info->delta_msr_wait)
#endif

#define SERIAL_IFX_ASC_MAJOR            TTY_MAJOR
#define SERIAL_IFX_ASC_MINOR            64

/* fake flag to indicate CREAD was not set -> throw away all bytes */
#define UART_DUMMY_UER_RX               1

#define ENABLE_IRQ(n)                   enable_irq((n))
#define DISABLE_IRQ(n)                  disable_irq_nosync((n))

#if defined(CONFIG_SERIAL_IFX_ASC_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
  #define SUPPORT_SYSRQ                 1
#endif

static u_int ifx_asc_tx_empty(struct uart_port *);
static void ifx_asc_stop_tx(struct uart_port *);
static void ifx_asc_start_tx(struct uart_port *);
static void ifx_asc_stop_rx(struct uart_port *);
static void ifx_asc_enable_ms(struct uart_port *);
static u_int ifx_asc_get_mctrl(struct uart_port *);
static void ifx_asc_set_mctrl(struct uart_port *, unsigned int);
static void ifx_asc_set_termios(struct uart_port *, struct ktermios *, struct ktermios *);
static void ifx_asc_break_ctl(struct uart_port *, int);
static int ifx_asc_startup(struct uart_port *);
static void ifx_asc_shutdown(struct uart_port *);
static const char *ifx_asc_type(struct uart_port *);
static void ifx_asc_release_port(struct uart_port *);
static int ifx_asc_request_port(struct uart_port *);
static void ifx_asc_config_port(struct uart_port *, int);
static int ifx_asc_verify_port(struct uart_port *, struct serial_struct *);

static void ifx_asc_rx_chars(struct uart_port *);
static void ifx_asc_serial_out(struct uart_port *, char);
static void ifx_asc_tx_chars(struct uart_port *port);
static irqreturn_t ifx_asc_tx_int(int, void *);
static irqreturn_t ifx_asc_er_int(int, void *);
static irqreturn_t ifx_asc_rx_int(int, void *);

static inline void config_gpio(void);
static inline void config_pmu(void);
static unsigned int cflag_to_baudrate(unsigned int);
static void get_fdv_and_reload_value(unsigned int, unsigned int *, unsigned int *);
static void ifx_asc_init_hardware(void);

#ifdef CONFIG_SERIAL_IFX_ASC_CONSOLE /*SUPPORT_SYSRQ*/
#ifdef used_and_not_const_char_pointer
static int ifx_asc_console_read(struct uart_port *port, char *s, u_int count);
#endif
static void ifx_asc_console_write(struct console *co, const char *s, u_int count);
struct tty_driver *ifx_asc_console_device(struct console *co, int *index);
static void __init ifx_asc_console_get_options(struct uart_port   *port, int *baud, int *parity, int *bits);
static int __init ifx_asc_console_setup(struct console *co, char *options);
#endif

static inline void proc_file_create(void);
static inline void proc_file_delete(void);
static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data);

static inline int ifx_asc_version(char *buf);

static int ifx_asc_port_initialized = 0;
static int ifx_asc_hw_initialized = 0;

static struct uart_port ifx_asc_port[UART_NR];

static struct uart_ops ifx_asc_pops = {
    .tx_empty       = ifx_asc_tx_empty,
    .set_mctrl      = ifx_asc_set_mctrl,
    .get_mctrl      = ifx_asc_get_mctrl,
    .stop_tx        = ifx_asc_stop_tx,
    .start_tx       = ifx_asc_start_tx,
    .stop_rx        = ifx_asc_stop_rx,
    .enable_ms      = ifx_asc_enable_ms,
    .break_ctl      = ifx_asc_break_ctl,
    .startup        = ifx_asc_startup,
    .shutdown       = ifx_asc_shutdown,
    .set_termios    = ifx_asc_set_termios,
    .type           = ifx_asc_type,
    .release_port   = ifx_asc_release_port,
    .request_port   = ifx_asc_request_port,
    .config_port    = ifx_asc_config_port,
    .verify_port    = ifx_asc_verify_port,
};

#ifdef CONFIG_SERIAL_IFX_ASC_CONSOLE
static struct uart_driver ifx_asc_drv;

static struct uart_port *g_asc_console_port = NULL;

static struct console ifx_asc_console = {
    .name           = "ttyS",
    .write          = ifx_asc_console_write,
#ifdef used_and_not_const_char_pointer
    .read           = ifx_asc_console_read,
#else
    .read           = NULL,
#endif
    .device         = ifx_asc_console_device,
    .setup          = ifx_asc_console_setup,
    .flags          = CON_PRINTBUFFER,
    .index          = IFX_ASC_CONSOLE_INDEX,
    .data           = &ifx_asc_drv,
};

#define IFX_ASC_CONSOLE        &ifx_asc_console
#else
#define IFX_ASC_CONSOLE        NULL
#endif

static struct uart_driver ifx_asc_drv = {
    .owner          = THIS_MODULE,
    .driver_name    = "ttyS",
    .dev_name       = "ttyS",
    .major          = TTY_MAJOR,
    .minor          = 64,
    .nr             = UART_NR,
    .cons           = IFX_ASC_CONSOLE,
    .state          = NULL,
    .tty_driver     = NULL
};

static struct proc_dir_entry* g_asc_dir = NULL;

#ifdef CONFIG_IFX_PMCU
  static IFX_PMCU_STATE_t g_uart_pwm_state = IFX_PMCU_STATE_D0;
#endif
static int g_clock_changing[UART_NR] = {0};
static char *g_console_buf[UART_NR] = {0};
static int g_console_buf_count[UART_NR] = {0};

/**
 * \fn    unsigned int ifx_asc_tx_empty(struct uart_port *port)
 *
 * \brief This function is used to check whether TX FIFO is empty or not
 *
 * \param port - pointer to struct uart_port
 *
 * \return  0 - TX FIFO is not empty
 *          TIOCSER_TEMT - TX FIFO is physically empty
 */
static unsigned int ifx_asc_tx_empty(struct uart_port *port)
{
    /*
     * FSTAT tells exactly how many bytes are in the FIFO.
     * The question is whether we really need to wait for all
     * 16 bytes to be transmitted before reporting that the
     * transmitter is empty.
     */
    return (ifx_asc_port_priv[port->line].base->asc_fstat & ASCFSTAT_TXFFLMASK) ? 0 : TIOCSER_TEMT;
}

/**
 * \fn    void ifx_asc_stop_tx(struct uart_port *port)
 * \brief Stop transmission
 *
 * This function stops transmission by disabling the Tx interrupt.
 *
 * \param port - pointer to struct uart_port
 *
 * \return      NONE
 */
static void ifx_asc_stop_tx(struct uart_port *port)
{
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    unsigned long flags;

    spin_lock_irqsave(&priv->lock, flags);
    if ( priv->tx_irq_on ) {
        priv->base->asc_irnen &= ~IFX_ASC_IRQ_LINE_TIR;
        DISABLE_IRQ(priv->tir);
        priv->tx_irq_on = 0;
    }
    spin_unlock_irqrestore(&priv->lock, flags);
}

/**
 * \fn void ifx_asc_start_tx(struct uart_port *port)
 *
 * \brief This function initiate a transmision by enabling the Tx interrupt. If the info
 * structure has already been set, buffered chars will be sent out if necessary.
 *
 * \param port - pointer to struct uart_port
 * \return None
 */
static void ifx_asc_start_tx(struct uart_port *port)
{
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    unsigned long flags;

    spin_lock_irqsave(&priv->lock, flags);
    if ( priv->tx_irq_on == 0 ) {
        ENABLE_IRQ(priv->tir);
        priv->base->asc_irnen |= IFX_ASC_IRQ_LINE_TIR;
        priv->tx_irq_on++;
    }
    spin_unlock_irqrestore(&priv->lock, flags);

    if ( UART_PORT_TO_STATE(port) ) {
        ifx_asc_tx_chars(port);
    }
}

/**
 * \fn     void ifx_asc_stop_rx(struct uart_port *port)
 *
 * \brief  This function is used to stop the transmision
 *
 * \param  port  pointer to struct uart_port
 *
 * \return  None
 */
static void ifx_asc_stop_rx(struct uart_port *port)
{
    /* clear the RX enable bit */
    ifx_asc_port_priv[port->line].base->asc_whbstate = ASCWHBSTATE_CLRREN;
}

/**
 * \fn   void ifx_asc_enable_ms(struct uart_port *port)
 *
 * \brief Enable modem signals.
 *
 * This function should enable modem signals, but this is not supported.
 *
 * \param  port     pointer to struct uart_port
 *
 * \return  None
 */
static void ifx_asc_enable_ms(struct uart_port *port)
{
    /* no modem signals */
    return;
}

/**
 * \fn     unsigned int ifx_asc_get_mctrl(struct uart_port *port)
 *
 * \brief  This function is used to get the modem signal status
 *
 * \param  port     pointer to struct uart_port
 *
 * \return  CTS/Carrier Detection/Data service ready
 */

static unsigned int ifx_asc_get_mctrl(struct uart_port *port)
{
    /* no modem control signals - the readme says to pretend all are set */
    return TIOCM_CTS | TIOCM_CAR | TIOCM_DSR;
}

/**
 * \fn    void ifx_asc_set_mctrl(struct uart_port *port, unsigned int mctrl)
 *
 * \brief  This function is used to set the modem signals
 *
 * \param  port     pointer to struct uart_port
 * \param  mctrl    modem control signals
 *
 * \return  None
 *
 */
static void ifx_asc_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
    /* no modem control - just return */
    return;
}

/**
 * \fn     void ifx_asc_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
 *
 * \brief  This function is used to update the serial hardware setup,
 *         due to change in teminal I/O settings.
 *
 *         Update all the ASC hardware setup depending on the terminal I/O setup
 *
 * \param  port        pointer to struct uart_port
 * \param  termios     structure pointer to present terminal I/O settngs
 * \param  old         structure pointer to old terminal I/O settngs
 *
 * \return  None
 */
static void ifx_asc_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
{
    unsigned int cflag = termios->c_cflag;
#define F_SIZE_CHG          0x01
#define F_BAUDRATE_CHG      0x02
    unsigned int f_chg = F_SIZE_CHG | F_BAUDRATE_CHG;
    ifx_asc_reg_t *asc_reg;
    u32 word_length = 0;
    u32 baudrate = 0;
    u32 fdv, reload;
    u32 asc_state;

    if ( (cflag & CSIZE) != CS7 && (cflag & CSIZE) != CS8 ) {
        cflag = (cflag & ~CSIZE) | (old ? (old->c_cflag & CSIZE) : CS8);
    }

    termios->c_cflag = cflag;

    //  only support baudrate change and word length change
    if ( old ) {
        if ( (old->c_cflag & CSIZE) == (cflag & CSIZE) ) {
            f_chg &= ~F_SIZE_CHG;
        }
        if ( (old->c_cflag & CBAUD) == (cflag & CBAUD) ) {
            f_chg &= ~F_BAUDRATE_CHG;
        }
    }

    if ( f_chg ) {
        asc_reg = ifx_asc_port_priv[port->line].base;

        if ( (f_chg & F_SIZE_CHG) ) {
            if ( (cflag & CSIZE) == CS7 ) {
                word_length = ASCMCON_WLS_7BIT << ASCMCON_WLSOFFSET;
            }
            else {
                word_length = ASCMCON_WLS_8BIT << ASCMCON_WLSOFFSET;
            }
        }

        if ( (f_chg & F_BAUDRATE_CHG) ) {
            baudrate = cflag_to_baudrate(cflag);
            if ( baudrate ) {
                get_fdv_and_reload_value(baudrate, &fdv, &reload);
                ifx_asc_port_priv[port->line].baudrate = baudrate;
            }
        }

        if ( (f_chg & F_BAUDRATE_CHG) && !baudrate ) {
            asc_state = 0;
        }
        else {
            asc_state = asc_reg->asc_state;
        }

        /* disable receiver */
        asc_reg->asc_whbstate = ASCWHBSTATE_CLRREN;
        if ( (f_chg & F_SIZE_CHG) ) {
            asc_reg->asc_mcon = (asc_reg->asc_mcon & ~(3 << 2)) | word_length;
        }
        if ( (f_chg & F_BAUDRATE_CHG) ) {
            /* Now we can write the new baudrate into the register */
            asc_reg->asc_fdv = fdv;
            asc_reg->asc_bg  = reload;
        }
        /* enable receiver if necessary */
        if ( (asc_state & ASCSTATE_REN) ) {
            asc_reg->asc_whbstate = ASCWHBSTATE_SETREN;
        }
    }
}

static void ifx_asc_break_ctl(struct uart_port *port, int break_state)
{
    /* no way to send a break */
    return;
}

static int ifx_asc_startup(struct uart_port *port)
{
    int retval;
    unsigned long flags;
    unsigned int line = port->line;
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[line];
    ifx_asc_reg_t *asc_reg = priv->base;

#ifdef CONFIG_IFX_DISABLE_ASC0
    if ( line == 0 )
        return -EIO;
#endif

    /* this assumes: CON.BRS = CON.FDE = 0 */

    /* block the IRQs */
    local_irq_save(flags);

    /*
     * XXX, Need to set the ASC registers here since we unset them in shutdown. Will
     * cause problems with new kernels which do a shutdown/startup...
     */
    asc_reg->asc_irnen = IFX_ASC_IRQ_LINE_MASK_ALL;

    /* Set fifo trigger level to 1 and flush fifo */
    asc_reg->asc_rxfcon = (IFX_ASC_RXFIFO_FL << ASCRXFCON_RXFITLOFF) | ASCRXFCON_RXFFLU;
    asc_reg->asc_txfcon = (IFX_ASC_TXFIFO_FL << ASCTXFCON_TXFITLOFF) | ASCTXFCON_TXFFLU;

    /* enable the fifos */
    asc_reg->asc_rxfcon |= ASCRXFCON_RXFEN;
    asc_reg->asc_txfcon |= ASCTXFCON_TXFEN;

    /* turn on baudrate generator */
    asc_reg->asc_mcon |= ASCMCON_R;

    /* enable receiver */
    asc_reg->asc_whbstate = ASCWHBSTATE_SETREN;

    local_irq_restore(flags);

    retval = request_irq(priv->rir, ifx_asc_rx_int, IRQF_DISABLED, priv->rx_irq_name, port);
    if ( retval )
        goto REQUEST_RIR_IRQ_FAIL;
    retval = request_irq(priv->tir, ifx_asc_tx_int, IRQF_DISABLED, priv->tx_irq_name,   port);
    if ( retval )
        goto REQUEST_TIR_IRQ_FAIL;
    DISABLE_IRQ(priv->tir);
    priv->tx_irq_on = 0;
    retval = request_irq(priv->eir, ifx_asc_er_int, IRQF_DISABLED, priv->err_irq_name, port);
    if ( retval )
        goto REQUEST_EIR_IRQ_FAIL;

    /* enable ASC interrupts in module */
    asc_reg->asc_irnen = IFX_ASC_IRQ_LINE_RIR | IFX_ASC_IRQ_LINE_EIR;

    return 0;

REQUEST_EIR_IRQ_FAIL:
    free_irq(ifx_asc_port_priv[line].tir, port);
REQUEST_TIR_IRQ_FAIL:
    free_irq(ifx_asc_port_priv[line].rir, port);
REQUEST_RIR_IRQ_FAIL:
    return retval;
}

static void ifx_asc_shutdown(struct uart_port *port)
{
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    ifx_asc_reg_t *asc_reg = priv->base;

    /* disable ASC interrupts in module */
    asc_reg->asc_irnen = IFX_ASC_IRQ_LINE_MASK_ALL;

    disable_irq(priv->rir);
    disable_irq(priv->tir);
    disable_irq(priv->eir);

    /*
     * Free the interrupts
     */
    free_irq(priv->rir, port);
    free_irq(priv->tir, port);
    free_irq(priv->eir, port);

    /* turn off baudrate generator */
    asc_reg->asc_mcon &= ~ASCMCON_R;

    /* flush and then disable the fifos */
    asc_reg->asc_rxfcon = ASCRXFCON_RXFFLU;
    asc_reg->asc_txfcon = ASCTXFCON_TXFFLU;
}

static const char *ifx_asc_type(struct uart_port *port)
{
    return port->type == PORT_IFX_ASC ? "IFX_ASC" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'
 */
static void ifx_asc_release_port(struct uart_port *port)
{
    return;
}

/*
 * Request the memory region(s) being used by 'port'
 */
static int ifx_asc_request_port(struct uart_port *port)
{
    return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void ifx_asc_config_port(struct uart_port *port, int flags)
{
    if ( (flags & UART_CONFIG_TYPE) ) {
        port->type = PORT_IFX_ASC;
        ifx_asc_request_port(port);
    }
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int ifx_asc_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    int ret = 0;

    if ( ser->type != PORT_UNKNOWN && ser->type != PORT_IFX_ASC )
        ret = -EINVAL;
    if ( ser->irq < 0 || ser->irq >= NR_IRQS )
        ret = -EINVAL;
    if ( ser->baud_base < 9600 )
        ret = -EINVAL;

    return ret;
}

/**
 * Receive chars.
 * This function reads received characters from the Rx fifo and stores them in
 * the tty buffer.
 *
 * \param info - Info structure for this uart device.
 * \param regs - In case of system request support the current context registers
 *               are also given.
 */
static void ifx_asc_rx_chars(struct uart_port *port)
{
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    ifx_asc_reg_t *asc_reg = priv->base;
    struct tty_struct *tty = UART_PORT_TO_TTY(port);
    unsigned int ch = 0, rsr = 0, fifocnt;
    char flag;

    fifocnt = asc_reg->asc_fstat & ASCFSTAT_RXFFLMASK;
    while (fifocnt--) {
        ch = asc_reg->asc_rbuf;
        rsr = (asc_reg->asc_state & ASCSTATE_ANY) | UART_DUMMY_UER_RX;

        flag = TTY_NORMAL;
        port->icount.rx++;
        priv->rx_bytes++;

        /*
         * Note that the error handling code is
         * out of the main execution path
         */
        if ( (rsr & ASCSTATE_ANY) ) {
            if ( (rsr & ASCSTATE_PE) ) {
                port->icount.parity++;
                priv->rx_parity_error++;
                asc_reg->asc_whbstate = ASCWHBSTATE_CLRPE;
            }
            else if ( (rsr & ASCSTATE_FE) ) {
                port->icount.frame++;
                priv->rx_frame_error++;
                asc_reg->asc_whbstate = ASCWHBSTATE_CLRFE;
            }

            if ( (rsr & ASCSTATE_ROE) ) {
                port->icount.overrun++;
                priv->rx_overrun_error++;
                asc_reg->asc_whbstate = ASCWHBSTATE_CLRROE;
            }

            rsr &= port->read_status_mask;
            if ( (rsr & ASCMCON_PAL) )
                flag = TTY_PARITY;
            else if ( (rsr & ASCMCON_FEN) )
                flag = TTY_FRAME;
        }

#ifdef SUPPORT_SYSRQ
        if ( uart_handle_sysrq_char(port, ch) )
            continue;
#endif

        uart_insert_char(port, rsr, ASCMCON_ROEN, ch, flag);
    }

    if ( ch != 0 ) //  If the final ch = 0, we will not do tty_flip_buffer_push.
        tty_flip_buffer_push(tty);

    return;
}

static void ifx_asc_serial_out(struct uart_port *port, char ch)
{
    int fifocnt;
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    ifx_asc_reg_t *asc_reg = priv->base;
    unsigned long flags;

    do {
        fifocnt = asc_reg->asc_fstat & ASCFSTAT_TXFFLMASK;
    } while ( fifocnt == (IFX_ASC_TXFIFO_FULL << ASCFSTAT_TXFFLOFF) );

    spin_lock_irqsave(&priv->lock, flags);
    if ( g_clock_changing[port->line] ) {
        if ( g_console_buf[port->line] && g_console_buf_count[port->line] < PAGE_SIZE )
            g_console_buf[port->line][g_console_buf_count[port->line]++] = ch;
        spin_unlock_irqrestore(&priv->lock, flags);
        return;
    }
    /* We have either portwidth = 8 or portwidth = 32 */
    if ( priv->portwidth == 8 ) {
        asc_reg->asc_tbuf = ch;
    }
    else {
        *(((char*)&asc_reg->asc_tbuf) + 3) = ch;
    }
    asm("sync");
    spin_unlock_irqrestore(&priv->lock, flags);
}

/**
 * Transmit chars.
 * This function transmits the characters currently stored in the tty tx buffer.
 * If the buffer is empty, the transmitter is stopped.
 *
 * \param info - Info structure for this uart device.
 */
static void ifx_asc_tx_chars(struct uart_port *port)
{
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    UART_PORT_STATE *state = UART_PORT_TO_STATE(port);

    if ( state->xmit.head == state->xmit.tail
      || UART_PORT_TO_TTY(port)->stopped
      || UART_PORT_TO_TTY(port)->hw_stopped
      || g_clock_changing[port->line] ) {
        ifx_asc_stop_tx(port);
        return;
    }

    if ( port->x_char ) {
        ifx_asc_serial_out(port, port->x_char);
        port->icount.tx++;
        port->x_char = 0;
        priv->tx_bytes++;
        return;
    }

    ifx_asc_serial_out(port, state->xmit.buf[state->xmit.tail]);
    state->xmit.tail = (state->xmit.tail + 1) & (UART_XMIT_SIZE - 1);
    port->icount.tx++;
    priv->tx_bytes++;

    if ( CIRC_CNT(state->xmit.head, state->xmit.tail, UART_XMIT_SIZE) < WAKEUP_CHARS )
        uart_write_wakeup(port);

    if ( state->xmit.head == state->xmit.tail )
        ifx_asc_stop_tx(port);
}

static irqreturn_t ifx_asc_tx_int(int irq, void *dev_id)
{
    struct uart_port *port = (struct uart_port *)dev_id;

    /* this interrupt tells us that the TXB is empty - fill it */
    ifx_asc_tx_chars(port);

    return IRQ_HANDLED;
}

static irqreturn_t ifx_asc_er_int(int irq, void *dev_id)
{
    struct uart_port *port = (struct uart_port *)dev_id;

    ifx_asc_port_priv[port->line].base->asc_whbstate = ASCWHBSTATE_CLRPE | ASCWHBSTATE_CLRFE | ASCWHBSTATE_CLRROE
                                                     | ASCWHBSTATE_SETRUE | ASCWHBSTATE_SETTOE | ASCWHBSTATE_SETBE;

    return IRQ_HANDLED;
}

static irqreturn_t ifx_asc_rx_int(int irq, void *dev_id)
{
    struct uart_port *port = (struct uart_port *)dev_id;

    ifx_asc_rx_chars(port);

    return IRQ_HANDLED;
}

static void ifx_asc_port_init(void)
{
    if ( !ifx_asc_port_initialized ) {
        int i;

        for ( i = 0; i < NUM_ENTITY(ifx_asc_port); i++ ) {
            spin_lock_init(&ifx_asc_port_priv[i].lock);

            ifx_asc_port_priv[i].portwidth = (ifx_asc_port_priv[i].base->asc_id & ASCID_TX32) ? 32 : 8;

            memset(&ifx_asc_port[i], 0, sizeof(ifx_asc_port[i]));

            ifx_asc_port[i].iobase      = (unsigned int)ifx_asc_port_priv[i].base;
            ifx_asc_port[i].membase     = (unsigned char __iomem *)ifx_asc_port_priv[i].base;
            ifx_asc_port[i].mapbase     = (unsigned long)ifx_asc_port_priv[i].base;
            ifx_asc_port[i].iotype      = SERIAL_IO_MEM;
            ifx_asc_port[i].irq         = ifx_asc_port_priv[i].rir;
            ifx_asc_port[i].uartclk     = ifx_get_fpi_hz();
            ifx_asc_port[i].fifosize    = (ifx_asc_port_priv[i].base->asc_id & ASCID_TXFS_MASK) >> ASCID_TXFS_OFF;
//            ifx_asc_port[i].unused[0]   = ifx_asc_port_priv[i].tir;
//            ifx_asc_port[i].unused[1]   = ifx_asc_port_priv[i].eir;
//            ifx_asc_port[i].unused[2]   = ifx_asc_port_priv[i].tbir;
            ifx_asc_port[i].type        = PORT_IFX_ASC;
            ifx_asc_port[i].ops         = &ifx_asc_pops;
            ifx_asc_port[i].flags       = ASYNC_BOOT_AUTOCONF;
            ifx_asc_port[i].line        = i;
        }

        ifx_asc_port_initialized++;
    }
}

static inline void config_gpio(void)
{
#if (defined(CONFIG_DANUBE) || defined(CONFIG_AR9)) && !defined(CONFIG_IFX_DISABLE_ASC0)
    //  if ASC0 is enabled, configure GPIO for ASC0
    //  otherwise, although ttyS0 is still created, GPIO for ASC0 is not configured
    ifx_gpio_deregister(IFX_GPIO_MODULE_ASC0);
    ifx_gpio_register(IFX_GPIO_MODULE_ASC0);
#endif
}

static inline void config_pmu(void)
{
#if (defined(CONFIG_DANUBE) || defined(CONFIG_AR9)) && !defined(CONFIG_IFX_DISABLE_ASC0)
    //  if ASC0 is enabled, enable it in PMU register
    //  otherwise, although ttyS0 is still created, bit in PMU register is not enabled
    UART0_PMU_SETUP(IFX_PMU_ENABLE);
#endif
}

static unsigned int cflag_to_baudrate(unsigned int cflag)
{
    unsigned int baud[] = {
        B1200,      1200,
        B2400,      2400,
        B4800,      4800,
        B9600,      9600,
        B19200,     19200,
        B38400,     38400,
        B57600,     57600,
        B115200,    115200,
        B230400,    230400,
        B460800,    460800,
        B921600,    921600,
    };
    int i;

    cflag &= CBAUD;
    for ( i = 0; i < NUM_ENTITY(baud); i += 2 ) {
        if ( cflag == baud[i] )
            return baud[i + 1];
    }

    return 0;
}

static void get_fdv_and_reload_value(unsigned int baudrate, unsigned int *fdv, unsigned int *reload)
{
    unsigned int fpi_clk = ifx_get_fpi_hz();
    unsigned int baudrate1 = baudrate * 8192;
    unsigned long long baudrate2 = (unsigned long long)baudrate * 1000;
    unsigned long long fdv_over_bg_fpi;
    unsigned long long fdv_over_bg;
    unsigned long long difference;
    unsigned long long min_difference;
    unsigned int bg;

    /* Sanity check first */
    if (baudrate >= (fpi_clk >> 4)) {
        printk(KERN_ERR "%s current fpi clock %u can't provide baudrate %u!!!\n",
            __func__, fpi_clk, baudrate);
        return;
    }
    //  baudrate = fpiclk * (fdv / 512) / (16 * (bg + 1))

    min_difference = UINT_MAX;
    fdv_over_bg_fpi = baudrate1;
    for ( bg = 1; bg <= 8192; bg++, fdv_over_bg_fpi += baudrate1 ) {
        fdv_over_bg = fdv_over_bg_fpi + fpi_clk / 2;
        do_div(fdv_over_bg, fpi_clk);
        if ( fdv_over_bg <= 512 ) {
            difference = fdv_over_bg * fpi_clk * 1000;
            do_div(difference, 8192 * bg);
            if ( difference < baudrate2 )
                difference = baudrate2 - difference;
            else
                difference -= baudrate2;
            if ( difference < min_difference ) {
                *fdv = (unsigned int)fdv_over_bg & 511;
                *reload = bg - 1;
                min_difference = difference;
            }
            /* Perfect one found */
            if (min_difference == 0) {
                break;
            }
        }
    }

#if defined(CONFIG_USE_EMULATOR)
    prom_printf("%s: fpi_clk = %u, baudrate = %u, fdv = %u, bg = %u, diff = %u.%u\n", __func__, fpi_clk, baudrate, *fdv, *reload, (u32)min_difference / 1000, (u32)min_difference % 1000);
#endif
}

static void ifx_asc_init_hardware(void)
{
    //  Console init is called too early
    //  no driver/kernel init yet
    //  so config_gpio may be called twice
    //  once in console init
    //  the other in ASC driver init
    config_gpio();

    if ( !ifx_asc_hw_initialized ) {
        u32 fdv, reload;
        ifx_asc_reg_t *asc_reg;
        int i;

        config_pmu();

        get_fdv_and_reload_value(CONFIG_IFX_ASC_DEFAULT_BAUDRATE, &fdv, &reload);

        for ( i = 0; i < NUM_ENTITY(ifx_asc_port_priv); i++ ) {
            ifx_asc_port_priv[i].baudrate = CONFIG_IFX_ASC_DEFAULT_BAUDRATE;

            asc_reg = ifx_asc_port_priv[i].base;

            /* Set CLC register*/
            asc_reg->asc_clc = 0x00000100;

            /* Initialy we are in async mode */
            asc_reg->asc_mcon = ASCMCON_M_8ASYNC | 0x00c00000;

            /* Set TXFIFO's filling level and enable FIFO */
            asc_reg->asc_txfcon |= ASCTXFCON_TXFEN;

            /* Set RXFIFO's filling level and enable FIFO */
            asc_reg->asc_rxfcon |= ASCRXFCON_RXFEN;

            /* enable error signals */
            asc_reg->asc_mcon |= ASCMCON_FEN;

            /* Clear all error interrupts and disable receiver */
            asc_reg->asc_whbstate = ASCWHBSTATE_CLRPE | ASCWHBSTATE_CLRFE | ASCWHBSTATE_CLRRUE | ASCWHBSTATE_CLRROE | ASCWHBSTATE_CLRTOE | ASCWHBSTATE_CLRBE;

            asc_reg->asc_eomcon = 0x00010300;

            /* set up to use divisor of 2 */
            asc_reg->asc_mcon |= ASCMCON_FDE;

            /* now we can write the new baudrate into the register */
            asc_reg->asc_fdv = fdv;
            asc_reg->asc_bg = reload;
        }

        ifx_asc_hw_initialized = 1;
    }
}

#ifdef CONFIG_SERIAL_IFX_ASC_CONSOLE

# ifdef used_and_not_const_char_pointer
static int ifx_asc_console_read(struct uart_port *port, char *s, u_int count)
{
    unsigned long flags;
    ifx_asc_port_priv_t *priv = &ifx_asc_port_priv[port->line];
    ifx_asc_reg_t *asc_reg = priv->base;
    int fifocnt;
    int c = 0;

    /* block the IRQ */
    local_irq_save(flags);

    fifocnt = asc_reg->asc_fstat & ASCFSTAT_RXFFLMASK;
    while ( c < count && fifocnt ) {
        *s++ = asc_reg->asc_rbuf;
        c++;
        fifocnt--;
    }

    /* clear any pending interrupts (RIR) is not necessary */

    /* unblock the IRQ */
    local_irq_restore(flags);

    /* return the count */
    return c;
}
# endif

static void ifx_asc_console_write(struct console *co, const char *s, u_int count)
{
    unsigned long flags;
    int i;

    if ( !g_asc_console_port )
        return;

    /* block the IRQ */
    local_irq_save(flags);

    /*
     *  Now, do each character
     */
    for ( i = 0; i < count; i++ ) {
        if ( s[i] == '\n' )
            ifx_asc_serial_out(g_asc_console_port,'\r');
        ifx_asc_serial_out(g_asc_console_port, s[i]);
    }

    /* clear any pending interrupts (TIR) is not necessary */

    /* restore the IRQ */
    local_irq_restore(flags);
}

struct tty_driver *ifx_asc_console_device(struct console *co, int *index)
{
    struct uart_driver *p = co->data;

    *index = co->index;

    return p ? p->tty_driver : NULL;
}

static void __init ifx_asc_console_get_options(struct uart_port *port, int *baud, int *parity, int *bits)
{
    ifx_asc_reg_t *asc_reg = ifx_asc_port_priv[port->line].base;
    u_int lcr_h;

    lcr_h = asc_reg->asc_mcon;

    /* do this only if the ASC is turned on */
    if ( lcr_h & ASCMCON_R ) {
        u_int quot, div, fdiv, frac;

        *parity = 'n';
        if ( (lcr_h & (ASCMCON_MODEMASK | ASCMCON_PEN)) == (ASCMCON_M_7ASYNC | ASCMCON_PEN) ||
                    (lcr_h & (ASCMCON_MODEMASK | ASCMCON_PEN)) == (ASCMCON_M_8ASYNC | ASCMCON_PEN) ) {
            if (lcr_h & ASCMCON_ODD)
                *parity = 'o';
            else
                *parity = 'e';
        }

        if ( (lcr_h & ASCMCON_MODEMASK) == ASCMCON_M_7ASYNC )
            *bits = 7;
        else
            *bits = 8;

        quot = asc_reg->asc_bg + 1;
        /* this gets hairy if the fractional divider is used */
        if ( (lcr_h & ASCMCON_FDE) ) {
            div = 1;
            fdiv = asc_reg->asc_fdv;
            if ( fdiv == 0 )
                fdiv = 512;
            frac = 512;
        }
        else {
            div = lcr_h & ASCMCON_BRS ? 3 : 2;
            fdiv = frac = 1;
        }
        /*
         * This doesn't work exactly because we use integer
         * math to calculate baud which results in rounding
         * errors when we try to go from quot -> baud !!
         * Try to make this work for both the fractional divider
         * and the simple divider. Also try to avoid rounding
         * errors using integer math.
         */
        *baud = (port->uartclk * fdiv) / (frac * div * 16 * quot);
        if (*baud > 1100 && *baud < 2300)
            *baud = 1200;
        if (*baud > 2300 && *baud < 4600)
            *baud = 2400;
        if (*baud > 4600 && *baud < 9300)
            *baud = 4800;
        if (*baud > 9300 && *baud < 18600)
            *baud = 9600;
        if (*baud > 18600 && *baud < 37200)
            *baud = 19200;
        if (*baud > 37200 && *baud < 55800)
            *baud = 38400;
        if (*baud > 55800 && *baud < 111700)
            *baud = 57600;
        if (*baud > 111700 && *baud < 223400)
            *baud = 115200;
    }
}

static int __init ifx_asc_console_setup(struct console *co, char *options)
{
    int baud = CONFIG_IFX_ASC_DEFAULT_BAUDRATE;
    int bits = 8;
    int parity = 'n';
    int flow = 'n';
    int i;

    /* this assumes: CON.BRS = CON.FDE = 0 */

    /*
     * Check whether an invalid uart number has been specified, and
     * if so, search for the first available port that does have
     * console support.
     */
    g_asc_console_port = uart_get_console(ifx_asc_port, UART_NR, co);
    for ( i = 0; i < NUM_ENTITY(ifx_asc_port); i++ )
        if ( g_asc_console_port == &ifx_asc_port[i] ) {
            /* start clock generator and receiver */
            ifx_asc_port_priv[i].base->asc_mcon |= ASCMCON_R;
            ifx_asc_port_priv[i].base->asc_whbstate = ASCWHBSTATE_SETREN;
        }

    if ( options )
        uart_parse_options(options, &baud, &parity, &bits, &flow);
    else
        ifx_asc_console_get_options(g_asc_console_port, &baud, &parity, &bits);

#if defined(CONFIG_USE_EMULATOR)
    if ( options )
        prom_printf("%s: options = %s, baud = %d, parity = %d (%c), bits = %d, flow = %d (%c)\n", __func__, options, baud, parity, (char)parity, bits, flow, (char)flow);
    else
        prom_printf("%s: options = NULL, baud = %d, parity = %d (%c), bits = %d, flow = %d (%c)\n", __func__, baud, parity, (char)parity, bits, flow, (char)flow);
#endif

    return uart_set_options(g_asc_console_port, co, baud, parity, bits, flow);
}

static int __init ifx_asc_console_init(void)
{
    ifx_asc_port_init();
    ifx_asc_init_hardware();

    register_console(&ifx_asc_console);

    return 0;
}

console_initcall(ifx_asc_console_init);
#endif /* CONFIG_SERIAL_IFX_ASC_CONSOLE */

static inline void proc_file_create(void)
{
    g_asc_dir = proc_mkdir("driver/ifx_asc", NULL);

    create_proc_read_entry("version",
                            0,
                            g_asc_dir,
                            proc_read_version,
                            NULL);

    //  TODO: proc to read rx/tx counters
}

static inline void proc_file_delete(void)
{
    remove_proc_entry("version", g_asc_dir);

    remove_proc_entry("driver/ifx_asc", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_asc_version(buf + len);

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;
    return len;
}

static inline int ifx_asc_version(char *buf)
{
    return ifx_drv_ver(buf, "ASC (UART)", IFX_ASC_VER_MAJOR, IFX_ASC_VER_MIDDLE, IFX_ASC_VER_MINOR);
}

#ifdef CONFIG_IFX_PMCU

static IFX_PMCU_RETURN_t  ifx_uart_pwm_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    unsigned long flags;
    unsigned int c0_count;
    unsigned int timeout = ifx_get_cpu_hz() / 2 / 100;
    int i;

    for ( i = 0; i < UART_NR; i++ ) {
        spin_lock_irqsave(&ifx_asc_port_priv[i].lock, flags);
        if ( g_clock_changing[i]++ == 0 ) {
            g_console_buf_count[i] = 0;
            g_console_buf[i] = (char *)__get_free_page(GFP_ATOMIC);

            c0_count = read_c0_count();
            while ( (ifx_asc_port_priv[i].base->asc_fstat & ASCFSTAT_TXFFLMASK) != 0 && read_c0_count() - c0_count < timeout );
            ifx_asc_port_priv[i].base->asc_txfcon |= ASCTXFCON_TXFFLU;
        }
        spin_unlock_irqrestore(&ifx_asc_port_priv[i].lock, flags);
        if ( (ifx_asc_port_priv[i].base->asc_fstat & ASCFSTAT_TXFFLMASK) != 0 )
            printk("ASC (pre): ifx_asc_port_priv[i].base->asc_fstat = 0x%08lx\n", ifx_asc_port_priv[i].base->asc_fstat);
    }

    return IFX_PMCU_RETURN_SUCCESS;
}

static IFX_PMCU_RETURN_t ifx_uart_pwm_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    unsigned long flags;
    int fifocnt;
    ifx_asc_port_priv_t *priv;
    ifx_asc_reg_t *asc_reg;
    int i, j;

    for ( i = 0; i < UART_NR; i++ ) {
        priv = &ifx_asc_port_priv[i];
        spin_lock_irqsave(&priv->lock, flags);
        if ( --g_clock_changing[i] == 0 ) {
            if ( g_console_buf[i] ) {
                asc_reg = priv->base;
                asc_reg->asc_txfcon |= ASCTXFCON_TXFFLU;
                for ( j = 0; j < g_console_buf_count[i]; j++ ) {
                    do {
                        fifocnt = asc_reg->asc_fstat & ASCFSTAT_TXFFLMASK;
                    } while ( fifocnt == (IFX_ASC_TXFIFO_FULL << ASCFSTAT_TXFFLOFF) );

                    /* We have either portwidth = 8 or portwidth = 32 */
                    if ( priv->portwidth == 8 ) {
                        asc_reg->asc_tbuf = g_console_buf[i][j];
                    }
                    else {
                        *(((char*)&asc_reg->asc_tbuf) + 3) = g_console_buf[i][j];
                    }
                    asm("sync");
                }
                free_page((unsigned long)g_console_buf[i]);
                g_console_buf[i] = NULL;
            }
        }
        spin_unlock_irqrestore(&priv->lock, flags);
        ifx_asc_start_tx(&ifx_asc_port[i]);
    }

    return IFX_PMCU_RETURN_SUCCESS;
}

static IFX_PMCU_RETURN_t ifx_uart_pwm_state_set(IFX_PMCU_STATE_t pmcuState)
{
    g_uart_pwm_state = pmcuState;
    return IFX_PMCU_RETURN_SUCCESS;
}

static IFX_PMCU_RETURN_t ifx_uart_pwm_state_get(IFX_PMCU_STATE_t *pmcuState)
{
    *pmcuState = g_uart_pwm_state;
    return IFX_PMCU_RETURN_SUCCESS;
}

static void ifx_asc_pwm_init(void)
{
    IFX_PMCU_REGISTER_t pmcuRegister = {0};

    pmcuRegister.pmcuModule    = IFX_PMCU_MODULE_UART;
    pmcuRegister.pmcuModuleNr  = 0;
    pmcuRegister.pmcuModuleDep = NULL;

    pmcuRegister.pre                   = ifx_uart_pwm_prechange;
    pmcuRegister.ifx_pmcu_state_change = ifx_uart_pwm_state_set;
    pmcuRegister.post                  = ifx_uart_pwm_postchange;
    pmcuRegister.ifx_pmcu_state_get    = ifx_uart_pwm_state_get;

    if ( ifx_pmcu_register(&pmcuRegister) != IFX_PMCU_RETURN_SUCCESS ) {
        printk(KERN_ERR "Fail in registering ASC (UART) to PMCU\n");
    }

    printk(KERN_INFO "Register ASC (UART) to PMCU.\n");
}

static void ifx_asc_pwm_exit(void)
{
    IFX_PMCU_REGISTER_t pmcuRegister = {0};

    pmcuRegister.pmcuModule    = IFX_PMCU_MODULE_UART;
    pmcuRegister.pmcuModuleNr  = 0;
    ifx_pmcu_unregister(&pmcuRegister);

    printk(KERN_INFO "Unregister ASC (UART) from PMCU.\n");
}

#endif

void ifx_update_asc_clock_settings(void)
{
    ifx_asc_reg_t *asc_reg;
    u32 fdv, reload;
    u32 asc_state;
    int i;

    for ( i = 0; i < NUM_ENTITY(ifx_asc_port); i++ ) {
        asc_reg = ifx_asc_port_priv[i].base;

        get_fdv_and_reload_value(ifx_asc_port_priv[i].baudrate, &fdv, &reload);

        asc_state = asc_reg->asc_state;
        /* disable receiver */
        asc_reg->asc_whbstate = ASCWHBSTATE_CLRREN;
        /* now we can write the new baudrate into the register */
        asc_reg->asc_fdv = fdv;
        asc_reg->asc_bg  = reload;
        /* enable receiver if necessary */
        if ( (asc_state & ASCSTATE_REN) )
            asc_reg->asc_whbstate = ASCWHBSTATE_SETREN;
    }
}
EXPORT_SYMBOL(ifx_update_asc_clock_settings);

static int __init ifx_asc_init(void)
{
    int ret = 0;
    char ver_str[256];
    int i;

    ifx_asc_port_init();
    ifx_asc_init_hardware();

    ret = uart_register_driver(&ifx_asc_drv);
    if ( ret == 0 ) {
        for ( i = 0; i < UART_NR; i ++ )
            uart_add_one_port(&ifx_asc_drv, &ifx_asc_port[i]);
    }

#ifdef CONFIG_IFX_PMCU
    ifx_asc_pwm_init();
#endif

    proc_file_create();

    ifx_asc_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return ret;
}

static void __exit ifx_asc_exit(void)
{
    proc_file_delete();

#ifdef CONFIG_IFX_PMCU
    ifx_asc_pwm_exit();
#endif

    uart_unregister_driver(&ifx_asc_drv);

#ifndef CONFIG_IFX_DISABLE_ASC0
    ifx_gpio_deregister(IFX_GPIO_MODULE_ASC0);
#endif
}

module_init(ifx_asc_init);
module_exit(ifx_asc_exit);

MODULE_AUTHOR("Xu Liang");
MODULE_DESCRIPTION("IFX ASC serial port driver");
MODULE_LICENSE("GPL");
