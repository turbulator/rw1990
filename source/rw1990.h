/*
 *  rw1990.h
 *
 *  This source is part of RW1990 command line tool.
 * 
 * Copyright (c) 2015, Alexey Vlasov <mail4turbo@gmail.com>
 * All rights reserved.
 * 
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. */
 
#ifndef RW1990_H
#define RW1990_H

#define PORT_NUM		0
#define ROM_LEN			8
#define SN_LEN			6

#define RW1990			1
#define RW1990_2		2

#define BITDELAY_MIN	10
#define BITDELAY_MAX	500

#define READROMCMD		0x33
#define WRITEROMCMD		0xD5
#define SETPROGMODECMD	0xD1
#define CHKPROGMODECMD	0xB5

#define PROGMODE2CMD	0x1D

SMALLINT rw1990ReadRom(int portnum, uchar *buffer);
SMALLINT rw1990WriteRom(int portnum, int delay, uchar *buffer);
SMALLINT rw1990WriteEnable(int portnum, int delay, SMALLINT sbit);
SMALLINT rw1990isWriteEnable(int portnum, int delay, int *mode);

#endif // RW1990_H