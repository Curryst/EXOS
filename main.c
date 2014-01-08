#include <stddef.h>
#include <stdint.h>
#include <system.h>

#if defined(__linux__)
#error "You are not using a cross-compiler"
#endif

#if !defined(__i386__)
#error "This need to be compiled with an ix86-elf compiler"
#endif

struct terminal_session
{
	size_t row;
	size_t column;
	uint8_t color;
	uint16_t* buffer;
	int escape;
};

struct GDT
{
	int limit;
	uint8_t base;
	uint8_t type;
};

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

void *memcpy(void *dest, const void *src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, size_t count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

size_t strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

struct terminal_session term;

void terminal_initialize()
{
	term.row = 0;
	term.column = 0;
	term.color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	term.buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			term.buffer[index] = make_vgaentry(' ', term.color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	term.color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	term.buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	if ( c == '\n' )
	{
		term.column = 0;
		term.row++;
		return;
	}
	else {
		terminal_putentryat(c, term.color, term.column, term.row);
		if ( ++term.column == VGA_WIDTH )
		{
			term.column = 0;
			if ( ++term.row == VGA_HEIGHT )
			{
				term.row = 0;
			}
		}
	}
}

void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		terminal_putchar(data[i]);
}

void kernel_main()
{
	gdt_install();
	idt_install();
	terminal_initialize();
	terminal_writestring("Hello, kernel world!\n");
}
