#include "stdafx.h"

// Normal spew
void Spew( const char *format, ... )
{
	va_list	argptr;
	static char	string[1024];

	va_start( argptr, format );
	vsnprintf_s( string, 1024, 1024, format, argptr );
	va_end( argptr );

	printf_s( string );
	OutputDebugStringA( string );
}

void WSpew( wchar_t *format, ... )
{
	va_list	argptr;
	static wchar_t string[1024];

	va_start( argptr, format );
	vwprintf_s( string, argptr );
	va_end( argptr );

	wprintf_s( string );
	OutputDebugStringW( string );
}

// Verbose spew
void V_Spew( const char *format, ... )
{
	if ( !Main_IsVerbose() )
		return;

	va_list	argptr;
	static char	string[1024];

	va_start( argptr, format );
	vsnprintf_s( string, 1024, 1024, format, argptr );
	va_end( argptr );

	printf_s( string );
	OutputDebugStringA( string );
}

void V_WSpew( wchar_t *format, ... )
{
	if ( !Main_IsVerbose() )
		return;

	va_list	argptr;
	static wchar_t string[1024];

	va_start( argptr, format );
	vwprintf_s( string, argptr );
	va_end( argptr );

	wprintf_s( string );
	OutputDebugStringW( string );
}

// Misc useful
BOOL StrEq( const char *sz1, const char *sz2 )
{
	return ( sz1 == sz2 || _stricmp( sz1, sz2 ) == 0 );
}

BOOL WStrEq( wchar_t *wcs1, wchar_t *wcs2 )
{
	return ( wcs1 == wcs2 || wcscmp( wcs1, wcs2 ) == 0 );
}

BOOL StrContains( const char *sz1, const char *sz2 )
{
	return ( strstr( sz1, sz2 ) != NULL );
}

BOOL WStrContains( wchar_t *wcs1, wchar_t *wcs2 )
{
	return ( wcsstr( wcs1, wcs2 ) != NULL );
}

void FixSlashes( char *pname, char separator )
{
	while ( *pname )
	{
		if ( *pname == '/' || *pname == '\\' )
		{
			*pname = separator;
		}
		pname++;
	}
}

void WFixSlashes( wchar_t *pname, char separator )
{
	while ( *pname )
	{
		if ( *pname == L'/' || *pname == L'\\' )
		{
			*pname = separator;
		}
		pname++;
	}
}

const char *VarArgs( const char *format, ... )
{
	va_list	argptr;
	static char	string[1024];

	va_start( argptr, format );
	vsnprintf_s( string, 1024, 1024, format, argptr );
	va_end( argptr );

	return string;
}

BOOL PATHSEPARATOR( char c )
{
	return c == '\\' || c == '/';
}

const char *GetFileExtension( const char *path )
{
	const char *src;

	src = path + strlen( path ) - 1;

	while ( src != path && *( src - 1 ) != '.' )
		src--;

	if ( src == path || PATHSEPARATOR( *src ) )
	{
		return NULL;
	}

	return src;
}

void StripExtension( const char *in, char *out, int outSize )
{
	int end = strlen( in ) - 1;
	while ( end > 0 && in[end] != '.' && !PATHSEPARATOR( in[end] ) )
	{
		--end;
	}

	if ( end > 0 && !PATHSEPARATOR( in[end] ) && end < outSize )
	{
		int nChars = min( end, outSize - 1 );
		if ( out != in )
		{
			memcpy( out, in, nChars );
		}
		out[nChars] = 0;
	}
	else
	{
		if ( out != in )
		{
			strncpy( out, in, outSize );
		}
	}
}

int GetLineFromString( const char *in, char *out, int outSize )
{
	strncpy( out, in, outSize );

	int len = strlen( in ) + 1;
	for ( int i = 0; i < len; i++ )
	{
		if ( in[i] == '\n' )
		{
			out[i] = '\0';
			break;
		}
	}

	int retLen = strlen( out ) - 1;
	return retLen;
}
