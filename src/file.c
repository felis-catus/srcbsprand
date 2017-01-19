// TODO: CLEANUP THIS MESS

#include "stdafx.h"

BOOL File_Init( const char *filename )
{
	Spew( "Checking if selected BSP is valid...\n" );
	FILE *file = fopen( filename, "r+b" );

	if ( !File_IsValid( file ) )
		return FALSE;

	if ( !File_CopyBSP( filename ) )
		return FALSE;

	return TRUE;
}

BOOL File_ReadBSP( const char *filename )
{
	Spew( "Starting to read BSP!\n" );
	FILE *file = fopen( filename, "r+b" );
	Map_t *map = BSPRand_GetCurrentMap();

	File_ReadHeader( file, map );
	File_ReadEntityList( file, map ); // TODO: Randomizer
	File_ReadTextures( file, map );
	//File_ReadModels( file, map ); // TODO: Randomizer
	//File_ReadDisplacements( file, map ); // TODO: Randomizer
	//File_ReadOverlays( file, map ); // TODO: Randomizer
	//File_ReadLights( file, map ); // TODO: Randomizer

	fclose( file );

	return TRUE;
}

BOOL File_CopyBSP( const char *filename )
{
	Spew( "Copying BSP...\n" );

	char buf[BUFSIZ];
	unsigned int size;

	char name[1024];
	strncpy( name, filename, 1024 );
	name[strlen( filename ) - 4] = 0;

	char destname[1024];
	_snprintf( destname, sizeof( destname ), "%s_r.bsp", name );

	FILE *source = fopen( filename, "rb" );
	FILE *dest = fopen( destname, "wb" );

	while ( size = fread( buf, 1, BUFSIZ, source ) )
	{
		fwrite( buf, 1, size, dest );
	}

	fclose( source );
	fclose( dest );

	if ( !File_ReadBSP( destname ) )
		return FALSE;

	return TRUE;
}

BOOL File_WriteBSP( const char *filename )
{
	Spew( "Starting to write BSP!\n" );
	FILE *file = fopen( filename, "r+b" );
	Map_t *map = BSPRand_GetCurrentMap();

	File_WriteHeader( file, map );
	File_WriteTextures( file, map );

	fclose( file );

	return TRUE;
}

BOOL File_IsValid( FILE *file )
{
	if ( !file )
	{
		Spew( "ERROR: NULL file!!!\n" );
		return FALSE;
	}

	Map_t map;
	fread( &map.header, 1, sizeof( BSPHeader_t ), file );

	if ( map.header.ident != IDBSPHEADER )
	{
		Spew( "ERROR: This isn't a VBSP?!\n" );
		return FALSE;
	}

	if ( map.header.version == 0 )
	{
		Spew( "FAIL!!!!! NULL VERSION\n" );
		return FALSE;
	}

	Spew( "BSP is valid!\n" );
	Spew( "Map version: %d\n", map.header.version );
	Spew( "Map revision: %d\n", map.header.mapRevision );
	return TRUE;
}

void File_ReadHeader( FILE *file, Map_t *map )
{
	Spew( "Reading header...\n" );

	if ( file )
		fread( &map->header, 1, sizeof( BSPHeader_t ), file );
}

void File_ReadEntityList( FILE *file, Map_t *map )
{
	Spew( "Reading entity lump...\n" );

	int size = map->header.lumps[LUMP_ENTITIES].filelen;
	map->entities.buffer = (char*)malloc( size );

	fseek( file, map->header.lumps[LUMP_ENTITIES].fileofs, SEEK_SET );
	fread( map->entities.buffer, size, sizeof( char ), file );

	if ( Main_ShouldDumpEntList() )
	{
		Spew( "Dumping entity lump to entitylist.txt\n" );

		FILE *entfile = fopen( "entitylist.txt", "w" );
		fwrite( map->entities.buffer, sizeof( char ), size, entfile );
	}
}

