//=============================================================================//
//
// Portions of the code by Valve
// Thanks thanks so much thanks for Valve Dev Community for excellent documentation! 
// https://developer.valvesoftware.com/wiki/Source_BSP_File_Format
//
// Purpose: Defines and structures for the BSP file format.
//
//=============================================================================//

#ifndef BSP_H
#define BSP_H

// little-endian "VBSP"   0x50534256
#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'V')

#define	HEADER_LUMPS 64

// Common limits
// leaffaces, leafbrushes, planes, and verts are still bounded by
// 16 bit short limits
#define	MAX_MAP_MODELS					1024
#define	MAX_MAP_BRUSHES					8192
#define	MAX_MAP_ENTITIES				8192
#define	MAX_MAP_TEXINFO					12288
#define MAX_MAP_TEXDATA					2048
#define MAX_MAP_DISPINFO				2048
#define MAX_MAP_DISP_VERTS				( MAX_MAP_DISPINFO * ((1<<MAX_MAP_DISP_POWER)+1) * ((1<<MAX_MAP_DISP_POWER)+1) )
#define MAX_MAP_DISP_TRIS				( (1 << MAX_MAP_DISP_POWER) * (1 << MAX_MAP_DISP_POWER) * 2 )
#define MAX_DISPVERTS					NUM_DISP_POWER_VERTS( MAX_MAP_DISP_POWER )
#define MAX_DISPTRIS					NUM_DISP_POWER_TRIS( MAX_MAP_DISP_POWER )
#define	MAX_MAP_AREAS					256
#define MAX_MAP_AREA_BYTES				(MAX_MAP_AREAS/8)
#define	MAX_MAP_AREAPORTALS				1024
// Planes come in pairs, thus an even number.
#define	MAX_MAP_PLANES					65536
#define	MAX_MAP_NODES					65536
#define	MAX_MAP_BRUSHSIDES				65536
#define	MAX_MAP_LEAFS					65536
#define	MAX_MAP_VERTS					65536
#define MAX_MAP_VERTNORMALS				256000
#define MAX_MAP_VERTNORMALINDICES		256000
#define	MAX_MAP_FACES					65536
#define	MAX_MAP_LEAFFACES				65536
#define	MAX_MAP_LEAFBRUSHES 			65536
#define	MAX_MAP_PORTALS					65536
#define MAX_MAP_CLUSTERS				65536
#define MAX_MAP_LEAFWATERDATA			32768
#define MAX_MAP_PORTALVERTS				128000
#define	MAX_MAP_EDGES					256000
#define	MAX_MAP_SURFEDGES				512000
#define	MAX_MAP_LIGHTING				0x1000000
#define	MAX_MAP_VISIBILITY				0x1000000			// increased BSPVERSION 7
#define	MAX_MAP_TEXTURES				1024
#define MAX_MAP_WORLDLIGHTS				8192
#define MAX_MAP_CUBEMAPSAMPLES			1024
#define MAX_MAP_OVERLAYS				512 
#define MAX_MAP_WATEROVERLAYS			16384
#define MAX_MAP_TEXDATA_STRING_DATA		256000
#define MAX_MAP_TEXDATA_STRING_TABLE	65536
// this is stuff for trilist/tristrips, etc.
#define MAX_MAP_PRIMITIVES				32768
#define MAX_MAP_PRIMVERTS				65536
#define MAX_MAP_PRIMINDICES				65536

typedef struct
{
	int	fileofs;	// offset into file (bytes)
	int	filelen;	// length of lump (bytes)
	int	version;	// lump format version
	char fourCC[4];	// lump ident code
} lump_t;

typedef struct
{
	unsigned int id;
	unsigned int actualSize;		// always little endian
	unsigned int lzmaSize;			// always little endian
	byte properties[5];
} lzma_header_t;

typedef struct
{
	int	ident;                // BSP file identifier
	int	version;              // BSP file version
	lump_t lumps[HEADER_LUMPS];  // lump directory array
	int	mapRevision;          // the map's revision (iteration, version) number
} BSPHeader_t;

