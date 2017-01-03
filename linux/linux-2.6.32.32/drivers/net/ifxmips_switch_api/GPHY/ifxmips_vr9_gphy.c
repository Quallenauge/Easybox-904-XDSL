/****************************************************************************
                               Copyright  2009
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************

   \file ifxmips_vr9_gphy.c
   \remarks implement GPHY driver on ltq platform
 *****************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/moduleparam.h>
#include <linux/types.h>  /* size_t */
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <asm/delay.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
//#include "ifx_ethsw_core_platform.h"
#include <asm/ifx/ifx_rcu.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include <ifx_ethsw_core_platform.h>
#if defined(CONFIG_VR9)
//	#include <ifx_ethsw_core_platform.h>
#if defined(CONFIG_GE_MODE)
	#include "gphy_fw_ge.h"
#endif
#if defined(CONFIG_FE_MODE)
	#include "gphy_fw_fe.h"
#endif
#endif /*CONFIG_VR9*/

#if defined(CONFIG_GE_MODE)
	#define IFX_GPHY_DEFAULT_FW             1
	#define IFX_GPHY_MODE                   "GE Mode"
#endif /*CONFIG_GE_MODE*/

#if defined(CONFIG_FE_MODE)
	#define IFX_GPHY_DEFAULT_FW             0
	#define IFX_GPHY_MODE                   "FE Mode"
#endif /*CONFIG_FE_MODE*/

#define IFX_DRV_MODULE_NAME             "ifxmips_vr9_gphy"
#define IFX_DRV_MODULE_VERSION          "0.9"
#define IFX_GPHY_INITCLK                "25MHz"
#define PROC_DATA_LENGTH                12

unsigned char  GSWIP_PHYID[15] = { 0};
static char version[]   __devinitdata = IFX_DRV_MODULE_NAME ": V" IFX_DRV_MODULE_VERSION "";
static char ver[]       __devinitdata = ":v" IFX_DRV_MODULE_VERSION "";
static char GPHY_CLK[]  __devinitdata = IFX_GPHY_INITCLK;

static char gphy_fw_dma[GPHY_FW_LEN_D];

struct proc_data_t {
   char name[PROC_DATA_LENGTH + 1];
   char gphyNum[PROC_DATA_LENGTH + 1];
   char value[PROC_DATA_LENGTH + 1];
};

struct proc_data_t  mdio_data_0, mdio_data_1, mdio_data_2, mdio_data_3, mode_0, mode_1;
struct proc_data_t  reset_data_0, power_data_0, reset_data_1, power_data_1;
static struct proc_dir_entry*   ifx_gphy_dir=NULL;
static struct proc_dir_entry*   ifx_gphy_0=NULL;
static struct proc_dir_entry*   ifx_gphy_1=NULL;
static IFX_uint32_t chip_id, dev_id; 

void inline ifxmips_mdelay( int delay){
	int i;
	for ( i=delay; i > 0; i--)
		udelay(1000);
}

int vr9_gphy_pmu_set(void)
{
	/* Config GPIO3 clock */
	SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
	GPHY_PMU_SETUP(IFX_PMU_ENABLE);
	return 0;
}

void ltq_write_mdio(unsigned int phyAddr, unsigned int regAddr,unsigned int data )
{
    unsigned int reg;
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }
    reg = MDIO_READ_WDATA(data);
    SW_WRITE_REG32( reg, MDIO_WRITE_REG);
    reg = ( MDIO_CTRL_OP_WR | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;
    SW_WRITE_REG32( reg, MDIO_CTRL_REG);
    udelay(100);
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
        udelay(10);
    }
    ifxmips_mdelay(10);
    return ;
}

unsigned short ltq_read_mdio(unsigned int phyAddr, unsigned int regAddr )
{
	unsigned int reg;
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }
	reg = ( MDIO_CTRL_OP_RD | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;
    SW_WRITE_REG32( reg, MDIO_CTRL_REG);
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }
    ifxmips_mdelay(10);
    reg = SW_READ_REG32(MDIO_READ_REG);
    return (MDIO_READ_RDATA(reg));
}

