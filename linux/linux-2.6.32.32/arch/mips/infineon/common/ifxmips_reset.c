/*
 *
 * Gary Jennejohn <gj@denx.de>
 * Copyright (C) 2003 Gary Jennejohn
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Reset the VR9 reference board.
 *
 */



#include <linux/kernel.h>
#include <linux/pm.h>
#include <asm/reboot.h>
#include <asm/system.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>


static void ifx_machine_restart(char *command)
{
    local_irq_disable();

#if defined(CONFIG_AR9)
    *IFX_RCU_PPE_CONF &= ~(3 << 30);   //  workaround for AFE (enable_afe) abnormal behavior
#endif
    *IFX_RCU_RST_REQ = IFX_RCU_RST_REQ_ALL;
    for (;;) {
        ; /* Do nothing */
    }
}

static void ifx_machine_halt(void)
{
    /* Disable interrupts and loop forever */
    printk(KERN_NOTICE "System halted.\n");
    local_irq_disable();
    for (;;) {
        ; /* Do nothing */
    }
}

static void ifx_machine_power_off(void)
{
    /* We can't power off without the user's assistance */
    printk(KERN_NOTICE "Please turn off the power now.\n");
    local_irq_disable();
    for (;;) {
        ; /* Do nothing */
    }
}

void
ifx_reboot_setup(void)
{
    _machine_restart = ifx_machine_restart;
    _machine_halt = ifx_machine_halt;
    pm_power_off = ifx_machine_power_off;
}

