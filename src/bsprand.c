#include "stdafx.h"

//
// Vars
//
Map_t *pCurrentMap;
int iTexdataCache[12288];
char **pszMatNameCache;
int iMatNameCacheSize;
int iRandSeed;
int iLooseMaterialCount;

int BSPRand_GetRandomSeed() { return iRandSeed; }
void BSPRand_SetRandomSeed( int seed ) { iRandSeed = seed; }

Map_t *BSPRand_GetCurrentMap() { return pCurrentMap; }
void BSPRand_SetCurrentMap( Map_t *map ) { pCurrentMap = map; }
//
//
//

BOOL BSPRand_Init()
{
	// and this is where the fun shall begin
	// TODO: go nuts

	iMatNameCacheSize = 0;
	iLooseMaterialCount = 0;

	BOOL success = TRUE;

	BSPRand_ChangeHeader();

	if ( !BSPRand_ChangeTextures() )
		return FALSE;

	return TRUE;
}

void BSPRand_Cleanup()
{
	Map_t *pMap = BSPRand_GetCurrentMap();
	if ( pMap )
	{
		for ( int i = 0; i < pMap->textureCount; i++ )
			free( pMap->textures[i] );
		free( pMap->textures );

		for ( int i = 0; i < pMap->texinfoCount; i++ )
			free( pMap->texinfo[i] );
		free( pMap->texinfo );

		for ( int i = 0; i < pMap->texdataCount; i++ )
			free( pMap->texdata[i] );
		free( pMap->texdata );

		free( pMap->texdataStringData );
		free( pMap->texdataStringTable );

		for ( int i = 0; i < pMap->modelCount; i++ )
			free( pMap->models[i] );
		free( pMap->models );
		
		for ( int i = 0; i < pMap->dispCount; i++ )
			free( pMap->displacements[i] );
		free( pMap->displacements );

		for ( int i = 0; i < pMap->overlayCount; i++ )
			free( pMap->overlays[i] );
		free( pMap->overlays );
		
		for ( int i = 0; i < pMap->lightCount; i++ )
			free( pMap->lights[i] );
		free( pMap->lights );

		free( pMap );
		BSPRand_SetCurrentMap( NULL );
	}

	// free cache
	for ( int i = 0; i < iMatNameCacheSize; i++ )
		free( pszMatNameCache[i] );

	free( pszMatNameCache );
	pszMatNameCache = NULL;
}

void BSPRand_CreateMap()
{
	// create map
	Map_t *pMap = (Map_t*)malloc( sizeof( Map_t ) );

	// init
	pMap->textures = NULL;
	pMap->texinfo = NULL;
	pMap->texdata = NULL;
	pMap->texdataStringData = "";
	pMap->texdataStringTable = NULL;
	pMap->models = NULL;
	pMap->displacements = NULL;
	pMap->overlays = NULL;
	pMap->lights = NULL;

	pMap->textureCount = 0;
	pMap->texinfoCount = 0;
	pMap->texdataCount = 0;
	pMap->texdataStringDataSize = 0;
	pMap->texdataStringTableCount = 0;
	pMap->modelCount = 0;
	pMap->dispCount = 0;
	pMap->overlayCount = 0;
	pMap->lightCount = 0;

	BSPRand_SetCurrentMap( pMap );
}

void BSPRand_ChangeHeader()
{
	Spew( "Changing header...\n" );

	Map_t *map = BSPRand_GetCurrentMap();

	// test manipulate header
	map->header.mapRevision = 1337;
}

BOOL BSPRand_ChangeTextures()
{
	Spew( "Starting texture randomizer...\n" );

	if ( !BSPRand_BuildTextureList() )
	{
		Spew( "FATAL ERROR: ChangeTextures() failed!\n" );
		return FALSE;
	}
	else
		return BSPRand_TextureRandomizer();
}

