/*
 *  main.c
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
#include <getopt.h>
#include <string.h>

#include "ownet.h"
#include "rw1990.h"

#define READ 		0
#define WRITE		1

char *device = "/dev/ttyUSB0";
uchar read_serial_number[SN_LEN] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
uchar write_serial_number[SN_LEN] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
int mode = READ;
int type = RW1990;
int delay = BITDELAY_MIN;

void usage (void)
{	
	printf ("Usage: rw1990 [OPTIONS]\n\n");
	
	printf ("Options:\n");
	printf ("  \033[1m-d, --device\033[0m\n     Specify the device, overriding the value /dev/ttyUSB0\n\n");
	printf ("  \033[1m-t, --type\033[0m\n      Specify the type of key:\n        1 - RW1990\n        2 - RW1990.2\n\n");
	printf ("  \033[1m-w, --write\033[0m\n      New serial number key like: 0000FFFFFF\n\n");
	printf ("  \033[1m-s, --delay\033[0m\n      Specify bit delay for write operation in range of 10 - 500 ms. Default is 10 ms.\n\n");
}


int main (int argc, char **argv)
{
	int i, c;	
	int ret;

	while (1) {
		static struct option long_options[] =
		{
			{"device",   required_argument, 0, 'd'},
			{"write",    required_argument, 0, 'w'},
			{"type",     required_argument, 0, 't'},
			{"delay",    required_argument, 0, 's'},
			{"help",     no_argument,       0, 'h'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "d:w:t:s:h",
							long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'd':
				if (optarg) 
					device = optarg;
				break;

			case 'w':
					mode = WRITE;
					if(optarg) {
						if(sscanf(optarg, "%02X%02X%02X%02X%02X%02X", 
							(unsigned int*)&write_serial_number[0], 
							(unsigned int*)&write_serial_number[1], 
							(unsigned int*)&write_serial_number[2], 
							(unsigned int*)&write_serial_number[3], 
							(unsigned int*)&write_serial_number[4], 
							(unsigned int*)&write_serial_number[5]) != SN_LEN) {
							usage();
							exit(-1);
						}
					}
				break;

			case 't':
				if (optarg) 
					type = atoi(optarg);
				if (type < RW1990 || type > RW1990_2) {
					usage();
					exit(-1);
				}
				break;

			case 's':
				if (optarg) 
					delay = atoi(optarg);
				if (delay < BITDELAY_MIN || delay > BITDELAY_MAX) {
					usage();
					exit(-1);
				}
				break;

			case 'h':
				usage();
				exit(-1);
				break;

			default:
				usage();
				exit(-1);
			}
	}

	ret = owAcquire(PORT_NUM, device);
	if(ret != TRUE) {
		owPrintErrorMsgStd();
		exit(-1);
	}

	ret = owTouchReset(PORT_NUM);
	if(ret != TRUE) {
		owPrintErrorMsgStd();
		exit(-1);
	}
	
	if (mode == WRITE) {
		ret = rw1990WriteRom(PORT_NUM, delay, write_serial_number);
		if (ret != TRUE) {
			owPrintErrorMsgStd();
			exit(-1);
		}
	}
	
	ret = rw1990ReadRom(PORT_NUM, read_serial_number);
	
	if (mode == WRITE) {
		if(memcmp(write_serial_number, read_serial_number, SN_LEN) == 0) 
			printf("Write ok: ");
		else
			printf("Write failed: ");
	}

	for (i = 0; i < SN_LEN; i++) {
		printf("%02X", read_serial_number[i]);
	}
	
	printf("\n");

    return 0;
}
