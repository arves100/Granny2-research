/*
	Granny2 File Dumper
	Copyright (C) 2020 Arves100.
	Released under Apache 2.0
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#if !defined(_MSC_VER) || _MSC_VER < 1500
void fopen_s(FILE** pOut, const char* szFile, const char* szMode)
{
	*pOut = fopen(szFile, szMode);
}
#endif

#define BTI(x) (uint32_t)x

struct t_Header
{
  uint32_t Magic[4];
  uint32_t Version;
  uint32_t mustBe0;
  uint8_t unk[8];
};

struct t_VersionInfo
{
	uint32_t HeaderVersion;
	uint32_t Tag;
	char* String;
};

struct t_VersionInfo s_GrannyVersionInfos[] = {
	{ 440, 0x80000015, "Granny 2.6.0.10" }
};

long ParseHeader(uint8_t* data, bool* isBE)
{
	struct t_Header* header = (struct t_Header*)data;

	if (header->Magic[0] == 3400558520 && header->Magic[1] == 263286264 && header->Magic[2] == 2123133572 && header->Magic[3] == 503322974)
	{
		printf("Endianess: Little\n");
		*isBE = false;
	}
	else if (header->Magic[0] == 3093803210 && header->Magic[1] == 4167938319 && header->Magic[2] == 2222099582 && header->Magic[3] == 1578696734)
	{
		printf("Endianess: Big\n");
		printf("!! Please note that BE files are currently not supported !!\n");
		*isBE = true;
	}
	else
	{
		printf("\t!! Invalid magic, the file might not be a GR2! Defaulting Little endian !!\n");
		*isBE = false;
	}

	printf("Header version: %u\n", header->Version);

	if (header->mustBe0 != 0)
	{
		printf("!! The 4 0x00 only bytes are not 0, the GR2 file might be tampred !!\n");
	}

	printf("Raw unknown 8 bytes begin:\n");
	for (int i = 0; i < 8; i++)
	{
		printf("0x%02x ", header->unk[i]);
	}

	printf("\n--- END OF HEADER ---\n");

	return 32;
}

long ParseGenericInfo(uint8_t* fileData, long p, long fileSizeReal)
{
	uint32_t fileSizeFromGr2, crc, headerSize;

	printf("File format: %d\n", *(int32_t*)(fileData + p)); // it's signed, verified on gr2 viewer
	p += 4;

	fileSizeFromGr2 = *(uint32_t*)(fileData + p);
	printf("Total file size: 0x%02x\n", fileSizeFromGr2);

	if (fileSizeFromGr2 != fileSizeReal)
	{
		printf("!! GR2 file size differs the real file size !!\n");
	}

	p += 4;

	crc = *(uint32_t*)(fileData + p);
	p += 4;
	
	printf("CRC32 Big endian: 0x%02x\n", crc);
	
	headerSize = *(uint32_t*)(fileData + p);
	
	printf("Header size: 0x%02x\n", headerSize);
	
	if (headerSize != 0x38)
	{
		printf("|| Header size is not 0x38, strange gr2 ||\n");
	}
	
	p += 4;
	
	printf("Sector number: 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Unknown? (Should be 0x06): 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Raw unknown 12 bytes begin:\n");
	for (int i = 0; i < 12; i++, p++)
	{
		printf("0x%02x ", fileData[p]);
	}
	
	printf("\n");
	
	printf("Tag: 0x%02x\n", *(uint32_t*)(fileData + p));
	p += 4;
	
	printf("Raw unknown 16 bytes begin:\n");
	for (int i = 0; i < 16; i++, p++)
	{
		printf("0x%02x ", fileData[p]);
	}

	printf("\n--- END OF GENERIC INFO ---\n");
	return p;
}

long Sector0Info(uint8_t* fileData, long p)
{
	uint32_t sec0comp;
	
	sec0comp = *(uint32_t*)(fileData + p);
	
	printf("Sector 0 compression type: %x\n", sec0comp);
	
	if (sec0comp != 0x00)
	{
		printf("!! Please not that compressed sector 0 is not supported !!");
	}
	
	p += 4;
	return p;
}

int main(int argc, char* argv[])
{
	FILE* gr2File = NULL;
	long fileSize, p = 0;
	uint8_t* fileData = NULL;
	bool isBE;

	printf("Granny2 File information dumper\n");
	
	if (argc < 2)
	{
		printf("%s [Granny2 file]\n", argv[0]);
		return 1;
	}
	
	// Open the file and get the size and get all the content
	fopen_s(&gr2File, argv[1], "rb");
	
	if (!gr2File)
	{
		printf("Cannot open Granny2 file!\n");
		return 1;
	}
	
	fseek(gr2File, 0, SEEK_END);
	fileSize = ftell(gr2File);
	
	if (fileSize < 32)
	{
		printf("Invalid Granny2 size!\n");
		fclose(gr2File);
		return 1;
	}
	
	rewind(gr2File);
	
	fileData = (uint8_t*)malloc(sizeof(uint8_t)*fileSize);
	if (!fileData)
	{
		printf("No memory for loading the file!\n");
		fclose(gr2File);
		return 1;
	}
	
	if (fread(fileData, sizeof(uint8_t), fileSize, gr2File) != fileSize)
	{
		printf("Cannot read all the file!\n");
		fclose(gr2File);
		free(fileData);
		return 1;
	}
	
	fclose(gr2File);

	p = ParseHeader(fileData, &isBE);
	p = ParseGenericInfo(fileData, p, fileSize);
	p = Sector0Info(fileData, p);
	
	free(fileData);

	printf("=========== FINISH ==========\n");
	printf("Readed up to %u bytes!\n", p);

	return 0;
}