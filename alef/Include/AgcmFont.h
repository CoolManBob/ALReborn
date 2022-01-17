#ifndef __AGCMFONT_H__
#define __AGCMFONT_H__

#include "AgcModule.h"
#include "rwcore.h"
#include "rpworld.h"
#include "AcDefine.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define		AGCM_FONT_MAX_TYPE_NUM			10		// Buffer는 충분히 잡자 (어차피 메모리 얼마 안먹는다.)
#define		AGCM_IMGFONT_MAX_TYPE_NUM		1
#define		AGCM_FONT_HASH_SIZE				19

#define		AGCM_FONT_NAME_LENGTH			64

#define		AGCM_FONT_WHITE_COLOR					0xffffffff
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR		0x50808080
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR2		0x50505050
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR3		0x28808080
#define		AGCM_FONT_TRANSPARENT_WHITE_COLOR4		0x28505050
#define		AGCM_FONT_WHITE_CLEAR_COLOR				0x00ffffff

#define		AGCM_FONT_SHADOW_ALPHA					184
#define		AGCM_FONT_SHAODW_COLOR					0xB8000000

#define		AGCM_FONT_GASIB_SIZE_15					4

enum AGCM_FONT_FLAG
{
	FF_LEFT = 1,
	FF_CENTER = 2,
	FF_RIGHT = 4,
	FF_VCENTER = 128					// vertical center align
};

typedef struct tag_TextRaster
{
	UINT16			ccode;					// 문자 코드
	UINT16			ftype;					// 문자 타입
	DWORD			color;					// 문자 색상

	RwRaster*		Ras;
	INT32			count;					// alive count (50 에서 1씩 감소,0되면 삭제)

	FLOAT			offset;					// freetype은 항상 천장에 붙어 ㅉ기힌다
	
	tag_TextRaster*	next;
}TextRaster;

typedef struct tag_TextRasterHash
{
	TextRaster*		hash[AGCM_FONT_HASH_SIZE];
}TextRasterHash;

typedef struct tag_FontType
{
	INT32	lID;
	CHAR	szType[AGCM_FONT_NAME_LENGTH];
	CHAR	szFileName[AGCM_FONT_NAME_LENGTH];
	INT32	lSize;
}FontType;

typedef struct tag_AgcdFontClippingBox
{
	FLOAT	x,y,w,h;
}AgcdFontClippingBox;

//한block에 5000개의 영문자(6*1000):: 동일 cameraz용(index버퍼 활용) 
#define			AGCM_FONT_ASCII_VERTS1_VERT			4000
#define			AGCM_FONT_ASCII_VERTS1_INDEX		6000
//한block에 200개의 영문자(6*500):: worldZ 용
#define			AGCM_FONT_ASCII_VERTS2		3000

// DX로 IM2D 대체하여 그림(2004.4.14)
class AgcmFont : public AgcModule
{
public:
	AgcmFont();
	~AgcmFont();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);						

	void	OnCameraStateChange(CAMERASTATECHANGETYPE	ctype);

	// Immediate Mode(2D in World)
	void	DrawTextIMMode2DWorld(float x,float y,float screen_z,float recip_z,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true,bool bDrawBack = false,DWORD backcolor = 0x7fff0000);
	void	DrawTextIMMode2DWorldScale(float x,float y,float size,float screen_z,float recip_z,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true);

	// ClippingSet 함수(NULL이면 clipping헤제)
	void	SetClippingArea(AgcdFontClippingBox*	area);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Immediate Mode
	// Clipping 지원 함수들
	void	DrawTextIM2D	 (float x,float y,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true,bool bBold = false,bool bDrawBack = false,DWORD backcolor = 0x7fff0000, INT32 length = -1);
	void	DrawTextIM2DEx	 (float x,float y,char* str, UINT sizeofchar, UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true,bool bBold = false,bool bDrawBack = false,DWORD backcolor = 0x7fff0000);
	RwRect	DrawTextRectIM2D (RwRect *pRect  ,char* str,UINT32 fonttype,UINT32 Flag = FF_LEFT,UINT8 alpha = 255,DWORD color = 0xffffffff,bool	bDrawShadow = true, INT32 length = -1);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void	DrawCaret		 (float x,float y,UINT32 fonttype, DWORD color = 0xffffffff, int width = 1);	// white & 한글
	void	DrawCaretE		 (float x,float y,UINT32 fonttype, DWORD color = 0xffffffff);					// white & English
	void	DrawTextIM2DScale(float x,float y,float size,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff,bool bDrawShadow = true, INT32 length = -1);

	// MapTool용 
	void	DrawTextMapTool(float x,float y,char* str,UINT32 fonttype,UINT8 alpha = 255,DWORD color = 0xffffffff, bool bDrawShadow = true);
	
	// ImageFont용
	void	DrawTextImgFont  (float x,float y,char* str,UINT32 imgtype,UINT8 Red = 255,UINT8 Green = 255,UINT8 Blue = 255,UINT8 Alpha = 255);

	// string의 pixel길이를 리턴한다
	INT32	GetTextExtent(UINT32 ftype, char* str, INT32 strlen) { return (this->*GetTextExtentPtr)(ftype, str, strlen); }
	int		GetCharacterWidth( UINT32 ftype, char* str ) { return (this->*GetCharacterWidthPtr)(ftype, str); }
	bool	IsLeadByte( unsigned char ch ) { return (this->*IsLeadBytePtr)(ch); }

	void	SetWorldCamera(RwCamera* pCam);

	// Parn님이 추가하신 Method들
	FontType*			GetSequenceFont(INT32 *plIndex);
	INT32				GetSequenceImageFont(INT32 *plIndex);

	// Block내 쓰이는 fonttype ID (다른 fonttype을 지정하려면 end호출후 다른 type으로 재설정)
	// FontDrawStart시에 clipping영역을 지정할수 있도록 하였다. 지정하면 폴리곤 생성시 clipping 처리..
	void				FontDrawStart(int FontType);
	void				FontDrawEnd();
	
