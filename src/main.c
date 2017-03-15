// Defines the entry point for the console application.
//

#include "stdafx.h"

//
// Vars
//
BOOL bVerbose;
char szOutputDir[MAX_PATH * 2];
unsigned int uSeed;
char szGameDir[MAX_PATH * 2];
char szModName[256];
BOOL bBSPTexturesOnly;
BOOL bIgnoreStoryNPCs;
BOOL bDumpEntList;
BOOL bDumpTexList;
BOOL bDumpStrTable;
clock_t clockStart;

BOOL Main_IsVerbose() { return bVerbose; }
const char *Main_GetOutputDir() { return szOutputDir; }
const char *Main_GetGameDir() { return szGameDir; }
const char *Main_GetModName() { return szModName; }
BOOL Main_BSPTexturesOnly() { return bBSPTexturesOnly; }
BOOL Main_IgnoreStoryNPCs() { return bIgnoreStoryNPCs; };
BOOL Main_ShouldDumpEntList() { return bDumpEntList; }
BOOL Main_ShouldDumpTexList() { return bDumpTexList; }
BOOL Main_ShouldDumpStringTable() { return bDumpStrTable; }
//
//
//

int Main_Execute( int argc, char *argv[] )
{
	if ( !Main_Init() )
		return -1;
	
	const char *separator = "========================\n";

	Spew( "Source BSP Randomizer 0.1\n" );

	Spew( separator );

	const char *filename = argv[1];

	if ( !filename )
	{
		Spew( "\nUsage:\nDrag n drop .bsp into the .exe and I'll mess it up!\nOr just use cmd\nDon't worry, the original .bsp will not be modified.\n\n" );
		Spew( "Args:\n%s%s%s%s%s%s%s",
			"-out, -output : Output directory.",
			"-v, -verbose : (Very) verbose output.\n",
			"-seed : Random seed, eg. -seed 133769420\n",
			"-game, -gamedir : Location for the Source game folder you wish to get materials from.\n",
			"-bsptexonly : Texture randomizer will only use textures inside the BSP. If no gamedir is available this will be used by default.\n",
			"-ignorestorynpcs : \"Story protected\" NPCs will be randomized.",
			"-dumpentlist : Dump entity list to the same dir as your BSP.\n",
			"-dumptexlist : Same as above, but for texture list.\n",
			"-dumpstrtable : Dumps both input and output string tables.\n" );

#ifdef _DEBUG
		system( "pause" );
#endif
		return 0;
	}

	Spew( "Selected file: %s\n", filename );

	// TODO: Need some generic config for BSPRand instead of cmd line, there will be a lot of options
	for ( int i = 1; i < argc; i++ )
	{
		if ( StrEq( argv[i], "-out" ) || StrEq( argv[i], "-output" ) )
		{
			strcpy_s( szOutputDir, sizeof( szOutputDir ), argv[i + 1] );
		}
		else if ( StrEq( argv[i], "-v" ) || StrEq( argv[i], "-verbose" ) )
		{
			bVerbose = TRUE;
			Spew( "bVerbose = true\n" );
		}
		else if ( StrEq( argv[i], "-seed" ) )
		{
			uSeed = strtol( argv[i + 1], NULL, 10 );
			Spew( "Seed: %d\n", uSeed );
		}
		else if ( StrEq( argv[i], "-game" ) || StrEq( argv[i], "-gamedir" ) )
		{
			strcpy_s( szGameDir, sizeof( szGameDir ), argv[i + 1] );
			Spew( "Using gamedir \"%s\"\n", szGameDir );

			const char *pToLastDirName = NULL;
			int len = strlen( szGameDir );

			if ( szGameDir[len - 1] == '\\' || szGameDir[len - 1] == '/' )
				len--; // trailing slash

			while ( len > 0 )
			{
				if ( szGameDir[len - 1] == '\\' || szGameDir[len - 1] == '/' )
				{
					pToLastDirName = &szGameDir[len];
					break;
				}

				len--;
			}

			strcpy_s( szModName, sizeof( szModName ), pToLastDirName );

			bBSPTexturesOnly = FALSE;
		}
		else if ( StrEq( argv[i], "-bsptexonly" ) )
		{
			bBSPTexturesOnly = TRUE;
			Spew( "bBSPTexturesOnly = true\n" );
		}
		else if ( StrEq( argv[i], "-ignorestorynpcs" ) )
		{
			bIgnoreStoryNPCs = TRUE;
			Spew( "bIgnoreStoryNPCs = true\n" );
		}
		else if ( StrEq( argv[i], "-dumpentlist" ) )
		{
			bDumpEntList = TRUE;
			Spew( "bDumpEntList = true\n" );
		}
		else if ( StrEq( argv[i], "-dumptexlist" ) )
		{
			bDumpTexList = TRUE;
			Spew( "bDumpTexList = true\n" );
		}
		else if ( StrEq( argv[i], "-dumpstrtable" ) )
		{
			bDumpStrTable = TRUE;
			Spew( "bDumpStrTable = true\n" );
		}
	}

	BSPRand_CreateMap();

	char name[1024];
	strncpy( name, filename, 1024 );
	name[strlen( filename ) - 4] = 0;

	char destname[1024];
	if ( szOutputDir[0] != '\0' )
	{
		char szFile[MAX_PATH];
		StripExtension( GetFileName( filename ), szFile, sizeof( szFile ) );
		_snprintf( destname, sizeof( destname ), "%s\\%s_r.bsp", szOutputDir, szFile );
	}
	else
	{
		_snprintf( destname, sizeof( destname ), "%s_r.bsp", name );
	}

	Spew( "Output: %s\n", destname );

	if ( !File_Init( argv[1], destname ) )
	{
		Spew( "BSP READ FAIL!!! Exiting...\n" );
#ifdef _DEBUG
		system( "pause" );
#endif
		return -1;
	}

	Spew( separator );

	Spew( "Randomizer init!\n" );

	if ( uSeed == 0 )
	{
		uSeed = (unsigned int)time( NULL );
		Spew( "Using unix time as seed (%d)\n", uSeed );
	}

	BSPRand_SetRandomSeed( uSeed );

	if ( szGameDir[0] == 0 )
	{
		Spew( "No game dir, using textures inside BSP\n" );
		bBSPTexturesOnly = TRUE;
	}

	if ( !BSPRand_Init() )
	{
		Spew( "RANDOMIZER FAIL!!! Exiting...\n" );
#ifdef _DEBUG
		system( "pause" );
#endif
		return -1;
	}

	Spew( separator );

	if ( !File_WriteBSP( destname ) )
	{
		Spew( "BSP WRITE FAIL!!! Exiting...\n" );
#ifdef _DEBUG
		system( "pause" );
#endif
		return -1;
	}

	Spew( separator );

	Spew( "Done!\n" );

	float time = ( ( (float)clock() ) - clockStart ) / CLOCKS_PER_SEC;
	Spew( "%f seconds elapsed.\n", time );

	Main_Cleanup();

#ifdef _DEBUG
	system( "pause" );
#endif
	return 0;
}

BOOL Main_Init()
{
	uSeed = 0;
	szOutputDir[0] = '\0';
	szGameDir[0] = '\0';
	bBSPTexturesOnly = FALSE;
	bDumpEntList = FALSE;
	bDumpTexList = FALSE;

	clockStart = clock();

	return TRUE;
}

void Main_Cleanup()
{
	BSPRand_Cleanup();
}

int main( int argc, char *argv[] )
{
	return Main_Execute( argc, argv );
}
