#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>

typedef struct __attribute__((packed)) {
	unsigned char MetaIndex;
	unsigned short Sector;
	unsigned short SectorByte;

	unsigned short EndSector;
	unsigned short EndSectorByte;
} DiskRegion;

typedef struct __attribute__((packed)){
	unsigned int RegionCount;
} DiskHeader;

typedef struct {
	char* File;
	int Size;
} FileInfo;

int main(int argc, char** argv) {
	FILE* file = fopen("fs.img", "w");

	char firstsector[512];

	DiskHeader header;

	header.RegionCount = (argc - 1) / 2;

	memcpy(firstsector, &header, sizeof(DiskHeader));

	int arg = 1;

	FileInfo files[1024];

	int count = 0;

	char metasector[512];

	char* ptr = metasector;

	if (argc > 1) {
		char* reg = firstsector + sizeof(DiskHeader);

		int usedsectors = 2;

		DiskRegion* last = NULL;

		int metaloc = 0;

		for (; arg < argc; ) {
			FileInfo f;

			f.File = argv[arg++];

			f.Size = atoi(argv[arg++]);

			DiskRegion region;

			region.MetaIndex = metaloc;

			memcpy(ptr, f.File, strlen(f.File) + 1);

			ptr += strlen(f.File) + 1;

			metaloc += strlen(f.File) + 1;

			if (last != NULL && last->EndSectorByte < 512) {
				region.Sector = last->EndSector;

				region.SectorByte = last->EndSectorByte;

				region.EndSector = last->EndSector + floor((double) f.Size / 512);
				
				region.EndSectorByte = region.SectorByte + f.Size - 512 * floor((double) f.Size / 512);

				last = &region;
			}
			else {
				region.Sector = usedsectors++;

				region.SectorByte = 0;

				region.EndSector = usedsectors + floor((double) f.Size / 512) - 1;
				
				region.EndSectorByte = region.SectorByte + f.Size - 512 * floor((double) f.Size / 512);

				last = &region;
			}

			printf("%i -> %i\n", region.SectorByte, region.EndSectorByte);

			memcpy(reg, &region, sizeof(DiskRegion));

			reg += sizeof(DiskRegion);

			files[count++] = f;
		}
	}

	fwrite(firstsector, 1, 512, file);

	fwrite(metasector, 1, 512, file);

	/*char* unfinished = NULL;
	FILE* fhandle;
	FileInfo last;
	int lastread = 0;
	int used = 0;

	for (int i = 0; i < count; i++) {
		FILE* fsfile = fopen(files[i].File, "r");

		int size = files[i].Size;

		char sector[512];

		int totalread = 0;

		if (unfinished != NULL) {
			int r = fread(unfinished + used + 1, 1, last.Size - used, fhandle);

			totalread += r;
			used += r;

			if (used < 512) {
				fhandle = fsfile;
				lastread = totalread;
				unfinished = sector;
				used = 0;

				continue;
			}
			else {
				unfinished = NULL;
				lastread = 0;
				used = 0;
				fclose(fhandle);
			}
		}

		totalread += fread(sector, 1, size > 512 ? 512 : size, fsfile);

		if (size < 512) {
			unfinished = sector;
			used = size;
			lastread = totalread;
			last = files[i];

			continue;
		}

		fwrite(sector, 1, 512, file);

		if (size > 512) {
			while (totalread < size) {
				char newsector[512];

				int read = fread(newsector, 1, size - totalread > 512 ? 512 : size - totalread, fsfile);

				totalread += read;

				fwrite(newsector, 1, 512, file);

				if (read == 0)
					break;
			}
		}
	}

	if (unfinished != NULL)
		fwrite(unfinished, 1, 512, file);*/

	int total = 0;

	for (int i = 0; i < count; i++) {
		FILE* fsfile = fopen(files[i].File, "r");

		int size = files[i].Size;

		char* buffer = malloc(size);

		while (size > 0) {
			int writeamount = size > 512 ? 512 : size;

			int read = fread(buffer, 1, writeamount, fsfile);

			printf("Byte: %i, Sector: %i\n", total, total / 512);

			total += read;

			if (read != writeamount) {
				printf("Failed to read %i bytes. Got %i.\n", writeamount, read);

				fwrite(buffer, 1, read, file);

				printf("End Byte: %i, Sector: %i\n", total, total / 512);

				break;
			}

			fwrite(buffer, 1, writeamount, file);

			size -= writeamount;
		}

		printf("End Byte: %i, Sector: %i\n", total, total / 512);

		free(buffer);
	}

	printf("Padding %i to sector\n", total);

	int i = 0;

	while (total % 512 != 0) {
		i++;
		total++;

		fwrite("", 1, 1, file);
	}

	printf("Added %i padding bytes\n", i);

	fclose(file);
}

/*
for (int i = 0; i < sectors; i++) {
		

		FILE* fsfile = fopen(argv[arg++], "r");

		int size = atoi(argv[arg++]);

		char sector[512];

		int totalread = fread(sector, 1, size, fsfile);

		fwrite(sector, 1, 512, file);

		if (size > 512) {
			while (totalread < size) {
				char newsector[512];


			}
		}
	}*/