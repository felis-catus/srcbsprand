// Defines the entry point for the console application.
//

#include "stdafx.h"

//
// Vars
//
BOOL bVerbose;
int iSeed;
char szGameDir[MAX_PATH * 2];
BOOL bBSPTexturesOnly;
BOOL bDumpEntList;
BOOL bDumpTexList;
BOOL bDumpStrTable;
clock_t clockStart;

BOOL Main_IsVerbose() { return bVerbose; }
int Main_GetSeed() { return iSeed; }
char *Main_GetGameDir() { return szGameDir; }
BOOL Main_BSPTexturesOnly() { return bBSPTexturesOnly; }
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
			"-v, -verbose : (Very) verbose output.\n",
			"-seed : Random seed, eg. -seed 133769420\n",
			"-gamedir : Location for the Source game folder you wish to get materials from.\n",
			"-bsptexonly : Texture randomizer will only use textures inside the BSP. If no gamedir is available this will be used by default.\n",
			"-dumpentlist : Dump entity list to the same dir as your BSP.\n",
			"-dumptexlist : Same as above, but for texture list.\n",
			"-dumpstrtable : Dumps both input and output string tables.\n" );

		return 0;
	}

	Spew( "Selected file: %s\n", filename );

	for ( int i = 1; i < argc; i++ )
	{
		if ( StrEq( argv[i], "-v" ) || StrEq( argv[i], "-verbose" ) )
		{
			bVerbose = TRUE;
			Spew( "bVerbose = true\n" );
		}
		else if ( StrEq( argv[i], "-seed" ) )
		{
			iSeed = strtol( argv[i + 1], NULL, 10 );
			Spew( "Seed: %d\n", iSeed );
		}
		else if ( StrEq( argv[i], "-gamedir" ) )
		{
			strcpy_s( szGameDir, sizeof( szGameDir ), argv[i + 1] );
			Spew( "Using gamedir \"%s\"\n", szGameDir );

			bBSPTexturesOnly = FALSE;
			Spew( "bBSPTexturesOnly = false\n" );
		}
		else if ( StrEq( argv[i], "-bsptexonly" ) )
		{
			bBSPTexturesOnly = TRUE;
			Spew( "bBSPTexturesOnly = true\n" );
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

	if ( !File_Init( argv[1] ) )
	{
		Spew( "BSP READ FAIL!!! Exiting...\n" );
		return -1;
	}

	Spew( separator );

	Spew( "Randomizer init!\n" );

	if ( iSeed == 0 )
	{
		iSeed = (int)time( 0 );
		Spew( "Using unix time as seed (%d)\n", iSeed );
	}

	BSPRand_SetRandomSeed( iSeed );

	if ( szGameDir[0] == 0 )
	{
		Spew( "No game dir, using textures inside BSP\n" );
		bBSPTexturesOnly = TRUE;
	}

	if ( !BSPRand_Init() )
	{
		Spew( "RANDOMIZER FAIL!!! Exiting...\n" );
		return -1;
	}

	char name[1024];
	strncpy( name, argv[1], 1024 );
	name[strlen( argv[1] ) - 4] = 0;

	char destname[1024];
	_snprintf( destname, sizeof( destname ), "%s_r.bsp", name );

	Spew( separator );

	Spew( "Output: %s\n", destname );

	if ( !File_WriteBSP( destname ) )
	{
		Spew( "BSP WRITE FAIL!!! Exiting...\n" );
		return -1;
	}

	Spew( separator );

	Spew( "Done!\n" );

	float time = ( ( (float)clock() ) - clockStart ) / CLOCKS_PER_SEC;
	Spew( "%f seconds elapsed.\n", time );

	Main_Cleanup();

	return 0;
}

BOOL Main_Init()
{
	iSeed = 0;
	memset( szGameDir, 0, sizeof( szGameDir ) );
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
