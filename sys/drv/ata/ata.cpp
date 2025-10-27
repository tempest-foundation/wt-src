// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#include "drv/ata/ata.hpp"

#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.hpp>
#endif
#include <kstddef.hpp>

#define ATA_PRIMARY_IO   0x1F0
#define ATA_PRIMARY_CTRL 0x3F6

// I/O registers (offsets from base)
#define ATA_REG_DATA     0x00
#define ATA_REG_ERROR    0x01
#define ATA_REG_SECCOUNT 0x02
#define ATA_REG_LBA_LO   0x03
#define ATA_REG_LBA_MID  0x04
#define ATA_REG_LBA_HI   0x05
#define ATA_REG_DRIVE    0x06
#define ATA_REG_STATUS   0x07
#define ATA_REG_COMMAND  0x07

// Control register
#define ATA_REG_CONTROL  0x02  // base 0x3F6 + 2 == 0x3F8, but 0x3F6 often used
#define ATA_CONTROL_nIEN 0x02

// Status bits
#define ATA_SR_BSY  0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DRQ  0x08
#define ATA_SR_ERR  0x01

#define ATA_CMD_READ_SECTORS 0x20

namespace ata {
	static inline void io_wait(void) {
		// 400ns delay by reading alternate-status port 4 times
		for( int i = 0; i < 4; i++ )
			(void) inb(ATA_PRIMARY_CTRL);
	}

	// Poll until BSY=0 and either DRQ=1 or ERR=1. Return 0 on ready, -1 on error
	static int poll(void) {
		uint8_t status;
		// Initial delay
		io_wait();
		while( (status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS)) & ATA_SR_BSY )
			;
		if( status & ATA_SR_ERR )
			return -1;
		if( !(status & ATA_SR_DRQ) )
			return -1;
		return 0;
	}

	int pio_read(uint64_t lba, uint32_t count, void *buf) {
		if( count == 0 || buf == nullptr )
			return -1;
		if( lba > 0x0FFFFFFF )  // 28-bit limit
			return -1;

		uint8_t *ptr = (uint8_t *) buf;
		for( uint32_t i = 0; i < count; i++ ) {
			uint32_t cur_lba = (uint32_t) (lba + i);

			// Select drive + LBA bits 24-27
			outb(ATA_PRIMARY_IO + ATA_REG_DRIVE,
			     0xE0 | ((cur_lba >> 24) & 0x0F));

			// Sector count
			outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT, 1);
			// LBA low/mid/high
			outb(ATA_PRIMARY_IO + ATA_REG_LBA_LO, (uint8_t) cur_lba);
			outb(ATA_PRIMARY_IO + ATA_REG_LBA_MID, (uint8_t) (cur_lba >> 8));
			outb(ATA_PRIMARY_IO + ATA_REG_LBA_HI, (uint8_t) (cur_lba >> 16));

			// Issue READ SECTORS command
			outb(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_READ_SECTORS);

			if( poll() != 0 )
				return -1;

			// Transfer 256 words (512 bytes)
			for( int w = 0; w < 256; w++ ) {
				uint16_t data = inw(ATA_PRIMARY_IO + ATA_REG_DATA);
				ptr[0]        = (uint8_t) data;
				ptr[1]        = (uint8_t) (data >> 8);
				ptr += 2;
			}
		}
		return 0;
	}
}  // namespace ata
