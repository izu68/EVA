CFLAGS  ?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
            -Wformat-truncation -fno-common -Wconversion \
            -g3 -Os -ffunction-sections -fdata-sections -I. \
            -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 $(EXTRA_CFLAGS)
LDFLAGS ?= -Teva.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = boot.c 

build: eva.elf

eva.elf: $(SOURCES)
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@

eva.bin: eva.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: eva.bin
	dfu-util -a 0 -s 0x08000000 -D $<

dump_sram:
	dfu-util -a 0 -s 0x38800000:0xFFF -U dump_sram.bin

clean:
	rm *.elf* eva.bin
