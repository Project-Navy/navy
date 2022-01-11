CONFIG_ARCH ?= x86_64

CC = clang
CFLAGS =                            	\
    -std=gnu2x                      	\
    -ffreestanding                      \
    -fno-stack-protector            	\
    -fno-pic                        	\
    -m64                            	\
    -g                              	\
    -Os                                 \
	-mno-80387                          \
	-mno-mmx                            \
	-mno-3dnow                          \
	-mno-sse                            \
	-mno-sse2                           \
	-mno-red-zone                       \
	-mcmodel=kernel                     \
	-Ithird-party/stivale				\
	-Isources/							\
    -Isources/arch/$(CONFIG_ARCH)       \
    -Isources/libs                      \
    -Isources/libs/ansi                 \
    -Ithird-party                       \
    -Wall		                		\
    -Wextra		                		\
    -Werror		                		\
    -MD							        \
    -D__$(CONFIG_ARCH)__=1              \

KFLAGS = $(CFLAGS) -DKERNEL=1

LD = ld.lld
LDFLAGS :=                  				    \
    -nostdlib               				    \
    -static                 				    \
    -Tsources/arch/$(CONFIG_ARCH)/link.ld	    \
    -z max-page-size=0x1000 				    \

ALDFLAGS :=                                     \
    -nostdlib                                   \
    -Tsources/arch/$(CONFIG_ARCH)/app_link.ld   \

KERNEL = kernel.elf
TARGETS = $(KERNEL)

SRC = $(wildcard sources/libs/liballoc/*.c)     \
      $(wildcard sources/kernel/*.c)            \
      sources/libs/ansi/string.c                \
      sources/libs/ansi/stdlib.c                \
      sources/libs/ansi/math.c                  \
      sources/libs/ansi/assert.c                \
      sources/libs/navy/fmt.c           \
      sources/libs/navy/vec.c           \
      sources/libs/navy/handover.c      \
      sources/libs/navy/itoa.c

BUILD_DIRECTORY := build
APP_BUILD_DIRECTORY := build/pkg
DIRECTORY_GUARD = @mkdir -p $(@D)
SYSROOT = sysroot

include sources/pkg/build.mk
include sources/arch/$(CONFIG_ARCH)/build.mk

OBJ := $(patsubst %.c, $(BUILD_DIRECTORY)/%.c.o, $(SRC)) \
        $(patsubst %.s, $(BUILD_DIRECTORY)/%.s.o, $(ASRC))


$(BUILD_DIRECTORY)/%.c.o: %.c
	@$(DIRECTORY_GUARD)
	@echo "[CC] $<"
	@$(CC) $(KFLAGS) -c $< -o $@

$(APP_BUILD_DIRECTORY)/%.c.o: %.c
	@$(DIRECTORY_GUARD)
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIRECTORY)/%.s.o: %.s
	@$(DIRECTORY_GUARD)
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

$(KERNEL): $(OBJ)
	@$(DIRECTORY_GUARD)
	@echo "[LD] $^"
	@$(LD) $(LDFLAGS) -o $@ $^
	nm kernel.elf -S > symbols.map

clean:
	rm -r $(BUILD_DIRECTORY)
	rm $(TARGETS)

all: $(TARGETS)

.PHONY: all run run-debug
.DEFAULT_GOAL := all
