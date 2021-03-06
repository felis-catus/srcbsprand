#include "stdafx.h"

//
// Vars
//
Map_t *pCurrentMap;
int iTexdataCache[12288];
char **pszMatNameCache;
int iMatNameCacheSize;

char **pszModelNameCache;
int iModelNameCacheSize;

// Models with propdata needs to be in separate array to prevent crashes
// TODO:
//char **pszPropModelNameCache;
//int iPropModelNameCacheSize;

char **pszSoundNameCache;
int iSoundNameCacheSize;

unsigned int uRandSeed;

int iLooseMaterialCount;
int iLooseModelCount;
int iLooseSoundCount;

unsigned int BSPRand_GetRandomSeed() { return uRandSeed; }
void BSPRand_SetRandomSeed( unsigned int seed ) 
{
	srand( seed );
	uRandSeed = seed;
}

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
	iModelNameCacheSize = 0;
	//iPropModelNameCacheSize = 0;
	iSoundNameCacheSize = 0;
	iLooseMaterialCount = 0;
	iLooseModelCount = 0;
	iLooseSoundCount = 0;

	BSPRand_BuildResourcesList();

	BSPRand_ChangeHeader();

	BSPRand_EntityRandomizer();

	if ( !BSPRand_TextureRandomizer() )
		return FALSE;

	return TRUE;
}

