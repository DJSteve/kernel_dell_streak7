/*
 * arch/arm/mach-tegra/board-ventana.c
 *
 * Copyright (c) 2010 - 2011, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/serial_8250.h>
#include <linux/i2c.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/i2c-tegra.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/platform_data/tegra_usb.h>
#include <linux/usb/android_composite.h>
#include <linux/usb/f_accessory.h>
#include <linux/mfd/tps6586x.h>
#include <linux/memblock.h>

#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/rtc.h>

#ifdef CONFIG_TOUCHSCREEN_PANJIT_I2C
#include <linux/i2c/panjit_ts.h>
#endif

#ifdef CONFIG_TOUCHSCREEN_ATMEL_mXT224
#include <mach/atmel_mXT224_touch_luna.h>
#endif


#ifdef CONFIG_ATA2538_CAPKEY
#include <mach/ATA2538_capkey.h>
#endif


#ifdef CONFIG_TEGRA_KEYPAD
#include <mach/tegra_keypad.h>
#endif

#ifdef CONFIG_TOUCHSCREEN_ATMEL_MT_T9
#include <linux/i2c/atmel_maxtouch.h>
#endif

#include <sound/wm8903.h>

#include <mach/clk.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/pinmux.h>
#include <mach/iomap.h>
#include <mach/io.h>
#include <mach/i2s.h>
#include <mach/spdif.h>
#include <mach/audio.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/usb_phy.h>
#include <mach/tegra_das.h>

#include "board.h"
#include "clock.h"
#include "board-ventana.h"
#include "devices.h"
#include "gpio-names.h"
#include "fuse.h"
#include "wakeups-t2.h"


extern int console_set_none;

static struct usb_mass_storage_platform_data tegra_usb_fsg_platform = {
	.vendor = "Dell", 
	.product = "Streak7",
	.nluns = 2,
};

static struct platform_device tegra_usb_fsg_device = {
	.name = "usb_mass_storage",
	.id = -1,
	.dev = {
		.platform_data = &tegra_usb_fsg_platform,
	},
};

static struct plat_serial8250_port debug_uart_platform_data[] = {
	{
		.membase	= IO_ADDRESS(TEGRA_UARTA_BASE),
		.mapbase	= TEGRA_UARTA_BASE,
		.irq		= INT_UARTA,
		.flags		= UPF_BOOT_AUTOCONF,
		
		
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= 216000000,
	}, {
		.flags		= 0,
	}
};

static struct platform_device debug_uart = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev = {
		.platform_data = debug_uart_platform_data,
	},
};

static struct tegra_audio_platform_data tegra_spdif_pdata = {
	.dma_on = true,  /* use dma by default */
	.spdif_clk_rate = 5644800,
};

static struct tegra_utmip_config utmi_phy_config[] = {
	[0] = {
			.hssync_start_delay = 9,
			.idle_wait_delay = 17,
			.elastic_limit = 16,
			.term_range_adj = 6,
			.xcvr_setup = 15,
			.xcvr_lsfslew = 2,
			.xcvr_lsrslew = 2,
	},
	[1] = {
			.hssync_start_delay = 9,
			.idle_wait_delay = 17,
			.elastic_limit = 16,
			.term_range_adj = 6,
			.xcvr_setup = 8,
			.xcvr_lsfslew = 2,
			.xcvr_lsrslew = 2,
	},
};

static struct tegra_ulpi_config ulpi_phy_config = {
	.reset_gpio = TEGRA_GPIO_PG2,
	.clk = "clk_dev2",
	.inf_type = TEGRA_USB_LINK_ULPI,
};

#ifdef CONFIG_BCM4329_RFKILL

