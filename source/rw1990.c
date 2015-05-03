/*
 *  rw1990.c
 *
 *  This source is part of RW1990 command line tool.
 * 
 * Copyright (c) 2015, Alexey Vlasov <mail4turbo@gmail.com>
 * All rights reserved.
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ownet.h"
#include "rw1990.h"

SMALLINT rw1990ReadRom(int portnum, uchar *buffer)
{
	uchar sendpacket[ROM_LEN];
	uchar lastcrc8;
	int i;

	/* reset the 1-wire */
	if (owTouchReset(portnum))
	{
		/* create a buffer to use with block function */
		memset(sendpacket, 0xFF, sizeof(sendpacket));

		owWriteByte(portnum, READROMCMD);

		/* send/recieve the transfer buffer */
		if (owBlock(portnum, FALSE, sendpacket, ROM_LEN))
		{
			for (i = 0; i < SN_LEN; i++) {
				buffer[i] = sendpacket[SN_LEN - i];
			}
			
			setcrc8(portnum, 0);
			for (i = 0; i < ROM_LEN; i++) {
				lastcrc8 = docrc8(portnum, sendpacket[i]);
			}

			if (lastcrc8 != 0) {
				OWERROR(OWERROR_CRC_FAILED);
				return FALSE;
			}
		}
		else {
			OWERROR(OWERROR_BLOCK_FAILED);
			return FALSE;
		}
	}
	else {
		OWERROR(OWERROR_NO_DEVICES_ON_NET);
		return FALSE;
	}

	/* reset or match echo failed */
	return TRUE;
}

SMALLINT rw1990WriteEnable(int portnum, int delay, SMALLINT sbit)
{
	/* reset the 1-wire */
	if (owTouchReset(portnum)) {
		/* go to programm mode */
		owTouchByte(portnum, SETPROGMODECMD);
		/* write flag */ 
		owTouchBit(portnum, sbit);
		/* delay */
		msDelay(delay);
	}
	else {
		OWERROR(OWERROR_BLOCK_FAILED);
		return FALSE;
	}

	return TRUE;
}

SMALLINT rw1990isWriteEnable(int portnum, int delay, int *mode)
{
	/* reset the 1-wire */
	if (owTouchReset(portnum))
	{
		/* check programm mode */
		owTouchByte(portnum, CHKPROGMODECMD);
		/* read flag */ 
		*mode = owTouchBit(portnum, 1);
		msDelay(delay);
	}
   	else {
		OWERROR(OWERROR_BLOCK_FAILED);
		return FALSE;
	}

	return TRUE;
}

SMALLINT rw1990TouchByte(int portnum, int delay, SMALLINT sendbyte)
{
	int i;

	for (i = 0; i < 8; i++) {
		owTouchBit(portnum, sendbyte & 0x01);
		msDelay(delay);
		sendbyte = sendbyte >> 1;
		printf(".");
		fflush(stdout);
	}
	
	return TRUE;
}

SMALLINT rw1990WriteRom(int portnum, int delay, uchar *buffer)
{
	uchar sendpacket[ROM_LEN] = {0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x9B}; 
	int i, mode;
	
	/* enable program mode */
	if (rw1990WriteEnable(portnum, delay, 0) != TRUE) {
		return FALSE;
	}
	
	/* check program mode */
	if (rw1990isWriteEnable(portnum, delay, &mode) != TRUE) {
		return FALSE;
	}
	
	if(mode != 1) { /* is program mode enabled */
		OWERROR(OWERROR_WRITE_ENABLE);
		return FALSE;
	}

	/* reset the 1-wire */
	if (owTouchReset(portnum)) {
		owWriteByte(portnum, WRITEROMCMD);

		/* copy serial number to buffer */
		for (i = 0; i < SN_LEN; i++) { 
			sendpacket[SN_LEN - i] = buffer[i];
		}
		
		/* calculate crc */
		setcrc8(portnum, 0);
		for (i = 0; i < ROM_LEN - 1; i++) {
			sendpacket[ROM_LEN - 1] = docrc8(portnum, sendpacket[i]);
		}

		/* invert all bits in buffer */
		for (i = 0; i < ROM_LEN; i++) {
			sendpacket[i] ^= 0xFF;
			rw1990TouchByte(portnum, delay, sendpacket[i]);
		}

		printf("\n");
	}
	else {
		return FALSE;
	}
	
	/* disable program mode */
	if (rw1990WriteEnable(portnum, delay, 1) != TRUE) {
		return FALSE;
	}
	
	/* check program mode */
	if (rw1990isWriteEnable(portnum, delay, &mode) != TRUE) {
		return FALSE;
	}
	
	if(mode != 0) { /* is program mode disabled */
		OWERROR(OWERROR_WRITE_DISABLE);
		return FALSE;
	}

	return TRUE;
}