int ltq_gphy_reset(int phy_num)
{
	unsigned int reg;
	reg = ifx_rcu_rst_req_read();
	if ( phy_num == 2 )
		reg |= ( 1 << 31 ) | (1 << 29);
	else if ( phy_num == 0 )
		reg |= (1 << 31);
	else if ( phy_num == 1 )
		reg |= (1 << 29);
	else
		return 1;
	ifx_rcu_rst_req_write(reg, reg);
	return 0;
}
 
int ltq_gphy_reset_released(int phy_num)
{
	unsigned int mask = 0;
	if (phy_num == 2)
		mask = ( 1 << 31 ) | (1 << 29);
	else if ( phy_num == 0 )
		mask = ( 1 << 31 );
	else if ( phy_num == 1 )
		mask = ( 1 << 29 );
	else
		return 1;
	ifx_rcu_rst_req_write(0, mask);
	return 0;
}


/** Driver version info */
static inline int gphy_drv_ver(char *buf)
{
	return sprintf(buf, "IFX GPHY driver %s, version %s - Firmware: %x\n", IFX_GPHY_MODE, version, ltq_read_mdio(0x11, 0x1e));
}

/** Driver version info for procfs */
static inline int gphy_ver(char *buf)
{
   return sprintf(buf, "IFX GPHY driver, version %s Firmware: %x\n", ver,  ltq_read_mdio(0x11, 0x1e));
}

/** Displays the MODE select status of GPHY module via proc file */
static int gphy_proc_reset_gphy1_write(struct file *file, const IFX_char_t *buf,
                                    unsigned long count, void *data)
{
	int buffer_size = 0;
	unsigned int phy;
	struct proc_data_t *reset_data = (struct proc_data_t *)data;

	buffer_size = count;
	if (buffer_size > PROC_DATA_LENGTH)
		buffer_size = PROC_DATA_LENGTH;
	if (copy_from_user(reset_data->value, buf, buffer_size)) {
		return IFX_ERROR;
	}
	reset_data->value[buffer_size] = '\0';
	if (reset_data->value[0] == '1') {
		phy = 1;
		/* RESET GPHY */
		ltq_gphy_reset(phy);
		/* Wait 2ms */
		ifxmips_mdelay(200);
		/* RESET GPHY */
		ltq_gphy_reset_released(phy);
	}
	reset_data->value[0] = 0;
	return buffer_size;
}

/** Displays the MODE select status of GPHY module via proc file */
static int gphy_proc_reset_gphy0_write(struct file *file, const IFX_char_t *buf,
                                    unsigned long count, void *data)
{
	int buffer_size = 0;
	unsigned int phy;
	struct proc_data_t *reset_data = (struct proc_data_t *)data;

	buffer_size = count;
	if (buffer_size > PROC_DATA_LENGTH)
		buffer_size = PROC_DATA_LENGTH;
	if (copy_from_user(reset_data->value, buf, buffer_size)) {
		return IFX_ERROR;
	}
	reset_data->value[buffer_size] = '\0';
	if (reset_data->value[0] == '1') {
		phy = 0;
		/* RESET GPHY */
		ltq_gphy_reset(phy);
		/* Wait 2ms */
		ifxmips_mdelay(200);
		/* RESET GPHY */
		ltq_gphy_reset_released(phy);
	}
	reset_data->value[0] = 0;
	return buffer_size;
}

/** Displays the MODE select status of GPHY module via proc file */
static int gphy_proc_reset_read(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0;
	struct proc_data_t *reset_data = (struct proc_data_t *)data;
	len += sprintf(buf + len," %s\n", reset_data->value );
	/* No sanity check cos length is smaller than one page */
	*eof = 1;
	return len;
}