void File_ReadTextures( FILE *file, Map_t *map )
{
	// Texinfo
	Spew( "Reading textures...\n" );

	int texcount = map->header.lumps[LUMP_TEXINFO].filelen / sizeof( texinfo_t );
	Spew( "%d textures found\n", texcount );

	fseek( file, map->header.lumps[LUMP_TEXINFO].fileofs, SEEK_SET );

	map->texinfo = malloc( texcount * sizeof( texinfo_t* ) );
	map->texinfoCount = texcount;
	for ( int i = 0; i < texcount; i++ )
	{
		texinfo_t *texinfo = malloc( sizeof( texinfo_t ) );
		fread( texinfo, 1, sizeof( texinfo_t ), file );
		map->texinfo[i] = texinfo;
	}

	// Texdata
	Spew( "Reading texdata...\n" );

	int texdatacount = map->header.lumps[LUMP_TEXDATA].filelen / sizeof( dtexdata_t );
	fseek( file, map->header.lumps[LUMP_TEXDATA].fileofs, SEEK_SET );

	map->texdata = malloc( texdatacount * sizeof( dtexdata_t* ) );
	map->texdataCount = texdatacount;
	for ( int i = 0; i < texdatacount; i++ )
	{
		dtexdata_t *texdata = malloc( sizeof( dtexdata_t ) );
		fread( texdata, 1, sizeof( dtexdata_t ), file );
		map->texdata[i] = texdata;
	}

	// String table offsets
	Spew( "Getting texdata string table offsets...\n" );

	int strtablecount = map->header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen / sizeof( int );
	fseek( file, map->header.lumps[LUMP_TEXDATA_STRING_TABLE].fileofs, SEEK_SET );

	map->texdataStringTable = malloc( strtablecount * sizeof( int ) );
	map->texdataStringTableCount = strtablecount;
	for ( int i = 0; i < strtablecount; i++ )
	{
		int offset;
		fread( &offset, 1, sizeof( int ), file );
		map->texdataStringTable[i] = offset;
	}

	// Texdata strings
	Spew( "Getting texdata strings...\n" );

	int size = map->header.lumps[LUMP_TEXDATA_STRING_DATA].filelen;
	map->texdataStringData = (char*)malloc( MAX_STRINGDATA );
	map->texdataStringDataSize = size;

	fseek( file, map->header.lumps[LUMP_TEXDATA_STRING_DATA].fileofs, SEEK_SET );
	fread( map->texdataStringData, size, sizeof( char ), file );

	// Construct textures
	Spew( "Contructing textures...\n" );

	FILE *texlog = NULL;
	if ( Main_ShouldDumpTexList() )
		texlog = fopen( "texlist.txt", "w" );

	FILE *strTableLog = NULL;
	if ( Main_ShouldDumpStringTable() )
	{
		char *strTable = File_GetPrintableStringTable( map->texdataStringData, map->header.lumps[LUMP_TEXDATA_STRING_DATA].filelen );
		strTableLog = fopen( "strtable_in.txt", "w" );
		fwrite( strTable, 1, strlen( strTable ), strTableLog );

		free( strTable );
	}

	map->textures = malloc( texcount * sizeof( Texture_t* ) );
	map->textureCount = texcount;
	for ( int i = 0; i < texcount; i++ )
	{
		Texture_t *tex = malloc( sizeof( Texture_t ) );
		tex->idx = i;
		tex->dataidx = map->texinfo[i]->texdata;

		int stridx = map->texdataStringTable[tex->dataidx];
		char *texname = &map->texdataStringData[stridx];
		tex->name = texname;

		V_Spew( "tex.name = %s\n", tex->name );

		char tempstr[128];
		sprintf( tempstr, "%s\n", tex->name );

		if ( texlog )
			fwrite( tempstr, 1, strlen( tempstr ), texlog );
		
		map->textures[i] = tex;
	}
}

void File_ReadModels( FILE *file, Map_t *map )
{
	Spew( "Reading models...\n" );
	int count = map->header.lumps[LUMP_MODELS].filelen / sizeof( dmodel_t );
	Spew( "%d models found\n", count );
	fseek( file, map->header.lumps[LUMP_MODELS].fileofs, SEEK_SET );

	map->models = malloc( count * sizeof( dmodel_t* ) );
	map->modelCount = count;
	for ( int i = 0; i < count; i++ )
	{
		dmodel_t *dmodel = malloc( sizeof( dmodel_t ) );
		fread( dmodel, 1, sizeof( dmodel_t ), file );
		map->models[i] = dmodel;
	}
}

