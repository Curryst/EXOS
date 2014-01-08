all: exos.bin

exos.bin: main.o idt.o gdt.o boot.o
	i586-elf-gcc -T linker.ld -o exos.bin -ffreestanding -O2 -nostdlib boot.o main.o idt.o gdt.o -lgcc

main.o:
	i586-elf-gcc -c main.c -o main.o -std=gnu99 -ffreestanding -I./include -O2 -Wall -Wextra

idt.o:
	i586-elf-gcc -c idt.c -o idt.o -std=gnu99 -ffreestanding -I./include -O2 -Wall -Wextra

gdt.o:
	i586-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -I./include -O2 -Wall -Wextra

boot.o:
	i586-elf-as boot.s -o boot.o

clean:
	rm *.o
	rm exos.bin