/** Turn the POWER of GPHY module via proc file */
static int gphy_proc_power_write(struct file *file, const IFX_char_t *buf,
                                    unsigned long count, void *data)
{
	int len = 0;
	unsigned int phy, reg = 0x0;
	unsigned short pdata;
	struct proc_data_t *pwr_data = (struct proc_data_t *)data;
	if (count > PROC_DATA_LENGTH)
		len = PROC_DATA_LENGTH;
	else
		len = count;
	if (copy_from_user(pwr_data->value, buf, len));
		return IFX_ERROR;
	if (pwr_data->value[0] == '0')
		pdata = 1;
	else
		pdata = 0;
	if (pwr_data->gphyNum[0] == '0')
		 phy = 0x11;
	else
		phy = 0x13;
	pdata = ltq_read_mdio(phy, reg);
	/* POWER DOWN */
	pdata |= (1 << 11);
	ltq_write_mdio(phy, reg, pdata);
	pwr_data->value[len] = '\0';
	return len;
}

/** Displays the POWER status of GPHY module via proc file */
static int gphy_proc_power_read(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
 	int len = 0;
	struct proc_data_t *pwr_data = (struct proc_data_t *)data;
	len = sprintf(buf," %s\n", pwr_data->value );
	/* No sanity check cos length is smaller than one page */
	*eof = 1;
	return len;
}

/** Displays the PHY address status of GPHY module via proc file */
static int gphy_proc_mdio_write(struct file *file, const IFX_char_t *buf,
                                    unsigned long count, void *data)
{
	int len = 0;
	return len;
}


/** Displays the PHY address status of GPHY module via proc file */
static int gphy_proc_mdio_read(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0;
	struct proc_data_t *mdio_data = (struct proc_data_t *)data;
	len = sprintf(buf," %s\n", mdio_data->value );
	/* No sanity check cos length is smaller than one page */
	*eof = 1;
	return len;
}

/** Displays the PHY address status of GPHY module via proc file */
static int gphy_proc_mode_write(struct file *file, const IFX_char_t *buf,
                                    unsigned long count, void *data)
{
	int len = 0;
	struct proc_data_t *mdio_data = (struct proc_data_t *)data;

	if (count > PROC_DATA_LENGTH)
		len = PROC_DATA_LENGTH;
	else
		len = count;

	if (copy_from_user(mdio_data->value, buf, len))
		return IFX_ERROR;

	if (mdio_data->value[0] == 'G') {
#if defined(CONFIG_GE_MODE)
		if (mdio_data->gphyNum[0] == '0') {
			if (dev_id == 0) {
				/* Load GPHY0 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_ge_fw_data), GFS_ADD0);
			} else {
				/* Load GPHY0 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_ge_fw_data_a12), GFS_ADD0);
			}
		} else {
			if (dev_id == 0) {
				/* Load GPHY1 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_ge_fw_data), GFS_ADD1);
			} else {
				/* Load GPHY1 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_ge_fw_data_a12), GFS_ADD1);
			}
		}
#endif
	} else {
#if defined(CONFIG_FE_MODE)
		if (mdio_data->gphyNum[0] == '0') {
			if (dev_id == 0) {
				/* Load GPHY0 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_fe_fw_data), GFS_ADD0);
			} else {
				/* Load GPHY0 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_fe_fw_data_a21), GFS_ADD0);
			}
		} else {
			if (dev_id == 0) {
				/* Load GPHY1 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_fe_fw_data), GFS_ADD1);
			} else {
				/* Load GPHY1 firmware module  */
				SW_WRITE_REG32( virt_to_phys(gphy_fe_fw_data_a21), GFS_ADD1);
			}
		}
#endif
	}
	mdio_data->value[len] = '\0';
	return len;
}


/** Displays the MODE select status of GPHY module via proc file */
static int gphy_proc_mode_read(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0;
	struct proc_data_t *mode_data = (struct proc_data_t *)data;
	len = sprintf(buf," %s\n", mode_data->value );
	/* No sanity check cos length is smaller than one page */
	*eof = 1;
	return len;
}

/** Displays the led status of GPHY module via proc file */
static int gphy_proc_led(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0, value = 0;
	/* No sanity check cos length is smaller than one page */
	len += sprintf(buf + len, "LED:%d\n", value);
	*eof = 1;
	return len;
}

