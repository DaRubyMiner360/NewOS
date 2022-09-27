typedef struct {
	char* Buffer;
	int Position;
	int Size;
	int MaxSize;
} Queue;

Queue Queue_Allocate(int size);
void Queue_Push(Queue* que, char byte);
char Queue_Next(Queue* que);

char Str_Cmp(char* a, char* b);
char Str_SCmp(char* a, char* b, int count);