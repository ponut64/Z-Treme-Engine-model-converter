#include <cstdint>


#define	    toFIXED(a)		((int32_t)(65536.0 * (a)))
#define	    POStoFIXED(x,y,z)	{toFIXED(x),toFIXED(y),toFIXED(z)}
#define	    ATTRIBUTE(f,s,t,c,g,a,d,o)	{f,(s)|(((d)>>16)&0x1c)|(o),t,(a)|(((d)>>24)&0xc0),c,g,(d)&0x3f}
#define	    SPR_ATTRIBUTE(t,c,g,a,d)	{t,(a)|(((d)>>24)&0xc0),c,g,(d)&0x0f3f}
#define	    DEGtoANG(d)		((ANGLE)((65536.0 * (d)) / 360.0))
#define     RADtoANG(d)         ((ANGLE)((65536.0 * (d)) / (2*M_PI)))
#define	    C_RGB(r,g,b)		(0x8000|((b)<<10)|((g)<<5)|(r))
#define	    DGTtoRGB(c)		(0x8000|(((c)&0x0f00)<<3)|(((c)&0x4000)>>4)|(((c)&0x00f0)<<2)|(((c)&0x2000)>>8)|(((c)&0x000f)<<1)|(((c)&0x1000)>>12))
#define	    SLocate(x,y,p)	((p) * Page + (y) * Line + (x))

/*  Sprite control function */

#define	    FUNC_Sprite		1
#define	    FUNC_Texture	2
#define	    FUNC_Polygon	4
#define	    FUNC_PolyLine	5
#define	    FUNC_Line		6
#define	    FUNC_SystemClip	9
#define	    FUNC_UserClip	8
#define	    FUNC_BasePosition	10
#define	    FUNC_End		-1

#define	    _ZmLT		(0x05 << 8)	/* Zoom base Left Top */
#define	    _ZmLC		(0x09 << 8)	/* Zoom base Left Center */
#define	    _ZmLB		(0x0d << 8)	/* Zoom base Left Bottom */
#define	    _ZmCT		(0x06 << 8)	/* Zoom base Center Top */
#define	    _ZmCC		(0x0a << 8)	/* Zoom base Center Center */
#define	    _ZmCB		(0x0e << 8)	/* Zoom base Center Bottom */
#define	    _ZmRT		(0x07 << 8)	/* Zoom base Right Top */
#define	    _ZmRC		(0x0b << 8)	/* Zoom base Right Center */
#define	    _ZmRB		(0x0f << 8)	/* Zoom base Right Bottom */

#define	    MSBon		(1 << 15)	/* �t��?[���o�b�t�@��?���?��ނl�r�a���P�ɂ��� */
#define	    HSSon		(1 << 12)	/* �n�C�X�s?[�h�V�������N�L�� */
#define	    HSSoff		(0 << 12)	/* �n�C�X�s?[�h�V�������N����(default) */
#define	    No_Window		(0 << 9)	/* �E�B���h�E��?������󂯂Ȃ�(default)*/
#define	    Window_In		(2 << 9)	/* �E�B���h�E�̓����ɕ\�� */
#define	    Window_Out		(3 << 9)	/* �E�B���h�E�̊O���ɕ\�� */
#define	    MESHoff		(0 << 8)	/* ��?�\��(default) */
#define	    MESHon		(1 << 8)	/* �?�b�V���ŕ\�� */
#define	    ECdis		(1 << 7)	/* �G���h�R?[�h���p���b�g�̂ЂƂƂ��Ďg�p */
#define	    ECenb		(0 << 7)	/* �G���h�R?[�h�L�� */
#define	    SPdis		(1 << 6)	/* �X�y?[�X�R?[�h���p���b�g�̂ЂƂƂ��Ďg�p */
#define	    SPenb		(0 << 6)	/* �X�y?[�X�͕\�����Ȃ�(default) */
#define	    CL16Bnk		(0 << 3)	/* �J��?[�o���N�P�U?F��?[�h (default) */
#define	    CL16Look		(1 << 3)	/* �J��?[���b�N�A�b�v�P�U?F��?[�h */
#define	    CL64Bnk		(2 << 3)	/* �J��?[�o���N�U�S?F��?[�h */
#define	    CL128Bnk		(3 << 3)	/* �J��?[�o���N�P�Q�W?F��?[�h */
#define	    CL256Bnk		(4 << 3)	/* �J��?[�o���N�Q�T�U?F��?[�h */
#define	    CL32KRGB		(5 << 3)	/* �q�f�a�R�Q�j?F��?[�h */
#define	    CL_Replace		0		/* ?d��?���(?�?���)��?[�h */
#define	    CL_Shadow		1		/* �e��?[�h */
#define	    CL_Half		2		/* ���P�x��?[�h */
#define	    CL_Trans		3		/* ��������?[�h */
#define	    CL_Gouraud		4		/* �O?[�??[�V�F?[�f�B���O��?[�h */

