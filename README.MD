# gr2tool
Some in development tools that supports parsing of RAD Game Tools Granny2 file format.

The parsing is done without the use of any closed source library. (No need of granny2.dll)

## How to help
Please run dumpinfo  and report whenever (just open an issue on github):
- The raw bytes are different that 0
- The unknown byte after the sectors is not 0
- The header size is not 0x38
- The file size does not match the real file size

## Contributing
You are allowed to comtribute with new info about the file format or oodle compression
Make sure that you do not have access to any RAD SDK and you do not push any copyrighted code by RAD.

Reading of most of this files are done by comparing already existing gr2 files in granny2 viewer (public release),
other common attemps are messing up with the CRC32 checksum.

The checksum starts after the header size + the magic bytes to the end of the file, the checksum is stored inside the
Granny2 file as Big Endian.
