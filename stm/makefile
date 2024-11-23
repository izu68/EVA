CFLAGS  ?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
            -Wformat-truncation -fno-common -Wconversion \
            -g3 -O0 -ffunction-sections -fdata-sections -I. \
            -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 $(EXTRA_CFLAGS)
LDFLAGS ?= -Teva.ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map
SOURCES = boot.c 

build: eva.elf eva.bin

eva.elf: $(SOURCES)
	arm-none-eabi-gcc $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $@

eva.bin: eva.elf
	arm-none-eabi-objcopy -O binary $< $@

debug:
	openocd -f openocd/stlink-v2.cfg -f openocd/stm32h7x_dual_bank.cfg &> /dev/null &

connect:
	telnet localhost 4444

clean:
	rm *.elf* eva.bin
