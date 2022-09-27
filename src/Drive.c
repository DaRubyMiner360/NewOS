#include <kernel.h>

char* Block;

char Ready = 0;

void ATA_Read(unsigned int LBA, unsigned char* buffer) {
	Block = buffer;
	
	outb(0x1F6, 0xF0 | (0xE0 << 4) | ((LBA >> 24) & 0x0F));
	outb(0x1F1, 0x00);
	outb(0x1F2, 1);
	outb(0x1F3, (unsigned char) LBA);
	outb(0x1F4, (unsigned char) (LBA >> 8));
	outb(0x1F5, (unsigned char) (LBA >> 16));
	outb(0x1F7, 0x20);
}

char* ATA_GetData() {
	return Block;
}

void ATA_Ready() {
	for (int i = 0; i < 512; i++) {
		unsigned short sh = inw(0x1F0);

		Block[i + 1] = sh >> 8;

		Block[i++] = sh & 0xFF;
	}

	Ready = 1;
}

char ATA_IsReady() {
	if (Ready) {
		Ready = 0;

		return 1;
	}

	return 0;
}