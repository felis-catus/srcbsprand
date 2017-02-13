#ifndef UTIL_H
#define UTIL_H

// Normal spew
void Spew( const char *format, ... );
void WSpew( wchar_t *format, ... );

// Verbose spew
void V_Spew( const char *format, ... );
void V_WSpew( wchar_t *format, ... );

// Misc useful
BOOL StrEq( const char *sz1, const char *sz2 );
BOOL WStrEq( wchar_t *wcs1, wchar_t *wcs2 );
BOOL StrContains( const char *sz1, const char *sz2 );
BOOL WStrContains( wchar_t *wcs1, wchar_t *wcs2 );

void FixSlashes( char *pname, char separator );
void WFixSlashes( wchar_t *pname, char separator );

const char *VarArgs( const char *format, ... );

BOOL PATHSEPARATOR( char c );

const char *GetFileExtension( const char *path );

void StripExtension( const char *in, char *out, int outSize );

int GetLineFromString( const char *in, char *out, int outSize );

#endif
