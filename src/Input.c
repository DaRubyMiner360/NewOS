#include <input.h>
#include <memory.h>

// Load keymap
void Load_Keymap() {
    int count = 0;

    scans[0] = ESC;

    for (int i = 1; i <= 11; i++) {
        scans[i] = '0' + i - 1;

        if (i == 11)
            scans[i] = '0';
    }

    scans[12] = '-';
    scans[13] = '=';

    scans[14] = BACKSPACE;

    scans[15] = TAB;

    scans[16] = 'q';
    scans[17] = 'w';
    scans[18] = 'e';
    scans[19] = 'r';
    scans[20] = 't';
    scans[21] = 'y';
    scans[22] = 'u';
    scans[23] = 'i';
    scans[24] = 'o';
    scans[25] = 'p';
    scans[26] = '[';
    scans[27] = ']';

    scans[28] = ENTER;
    scans[29] = CTRL;

    scans[30] = 'a';
    scans[31] = 's';
    scans[32] = 'd';
    scans[33] = 'f';
    scans[34] = 'g';
    scans[35] = 'h';
    scans[36] = 'j';
    scans[37] = 'k';
    scans[38] = 'l';
    scans[39] = ';';
    scans[40] = '\'';
    scans[41] = '`';

    scans[42] = LSHIFT;

    scans[43] = '\\';

    scans[44] = 'z';
    scans[45] = 'x';
    scans[46] = 'c';
    scans[47] = 'v';
    scans[48] = 'b';
    scans[49] = 'n';
    scans[50] = 'm';
    scans[51] = ',';
    scans[52] = '.';
    scans[53] = '/';

    scans[54] = RSHIFT;

    scans[55] = PRNTSCN;
    scans[56] = ALT;

    scans[57] = ' ';
    scans[58] = CAPS;
}

char ReadKey() {
    if (keybuffer.Size == 0)
		return 0;

    char c = Queue_Next(&keybuffer);

    return c;
}

char* ReadLine() {
    char* line = (char*) kalloc(100);

    int cur = 0;

    char c = ReadKey();

    while (c != '\n' && keybuffer.Size > 0) {
		if (c == 0)
			break;
		
        line[cur++] = c;

        c = ReadKey();
    }

    line[cur] = '\0';
	
    return line;
}