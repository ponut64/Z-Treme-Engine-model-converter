#ifndef COMMON_H
#define COMMON_H


using namespace std;
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "ZTE/saturnDef.h"

#define     LUTaddr             (0x25C7A960)
#define     returnLUTaddr(n)    (LUTaddr + (0x20 * n))
#define     LUTidx(n)           (uint16_t)(62764 + (4 * n))
#define     CRAM_Base           (0x25f00200)
#define     returnCRAMaddr(n)   (CRAM_Base + (0x20 * n))
#define     LUTcramIdx(n)       ((256 + (16 * n)))

#define     toFIXED16(n)          (int32_t)(n*65536.0)
#define     toFIXED8(n)         (int16_t)(n*256.0)
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define MIN(x, y)	((x) < (y) ? (x) : (y))

#define CAN_JUMP (1)
#define MAX_SPRITES             (2000)

const float conv              = M_PI/180.0f;

enum
{
 X=0,
 Y=1,
 Z=2,
 XY=2,
 XYZ=3,
 XYZS=4
};

typedef     float   MATRIX[16];
typedef     float	VECTOR[XYZ] ;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
    uint16_t rgb; //Saturn 16 bits format
    uint16_t palette_idx; //Palette/Color bank index
} pixel_t;

typedef struct
{
    float point[3];
    float normal[3];
} vertex_t;

typedef struct
{
    unsigned int vertIdx[4];
    int texture;
	int luma;
    float normal[3];
} polygon_t;

typedef struct
{
    unsigned int nbPoint;
    unsigned int nbPolygon;
    vertex_t * pntbl;
    polygon_t * pltbl;
} model_t;

typedef struct
{
    vertex_t * cVert;  //Uncompressed for now // ???
    float * cNorm[3]; //Will probably stay unused // ???
}key_frame_t;


/*
Render data flags:
	
	Byte 1 of render_data_flags:
	|	0		|	1		|	2		|	3		|	4	-	5 	|		6	-	7 	 	|		
	  Dual-plane	Mesh	 Polyline		MSB On		Tex. flip	  		Sorting rule
	Byte 2 of render_data_flags:
	|	8		|	9		|	10		|	11		|	12	 -	13	|	14		|	15		|		
	Subdivision	  Collision		Ladder	Climbable		Portal Specs	Anim		On/off
*/
	#define GV_FLAG_SINGLE		(0x1) // Zero, dual-plane. One, single-plane.
	#define GV_FLAG_MESH		(0x2) // Zero, no mesh. One, mesh.
	#define GV_FLAG_POLYLINE	(0x4) // Zero, normal polygon. One, draws with polyline. Could be somewhere else.
	#define GV_FLAG_DARK		(0x8) // Zero, normal light. One, MSB is enabled, making the polygon dark.
	#define GV_FLAG_NDIV		(0x100) // Zero, polygon can subdivide (in supported objects). One, no subdivision.
	#define GV_FLAG_PHYS		(0x200) // One, physical plane (in supported objects). Zero, no collision with plane.
	#define GV_FLAG_LADDER		(0x400) // Boolean. 1 = ladder. 0 = no ladder. Notice: all ladders are climbable.
	#define GV_FLAG_CLIMBABLE	(0x800) //Boolean. 1 = Climbable. 0 = not climbable.
	#define GV_FLAG_PORTAL		(0x1000) //Boolean. 0, not a portal. 1, is a portal.
	#define GV_FLAG_PORT_IN		(0x2000) //Boolean. 0, portal OUT processing. 1, portal IN processing.
	#define GV_FLAG_ANIM		(0x4000) //Boolean. 0, texture will not animate. 1, texture is potentially animated.
	#define GV_FLAG_DISPLAY		(0x8000) //Boolean. 1, polygon is treated as polygon. 0, polygon is not rendered. 
	#define GV_SORT_MAX			(0x40)
	#define GV_SORT_CEN			(0x80)
	#define GV_SORT_MIN			(0xC0)
	#define GV_FLIP_V			(0x20)
	#define GV_FLIP_H			(0x10)
	#define GV_FLIP_HV			(0x30)
	#define GET_SORT_DATA(n)	(n & 0xC0)
	#define GET_FLIP_DATA(n)	(n & 0x30)
	#define GV_SCTR_MOVER		(0xF0) //Special flag only scanned for on sector data. Marks the surface as belonging to a MOVER.
	
	//Speicl Flags. These flags do NOT imply the combination of the individual flags.
	//Instead, something special happens when all of these conditions are met.
	//Case 1: Will remove polygons of the heightmap below these polygons, if they are facing Y+, on map load.
	#define GV_SPECIAL_FLAG_UNRENDER_MAP	(GV_FLAG_DARK | GV_FLAG_MESH | GV_FLAG_POLYLINE | GV_FLAG_NDIV)