typedef enum
{
	VER_VAMPIRE = 17,
	VER_HL2LEAK = 18,
	VER_SDK2006 = 19,
	VER_SDK2007 = 20,
	VER_SDKL4D = 21,
	VER_DOTA2 = 22,
} BSPVersion;

enum
{
	CHILDNODE_UPPER_RIGHT = 0,
	CHILDNODE_UPPER_LEFT = 1,
	CHILDNODE_LOWER_LEFT = 2,
	CHILDNODE_LOWER_RIGHT = 3
};


// Corner indices. Used to index m_CornerNeighbors.
enum
{
	CORNER_LOWER_LEFT = 0,
	CORNER_UPPER_LEFT = 1,
	CORNER_UPPER_RIGHT = 2,
	CORNER_LOWER_RIGHT = 3
};


// These edge indices must match the edge indices of the CCoreDispSurface.
enum
{
	NEIGHBOREDGE_LEFT = 0,
	NEIGHBOREDGE_TOP = 1,
	NEIGHBOREDGE_RIGHT = 2,
	NEIGHBOREDGE_BOTTOM = 3
};


// These denote where one dispinfo fits on another.
// Note: tables are generated based on these indices so make sure to update
//       them if these indices are changed.
typedef enum
{
	CORNER_TO_CORNER = 0,
	CORNER_TO_MIDPOINT = 1,
	MIDPOINT_TO_CORNER = 2
} NeighborSpan;


// These define relative orientations of displacement neighbors.
typedef enum
{
	ORIENTATION_CCW_0 = 0,
	ORIENTATION_CCW_90 = 1,
	ORIENTATION_CCW_180 = 2,
	ORIENTATION_CCW_270 = 3
} NeighborOrientation;


//=============================================================================

