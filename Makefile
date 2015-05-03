#
#  Makefile
#
#  This source is part of RW1990 command line tool.
#
#  Copyright (c) 2015, Alexey Vlasov <mail4turbo@gmail.com>
#  All rights reserved.
#
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE file.

################################################################################
# Include the make variables (CC, etc...)                                      #
################################################################################
AS      = as
LD      = ld
CC      = gcc
CPP     = $(CC) -E
AR      = ar
NM      = nm
LDR     = ldr
STRIP   = strip
OBJCOPY = objcopy
OBJDUMP = objdump
RANLIB  = RANLIB
DEPEND  = $(CC) -E -M

################################################################################

CFLAGS  = -g -Wall -O2 -I./source
LDFLAGS = -lc
ARFLAGS = crv

SRCS = 
SRCS += $(wildcard ./source/*.c)

OBJS = $(SRCS:.c=.o)

TARGET = rw1990

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(OBJDUMP) -D $@ > $@.dump
	$(STRIP) $@ $@


clean:
	rm -rf ./source/*.o ./source/*.d 
	rm -rf $(TARGET) $(TARGET).dump

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
        
%.d: %.c
	@$(DEPEND) $(CFLAGS) -MF $@ -c $<
                
include $(SRCS:.c=.d)

#########################################################################

