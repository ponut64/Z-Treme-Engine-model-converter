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
    float color[3];
    float normal[3];
    unsigned int normalArray;
} vertex_t;

typedef struct
{
    unsigned int vertIdx[4];
    int texture;
    bool dualPlane;
    int sorting;
    pixel_t color; //If it's a single color
    unsigned int vdp2_effect; //Bump mapping?
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

typedef struct {
    uint8_t	     flag ;
    uint8_t	     sorting ;
    uint16_t	 texno ;
    uint16_t	 atrb ;
    uint16_t	 colno ;
    uint16_t	 gstb ;
    uint16_t	 dir ;
} ATTR ;

//What will *I* need, in my attribute data?
// 1 -> Dual or single plane flag
// 1a -> Mesh or un-mesh flag
// 2 -> Collectible or un-collidable flag (sometimes used)
// 3 -> Two flags for sorting rules, I guess? Engine doesn't use these...
// 4 -> Texture number, as an unsigned 16-bit value
// 5 -> Sector numbers, as two unsigned 8-bit values.
// 6 -> Portal or not a portal flag
/*
Render data flags:
	
	Byte 1 of render_data_flags:
	|	0		|	1		|	2		|	3		|	4	-	5 	|	6	-	7 |		
	  Dual-plane	Mesh		-			MSB On		Tex. flip	  Sorting rule
	Byte 2 of render_data_flags:
	|	8		|	9		|	10		|	11		|	12	-	13 	|	14	-	15 |		
	Subdivision	  Collision					 
*/
	#define GV_FLAG_SINGLE	(0x1) // Zero, dual-plane. One, single-plane.
	#define GV_FLAG_MESH	(0x2) // Zero, no mesh. One, mesh.
	#define GV_FLAG_DARK	(0x8) // Zero, normal light. One, MSB is enabled, making the polygon dark.
	#define GV_FLAG_NDIV	(0x100) // Zero, polygon can subdivide (in supported objects). One, no subdivision.
	#define GV_FLAG_PHYS	(0x200) // Zero, physical plane (in supported objects). One, no collision with plane.
	#define GV_SORT_MAX		(0x40)
	#define GV_SORT_CEN		(0x80)
	#define GV_SORT_MIN		(0xC0)
	#define GV_FLIP_V		(0x20)
	#define GV_FLIP_H		(0x10)
	#define GV_FLIP_HV		(0x30)
	#define GET_SORT_DATA(n)	(n & 0xC0)
	#define GET_FLIP_DATA(n)	(n & 0x30)
	
/*
Portal information:
Polygon ID of the next portal. 
If it is 255, this polygon is not a portal.
If it is 254, this is the last portal.
*/

typedef struct {
	uint16_t render_data_flags;
	uint16_t portal_information;
	uint8_t first_sector_number;
	uint8_t second_sector_number;
	uint16_t texno;
} gvAtr;

typedef struct
{
    unsigned int    width;
    unsigned int    height;
    unsigned int    colorDepth;
    string          name;
    unsigned int    textureId;
    int             paletteIdx;
    pixel_t *       pixel;
    uint16_t        clut[16];
    uint8_t         nbFrames;
    char			attribute;
    ATTR            SGL_ATTR; //For SGL mainly, can be converted for ZTE format
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
float dotProduct(float * vct1, float * vct2);
animated_model_t * loading(void);

#endif // COMMON_H
