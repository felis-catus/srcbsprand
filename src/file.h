#ifndef FILE_H
#define FILE_H

BOOL File_Init( const char *filename );

BOOL File_CopyBSP( const char *filename );
BOOL File_IsValid( FILE *file );
BOOL File_ReadBSP( const char *filename );

void File_ReadHeader( FILE *file, Map_t *map );
void File_ReadEntityList( FILE *file, Map_t *map );
void File_ReadTextures( FILE *file, Map_t *map );
void File_ReadModels( FILE *file, Map_t *map );
void File_ReadDisplacements( FILE *file, Map_t *map );
void File_ReadOverlays( FILE *file, Map_t *map );
void File_ReadLights( FILE *file, Map_t *map );

BOOL File_WriteBSP( const char *filename );

void File_WriteHeader( FILE *file, Map_t *map );
void File_WriteEntityList( FILE *file, Map_t *map );
void File_WriteTextures( FILE *file, Map_t *map );
void File_WriteModels( FILE *file, Map_t *map );
void File_WriteDisplacements( FILE *file, Map_t *map );
void File_WriteOverlays( FILE *file, Map_t *map );
void File_WriteLights( FILE *file, Map_t *map );

char *File_GetPrintableStringTable( const char *strtable, int size );

#endif