void BSPRand_Cleanup()
{
	Map_t *pMap = BSPRand_GetCurrentMap();
	if ( pMap )
	{
		for ( int i = 0; i < pMap->entitiesCount; i++ )
		{
			for ( int j = 0; j < pMap->entities[i]->kvCount; j++ )
				free( pMap->entities[i]->keyValues[j] );

			free( pMap->entities[i] );
		}
		free( pMap->entities );

		free( pMap->entitiesBuffer );

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

	// free caches
	for ( int i = 0; i < iMatNameCacheSize; i++ )
		free( pszMatNameCache[i] );

	free( pszMatNameCache );
	pszMatNameCache = NULL;

	for ( int i = 0; i < iModelNameCacheSize; i++ )
		free( pszModelNameCache[i] );

	free( pszModelNameCache );
	pszModelNameCache = NULL;

	for ( int i = 0; i < iSoundNameCacheSize; i++ )
		free( pszSoundNameCache[i] );

	free( pszSoundNameCache );
	pszSoundNameCache = NULL;
}

void BSPRand_CreateMap()
{
	// create map
	Map_t *pMap = (Map_t*)malloc( sizeof( Map_t ) );

	// init
	pMap->entities = NULL;
	pMap->entitiesBuffer = NULL;
	pMap->textures = NULL;
	pMap->texinfo = NULL;
	pMap->texdata = NULL;
	pMap->texdataStringData = "";
	pMap->texdataStringTable = NULL;
	pMap->models = NULL;
	pMap->displacements = NULL;
	pMap->overlays = NULL;
	pMap->lights = NULL;

	pMap->entitiesCount = 0;
	pMap->entitiesBufferSize = 0;
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

BOOL BSPRand_BuildResourcesList()
{
	Map_t *map = BSPRand_GetCurrentMap();
	BOOL hasGameDir = ( Main_GetGameDir()[0] != 0 );

	if ( hasGameDir && !Main_BSPTexturesOnly() )
	{
		// First scan VPKs, does both textures and sounds
		Spew( "Scanning VPKs for resources...\n" );
		BSPRand_VPKResourceScan( Main_GetGameDir() );

		// After that, loose files
		Spew( "Scanning loose files for resources...\n" );

		// Loose Textures
		if ( !Main_BSPTexturesOnly() )
		{
			char *materialsDir = "materials\\";

			if ( hasGameDir )
			{
				char temp[MAX_PATH * 2];
				sprintf( temp, "%s\\materials", Main_GetGameDir() );
				materialsDir = temp;
			}

			BSPRand_RecursiveResourceScan( materialsDir );
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

		// Loose Sounds
		char *soundDir = "sound\\";

		if ( hasGameDir )
		{
			char temp[MAX_PATH * 2];
			sprintf( temp, "%s\\sound", Main_GetGameDir() );
			soundDir = temp;
		}

		BSPRand_RecursiveResourceScan( soundDir );
		Spew( "%d loose sounds found\n", iLooseMaterialCount );
	}
	else
	{
		// Use textures from BSP
		Spew( "Getting textures from BSP...\n" );

		for ( int i = 0; i < map->textureCount; i++ )
		{
			Texture_t *tex = map->textures[i];
			iTexdataCache[i] = tex->dataidx;
		}
	}

	return TRUE;
}

BOOL BSPRand_VPKResourceScan( const char *dir )
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
			int modelCount = 0;
			int soundCount = 0;

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

						char fileName[512];
						sprintf( fileName, "%s/%s", path, filename );

						// TODO: Cleanup
						if ( StrContains( fileName, "materials/" ) )
						{
							// Strip first dir
							char *finalMatName = malloc( 512 );
							for ( size_t i = 0; i < strlen( fileName ); i++ )
							{
								if ( PATHSEPARATOR( fileName[i] ) )
								{
									strncpy( finalMatName, &fileName[i + 1], 512 );
									break;
								}
							}

							pszMatNameCache = realloc( pszMatNameCache, ( iMatNameCacheSize + 1 ) * sizeof( char* ) );
							pszMatNameCache[iMatNameCacheSize] = finalMatName;
							iMatNameCacheSize++;

							matCount++;
						}
						else if ( StrContains( fileName, "models/" ) && StrEq( extension, "mdl" ) )
						{
							char *finalModelName = malloc( 512 );
							strncpy( finalModelName, fileName, 512 );

							strncat( finalModelName, ".", 1 );
							strncat( finalModelName, extension, strlen( extension ) );

							pszModelNameCache = realloc( pszModelNameCache, ( iModelNameCacheSize + 1 ) * sizeof( char* ) );
							pszModelNameCache[iModelNameCacheSize] = finalModelName;
							iModelNameCacheSize++;

							modelCount++;
						}
						else if ( StrContains( fileName, "sound/" ) )
						{
							// ditto
							char *finalSoundName = malloc( 512 );
							for ( size_t i = 0; i < strlen( fileName ); i++ )
							{
								if ( PATHSEPARATOR( fileName[i] ) )
								{
									strncpy( finalSoundName, &fileName[i + 1], 512 );
									break;
								}
							}

							strncat( finalSoundName, ".", 1 );
							strncat( finalSoundName, extension, strlen( extension ) );

							pszSoundNameCache = realloc( pszSoundNameCache, ( iSoundNameCacheSize + 1 ) * sizeof( char* ) );
							pszSoundNameCache[iSoundNameCacheSize] = finalSoundName;
							iSoundNameCacheSize++;

							soundCount++;
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
			Spew( "%s: %d models found\n", file.cFileName, modelCount );
			Spew( "%s: %d sounds found\n", file.cFileName, soundCount );
		}
	}

	FindClose( hFind );
	return TRUE;
}

BOOL BSPRand_RecursiveResourceScan( char *current )
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

				success = BSPRand_RecursiveResourceScan( nextDir );
			}
			else
			{
				char relative[512];
				if ( current[0] )
					sprintf( relative, "%s\\%s", current, file.cFileName );
				else
					sprintf( relative, "%s", file.cFileName );

				FixSlashes( relative, '\\' );

				// If it's not a file we want bail out
				const char *extension = GetFileExtension( file.cFileName );
				if ( !StrEq( extension, "vmt" ) ||
					!StrEq( extension, "mdl" ) ||
					!StrEq( extension, "wav" ) ||
					!StrEq( extension, "mp3" ) )
				{
					continue;
				}

				// Alloc some for the name, we need to keep it
				char *fileName = malloc( 512 );
				strncpy( fileName, relative, sizeof( relative ) );

				// Strip file extension
				StripExtension( fileName, fileName, sizeof( fileName ) );

				// Strip directories from front to mimic the Source filesystem
				if ( StrEq( extension, "vmt" ) )
				{
					const char *matDir = "materials\\";
					strncpy( fileName, strstr( fileName, matDir ), 512 );
					strncpy( fileName, fileName + strlen( matDir ), 512 );

					pszMatNameCache = realloc( pszMatNameCache, ( iMatNameCacheSize + 1 ) * sizeof( char* ) );
					pszMatNameCache[iMatNameCacheSize] = fileName;
					iMatNameCacheSize++;

					iLooseMaterialCount++;
				}
				else if ( StrEq( extension, "mdl" ) )
				{
					const char *modelDir = "models\\";
					strncpy( fileName, strstr( fileName, modelDir ), 512 );
					strncpy( fileName, fileName + strlen( modelDir ), 512 );

					pszModelNameCache = realloc( pszModelNameCache, ( iModelNameCacheSize + 1 ) * sizeof( char* ) );
					pszModelNameCache[iModelNameCacheSize] = fileName;
					iModelNameCacheSize++;

					iLooseModelCount++;
				}
				else if ( StrEq( extension, "wav" ) || StrEq( extension, "mp3" ) )
				{
					const char *soundDir = "sound\\";
					strncpy( fileName, strstr( fileName, soundDir ), 512 );
					strncpy( fileName, fileName + strlen( soundDir ), 512 );

					pszSoundNameCache = realloc( pszSoundNameCache, ( iSoundNameCacheSize + 1 ) * sizeof( char* ) );
					pszSoundNameCache[iSoundNameCacheSize] = fileName;
					iSoundNameCacheSize++;

					iLooseSoundCount++;
				}
			}
		}
	}

	FindClose( hFind );
	return success;
}

