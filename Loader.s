    global loader
	global ink
    global inb
    global outb
	global inw
	global outw
    global loadgdt
    global gdtstart
    global loadidt
    global divzero
    global paging_enable
    global paging_disable

    bits 32

	  STACK_SIZE equ 4096
    MODULE_ALIGN equ 0x1

    MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
    FLAGS        equ 0x0            ; multiboot flags
    CHECKSUM     equ -(MAGIC_NUMBER + MODULE_ALIGN)  ; calculate the checksum
    ;VM_BASE     equ 0xC0000000
    ;PDE_INDEX   equ (VM_BASE >> 22)
    ;PSE_BIT     equ 0x00000010
    ;PG_BIT      equ 0x80000000

    section .text
	
    align 4                         ; the code must be 4 byte aligned
        dd MAGIC_NUMBER              
        dd MODULE_ALIGN
        dd CHECKSUM                 ; and the checksum


    times 1024 db 8

    inb:
        mov edx, [esp + 4]
        in al, dx
        ret

    outb:
        xor edx, edx
        xor al, al
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out dx, al
        ret
	
	inw:
		xor al, al
        mov edx, [esp + 4]
        in word ax, dx
        ret

	outw:
        xor edx, edx
        xor al, al
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out word dx, al
        ret

global page_fault
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
global syscall
 
global load_idt

extern pagefault_handler
extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler
extern syscall_handler

;global TEMP_PAGE_DIRECTORY
  ;align 4096
  ;TEMP_PAGE_DIRECTORY:
  ;  dd 0x00000083
  ;  times(PDE_INDEX - 1) dd 0
  ;  dd 0x00000083
  ;  times(4096 - PDE_INDEX - 1) dd 0 

global boot_page_directory;

global boot_page_table1;

section .pt:
	align 4096
boot_page_directory:
	times(4096) dd 0
boot_page_table1:
	times(4096) dd 0

times(4096 * 100) dd 0

section .text:

page_fault:
  pushad
  call pagefault_handler
  popad
  iret

irq0:
  pushad
  call irq0_handler
  popa
  iret
 
irq1:
  pushad
  call irq1_handler
  popa
  iret
 
irq2:
  pushad
  call irq2_handler
  popa
  iret
 
irq3:
  pushad
  call irq3_handler
  popa
  iret
 
irq4:
  pushad
  call irq4_handler
  popa
  iret
 
irq5:
  pushad
  call irq5_handler
  popa
  iret
 
irq6:
  pushad
  call irq6_handler
  popa
  iret
 
irq7:
  pushad
  call irq7_handler
  popa
  iret
 
irq8:
  pushad
  call irq8_handler
  popa
  iret
 
irq9:
  pushad
  call irq9_handler
  popa
  iret
 
irq10:
  pushad
  call irq10_handler
  popa
  iret
 
irq11:
  pushad
  call irq11_handler
  popa
  iret
 
irq12:
  pushad
  call irq12_handler
  popa
  iret
 
irq13:
  pushad
  call irq13_handler
  popa
  iret
 
irq14:
  pushad
  call irq14_handler
  popa
  iret
 
irq15:
  pushad
  call irq15_handler
  popa
  iret

syscall:
  push ecx
  push eax
  call syscall_handler
  pop eax
  pop ecx
  iret

  global load_page_directory
  load_page_directory:
		pop ebp
  		mov ecx, ebp
        mov  cr3, ecx

        ; Enable paging and the write-protect bit.
        mov  ecx, cr0
        or  ecx, 080010000h
        mov  cr0,ecx

		ret
 
load_idt:
    cli
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

    loadgdt:
        lgdt [esp + 4]
        
        mov eax, cr0
        or eax, 1
        mov cr0, eax

        jmp 0x08:loadss
        loadss:
        ret
		
        .loop:
            hlt
            jmp .loop

    global Execute
    Execute:
	  pushad
	  
	  pop edx
	  pop eax

	  
	  push edx
      call eax

	  popad

	  ret

    global shrtptr
    shrtptr:
      mov eax, dword [esp + 4]
      ret

	extern kstart
	extern kend

	loader:
		push ebx
		push eax
		
 		mov edi, boot_page_table1 - 0xC0000000
		 
        mov  esi,0
        mov  ecx,1023

.l1: 
        ; Only map the kernel.
        cmp  esi, kstart
        jl short .l2
		cmp esi, kend - 0xC0000000
        jge short .l3

        ; Map physical address as "present, writable". Note that this maps
        ; .text and .rodata as writable. Mind security and map them as non-writable.
        mov  edx,esi
        or  edx,0003h
        mov  [edi],edx

.l2: 
        ; Size of page is 4096 bytes.
        add  esi,4096
        ; Size of entries in boot_page_table1 is 4 bytes.
        add  edi,4
        ; Loop to the next entry if we havent finished.
        loop .l1

.l3: 
        ; Map VGA video memory to 0xC03FF000 as "present, writable".
		mov dword [boot_page_table1 - 0xC0000000 + 1023 * 4], 0x000B8003

		mov dword [boot_page_directory - 0xC0000000 + 0], boot_page_table1 - 0xC0000000 + 0x003 
		mov dword [boot_page_directory - 0xC0000000 + 768 * 4], boot_page_table1 - 0xC0000000 + 0x003

        ; Set cr3 to the address of the boot_page_directory.
		mov ecx, boot_page_directory - 0xC0000000
        mov  cr3, ecx

        ; Enable paging and the write-protect bit.
        mov  ecx, cr0
        or  ecx, 080010000h
        mov  cr0,ecx

		lea ecx, [high]
		jmp ecx

    high:
      extern Main  
      mov esp, stack + STACK_SIZE
      push ebx
      call Main
      
    .loop:
        hlt
        jmp .loop

    global exeend
    exeend:

modules:

global stack

section .bss
    global datastart
    stack:
            resb 4096
    global heap
    heap:
        resb 0
    global dataend

    ; RamDisk is last in memory and most likely to run out

    global ramdisk
    ramdisk:


