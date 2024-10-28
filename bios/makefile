AS = vasmm68k_psi-x
CC = m68k-elf-gcc
OBJCOPY = m68k-elf-objcopy
ROMFIX = ./tools/romfix

OBJDIR = obj
INCDIR = inc
SRCDIR = src
GFXDIR = gfx
ROMDIR = rom
LINKER_SCRIPT = md.ld

ASFLAGS = -Felf -I $(INCDIR)
CFLAGS = -m68000 -Wall -Wextra -Wno-shift-negative-value -Wno-main -Wno-unused-parameter -fno-builtin -fms-extensions -I$(INCDIR) -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -flto -MMD -c
LDFLAGS = -nostartfiles -n -T $(LINKER_SCRIPT) -Wl,--gc-sections -flto -L /opt/toolchains/m68k-elf/m68k-elf/lib -lnosys -L /opt/toolchains/m68k-elf/lib/gcc/m68k-elf/13.1.0 -lgcc

ASM_SRCS = $(wildcard $(SRCDIR)/*.68k) $(wildcard $(GFXDIR)/*.68k)
C_SRCS = $(wildcard $(SRCDIR)/*.c)

ASM_OBJS = $(patsubst %.68k, $(OBJDIR)/%.o, $(ASM_SRCS))
C_OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(C_SRCS))

# both asm and c object collection
OBJS = $(ASM_OBJS) $(C_OBJS)

# binary exec
TARGET = $(ROMDIR)/nihil.elf
TARGET_ROM = $(ROMDIR)/nihil.bin

# default target
all: $(OBJDIR) $(TARGET)

# create build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# assemble
$(OBJDIR)/%.o: %.68k
	$(AS) $(ASFLAGS) -o $@ $<

# compile
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

# link
$(TARGET): $(OBJS)
	$(CC) -m68000 $(LDFLAGS) $(OBJS) -o $(TARGET)
	$(OBJCOPY) -O binary $(TARGET) $(TARGET_ROM)
	#$(ROMFIX) $(TARGET_ROM)

# clean
clean:
	rm -rf $(OBJDIR)
	mkdir -p obj/src obj/gfx

run:
	../maria/maria rom/nihil.bin
runb:
	/opt/toolchains/m68k-elf/emu/blastem rom/nihil.bin &> /dev/null &
runm:
	mame genesis -cart rom/nihil.bin -window -resolution 640x480 -debug -nofilter

.PHONY: all clean run runb runm
