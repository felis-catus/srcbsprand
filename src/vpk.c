#include "stdafx.h"

const char *VPK_ReadString( FILE *file )
{
	static char string[256];
	memset( string, '\0', 256 );

	for ( int i = 0; i < 256; i++ )
	{
		char ch;
		fread( &ch, 1, 1, file );

		if ( ch == '\0' )
			break;

		string[i] = ch;
	}

	return string;
}

char *VPK_ReadAndAllocString( FILE *file )
{
	char *string = malloc( 256 );
	memset( string, '\0', 256 );

	for ( int i = 0; i < 256; i++ )
	{
		char ch;
		fread( &ch, 1, 1, file );

		if ( ch == '\0' )
			break;

		string[i] = ch;
	}

	return string;
}
