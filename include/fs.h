typedef struct __attribute__((packed)) {
	unsigned char MetaIndex;
	unsigned short Sector;
	unsigned short SectorByte;

	unsigned short EndSector;
	unsigned short EndSectorByte;
} DiskRegion;

typedef struct __attribute__((packed)) {
	unsigned int RegionCount;
} DiskHeader;

void FS_Init();

int FS_ReadBuf(char* name, char* buffer, int size);

void FS_Debug();