const char *BSPRand_GetRandomMaterial()
{
	if ( iMatNameCacheSize == 0 )
		return "";

	int iRand = rand() % iMatNameCacheSize;

	return pszMatNameCache[iRand];
}

const char *BSPRand_GetRandomModel()
{
	if ( iModelNameCacheSize == 0 )
		return "";

	int iRand = rand() % iModelNameCacheSize;

	return pszModelNameCache[iRand];
}

const char *BSPRand_GetRandomSound()
{
	if ( iSoundNameCacheSize == 0 )
		return "";

	int iRand = rand() % iSoundNameCacheSize;

	return pszSoundNameCache[iRand];
}

color32 BSPRand_GetRandomColor()
{
	color32 clr;
	clr.r = rand() % 256;
	clr.g = rand() % 256;
	clr.b = rand() % 256;
	clr.a = rand() % 256;
	return clr;
}

void BSPRand_ChangeHeader()
{
	Spew( "Changing header...\n" );

	Map_t *map = BSPRand_GetCurrentMap();

	// test manipulate header
	map->header.mapRevision = 1337;
}

BOOL BSPRand_EntityRandomizer()
{
	Spew( "Starting entity randomizer...\n" );

	Map_t *pMap = BSPRand_GetCurrentMap();

	BSPRand_ExtractEnts();

	if ( Main_IsVerbose() )
	{
		for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
		{
			Entity_t *ent = pCurrentMap->entities[i];
			if ( ent )
			{
				const char *classname = Entity_GetClassname( ent );
				Vector origin = Entity_GetOrigin( ent );
				QAngle angles = Entity_GetAngles( ent );

				Spew( "classname = %s\n", classname );
				Spew( "origin = %.0f, %.0f, %.0f\n", origin.x, origin.y, origin.z );
				Spew( "angles = %.0f, %.0f, %.0f\n", angles.x, angles.y, angles.z );
			}
		}
	}

	// TODO:
	// Randomize models
	Spew( "Randomizing models...\n" );
	for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent )
			continue;

		// All props
		const char *classname = Entity_GetClassname( ent );
		if ( StrContains( classname, "prop_" ) )
		{
			const char *model = Entity_KvGetValue( ent, "model" );
			if ( model[0] == '*' )
			{
				// Brush model, ignore for now
				continue;
			}

			// TODO: Physics props, need to be in separate array to prevent crashes
			if ( StrContains( classname, "prop_physics" ) )
				continue;

			const char *randModel = BSPRand_GetRandomModel();
			Entity_KvSetString( ent, "model", randModel );

			color32 randColor = BSPRand_GetRandomColor();
			Entity_KvSetColor( ent, "rendercolor", randColor );
		}
	}

	// Randomize sounds
	Spew( "Randomizing sounds...\n" );
	if ( iSoundNameCacheSize != 0 )
	{
		for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
		{
			Entity_t *ent = pCurrentMap->entities[i];
			if ( !ent )
				continue;

			if ( StrEq( Entity_GetClassname( ent ), "ambient_generic" ) ||
				StrEq( Entity_GetClassname( ent ), "ambient_fmod" ) ) // PVK2 has these :)
			{
				// Generic ambient sounds
				const char *randSound = BSPRand_GetRandomSound();
				Entity_KvSetString( ent, "message", randSound );
			}
			else if ( StrEq( Entity_GetClassname( ent ), "func_door" ) ||
				StrEq( Entity_GetClassname( ent ), "func_door_rotating" ) )
			{
				// Doors, randomize open & close sounds
				const char *randSound1 = BSPRand_GetRandomSound();
				const char *randSound2 = BSPRand_GetRandomSound();

				const char *randCloseSound1 = BSPRand_GetRandomSound();
				const char *randCloseSound2 = BSPRand_GetRandomSound();

				Entity_KvSetString( ent, "noise1", randSound1 );
				Entity_KvSetString( ent, "noise2", randSound2 );

				Entity_KvSetString( ent, "startclosesound", randCloseSound1 );
				Entity_KvSetString( ent, "closesound", randCloseSound2 );

				// Always loop moving sound for lulz
				Entity_KvSetBool( ent, "loopmovesound", TRUE );
			}
		}
	}

	// Randomize lights
	// TODO:
	/*
	Spew( "Randomizing lights...\n" );
	for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent )
			continue;

		if ( StrEq( Entity_GetClassname( ent ), "light" ) )
		{

		}
	}
	*/

	// Randomize rotating brushes (func_rotating)
	Spew( "Randomizing rotating brushes...\n" );
	for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent || !StrEq( Entity_GetClassname( ent ), "func_rotating" ) )
			continue;

		int rotationSpeed = rand() % 10000 + 1;
		int friction = rand() % 100 + 1;
		const char *sound = BSPRand_GetRandomSound();
		int volume = rand() % 10 + 1;

		Entity_KvSetInt( ent, "maxspeed", rotationSpeed );
		Entity_KvSetInt( ent, "fanfriction", friction );
		Entity_KvSetString( ent, "message", sound );
		Entity_KvSetInt( ent, "volume", volume );
	}

	// Set changelevel trigger destinations to randomized (_r) maps
	for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent || !StrEq( Entity_GetClassname( ent ), "trigger_changelevel" ) )
			continue;

		char buf[256];
		strcpy_s( buf, sizeof( buf ), Entity_KvGetString( ent, "map" ) );
		strcat_s( buf, sizeof( buf ), "_r" );
		Entity_KvSetString( ent, "map", buf );
	}

	// Randomize NPCs (game specific)
	if ( StrEq( Main_GetModName(), "hl2" ) ) // Half-Life 2
	{
		Spew( "Randomizing NPCs for Half-Life 2...\n" );

		const char *szHL2NPCList[] =
		{
			"npc_alyx",
			"npc_antlion",
			"npc_antlionguard",
			"npc_barnacle",
			"npc_barney",
			"npc_breen",
			"npc_citizen",
			"npc_clawscanner",
			"npc_combine_s",
			"npc_combinegunship",
			"npc_crow",
			"npc_dog",
			"npc_eli",
			"npc_fastzombie",
			"npc_gman",
			"npc_headcrab",
			"npc_headcrab_black",
			"npc_headcrab_fast",
			"npc_helicopter",
			"npc_kleiner",
			"npc_manhack",
			"npc_metropolice",
			"npc_monk",
			"npc_mossman",
			"npc_pigeon",
			"npc_poisonzombie",
			"npc_rollermine",
			"npc_seagull",
			"npc_stalker",
			"npc_strider",
			"npc_vortigaunt",
			"npc_zombie",
		};

		NPCList_t npcs;
		npcs.list = szHL2NPCList;
		npcs.count = ARRAYSIZE( szHL2NPCList );

		// For generic/cycler actors
		const char *szHL2ActorModelList[] =
		{
			"models/alyx.mdl",
			"models/barney.mdl",
			"models/breen.mdl",
			"models/combine_soldier.mdl",
			"models/eli.mdl",
			"models/gman.mdl",
			"models/kleiner.mdl",
			"models/mossman.mdl",
			"models/odessa.mdl",
			"models/police.mdl",
			"models/vortigaunt.mdl"
		};

		NPCList_t actorModels;
		actorModels.list = szHL2ActorModelList;
		actorModels.count = ARRAYSIZE( szHL2ActorModelList );

		// Story protected NPCs won't be randomized by default
		const char *szHL2StoryProtectedNPCs[] =
		{
			"npc_alyx",
			"npc_barney",
			"npc_breen",
			"npc_citizen",
			"npc_dog",
			"npc_eli",
			"npc_gman",
			"npc_kleiner",
			"npc_monk",
			"npc_mossman"
		};

		NPCList_t storyProtected;
		storyProtected.list = szHL2StoryProtectedNPCs;
		storyProtected.count = ARRAYSIZE( szHL2StoryProtectedNPCs );

		BSPRand_RandomizeNPCs( npcs, actorModels, storyProtected, NPCLIST_HL2 );
	}
	else if ( StrEq( Main_GetModName(), "hl1" ) ) // Half-Life: Source
	{
		Spew( "Randomizing NPCs for Half-Life: Source...\n" );

		const char *szHL1NPCList[] =
		{
			"monster_alien_controller",
			"monster_alien_grunt",
			"monster_alien_slave",
			//"monster_apache",
			"monster_babycrab",
			"monster_barnacle",
			"monster_barney",
			"monster_bigmomma",
			"monster_bullchicken",
			"monster_cockroach",
			"monster_gargantua",
			"monster_gman",
			"monster_headcrab",
			"monster_houndeye",
			"monster_human_assassin",
			"monster_human_grunt",
			//"monster_ichthyosaur",
			//"monster_leech",
			//"monster_miniturret",
			"monster_nihilanth",
			//"monster_osprey",
			"monster_scientist",
			//"monster_sentry",
			"monster_snark",
			"monster_tentacle",
			//"monster_turret",
			"monster_zombie"
		};

		NPCList_t npcs;
		npcs.list = szHL1NPCList;
		npcs.count = ARRAYSIZE( szHL1NPCList );

		// No actor models in HL1
		NPCList_t actorModels;
		actorModels.list = NULL;
		actorModels.count = 0;

		// Story protected NPCs won't be randomized by default
		const char *szHL1StoryProtectedNPCs[] =
		{
			"monster_barney",
			"monster_gman",
			"monster_scientist",
		};

		NPCList_t storyProtected;
		storyProtected.list = szHL1StoryProtectedNPCs;
		storyProtected.count = ARRAYSIZE( szHL1StoryProtectedNPCs );

		BSPRand_RandomizeNPCs( npcs, actorModels, storyProtected, NPCLIST_HL1 );
	}
	else if ( StrEq( Main_GetModName(), "bms" ) ) // Black Mesa
	{
		Spew( "Randomizing NPCs for Black Mesa...\n" );

		const char *szBMSNPCList[] =
		{
			"npc_alien_controller",
			"npc_alien_grunt",
			"npc_alien_grunt_unarmored"
			"npc_alien_slave",
			"npc_babycrab",
			"npc_barnacle",
			"npc_barney",
			"npc_bullsquid",
			"npc_cockroach",
			"npc_gargantua",
			"npc_gonarch",
			"npc_gman",
			"npc_headcrab",
			"npc_houndeye",
			"npc_human_assassin",
			"npc_human_commander",
			"npc_human_grenadier",
			"npc_human_grunt",
			"npc_human_medic",
			"npc_human_security",
			"npc_human_scientist_kleiner",
			"npc_human_scientist_eli",
			"npc_human_scientist",
			"npc_human_scientist_female",
			"npc_snark",
			"npc_tentacle",
			"npc_zombie_scientist",
			"npc_zombie_scientist_torso",
			"npc_zombie_security",
			"npc_zombie_grunt",
			"npc_zombie_grunt_torso",
		};

		NPCList_t npcs;
		npcs.list = szBMSNPCList;
		npcs.count = ARRAYSIZE( szBMSNPCList );

		// TODO:
		NPCList_t actorModels;
		actorModels.list = NULL;
		actorModels.count = 0;

		// Story protected NPCs won't be randomized by default
		const char *szBMSStoryProtectedNPCs[] =
		{
			"npc_barney",
			"npc_gman",
			"npc_human_security",
			"npc_human_scientist_kleiner",
			"npc_human_scientist_eli",
			"npc_human_scientist",
			"npc_human_scientist_female",
		};

		NPCList_t storyProtected;
		storyProtected.list = szBMSStoryProtectedNPCs;
		storyProtected.count = ARRAYSIZE( szBMSStoryProtectedNPCs );

		BSPRand_RandomizeNPCs( npcs, actorModels, storyProtected, NPCLIST_BMS );
	}

	BSPRand_BuildEntBuffer();

	Spew( "Entities randomized!\n" );
	return TRUE;
}

