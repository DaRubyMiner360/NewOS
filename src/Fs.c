#include <drive.h>
#include <fs.h>
#include <collections.h>
#include <memory.h>

// TODO Make improved FS api

// Currently only holds the number of regions on the disk
DiskHeader header;


// Each region holds the sector they are in on the disk and what byte they start at.
// They also hold an offset into the metadata sector which for now just holds file names
DiskRegion* regions;

char* metadata;

void FS_Init() {
	char* data = kalloc(512);

	ATA_Read(0, data);

	while (! ATA_IsReady());

	metadata = (char*) kalloc(512);

	ATA_Read(1, metadata);

	while (! ATA_IsReady());

	header = *((DiskHeader*) data);

	regions = (DiskRegion*) (data + sizeof(DiskHeader));
}

int FS_ReadBuf(char* name, char* buffer, int size) {
	int read = 0;

	for (int i = 0; i < header.RegionCount; i++) {
		if (Str_Cmp(name, metadata + regions[i].MetaIndex)) {
			int sector = regions[i].Sector;
			int byte = regions[i].SectorByte;

			int fsize = 512 * (regions[i].EndSector - regions[i].Sector) + regions[i].EndSectorByte + regions[i].SectorByte;

			fsize = fsize > 0 ? fsize : -fsize;

			while (read <= size) {
				char data[512];

				ATA_Read(sector, data);

				while (! ATA_IsReady());

				for (int i = byte; i < 512; i++) {
					if (read >= size || read >= fsize)
						return read;
						
					buffer[read++] = data[i];
				}

				

				sector++;
				byte = 0;
			}
		}
	}

	return read;
}

void FS_Debug() {
	kprintf("Disk Regions: %i\n", header.RegionCount);

	for (int i = 0; i < header.RegionCount; i++) {
		DiskRegion reg = regions[i];

		int size = 512 * (reg.EndSector - reg.Sector) + reg.EndSectorByte + reg.SectorByte;

		size = size > 0 ? size : -size;

		kprintf("    Name: %s Sector: %i End Sector: %i Size: %i\n", metadata + reg.MetaIndex, reg.Sector, reg.EndSector, size);
	}
}