BOOL BSPRand_TextureRandomizer()
{
	Map_t *map = BSPRand_GetCurrentMap();
	BOOL useMatNames = ( iMatNameCacheSize != 0 );

	if ( useMatNames )
	{
		// recreate the string table from random material names
		int stringTableSize = map->texdataStringTableCount;
		StringTableItem_t **stringTable = malloc( stringTableSize * sizeof( StringTableItem_t* ) );

		int totalLen = -1;
		for ( int i = 0; i < stringTableSize; i++ )
		{
			StringTableItem_t *item = malloc( sizeof( StringTableItem_t ) );

			// TODO: need better rand? lol
			int seed = ( BSPRand_GetRandomSeed() + ( i * 0xD1CE ) );
			srand( seed );
			int iRand = rand() % iMatNameCacheSize;

			// we put random string in here, also fixing up indices
			item->data = pszMatNameCache[iRand];
			item->index = totalLen + 1;
			stringTable[i] = item;

			totalLen += strlen( item->data ) + 1;

			V_Spew( "Adding %s to the string table (idx: %d) (rand: %d)\n", item->data, item->index, iRand );
		}

		char newStringData[MAX_STRINGDATA];
		memset( newStringData, 0, sizeof( newStringData ) );

		// now append into the new data
		int loc = 0;
		for ( int i = 0; i < stringTableSize; i++ )
		{
			StringTableItem_t *item = stringTable[i];
			if ( !item )
				continue;

			char *dest = &newStringData[loc];
			const char *src = item->data;
			int len = strlen( src );

			while ( len-- )
			{
				if ( loc >= MAX_STRINGDATA - 1 )
					break;

				*dest++ = *src++;
				loc++;
			}

			dest[++loc] = 0;
		}

		// fix up the string table indices
		for ( int i = 0; i < stringTableSize; i++ )
		{
			StringTableItem_t *item = stringTable[i];
			if ( !item )
				continue;

			map->texdataStringTable[i] = item->index;
		}

		// the magic happens here
		memset( map->texdataStringData, 0, map->texdataStringDataSize );
		int len = loc;
		int i = 0;
		while ( len-- )
		{
			char *dest = map->texdataStringData;
			char *src = newStringData;
			dest[i] = src[i];
			i++;
		}

		// free
		for ( int i = 0; i < stringTableSize; i++ )
		{
			StringTableItem_t *item = stringTable[i];
			if ( !item )
				continue;

			free( item );
		}

		free( stringTable );
	}
	else
	{
		for ( int i = 0; i < map->textureCount; i++ )
		{
			int cachesize = map->textureCount;

			// TODO: need better rand? lol
			int seed = ( BSPRand_GetRandomSeed() + ( i * 0xD1CE ) );
			srand( seed );
			int iRand = rand() % cachesize;

			int stridx = map->texdataStringTable[map->texinfo[i]->texdata];
			char *texName = &map->texdataStringData[stridx];
			if ( iRand < cachesize )
			{
				// only replace texdata, changes to another texture inside bsp
				map->texinfo[i]->texdata = iTexdataCache[iRand];
				char *texname = &map->texdataStringData[stridx];
				V_Spew( "RAND: tex.name = %s (%d)\n", texname, iRand );
			}
		}
	}

	Spew( "Textures randomized!\n" );
	return TRUE;
}

BOOL BSPRand_BuildTextureList()
{
	Map_t *map = BSPRand_GetCurrentMap();

	if ( !Main_BSPTexturesOnly() )
	{
		// First scan VPKs
		Spew( "Scanning VPKs for materials...\n" );
		BSPRand_VPKTextureScan( Main_GetGameDir() );

		// After that, loose files
		Spew( "Scanning loose material files...\n" );
		char *materialsDir = "materials\\";

		if ( Main_GetGameDir()[0] != 0 )
		{
			char temp[MAX_PATH*2];
			sprintf( temp, "%s\\materials", Main_GetGameDir() );
			materialsDir = temp;
		}

		BSPRand_RecursiveTextureScan( materialsDir );
		Spew( "%d loose materials found\n", iLooseMaterialCount );

		// Nothing in the cache? Failed!
		if ( iMatNameCacheSize == 0 )
		{
			Spew( "Texture scan failed, using data inside .bsp instead.\n" );

			for ( int i = 0; i < map->textureCount; i++ )
			{
				Texture_t *tex = map->textures[i];
				iTexdataCache[i] = tex->dataidx;
			}
		}
	}
	else
	{
		Spew( "Getting textures from BSP...\n" );

		// push texnames into cache
		for ( int i = 0; i < map->textureCount; i++ )
		{
			Texture_t *tex = map->textures[i];
			iTexdataCache[i] = tex->dataidx;
		}
	}

	return TRUE;
}