void BSPRand_RandomizeNPCs( NPCList_t npcs, NPCList_t actorModels, NPCList_t storyProtected, int game )
{
	if ( !npcs.list || npcs.count == 0 )
		return;

	for ( int i = 0; i < pCurrentMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent )
			continue;

		if ( game != NPCLIST_HL1 && StrContains( Entity_GetClassname( ent ), "npc_" ) ||
			game == NPCLIST_HL1 && StrContains( Entity_GetClassname( ent ), "monster_" ) )
		{
			BOOL canRandomize = TRUE;
			if ( !Main_IgnoreStoryNPCs() && storyProtected.list )
			{
				for ( int i = 0; i < storyProtected.count; i++ )
				{
					// Story protected?
					if ( StrEq( Entity_GetClassname( ent ), storyProtected.list[i] ) )
						canRandomize = FALSE;
				}
			}

			if ( canRandomize )
			{
				int iRand = rand() % npcs.count;
				const char *randNPC = npcs.list[iRand];
				Entity_KvSetString( ent, "classname", randNPC );
			}
		}
		else if ( actorModels.list && actorModels.count != 0 &&
			StrEq( Entity_GetClassname( ent ), "generic_actor" ) ||
			StrEq( Entity_GetClassname( ent ), "cycler_actor" ) )
		{
			int iRand = rand() % actorModels.count;
			const char *randModel = actorModels.list[iRand];
			Entity_KvSetString( ent, "model", randModel );
		}
	}
}

