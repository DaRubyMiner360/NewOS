#include <collections.h>

// Definition of special keys
#define ESC 0xF1
#define BACKSPACE 0xF2
#define TAB 0xF3
#define ENTER 0x1C
#define CTRL 0xF5
#define LSHIFT 0xF6
#define RSHIFT 0xF7
#define PRNTSCN 0xF8
#define ALT 0xF9
#define CAPS 0xFA

Queue keybuffer;
short size;

char scans[84];

void Load_Keymap();

char ReadKey();

char* ReadLine();

typedef void (*KeyHandler)(char c);

KeyHandler KeyHook;
