#:/bin/bash
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

nasm bootload.asm
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc seek=0

bcc -ansi -c -o kernel.o kernel.c
as86 kernel.asm -o kernel_asm.o
as86 lib.asm -o lib.o

gcc -o loadFile loadFile.c

bcc -ansi -c -o userlib.o userlib.c
bcc -ansi -c -o uprog1.o uprog1.c
bcc -ansi -c -o uprog2.o uprog2.c
bcc -ansi -c -o shell.o shell.c

ld86 -o uprog1 -d uprog1.o lib.o
ld86 -o uprog2 -d uprog2.o lib.o
ld86 -o shell -d shell.o userlib.o lib.o
ld86 -o kernel -d kernel.o kernel_asm.o userlib.o

./loadFile uprog1
./loadFile uprog2
./loadFile message.txt
./loadFile shell

dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 conv=notrunc seek=30