static struct resource ventana_bcm4329_rfkill_resources[] = {
	{
		.name   = "bcm4329_nreset_gpio",
		.start  = TEGRA_GPIO_PU2,
		.end    = TEGRA_GPIO_PU2,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "bcm4329_nshutdown_gpio",
		.start  = TEGRA_GPIO_PX0,
		.end    = TEGRA_GPIO_PX0,
		.flags  = IORESOURCE_IO,
	},
	{
		.name   = "bcm4329_nwakepin_gpio",
		.start  = TEGRA_GPIO_PU3,
		.end    = TEGRA_GPIO_PU3,
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device ventana_bcm4329_rfkill_device = {
	.name = "bcm4329_rfkill",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(ventana_bcm4329_rfkill_resources),
	.resource       = ventana_bcm4329_rfkill_resources,
};

static noinline void __init ventana_bt_rfkill(void)
{
	/*Add Clock Resource*/
	clk_add_alias("bcm4329_32k_clk", ventana_bcm4329_rfkill_device.name, \
				"blink", NULL);

	platform_device_register(&ventana_bcm4329_rfkill_device);

	return;
}
#else
static inline void ventana_bt_rfkill(void) { }
#endif

#ifdef CONFIG_BT_BLUESLEEP
static noinline void __init tegra_setup_bluesleep(void)
{
	struct platform_device *pdev = NULL;
	struct resource *res;

	pdev = platform_device_alloc("bluesleep", 0);
	if (!pdev) {
		pr_err("unable to allocate platform device for bluesleep");
		return;
	}

	res = kzalloc(sizeof(struct resource) * 3, GFP_KERNEL);
	if (!res) {
		pr_err("unable to allocate resource for bluesleep\n");
		goto err_free_dev;
	}

	res[0].name   = "gpio_host_wake";
	res[0].start  = TEGRA_GPIO_PU6;
	res[0].end    = TEGRA_GPIO_PU6;
	res[0].flags  = IORESOURCE_IO;

	res[1].name   = "gpio_ext_wake";
	res[1].start  = TEGRA_GPIO_PU3;
	res[1].end    = TEGRA_GPIO_PU3;
	res[1].flags  = IORESOURCE_IO;

	res[2].name   = "host_wake";
	res[2].start  = gpio_to_irq(TEGRA_GPIO_PU6);
	res[2].end    = gpio_to_irq(TEGRA_GPIO_PU6);
	res[2].flags  = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWEDGE;

	if (platform_device_add_resources(pdev, res, 3)) {
		pr_err("unable to add resources to bluesleep device\n");
		goto err_free_res;
	}

	if (platform_device_add(pdev)) {
		pr_err("unable to add bluesleep device\n");
		goto err_free_res;
	}

	tegra_gpio_enable(TEGRA_GPIO_PU6);
	tegra_gpio_enable(TEGRA_GPIO_PU3);

	return;

err_free_res:
	kfree(res);
err_free_dev:
	platform_device_put(pdev);
	return;
}
#else
static inline void tegra_setup_bluesleep(void) { }
#endif

static __initdata struct tegra_clk_init_table ventana_clk_init_table[] = {
	/* name		parent		rate		enabled */
	{ "uartd",	"pll_p",	216000000,	true}, 
	{ "uarta",	"pll_p",	216000000,	true}, 
	{ "uartc",	"pll_m",	600000000,	false},
	{ "blink",	"clk_32k",	32768,		false},
	{ "pll_p_out4",	"pll_p",	24000000,	true },
	{ "pwm",	"clk_32k",	32768,		true},
	{ "pll_a",	NULL,		56448000,	false},
	{ "pll_a_out0",	NULL,		11289600,	false},
	{ "clk_dev1",	"pll_a_out0",	0,		true},
	{ "i2s1",	"pll_a_out0",	11289600,	true},
	{ "i2s2",	"pll_a_out0",	11289600,	false},
	{ "audio",	"pll_a_out0",	11289600,	true},
	{ "audio_2x",	"audio",	22579200,	false},
	{ "spdif_out",	"pll_a_out0",	5644800,	false},
	{ "kbc",	"clk_32k",	32768,		true},
	{ NULL,		NULL,		0,		0},
};

#define USB_MANUFACTURER_NAME		"Dell"
#define USB_PRODUCT_NAME		"Streak7"
#define USB_PRODUCT_ID_MTP_ADB		0x7100
#define USB_PRODUCT_ID_MTP		0x7102
#define USB_PRODUCT_ID_RNDIS		0x7103
#define USB_PRODUCT_ID_UMS_ADB		0xb104
#define USB_PRODUCT_ID_STREAK7_MTP_ADB		0xb10b
#define USB_PRODUCT_ID_RNDIS_ADB	0xb102
#define USB_VENDOR_ID			0x413c

#define USB_PRODUCT_DEFAULT_ID	USB_PRODUCT_ID_STREAK7_MTP_ADB



#ifdef CONFIG_USB_ANDROID_ACCESSORY
static char *usb_functions_accessory[] = { "accessory" };
static char *usb_functions_accessory_adb[] = { "accessory", "adb" };
#endif
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
static char *usb_functions_ums_adb[] = { "usb_mass_storage", "adb" };
#endif
#ifdef CONFIG_USB_ANDROID_MTP
static char *usb_functions_mtp_adb[] = { "mtp", "adb" };
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
static char *usb_functions_rndis[] = { "rndis" };
static char *usb_functions_rndis_adb[] = { "rndis", "adb" };
#endif
static char *usb_functions_all[] = {
#ifdef CONFIG_USB_ANDROID_RNDIS
	"rndis",
#endif
#ifdef CONFIG_USB_ANDROID_MTP
	"mtp",
#endif	
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
	"usb_mass_storage",
#endif
#ifdef CONFIG_USB_ANDROID_ADB	
	"adb",	
#endif	
#ifdef CONFIG_USB_ANDROID_ACCESSORY
	"accessory",
#endif
};

static struct android_usb_product usb_products[] = {
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE	
	{
		.product_id     = USB_PRODUCT_ID_UMS_ADB,
		.num_functions  = ARRAY_SIZE(usb_functions_ums_adb),
		.functions      = usb_functions_ums_adb,
	},
#endif	
#ifdef CONFIG_USB_ANDROID_MTP	
	{
		.product_id     = USB_PRODUCT_ID_STREAK7_MTP_ADB,
		.num_functions  = ARRAY_SIZE(usb_functions_mtp_adb),
		.functions      = usb_functions_mtp_adb,
	},
#endif	
/*	
	{
		.product_id     = USB_PRODUCT_ID_MTP,
		.num_functions  = ARRAY_SIZE(usb_functions_mtp_ums),
		.functions      = usb_functions_mtp_ums,
	},
	{
		.product_id     = USB_PRODUCT_ID_MTP_ADB,
		.num_functions  = ARRAY_SIZE(usb_functions_mtp_adb_ums),
		.functions      = usb_functions_mtp_adb_ums,
	},
*/	
#ifdef CONFIG_USB_ANDROID_RNDIS
	
	{
		.product_id     = USB_PRODUCT_ID_RNDIS_ADB,
		.num_functions  = ARRAY_SIZE(usb_functions_rndis_adb),
		.functions      = usb_functions_rndis_adb,
	},
#endif
#ifdef CONFIG_USB_ANDROID_ACCESSORY
	{
		.vendor_id      = USB_ACCESSORY_VENDOR_ID,
		.product_id     = USB_ACCESSORY_PRODUCT_ID,
		.num_functions  = ARRAY_SIZE(usb_functions_accessory),
		.functions      = usb_functions_accessory,
	},
	{
		.vendor_id      = USB_ACCESSORY_VENDOR_ID,
		.product_id     = USB_ACCESSORY_ADB_PRODUCT_ID,
		.num_functions  = ARRAY_SIZE(usb_functions_accessory_adb),
		.functions      = usb_functions_accessory_adb,
	},
#endif
};

/* standard android USB platform data */
static struct android_usb_platform_data andusb_plat = {
	.vendor_id              = USB_VENDOR_ID,
	.product_id             = USB_PRODUCT_DEFAULT_ID,
	.manufacturer_name      = USB_MANUFACTURER_NAME,
	.product_name           = USB_PRODUCT_NAME,
	.serial_number          = NULL,
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_all),
	.functions = usb_functions_all,
};

static struct platform_device androidusb_device = {
	.name   = "android_usb",
	.id     = -1,
	.dev    = {
		.platform_data  = &andusb_plat,
	},
};

#ifdef CONFIG_USB_ANDROID_RNDIS
static struct usb_ether_platform_data rndis_pdata = {
	.ethaddr = {0, 0, 0, 0, 0, 0},
	.vendorID = USB_VENDOR_ID,
	.vendorDescr = USB_MANUFACTURER_NAME,
};

static struct platform_device rndis_device = {
	.name   = "rndis",
	.id     = -1,
	.dev    = {
		.platform_data  = &rndis_pdata,
	},
};
#endif

static struct wm8903_platform_data wm8903_pdata = {
	.irq_active_low = 0,
	.micdet_cfg = 0x83,           /* enable mic bias current */
	.micdet_delay = 0,
	.gpio_base = WM8903_GPIO_BASE,
	.gpio_cfg = {
		WM8903_GPIO_NO_CONFIG,
		WM8903_GPIO_NO_CONFIG,
		0,                     /* as output pin */
		WM8903_GPn_FN_GPIO_MICBIAS_CURRENT_DETECT
		<< WM8903_GP4_FN_SHIFT, /* as micbias current detect */
		WM8903_GPIO_NO_CONFIG,
	},
};

static struct i2c_board_info __initdata ventana_i2c_bus1_board_info[] = {
	{
		I2C_BOARD_INFO("wm8903", 0x1a),
		.platform_data = &wm8903_pdata,
	},
};

static struct tegra_ulpi_config ventana_ehci2_ulpi_phy_config = {
	.reset_gpio = TEGRA_GPIO_PV1,
	.clk = "clk_dev2",
};
#if 0 
static struct tegra_ehci_platform_data ventana_ehci2_ulpi_platform_data = {
	.operating_mode = TEGRA_USB_HOST,
	.power_down_on_bus_suspend = 1,
	.phy_config = &ventana_ehci2_ulpi_phy_config,
};
#endif
static struct tegra_i2c_platform_data ventana_i2c1_platform_data = {
	.adapter_nr	= 0,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },
	.slave_addr = 0x00FC,
};

