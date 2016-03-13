/*
 * arch/arm/mach-tegra/include/mach/tegra3_i2s.h
 *
 * Copyright (c) 2010-2011, NVIDIA Corporation.
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

#ifndef __ARCH_ARM_MACH_TEGRA3_I2S_H
#define __ARCH_ARM_MACH_TEGRA3_I2S_H

#include <linux/kernel.h>
#include <linux/types.h>
#include <mach/audio.h>
#include <mach/audio_switch.h>
#include <mach/tegra_i2s.h>

#define NR_I2S_IFC	5

/*
*	I2S RegisterBase offsets from the Audio
*	cluster base
*/
#define TEGRA_I2S0_BASE_OFFSET	0x300
#define TEGRA_I2S1_BASE_OFFSET	0x400
#define TEGRA_I2S2_BASE_OFFSET	0x500
#define TEGRA_I2S3_BASE_OFFSET	0x600
#define TEGRA_I2S4_BASE_OFFSET	0x700

/* Register Offsets from TEGRA_I2S_BASE */

#define I2S_CTRL_0				0x0
#define I2S_TIMING_0				0x4
#define I2S_OFFSET_0				0x08
#define I2S_CH_CTRL_0				0x0c
#define I2S_SLOT_CTRL_0				0x10
#define I2S_AUDIOCIF_I2STX_CTRL_0		0x14
#define I2S_AUDIOCIF_I2SRX_CTRL_0		0x18
#define I2S_FLOWCTL_0				0x1c
#define I2S_TX_STEP_0				0x20
#define I2S_FLOW_STATUS_0			0x24
#define I2S_FLOW_TOTAL_0			0x28
#define I2S_FLOW_OVER_0 			0x2c
#define I2S_LCOEF_1_4_0_0			0x34
#define I2S_LCOEF_1_4_1_0			0x38
#define I2S_LCOEF_1_4_2_0			0x3c
#define I2S_LCOEF_1_4_3_0			0x40
#define I2S_LCOEF_1_4_4_0			0x44
#define I2S_LCOEF_1_4_5_0			0x48
#define I2S_LCOEF_2_4_0_0			0x4c
#define I2S_LCOEF_2_4_1_0			0x50
#define I2S_LCOEF_2_4_2_0			0x54

#define I2S_REG_MAXINDEX	(I2S_LCOEF_2_4_2_0 >> 2)
/*
 * I2S_CTRL_0
 */

#define I2S_CTRL_XFER_EN_TX			(1<<31)
#define I2S_CTRL_XFER_EN_RX			(1<<30)
#define I2S_CTRL_CG_EN				(1<<29)
#define I2S_CTRL_SOFT_RESET			(1<<28)
#define I2S_CTRL_TX_FLOWCTL_EN			(1<<27)
#define I2S_CTRL_OBS_SEL			(1<<24)

#define I2S_CTRL_FRAME_FORMAT_SHIFT	12
#define I2S_CTRL_FRAME_FORMAT_MASK	\
		(0x7<<I2S_CTRL_FRAME_FORMAT_SHIFT);
#define I2S_CTRL_FRAME_FORMAT_LRCK	\
		(0<<I2S_CTRL_FRAME_FORMAT_SHIFT) /* BASIC/LJM/RJM */
#define I2S_CTRL_FRAME_FORMAT_FSYNC	\
		(1<<I2S_CTRL_FRAME_FORMAT_SHIFT) /* DSP/PCM/NW/TDM */

#define I2S_CTRL_MASTER_ENABLE		(1<<10)

/* Left/Right Control Polarity.
*  0= Left channel when LRCK is low,
*  Right channel  when LRCK is high, 1= vice versa
*/
#define I2S_LRCK_SHIFT		9

#define I2S_CTRL_LRCK_MASK	(1<<I2S_LRCK_SHIFT)
#define I2S_CTRL_LRCK_L_LOW	(AUDIO_LRCK_LEFT_LOW << I2S_LRCK_SHIFT)
#define I2S_CTRL_LRCK_R_LOW	(AUDIO_LRCK_RIGHT_LOW << I2S_LRCK_SHIFT)

#define I2S_CTRL_LPBK_ENABLE	(1<<8)

#define I2S_CTRL_BIT_CODE_SHIFT		4

#define I2S_CTRL_BIT_CODE_MASK		(3<<I2S_CTRL_BIT_CODE_SHIFT)
#define I2S_CTRL_BIT_CODE_LINEAR	\
		(AUDIO_BIT_CODE_LINEAR<<I2S_CTRL_BIT_CODE_SHIFT)
#define I2S_CTRL_BIT_CODE_ULAW		\
		(AUDIO_BIT_CODE_ULAW<<I2S_CTRL_BIT_CODE_SHIFT)
#define I2S_CTRL_BIT_CODE_ALAW		\
		(AUDIO_BIT_CODE_ALAW<<I2S_CTRL_BIT_CODE_SHIFT)
