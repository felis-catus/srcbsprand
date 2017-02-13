#ifndef BSPRAND_H
#define BSPRAND_H

#define MAX_STRINGDATA 256000 // move this
#define	MAX_MAP_ENTITIES 8192 // move this too

// texture struct
typedef struct
{
	char *name; // texture name
	int idx; // general idx, can get texinfo with it
	int dataidx; // texdata index
} Texture_t;

typedef struct
{
	int index;
	char *data;
} StringTableItem_t;

// Single entity, not the buffer!
typedef struct
{
	//char *classname;
	KeyValue_t **keyValues;
	int kvCount;
} Entity_t;

// The loaded map
typedef struct
{
	BSPHeader_t header;
	BSPVersion version;

	Entity_t **entities;
	int entitiesCount;
	char *entitiesBuffer;
	int entitiesBufferSize;

	Texture_t **textures;
	int textureCount;

	texinfo_t **texinfo;
	int texinfoCount;

	dtexdata_t **texdata;
	int texdataCount;

	char *texdataStringData;
	int texdataStringDataSize;

	int *texdataStringTable;
	int texdataStringTableCount;

	dmodel_t **models;
	int modelCount;

	ddispinfo_t **displacements;
	int dispCount;

	doverlay_t **overlays;
	int overlayCount;

	ColorRGBExp32 **lights;
	int lightCount;
} Map_t;

extern Map_t *pCurrentMap;
extern int iTexdataCache[12288];
extern char **pszMatNameCache;
extern int iRandSeed;

BOOL BSPRand_Init();
void BSPRand_Cleanup();

void BSPRand_CreateMap();

void BSPRand_ChangeHeader();
BOOL BSPRand_ChangeTextures();
void BSPRand_ChangeModels();

BOOL BSPRand_BuildTextureList();
BOOL BSPRand_TextureRandomizer();

BOOL BSPRand_ExtractEnts();
BOOL BSPRand_EntityRandomizer();
BOOL BSPRand_BuildEntBuffer();

/* TODO: Move these */
const char *BSPRand_KvGetValue( Entity_t *ent, const char *key );
void BSPRand_KvSetValue( Entity_t *ent, const char *key, const char *value );
KeyValue_t *BSPRand_KvCreate( Entity_t *ent, const char *key, const char *value );

Vector BSPRand_GetEntOrigin( Entity_t *ent );
QAngle BSPRand_GetEntAngles( Entity_t *ent );

void BSPRand_SetEntOrigin( Entity_t *ent, Vector origin );
void BSPRand_SetEntAngles( Entity_t *ent, QAngle angles );
/*------------------*/

void BSPRand_ClearCaches();

int BSPRand_GetRandomSeed();
void BSPRand_SetRandomSeed( int seed );

Map_t *BSPRand_GetCurrentMap();
void BSPRand_SetCurrentMap( Map_t *map );

BOOL BSPRand_RecursiveTextureScan( char *current );
BOOL BSPRand_VPKTextureScan( char *dir );

#endif