static const struct tegra_pingroup_config i2c2_ddc = {
	.pingroup	= TEGRA_PINGROUP_DDC,
	.func		= TEGRA_MUX_I2C2,
};

static const struct tegra_pingroup_config i2c2_gen2 = {
	.pingroup	= TEGRA_PINGROUP_PTA,
	.func		= TEGRA_MUX_I2C2,
};

static struct tegra_i2c_platform_data ventana_i2c2_platform_data = {
	.adapter_nr	= 1,
	.bus_count	= 2,
	.bus_clk_rate	= { 100000, 10000 },
	.bus_mux	= { &i2c2_ddc, &i2c2_gen2 },
	.bus_mux_len	= { 1, 1 },
	.slave_addr = 0x00FC,
};

static struct tegra_i2c_platform_data ventana_i2c3_platform_data = {
	.adapter_nr	= 3,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },
	.slave_addr = 0x00FC,
};

static struct tegra_i2c_platform_data ventana_dvc_platform_data = {
	.adapter_nr	= 4,
	.bus_count	= 1,
	.bus_clk_rate	= { 400000, 0 },
	.is_dvc		= true,
};

static struct tegra_audio_platform_data tegra_audio_pdata[] = {
	/* For I2S1 */
	[0] = {
		.i2s_master	= true,
		.dma_on		= true,  /* use dma by default */
		.i2s_master_clk = 44100,
		.i2s_clk_rate	= 11289600,
		.dap_clk	= "clk_dev1",
		.audio_sync_clk = "audio_2x",
		.mode		= I2S_BIT_FORMAT_I2S,
		.fifo_fmt	= I2S_FIFO_PACKED,
		.bit_size	= I2S_BIT_SIZE_16,
		.i2s_bus_width = 32,
		.dsp_bus_width = 16,
		.en_dmic = false, /* by default analog mic is used */
	},
	/* For I2S2 */
	[1] = {
		.i2s_master	= true,
		.dma_on		= true,  /* use dma by default */
		.i2s_master_clk = 8000,
		.dsp_master_clk = 8000,
		.i2s_clk_rate	= 2000000,
		.dap_clk	= "clk_dev1",
		.audio_sync_clk = "audio_2x",
		.mode		= I2S_BIT_FORMAT_DSP,
		.fifo_fmt	= I2S_FIFO_16_LSB,
		.bit_size	= I2S_BIT_SIZE_16,
		.i2s_bus_width = 32,
		.dsp_bus_width = 16,
	}
};