#define I2S_CTRL_BIT_CODE_RSVD		\
		(AUDIO_BIT_CODE_RSVD<<I2S_CTRL_BIT_CODE_SHIFT)

#define I2S_CTRL_BIT_SIZE_SHIFT	0

#define I2S_CTRL_BIT_SIZE_MASK	(0x7 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_8	\
		(AUDIO_BIT_SIZE_8 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_12	\
		(AUDIO_BIT_SIZE_12 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_16	\
		(AUDIO_BIT_SIZE_16 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_20	\
		(AUDIO_BIT_SIZE_20 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_24	\
		(AUDIO_BIT_SIZE_24 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_28	\
		(AUDIO_BIT_SIZE_28 << I2S_CTRL_BIT_SIZE_SHIFT)
#define I2S_CTRL_BIT_SIZE_32	\
		(AUDIO_BIT_SIZE_32 << I2S_CTRL_BIT_SIZE_SHIFT)


/*
 * I2S_TIMING_0
 */

#define I2S_TIMING_NON_SYM_ENABLE		(1<<12)
#define I2S_TIMING_CHANNEL_BIT_COUNT_MASK	0x7ff
#define I2S_TIMING_CHANNEL_BIT_COUNT		(1<<0)


/*
 * I2S_OFFSET_0
 */

#define I2S_OFFSET_RX_DATA_OFFSET_SHIFT		16
#define I2S_OFFSET_TX_DATA_OFFSET_SHIFT		0
#define I2S_OFFSET_RX_DATA_OFFSET_MASK		\
		(0x7ff<<I2S_OFFSET_RX_DATA_OFFSET_SHIFT)
#define I2S_OFFSET_TX_DATA_OFFSET_MASK		\
		(0x7ff<<I2S_OFFSET_TX_DATA_OFFSET_SHIFT)

/*
 * I2S_CH_CTRL_0
 */

/*
* Fsync in terms of bit clocks
* 0 - 1 clock wide
* 1 - 2 clock wide
* n - n+1 clock wide
*/
#define I2S_CH_CTRL_FSYNC_WIDTH_SHIFT		24
#define I2S_CH_CTRL_FSYNC_WIDTH_MASK		\
		(0xff << I2S_CH_CTRL_FSYNC_WIDTH_SHIFT)

/*
* Highz control
*/
#define I2S_CH_CTRL_HIGHZ_CTRL_SHIFT	12
#define I2S_CH_CTRL_HIGHZ_CTRL_MASK	\
		(3 << I2S_CH_CTRL_HIGHZ_CTRL_SHIFT)

/*
*  BIT_ORDER
*/
#define I2S_CH_CTRL_RX_BIT_ORDER_SHIFT	10
#define I2S_CH_CTRL_TX_BIT_ORDER_SHIFT	9
#define I2S_CH_CTRL_RX_BIT_MSB_FIRST	\
		(AUDIO_BIT_ORDER_MSB_FIRST << I2S_CH_CTRL_RX_BIT_ORDER_SHIFT)
#define I2S_CH_CTRL_TX_BIT_MSB_FIRST	\
		(AUDIO_BIT_ORDER_MSB_FIRST << I2S_CH_CTRL_TX_BIT_ORDER_SHIFT)
#define I2S_CH_CTRL_RX_BIT_LSB_FIRST	\
		(AUDIO_BIT_ORDER_LSB_FIRST << I2S_CH_CTRL_RX_BIT_ORDER_SHIFT)
#define I2S_CH_CTRL_TX_BIT_LSB_FIRST	\
		(AUDIO_BIT_ORDER_LSB_FIRST << I2S_CH_CTRL_TX_BIT_ORDER_SHIFT)

/*
*  Edge Control
*/
#define I2S_CH_CTRL_EGDE_CTRL_SHIFT		8
#define I2S_CH_CTRL_EGDE_CTRL_POS_EDGE		\
		(AUDIO_SAMPLE_POS_EDGE << I2S_CH_CTRL_EGDE_CTRL_SHIFT)
#define I2S_CH_CTRL_EGDE_CTRL_NEG_EDGE		\
		(AUDIO_SAMPLE_NEG_EDGE << I2S_CH_CTRL_EGDE_CTRL_SHIFT)

/*
*  Mask bits
*/
#define I2S_CH_CTRL_RX_MASK_BITS_SHIFT		4
#define I2S_CH_CTRL_TX_MASK_BITS_SHIFT		0


#define I2S_CH_CTRL_RX_MASK_BITS_MASK	\
		(7 << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_ZERO	\
		(AUDIO_MASK_BITS_ZERO << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_ONE	\
		(AUDIO_MASK_BITS_ONE << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_TWO	\
		(AUDIO_MASK_BITS_TWO << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_THREE	\
		(AUDIO_MASK_BITS_THREE << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_FOUR	\
		(AUDIO_MASK_BITS_FOUR << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_FIVE	\
		(AUDIO_MASK_BITS_FIVE << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_SIX	\
		(AUDIO_MASK_BITS_SIX << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_RX_MASK_BITS_SEVEN	\
		(AUDIO_MASK_BITS_SEVEN << I2S_CH_CTRL_RX_MASK_BITS_SHIFT)

#define I2S_CH_CTRL_TX_MASK_BITS_MASK	\
		(7 << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_ZERO	\
		(AUDIO_MASK_BITS_ZERO << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_ONE	\
		(AUDIO_MASK_BITS_ONE << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_TWO	\
		(AUDIO_MASK_BITS_TWO << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_THREE	\
		(AUDIO_MASK_BITS_THREE << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_FOUR	\
		(AUDIO_MASK_BITS_FOUR << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_FIVE	\
		(AUDIO_MASK_BITS_FIVE << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_SIX	\
		(AUDIO_MASK_BITS_SIX << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)
#define I2S_CH_CTRL_TX_MASK_BITS_SEVEN	\
		(AUDIO_MASK_BITS_SEVEN << I2S_CH_CTRL_TX_MASK_BITS_SHIFT)

/*
*  Slot control
*  0 - frame with 1 slot
*  1 - frame with 2 slot
*  n - frame with n+1 slot
*/
#define I2S_SLOT_CTRL_TOTAL_SLOT_SHIFT	16
#define I2S_SLOT_CTRL_RX_SLOT_SHIFT	8
#define I2S_SLOT_CTRL_TX_SLOT_SHIFT	0

#define I2S_SLOT_CTRL_TOTAL_SLOT_MASK	\
		(7 << I2S_SLOT_CTRL_TOTAL_SLOT_SHIFT)
#define I2S_SLOT_CTRL_TOTAL_SLOT_ZERO	\
		(0 << I2S_SLOT_CTRL_TOTAL_SLOT_SHIFT)
#define I2S_SLOT_CTRL_TOTAL_SLOT_ONE	\
		(1 << I2S_SLOT_CTRL_TOTAL_SLOT_SHIFT)

/* Used for TDM mode */
#define I2S_SLOT_CTRL_RX_SLOT_MASK	\
		(0xff << I2S_SLOT_CTRL_RX_SLOT_SHIFT)
#define I2S_SLOT_CTRL_TX_SLOT_MASK	\
		(0xff << I2S_SLOT_CTRL_TX_SLOT_SHIFT)

/*
* I2sTx Audiocif Ctrl
* I2sRx Audiocif Ctrl
* Use the generic code provided in audio_common.h
*/

/*
* Flow control
*/
#define I2S_FLOWCTL_FILTER_SHIFT	31
#define I2S_FLOWCTL_FILTER_LINEAR	\
		(0 << I2S_FLOWCTL_FILTER_SHIFT)
#define I2S_FLOWCTL_FILTER_QUAD		\
		(1 << I2S_FLOWCTL_FILTER_SHIFT)

/*
* Tx Step
*/
#define I2S_TX_STEP_SHIFT	0
#define I2S_TX_STEP_MASK	\
		(0xffff << I2S_TX_STEP_SHIFT)

/*
* Flow control status
*/
#define I2S_FLOW_STATUS_UNDERFLOW_UNDER		(1<<31)
#define I2S_FLOW_STATUS_OVERFLOW_OVER		(1<<30)
#define I2S_FLOW_MONITOR_INT_EN			(1<<4)
#define I2S_FLOW_COUNTER_EN			(1<<1)
#define I2S_FLOW_MONITOR_EN			(1<<0)

/*
* coefficients
*/
#define I2S_LCOEF_COEF_MASK		(0xffff<<0)

#define I2S_FIFO_ATN_LVL_FOUR_SLOTS		4
#define I2S_FIFO_ATN_LVL_EIGHT_SLOTS		8
#define I2S_FIFO_ATN_LVL_ONE_SLOT		1
#define I2S_FIFO_TX_BUSY			(1<<1)
#define I2S_FIFO_RX_BUSY			(1<<0)


/*
 * API
 */

int i2s_set_clock_gating(int ifc, int enable);
int i2s_set_soft_reset(int ifc, int enable);

int  i2s_set_bit_code(int ifc, unsigned bitcode);
int  i2s_set_data_offset(int ifc, int tx, int dataoffset);
int  i2s_set_edge_control(int ifc, int edgectrl);
int  i2s_set_highz_control(int ifc, int highzvalue);
int  i2s_set_fsync_width(int ifc, int fsyncwidth);
int  i2s_set_slot_control(int ifc, int tx, int totalslot, int numslots);
int  i2s_set_bit_order(int ifc, int tx, int bitorder);
int  i2s_set_bit_mask(int ifc, int tx, int maskbit);
int  i2s_set_flow_control(int ifc, int enable, int filtertype, int stepsize);

int i2s_set_acif(int ifc, int fifo_mode, struct audio_cif *cifInfo);
int i2s_set_dma_channel(int ifc, int fifo_mode, int dma_ch);

#endif /* __ARCH_ARM_MACH_TEGRA3_I2S_H */

