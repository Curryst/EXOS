.set ALIGN,		1<<0
.set MEMINFO,		1<<1
.set FLAGS,		ALIGN | MEMINFO
.set MAGIC,		0x1BADB002
.set CHECKSUM,		-(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Linker specificies _start as the entry point.  No return value because bootloader is gone.
.section .text
.global _start
.type _start, @function
_start:
	# Point the esp register to the top of our stack
	movl $stack_top, %esp

	# Now call the kernel from C
	call kernel_main

	# In case the function returns, put it in an infinite loop.
	# Use 'cli' to disable interrupts, then 'hlt' to halt the CPU until the next interrupt.
	cli
	hlt
.Lhang:
	jmp .Lhang

.size _start, . - _start