static struct tegra_das_platform_data tegra_das_pdata = {
	.dap_clk = "clk_dev1",
	.tegra_dap_port_info_table = {
		/* I2S1 <--> DAC1 <--> DAP1 <--> Hifi Codec */
		[0] = {
			.dac_port = tegra_das_port_i2s1,
			.dap_port = tegra_das_port_dap1,
			.codec_type = tegra_audio_codec_type_hifi,
			.device_property = {
				.num_channels = 2,
				.bits_per_sample = 16,
				.rate = 44100,
				.dac_dap_data_comm_format =
						dac_dap_data_format_all,
			},
		},
		[1] = {
			.dac_port = tegra_das_port_none,
			.dap_port = tegra_das_port_none,
			.codec_type = tegra_audio_codec_type_none,
			.device_property = {
				.num_channels = 0,
				.bits_per_sample = 0,
				.rate = 0,
				.dac_dap_data_comm_format = 0,
			},
		},
		[2] = {
			.dac_port = tegra_das_port_none,
			.dap_port = tegra_das_port_none,
			.codec_type = tegra_audio_codec_type_none,
			.device_property = {
				.num_channels = 0,
				.bits_per_sample = 0,
				.rate = 0,
				.dac_dap_data_comm_format = 0,
			},
		},
		/* I2S2 <--> DAC2 <--> DAP4 <--> BT SCO Codec */
		[3] = {
			.dac_port = tegra_das_port_i2s2,
			.dap_port = tegra_das_port_dap4,
			.codec_type = tegra_audio_codec_type_bluetooth,
			.device_property = {
				.num_channels = 1,
				.bits_per_sample = 16,
				.rate = 8000,
				.dac_dap_data_comm_format =
					dac_dap_data_format_dsp,
			},
		},
		[4] = {
			.dac_port = tegra_das_port_none,
			.dap_port = tegra_das_port_none,
			.codec_type = tegra_audio_codec_type_none,
			.device_property = {
				.num_channels = 0,
				.bits_per_sample = 0,
				.rate = 0,
				.dac_dap_data_comm_format = 0,
			},
		},
	},

	.tegra_das_con_table = {
		[0] = {
			.con_id = tegra_das_port_con_id_hifi,
			.num_entries = 2,
			.con_line = {
				[0] = {tegra_das_port_i2s1, tegra_das_port_dap1, true},
				[1] = {tegra_das_port_dap1, tegra_das_port_i2s1, false},
			},
		},
		[1] = {
			.con_id = tegra_das_port_con_id_bt_codec,
			.num_entries = 4,
			.con_line = {
				[0] = {tegra_das_port_i2s2, tegra_das_port_dap4, true},
				[1] = {tegra_das_port_dap4, tegra_das_port_i2s2, false},
				[2] = {tegra_das_port_i2s1, tegra_das_port_dap1, true},
				[3] = {tegra_das_port_dap1, tegra_das_port_i2s1, false},
			},
		},
	}
};

static void ventana_i2c_init(void)
{
	tegra_i2c_device1.dev.platform_data = &ventana_i2c1_platform_data;
	tegra_i2c_device2.dev.platform_data = &ventana_i2c2_platform_data;
	tegra_i2c_device3.dev.platform_data = &ventana_i2c3_platform_data;
	tegra_i2c_device4.dev.platform_data = &ventana_dvc_platform_data;

	i2c_register_board_info(0, ventana_i2c_bus1_board_info, 1);

	platform_device_register(&tegra_i2c_device1);
	platform_device_register(&tegra_i2c_device2);
	platform_device_register(&tegra_i2c_device3);
	platform_device_register(&tegra_i2c_device4);
}


#ifdef CONFIG_KEYBOARD_GPIO
#define GPIO_KEY(_id, _gpio, _iswake)		\
	{					\
		.code = _id,			\
		.gpio = TEGRA_GPIO_##_gpio,	\
		.active_low = 1,		\
		.desc = #_id,			\
		.type = EV_KEY,			\
		.wakeup = _iswake,		\
		.debounce_interval = 10,	\
	}
#if 0
static struct gpio_keys_button ventana_keys[] = {
	[0] = GPIO_KEY(KEY_FIND, PQ3, 0),
	[1] = GPIO_KEY(KEY_HOME, PQ1, 0),
	[2] = GPIO_KEY(KEY_BACK, PQ2, 0),
	[3] = GPIO_KEY(KEY_VOLUMEUP, PQ5, 0),
	[4] = GPIO_KEY(KEY_VOLUMEDOWN, PQ4, 0),
	[5] = GPIO_KEY(KEY_POWER, PV2, 1),
	[6] = GPIO_KEY(KEY_MENU, PC7, 0),
};

