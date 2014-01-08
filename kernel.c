#include <stddef.h>
#include <stdint.h>

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

void encodeGdtEntry(uint8_t *target, struct GDT source)
{
	// Check the limit to make sure it can be encoded
	if ((source.limit > 65536> && (soure.limit & 0xFFF) != 0xFFF)) {
		kerror("You can't do that");
	}
	if (source.limit > 65536) {
		// Adjust granularity if required
		source.limit = source.limit >> 12;
		target[6] = 0xC0;
	} else {
		target[6] = 0x40;
	}

	// Encode the limit
	target[0] = source.limit & 0xFF;
	target[1] = (source.limit >> 8) & 0xFF;
	target[6] |= (source.limit >> 16) & 0xF;

	// Encode the base
	target[2] = source.base & 0xFF;
	target[3] = (source.base >> 8) &0xFF;
	target[4] = (source.base >> 16) & 0xFF;
	target[7] = (source.base >> 24) & 0xFF;

	target[5] = source.type;
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

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
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
	terminal_initialize();
	terminal_writestring("Hello, kernel world!\n");
}