/** Displays the clock status of GPHY module via proc file */
static int gphy_proc_clk(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0;
	/* No sanity check cos length is smaller than one page */
	len += sprintf(buf + len, "%s\n", GPHY_CLK);
	*eof = 1;
	return len;
}

/** Displays the version of GPHY module via proc file */
static int gphy_proc_version(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
	int len = 0;
	/* No sanity check cos length is smaller than one page */
	len += gphy_ver(buf + len);
	*eof = 1;
	return len;
}

/** create proc for debug  info, \used ifx_eth_module_init */
static int gphy_proc_create(void)
{
	/* procfs */
	ifx_gphy_dir = proc_mkdir ("driver/ifx_gphy", NULL);
	if (ifx_gphy_dir == NULL) {
		printk(KERN_ERR "%s: Create proc directory (/driver/ifx_gphy) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	create_proc_read_entry("version", 0, ifx_gphy_dir, gphy_proc_version,  NULL);
	create_proc_read_entry("clk", 0, ifx_gphy_dir, gphy_proc_clk,  NULL);
	create_proc_read_entry("led", 0, ifx_gphy_dir, gphy_proc_led,  NULL);
	/* gphy0 proc */
	ifx_gphy_dir = proc_mkdir ("driver/ifx_gphy/gphy0", NULL);
	if (ifx_gphy_dir == NULL) {
		printk(KERN_ERR "%s: Create proc directory (/driver/ifx_gphy/gphy0) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	ifx_gphy_0 = create_proc_entry("mode", 0, ifx_gphy_dir);
	if (ifx_gphy_0 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (mode) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(mode_0.name, "mode");
	strcpy(mode_0.value, "GE");
	strcpy(mode_0.gphyNum, "0");
	ifx_gphy_0->data = &mode_0;
	ifx_gphy_0->read_proc = gphy_proc_mode_read;
	ifx_gphy_0->write_proc = gphy_proc_mode_write;
	ifx_gphy_0 = create_proc_entry("phy0_mdio", 0, ifx_gphy_dir);
	if (ifx_gphy_0 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (phy0_mdio) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(mdio_data_0.name, "phy0_mdio");
	strcpy(mdio_data_0.value, "0xB");
	strcpy(mdio_data_0.gphyNum, "0");
	ifx_gphy_0->data = &mdio_data_0;
	ifx_gphy_0->read_proc = gphy_proc_mdio_read;
	ifx_gphy_0->write_proc = gphy_proc_mdio_write;
	if (IFX_GPHY_DEFAULT_FW == 0) {
		ifx_gphy_0 = create_proc_entry("phy1_mdio", 0, ifx_gphy_dir);
		if (ifx_gphy_0 == NULL) {
			printk(KERN_ERR "%s: Create proc directory (phy1_mdio) failed!!!\n", __func__);
			return IFX_ERROR;
		}
		strcpy(mdio_data_1.name, "phy1_mdio");
		strcpy(mdio_data_1.value, "0xC");
		strcpy(mdio_data_0.gphyNum, "0");
		ifx_gphy_0->data = &mdio_data_1;
		ifx_gphy_0->read_proc = gphy_proc_mdio_read;
		ifx_gphy_0->write_proc = gphy_proc_mdio_write;
	}
	ifx_gphy_0 = create_proc_entry("reset", 0, ifx_gphy_dir);
	if (ifx_gphy_0 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (reset) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(reset_data_0.name, "reset");
	strcpy(reset_data_0.value, "0");
	strcpy(mdio_data_0.gphyNum, "0");
	ifx_gphy_0->data = &reset_data_0;
	ifx_gphy_0->read_proc = gphy_proc_reset_read;
	ifx_gphy_0->write_proc = gphy_proc_reset_gphy0_write;

	ifx_gphy_0 = create_proc_entry("power", 0, ifx_gphy_dir);
	if (ifx_gphy_0 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (power) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(power_data_0.name, "power");
	strcpy(power_data_0.value, "1");
	strcpy(mdio_data_0.gphyNum, "0");
	ifx_gphy_0->data = &power_data_0;
	ifx_gphy_0->read_proc = gphy_proc_power_read;
	ifx_gphy_0->write_proc = gphy_proc_power_write;

	/* gphy1 proc */
	ifx_gphy_dir = proc_mkdir ("driver/ifx_gphy/gphy1", NULL);
	if (ifx_gphy_dir == NULL) {
		printk(KERN_ERR "%s: Create proc directory (/driver/ifx_gphy/gphy1) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	ifx_gphy_1 = create_proc_entry("mode", 0, ifx_gphy_dir);
	if (ifx_gphy_1 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (mode) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(mode_1.name, "mode");
	strcpy(mode_1.value, "GE");
	strcpy(mode_1.gphyNum, "1");
	ifx_gphy_1->data = &mode_1;
	ifx_gphy_1->read_proc = gphy_proc_mode_read;
	ifx_gphy_1->write_proc = gphy_proc_mode_write;

	ifx_gphy_1 = create_proc_entry("phy0_mdio", 0, ifx_gphy_dir);
	if (ifx_gphy_1 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (phy0_mdio) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(mdio_data_2.name, "phy0_mdio");
	strcpy(mdio_data_2.value, "0xD");
	strcpy(mdio_data_0.gphyNum, "1");
	ifx_gphy_1->data = &mdio_data_2;
	ifx_gphy_1->read_proc = gphy_proc_mdio_read;
	ifx_gphy_1->write_proc = gphy_proc_mdio_write;
	if (IFX_GPHY_DEFAULT_FW == 0) {
		ifx_gphy_1 = create_proc_entry("phy1_mdio", 0, ifx_gphy_dir);
		if (ifx_gphy_1 == NULL) {
			printk(KERN_ERR "%s: Create proc directory (phy1_mdio) failed!!!\n", __func__);
			return IFX_ERROR;
		}
		strcpy(mdio_data_3.name, "phy1_mdio");
		strcpy(mdio_data_3.value, "0xE");
		strcpy(mdio_data_0.gphyNum, "1");
		ifx_gphy_1->data = &mdio_data_3;
		ifx_gphy_1->read_proc = gphy_proc_mdio_read;
		ifx_gphy_1->write_proc = gphy_proc_mdio_write;
	}
	ifx_gphy_1 = create_proc_entry("reset", 0, ifx_gphy_dir);
	if (ifx_gphy_1 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (reset) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(reset_data_1.name, "reset");
	strcpy(reset_data_1.value, "0");
	strcpy(mdio_data_0.gphyNum, "1");
	ifx_gphy_1->data = &reset_data_1;
	ifx_gphy_1->read_proc = gphy_proc_reset_read;
	ifx_gphy_1->write_proc = gphy_proc_reset_gphy1_write;

	ifx_gphy_1 = create_proc_entry("power", 0, ifx_gphy_dir);
	if (ifx_gphy_0 == NULL) {
		printk(KERN_ERR "%s: Create proc directory (power) failed!!!\n", __func__);
		return IFX_ERROR;
	}
	strcpy(power_data_1.name, "power");
	strcpy(power_data_1.value, "1");
	strcpy(mdio_data_0.gphyNum, "1");
	ifx_gphy_1->data = &power_data_1;
	ifx_gphy_1->read_proc = gphy_proc_power_read;
	ifx_gphy_1->write_proc = gphy_proc_power_write;

	return IFX_SUCCESS;
}

/** remove of the proc entries, \used ifx_eth_module_exit */
static void gphy_proc_delete(void)
{
#if 0
	remove_proc_entry("version", gphy_drv_ver);
	remove_proc_entry("clk", gphy_drv_ver);
	remove_proc_entry("led", gphy_drv_ver);

	remove_proc_entry("mode", gphy_drv_ver);
	remove_proc_entry("phy0_mdio", gphy_drv_ver);
	remove_proc_entry("phy1_mdio", gphy_drv_ver);
	remove_proc_entry("gphy1_reset", gphy_drv_ver);
	remove_proc_entry("gphy1_power", gphy_drv_ver);
	remove_proc_entry("driver/ifx_gphy/gphy0",  NULL);
	remove_proc_entry("driver/ifx_gphy/gphy1",  NULL);
	remove_proc_entry("driver/ifx_gphy",  NULL);
#endif
}

int ltq_gphy_firmware_load(void)
{
	int i;
	unsigned int value, value2;
	char *ptr;

	value = (u32)gphy_fw_dma;
	value2 = (value & 0xFFFF);
	value2 = 65536 - value2;
	memset(gphy_fw_dma, 0, sizeof(gphy_fw_dma));
	ptr = gphy_fw_dma;
	/** just backup when want to cleanup the module */
	ptr += value2;
	for(i=0;i<65536;i++) {
#if defined(CONFIG_FE_MODE)
		if (dev_id == 0) {
			*(ptr+i) = gphy_fe_fw_data[i];
		} else {
			*(ptr+i) = gphy_fe_fw_data_a21[i];
		}
#endif
#if defined(CONFIG_GE_MODE)
		if (dev_id == 0 ) {
			*(ptr+i) = gphy_ge_fw_data[i];
		} else {
			*(ptr+i) = gphy_ge_fw_data_a12[i];
		}
#endif
	}
	value = (u32)gphy_fw_dma;
	value = value & 0xFFFF0000;
	value = value + 0x10000;
	value = value & 0xFFFFFF;
	printk("GPHY FIRMWARE LOAD SUCCESSFULLY AT ADDR : %x\n",value);
	/* Load GPHY0 firmware module  */
	SW_WRITE_REG32( value, GFS_ADD0);
	/* Load GPHY1 firmware module  */
	SW_WRITE_REG32( value, GFS_ADD1);
	return 0;
}
/* Workaround for Clean-on-Read to external ports*/
void gphy_COR_configure(int phy_addr)
{
	unsigned short mdio_read_value;
	
	ltq_write_mdio(phy_addr, 0xd, 0x1F);
	ltq_write_mdio(phy_addr, 0xe, 0x1FF);
	ltq_write_mdio(phy_addr, 0xd, 0x401F);
	mdio_read_value =  ltq_read_mdio(0x0, 0xe);
	mdio_read_value |= 1;
	ltq_write_mdio(phy_addr, 0xd, 0x1F);
	ltq_write_mdio(phy_addr, 0xe, 0x1FF);
	ltq_write_mdio(phy_addr, 0xd, 0x401F);
	ltq_write_mdio(phy_addr, 0xe, mdio_read_value); 
}

 /* MDIO auto-polling for T2.12 version */
void gphy_MDIO_AutoPoll_configure(int phy_addr) 
{
	/* MDIO auto-polling for T2.12 version */
	ltq_write_mdio(phy_addr, 0xd, 0x1F);
	ltq_write_mdio(phy_addr, 0xe, 0x1FF);
	ifxmips_mdelay(10);
	ltq_write_mdio(phy_addr, 0xd, 0x401F);
	ifxmips_mdelay(10);
	ltq_write_mdio(phy_addr, 0xe, 0x1);
	ifxmips_mdelay(10);
	/* Enable STICKY functionality for internal GPHY */
	ltq_write_mdio(phy_addr, 0x14, 0x8106);
	/* Issue GPHY reset */
	ltq_write_mdio(phy_addr, 0x0, 0x9000);
	ifxmips_mdelay(30);
}

 /* Disable the MDIO interrupt for external GPHY */
void gphy_disable_MDIO_interrupt(int phy_addr) 
{
	ltq_write_mdio(0x0, 0xd, 0x1F);
	ifxmips_mdelay(10);
	ltq_write_mdio(0x0, 0xe, 0x0703);
	ifxmips_mdelay(10);
	ltq_write_mdio(0x0, 0xd, 0x401F);
	ifxmips_mdelay(10);
	ltq_write_mdio(0x0, 0xe, 0x0002);
	ifxmips_mdelay(10);
}

 /* preferred MASTER device */
void configure_gphy_master_mode(int phy_addr)
{
   ltq_write_mdio(0x0, 0x9, 0x700);
   ifxmips_mdelay(20);
}

/* Disabling of the Power-Consumption Scaling for external GPHY (Bit:2)*/
void disable_power_scaling_mode(int phy_addr)
{
	unsigned short value;
	value =  ltq_read_mdio(phy_addr, 0x14);
	value &= ~(1<<2);
	ltq_write_mdio(phy_addr, 0x14, value);
}

void ltq_phy_mmd_write(unsigned char phyaddr, unsigned int reg_addr, unsigned short data )
{
	unsigned int temp = SW_READ_REG32(MDC_CFG_0_REG);
	/* Disable Switch Auto Polling for all ports */
	SW_WRITE_REG32(0x0, MDC_CFG_0_REG);
	ltq_write_mdio(phyaddr, 0xd, 0x1F);
	ltq_write_mdio(phyaddr, 0xe, reg_addr);
	ltq_write_mdio(phyaddr, 0xd, 0x401F);
	ltq_write_mdio(phyaddr, 0xe, data);
	/* Enable Switch Auto Polling for all ports */
	SW_WRITE_REG32(temp, MDC_CFG_0_REG);
}
unsigned short ltq_phy_mmd_read(unsigned char phyaddr, unsigned int reg_addr )
{
	unsigned short data;
	unsigned int temp = SW_READ_REG32(MDC_CFG_0_REG);
	/* Disable Switch Auto Polling for all ports */
	SW_WRITE_REG32(0x0, MDC_CFG_0_REG);
	ltq_write_mdio(0x0, 0xd, 0x1F);
	ltq_write_mdio(phyaddr, 0xd, 0x1F);
	ltq_write_mdio(phyaddr, 0xe, reg_addr);
	ltq_write_mdio(phyaddr, 0xd, 0x401F);
	data = ltq_read_mdio(phyaddr, 0xe);
	/* Enable Switch Auto Polling for all ports */
	SW_WRITE_REG32(temp, MDC_CFG_0_REG);
	return data;
}

void ltq_gphy_led0_config (unsigned char phyaddr)
{
	/* for GE modes, For LED0    (SPEED/LINK INDICATION ONLY) */
	ltq_phy_mmd_write(phyaddr, 0x1e2, 0x42);
	ltq_phy_mmd_write(phyaddr, 0x1e3, 0x10);
}

void ltq_gphy_led1_config (unsigned char phyaddr)
{
	/* for GE modes, For LED1, DATA TRAFFIC INDICATION ONLY */
	ltq_phy_mmd_write(phyaddr, 0x1e4, 0x70);
	ltq_phy_mmd_write(phyaddr, 0x1e5, 0x03);
}

/**
Mode			Link-LED			Data-LED
Link-Down		OFF					OFF
10baseT			BLINK-SLOW			PULSE on TRAFFIC
100baseTX		BLINK-FAST			PULSE on TRAFFIC
1000baseT		ON					PULSE on TRAFFIC
**/
void ltq_gphy_led_config (void)
{
#if defined(CONFIG_GE_MODE)	
	ltq_gphy_led0_config(0);
	ltq_gphy_led1_config(0);
	ltq_gphy_led0_config(1);
	ltq_gphy_led1_config(1);
	ltq_gphy_led0_config(0x11);
	ltq_gphy_led1_config(0x11);
	ltq_gphy_led0_config(0x13);
	ltq_gphy_led1_config(0x13);
	ltq_gphy_led0_config(5);
	ltq_gphy_led1_config(5);
#endif
}

int ltq_Gphy_hw_Specify_init(void)
{
/* Workaround for Clean-on-Read to external ports*/
	gphy_COR_configure(0);
	gphy_COR_configure(1);
	gphy_COR_configure(5);
	
	/* MDIO auto-polling for T2.12 version */
	gphy_MDIO_AutoPoll_configure(0x11);
	gphy_MDIO_AutoPoll_configure(0x13);
	
	 /* Disable the MDIO interrupt for external GPHY */
	gphy_disable_MDIO_interrupt(0);
	gphy_disable_MDIO_interrupt(1);
	gphy_disable_MDIO_interrupt(5);

	/* preferred MASTER device */
	configure_gphy_master_mode(0);
	configure_gphy_master_mode(1);
	configure_gphy_master_mode(5);
	configure_gphy_master_mode(0x11);
	configure_gphy_master_mode(0x13);
   
    /* Disabling of the Power-Consumption Scaling for external GPHY */
	disable_power_scaling_mode(0x0);
	disable_power_scaling_mode(0x1);
	disable_power_scaling_mode(0x5);

	return 0;
}


/** Initilization GPHY module */
int ifx_phy_module_init (void)
{
	int gphy_numbers;
	IFX_uint32_t PHY_ID;
	char ver_str[128] = {0};

	gphy_numbers = 2;
	PHY_ID = GSWIP_REG_ACCESS(MPS_CHIPID);
	chip_id = ( (PHY_ID & 0xFFF000) >> 12);
	/* Disable Switch Auto Polling for all ports */
	SW_WRITE_REG32(0x0, MDC_CFG_0_REG);
	printk("Switch Auto Polling value = %x\n", SW_READ_REG32(MDC_CFG_0_REG));
	ifxmips_mdelay(10);

#if defined(CONFIG_VR9)
  
	switch ( ( chip_id )) {
		case 0x1c0:
		case 0x1c1:
		case 0x1c2:
		case 0x1c8:
		case 0x1c9:
			dev_id = 0;
			printk(KERN_ERR "GPHY FW load for A1x, chip id %x!!\n", chip_id);
			break;
		case 0x0b:
		default:
			printk(KERN_ERR "GPHY FW load for A2x, chip id %x!!\n", chip_id);
			dev_id = 1;
	}
	
	/* RESET GPHY */
	if(ltq_gphy_reset(gphy_numbers) == 1)
		printk(KERN_ERR "GPHY driver init RESET FAILED !!\n");
	/* Load GPHY firmware */
	ltq_gphy_firmware_load();
	/* Wait 1s */
	ifxmips_mdelay(100);
	/* RESET GPHY */
	if(ltq_gphy_reset_released(gphy_numbers) == 1)
		printk(KERN_ERR "GPHY driver init RELEASE FAILED !!\n");
	if (dev_id == 0) {
		if(ltq_Gphy_hw_Specify_init() == 1)
			printk(KERN_ERR "GPHY driver Specify init FAILED !!\n");
	}

  #if 1 /* ctc, power down at bootup */
	ltq_write_mdio( 0x00, 0x0, ltq_read_mdio( 0x00, 0x0 ) | 0x0800 );
	ltq_write_mdio( 0x01, 0x0, ltq_read_mdio( 0x01, 0x0 ) | 0x0800 );
	ltq_write_mdio( 0x11, 0x0, ltq_read_mdio( 0x11, 0x0 ) | 0x0800 );
	ltq_write_mdio( 0x12, 0x0, ltq_read_mdio( 0x12, 0x0 ) | 0x0800 );
	ltq_write_mdio( 0x13, 0x0, ltq_read_mdio( 0x13, 0x0 ) | 0x0800 );
	ltq_write_mdio( 0x14, 0x0, ltq_read_mdio( 0x14, 0x0 ) | 0x0800 );
  #endif

	ltq_gphy_led_config();
	/* Enable Switch Auto Polling for all ports */
	SW_WRITE_REG32(0x3f, MDC_CFG_0_REG);
	ifxmips_mdelay(10);

#endif /* CONFIG_VR9*/
	/* Create proc entry */
	gphy_proc_create();
	/* Print the driver version info */
	gphy_drv_ver(ver_str);
	printk(KERN_INFO "%s", ver_str);
	return  0;
}


void  ifx_phy_module_exit (void)
{
	gphy_proc_delete();
}

module_init(ifx_phy_module_init);
module_exit(ifx_phy_module_exit);

MODULE_AUTHOR("Sammy Wu");
MODULE_DESCRIPTION("IFX GPHY driver (Supported LTQ devices)");
MODULE_LICENSE("GPL");
MODULE_VERSION(IFX_DRV_MODULE_VERSION);
