/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include <nuttx/config.h>
#include <board_config.h>

#include <nuttx/spi/spi.h>
#include <px4_platform_common/px4_manifest.h>

static const px4_mft_device_t spi4 = {             // FM25V02A on FMUM native: 128K X 8, emulated as (1024 Blocks of 32)
	.bus_type = px4_mft_device_t::SPI,
	.devid    = SPIDEV_FLASH(0)
};
static const px4_mft_device_t i2c4 = {             // 24LC64T   8K 32 X 256
	.bus_type =  px4_mft_device_t::I2C,
	.devid    =  PX4_MK_I2C_DEVID(4, 0x50)
};


static const px4_mtd_entry_t fmum_fram = {
	.device = &spi4,
	.npart = 1,
	.partd = {
		{
			.type = MTD_PARAMETERS,
			.path = "/mnt/microsd",
			.nblocks = (131072 / (1 << CONFIG_RAMTRON_EMULATE_SECTOR_SHIFT))
		}
	},
};

static const px4_mtd_entry_t fmum_eeprom = {
	.device = &i2c4,
	.npart = 5,
	.partd = {
		{
			.type = MTD_CALDATA,
			.path = "/fs/mtd_caldata",
			.nblocks = 224
		},
		{
			.type = MTD_MFT_VER,
			.path = "/fs/mtd_mft_ver",
			.nblocks = 8
		},
		{
			.type = MTD_MFT_REV,
			.path = "/fs/mtd_mft_rev",
			.nblocks = 8
		},
		{
			.type = MTD_ID,
			.path = "/fs/mtd_id",
			.nblocks = 8 // 256 = 32 * 8
		},
		{
			.type = MTD_NET,
			.path = "/fs/mtd_net",
			.nblocks = 8 // 256 = 32 * 8
		}
	},
};

static const px4_mtd_manifest_t board_mtd_config = {
	.nconfigs   = 2,
	.entries = {
		&fmum_fram,
		&fmum_eeprom
	}
};

static const px4_mft_entry_s mtd_mft = {
	.type = MTD,
	.pmft = (void *) &board_mtd_config,
};

static const px4_mft_entry_s mft_mft = {
	.type = MFT,
	.pmft = (void *) system_query_manifest,
};

static const px4_mft_s mft = {
	.nmft = 2,
	.mfts = {
		&mtd_mft,
		&mft_mft,
	}
};

const px4_mft_s *board_get_manifest(void)
{
	return &mft;
}