#define PMC_WAKE_STATUS 0x14

static int ventana_wakeup_key(void)
{
	unsigned long status =
		readl(IO_ADDRESS(TEGRA_PMC_BASE) + PMC_WAKE_STATUS);

	return status & TEGRA_WAKE_GPIO_PV2 ? KEY_POWER : KEY_RESERVED;
}

static struct gpio_keys_platform_data ventana_keys_platform_data = {
	.buttons	= ventana_keys,
	.nbuttons	= ARRAY_SIZE(ventana_keys),
	.wakeup_key	= ventana_wakeup_key,
};

static struct platform_device ventana_keys_device = {
	.name	= "gpio-keys",
	.id	= 0,
	.dev	= {
		.platform_data	= &ventana_keys_platform_data,
	},
};

static void ventana_keys_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ventana_keys); i++)
		tegra_gpio_enable(ventana_keys[i].gpio);
}
#endif
#endif

static struct platform_device tegra_camera = {
	.name = "tegra_camera",
	.id = -1,
};


#ifdef CONFIG_TEGRA_KEYPAD
#define KEYPAD_PWR_GPIO_NUM   TEGRA_GPIO_PV2
#define KEYPAD_VOLDL_GPIO_NUM TEGRA_GPIO_PQ0
#define KEYPAD_VOLUP_GPIO_NUM TEGRA_GPIO_PQ1
#define BODYSAR_GPIO_NUM      TEGRA_GPIO_PB2
#define BODYSAR_PWR_GPIO_NUM  TEGRA_GPIO_PZ2



static struct tegra_keypad_platform_data_t tegra_keypad_data = {
	.gpio_power = KEYPAD_PWR_GPIO_NUM,
	.gpio_voldown = KEYPAD_VOLDL_GPIO_NUM,
	.gpio_volup = KEYPAD_VOLUP_GPIO_NUM,
	.gpio_bodysar = BODYSAR_GPIO_NUM,
	.gpio_bodysar_pwr = BODYSAR_PWR_GPIO_NUM,
	
	
	
};

static struct platform_device tegra_keypad_device = {
	.name	= KEYPAD_DRIVER_NAME,
	.id	= -1,
	.dev	= {
		.platform_data	= &tegra_keypad_data,
	},
};

static int __init tegra_keypad_init(void)
{
	platform_device_register(&tegra_keypad_device);
	
	tegra_gpio_enable(KEYPAD_PWR_GPIO_NUM);
	tegra_gpio_enable(KEYPAD_VOLDL_GPIO_NUM);
	tegra_gpio_enable(KEYPAD_VOLUP_GPIO_NUM);
	tegra_gpio_enable(BODYSAR_GPIO_NUM);
	tegra_gpio_enable(BODYSAR_PWR_GPIO_NUM);
	
	
	
	printk("%s %d\n",__func__,__LINE__);
	return 0;
}	
#endif



#ifdef CONFIG_LUNA_BATTERY
static struct platform_device luna_battery_device =
{
  .name = "luna_battery",
  .id   = -1,
};
#endif
#ifdef CONFIG_LUNA_VIBRATOR
static struct platform_device luna_vibrator_device =
{
  .name = "luna_vibrator",
  .id   = -1,
};
#endif
#ifdef CONFIG_LUNA_LSENSOR
static struct platform_device luna_lsensor_device =
{
  .name = "luna_lsensor",
  .id   = -1,
};
#endif
#ifdef CONFIG_LUNA_LEDS
static struct platform_device luna_led_device =
{
  .name = "luna_led",
  .id   = -1,
};
#endif


static struct platform_device *ventana_devices[] __initdata = {
	
	&tegra_uartc_device,
	
	
	&tegra_uartd_device,
	
	&pmu_device,
	&tegra_gart_device,
	&tegra_aes_device,
#ifdef CONFIG_KEYBOARD_GPIO
	
#endif
	&tegra_wdt_device,
	&tegra_i2s_device1,
	&tegra_i2s_device2,
	&tegra_spdif_device,
	&tegra_avp_device,
	&tegra_camera,
	&tegra_das_device,
  
  #ifdef CONFIG_LUNA_BATTERY
    &luna_battery_device,
  #endif
  #ifdef CONFIG_LUNA_VIBRATOR
    &luna_vibrator_device,
  #endif
  #ifdef CONFIG_LUNA_LSENSOR
    &luna_lsensor_device,
  #endif
  #ifdef CONFIG_LUNA_LEDS
    &luna_led_device,
  #endif
  
};


#ifdef CONFIG_TOUCHSCREEN_PANJIT_I2C
static struct panjit_i2c_ts_platform_data panjit_data = {
	.gpio_reset = TEGRA_GPIO_PQ7,
};

static const struct i2c_board_info ventana_i2c_bus1_touch_info[] = {
	{
	 I2C_BOARD_INFO("panjit_touch", 0x3),
	 .irq = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PV6),
	 .platform_data = &panjit_data,
	 },
};

