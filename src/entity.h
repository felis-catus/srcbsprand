#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
	KeyValue_t **keyValues;
	int kvCount;
} Entity_t;

const char *Entity_KvGetValue( Entity_t *ent, const char *key );
void Entity_KvSetValue( Entity_t *ent, const char *key, const char *value );
KeyValue_t *Entity_KvCreate( Entity_t *ent, const char *key, const char *value );

Vector Entity_GetOrigin( Entity_t *ent );
QAngle Entity_GetAngles( Entity_t *ent );
const char *Entity_GetClassname( Entity_t *ent );

void Entity_SetOrigin( Entity_t *ent, Vector origin );
void Entity_SetAngles( Entity_t *ent, QAngle angles );
#endif
