#include <stdarg.h>
#include <terminal.h>
#include <input.h>
#include <fs.h>

// TODO Implement scrolling

// VGA framebuffer
// Mapped to 0xC03FF000 by loader
char* fb = (char*) 0xC03FF000;

void DrawChar(int x, int y, char c, int color) {
    int start = 2 * x + 160 * y;
    fb[start] = c;
    fb[start + 1] = color;
}

// If len is negative write until null terminator
void Write(char* buf, int len, int color) {
    char* at = fb + 2 * CursorX + 160 * CursorY;

    if (len < 0) {
        int i = 0;
        while (buf[i] != '\0') {
            if (buf[i] == '\n') {
                CursorX = 0;
                CursorY++;

				at = fb + 2 * CursorX + 160 * CursorY;
                i++;
                continue;
            }

			at[0] = buf[i];
			at[1] = color;
			
            CursorX++;
			at += 2;
            i++;
        }

        return;
    }

    for (int i = 0; i < len; i++) {
        if (buf[i] == '\n') {
            CursorX = 0;
            CursorY++;

			at = fb + 2 * CursorX + 160 * CursorY;

            i++;
            continue;
        }

		at[0] = buf[i];
		at[1] = color;

		CursorX++;

		at += 2;
    }
}

void Clear() {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            DrawChar(x, y, ' ', 0x0);
        }
    }
}

void ClearColor(unsigned short color) {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            DrawChar(x, y, ' ', color);
        }
    }
}

void reverse(char* str, int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char t = str[start];
		
		str[start] = str[end];

		str[end] = t;

        start++;
        end--;
    }
}
  
// Implementation of itoa copied from google
char* itoa(int num, char* str, int base)
{
    int i = 0;
    char isNegative = 0;
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
  
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
  
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
  
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
  
    str[i] = '\0'; // Append string terminator
  
    // Reverse the string
    reverse(str, i);
  
    return str;
}

// Same thing but for unsigned integers
char* uitoa(unsigned int num, char* str, int base)
{
    int i = 0;
    char isNegative = 0;
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
  
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (base == 10)
    {
        isNegative = 1;
        num = -num;
    }
  
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
  
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
  
    str[i] = '\0'; // Append string terminator
  
    // Reverse the string
    reverse(str, i);
  
    return str;
}

// printf implementation
void kprintf(char* format, ...) {

   va_list valist;
   int i = 0;

   va_start(valist, format);

   while (format[i] != '\0') {

	   if (format[i] == '%') {
		   char str[20];

		   if (format[i + 1] == 'c') {
			    char c = (char) va_arg(valist, int);

				Write(&c, 1, 0x000F);
			}

			if (format[i + 1] == 'i') {
				itoa(va_arg(valist, int), str, 10);

				Write(str, -1, 0x000F);
			}

			if (format[i + 1] == 'x' || format[i + 1] == 'X') {
				uitoa(va_arg(valist, unsigned int), str, 16);

				Write(str, -1, 0x000F);
			}

			if (format[i + 1] == 'p') {
				uitoa(va_arg(valist, unsigned int), str, 16);

				Write(str, -1, 0x000F);
			}

			if (format[i + 1] == 's') {
				Write(va_arg(valist, char*), -1, 0x000F);
			}

			if (format[i + 1] == '\0')
				break;

			i += 2;

			continue;
	   }
	   
	   Write(format + i, 1, 0x000F);

	   i++;
   }
	
   /* clean memory reserved for valist */
   va_end(valist);
}

char prompt = 0;

int Len(char* ch) {
    int len = 0;
    
    while (ch[len] != '\0') {
        len++;
    }
    
    return len;
}

char Cmp(char* one, char* two, int n) {
    char eq = 1;
    
    for (int i = 0; i < n; i++) {
        if (one[i] != two[i]) {
            eq = 0;
        }
    }
    
    return eq;
}

char lineready = 0;

// TODO Implement backspace
void Term_Hook(char c) {
    kprintf("%c", c);

    if (c == '\n') {
        lineready = 1;
    }
}

void Term_Refresh() {
	if (! lineready)
		return;
	
	lineready = 0;

	char* cmd = ReadLine();

	if (Cmp("clear", cmd, 5)) {
		Clear();

		CursorX = 0;
		CursorY = 0;

		kprintf("NewOS (Use 'help' for a list of commands)\n");
	}

	if (Cmp("echo", cmd, 4)) {
		kprintf("%s\n", cmd + 5);
	}

	// System calls test
	if (Cmp("sys", cmd, 3)) {
		__asm__ ("mov $3, %eax");

		__asm__ ("mov %0,  %%ecx" :: "r" (cmd) : "%ecx");
		
		__asm__ ("int $128");

		__asm__("mov $1, %%eax" ::: "%eax");

		__asm__("int $128");
	}

	// Filesystem data
	if (Cmp("fs", cmd, 2)) {
		FS_Debug();
	}

	// TODO implement good string functions
	if (Cmp("read", cmd, 4)) {
		// TODO Find file size before starting read
		char* buf = (char*) kalloc(200);

		int read = FS_ReadBuf(cmd + 5, buf, 200);

		buf[read] = 0;

		kprintf("%s\n", buf);
	}

	// Run flat binary
	// TODO Load ELF binaries
	// TODO Enter user mode
	if (Cmp("run", cmd, 3)) {
		// TODO Find executable size dynamically
		char* exe = (char*) kalloc(200);

		FS_ReadBuf(cmd + 4, exe, 200);

		char* heap = (char*) kalloc(100);

		Execute(exe + 1, heap);
	}

	if (Cmp("help", cmd, 4)) {
		kprintf("clear - Clears the screen\necho <text> - Prints <text>\nfs - Prints filesytem debug info\nread <name> - Outputs the file's contents\nrun <name> - Executes program loaded in file <name>\n\n");
	}

	kprintf("NewOS> ");
}

void Term_Init() {
	kprintf("NewOS (Use 'help' for a list of commands)\nNewOS> ");

	KeyHook = Term_Hook;
}
