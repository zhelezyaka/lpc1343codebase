OUTFILE = firmware

# The family, flash and ram of the LPC1xxx microprocessor.
TARGET = LPC1343
FLASH = 32K
SRAM = 8K

# For USB support the LPC1xxx reserves 384 bytes from the sram,
# if you don't want to use the USB features, just use 0 here.
SRAM_USB = 384

# GNU GCC compiler prefix and location
CROSS_COMPILE = arm-none-eabi-
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

VPATH = core cpu gpio timer32 uart systick
OBJS = cpu.o gpio.o timer32.o uart.o systick.o main.o boot_LPC1xxx.o

ifneq ($(findstring LPC111,$(TARGET)),)
  CPU_TYPE = cortex-m0
  CPU_INCLUDE = LPC111x.h
  OBJS += core_cm0.o system_LPC11xx.o handlers_LPC11xx.o
endif

ifneq ($(findstring LPC134,$(TARGET)),)
  CPU_TYPE = cortex-m3
  CPU_INCLUDE = LPC134x.h
  OBJS += core_cm3.o system_LPC13xx.o handlers_LPC13xx.o
  HEADERS += LPC134x.h
endif

ifneq ($(findstring LPC17,$(TARGET)),)
  CPU_TYPE = cortex-m3
  CPU_INCLUDE = LPC17xx.h
  OBJS += core_cm3.o system_LPC17xx.o handlers_LPC17xx.0
endif

# Compiler settings, parameters and flags
CFLAGS  = -c -O2 -I. -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -include $(CPU_INCLUDE)
ASFLAGS = -c -O2 -I. -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
LDFLAGS = -nostartfiles -mcpu=$(CPU_TYPE) -mthumb -Wl,--gc-sections
OCFLAGS = --strip-debug --strip-unneeded

all: firmware

%.o : %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $<

firmware: $(OBJS)
	-@echo "MEMORY {"\
           "  flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH)"\
           "  sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB) }"\
	       "INCLUDE linkscript.ld" > memory.ld
	$(LD) $(LDFLAGS) -T memory.ld -o $(OUTFILE).elf $(OBJS)
	-@echo ' '
	$(SIZE) $(OUTFILE).elf;
	-@echo ' '
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUTFILE).elf $(OUTFILE).hex

clean:
	rm -f $(OBJS) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex

