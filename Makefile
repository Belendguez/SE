#TOOLCHAIN=~/toolchain/gcc-arm-none-eabi-4_9-2014q4/bin
#PREFIX=$(TOOLCHAIN)/arm-none-eabi-
PREFIX=arm-none-eabi-

INCLUDE_DIR := includes
DRIVERS_DIR := drivers
SRC_DIR := .

ARCHFLAGS=-mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z256VLL4
COMMONFLAGS=-g -Og -Wall -Werror $(ARCHFLAGS)

CFLAGS=-I$(INCLUDE_DIR) $(COMMONFLAGS)
LD_LEDFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(TARGET_LED).map,-Tlink.ld

LD_HELLOFLAGS=$(COMMONFLAGS) --specs=nano.specs -Wl,--gc-sections,-Map,$(TARGET_HELLO).map,-Tlink.ld

LDLIBS=

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
SIZE=$(PREFIX)size
RM=rm -f

TARGET_LED=led_blinky
TARGET_HELLO=hello_world

SRC_LED=$(wildcard $(SRC_DIR)/led_blinky.c $(SRC_DIR)/$(DRIVERS_DIR)/*.c)
OBJ_LED=$(patsubst %.c, %.o, $(SRC_LED))

SRC_HELLO=$(wildcard $(SRC_DIR)/hello_world.c $(SRC_DIR)/$(DRIVERS_DIR)/*.c)
OBJ_HELLO=$(patsubst %.c, %.o, $(SRC_HELLO))

all: build_led build_hello
build_led: elf1 srec1 bin1
elf1: $(TARGET_LED).elf
srec1: $(TARGET_LED).srec
bin1: $(TARGET_LED).bin

build_hello: elf2 srec2 bin2
elf2: $(TARGET_HELLO).elf
srec2: $(TARGET_HELLO).srec
bin2: $(TARGET_HELLO).bin



clean: clean_led clean_hello

clean_led:
	$(RM) $(TARGET_LED).srec $(TARGET_LED).elf $(TARGET_LED).bin $(TARGET_LED).map $(OBJ_LED)
	
clean_hello:
	$(RM) $(TARGET_HELLO).srec $(TARGET_HELLO).elf $(TARGET_HELLO).bin $(TARGET_HELLO).map $(OBJ_HELLO)


$(TARGET_LED).elf: $(OBJ_LED)
	$(LD) $(LD_LEDFLAGS) $(OBJ_LED) $(LDLIBS) -o $@
	
$(TARGET_HELLO).elf: $(OBJ_HELLO)
	$(LD) $(LD_HELLOFLAGS) $(OBJ_HELLO) $(LDLIBS) -o $@

%.srec: %.elf
	$(OBJCOPY) -O srec $< $@

%.bin: %.elf
	    $(OBJCOPY) -O binary $< $@

size:
	$(SIZE) $(TARGET).elf

flash_led: build_led
	openocd -f openocd.cfg -c "program $(TARGET_LED).elf verify reset exit"
	
flash_hello: build_hello
	openocd -f openocd.cfg -c "program $(TARGET_HELLO).elf verify reset exit"