/*
Portal information:
Polygon ID of the next portal. 
If it is 255, this polygon is not a portal.
If it is 254, this is the last portal.

Plane information:
Information about the scale and subdivision rules of the plane.
0-1: First subdivision rule
2-3: Second subdivision rule
4-5: Third subdivision rule
6-7: ???
*/

	#define SUBDIVIDE_X		(1) // |
	#define SUBDIVIDE_Y		(2) // -
	#define SUBDIVIDE_XY	(3) // +
	
	#define SUBDIVIDE_3XY	(0x3F) // +++
	#define SUBDIVIDE_1Y2XY	(0x3E) // -++
	#define SUBDIVIDE_1X2XY	(0x3D) // |++
	#define SUBDIVIDE_2Y1XY	(0x3A) // --+
	#define SUBDIVIDE_2X1XY	(0x35) // ||+
	#define SUBDIVIDE_3Y	(0x2A) // ---
	#define SUBDIVIDE_3X	(0x15) // |||
	
	#define SUBDIVIDE_2XY	(0xF) // ++
	#define SUBDIVIDE_1Y1XY	(0xE) // -+
	#define SUBDIVIDE_1X1XY	(0xD) // |+
	#define SUBDIVIDE_2Y	(0xA) // --
	#define SUBDIVIDE_2X	(0x5) // ||

	#define SUBDIVISION_SCALE (64)
typedef struct {
	uint16_t render_data_flags;
	uint8_t tile_information;
	uint8_t plane_information;
	uint8_t uv_id;
	uint8_t first_sector;
	uint16_t texno;
} gvAtr;

//Struct which stores the vertex indexes of a polygon
//The entries here are for the pntbl array.
typedef struct {
	uint16_t vertices[4];
} _quad;

typedef struct {
    uint32_t nbPoint;		/* Number of vertices */
    uint32_t nbPolygon;		/* Number of polygons */
    POINT *		pntbl;		/* Vertex position data table */
    _quad *		pltbl;		/* Polygon definition table */
	POINT *		nmtbl;		/* Normal definition table */
	uint8_t * maxtbl;		/* Major axis table */
    gvAtr *		attbl;		/* The attribute table for the polygon */
	uint8_t * lumatbl;		/* Lighting table, as <<9 units. */
} GVPLY ;

//////////////////////////////////
//	Numerical Normal ID Shorthands
//////////////////////////////////
#define N_Xp (0)
#define N_Xn (1)
#define N_Yp (2)
#define N_Yn (3)
#define N_Zp (4)
#define N_Zn (5)

typedef struct
{
    unsigned int    width;
    unsigned int    height;
    unsigned int    colorDepth;
    string          name;
    unsigned int    textureId;
    int             paletteIdx;
    pixel_t *       pixel;
    uint8_t         nbFrames;
	gvAtr			GV_ATTR; //For my stupid single-use format
} texture_t;


typedef struct
{
    unsigned int    nbFrames;
    unsigned int    nbTextures;
    unsigned int    nbModels;
    string          name;
    key_frame_t *   keyFrames;
    model_t *       model;
    texture_t *     texture;
	unsigned short x_radius;
	unsigned short y_radius;
	unsigned short z_radius;
} animated_model_t;

typedef struct
{
    uint16_t animID;
    uint16_t currentFrm;
    uint16_t currentKeyFrm;
    uint16_t fps;
    uint16_t startFrm;
    uint16_t endFrm;
} animationControl_t;

extern int		item_exception_vertices[64][4];
extern int		items[64];
extern int		item_number_package[64];
extern float	item_positions[64][XYZ];
extern int		number_of_items;
extern int		number_of_unique_items;

uint16_t        swap_endian_ushort(uint16_t value);
int16_t         swap_endian_sshort( int16_t value);
uint32_t        swap_endian_uint(uint32_t value);
int32_t         swap_endian_sint( int32_t value);

/**Functions**/
float	v3Dist(float * a, float * b);
float dotProduct(float * vct1, float * vct2);
animated_model_t * loading(void);

#endif // COMMON_H
