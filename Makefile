MCU_FAMILY = mcs51
MCU = at89c2051
CHIP_PACKAGE = DIP20

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########     Won't need to change if they're in your PATH     ##########
##########------------------------------------------------------##########

CC = sdcc
MINIPRO = minipro

## The name of your project (without the .c)
# TARGET = simon
## Or name it automatically after the enclosing directory
TARGET = $(lastword $(subst /, ,$(CURDIR)))

# Object files: will find all .c/.h files in current directory
#  and in LIBDIR.  If you have any other (sub-)directories with code,
#  you can add them in to SOURCES below in the wildcard statement.
SOURCES=$(wildcard *.c $(LIBDIR)/*.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

TARGET_ARCH = -m$(MCU_FAMILY) -p$(MCU)

%.ihx: %.c Makefile
	$(CC) $(TARGET_ARCH) $<;

## These targets don't have files named after them
.PHONY: all clean flash

all: $(TARGET).ihx

debug:
	@echo
	@echo "Target:" $(TARGET)
	@echo "Source files:" $(SOURCES)
	@echo "Header files:" $(HEADERS)
	@echo "MCU, MCU_FAMILY, BAUD:"  $(MCU), $(MCU_FAMILY)
	@echo

clean:
	rm -f $(TARGET).elf $(TARGET).ihx $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom $(TARGET).asm \
	$(TARGET).lk $(TARGET).mem $(TARGET).rel $(TARGET).rst

flash: $(TARGET).ihx
	$(MINIPRO) -p $(MCU)@$(CHIP_PACKAGE) -w $<