typedef enum
{
	LUMP_ENTITIES = 0,	// *
	LUMP_PLANES = 1,	// *
	LUMP_TEXDATA = 2,	// *
	LUMP_VERTEXES = 3,	// *
	LUMP_VISIBILITY = 4,	// *
	LUMP_NODES = 5,	// *
	LUMP_TEXINFO = 6,	// *
	LUMP_FACES = 7,	// *
	LUMP_LIGHTING = 8,	// *
	LUMP_OCCLUSION = 9,
	LUMP_LEAFS = 10,	// *
	LUMP_FACEIDS = 11,
	LUMP_EDGES = 12,	// *
	LUMP_SURFEDGES = 13,	// *
	LUMP_MODELS = 14,	// *
	LUMP_WORLDLIGHTS = 15,	// 
	LUMP_LEAFFACES = 16,	// *
	LUMP_LEAFBRUSHES = 17,	// *
	LUMP_BRUSHES = 18,	// *
	LUMP_BRUSHSIDES = 19,	// *
	LUMP_AREAS = 20,	// *
	LUMP_AREAPORTALS = 21,	// *
	LUMP_UNUSED0 = 22,
	LUMP_UNUSED1 = 23,
	LUMP_UNUSED2 = 24,
	LUMP_UNUSED3 = 25,
	LUMP_DISPINFO = 26,
	LUMP_ORIGINALFACES = 27,
	LUMP_PHYSDISP = 28,
	LUMP_PHYSCOLLIDE = 29,
	LUMP_VERTNORMALS = 30,
	LUMP_VERTNORMALINDICES = 31,
	LUMP_DISP_LIGHTMAP_ALPHAS = 32,
	LUMP_DISP_VERTS = 33,		// CDispVerts
	LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,	// For each displacement
	//     For each lightmap sample
	//         byte for index
	//         if 255, then index = next byte + 255
	//         3 bytes for barycentric coordinates
	// The game lump is a method of adding game-specific lumps
	// FIXME: Eventually, all lumps could use the game lump system
	LUMP_GAME_LUMP = 35,
	LUMP_LEAFWATERDATA = 36,
	LUMP_PRIMITIVES = 37,
	LUMP_PRIMVERTS = 38,
	LUMP_PRIMINDICES = 39,
	// A pak file can be embedded in a .bsp now, and the file system will search the pak
	//  file first for any referenced names, before deferring to the game directory 
	//  file system/pak files and finally the base directory file system/pak files.
	LUMP_PAKFILE = 40,
	LUMP_CLIPPORTALVERTS = 41,
	// A map can have a number of cubemap entities in it which cause cubemap renders
	// to be taken after running vrad.
	LUMP_CUBEMAPS = 42,
	LUMP_TEXDATA_STRING_DATA = 43,
	LUMP_TEXDATA_STRING_TABLE = 44,
	LUMP_OVERLAYS = 45,
	LUMP_LEAFMINDISTTOWATER = 46,
	LUMP_FACE_MACRO_TEXTURE_INFO = 47,
	LUMP_DISP_TRIS = 48,
	LUMP_PHYSCOLLIDESURFACE = 49,	// deprecated.  We no longer use win32-specific havok compression on terrain
	LUMP_WATEROVERLAYS = 50,
	LUMP_LEAF_AMBIENT_INDEX_HDR = 51,	// index of LUMP_LEAF_AMBIENT_LIGHTING_HDR
	LUMP_LEAF_AMBIENT_INDEX = 52,	// index of LUMP_LEAF_AMBIENT_LIGHTING

	// optional lumps for HDR
	LUMP_LIGHTING_HDR = 53,
	LUMP_WORLDLIGHTS_HDR = 54,
	LUMP_LEAF_AMBIENT_LIGHTING_HDR = 55,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.
	LUMP_LEAF_AMBIENT_LIGHTING = 56,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.

	LUMP_XZIPPAKFILE = 57,   // deprecated. xbox 1: xzip version of pak file
	LUMP_FACES_HDR = 58,	// HDR maps may have different face data.
	LUMP_MAP_FLAGS = 59,   // extended level-wide flags. not present in all levels
	LUMP_OVERLAY_FADES = 60,	// Fade distances for overlays
} BSPLump;

// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100

#define CONTENTS_HASOBJECTIVE	0x200
#define CONTENTS_PIRATES		0x400
#define CONTENTS_VIKINGS		0x800	// per team contents used to differentiate collisions 
#define CONTENTS_KNIGHTS		0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace


// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture

#define	SURF_HINT		0x0100	// make a primary bsp splitter

#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox



// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

// little-endian "LZMA"
#define LZMA_ID	(('A'<<24)|('M'<<16)|('Z'<<8)|('L'))

//============
//	LUMPS
//============

typedef struct
{
	char* buffer; // ASCII buffer of ents
} entity_t;

typedef struct
{
	Vector normal;	// normal vector
	float dist;	// distance from origin
	int	type;	// plane axis identifier
} dplane_t;

typedef struct
{
	Vector point;
} dvertex_t;

typedef struct
{
	word v[2];	// vertex indices
} dedge_t;

typedef struct
{
	word planenum;		// the plane number
	byte side;			// faces opposite to the node's plane direction
	byte onNode;			// 1 of on node, 0 if in leaf
	int firstedge;		// index into surfedges
	short numedges;		// number of surfedges
	short texinfo;		// texture info
	short dispinfo;		// displacement info
	short surfaceFogVolumeID;	// ?
	byte styles[4];		// switchable lighting info
	int lightofs;		// offset into lightmap lump
	float area;			// face area in units^2
	int LightmapTextureMinsInLuxels[2];	// texture lighting info
	int LightmapTextureSizeInLuxels[2];	// texture lighting info
	int origFace;		// original face this was split from
	word numPrims;		// primitives
	word firstPrimID;
	unsigned int smoothingGroups;	// lightmap smoothing group
} dface_t;