BOOL BSPRand_RecursiveTextureScan( char *current )
{
	BOOL success = TRUE;

	char path[512];
	if ( current[0] )
		sprintf( path, "%s\\*.*", current );
	else
		sprintf( path, "*.*" );

	FixSlashes( path, '\\' );

	// I'm too lazy to install any file system libraries so WinAPI works !FOR NOW!
	WIN32_FIND_DATA file;
	HANDLE hFind = FindFirstFileEx( path, FindExInfoStandard, &file, FindExSearchNameMatch, NULL, 0 );

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		Spew( "RecursiveTextureScan threw %d, bailing\n", GetLastError() );
		return FALSE;
	}

	while ( FindNextFile( hFind, &file ) )
	{
		if ( file.cFileName[0] != '.' )
		{
			V_Spew( "fileName = %s\n", file.cFileName );
			if ( file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				char nextDir[512];
				if ( current[0] )
					sprintf( nextDir, "%s\\%s", current, file.cFileName );
				else
					sprintf( nextDir, "%s", file.cFileName );

				success = BSPRand_RecursiveTextureScan( nextDir );
			}
			else
			{
				char relative[512];
				if ( current[0] )
					sprintf( relative, "%s\\%s", current, file.cFileName );
				else
					sprintf( relative, "%s", file.cFileName );

				FixSlashes( relative, '\\' );

				// Alloc some for material name, we need to keep it
				char *matName = malloc( 512 );
				strncpy( matName, relative, sizeof( relative ) );

				// If it's not vmt bail out
				if ( !StrEq( GetFileExtension( matName ), "vmt" ) )
				{
					free( matName );
					continue;
				}

				// Strip file extension
				StripExtension( matName, matName, sizeof( matName ) );

				// Strip directories from front to mimic the Source filesystem
				const char *matDir = "materials\\";
				strncpy( matName, strstr( matName, matDir ), 512 );
				strncpy( matName, matName + strlen( matDir ), 512 );

				pszMatNameCache = realloc( pszMatNameCache, ( iMatNameCacheSize + 1 ) * sizeof( char* ) );
				pszMatNameCache[iMatNameCacheSize] = matName;
				iMatNameCacheSize = iMatNameCacheSize;
				iMatNameCacheSize++;

				iLooseMaterialCount++;
			}
		}
	}

	FindClose( hFind );
	return success;
}

BOOL BSPRand_VPKTextureScan( char *dir )
{
	char path[512];
	sprintf( path, "%s\\*.*", dir );

	WIN32_FIND_DATA file;
	HANDLE hFind = FindFirstFileEx( path, FindExInfoStandard, &file, FindExSearchNameMatch, NULL, 0 );

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		Spew( "VPKTextureScan threw %d, bailing\n", GetLastError() );
		return FALSE;
	}

	while ( FindNextFile( hFind, &file ) )
	{
		const char *extension = GetFileExtension( file.cFileName );
		if ( file.cFileName[0] != '.' && extension && StrEq( extension, "vpk" ) )
		{
			int matCount = 0;

			char relative[512];
			if ( dir[0] )
				sprintf( relative, "%s\\%s", dir, file.cFileName );
			else
				sprintf( relative, "%s", file.cFileName );

			FILE *vpkFile = fopen( relative, "r+b" );
			if ( !vpkFile )
				continue;

			VPKHeader header;
			fread( &header, 1, sizeof( VPKHeader ), vpkFile );

			if ( header.signature != VPK_SIGNATURE )
			{
				V_Spew( "%s: Bad signature\n", file.cFileName );
				continue;
			}

			// Extension
			for ( ;; )
			{
				char *extension = VPK_ReadAndAllocString( vpkFile );
				if ( !extension || StrEq( extension, "" ) )
					break;

				// Path
				for ( ;; )
				{
					char *path = VPK_ReadAndAllocString( vpkFile );
					if ( !path || StrEq( path, "" ) )
						break;

					// Filename
					for ( ;; )
					{
						char *filename = VPK_ReadAndAllocString( vpkFile );
						if ( !filename || StrEq( filename, "" ) )
							break;

						char matName[512];
						sprintf( matName, "%s\\%s", path, filename );

						if ( StrContains( matName, "materials\\" ) || StrContains( matName, "materials/" ) )
						{
							// Strip first dir
							char *finalMatName = malloc( 512 );
							for ( size_t i = 0; i < strlen( matName ); i++ )
							{
								if ( PATHSEPARATOR( matName[i] ) )
								{
									strncpy( finalMatName, &matName[i + 1], 512 );
									break;
								}
							}

							pszMatNameCache = realloc( pszMatNameCache, ( iMatNameCacheSize + 1 ) * sizeof( char* ) );
							pszMatNameCache[iMatNameCacheSize] = finalMatName;
							iMatNameCacheSize = iMatNameCacheSize;
							iMatNameCacheSize++;

							matCount++;
						}

						VPKDirectoryEntry dirEntry;
						fread( &dirEntry.crc, 1, sizeof( dirEntry.crc ), vpkFile );
						fread( &dirEntry.preloadBytes, 1, sizeof( dirEntry.preloadBytes ), vpkFile );
						fread( &dirEntry.archiveIndex, 1, sizeof( dirEntry.archiveIndex ), vpkFile );
						fread( &dirEntry.entryOffset, 1, sizeof( dirEntry.entryOffset ), vpkFile );
						fread( &dirEntry.entryLength, 1, sizeof( dirEntry.entryLength ), vpkFile );
						fread( &dirEntry.terminator, 1, sizeof( dirEntry.terminator ), vpkFile );

						free( filename );
						filename = NULL;
					}

					free( path );
					path = NULL;
				}

				free( extension );
				extension = NULL;
			}

			Spew( "%s: %d materials found\n", file.cFileName, matCount );
		}
	}

	FindClose( hFind );
	return TRUE;
}