static int __init ventana_touch_init_panjit(void)
{
	tegra_gpio_enable(TEGRA_GPIO_PV6);

	tegra_gpio_enable(TEGRA_GPIO_PQ7);
	i2c_register_board_info(0, ventana_i2c_bus1_touch_info, 1);

	return 0;
}
#endif

#ifdef CONFIG_TOUCHSCREEN_ATMEL_mXT224
#define TOUCHSCREEN_RST_GPIO_NUM TEGRA_GPIO_PI6
#define TOUCHSCREEN_INT_GPIO_NUM TEGRA_GPIO_PI3
#define TOUCHSCREEN_I2C_ADDR 0x4A

static struct atmel_touchscreen_platform_data_t atmel_touchscreen_plat_data = {
	.gpiorst = TOUCHSCREEN_RST_GPIO_NUM,
	.gpioirq = TOUCHSCREEN_INT_GPIO_NUM,
};

static const struct i2c_board_info ventana_i2c_bus1_touch_info[] = {
	{
	 I2C_BOARD_INFO( ATMEL_TOUCHSCREEN_DRIVER_NAME, TOUCHSCREEN_I2C_ADDR),
	 .type           = ATMEL_TOUCHSCREEN_DRIVER_NAME,
	 .irq = TEGRA_GPIO_TO_IRQ(TOUCHSCREEN_INT_GPIO_NUM),
	 .platform_data = &atmel_touchscreen_plat_data,
	 },
};

static int __init ventana_touch_init_atmel(void)
{
	tegra_gpio_enable(TOUCHSCREEN_RST_GPIO_NUM);

	tegra_gpio_enable(TOUCHSCREEN_INT_GPIO_NUM);
	i2c_register_board_info(3, ventana_i2c_bus1_touch_info, 1);
	printk("%s %d\n",__func__,__LINE__);
	return 0;
}
#endif



#ifdef CONFIG_ATA2538_CAPKEY
#define CAPKEY_RST_GPIO_NUM TEGRA_GPIO_PT5
#define CAPKEY_INT_GPIO_NUM TEGRA_GPIO_PB1
#define CAPKEY_I2C_ADDR 0x68

static struct ata2538_capkey_platform_data_t ata2538_capkey_data = {
	.gpiorst = CAPKEY_RST_GPIO_NUM,
	.gpioirq = CAPKEY_INT_GPIO_NUM,
};

static struct i2c_board_info ventana_capkey_info[] = {
	{
		I2C_BOARD_INFO(CAPKEY_DRIVER_NAME, CAPKEY_I2C_ADDR),
		.type           = CAPKEY_DRIVER_NAME,
		.irq            = TEGRA_GPIO_TO_IRQ(CAPKEY_INT_GPIO_NUM),
		.platform_data  = &ata2538_capkey_data,
	},
};

static int __init ventana_capkey_init(void)
{
	tegra_gpio_enable(CAPKEY_RST_GPIO_NUM);
	
	tegra_gpio_enable(CAPKEY_INT_GPIO_NUM);
	i2c_register_board_info(3, ventana_capkey_info, ARRAY_SIZE(ventana_capkey_info));
	printk("%s %d\n",__func__,__LINE__);
	return 0;
}	
#endif


#ifdef CONFIG_TOUCHSCREEN_ATMEL_MT_T9
/* Atmel MaxTouch touchscreen              Driver data */
/*-----------------------------------------------------*/
/*
 * Reads the CHANGELINE state; interrupt is valid if the changeline
 * is low.
 */
static u8 read_chg(void)
{
	return gpio_get_value(TEGRA_GPIO_PV6);
}

static u8 valid_interrupt(void)
{
	return !read_chg();
}

static struct mxt_platform_data Atmel_mxt_info = {
	/* Maximum number of simultaneous touches to report. */
	.numtouch = 10,
	// TODO: no need for any hw-specific things at init/exit?
	.init_platform_hw = NULL,
	.exit_platform_hw = NULL,
	.max_x = 1366,
	.max_y = 768,
	.valid_interrupt = &valid_interrupt,
	.read_chg = &read_chg,
};

static struct i2c_board_info __initdata i2c_info[] = {
	{
	 I2C_BOARD_INFO("maXTouch", MXT_I2C_ADDRESS),
	 .irq = TEGRA_GPIO_TO_IRQ(TEGRA_GPIO_PV6),
	 .platform_data = &Atmel_mxt_info,
	 },
};

static int __init ventana_touch_init_atmel(void)
{
	tegra_gpio_enable(TEGRA_GPIO_PV6);
	tegra_gpio_enable(TEGRA_GPIO_PQ7);

	gpio_set_value(TEGRA_GPIO_PQ7, 0);
	msleep(1);
	gpio_set_value(TEGRA_GPIO_PQ7, 1);
	msleep(100);


	i2c_register_board_info(0, i2c_info, 1);

	return 0;
}
#endif

static struct usb_phy_plat_data tegra_usb_phy_pdata[] = {
	[0] = {
			.instance = 0,
			.vbus_irq = TPS6586X_INT_BASE + TPS6586X_INT_USB_DET,
			.vbus_gpio = TEGRA_GPIO_PD0,
	},
	[1] = {
			.instance = 1,
			.vbus_gpio = -1,
	},
	[2] = {
			.instance = 2,
			.vbus_gpio = TEGRA_GPIO_PD3,
	},
};

