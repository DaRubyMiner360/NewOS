#define uint32_t unsigned int
#define uint16_t unsigned short

#ifndef KERNEL
#define KERNEL

#define KERNEL_BASE 0xC0000000

#define halt() asm ("hlt")

extern void Execute(unsigned int addr, unsigned int heap);

typedef struct
{
	unsigned int limit_low:16;
	unsigned int base_low : 24;
     //attribute byte split into bitfields
	unsigned int accessed :1;
	unsigned int read_write :1; //readable for code, writable for data
	unsigned int conforming_expand_down :1; //conforming for code, expand down for data
	unsigned int code :1; //1 for code, 0 for data
	unsigned int always_1 :1; //should be 1 for everything but TSS and LDT
	unsigned int DPL :2; //priviledge level
	unsigned int present :1;
     //and now into granularity
	unsigned int limit_high :4;
	unsigned int available :1;
	unsigned int always_0 :1; //should always be 0
	unsigned int big :1; //32bit opcodes for code, uint32_t stack for data
	unsigned int gran :1; //1 to use 4k page addressing, 0 for byte addressing
	unsigned int base_high :8;
} __attribute((packed)) gdt_entry_bits;

typedef struct tss_entry_struct
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // everything below here is unusued now.. 
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         
   uint32_t cs;        
   uint32_t ss;        
   uint32_t ds;        
   uint32_t fs;       
   uint32_t gs;         
   uint32_t ldt;      
   uint16_t trap;
   uint16_t iomap_base;
} __attribute((packed)) tss_entry;

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH 14
#define FB_LOW 15

typedef struct {
        unsigned short size;
        unsigned int address;
} __attribute__((packed)) gdt;

typedef struct {
    unsigned short limit;
    unsigned short base0_15;
    unsigned char base16_23;
    unsigned char access;
    unsigned char flags;
    unsigned char base24_31;
} segment;

typedef struct {
    unsigned short offset_1;
    unsigned char type;
    unsigned char reserved;
    unsigned short segment;
    unsigned short offset_2;
} __attribute__((packed)) idtentry;

unsigned char inb();
char ink();
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short data);
void outb(unsigned short port, unsigned char data);

void* gdtstart;

void loadgdt(gdt table);
void load_idt(void* idt);
segment Make_Segment(unsigned int address, unsigned int size, unsigned char access);

void idt_init(void);

#endif