BOOL BSPRand_ExtractEnts()
{
	Spew( "Extracting entities...\n" );

	Map_t *pMap = BSPRand_GetCurrentMap();

	// extract ents from the ASCII buffer
	const char *pszEntBuffer = pMap->entitiesBuffer;
	char line[256];
	int len = strlen( pszEntBuffer );
	int cur = 0;
	BOOL endBracketFound = FALSE;

	Entity_t **ents = NULL;
	Entity_t *curEnt = NULL;
	int entCount = 0;

	int kvCount = 0;

	while ( cur < len )
	{
		// get line
		int lineLen = GetLineFromString( &pszEntBuffer[cur], line, sizeof( line ) );

		if ( line[0] == '\0' || line[0] == '\n' )
		{
			cur++;
			continue;
		}

		// do stuff with the line
		if ( line[0] == '{' )
		{
			if ( curEnt )
				free( curEnt );

			// new entity
			curEnt = malloc( sizeof( Entity_t ) );
			curEnt->keyValues = NULL;

			endBracketFound = FALSE;
		}
		else if ( line[0] == '}' )
		{
			// put entity in list
			if ( curEnt )
			{
				entCount++;
				ents = realloc( ents, entCount * sizeof( Entity_t* ) );
				ents[entCount - 1] = curEnt;
				curEnt = NULL;
				kvCount = 0;
			}

			endBracketFound = TRUE;
		}
		else if ( !endBracketFound && curEnt )
		{
			// get keyvalue
			KeyValue_t *kv = malloc( sizeof( KeyValue_t ) );
			int set = 0;
			int start = -1;
			int end = -1;

			// loop through line to find quotes
			for ( int i = 0; i < lineLen + 1; i++ )
			{
				if ( line[i] == '"' )
				{
					if ( start == -1 ) // first '"'
					{
						start = i + 1;
					}
					else // second '"'
					{
						end = i;

						char *ptr;
						if ( set == 0 )
						{
							// key
							ptr = kv->key;
							set = 1;
						}
						else
						{
							// value
							ptr = kv->value;
							set = 2;
						}

						// slice string
						int len = strlen( line );
						int loc = 0;
						for ( int j = start; j < len; j++ )
						{
							if ( j == end )
							{
								ptr[loc] = '\0';
								break;
							}

							ptr[loc] = line[j];
							loc++;
						}

						ptr[loc] = '\0';

						if ( set == 2 )
						{
							// finished
							break;
						}
						else if ( set == 1 )
						{
							// key set
							start = -1;
							end = -1;
						}
					}
				}
			}

			// add kv to list
			kvCount++;
			curEnt->keyValues = realloc( curEnt->keyValues, kvCount * sizeof( KeyValue_t* ) );
			curEnt->keyValues[kvCount - 1] = kv;
			curEnt->kvCount = kvCount;
		}

		// into next
		cur += lineLen + 1;
	}

	pMap->entities = ents;
	pMap->entitiesCount = entCount;

	Spew( "%d entities extracted\n", entCount );
	return TRUE;
}

