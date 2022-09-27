char ATA_IsReady();

void ATA_Read(unsigned int LBA, unsigned char* buffer);

void ATA_Ready();

char* ATA_GetData();