private:	
	// 내부용
	// 기존에 text가 있으면 재활용 없으면 생성 ^^v
	void	PrepareIMMode2D(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow);
	
	void	RenderAsciiFont(float x,float y,float w,float h,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);
	void	RenderAsciiFont_CLIP(float x,float y,float w,float h,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);

	void	RenderFont(float x,float y,float size,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);
	void	RenderFont_CLIP(float x,float y,float size,UINT16	ccode,UINT32	fonttype,UINT8	alpha,bool	bShadow);

	void	RenderIMMode2DSCALE(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow,float size=1.0f);
	void	RenderIMMode2DWORLD(float x,float y,float screen_z,float recip_z,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow,float size=1.0f);

	void	RenderMapTool(float x,float y,char* str,INT32 len,UINT32 fonttype,UINT8 alpha,bool bShadow);
	
	INT32	GetHashKey(WORD	ccode);

	RwTexture*			CreateShareTexture(int w,int h,int type);			// 통맵 texture생성
	int					PaintAlphabet( DWORD* pBitMapBit, int TexW, int TexH, int ftype,BOOL bMeasureOnly );

	RwRaster*			MakeFontRasterFromBitMapBits( DWORD* pBitMapBit, int TexW, int TexH );

	RwRaster*			MakeFontRasterFromFTBits(FT_GlyphSlot  slot, int width, int height );

	void				CameraStatusChange();								// Camera설정 변경 vertexdata 다시 set
	RwRaster*			CreateFontRaster(int fonttype,char*	str,int len,FLOAT&	offset);	//	글자가 없을경우 새로 raster를 만든다.

	// 언어별 추가 함수
	BOOL	InitFuncPtr();
	
	INT32	(AgcmFont::*GetTextExtentPtr)(UINT32, char*, INT32);
	INT32	GetTextExtentKr( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentEn( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentJp( UINT32 ftype, char* str, INT32 strlen );
	INT32	GetTextExtentCn( UINT32 ftype, char* str, INT32 strlen );

	int (AgcmFont::*GetCharacterWidthPtr)(UINT32, char*);
	int GetCharacterWidthKr( UINT32 ftype, char* str );
	int GetCharacterWidthEn( UINT32 ftype, char* str );
	int GetCharacterWidthJp( UINT32 ftype, char* str );
	int GetCharacterWidthCn( UINT32 ftype, char* str );

	bool (AgcmFont::*IsLeadBytePtr)( unsigned char );
	bool IsLeadByteKr( unsigned char ch );
	bool IsLeadByteEn( unsigned char ch );
	bool IsLeadByteJp( unsigned char ch );
	bool IsLeadByteCn( unsigned char ch );

public:
	FontType			m_astFonts[AGCM_FONT_MAX_TYPE_NUM];					// Parn님이 추가하신 FontType Array
	UINT8				m_iXLH[AGCM_FONT_MAX_TYPE_NUM];						// x width(한글)
	UINT8				m_iXLE[AGCM_FONT_MAX_TYPE_NUM][256];				// x width(Endglish)
	UINT8				m_iYL[AGCM_FONT_MAX_TYPE_NUM];						// 세로 길이
	
	vector<UINT8>		m_widthJpn[AGCM_FONT_MAX_TYPE_NUM];
	
	bool				m_bUpdateThisFrame;									// Tuner에서 설정

private:
// data
	RwCamera*			m_pCamera;
	RwReal				m_fScreenZ;											//Im2D용 
	RwReal				m_fRecipZ;											//Im2D용
	
	INT32				m_iTexSize[AGCM_FONT_MAX_TYPE_NUM];					// Texture Size
	INT32				m_iTexXLE[AGCM_FONT_MAX_TYPE_NUM][256];				// x width(English) - 2의 배수크기
	INT32				m_iTexYL[AGCM_FONT_MAX_TYPE_NUM];					// 세로 길이 - 2의 배수크기

	TextRasterHash		m_listTextRaster;
	//HFONT				m_font[AGCM_FONT_MAX_TYPE_NUM];
	RwTexture*			m_pColonTexture;								// ■ 

	RwIm2DVertex		m_vTriFan2World[4];								// UV비변화(WORLD)
	My2DVertex			m_vTriFan3[4];									// UV비변화
	
	INT32				m_iCameraRasterWidth;
	INT32				m_iCameraRasterHeight;
	
	//통맵용
	FLOAT				m_fTexCoords[AGCM_FONT_MAX_TYPE_NUM][95][4];
	bool				m_bUseShare[AGCM_FONT_MAX_TYPE_NUM];			// 통맵 사용 여부 (256 size초과시 통맵 사용x)
	RwTexture*			m_pEnglishTextTexture[AGCM_FONT_MAX_TYPE_NUM];	// 영문용 통맵(32~126까지 갖고있음)
	FLOAT				m_iShareSize[AGCM_FONT_MAX_TYPE_NUM];

	RwRGBA				m_stColor;										// String단위로 색 set(연산 감소를 위해)
	DWORD				m_iColor2;
		
	//	Ascii용 POOL(FontDrawStart 와 FontDrawEnd 사이에 있는 글들을 모아 그린다.)
	INT16				m_pIndexAscii1[AGCM_FONT_ASCII_VERTS1_INDEX];
	My2DVertex			m_vBufferAscii1[AGCM_FONT_ASCII_VERTS1_VERT];	

	RwIm2DVertex		m_vTriListAscii2[AGCM_FONT_ASCII_VERTS2];		// cameraz 변화(WorldZ)

	INT32				m_iBeforeFilter;

	INT32				m_iAsciiNextIndex1;								// indexbuffer index와 동일
	INT32				m_iAsciiNextBufferIndex1;						// vertexbuffer index와 동일
	INT32				m_iAsciiNextIndex2;								// vert num과 동일
	INT32				m_iCurFontType;
	INT32				m_iFontBlockStatus;

	BOOL				m_bUseClipping;
	AgcdFontClippingBox	m_stClipArea;
	BOOL				m_bClippingRoughTestResult;

	//숫자 image font용
	FLOAT				m_fImgFontUV[AGCM_IMGFONT_MAX_TYPE_NUM][13][4];
	INT32				m_iImgFontW[AGCM_IMGFONT_MAX_TYPE_NUM][13];				// x width(Endglish)
	INT32				m_iImgFontTexW[AGCM_IMGFONT_MAX_TYPE_NUM];
	INT32				m_iImgFontH[AGCM_IMGFONT_MAX_TYPE_NUM];					// 세로 길이
	INT32				m_iImgFontTexH[AGCM_IMGFONT_MAX_TYPE_NUM];
	RwTexture*			m_pImgFontTexture[AGCM_IMGFONT_MAX_TYPE_NUM];

	// FreeType!!
	FT_Library			m_pFTLibrary;
	FT_Face				m_pFTFaces[AGCM_FONT_MAX_TYPE_NUM];

	// 서유럽 언어를 위해서, 0x80을 넘어가는 글자들이 있다
	int					m_mbcsLetters;
	std::vector<WORD>	m_sjis2uniTable;

private:
	inline int GetJapaneseCharWidth( int fontIndex, char* str );
	
	void SetJapaneseTable();
	int  SJIS_TO_UNICODE( int charIndex );
	INT32 GetMaxTexSize( int max );

public :
	AgcdFontClippingBox		GetCurrentClippingArea( void )		{ return m_stClipArea; }
};

// inline
int AgcmFont::GetJapaneseCharWidth( int fontIndex, char* str )
{
	int  width;
	WORD dc = MAKEWORD( str[1], str[0] );

	if ( dc >= 0x8140 && dc <= 0x8396 && str[1] != 0 )
		width = m_widthJpn[fontIndex][dc - 0x8140];
	else
		width = m_iXLH[fontIndex];

	return width;
}


#endif