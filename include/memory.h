#include <multiboot.h>

typedef struct {
	char Present;
	char Write;
	char Supervisor;
	char Write_Through;
	char Cache_Disabled;
	char Accessed;
	char Dirty;
	char Zero;
	char Global;
	short Unused;
} Page_Table_Flags;

typedef struct {
		Page_Table_Flags Flags;
		unsigned int Address;
} Page_Table_Entry;

typedef struct {
	char Present;
	char Write;
	char Supervisor;
	char Write_Through;
	char Cache_Disabled;
	char Accessed;
	char Zero;
	char Large_Pages;
	char Global;
	short Unused;
} Page_Directory_Flags;

typedef struct {
		Page_Directory_Flags Flags;
		unsigned int* Table;
} Page_Directory_Entry;

extern unsigned int boot_page_directory[1024];

extern unsigned int boot_page_table1[1024];

void Mem_Map_Page(unsigned int vaddr, unsigned int addr);

void Mem_Free_Page(unsigned int vaddr);

void Mem_Init(multiboot_info_t* minfo);

void Mem_Init_Heap();

typedef struct {
	unsigned char* Data;
	int Size;
} MemoryPool;

typedef struct {
	int TotalUsed;
	int MaxSize;

	MemoryPool* UsedRegions;

	unsigned char* Data;
} Heap;

void* kalloc(int size);