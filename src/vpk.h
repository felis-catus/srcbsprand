#ifndef VPK_H
#define VPK_H

#define VPK_SIGNATURE 0x55aa1234

// https://developer.valvesoftware.com/wiki/VPK_File_Format
typedef struct
{
	unsigned int signature;
	unsigned int version;

	// The size, in bytes, of the directory tree
	unsigned int treeSize;

	// How many bytes of file content are stored in this VPK file (0 in CSGO)
	unsigned int fileDataSectionSize;

	// The size, in bytes, of the section containing MD5 checksums for external archive content
	unsigned int archiveMD5SectionSize;

	// The size, in bytes, of the section containing MD5 checksums for content in this file (should always be 48)
	unsigned int otherMD5SectionSize;

	// The size, in bytes, of the section containing the public key and signature. This is either 0 (CSGO & The Ship) or 296 (HL2, HL2:DM, HL2:EP1, HL2:EP2, HL2:LC, TF2, DOD:S & CS:S)
	unsigned int signatureSectionSize;
} VPKHeader;

typedef struct
{
	unsigned int crc; // A 32bit CRC of the file's data.
	unsigned short preloadBytes; // The number of bytes contained in the index file.

	// A zero based index of the archive this file's data is contained in.
	// If 0x7fff, the data follows the directory.
	unsigned short archiveIndex;

	// If ArchiveIndex is 0x7fff, the offset of the file data relative to the end of the directory (see the header for more details).
	// Otherwise, the offset of the data from the start of the specified archive.
	unsigned int entryOffset;

	// If zero, the entire file is stored in the preload data.
	// Otherwise, the number of bytes stored starting at EntryOffset.
	unsigned int entryLength;

	unsigned short terminator;
} VPKDirectoryEntry;

typedef struct
{
	unsigned int archiveIndex;
	unsigned int startingOffset; // where to start reading bytes
	unsigned int count; // how many bytes to check
	char md5Checksum[16]; // expected checksum
} VPKArchiveMD5SectionEntry;

typedef struct
{
	char treeChecksum[16];
	char archiveMD5SectionChecksum[16];
	char unknown[16]; // ???
} VPKOtherMD5Section;

typedef struct
{
	unsigned int publicKeySize; // always seen as 160 (0xA0) bytes
	char publicKey[160];

	unsigned int signatureSize; // always seen as 128 (0x80) bytes
	char signature[128];
} VPKSignatureSection;

const char *VPK_ReadString( FILE *file );
char *VPK_ReadAndAllocString( FILE *file );
#endif
