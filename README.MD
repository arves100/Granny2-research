# RAD Granny2 decoding
Attempts at decoding the Granny2 file format.

## References
Several interesting structures (FF6): https://github.com/kangcliff/Age-of-Empires-III/blob/aa422d85deb7b1be47dbd9ba9c07033614455bce/grnreaderX/Granny.h

## File Format Specification (FF6)

### Header (32 bytes)
The first 16 bytes are used as a magic "B8 67 B0 CA F8 6D B1 0F 84 72 8C 7E 5E 19 00 1E", if the magic matches this the file must be parsed in Little Endian.

If this first 16 bytes equals to "CA B0 67 B8 0F B1 6D F8 7E 8C 72 84 1E 00 19 E5" (each 4 bytes are swapped) then the file must be parsed in BIG ENDIAN!

The next 4 bytes tells us the size of Uncompressed File information.

The next 4 bytes MUST BE 0 otherwise the file is threated as invalid.

The next 8 bytes are unknown.

## Uncompressed file information (?)
First 4 bytes is the File Format version.
Next 4 bytes tells us the total file size.
The next 4 bytes tells us the CRC32, which is Big endian.
    CRC is computed from the end of the file to the header size?
Next 4 byte is the Header size.
The next 4 bytes is the Sector count.
The next 8 bytes are a reference for something
Same the next 8 bytes (All 00 00 00 00 ?)
Next 4 bytes is the Tag, which is another number of tell the granny2 version, for some reason, different DLLs have a different Tag number hardcoded. It might be used for distinguish major versions.
All the tags starts from 0x800000. If the tag is 0x800000F then the file is invalid!
Next 16 bytes: ?
(In file format 7, there are 16 extra unknown bytes)

## Sector information
First 4 bytes is the Sector compression type
    0 : No compression
    1: Oodlee 0
    2: Oodlee 1 (FF7?)
Next 4 bytes: Offset of the sector data
Next 4 bytes: Decompressed data length
Next 4 bytes: Compressed data length (Might be 0x00 when compression is 0x00)
18 unknown bytes.
Perhaps sector data?

## Contributing
You are allowed to comtribute with new info about the file format or oodle compression
Make sure that you do not have access to any RAD SDK and you do not push any copyrighted code by RAD.

The checksum starts after the header size + the magic bytes to the end of the file, the checksum is stored inside the
Granny2 file as Big Endian.
