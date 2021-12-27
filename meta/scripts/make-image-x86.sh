#!/bin/sh

set -v

path="$(dirname $0)/../../"
mkdir -p tmp/bin tmp/boot

if [ -f $path/navy.iso ]; then 
	rm $path/navy.iso 
fi

cp $path/kernel.elf ./tmp/boot/ || exit 1
cp $path/sysroot/boot/limine.cfg ./tmp/boot || exit 1
cp $path/symbols.map ./tmp/boot/ || exit 1
cp $path/third-party/limine/limine.sys ./tmp/boot || exit 1
cp $path/third-party/limine/*.bin ./tmp/boot || exit 1

xorriso -as mkisofs -b /boot/limine-cd.bin -no-emul-boot -boot-load-size $(nproc --all) \
	-boot-info-table --efi-boot /boot/limine-eltorito-efi.bin -efi-boot-part --efi-boot-image \
	--protective-msdos-label ./tmp -o $path/navy.iso || exit 1

$path/third-party/limine/limine-install-linux-x86_64  $path/navy.iso || exit 1
ls -R ./tmp/
rm -rf ./tmp
