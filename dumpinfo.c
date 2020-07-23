/*
	Granny2 MasterTool dumpinfo
*/
#include <stdio.h>
#include <stdint.h>

/*
	GR2 FF6 INFO
	
	32-byte magic 
	
	B8 67 B0 CA F8 6D B1 0F 84 72 8C 7E 5E 19 00 1E B8 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	--- End of magic
	4 bytes : file format revision == 6 (LE)
	4 bytes : Total file size (LE)
	4 bytes: CRC32 (BE)
		Starts from offset 0x58 (Maybe all files?) and ends at the end of the file
	4 bytes: Size of the header
	4 bytes: Sector count
	4 bytes: Unknown but
		From 1 to 7 it works
		6 shows the 3d art info for some reason
		any other number (possibly) makes it crash
		With metin2 models also any other number different from 6 crash
		99% information flags
	12 bytes: ?
	4 bytes: tag
	16 bytes: ?
	---- End of header
	
	4 bytes: Sector 0 compression type
		0 : No compression
		1: Oodlee 0
		2: Oodlee 1
	
	--- End of compression info
	
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static uint8_t magic[] = { 0xB8, 0x67, 0xB0, 0xCA, 0xF8, 0x6D, 0xB1, 0x0F, 0x84, 0x72, 0x8C, 0x7E, 0x5E, 0x19, 0x00, 0x1E, 0xB8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int main(int argc, char* argv[])
{
	FILE* gr2File = NULL;
	long fileSize;
	uint8_t* fileData = NULL;
	size_t i, p = 0;
	uint32_t crc, fileSizeFromGr2, headerSize, sec0comp;
	
	printf("GRANNY 2 DUMP INFO TOOL (By Arves100)\n");
	
	if (argc < 2)
	{
		printf("%s [file.gr2]\n", argv[0]);
		return 1;
	}
	
	fopen_s(&gr2File, argv[1], "rb");
	
	if (!gr2File)
	{
		printf("Cannot open\n");
		return 1;
	}
	
	fseek(gr2File, 0, SEEK_END);
	fileSize = ftell(gr2File);
	
	if (fileSize < 0x2C)
	{
		printf("Invalid size\n");
		fclose(gr2File);
		return 1;
	}
	
	rewind(gr2File);
	
	fileData = (uint8_t*)malloc(sizeof(uint8_t)*fileSize);
	if (!fileData)
	{
		printf("No memory\n");
		fclose(gr2File);
		return 1;
	}
	
	if (fread(fileData, sizeof(uint8_t), fileSize, gr2File) != fileSize)
	{
		printf("Error at read\n");
		fclose(gr2File);
		free(fileData);
		return 1;
	}
	
	fclose(gr2File);
	
	if (memcmp(fileData, magic, 32) != 0)
	{
		printf("Magic is not valid, file might not be a gr2?\n");
	}
	p += 32;
	
	printf("File format: %u\n", *(uint32_t*)(fileData + p));
	p += 4;

	fileSizeFromGr2 = *(uint32_t*)(fileData + p);
	printf("Total file size: 0x%02x\n", fileSizeFromGr2);

	if (fileSizeFromGr2 != fileSize)
	{
		printf("File size differs, corrupted file?\n");
	}
	
	p += 4;
	
	/*crc = fileData[p] << 0x18;
	p++;
	crc += fileData[p] << 0x10;
	p++;
	crc += fileData[p] << 8;
	p++;
	crc += fileData[p];
	p++;*/
	
	crc = *(uint32_t*)(fileData + p);
	p += 4;
	
	printf("CRC32 Big endian: 0x%02x\n", crc);
	
	headerSize = *(uint32_t*)(fileData + p);
	
	printf("Header size: 0x%02x\n", headerSize);
	
	if (headerSize != 0x38)
	{
		printf("Header size is not 0x38, strange gr2?\n");
	}
	
	p += 4;
	
	printf("Sector number: 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Unknown? (Should be 0x06): 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Raw unknown 12 bytes begin:\n");
	for (i = 0; i < 12; i++, p++)
	{
		printf("0x%02x ", fileData[p]);
	}
	
	printf("\n");
	
	printf("Tag: 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Raw unknown 16 bytes begin:\n");
	for (i = 0; i < 16; i++, p++)
	{
		printf("0x%02x ", fileData[p]);
	}

	printf("\n");
	
	sec0comp = *(uint32_t*)(fileData + p);
	
	printf("Sector 0 compression type: %x\n", sec0comp);
	
	if (sec0comp != 0x00)
	{
		printf("Please not that sector 0 compressed with oodle or oodle-1 is not supported\n");
	}
	
	p += 4;
	
	// sector 0 decomp info etc etc
	
	free(fileData);
	return 0;
}