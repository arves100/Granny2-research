# RAD Granny2 decoding
Attempts at decoding the Granny2 file format.

## References
Several interesting structures (FF6): https://github.com/kangcliff/Age-of-Empires-III/blob/aa422d85deb7b1be47dbd9ba9c07033614455bce/grnreaderX/Granny.h

## File Format Specification (FF6)

### Header (32 bytes)
The first 16 bytes are used as a magic "B8 67 B0 CA F8 6D B1 0F 84 72 8C 7E 5E 19 00 1E", if the magic matches this the file must be parsed in Little Endian.

If this first 16 bytes equals to "CA B0 67 B8 0F B1 6D F8 7E 8C 72 84 1E 00 19 E5" (each 4 bytes are swapped) then the file must be parsed in BIG ENDIAN!

The next 4 bytes tells us the Granny2 internal version
For Granny 2.11.8.0 is 456
For Granny 2.4.0.10 is 440

The next 4 bytes MUST BE 0 otherwise the file is threated as invalid.

The next 8 bytes are unknown.

## Uncompressed file information (?)
First 4 bytes is the File Format version.
Next 4 bytes tells us the total file size.
The next 4 bytes tells us the CRC32, which is Big endian on Little endian files
and (supposibly) Little endian on Big Endian files.
CRC32 starts from byte 0x58 (It might be hardcoded?) to all the file.
Next 4 byte is the Header size.
The next 4 bytes is the Sector count.
Next 4 bytes: Unknown but
		From 1 to 7 it works
		6 shows the 3d art info for some reason
		any other number (possibly) makes it crash
Next 12 bytes: ?
Next 4 bytes is the Tag, which is another number of tell the granny2 version, for some reason, different DLLs have a different Tag number hardcoded. It might be used for distinguish major versions.
All the tags starts from 0x800000.
Next 16 bytes: ?

## Sector information
First 4 bytes is the Sector compression type
    0 : No compression
    1: Oodlee 0
    2: Oodlee 1

## Contributing
You are allowed to comtribute with new info about the file format or oodle compression
Make sure that you do not have access to any RAD SDK and you do not push any copyrighted code by RAD.

The checksum starts after the header size + the magic bytes to the end of the file, the checksum is stored inside the
Granny2 file as Big Endian.
