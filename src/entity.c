#include "stdafx.h"

const char *Entity_KvGetValue( Entity_t *ent, const char *key )
{
	if ( ent )
	{
		for ( int i = 0; i < ent->kvCount; i++ )
		{
			KeyValue_t *kv = ent->keyValues[i];
			if ( !kv )
				continue;

			if ( StrEq( kv->key, key ) )
				return kv->value;
		}
	}

	return "";
}

void Entity_KvSetValue( Entity_t *ent, const char *key, const char *value )
{
	if ( !ent )
		return;

	BOOL found = FALSE;

	for ( int i = 0; i < ent->kvCount; i++ )
	{
		KeyValue_t *kv = ent->keyValues[i];
		if ( !kv )
			continue;

		if ( StrEq( kv->key, key ) )
		{
			strncpy( kv->value, value, MAX_VALUE );
			found = TRUE;
		}
	}

	if ( !found )
	{
		// not found, create new
		Entity_KvCreate( ent, key, value );
	}
}

KeyValue_t *Entity_KvCreate( Entity_t *ent, const char *key, const char *value )
{
	if ( !ent )
		return NULL;

	KeyValue_t *kv = malloc( sizeof( KeyValue_t ) );
	strncpy( kv->key, key, MAX_KEY );
	strncpy( kv->value, value, MAX_VALUE );

	ent->kvCount++;
	ent->keyValues = realloc( ent->keyValues, ent->kvCount * sizeof( KeyValue_t* ) );
	ent->keyValues[ent->kvCount - 1] = kv;
	return kv;
}

Vector Entity_GetOrigin( Entity_t *ent )
{
	Vector vec;
	vec.x = 0;
	vec.y = 0;
	vec.z = 0;

	if ( ent )
	{
		const char *value = Entity_KvGetValue( ent, "origin" );
		if ( value[0] != 0 )
			sscanf( value, "%f %f %f", &vec.x, &vec.y, &vec.z );
	}

	return vec;
}

QAngle Entity_GetAngles( Entity_t *ent )
{
	QAngle ang;
	ang.x = 0;
	ang.y = 0;
	ang.z = 0;

	if ( ent )
	{
		const char *value = Entity_KvGetValue( ent, "angles" );
		if ( value[0] != 0 )
			sscanf( value, "%f %f %f", &ang.x, &ang.y, &ang.z );
	}

	return ang;
}

const char *Entity_GetClassname( Entity_t *ent )
{
	return Entity_KvGetValue( ent, "classname" );
}

void Entity_SetOrigin( Entity_t *ent, Vector origin )
{
	if ( ent )
	{
		char value[MAX_VALUE];
		sprintf_s( value, MAX_VALUE, "%f %f %f", origin.x, origin.y, origin.z );
		Entity_KvSetValue( ent, "origin", value );
	}
}

void Entity_SetAngles( Entity_t *ent, QAngle angles )
{
	if ( ent )
	{
		char value[MAX_VALUE];
		sprintf_s( value, MAX_VALUE, "%f %f %f", angles.x, angles.y, angles.z );
		Entity_KvSetValue( ent, "origin", value );
	}
}
