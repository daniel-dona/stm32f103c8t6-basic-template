PREFIX?=arm-none-eabi-
CC=$(PREFIX)gcc
OBJCOPY=$(PREFIX)objcopy
OD=bin

all: realall.really

SFLAGS= --static -nostartfiles -std=c11 -g3 -Os
SFLAGS+= -fno-common -ffunction-sections -fdata-sections
SFLAGS+= -I./libopencm3/include -L./libopencm3/lib
LFLAGS+=-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

M3_FLAGS= $(SFLAGS) -mcpu=cortex-m3 -mthumb -msoft-float
LFLAGS_STM32=$(LFLAGS) main.c -T ld.stm32.basic

STM32F1_CFLAGS=$(M3_FLAGS) -DSTM32F1 $(LFLAGS_STM32) -lopencm3_stm32f1

BOARDS_ELF+=$(OD)/firmware.elf
BOARDS_BIN+=$(OD)/firmware.bin
BOARDS_HEX+=$(OD)/firmware.hex

$(OD)/firmware.elf: main.c libopencm3/lib/libopencm3_stm32f1.a
	@echo "  stm32f1 -> Creating $(OD)/firmware.elf"
	$(CC) $(STM32F1_CFLAGS) -o $(OD)/firmware.elf

realall.really: outdir $(BOARDS_ELF) $(BOARDS_BIN) $(BOARDS_HEX)

libopencm3/Makefile:
	@echo "Initializing libopencm3 submodule"
	git submodule update --init

libopencm3/lib/libopencm3_%.a: libopencm3/Makefile
	$(MAKE) -C libopencm3

%.bin: %.elf
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

flash:
	st-flash --reset write $(BOARDS_BIN) 0x08000000
	
outdir:
	mkdir -p $(OD)

clean:
	$(RM) $(BOARDS_ELF) $(BOARDS_BIN) $(BOARDS_HEX)
	$(RM) -r $(OD)

.PHONY: realall.really outdir clean all
$(V).SILENT:
