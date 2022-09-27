nasm -f bin Program.s -o fsgen/program

nasm -f elf32 Loader.s -o bin/Loader.o

cd bin

gcc -c ../src/*.c -fno-pic -m32 -nostdlib -ffixed-ecx -fno-builtin -fno-stack-protector -nostartfiles -ffreestanding -nodefaultlibs -W -I../include

cd ..

ld -T Link.ld -melf_i386 bin/*.o -o ./iso/boot/kernel

 mkisofs -R                              \
                -b boot/grub/stage2_eltorito    \
                -no-emul-boot                   \
                -boot-load-size 4               \
                -A os                           \
                -input-charset utf8             \
                -quiet                          \
                -boot-info-table                \
                -o os.iso                       \
                iso

#cd qemu

# qemu-system-x86_64 -kernel ./iso/boot/kernel

rm *.lock

bochs -q -rc start.rc