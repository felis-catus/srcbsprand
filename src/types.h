#ifndef TYPES_H
#define TYPES_H

// Adding types here when I need them...
/*
typedef int BOOL;
#define FALSE 0
#define TRUE 1
*/
typedef unsigned char byte;
typedef unsigned short word;
typedef float vec_t;

typedef struct
{
	vec_t x, y, z;
} Vector;

typedef struct
{
	vec_t x, y, z;
} QAngle;

typedef struct
{
	byte r, g, b, a;
} color32;

#define MAX_PATH 260

#endif
