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
  uint32_t SizeWithSectors;
  uint32_t mustBe0;
  uint8_t unk[8];
};

struct t_Reference
{
	uint32_t SectorNumber;
	uint32_t Position;
};

struct t_FileInfo
{
  int32_t FileFormat;
  uint32_t TotalFileSize;
  uint32_t CRC32;
  uint32_t FileInfoSize;
  uint32_t SectorCount;
  struct t_Reference Ref1;
  struct t_Reference Ref2;
  uint32_t Tag;
  uint8_t unk[32];
};

struct t_Sector
{
	uint32_t CompressionType;
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

	printf("Header size with sectors?: %u\n", header->SizeWithSectors);

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

long ParseFileInfo(uint8_t* fileData, long p, long fileSizeReal)
{
	struct t_FileInfo* fi = (struct t_FileInfo*)(fileData + p);
	uint32_t ExpectedHeaderSize = 0x38;

	printf("File format: %d\n", fi->FileFormat);

	if (fi->FileFormat == 7)
	{
		ExpectedHeaderSize = 0x48;
		printf("!! File Format 7 support is not complete, data might be invalid !!\n");
	}
	else if (fi->FileFormat != 6)
	{
		printf("!! Only File Format 6 is supported, data might be invalid !!\n");
	}

	printf("Total file size: 0x%02x\n", fi->TotalFileSize);

	if (fi->TotalFileSize != fileSizeReal)
	{
		printf("!! GR2 file size differs the real file size !!\n");
	}

	printf("CRC32 Big endian: 0x%02x\n", fi->CRC32);
	printf("Header size: 0x%02x\n", fi->FileInfoSize);
	
	if (fi->FileInfoSize != ExpectedHeaderSize)
	{
		printf("!! Header size is not %u, strange gr2 !!\n", ExpectedHeaderSize);
	}
	
	printf("Sector total numbers: 0x%02x\n", fi->SectorCount);

	printf("Reference 1:\n");
	printf("\tSector: %u\n", fi->Ref1.SectorNumber);
	printf("\tPosition: %u\n", fi->Ref1.Position);

	printf("Reference 2:\n");
	printf("\tSector: %u\n", fi->Ref2.SectorNumber);
	printf("\tPosition: %u\n", fi->Ref2.Position);
	
	printf("Tag: 0x%02x\n", fi->Tag);

	printf("Raw unknown %u bytes begin:\n", ExpectedHeaderSize - 0x28);
	for (int i = 0; i < (ExpectedHeaderSize - 0x28); i++, p++)
	{
		printf("0x%02x ", fi->unk[p]);
	}

	printf("\n--- END OF GENERIC INFO ---\n");

	return fi->FileInfoSize;
}

long Sector0Info(uint8_t* fileData, long p)
{
	struct t_Sector* sector = (struct t_Sector*)(fileData+p);

	printf("Sector 0 compression type: %x\n", sector->CompressionType);
	
	if (sector->CompressionType != 0x00)
	{
		printf("!! Please not that compressed sector 0 is not supported !!");
	}

	return sizeof(struct t_Sector);
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
	p = ParseFileInfo(fileData, p, fileSize);
	p = Sector0Info(fileData, p);
	
	free(fileData);

	printf("=========== FINISH ==========\n");
	printf("Readed up to %u bytes!\n", p);

	return 0;
}