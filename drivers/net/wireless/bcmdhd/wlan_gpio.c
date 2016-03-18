/*
* Customer code to add GPIO control during WLAN start/stop
* Copyright (C) 1999-2010, Broadcom Corporation
*
*      Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2 (the "GPL"),
* available at http://www.broadcom.com/licenses/GPLv2.php, with the
* following added to such license:
*
*      As a special exception, the copyright holders of this software give you
* permission to link this software with independent modules, and to copy and
* distribute the resulting executable under terms of your choice, provided that
* you also meet, for each linked independent module, the terms and conditions of
* the license of that module.  An independent module is a module which is not
* derived from this software.  The special exception does not apply to any
* modifications of the software.
*
*      Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*
* $Id: dhd_custom_gpio.c,v 1.1.4.6 2010/02/19 22:56:49 Exp $
*/

#include <linux/delay.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <hndsoc.h>
#include <sbchipc.h>
#include <pcicfg.h>

#define VENTANA_WLAN_PWR        TEGRA_GPIO_PX1
#define VENTANA_WLAN_IRQ        TEGRA_GPIO_PU5

extern void tegra_sdhci_force_presence_change();
extern void ventana_wifi_set_carddetect(int);
extern void ventana_wifi_power(int);

/* this is called by exit() */
void nvidia_wlan_poweron(int flag)
{
	if (flag == 1) {
		// power
		ventana_wifi_power(1);
		ventana_wifi_set_carddetect(1);
	} else {
		OSL_DELAY(150);
	}
}
EXPORT_SYMBOL(nvidia_wlan_poweron);
void nvidia_wlan_poweroff(int flag)
{
	if (flag == 1) {
		// power
                ventana_wifi_power(0);
                ventana_wifi_set_carddetect(0);
	} else {
		pr_info("nvidia_wlan_poweroff ==== skip\n");
	}
}
EXPORT_SYMBOL(nvidia_wlan_poweroff);