void File_ReadDisplacements( FILE *file, Map_t *map )
{
	Spew( "Reading dispinfo...\n" );
	int count = map->header.lumps[LUMP_DISPINFO].filelen / sizeof( ddispinfo_t );
	Spew( "%d disps found\n", count );
	fseek( file, map->header.lumps[LUMP_DISPINFO].fileofs, SEEK_SET );

	map->displacements = malloc( count * sizeof( ddispinfo_t* ) );
	map->dispCount = count;
	for ( int i = 0; i < count; i++ )
	{
		ddispinfo_t *dispinfo = malloc( sizeof( ddispinfo_t ) );
		fread( dispinfo, 1, sizeof( ddispinfo_t ), file );
		map->displacements[i] = dispinfo;
	}
}

void File_ReadOverlays( FILE *file, Map_t *map )
{
	Spew( "Reading overlays...\n" );
	int count = map->header.lumps[LUMP_OVERLAYS].filelen / sizeof( doverlay_t );
	Spew( "%d overlays found...\n", count );
	fseek( file, map->header.lumps[LUMP_DISPINFO].fileofs, SEEK_SET );

	map->overlays = malloc( count * sizeof( doverlay_t* ) );
	map->overlayCount = count;
	for ( int i = 0; i < count; i++ )
	{
		doverlay_t *doverlay = malloc( sizeof( doverlay_t ) );
		fread( doverlay, 1, sizeof( doverlay_t ), file );
		map->overlays[i] = doverlay;
	}
}

void File_WriteHeader( FILE *file, Map_t *map )
{
	Spew( "Writing BSP header...\n" );
	if ( file )
		fwrite( &map->header, 1, sizeof( BSPHeader_t ), file );
}

void File_WriteTextures( FILE *file, Map_t *map )
{
	Spew( "Writing textures...\n" );

	int texcount = map->header.lumps[LUMP_TEXINFO].filelen / sizeof( texinfo_t );

	fseek( file, map->header.lumps[LUMP_TEXINFO].fileofs, SEEK_SET );

	for ( int i = 0; i < texcount; i++ )
		fwrite( map->texinfo[i], 1, sizeof( texinfo_t ), file );
	
	if ( !Main_BSPTexturesOnly() )
	{
		Spew( "Replacing old string table with new one...\n" );

		int strtablecount = map->header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen / sizeof( int );
		fseek( file, map->header.lumps[LUMP_TEXDATA_STRING_TABLE].fileofs, SEEK_SET );

		for ( int i = 0; i < strtablecount; i++ )
		{
			char *texName = &map->texdataStringData[map->texdataStringTable[i]];
			V_Spew( "%s\n", texName );

			fwrite( &map->texdataStringTable[i], 1, sizeof( int ), file );
		}

		FILE *strTableLog = NULL;
		if ( Main_ShouldDumpStringTable() )
		{
			char *strTable = File_GetPrintableStringTable( map->texdataStringData, map->header.lumps[LUMP_TEXDATA_STRING_DATA].filelen );
			strTableLog = fopen( "strtable_out.txt", "w" );
			fwrite( strTable, 1, strlen( strTable ), strTableLog );

			free( strTable );
		}

		Spew( "Replacing string data...\n" );

		int size = map->header.lumps[LUMP_TEXDATA_STRING_DATA].filelen;

		fseek( file, map->header.lumps[LUMP_TEXDATA_STRING_DATA].fileofs, SEEK_SET );
		fwrite( map->texdataStringData, size, sizeof( char ), file );
	}
}

// fix this crap, spams newlines
char *File_GetPrintableStringTable( const char *strtable, int size )
{
	char *buffer = malloc( size + 1 );

	int i = 0;
	while ( i < size )
	{
		if ( strtable[i] == '\0' )
		{
			buffer[i] = '\n';
			i++;
			continue;
		}

		buffer[i] = strtable[i];
		i++;
	}

	buffer[i] = '\0';

	return buffer;
}