BOOL BSPRand_BuildEntBuffer()
{
	Spew( "Building entity buffer...\n" );

	Map_t *pMap = BSPRand_GetCurrentMap();

	if ( pMap->entitiesBuffer )
		free( pMap->entitiesBuffer );

	int totalSize = 0;
	pMap->entitiesBuffer = NULL;

	int cur = -1;
	for ( int i = 0; i < pMap->entitiesCount; i++ )
	{
		Entity_t *ent = pCurrentMap->entities[i];
		if ( !ent )
			continue;

		// start bracket
		totalSize += 2;
		pMap->entitiesBuffer = realloc( pMap->entitiesBuffer, totalSize );
		pMap->entitiesBuffer[++cur] = '{';
		pMap->entitiesBuffer[++cur] = '\n';

		for ( int j = 0; j < ent->kvCount; j++ )
		{
			KeyValue_t *kv = ent->keyValues[j];
			if ( !kv )
				continue;

			char line[256];
			sprintf_s( line, sizeof( line ), "\"%s\" \"%s\"", kv->key, kv->value );

			totalSize += strlen( line ) + 1;
			pMap->entitiesBuffer = realloc( pMap->entitiesBuffer, totalSize );

			// append
			char *dest = &pMap->entitiesBuffer[++cur];
			const char *src = line;
			int len = strlen( src );

			while ( len-- )
			{
				if ( cur >= totalSize - 1 )
					break;

				*dest++ = *src++;
				cur++;
			}

			pMap->entitiesBuffer[cur] = '\n';
		}

		// end bracket
		totalSize += 2;
		pMap->entitiesBuffer = realloc( pMap->entitiesBuffer, totalSize );
		pMap->entitiesBuffer[++cur] = '}';
		pMap->entitiesBuffer[++cur] = '\n';
	}

	// null terminate
	totalSize++;
	pMap->entitiesBuffer = realloc( pMap->entitiesBuffer, totalSize );
	pMap->entitiesBuffer[++cur] = '\0';

	pMap->entitiesBufferSize = totalSize;

	return TRUE;
}

BOOL BSPRand_TextureRandomizer()
{
	Spew( "Starting texture randomizer...\n" );

	Map_t *map = BSPRand_GetCurrentMap();

	if ( iMatNameCacheSize != 0 && !Main_BSPTexturesOnly() )
	{
		// recreate the string table from random material names
		int stringTableSize = map->texdataStringTableCount;
		StringTableItem_t **stringTable = malloc( stringTableSize * sizeof( StringTableItem_t* ) );

		int totalLen = -1;
		for ( int i = 0; i < stringTableSize; i++ )
		{
			StringTableItem_t *item = malloc( sizeof( StringTableItem_t ) );

			// we put random string in here, also fixing up indices
			item->data = BSPRand_GetRandomMaterial();
			item->index = totalLen + 1;
			stringTable[i] = item;

			totalLen += strlen( item->data ) + 1;

			V_Spew( "Adding %s to the string table (idx: %d)\n", item->data, item->index );
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
