#include <multiboot.h> // https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot_002eh.html
#include <kernel.h>
#include <memory.h>

#include <terminal.h>

#include <input.h>

#include <fs.h>

multiboot_module_t* modules;

segment Make_Segment(unsigned int address, unsigned int size, unsigned char access) {
    segment seg;

    seg.limit = size;
    seg.base0_15 = (address >> 8 * 2) & 0x000F;
    seg.base16_23 = (address >> 8) & 0x0;
    seg.base24_31 = address & 0x0;
    seg.access = access;
    seg.flags = 0xCF;

    return seg;
}

segment gdttbl[6];

int Main(multiboot_info_t* mbinfo, unsigned int magic) {
	KeyHook = 0;

	keybuffer = Queue_Allocate(500);

    Load_Keymap();
    
    gdt g;
    g.address = (unsigned int) &gdttbl;
    g.size = 8 * 2;
    gdttbl[1] = Make_Segment(0, 0xFFFFFFFF, 0x9A);
    gdttbl[2] = Make_Segment(0, 0xFFFFFFFF, 0x92);

	Mem_Map_Page(0, 0);

	Mem_Map_Page(0x2C000, 0x2C000);

	idt_init();

	loadgdt(g);

	Clear();

    unsigned char mask = inb (0x21);
    outb(0x21, mask & 0xFD); // Enable keyboard interrupt, probably not necessary but just in case

	Mem_Init(mbinfo);

	FS_Init();

	Term_Init();

	while (1) {
		Term_Refresh();
	}
	
    return 0;
}
