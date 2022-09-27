#include <kernel.h>
#include <memory.h>
#include <multiboot.h>

// TODO memory functions memcpy, memset, etc
// TODO better memory allocation algorithm
// TODO ability to free memory

// TODO Dynamically allocate page tables
// Only the first 8 megabytes can be mapped
static unsigned int Table[1024] __attribute__((aligned(4096)));

unsigned int PageToInt(Page_Table_Entry entry) {
	unsigned int i = entry.Address;

	i |= entry.Flags.Present;
	i |= entry.Flags.Write << 1;
	i |= entry.Flags.Supervisor << 2;
    
    return i;
}

Page_Table_Entry IntToPage(unsigned int data) {
	Page_Table_Entry entry;

	entry.Address = data & 0xFFF000;

	entry.Flags.Present = data & 1 > 0;
	entry.Flags.Write = data & 2 > 0;
    entry.Flags.Supervisor = data & 3 > 0;
    
    return entry;
}

unsigned int PageDirToInt(Page_Directory_Entry entry) {
	unsigned int i = (unsigned int) entry.Table;

	i = i << 12;

	i |= entry.Flags.Present;
	i |= entry.Flags.Write << 1;
    
    return i;
}

Page_Directory_Entry IntToPageDir(unsigned int data) {
	Page_Directory_Entry entry;

	entry.Table = (unsigned int*) (data & 0xFFF000) + KERNEL_BASE;

	entry.Flags.Present = data & 1 > 0;
	entry.Flags.Write = data & 2 > 0;
    entry.Flags.Supervisor = data & 3 > 0;
	entry.Flags.Write_Through = data & 4 > 0;
	entry.Flags.Cache_Disabled = data & 5 > 0;
	entry.Flags.Accessed = data & 6 > 0;
	entry.Flags.Zero = data & 7 > 0;
	entry.Flags.Large_Pages = data & 8 > 0;
	entry.Flags.Global = data & 9 > 0;
	//entry.Flags.Unused = data & (0xFF << 10) > 0;
    
    return entry;
}

Page_Table_Entry Mem_Get_Page(unsigned int vaddr) {
	int directory = vaddr / 0x400000;

	int page = 0;

	unsigned int curaddr = 0x400000 * directory;

	while (curaddr < vaddr) {
		curaddr += 0x1000;

		page++;
	}

	Page_Directory_Entry dir = IntToPageDir(boot_page_directory[directory]);

	// dir.Flags.Present = 1;
	// dir.Flags.Write = 1;
	// dir.Flags.Supervisor = 1;

	// Error if its not present

	Page_Table_Entry tbl = IntToPage(dir.Table[page]);

	return tbl;
}

// Map a virtual address to a physical address
// Addresses must be 4KB aligned
void Mem_Map_Page(unsigned int vaddr, unsigned int addr) {
	int directory = vaddr / 0x400000;

	int page = 0;

	unsigned int curaddr = 0x400000 * directory;

	while (curaddr < vaddr) {
		curaddr += 0x1000;

		page++;
	}

	if (directory == 0) {
		Page_Table_Entry tbl = IntToPage(boot_page_table1[page]);

		tbl.Address = addr;

		tbl.Flags.Present = 1;
		tbl.Flags.Write = 1;
		// tbl.Flags.Supervisor = 1;

		boot_page_table1[page] = PageToInt(tbl);

		return;
	}

	Page_Directory_Entry dir = IntToPageDir(boot_page_directory[directory]);

	dir.Flags.Present = 1;
	dir.Flags.Write = 1;
	// dir.Flags.Supervisor = 1;

	dir.Table = (Table - KERNEL_BASE);

	boot_page_directory[directory] = PageDirToInt(dir);

	Page_Table_Entry tbl = IntToPage(Table[page]);

	tbl.Address = addr;

	tbl.Flags.Present = 1;
	tbl.Flags.Write = 1;
	// tbl.Flags.Supervisor = 1;

	Table[page] = PageToInt(tbl);
}

// Mark a page as not present
void Mem_Free_Page(unsigned int vaddr) {
	int directory = vaddr / 0x400000;

	int page = 0;

	unsigned int curaddr = 0x400000 * directory;

	while (curaddr < vaddr) {
		curaddr += 0x1000;

		page++;
	}

	Page_Directory_Entry dir = IntToPageDir(boot_page_directory[directory]);

	// dir.Flags.Present = 1;
	// dir.Flags.Write = 1;
	// dir.Flags.Supervisor = 1;

	// Error if its not present

	Page_Table_Entry tbl = IntToPage(dir.Table[page]);

	tbl.Flags.Present = 0;

	dir.Table[page] = PageToInt(tbl);

	boot_page_directory[directory] = PageDirToInt(dir);
}

// Regions of available memory
MemoryPool MemoryMap[32];

int MemoryMap_Size = 0;

extern char kend;

// Parse available memory
void Mem_Init(multiboot_info_t* minfo) {
	int i;

    for(i = 0; i < minfo->mmap_length; 
        i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = 
            (multiboot_memory_map_t*) (minfo->mmap_addr + i);
 
        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
			unsigned int kernelend = (unsigned int) &kend - KERNEL_BASE + 1;

			if (mmmt->addr + mmmt->len > kernelend) {
				int dif = kernelend - mmmt->addr;

				if (dif < 0)
					dif = -dif;

				MemoryPool pool;

				pool.Data = mmmt->addr + dif;

				pool.Size = mmmt->len - dif;
				
				MemoryMap[MemoryMap_Size++] = pool;
			}
        }
    }
}

char init = 0;

Heap kheap;

// Initialize the kernel's heap
void Mem_Init_Heap() {
	Heap heap;

	heap.Data = MemoryMap[0].Data;

	heap.TotalUsed = 0;

	heap.MaxSize = 0x9000;

	kheap = heap;
}

// Bump allocate memory
void* kalloc(int size) {
	if (! init) {
		Mem_Init_Heap();

		init = 1;
	}

	void* ptr = kheap.Data + kheap.TotalUsed;

	kheap.TotalUsed += size;

	if (kheap.TotalUsed >= kheap.MaxSize) {
		kprintf("HEAP SIZE EXCEEDED\n");
	}

	Page_Table_Entry page = Mem_Get_Page(ptr);

	// If the page isn't present request a new identity mapped page
	if (! page.Flags.Present) {
		Mem_Map_Page(ptr, ptr);
	}

	return ptr;
}