all: exos.bin

exos.bin: kernel.o boot.o
	i586-elf-gcc -T linker.ld -o exos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel.o:
	i586-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o:
	i586-elf-as boot.s -o boot.o

clean:
	rm *.o
	rm exos.bin