typedef struct
{
	int	firstside;	// first brushside
	int	numsides;	// number of brushsides
	int	contents;	// contents flags
} dbrush_t;

typedef struct
{
	word planenum;	// facing out of the leaf
	short texinfo;	// texture info
	short dispinfo;	// displacement info
	short bevel;		// is the side a bevel plane?
} dbrushside_t;

typedef struct
{
	int planenum;	// index into plane array
	int children[2];	// negative numbers are -(leafs + 1), not nodes
	short mins[3];	// for frustum culling
	short maxs[3];
	word firstface;	// index into face array
	word numfaces;	// counting both sides
	short area;		// If all leaves below this node are in the same area, then
	// this is the area index. If not, this is -1.
	short paddding;	// pad to 32 bytes length
} dnode_t;

typedef struct
{
	int	contents;		// OR of all brushes (not needed?)
	short cluster;		// cluster this leaf is in
	short area : 9;			// area this leaf is in
	short flags : 7;		// flags
	short mins[3];		// for frustum culling
	short maxs[3];
	word firstleafface;		// index into leaffaces
	word numleaffaces;
	word firstleafbrush;		// index into leafbrushes
	word numleafbrushes;
	short leafWaterDataID;	// -1 for not in water

	//!!! NOTE: for maps of version 19 or lower uncomment this block
	/*
	CompressedLightCube	ambientLighting;	// Precaculated light info for entities.
	short			padding;		// padding to 4-byte boundary
	*/
} dleaf_t;

typedef struct
{
	float textureVecs[2][4];	// [s/t][xyz offset]
	float lightmapVecs[2][4];	// [s/t][xyz offset] - length is in units of texels/area
	int	flags;			// miptex flags	overrides
	int	texdata;		// Pointer to texture name, size, etc.
} texinfo_t;

typedef struct
{
	Vector	reflectivity;		// RGB reflectivity
	int	nameStringTableID;	// index into TexdataStringTable
	int	width, height;		// source image
	int	view_width, view_height;
} dtexdata_t;

typedef struct
{
	Vector mins, maxs;		// bounding box
	Vector origin;			// for sounds or lights
	int	headnode;		// index into node array
	int	firstface, numfaces;	// index into face array
} dmodel_t;

typedef struct
{
	int	numclusters;
	int	byteofs[8][2];
} dvis_t;

typedef struct
{
	int	id;		// gamelump ID
	word flags;		// flags
	word version;	// gamelump version
	int fileofs;	// offset to this gamelump
	int filelen;	// length
} dgamelump_t;

typedef struct
{
	int lumpCount;	// number of game lumps
} dgamelumpheader_t;
/*
struct StaticPropDictLump_t
{
	int	dictEntries;
	char name[dictEntries];	// model name
};

struct StaticPropLeafLump_t
{
	int leafEntries;
	word leaf[leafEntries];
};
*/
typedef struct
{
	// v4
	Vector Origin;		 // origin
	QAngle Angles;		 // orientation (pitch roll yaw)
	word PropType;	 // index into model name dictionary
	word FirstLeaf;	 // index into leaf array
	word LeafCount;
	byte Solid;		 // solidity type
	byte Flags;
	int Skin;		 // model skin numbers
	float FadeMinDist;
	float FadeMaxDist;
	Vector LightingOrigin;  // for lighting
	// since v5
	float ForcedFadeScale; // fade distance scale
	// v6 and v7 only
	word MinDXLevel;      // minimum DirectX version to be visible
	word MaxDXLevel;      // maximum DirectX version to be visible
	// since v8
	byte MinCPULevel;
	byte MaxCPULevel;
	byte MinGPULevel;
	byte MaxGPULevel;
	// since v7
	color32 DiffuseModulation; // per instance color and alpha modulation
	// since v10
	float unknown;
	// since v9
	BOOL DisableX360;     // if TRUE, don't show on XBox 360
} StaticPropLump_t;