#define	    UseTexture		(1 << 2)	/* �e�N�X�`����\��|���S�� */
#define	    UseLight		(1 << 3)	/* �����̉e�����󂯂�|���S�� */
#define	    UsePalette		(1 << 5)	/* �|���S���̃J��?[ */

/* SGL Ver3.0 Add */
#define	    UseNearClip		(1 << 6)	/* �j�A?E�N���b�s���O������ */
#define	    UseGouraud		(1 << 7)	/* ���A���O?[�??[ */
#define	    UseDepth		(1 << 4)	/* �f�v�X�L��?[ */
/* */

#if 0 /* SGL Ver3.0 Delete */
#define	    UseClip		(1 << 4)		/* ��ʊO�ɒ��_��?o����\�����Ȃ� */
#else
#define	    UseClip		UseNearClip		/* �j�A?E�N���b�s���O������ */
#endif

#define	    sprHflip		((1 << 4) | FUNC_Texture | (UseTexture << 16))
#define	    sprVflip		((1 << 5) | FUNC_Texture | (UseTexture << 16))
#define	    sprHVflip		((3 << 4) | FUNC_Texture | (UseTexture << 16))
#define	    sprNoflip		((0) | FUNC_Texture | (UseTexture << 16))
#define	    sprPolygon		(FUNC_Polygon | ((ECdis | SPdis) << 24))
#define	    sprPolyLine		(FUNC_PolyLine | ((ECdis | SPdis) << 24))
#define	    sprLine		(FUNC_Line | ((ECdis | SPdis) << 24))

#define	    No_Texture		0		/* �e�N�X�`�����g�p���Ȃ��� */
#define	    No_Option		0		/* �I�v�V�������g�p���Ȃ��� */
#define	    No_Gouraud		0		/* �O?[�??[�V�F?[�f�B���O���g�p���Ȃ��� */
#define	    No_Palet	        0		/* �J��?[�p���b�g�̎w�肪����Ȃ��� */
#define	    GouraudRAM		(0x00080000 - (32*8))	/* �����̉e���p�̃O?[�??[�o�b�t�@ */

#define	    COL_16	(2+1)		/* �J��?[�o���N�P�U?F��?[�h */
#define	    COL_64	(2+0)		/* �J��?[�o���N�U�S?F��?[�h */
#define	    COL_128	(2+0)		/* �J��?[�o���N�P�Q�W?F��?[�h */
#define	    COL_256	(2+0)		/* �J��?[�o���N�Q�T�U?F��?[�h */
#define	    COL_32K	(2-1)		/* �q�f�a�R�Q�j?F��?[�h */

    enum base {
	SORT_BFR ,			/* ���O�ɕ\�������|���S���̈ʒu���g�� */
	SORT_MIN ,			/* �S�_�̓�?A��Ԏ�O�̓_���g�� */
	SORT_MAX ,			/* �S�_�̓���ԉ����_���g�� */
	SORT_CEN			/* �S�_�̕��ψʒu���g�� */
    } ;

    enum pln {
	Single_Plane ,			/* �Жʃ|���S�� */
	Dual_Plane			/* ���ʃ|���S��(�\������̌��ʂ����Ȃ�) */
    } ;

typedef struct {
    uint16_t	CTRL ;			/* �R���g�??[���t�@���N�V���� */
    uint16_t	LINK ;			/* �����N�A�h���X */
    uint16_t	PMOD ;			/* �v�b�g��?[�h */
    uint16_t	COLR ;			/* �J��?[�f?[�^ */
    uint16_t	SRCA ;			/* �b�f�A�h���X */
    uint16_t	SIZE ;			/* �L�����N�^�T�C�Y */
    int16_t	    XA ;			/* �\���ʒu �` */
    int16_t	    YA ;
    int16_t	    XB ;			/* �\���ʒu �a */
    int16_t	    YB ;
    int16_t	    XC ;			/* �\���ʒu �b */
    int16_t	    YC ;
    int16_t	    XD ;			/* �\���ʒu �c */
    int16_t	    YD ;
    uint16_t	GRDA ;			/* �O?[�??[�V�F?[�f�B���O�e?[�u�� */
    int16_t	    Z ;			/* Z distance for the z sort buffer */
    uint32_t *  NEXT ;
} SPRITE_T ;

extern const void* Zbuffer;
extern const SPRITE_T* SpriteBuf;



typedef int16_t ZPOINT[4]; //vertex data = 3 Points, 1 color data

typedef struct {
    uint8_t   vertices[4];
    uint16_t  texno;
    uint16_t  cmdctrl;
    uint16_t  cmdpmod;
    uint16_t  cmdcolr;
} ZPOLYGON; //12 bytes each


typedef struct {
    uint16_t StartPoint;
    uint16_t EndPoint;
    uint16_t StartPol;
    uint16_t EndPol;
    uint16_t LightID; //A copy to transfer to the slave
    int16_t  LightDistance;
} ZPDATA;


#define	    SpriteVRAM		0x25c00000
