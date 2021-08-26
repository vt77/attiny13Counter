
# Compiler Options
DEVICE  = attiny13a
CLOCK   = 9600000

# Programmer Options
PROGRMR = usbasp
TGTDEV  = attiny13
LFUSE   = 0x3a
HFUSE   = 0xf

TARGET = smbus-slave-$(DEVICE)

AVRDUDE = avrdude -c $(PROGRMR) -p $(TGTDEV)

LIBS = lib/smbus/smbus.c

#Flags : -flto
COMPILE = avr-gcc -Wall -Os -g -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) -x c++ -Ilib -Iinclude  $(LIBS) main.c
CLEAN   = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.s

var_dir=d:\Programs\avr-gcc\bin

export PATH := $(var_dir):$(PATH)


all: buildbin buildhex buildasm size

buildasm:
	@echo "Disassembling to $(TARGET).asm ..."
	@avr-objdump -d $(TARGET).elf > $(TARGET).asm

	@echo "Building $(TARGET).hex ..."
	@avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

buildhex:
	@echo "Building $(TARGET).hex ..."
	@avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

buildbin:
	@echo "Building $(TARGET).elf for $(DEVICE) @ $(CLOCK)Hz ..."
	@$(COMPILE) -o $(TARGET).elf

size:
	@echo "FLASH: $(shell avr-size -d $(TARGET).elf | awk '/[0-9]/ {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell avr-size -d $(TARGET).elf | awk '/[0-9]/ {print $$2 + $$3}') bytes"

upload:	hex
	@echo "Uploading to $(DEVICE) ..."
	@$(AVRDUDE) -U flash:w:$(TARGET).hex:i

fuses:
	@echo "Burning fuses of $(DEVICE) ..."
	@$(AVRDUDE) -U lfuse:w:$(LFUSE):m  -U hfuse:w:$(HFUSE):m
