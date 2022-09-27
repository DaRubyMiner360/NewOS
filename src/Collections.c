#include <collections.h>
#include <memory.h>

Queue Queue_Allocate(int size) {
	Queue que;

	que.Buffer = (char*) kalloc(size);

	que.MaxSize = size;

	que.Size = 0;

	que.Position = 0;

	return que;
}

void Queue_Push(Queue* que, char byte) {
	que->Buffer[que->Size++] = byte;
}

char Queue_Next(Queue* que) {
	return que->Buffer[que->Position++];
}

char Str_Cmp(char* a, char* b) {
	int i = 0;

	while (a[i] != '\0' && b[i] != '\0') {
		if (a[i] != b[i])
			return 0;
		
		i++;
	}

	if (a[i] != '\0' || b[i] != '\0')
		return 0;

	return 1;
}

char Str_SCmp(char* a, char* b, int count) {
	for (int i = 0; i < count; i++) {
		if (a[i] != b[i])
			return 0;
	}

	return 1;
}