typedef struct
{
	word		m_iNeighbor;		// This indexes into ddispinfos.
	// 0xFFFF if there is no neighbor here.

	byte		m_NeighborOrientation;		// (CCW) rotation of the neighbor wrt this displacement.

	// These use the NeighborSpan type.
	byte		m_Span;						// Where the neighbor fits onto this side of our displacement.
	byte		m_NeighborSpan;				// Where we fit onto our neighbor.
} DispSubNeighbor_t;


typedef struct
{
	// Note: if there is a neighbor that fills the whole side (CORNER_TO_CORNER),
	//       then it will always be in CDispNeighbor::m_Neighbors[0]
	DispSubNeighbor_t	m_SubNeighbors[2];
} DispNeighbor_t;


typedef struct
{
	word m_Neighbors[4];	// indices of neighbors.
	byte m_nNeighbors;
} DispCornerNeighbors_t;


typedef struct
{
	Vector		m_vVector;		// Vector field defining displacement volume.
	float		m_flDist;		// Displacement distances.
	float		m_flAlpha;		// "per vertex" alpha values.
} DispVert_t;

typedef struct
{
	Vector			startPosition;		// start position used for orientation
	int			DispVertStart;		// Index into LUMP_DISP_VERTS.
	int			DispTriStart;		// Index into LUMP_DISP_TRIS.
	int			power;			// power - indicates size of surface (2^power	1)
	int			minTess;		// minimum tesselation allowed
	float			smoothingAngle;		// lighting smoothing angle
	int			contents;		// surface contents
	word		MapFace;		// Which map face this displacement comes from.
	int			LightmapAlphaStart;	// Index into ddisplightmapalpha.
	int			LightmapSamplePositionStart;	// Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.
	DispNeighbor_t		EdgeNeighbors[4];	// Indexed by NEIGHBOREDGE_ defines.
	DispCornerNeighbors_t	CornerNeighbors[4];	// Indexed by CORNER_ defines.
	unsigned int		AllowedVerts[10];	// active verticies
} ddispinfo_t;

#define DISPTRI_TAG_SURFACE			(1<<0)
#define DISPTRI_TAG_WALKABLE		(1<<1)
#define DISPTRI_TAG_BUILDABLE		(1<<2)
#define DISPTRI_FLAG_SURFPROP1		(1<<3)
#define DISPTRI_FLAG_SURFPROP2		(1<<4)
#define DISPTRI_TAG_REMOVE			(1<<5)

typedef struct
{
	Vector vec;	// Vector field defining displacement volume.
	float dist;	// Displacement distances.
	float alpha;	// "per vertex" alpha values.
} dDispVert;

typedef struct
{
	word Tags;	// Displacement triangle tags.
} dDispTri;

typedef struct
{
	int		origin[3];	// position of light snapped to the nearest integer
	int	        size;		// resolution of cubemap, 0 - default
} dcubemapsample_t;

typedef struct
{
	int		Id;
	short		TexInfo;
	word	FaceCountAndRenderOrder;
	int		Ofaces[64];
	float		U[2];
	float		V[2];
	Vector		UVPoints[4];
	Vector		Origin;
	Vector		BasisNormal;
} doverlay_t;

typedef struct
{
	byte r, g, b;
	byte exponent;
} ColorRGBExp32;
/*
struct doccluder_t
{
	int			count;
	doccluderdata_t		data[count];
	int			polyDataCount;
	doccluderpolydata_t	polyData[polyDataCount];
	int			vertexIndexCount;
	int			vertexIndices[vertexIndexCount];
};

struct doccluderdata_t
{
	int	flags;
	int	firstpoly;	// index into doccluderpolys
	int	polycount;	// amount of polygons
	Vector	mins;	        // minima of all vertices
	Vector	maxs;	        // maxima of all vertices
	// since v1
	int	area;
};

struct doccluderpolydata_t
{
	int	firstvertexindex;	// index into doccludervertindices
	int	vertexcount;		// amount of vertex indices
	int	planenum;
};
*/
#endif
