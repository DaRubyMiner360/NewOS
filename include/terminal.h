unsigned short CursorX;
unsigned short CursorY;

void Write(char* buf, int len, int color);

void Clear();

void ClearColor(unsigned short color);

void DrawChar(int x, int y, char c, int color);

void kprintf(char* format, ...);

void Term_Init();

void Term_Refresh();