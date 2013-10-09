TARGET=dropcounter

CC=msp430-gcc
SIZE=msp430-size
STRIP=msp430-strip
OBJDUMP=msp430-objdump

CFLAGS=-Os -g -Wall -g -mmcu=msp430g2553 -ffunction-sections -fdata-sections -finline-small-functions

LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,--gc-sections

OBJS=main.o display.o spi.o
LST=main.lst display.lst spi.lst

all: $(TARGET).elf
lst: $(LST)

$(TARGET).elf: $(OBJS) $(LST) Makefile
	$(CC) $(CFLAGS) -o $(TARGET).elf $(OBJS) $(LDFLAGS)
	$(STRIP) $(TARGET).elf
	$(SIZE) --format=sysv $(TARGET).elf

install: $(TARGET).elf
	mspdebug rf2500 "prog $(TARGET).elf"

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.lst: %.c
	$(CC) -c $(CFLAGS) -Wa,-anlhd $< > $@

clean:
	rm -rf $(OBJS) *.lst *.elf *.map
