#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
	KeyValue_t **keyValues;
	int kvCount;
} Entity_t;

const char *Entity_KvGetValue( Entity_t *ent, const char *key );
const char *Entity_KvGetString( Entity_t *ent, const char *key );
int Entity_KvGetInt( Entity_t *ent, const char *key );
float Entity_KvGetFloat( Entity_t *ent, const char *key );
color32 Entity_KvGetColor( Entity_t *ent, const char *key );
BOOL Entity_KvGetBool( Entity_t *ent, const char *key );

void Entity_KvSetValue( Entity_t *ent, const char *key, const char *value );
void Entity_KvSetString( Entity_t *ent, const char *key, const char *value );
void Entity_KvSetInt( Entity_t *ent, const char *key, int value );
void Entity_KvSetFloat( Entity_t *ent, const char *key, float value );
void Entity_KvSetColor( Entity_t *ent, const char *key, color32 value );
void Entity_KvSetBool( Entity_t *ent, const char *key, BOOL value );

KeyValue_t *Entity_KvCreate( Entity_t *ent, const char *key, const char *value );

Vector Entity_GetOrigin( Entity_t *ent );
QAngle Entity_GetAngles( Entity_t *ent );
const char *Entity_GetClassname( Entity_t *ent );

void Entity_SetOrigin( Entity_t *ent, Vector origin );
void Entity_SetAngles( Entity_t *ent, QAngle angles );
#endif