static struct tegra_ehci_platform_data tegra_ehci_pdata[] = {
	[0] = {
			.phy_config = &utmi_phy_config[0],
			.operating_mode = TEGRA_USB_HOST,
			.power_down_on_bus_suspend = 1,
	},
	[1] = {
			.phy_config = &ulpi_phy_config,
			.operating_mode = TEGRA_USB_HOST,
			.power_down_on_bus_suspend = 1,
	},
	[2] = {
			.phy_config = &utmi_phy_config[1],
			.operating_mode = TEGRA_USB_HOST,
			.power_down_on_bus_suspend = 1,
#ifdef CONFIG_USB_TEGRA_REMOTE_WAKEUP_WORKAROUND
			.remote_wakeup_gpio = TEGRA_GPIO_PJ7,
#endif
	},
};

static struct platform_device *tegra_usb_otg_host_register(void)
{
	struct platform_device *pdev;
	void *platform_data;
	int val;

	pdev = platform_device_alloc(tegra_ehci1_device.name, tegra_ehci1_device.id);
	if (!pdev)
		return NULL;

	val = platform_device_add_resources(pdev, tegra_ehci1_device.resource,
		tegra_ehci1_device.num_resources);
	if (val)
		goto error;

	pdev->dev.dma_mask =  tegra_ehci1_device.dev.dma_mask;
	pdev->dev.coherent_dma_mask = tegra_ehci1_device.dev.coherent_dma_mask;

	platform_data = kmalloc(sizeof(struct tegra_ehci_platform_data), GFP_KERNEL);
	if (!platform_data)
		goto error;

	memcpy(platform_data, &tegra_ehci_pdata[0],
				sizeof(struct tegra_ehci_platform_data));
	pdev->dev.platform_data = platform_data;

	val = platform_device_add(pdev);
	if (val)
		goto error_add;

	return pdev;

error_add:
	kfree(platform_data);
error:
	pr_err("%s: failed to add the host contoller device\n", __func__);
	platform_device_put(pdev);
	return NULL;
}

static void tegra_usb_otg_host_unregister(struct platform_device *pdev)
{
	kfree(pdev->dev.platform_data);
	pdev->dev.platform_data = NULL;
	platform_device_unregister(pdev);
}

static struct tegra_otg_platform_data tegra_otg_pdata = {
	.host_register = &tegra_usb_otg_host_register,
	.host_unregister = &tegra_usb_otg_host_unregister,
};


#define GPS_RESET_GPIO TEGRA_GPIO_PO7
#define GPS_PWR_GPIO TEGRA_GPIO_PP3


static int __init ventana_gps_init(void)
{
	struct clk *clk32 = clk_get_sys(NULL, "blink");
	if (!IS_ERR(clk32)) {
		clk_set_rate(clk32,clk32->parent->rate);
		clk_enable(clk32);
	}

	
	gpio_request(GPS_RESET_GPIO, "gps_reset");
	gpio_direction_output(GPS_RESET_GPIO, 0);
	tegra_gpio_enable(GPS_RESET_GPIO);

	tegra_gpio_enable(GPS_PWR_GPIO);

	gpio_set_value(GPS_RESET_GPIO, 0);
	msleep(1);
	gpio_set_value(GPS_RESET_GPIO, 1);
	
	
	
	return 0;
}

#define	POWEROFF_LOG_PATH "/data/misc"
#define POWEROFF_LOG_NAME "power_off"
#define POWER_OFF_LOG POWEROFF_LOG_PATH "/" POWEROFF_LOG_NAME

static char power_off_buffer[1024];

