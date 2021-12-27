SRC += $(wildcard sources/arch/x86_64/*.c) $(wildcard sources/arch/cross-x86/*.c)
ASRC += $(wildcard sources/arch/x86_64/*.s) $(wildcard sources/arch/cross-x86/*.s)

CFLAGS += --target=x86_64-elf-none 
AS = nasm
ASFLAGS = -felf64

run: $(TARGET)
	meta/scripts/make-image-x86.sh
	qemu-system-$(CONFIG_ARCH) -m 4G -serial stdio -cdrom navy.iso -enable-kvm 

run-debug: $(TARGET)
	meta/scripts/make-image-x86.sh
	qemu-system-$(CONFIG_ARCH) -m 4G -serial stdio -cdrom navy.iso -s -S -no-reboot -d int

run-noreboot: $(TARGET)
	meta/scripts/make-image-x86.sh
	qemu-system-$(CONFIG_ARCH) -m 4G -serial stdio -cdrom navy.iso -no-reboot -d int -no-shutdown

image: $(TARGET)
	meta/scripts/make-image-x86.sh