static void poweroff_flush_to_file(void)
{
	mm_segment_t oldfs;
	struct file *filp = NULL;
	unsigned long long offset = 0;
	int size = 0;
	struct rtc_time tm;
	struct timespec now;

	printk(KERN_INFO "Updating power off log.\n");

	oldfs = get_fs();
	set_fs(get_ds());

	getnstimeofday(&now);
	rtc_time_to_tm(now.tv_sec, &tm);

	size += snprintf(power_off_buffer+size, PAGE_SIZE-size,
		"%s-%d-%02d-%02d-%02d:%02d:%02d.log",
		POWER_OFF_LOG, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
	filp = filp_open(power_off_buffer, O_RDWR|O_APPEND|O_CREAT, S_IRWXU);

	if (IS_ERR(filp)) {
		printk(KERN_ERR "%s: Can't open %s. %ld\n", __func__, 
			POWER_OFF_LOG, PTR_ERR(filp));
		set_fs(oldfs);
		return;
	}

	vfs_write(filp, power_off_buffer, 1024, &offset);

	sys_sync();
	filp_close(filp, NULL);
	set_fs(oldfs);
}

static void ventana_power_off(void)
{
	int ret;

	poweroff_flush_to_file();
	ret = tps6586x_power_off();
	if (ret)
		pr_err("ventana: failed to power off\n");

	while(1);
}

static void __init ventana_power_off_init(void)
{
	pm_power_off = ventana_power_off;
}

#define SERIAL_NUMBER_LENGTH 20
static char usb_serial_num[SERIAL_NUMBER_LENGTH];
static void ventana_usb_init(void)
{
	char *src = NULL;
	int i;

	tegra_usb_phy_init(tegra_usb_phy_pdata, ARRAY_SIZE(tegra_usb_phy_pdata));
	/* OTG should be the first to be registered */
	tegra_otg_device.dev.platform_data = &tegra_otg_pdata;
	platform_device_register(&tegra_otg_device);

	platform_device_register(&tegra_usb_fsg_device);
	platform_device_register(&androidusb_device);
	platform_device_register(&tegra_udc_device);
	

	tegra_ehci3_device.dev.platform_data=&tegra_ehci_pdata[2];
	platform_device_register(&tegra_ehci3_device);

#ifdef CONFIG_USB_ANDROID_RNDIS
	src = usb_serial_num;

	/* create a fake MAC address from our serial number.
	 * first byte is 0x02 to signify locally administered.
	 */
	rndis_pdata.ethaddr[0] = 0x02;
	for (i = 0; *src; i++) {
		/* XOR the USB serial across the remaining bytes */
		rndis_pdata.ethaddr[i % (ETH_ALEN - 1) + 1] ^= *src++;
	}
	platform_device_register(&rndis_device);
#endif
}

static void __init tegra_ventana_init(void)
{
#if defined(CONFIG_TOUCHSCREEN_PANJIT_I2C) || \
	defined(CONFIG_TOUCHSCREEN_ATMEL_MT_T9)
	struct board_info BoardInfo;
#endif

	tegra_common_init();
	tegra_clk_init_from_table(ventana_clk_init_table);
	ventana_pinmux_init();
	ventana_i2c_init();
	snprintf(usb_serial_num, sizeof(usb_serial_num), "%llx", tegra_chip_uid());
	andusb_plat.serial_number = kstrdup(usb_serial_num, GFP_KERNEL);
	tegra_i2s_device1.dev.platform_data = &tegra_audio_pdata[0];
	tegra_i2s_device2.dev.platform_data = &tegra_audio_pdata[1];
	tegra_spdif_device.dev.platform_data = &tegra_spdif_pdata;
#ifdef CONFIG_BUILDTYPE_SHIP
	if (console_set_none == 1)
	{
		tegra_pinmux_set_tristate(TEGRA_PINGROUP_IRRX,
					TEGRA_TRI_TRISTATE);
		tegra_pinmux_set_tristate(TEGRA_PINGROUP_IRTX,
					TEGRA_TRI_TRISTATE);
	}
	else
	{
		if (is_tegra_debug_uartport_hs() == true)
			platform_device_register(&tegra_uartd_device);
		else
			platform_device_register(&debug_uart);
	}
#else
	if (is_tegra_debug_uartport_hs() == true)
		platform_device_register(&tegra_uartd_device);
	else
		platform_device_register(&debug_uart);
#endif
	tegra_das_device.dev.platform_data = &tegra_das_pdata;
	
	
	platform_add_devices(ventana_devices, ARRAY_SIZE(ventana_devices));

	ventana_sdhci_init();
	
	ventana_regulator_init();

#if defined(CONFIG_TOUCHSCREEN_PANJIT_I2C) || \
	defined(CONFIG_TOUCHSCREEN_ATMEL_MT_T9)

	tegra_get_board_info(&BoardInfo);

	/* boards with sku > 0 have atmel touch panels */
/*	if (BoardInfo.sku) {
		pr_info("Initializing Atmel touch driver\n");
		ventana_touch_init_atmel();
	} else {
		pr_info("Initializing Panjit touch driver\n");
		ventana_touch_init_panjit();
	}*/ 
#endif


#ifdef CONFIG_TOUCHSCREEN_ATMEL_mXT224 
	ventana_touch_init_atmel();
#endif



#ifdef CONFIG_ATA2538_CAPKEY
	ventana_capkey_init();
#endif


#ifdef CONFIG_TEGRA_KEYPAD
	tegra_keypad_init();
#endif

#ifdef CONFIG_KEYBOARD_GPIO
	
#endif
#ifdef CONFIG_KEYBOARD_TEGRA
	
#endif

	
	ventana_datacards_init();
	ventana_usb_init();
	ventana_gps_init();
	ventana_panel_init();
	ventana_sensors_init();
	ventana_bt_rfkill();
	ventana_power_off_init();
	
	tegra_setup_bluesleep();
}

int __init tegra_ventana_protected_aperture_init(void)
{
	tegra_protected_aperture_init(tegra_grhost_aperture);
	return 0;
}
late_initcall(tegra_ventana_protected_aperture_init);

void __init tegra_ventana_reserve(void)
{
	if (memblock_reserve(0x0, 4096) < 0)
		pr_warn("Cannot reserve first 4K of memory for safety\n");

	tegra_reserve(SZ_128M, SZ_2M, SZ_2M);
}

MACHINE_START(VENTANA, "Streak7")
	.boot_params    = 0x00000100,
	.phys_io        = IO_APB_PHYS,
	.io_pg_offst    = ((IO_APB_VIRT) >> 18) & 0xfffc,
	.init_irq       = tegra_init_irq,
	.init_machine   = tegra_ventana_init,
	.map_io         = tegra_map_common_io,
	.reserve        = tegra_ventana_reserve,
	.timer          = &tegra_timer,
MACHINE_END
