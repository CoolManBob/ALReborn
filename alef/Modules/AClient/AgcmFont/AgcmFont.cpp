#include "skeleton.h"
#include "AgcmFont.h"
#include <d3d9.h>

#include "ApMemoryTracker.h"
#include "AuInputComposer.h"

namespace
{
	int GetHalfJapaneseCode(unsigned ch)
	{
		if ( ch < 0x80 ) return ch;
		int uni = ch;

		if ( ch >= 0xA1 && ch <= 0xDF )
			uni += 0xFEC0;

		return uni;
	}

	int GetLatinCode(unsigned ch)
	{
		if ( ch < 0x80 ) return ch;

		int uni = ch;

		switch ( ch )
		{
		case 0x80: uni = 0x20AC; break;
		case 0x82: uni = 0x201A; break;
		case 0x83: uni = 0x0192; break;
		case 0x84: uni = 0x201E; break;
		case 0x85: uni = 0x2026; break;
		case 0x86: uni = 0x2020; break;
		case 0x87: uni = 0x2021; break;
		case 0x88: uni = 0x02C6; break;
		case 0x89: uni = 0x2030; break;
		case 0x8A: uni = 0x0160; break;
		case 0x8B: uni = 0x2039; break;
		case 0x8C: uni = 0x0152; break;
		case 0x8E: uni = 0x017D; break;
		case 0x91: uni = 0x2018; break;
		case 0x92: uni = 0x2019; break;
		case 0x93: uni = 0x201C; break;
		case 0x94: uni = 0x201D; break;
		case 0x95: uni = 0x2022; break;
		case 0x96: uni = 0x2013; break;
		case 0x97: uni = 0x2014; break;
		case 0x98: uni = 0x02DC; break;
		case 0x99: uni = 0x2122; break;
		case 0x9A: uni = 0x0161; break;
		case 0x9B: uni = 0x203A; break;
		case 0x9C: uni = 0x0153; break;
		case 0x9E: uni = 0x017E; break;
		case 0x9F: uni = 0x0178; break;
		case 0xA0: uni = 0x00A0; break;
		case 0xA1: uni = 0x00A1; break;
		case 0xA2: uni = 0x00A2; break;
		case 0xA3: uni = 0x00A3; break;
		case 0xA4: uni = 0x00A4; break;
		case 0xA5: uni = 0x00A5; break;
		case 0xA6: uni = 0x00A6; break;
		case 0xA7: uni = 0x00A7; break;
		case 0xA8: uni = 0x00A8; break;
		case 0xA9: uni = 0x00A9; break;
		case 0xAA: uni = 0x00AA; break;
		case 0xAB: uni = 0x00AB; break;
		case 0xAC: uni = 0x00AC; break;
		case 0xAD: uni = 0x00AD; break;
		case 0xAE: uni = 0x00AE; break;
		case 0xAF: uni = 0x00AF; break;
		case 0xB0: uni = 0x00B0; break;
		case 0xB1: uni = 0x00B1; break;
		case 0xB2: uni = 0x00B2; break;
		case 0xB3: uni = 0x00B3; break;
		case 0xB4: uni = 0x00B4; break;
		case 0xB5: uni = 0x00B5; break;
		case 0xB6: uni = 0x00B6; break;
		case 0xB7: uni = 0x00B7; break;
		case 0xB8: uni = 0x00B8; break;
		case 0xB9: uni = 0x00B9; break;
		case 0xBA: uni = 0x00BA; break;
		case 0xBB: uni = 0x00BB; break;
		case 0xBC: uni = 0x00BC; break;
		case 0xBD: uni = 0x00BD; break;
		case 0xBE: uni = 0x00BE; break;
		case 0xBF: uni = 0x00BF; break;
		case 0xC0: uni = 0x00C0; break;
		case 0xC1: uni = 0x00C1; break;
		case 0xC2: uni = 0x00C2; break;
		case 0xC3: uni = 0x00C3; break;
		case 0xC4: uni = 0x00C4; break;
		case 0xC5: uni = 0x00C5; break;
		case 0xC6: uni = 0x00C6; break;
		case 0xC7: uni = 0x00C7; break;
		case 0xC8: uni = 0x00C8; break;
		case 0xC9: uni = 0x00C9; break;
		case 0xCA: uni = 0x00CA; break;
		case 0xCB: uni = 0x00CB; break;
		case 0xCC: uni = 0x00CC; break;
		case 0xCD: uni = 0x00CD; break;
		case 0xCE: uni = 0x00CE; break;
		case 0xCF: uni = 0x00CF; break;
		case 0xD0: uni = 0x00D0; break;
		case 0xD1: uni = 0x00D1; break;
		case 0xD2: uni = 0x00D2; break;
		case 0xD3: uni = 0x00D3; break;
		case 0xD4: uni = 0x00D4; break;
		case 0xD5: uni = 0x00D5; break;
		case 0xD6: uni = 0x00D6; break;
		case 0xD7: uni = 0x00D7; break;
		case 0xD8: uni = 0x00D8; break;
		case 0xD9: uni = 0x00D9; break;
		case 0xDA: uni = 0x00DA; break;
		case 0xDB: uni = 0x00DB; break;
		case 0xDC: uni = 0x00DC; break;
		case 0xDD: uni = 0x00DD; break;
		case 0xDE: uni = 0x00DE; break;
		case 0xDF: uni = 0x00DF; break;
		case 0xE0: uni = 0x00E0; break;
		case 0xE1: uni = 0x00E1; break;
		case 0xE2: uni = 0x00E2; break;
		case 0xE3: uni = 0x00E3; break;
		case 0xE4: uni = 0x00E4; break;
		case 0xE5: uni = 0x00E5; break;
		case 0xE6: uni = 0x00E6; break;
		case 0xE7: uni = 0x00E7; break;
		case 0xE8: uni = 0x00E8; break;
		case 0xE9: uni = 0x00E9; break;
		case 0xEA: uni = 0x00EA; break;
		case 0xEB: uni = 0x00EB; break;
		case 0xEC: uni = 0x00EC; break;
		case 0xED: uni = 0x00ED; break;
		case 0xEE: uni = 0x00EE; break;
		case 0xEF: uni = 0x00EF; break;
		case 0xF0: uni = 0x00F0; break;
		case 0xF1: uni = 0x00F1; break;
		case 0xF2: uni = 0x00F2; break;
		case 0xF3: uni = 0x00F3; break;
		case 0xF4: uni = 0x00F4; break;
		case 0xF5: uni = 0x00F5; break;
		case 0xF6: uni = 0x00F6; break;
		case 0xF7: uni = 0x00F7; break;
		case 0xF8: uni = 0x00F8; break;
		case 0xF9: uni = 0x00F9; break;
		case 0xFA: uni = 0x00FA; break;
		case 0xFB: uni = 0x00FB; break;
		case 0xFC: uni = 0x00FC; break;
		case 0xFD: uni = 0x00FD; break;
		case 0xFE: uni = 0x00FE; break;
		case 0xFF: uni = 0x00FF; break;
		}

		return uni;
	}
}

AgcmFont::AgcmFont()
{
	//Set Module Name 
	SetModuleName("AgcmFont");
	EnableIdle(TRUE);

	m_pCamera	= NULL;
	m_fScreenZ	= 0.0f;			//Im2D용 
	m_fRecipZ	= 0.0f;			//Im2D용

	ZeroMemory(m_iXLH, sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM);						// x width(한글)
	ZeroMemory(m_iXLE, sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM * 256);
	ZeroMemory(m_iYL, sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM);

	ZeroMemory(m_iTexSize, sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM);					// Texture Size

	m_pColonTexture	= NULL;						// ■ 

	ZeroMemory(m_vTriFan3, sizeof(My2DVertex) * 4);									// UV비변화
	ZeroMemory(m_vTriFan2World, sizeof(RwIm2DVertex) * 4);

	m_iCameraRasterWidth	= 0;
	m_iCameraRasterHeight	= 0;
	
//통맵용
	ZeroMemory(m_fTexCoords, sizeof(FLOAT) * AGCM_FONT_MAX_TYPE_NUM * 95 * 4);
	ZeroMemory(m_bUseShare, sizeof(bool) * AGCM_FONT_MAX_TYPE_NUM);
	ZeroMemory(m_pEnglishTextTexture, sizeof(RwTexture*) * AGCM_FONT_MAX_TYPE_NUM);
	ZeroMemory(m_iShareSize, sizeof(FLOAT) * AGCM_FONT_MAX_TYPE_NUM);

	ZeroMemory(&m_stColor, sizeof(RwRGBA));
	m_bUpdateThisFrame = FALSE;

	ZeroMemory(m_pIndexAscii1, sizeof(INT16) * AGCM_FONT_ASCII_VERTS1_INDEX);
	ZeroMemory(m_vBufferAscii1, sizeof(My2DVertex) * AGCM_FONT_ASCII_VERTS1_VERT);
	ZeroMemory(m_vTriListAscii2, sizeof(RwIm2DVertex) * AGCM_FONT_ASCII_VERTS2);

	m_iBeforeFilter	= 0;

	m_iAsciiNextIndex1	= 0;
	m_iAsciiNextBufferIndex1 = 0;
	m_iAsciiNextIndex2	= 0;								// vert num과 동일
	m_iCurFontType	= 0;
	m_iFontBlockStatus	= 0;

	ZeroMemory(m_fImgFontUV, sizeof(FLOAT) * AGCM_IMGFONT_MAX_TYPE_NUM * 13 * 4);
	ZeroMemory(m_iImgFontW, sizeof(FLOAT) * AGCM_IMGFONT_MAX_TYPE_NUM * 13);
	ZeroMemory(m_iImgFontH, sizeof(FLOAT) * AGCM_IMGFONT_MAX_TYPE_NUM);
	ZeroMemory(m_iImgFontTexW, sizeof(FLOAT) * AGCM_IMGFONT_MAX_TYPE_NUM);
	ZeroMemory(m_iImgFontTexH, sizeof(FLOAT) * AGCM_IMGFONT_MAX_TYPE_NUM);

	INT32	i;

	for(i=0;i<AGCM_FONT_HASH_SIZE;++i)
	{
		m_listTextRaster.hash[i] = NULL;
	}

	memset(m_astFonts, 0, sizeof(FontType) * AGCM_FONT_MAX_TYPE_NUM);

	for(i=0;i<AGCM_FONT_MAX_TYPE_NUM;++i)
	{
		m_astFonts[i].lID = i;
	}

	for(i=0;i<AGCM_IMGFONT_MAX_TYPE_NUM;++i)
	{
		m_pImgFontTexture[i] = NULL;
	}

	m_bUseClipping = FALSE;
	ZeroMemory(&m_stClipArea, sizeof(AgcdFontClippingBox));

	m_pFTLibrary = NULL;
	ZeroMemory(m_pFTFaces, sizeof(FT_Face) * AGCM_FONT_MAX_TYPE_NUM);

	ZeroMemory(m_iTexXLE,sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM * 256 );
	ZeroMemory(m_iTexYL,sizeof(INT32) * AGCM_FONT_MAX_TYPE_NUM);
	m_bClippingRoughTestResult	= FALSE;

	m_mbcsLetters = g_eServiceArea == AP_SERVICE_AREA_WESTERN ? 1 : 2;

	if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
		SetJapaneseTable();

	InitFuncPtr();
}

AgcmFont::~AgcmFont()
{

}

BOOL AgcmFont::InitFuncPtr()
{
	if (AP_SERVICE_AREA_KOREA == g_eServiceArea )
	{
		GetTextExtentPtr = &AgcmFont::GetTextExtentKr;
		GetCharacterWidthPtr = &AgcmFont::GetCharacterWidthKr;
		IsLeadBytePtr = &AgcmFont::IsLeadByteKr;
	}

	if (AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		GetTextExtentPtr = &AgcmFont::GetTextExtentCn;
		GetCharacterWidthPtr = &AgcmFont::GetCharacterWidthCn;
		IsLeadBytePtr = &AgcmFont::IsLeadByteCn;
	}

	if (AP_SERVICE_AREA_WESTERN == g_eServiceArea )
	{
		GetTextExtentPtr = &AgcmFont::GetTextExtentEn;
		GetCharacterWidthPtr = &AgcmFont::GetCharacterWidthEn;
		IsLeadBytePtr = &AgcmFont::IsLeadByteEn;
	}

	if (AP_SERVICE_AREA_JAPAN == g_eServiceArea )
	{
		GetTextExtentPtr = &AgcmFont::GetTextExtentJp;
		GetCharacterWidthPtr = &AgcmFont::GetCharacterWidthJp;
		IsLeadBytePtr = &AgcmFont::IsLeadByteJp;
	}

	return TRUE;
}

BOOL AgcmFont::OnAddModule()
{
	SetWorldCamera(GetCamera());

	return TRUE;
}

BOOL AgcmFont::OnDestroy()
{
	TextRaster	*cur_node,*del_node;
	int	i;

	for(i =0;i<AGCM_FONT_HASH_SIZE;++i)
	{
		cur_node = m_listTextRaster.hash[i];

		while(cur_node)
		{
			del_node = cur_node;
			cur_node = cur_node->next;

			if(del_node->Ras)	RwRasterDestroy(del_node->Ras);
			del_node->Ras = NULL;
			delete del_node;
		}
	}

	if (m_pColonTexture)
	{
		RwTextureDestroy(m_pColonTexture);
		m_pColonTexture = NULL;
	}

	for(i=0;i<AGCM_FONT_MAX_TYPE_NUM;++i)
	{
		if(m_pEnglishTextTexture[i])
		{
			RwTextureDestroy( m_pEnglishTextTexture[i] );
			m_pEnglishTextTexture[i] = NULL;
		}
	}

	for(i=0;i<AGCM_IMGFONT_MAX_TYPE_NUM;++i)
	{
		if(m_pImgFontTexture[i])
		{
			RwTextureDestroy(m_pImgFontTexture[i]);
			m_pImgFontTexture[i] = NULL;
		}
	}

	for(i=0;i<AGCM_FONT_MAX_TYPE_NUM;++i)
	{
		FT_Done_Face(m_pFTFaces[i]);
	}
	
	return TRUE;
}

BOOL AgcmFont::OnInit()
{
	INT32	lFontMaxType;
	lFontMaxType = 0;

	char* FontName = 0;

	switch ( g_eServiceArea )
	{
	case AP_SERVICE_AREA_CHINA:
		FontName = "FZCuYuan-M03S";
		break;

	case AP_SERVICE_AREA_WESTERN:
		FontName = "Georgia";
		break;

	case AP_SERVICE_AREA_JAPAN:
		FontName = "MS UI Gothic";
		break;

	default:
		FontName = "가시B";
	}

	#ifdef USE_MFC
	m_astFonts[lFontMaxType].lID = lFontMaxType;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 13;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = lFontMaxType;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 12;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 2;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 11;
	++lFontMaxType;

	// Quest에서 사용함 (AgcmTextBoard) 일단 하드코딩함... (parn, 2004/10/25 04:03)
	m_astFonts[lFontMaxType].lID = 3;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 30;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 4;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 15;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 5;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 20;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 6;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/etc/af1.grp");
	m_astFonts[lFontMaxType].lSize = 45;
	++lFontMaxType;
	#else
	m_astFonts[lFontMaxType].lID = lFontMaxType;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = g_eServiceArea == AP_SERVICE_AREA_WESTERN ? 11 : 13;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = lFontMaxType;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = g_eServiceArea == AP_SERVICE_AREA_WESTERN ? 11 : 12;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 2;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = g_eServiceArea == AP_SERVICE_AREA_WESTERN ? 10 : 11;
	++lFontMaxType;

	// Quest에서 사용함 (AgcmTextBoard) 일단 하드코딩함... (parn, 2004/10/25 04:03)
	m_astFonts[lFontMaxType].lID = 3;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = 30;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 4;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = 15;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 5;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = 20;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 6;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = 40;
	++lFontMaxType;

	m_astFonts[lFontMaxType].lID = 7;
	strcpy(m_astFonts[lFontMaxType].szType, FontName);
	strcpy(m_astFonts[lFontMaxType].szFileName, "Texture/NotPacked/af1.grp");
	m_astFonts[lFontMaxType].lSize = 25;
	++lFontMaxType;
	#endif

	m_pImgFontTexture[0] = RwTextureRead("Img_Font1.png", NULL);

	#ifndef USE_MFC
	ASSERT(m_pImgFontTexture[0] && "Img_Font1.png 읽기 실패!");
	#endif
	
	if (m_pImgFontTexture[0])
	{
		RwTextureSetFilterMode(m_pImgFontTexture[0],rwFILTERNEAREST);
		RwTextureSetAddressing (m_pImgFontTexture[0],rwTEXTUREADDRESSCLAMP );

		m_iImgFontTexW[0] = 8;
		m_iImgFontTexH[0] = 16;

		m_iImgFontW[0][0] = 3;		// '.'
		m_iImgFontW[0][1] = 6;		// '/'
		m_iImgFontW[0][2] = 7;		// '0'
		m_iImgFontW[0][3] = 6;		// '1'
		m_iImgFontW[0][4] = 7;		// '2'
		m_iImgFontW[0][5] = 6;		// '3'
		m_iImgFontW[0][6] = 7;		// '4'
		m_iImgFontW[0][7] = 7;		// '5'
		m_iImgFontW[0][8] = 7;		// '6'
		m_iImgFontW[0][9] = 6;		// '7'
		m_iImgFontW[0][10] = 7;		// '8'
		m_iImgFontW[0][11] = 7;		// '9'
		m_iImgFontW[0][12] = 8;		// '~'

		m_iImgFontH[0] = 10;

		RwRaster*	pRaster = RwTextureGetRaster(m_pImgFontTexture[0]);
		FLOAT		imgW = (FLOAT)RwRasterGetWidth(pRaster);
		FLOAT		imgH = (FLOAT)RwRasterGetHeight(pRaster);

		INT32		curX = 0;
		INT32		curY = 0;

		for( int j = 0; j < 13; ++j )
		{
			m_fImgFontUV[0][j][0] = (FLOAT)curX / imgW;
			m_fImgFontUV[0][j][1] = (FLOAT)curY / imgH;
			m_fImgFontUV[0][j][2] = ((FLOAT)(curX + m_iImgFontTexW[0])) / imgW;
			m_fImgFontUV[0][j][3] = ((FLOAT)(curY + m_iImgFontTexH[0])) / imgH;

			curX += m_iImgFontTexW[0];
		}
	}

	for( int i = 0; i < 4; ++i )
	{
		m_vTriFan3[i].z = 0.0f;
		m_vTriFan3[i].rhw = 1.0f;
	}
	
	m_vTriFan3[0].u = 1.0f;
	m_vTriFan3[0].v = 1.0f;

	m_vTriFan3[1].u = 1.0f;
	m_vTriFan3[1].v = 0.0f;

	m_vTriFan3[2].u = 0.0f;
	m_vTriFan3[2].v = 0.0f;
	
	m_vTriFan3[3].u = 0.0f;
	m_vTriFan3[3].v = 1.0f;

	// FreeType
	int error = FT_Init_FreeType(&m_pFTLibrary);
	if(error)
	{
		ASSERT(0);
		return TRUE;
	}

	for( int fontIndex = 0; fontIndex < lFontMaxType; ++fontIndex )
	{
		error = FT_New_Face( m_pFTLibrary,m_astFonts[fontIndex].szFileName, fontIndex, &m_pFTFaces[fontIndex] );
		if ( error == FT_Err_Unknown_File_Format )
		{
			ASSERT(!"FT_Err_Unknown_File_Format!");
		}
		else if ( error )
		{
			char str[100];
			sprintf( str, "%s Font File이 없습니다.!", m_astFonts[fontIndex].szFileName);

			ASSERT(!str);

			return error;
		}

		error = FT_Set_Char_Size(
				m_pFTFaces[fontIndex],					// handle to face object
				0,//m_astFonts[i].lSize  * 64,			// char_width in 1/64th of points  
				m_astFonts[fontIndex].lSize * 64,		// char_height in 1/64th of points 
				0,										// horizontal device resolution    
				0);										// vertical device resolution      
		if ( error )
		{
			ASSERT(!"FT_Err_Set_CharSize!");
			return error;
		}

		FT_Select_Charmap( m_pFTFaces[fontIndex],FT_ENCODING_UNICODE );
	}

	//
	// 글자 간격을 구한다.
	//
	int	calcStr;
	int charWidth = 0;

	for( int fontIndex = 0; fontIndex < lFontMaxType; ++fontIndex )
	{
		for( int k = 0; k < 256; ++k)
		{
			if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
				calcStr = GetHalfJapaneseCode(k);
			else
				calcStr = GetLatinCode(k);

			error = FT_Load_Char( m_pFTFaces[fontIndex], calcStr, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);
			charWidth = m_pFTFaces[fontIndex]->glyph->bitmap.width;

			if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
				if (k >= 48 && k <= 57) ++charWidth;

			m_iXLE[fontIndex][k]    = charWidth;
			m_iTexXLE[fontIndex][k] = GetMaxTexSize( charWidth );
		}
		
		m_iXLE[fontIndex][32] = (INT32)((FLOAT)m_astFonts[fontIndex].lSize * 0.6f);		// 공백 문자

		if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		{
			m_iXLH[fontIndex] = (INT32)((FLOAT)m_astFonts[fontIndex].lSize * 1.1f);
		}
		else if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
		{
			m_iXLH[fontIndex] = (INT32)(FLOAT)m_astFonts[fontIndex].lSize;
		}
		else
		{
			m_iXLH[fontIndex] = (INT32)((FLOAT)m_astFonts[fontIndex].lSize * 0.9f);
		}

		m_iYL[fontIndex] = m_iXLH[fontIndex];

		// 일본어의 경우에는 따로 글자 크기를 관리하자. 카타카나/히라가나/전각숫자, 0x813F ~ 0x839F
		if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
		{
			m_widthJpn[fontIndex].push_back( (INT32)((FLOAT)m_astFonts[fontIndex].lSize * 0.6f) );	// 공백 문자

			for ( int charIndex = 0x8141; charIndex <= 0x8396; ++charIndex )
			{
				error = FT_Load_Char( m_pFTFaces[fontIndex], SJIS_TO_UNICODE(charIndex), FT_LOAD_RENDER | FT_LOAD_NO_BITMAP );
				charWidth = m_pFTFaces[fontIndex]->glyph->bitmap.width;

				if ( 0x815B == charIndex ) charWidth += 2;

				m_widthJpn[fontIndex].push_back( charWidth );
			}
		}
	}

	for ( int fontIndex = 0; fontIndex < lFontMaxType; ++fontIndex )
		m_iTexYL[fontIndex] = GetMaxTexSize( m_iYL[fontIndex] );
	
	//좌우 상하 여분 1픽셀씩 주자 
	for ( int fontIndex = 0; fontIndex < lFontMaxType; ++fontIndex )
	{
		// 이해 할수 없는 코드. 왜 m_iTexSize[fontIndex]를 두번에 걸처 계산하는지 모르겠음 by skcho
		m_iTexSize[fontIndex] = GetMaxTexSize( m_iXLH[fontIndex]+2 );
		m_iTexSize[fontIndex] = GetMaxTexSize( m_iYL[fontIndex]+2 );
	}

	// Caret 만들기 (■)
	RwImage *image = RwImageCreate(8, 8, 32);

	if (!RwImageAllocatePixels(image))
	{
	   RwImageDestroy(image);
	   return FALSE;
	}
		
	RwRGBA  *rpDestin = (RwRGBA*) image->cpPixels;

	int tx, ty;
	for( ty=0; ty < 8; ++ty )
	{
		for( tx=0; tx < 8; ++tx )
		{
			DWORD	tcolor = 0xffffffff;
			RwRGBASetFromPixel(rpDestin,tcolor,rwRASTERFORMAT8888 );
					
			++rpDestin;
		}
	}

	RwRaster*	ras = RwRasterCreate( 8, 8, 16, rwRASTERTYPETEXTURE | rwRASTERFORMAT1555);
	RwRasterSetFromImage(ras,image);			
	m_pColonTexture = RwTextureCreate(ras);
	RwTextureSetFilterMode(m_pColonTexture,rwFILTERNEAREST);
	RwImageDestroy(image);

	// 통alphbet 이미지 만들기
	int		share_font_num = 94;
	int		share_size;		// 128 or 256

	for( int fontIndex = 0; fontIndex < lFontMaxType; ++fontIndex )
	{
		share_size = 128;

		// Calculate the dimensions for the smallest power-of-two texture which
		// can hold all the printable characters
		share_size = 128;
		while( 2 == PaintAlphabet( NULL, share_size, share_size, fontIndex, true ) )
		{
			share_size <<= 1;
		}

		if(share_size > 512) // 256초과
		{
			m_bUseShare[fontIndex] = false;
			m_pEnglishTextTexture[fontIndex] = NULL;

			ASSERT(!"m_pEnglishTextTexture 생성 실패!");	
		}
		else		// 통맵 만든다
		{
			m_bUseShare[fontIndex] = true;
			m_iShareSize[fontIndex] = (FLOAT)share_size;
			m_pEnglishTextTexture[fontIndex] = CreateShareTexture( share_size, share_size, fontIndex );

			ASSERT(m_pEnglishTextTexture[fontIndex]);
		}
	}

	m_bUpdateThisFrame = false;

	// Ascii용 POOL
	for( int i = 0; i < AGCM_FONT_ASCII_VERTS1_VERT; ++i )
	{
		m_vBufferAscii1[i].z = 0.0f;
		m_vBufferAscii1[i].rhw = 1.0f;
	}

	INT32	iBufferIndex = 0;
	for( int i = 0; i < AGCM_FONT_ASCII_VERTS1_INDEX; i += 6 )
	{
		m_pIndexAscii1[i] = iBufferIndex;
		m_pIndexAscii1[i+1] = iBufferIndex + 1;
		m_pIndexAscii1[i+2] = iBufferIndex + 2;
		m_pIndexAscii1[i+3] = iBufferIndex + 2;
		m_pIndexAscii1[i+4] = iBufferIndex + 3;
		m_pIndexAscii1[i+5] = iBufferIndex;

		iBufferIndex += 4;
	}

	return TRUE;
}

BOOL AgcmFont::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmFont::OnIdle");
	
	if(m_bUpdateThisFrame)
	{
		m_bUpdateThisFrame = false;
	
		//hashtable update
		TextRaster	*cur_node,*del_node,*before_node;
		for(INT32 i =0;i<AGCM_FONT_HASH_SIZE;++i)
		{
			before_node = cur_node = m_listTextRaster.hash[i];

			while(cur_node)
			{
				if(--cur_node->count < 0)
				{
					del_node = cur_node;
					if(cur_node == m_listTextRaster.hash[i])
					{
						m_listTextRaster.hash[i] = cur_node->next;
						before_node = m_listTextRaster.hash[i];
					}
					else
					{
						before_node->next = cur_node->next;
					}

					cur_node = cur_node->next;

					if(del_node->Ras)	RwRasterDestroy(del_node->Ras);
					del_node->Ras = NULL;
					delete	del_node;
				}
				else
				{
					before_node = cur_node;
					cur_node = cur_node->next;
				}
			}
		}
	}

	return TRUE;
}

int		AgcmFont::PaintAlphabet(DWORD* pBitMapBits, int TexW, int TexH, int ftype,BOOL bMeasureOnly )
{
	DWORD x = 2;
    DWORD y = 2;

	CHAR	str[2];
	str[1] = '\0';
    
    // For each character, draw text on the DC and advance the current position
    for( char c = 32; c < 127; ++c )
    {
		str[0] = c;
        if( (DWORD)(x + m_iTexXLE[ftype][c]) > (DWORD)TexW )
        {
            x  = 2;
            y += m_iTexYL[ftype] +2;
        }

        // Check to see if there's room to write the character here
        if( y + m_iTexYL[ftype] > (DWORD) TexH )
            return 2;				// 더 많은 사이즈 필요
           
        FT_GlyphSlot  slot = m_pFTFaces[ftype]->glyph;
		int error;
		error = FT_Load_Char( m_pFTFaces[ftype], str[0], FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);
		FT_Bitmap	bitmap = slot->bitmap;
		int fh = bitmap.rows;
		int fw = bitmap.width;
		DWORD			nw_color;
		unsigned char	a;
		int		tempx,tempy;

		tempy = y + m_astFonts[ftype].lSize - slot->bitmap_top;
		tempx = x + slot->bitmap_left;

		tempy -= 1;
		
		if( !bMeasureOnly )
		{
			for(int fy=0; fy < fh; ++fy )
			{
				for(int fx=0; fx < fw; ++fx )
				{
					a = (bitmap.buffer[fy * fw + fx]);
					if(a==00)
					{
						continue;
					}
					
					nw_color = 0x00ffffff | a << 24;
					if(tempy + fy >= TexH || tempx + fx >= TexW) continue;

					pBitMapBits[(tempy + fy) * TexW + (tempx+fx)] = nw_color;
				}
			}
			
			m_fTexCoords[ftype][c-32][0] = ((FLOAT)(x + slot->bitmap_left ))/(FLOAT)TexW;
			m_fTexCoords[ftype][c-32][1] = ((FLOAT)(y))/(FLOAT)TexH;
			m_fTexCoords[ftype][c-32][2] = ((FLOAT)(x + m_iTexXLE[ftype][c] + slot->bitmap_left ))/(FLOAT)TexW;
			m_fTexCoords[ftype][c-32][3] = ((FLOAT)(y + m_iTexYL[ftype]))/(FLOAT)TexH;
		}
		
		x += m_iTexXLE[ftype][c] + slot->bitmap_left + 2;
    }
	return 1;
}

RwRaster*	AgcmFont::MakeFontRasterFromFTBits(FT_GlyphSlot	slot,int w,int h)
{
	FT_Bitmap*	bitmap = &slot->bitmap;
	
	// Render to Raster(일단 32bit로 생각)
	RwImage            *image = RwImageCreate(w, h, 32);

	if (!RwImageAllocatePixels(image))
	{
	   RwImageDestroy(image);
	   return NULL;
	}

	int				i,j;//,k,l;
	unsigned char	a;
	RwRaster*		new_raster;
	DWORD			nw_color;
	RwRGBA			*rpDestin = (RwRGBA*) image->cpPixels;

	// clear
	for( i=0; i < h; ++i )
	{
		rpDestin = (RwRGBA*) (image->cpPixels + i * image->stride);
		for( j=0; j < w; ++j )
		{
			RwRGBASetFromPixel(rpDestin,AGCM_FONT_WHITE_CLEAR_COLOR,rwRASTERFORMAT8888 );
			++rpDestin;
		}
	}

	for( i=0; i < bitmap->rows; ++i )
	{
		rpDestin = (RwRGBA*) (image->cpPixels + i * image->stride);
		for( j=0; j < bitmap->width; ++j )
		{
			a = (bitmap->buffer[i * bitmap->width + j]);
			if(a==00)
			{
				++rpDestin;
				continue;
			}
			
			nw_color = 0x00ffffff | a << 24;
			RwRGBASetFromPixel(rpDestin,nw_color,rwRASTERFORMAT8888 );
			
			++rpDestin;
		}
	}

	new_raster = RwRasterCreate( w, h,  32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
	// edited by netong
	if (!new_raster)
	{
		RwImageDestroy(image);
		return NULL;
	}
	// edited by netong
	RwRasterSetFromImage(new_raster,image);			
	RwImageDestroy(image);

	return new_raster;
}

RwRaster*	AgcmFont::MakeFontRasterFromBitMapBits(DWORD* pBitmapBits,int w,int h)
{
	// Render to Raster(일단 32bit로 생각)
	RwImage            *image = RwImageCreate(w, h, 32);

	if (!RwImageAllocatePixels(image))
	{
	   RwImageDestroy(image);
	   return NULL;
	}
	
	RwRGBA			*rpDestin = (RwRGBA*) image->cpPixels;
	int				i, j;
	DWORD			nw_color;
	RwRaster*		new_raster;

	for( i=0; i < h; ++i )
	{
		rpDestin = (RwRGBA*) (image->cpPixels + i * image->stride);
		for( j=0; j < w; ++j )
		{
			RwRGBASetFromPixel(rpDestin,AGCM_FONT_WHITE_CLEAR_COLOR,rwRASTERFORMAT8888 );
			++rpDestin;
		}
	}

	for( i=0; i < h; ++i )
	{
		rpDestin = (RwRGBA*) (image->cpPixels + i * image->stride);
		for( j=0; j < w; ++j )
		{
			nw_color = pBitmapBits[i * w + j];
			if(nw_color==00)
			{
				++rpDestin;
				continue;
			}
			
			RwRGBASetFromPixel(rpDestin,nw_color,rwRASTERFORMAT8888 );
			++rpDestin;
		}
	}

	new_raster = RwRasterCreate( w, h,  32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
	// edited by netong
	if (!new_raster)
	{
		RwImageDestroy(image);
		return NULL;
	}
	// edited by netong
	RwRasterSetFromImage(new_raster,image);			
	RwImageDestroy(image);

	return new_raster;
}

RwTexture*	AgcmFont::CreateShareTexture(int w,int h,int type)
{
	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  w;
	bmi.bmiHeader.biHeight      = -h;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;

	// Create a DC and a bitmap for the font
	HDC     hDC       = CreateCompatibleDC( NULL );
	HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
										  (VOID**)&pBitmapBits, NULL, 0 );

	// edited by netong
	if (!pBitmapBits)
	{
		DeleteDC( hDC );
		return NULL;
	}
	// edited by netong

	SetMapMode( hDC, MM_TEXT );

	// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
	// antialiased font, but this is not guaranteed.
	SelectObject( hDC, hbmBitmap );

	PaintAlphabet(pBitmapBits,w,h,type,false);

	RwRaster*	new_raster = MakeFontRasterFromBitMapBits(pBitmapBits,w,h);
	
	DeleteObject( hbmBitmap );
	DeleteDC( hDC );

	RwTexture*	new_texture;
	
	if(new_raster)
	{
		new_texture = RwTextureCreate(new_raster);
		RwTextureSetFilterMode(new_texture,rwFILTERNEAREST);
	}
	else
	{
		new_texture = NULL;
	}

	return new_texture;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Immediate 2D Mode
/////////////////////////////////////////////////////////////////////////////////////////////////////
void	AgcmFont::DrawTextIMMode2DWorld(float x,float y,float screen_z,float recip_z,char* pstr,UINT32 ftype,UINT8 alpha,DWORD color,bool bDrawShadow,bool bDrawBack,DWORD backcolor)
{
	PROFILE("AgcmFont::DrawTextIMMode2DWorld");

	x = ( float ) ( ( int ) x ) + 0.5f;
	y = ( float ) ( ( int ) y ) + 0.5f;

	if(m_pFTFaces[ftype] == NULL) return;

	//RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERLINEAR );

	if( x > m_iCameraRasterWidth || y < -m_astFonts[ftype].lSize || y > m_iCameraRasterHeight)
		return;		// cliping

	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;
	RwV2d	shadow1,shadow2;

	for(int i=0;i<4;++i)
	{
		RwIm2DVertexSetScreenZ(&m_vTriFan2World[i],screen_z);
		RwIm2DVertexSetRecipCameraZ(&m_vTriFan2World[i], recip_z);

		RwIm2DVertexSetU(&m_vTriFan2World[0], 1.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[0], 1.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[1], 1.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[1], 0.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[2], 0.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[2], 0.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[3], 0.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[3], 1.0f,recip_z);
	}

	if(bDrawBack)
	{
		// backboard draw
		int xwidth = GetTextExtent( ftype, pstr, strlen(pstr) );

		float w = (float)xwidth + 6;
		float h = (float) m_iYL[ftype] + 4;

		memcpy(&m_stColor , &backcolor, 4);

		RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+w-3);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+h-2); 
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[0], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+w-3);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y-2); 
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[1], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x-3);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y-2); 
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[2], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x-3);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+h-2); 
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[3], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pColonTexture));
		RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 
	}

	memcpy(&m_stColor , &color, 4);

	while(*(pstr+s) != '\0')
	{
		ChatPt.x += xoffset;
		shadow1.x = ChatPt.x + 1; shadow1.y = ChatPt.y ;
		shadow2.x = ChatPt.x ; shadow2.y = ChatPt.y + 1;
			
		if( IsLeadByte( pstr[s] ) ) // DBCS
		{
			RenderIMMode2DWORLD(ChatPt.x-1,ChatPt.y-1,screen_z,recip_z,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);
			
			xoffset = GetCharacterWidth( ftype, pstr + s );
			++s;
		}
		else
		{
			RenderIMMode2DWORLD(ChatPt.x,ChatPt.y,screen_z,recip_z,pstr+s,1,ftype,alpha,bDrawShadow);

			xoffset = m_iXLE[ftype][(unsigned char)pstr[s]];
		}

		++s;
	}
}

void	AgcmFont::DrawTextIMMode2DWorldScale(float x,float y,float size,float screen_z,float recip_z,char* pstr,UINT32 ftype,UINT8 alpha,DWORD color,bool bDrawShadow)
{
	PROFILE("AgcmFont::DrawTextIMMode2DWorldScale");
	//RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERLINEAR );

	x = ( float ) ( ( int ) x ) + 0.5f;
	y = ( float ) ( ( int ) y ) + 0.5f;

	if(m_pFTFaces[ftype] == NULL) return;

	if( x > m_iCameraRasterWidth || y < -m_astFonts[ftype].lSize || y > m_iCameraRasterHeight)
		return;		// cliping

	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;
	RwV2d	shadow1,shadow2;

	for(int i=0;i<4;++i)
	{
		RwIm2DVertexSetScreenZ(&m_vTriFan2World[i],screen_z);
		RwIm2DVertexSetRecipCameraZ(&m_vTriFan2World[i], recip_z);
	}

	memcpy(&m_stColor , &color, 4);
		
	while(*(pstr+s) != '\0')
	{
		ChatPt.x += xoffset;
		shadow1.x = ChatPt.x + 1; shadow1.y = ChatPt.y ;
		shadow2.x = ChatPt.x ; shadow2.y = ChatPt.y + 1;
			
		if( IsLeadByte( pstr[s] ) )// DBCS
		{
			RenderIMMode2DWORLD(ChatPt.x-1.0f*size,ChatPt.y-1.0f*size,screen_z,recip_z,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow,size);
			
			xoffset = GetCharacterWidth( ftype, pstr + s );
			++s;
		}
		else
		{
			RenderIMMode2DWORLD(ChatPt.x,ChatPt.y,screen_z,recip_z,pstr+s,1,ftype,alpha,bDrawShadow,size);
			xoffset = m_iXLE[ftype][(unsigned char)pstr[s]];
		}
		
		++s;
	}
}

void	AgcmFont::DrawCaret(float x,float y,UINT32 fonttype, DWORD color, int width)
{
	if(!m_pColonTexture) return;

	//float w = (float) m_iXLH[fonttype];
	float h = (float) m_iYL[fonttype];

	y += 1;

	DWORD	Vcolor = color;

	m_vTriFan3[0].x = x + width;
	m_vTriFan3[0].y = y+h;
	m_vTriFan3[0].color = Vcolor;

	m_vTriFan3[1].x = x + width;
	m_vTriFan3[1].y = y;
	m_vTriFan3[1].color = Vcolor;

	m_vTriFan3[2].x = x;
	m_vTriFan3[2].y = y;
	m_vTriFan3[2].color = Vcolor;

	m_vTriFan3[3].x = x;
	m_vTriFan3[3].y = y+h;
	m_vTriFan3[3].color = Vcolor;
	
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pColonTexture));

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
}

void	AgcmFont::DrawCaretE(float x,float y,UINT32 fonttype, DWORD color)
{
	if(!m_pColonTexture) return;

	float w = 2.0f;
	float h = (float) m_iYL[fonttype];

	y += 1;

	DWORD	Vcolor = color;

	m_vTriFan3[0].x = x+w;
	m_vTriFan3[0].y = y+h;
	m_vTriFan3[0].color = Vcolor;

	m_vTriFan3[1].x = x+w;
	m_vTriFan3[1].y = y;
	m_vTriFan3[1].color = Vcolor;

	m_vTriFan3[2].x = x;
	m_vTriFan3[2].y = y;
	m_vTriFan3[2].color = Vcolor;

	m_vTriFan3[3].x = x;
	m_vTriFan3[3].y = y+h;
	m_vTriFan3[3].color = Vcolor;
	
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pColonTexture));

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
}

void	AgcmFont::DrawTextIM2DEx(float x,float y,char* pstr, UINT sizeofchar, UINT32 ftype,UINT8 alpha,DWORD color, bool bDrawShadow,bool bBold,bool bDrawBack,DWORD backcolor)
{
		PROFILE("AgcmFont::DrawTextIM2D");
	//RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERLINEAR );

	if(m_pFTFaces[ftype] == NULL) return;

	if(bDrawBack)
	{
		int			xwidth = GetTextExtent(ftype, pstr, sizeofchar);

		float w = (float)xwidth;
		
		// 2005. 08. 01 Nonstopdj
		// m_iYL[ftype]의 높이와 실제 출력되는 font의 높이가 맞지 않는다.
		// m_astFonts[ftype].lSize로 수정하여 정상출력
		//float h = (float) m_iYL[ftype];
		float h = (float)m_astFonts[ftype].lSize;

		// 마고자 (2005-03-30 오후 5:03:27) : 공식이 잘못 돼어있따
		// m_iColor2 = color | (alpha & 0xff) << 24; --> 칼라에 알파값을 무시한다.
		// 쓰는건 backcolor인데 m_uColor2를 왜 계산할까..;
		UINT8 ualphaBack = ( ( INT32 ) ( ( FLOAT ) ( ( backcolor & 0xff000000 ) >> 24 ) * ( FLOAT ) alpha / 255.0f ) ) & 0xff;
		backcolor =	( backcolor & 0x00ffffff ) | ( (ualphaBack & 0xff) << 24 );
	
		// 2005. 08. 01 Nonstopdj
		// 정확한 출력을 위해 vertex좌표에 offset(0.5f)를 걸어준다.
		// 여전히 폰트의 위치와 Backboard의 위치는 어긋나지만 이전보다 깔끔한 박스가 그려졌음.
		
		// offset걸기. 텍셀과 픽셀을 정확하게 일치시킨다.
		x -= 0.5f;	
		y -= 0.5f;

		// [수정사항] : x축으로 한픽셀을 더 밀어서 찍히는 문자와 일치시켰음.
		m_vTriFan3[0].x = x+w	 - 1.0f;	
		m_vTriFan3[0].y = y+h;
		m_vTriFan3[0].color = backcolor;

		m_vTriFan3[1].x = x+w	 - 1.0f;
		m_vTriFan3[1].y = y;
		m_vTriFan3[1].color = backcolor;

		m_vTriFan3[2].x = x		 - 1.0f;
		m_vTriFan3[2].y = y;
		m_vTriFan3[2].color = backcolor;

		m_vTriFan3[3].x = x 	 - 1.0f;
		m_vTriFan3[3].y = y+h;
		m_vTriFan3[3].color = backcolor;
		
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pColonTexture));

		RwD3D9SetFVF(MY2D_VERTEX_FLAG);
		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
	}
	
	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;

	// 마고자 (2005-03-30 오후 5:03:27) : 공식이 잘못 돼어있따
	// m_iColor2 = color | (alpha & 0xff) << 24; --> 칼라에 알파값을 무시한다.
	m_iColor2 = ( color & 0x00ffffff ) | ( (alpha & 0xff) << 24 );

	m_bClippingRoughTestResult = FALSE;
	if(m_bUseClipping)
	{
		if(x < m_stClipArea.x || y < m_stClipArea.y)
		{
			m_bClippingRoughTestResult = TRUE;
		}
		else
		{
			int fw = GetTextExtent( ftype, pstr, strlen(pstr) );
			if( x + fw > m_stClipArea.x + m_stClipArea.w )
			{
				m_bClippingRoughTestResult = TRUE;
			}
			else if( y + m_iYL[ftype] > m_stClipArea.y + m_stClipArea.h )
			{
				m_bClippingRoughTestResult = TRUE;
			}
		}
	}
	
	if(bBold)
	{
		// 2005. 08. 01 Nonstopdj
		// 주어진 길이(sizofchar)만큼만 그린다.
		while( sizeofchar > ( UINT ) s ) //|| *(pstr+s) != '\0')
		{
			ChatPt.x += xoffset;
			if( IsLeadByte( pstr[s] ) ) // DBCS
			{
				PrepareIMMode2D(ChatPt.x-1,ChatPt.y-1,pstr+s,m_mbcsLetters,ftype,alpha,false);
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);

				xoffset = GetCharacterWidth( ftype, pstr + s );
				++s;
			}
			else
			{
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,false);
				PrepareIMMode2D(ChatPt.x+1,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow);

				xoffset = m_iXLE[ftype][ (unsigned char)pstr[s] ];
			}
			
			++s;
		}
	}
	else
	{
		// 2005. 08. 01 Nonstopdj
		// 주어진 길이(sizofchar)만큼만 그린다.
		while( sizeofchar > ( UINT ) s ) //|| *(pstr+s) != '\0')
		{
			ChatPt.x += xoffset;
			if( IsLeadByte( pstr[s] ) ) // DBCS
			{
				PrepareIMMode2D(ChatPt.x-1,ChatPt.y-1,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);

				xoffset = GetCharacterWidth( ftype, pstr + s );
				++s;
			}
			else
			{
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow);
				xoffset = m_iXLE[ftype][ (unsigned char)pstr[s] ];
			}
			
			++s;
		}
	}
}

void	AgcmFont::DrawTextIM2D(float x,float y,char* pstr,UINT32 ftype,UINT8 alpha,DWORD color, bool bDrawShadow,bool bBold,bool bDrawBack,DWORD backcolor, INT32 length)
{
	PROFILE("AgcmFont::DrawTextIM2D");
	//RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERLINEAR );

	if(m_pFTFaces[ftype] == NULL) return;

	if(bDrawBack)
	{
		// backboard draw
		int		xwidth = this->GetTextExtent(ftype, pstr, length);

		float w = (float)xwidth;
//		float h = (float) m_iYL[ftype];
		float h = (float) m_astFonts[ftype].lSize + 2;

		// 마고자 (2005-03-30 오후 5:03:27) : 공식이 잘못 돼어있따
		// m_iColor2 = color | (alpha & 0xff) << 24; --> 칼라에 알파값을 무시한다.
		// 쓰는건 backcolor인데 m_uColor2를 왜 계산할까..;
		UINT8 ualphaBack = ( ( INT32 ) ( ( FLOAT ) ( ( backcolor & 0xff000000 ) >> 24 ) * ( FLOAT ) alpha / 255.0f ) ) & 0xff;
		backcolor =	( backcolor & 0x00ffffff ) | ( (ualphaBack & 0xff) << 24 );
	
		m_vTriFan3[0].x = x+w;
		m_vTriFan3[0].y = y+h;
		m_vTriFan3[0].color = backcolor;

		m_vTriFan3[1].x = x+w;
		m_vTriFan3[1].y = y;
		m_vTriFan3[1].color = backcolor;

		m_vTriFan3[2].x = x;
		m_vTriFan3[2].y = y;
		m_vTriFan3[2].color = backcolor;

		m_vTriFan3[3].x = x;
		m_vTriFan3[3].y = y+h;
		m_vTriFan3[3].color = backcolor;
		
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pColonTexture));

		RwD3D9SetFVF(MY2D_VERTEX_FLAG);
		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
	}
	
	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;

	// 마고자 (2005-03-30 오후 5:03:27) : 공식이 잘못 돼어있따
	// m_iColor2 = color | (alpha & 0xff) << 24; --> 칼라에 알파값을 무시한다.
	m_iColor2 = ( color & 0x00ffffff ) | ( (alpha & 0xff) << 24 );

	m_bClippingRoughTestResult = FALSE;
	if(m_bUseClipping)
	{
		if(x < m_stClipArea.x || y < m_stClipArea.y)
		{
			m_bClippingRoughTestResult = TRUE;
		}
		else
		{
			int fw = GetTextExtent( ftype, pstr, strlen(pstr) );
			if( x + fw > m_stClipArea.x + m_stClipArea.w )
			{
				m_bClippingRoughTestResult = TRUE;
			}
			else if( y + m_iYL[ftype] > m_stClipArea.y + m_stClipArea.h )
			{
				m_bClippingRoughTestResult = TRUE;
			}
		}
	}
	
	if(bBold)
	{
		while((length == -1 && *(pstr+s) != '\0') ||
			  (length != -1 && s < length))
		{
			ChatPt.x += xoffset;

			if( IsLeadByte( pstr[s] ) ) // DBCS
			{
				PrepareIMMode2D(ChatPt.x-1,ChatPt.y-1,pstr+s,m_mbcsLetters,ftype,alpha,false);
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);

				xoffset = GetCharacterWidth( ftype, pstr + s );
				++s;
			}
			else
			{
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,false);
				PrepareIMMode2D(ChatPt.x+1,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow);

				xoffset = m_iXLE[ftype][ (unsigned char)pstr[s] ];
			}
			
			++s;
		}
	}
	else
	{
		while((length == -1 && *(pstr+s) != '\0') ||
			  (length != -1 && s < length))
		{
			ChatPt.x += xoffset;
			if( IsLeadByte( pstr[s] ) ) // DBCS
			{
				PrepareIMMode2D(ChatPt.x-1,ChatPt.y-1,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);

				xoffset = GetCharacterWidth( ftype, pstr + s );
				++s;
			}
			else
			{
				PrepareIMMode2D(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow);
				xoffset = m_iXLE[ftype][ (unsigned char)pstr[s] ];
			}
			
			++s;
		}
	}
}

void	AgcmFont::DrawTextIM2DScale(float x,float y,float size,char* pstr,UINT32 ftype,UINT8 alpha,DWORD color, bool bDrawShadow, INT32 length)
{
	PROFILE("AgcmFont::DrawTextIM2DScale");

	if(m_pFTFaces[ftype] == NULL) return;

	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;
	
	m_iColor2 = (color & 0x00ffffff) | (alpha & 0xff) << 24;
	m_bClippingRoughTestResult = FALSE;

		while((length == -1 && *(pstr+s) != '\0') ||
			  (length != -1 && s < length))
	{
		ChatPt.x += xoffset;
		if( IsLeadByte( pstr[s] ) ) // DBCS
		{
			RenderIMMode2DSCALE(ChatPt.x,ChatPt.y,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow,size);

			xoffset = (int)(GetCharacterWidth( ftype, pstr + s )* size);
			++s;
		}
		else
		{
			RenderIMMode2DSCALE(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow,size);
			xoffset = (int)(m_iXLE[ftype][ (unsigned char)pstr[s] ] * size);
		}
		
		++s;
	}
}

RwRect	AgcmFont::DrawTextRectIM2D(RwRect *pRect,char* pstr,UINT32 ftype,UINT32 Flag,UINT8 alpha,DWORD color,bool	bDrawShadow, INT32 length)
{
	RwRect	ResRect;
	ZeroMemory(&ResRect,sizeof(RwRect));

	if(m_pFTFaces[ftype] == NULL) return	ResRect;

	int		s = 0;
	int		xoffset = 0;

	INT32	yStart;

	INT32	str_offset[20];
	int		str_startx[20];
	INT32	str_end_index = 1;	str_offset[0] = 0;
	int		minx = 5000;
	int		maxx = 0;

	m_iColor2 = (color & 0x00ffffff) | (alpha & 0xff) << 24;
	
	// line 수와 line시작 위치 구하기
	while((length == -1 && *(pstr+s) != '\0') ||
			(length != -1 && s < length))
	{
		if(xoffset > pRect->w)			// line변경
		{
			str_offset[str_end_index] = s;
			if(Flag & FF_LEFT)
			{
				str_startx[str_end_index-1] = pRect->x;
				if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
			}
			else if(Flag & FF_CENTER)
			{
				str_startx[str_end_index-1] = pRect->x + (pRect->w/2) - xoffset/2; 
				if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
			}
			else if(Flag && FF_RIGHT)
			{
				str_startx[str_end_index-1] = pRect->x + pRect->w - xoffset; 
				if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
			}

			++str_end_index;
			if(xoffset > maxx) maxx = xoffset;
			xoffset = 0;
		}

		if( IsLeadByte( pstr[s] ) ) // 한글이라면
		{
			xoffset += GetCharacterWidth( ftype, pstr + s );
			s += 2;
		}
		else
		{
			xoffset += m_iXLE[ftype][ (unsigned char)pstr[s] ];
			++s;
		}
	}

	str_offset[str_end_index] = s;
	if(Flag & FF_LEFT)
	{
		str_startx[str_end_index-1] = pRect->x;
		if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
	}
	else if(Flag & FF_CENTER)
	{
		str_startx[str_end_index-1] = pRect->x + (pRect->w/2) - xoffset/2; 
		if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
	}
	else if(Flag && FF_RIGHT)
	{
		str_startx[str_end_index-1] = pRect->x + pRect->w - xoffset; 
		if(str_startx[str_end_index-1] < minx) minx = str_startx[str_end_index-1];
	}
	++str_end_index;
	if(xoffset > maxx) maxx = xoffset;

	if(Flag & FF_VCENTER)
	{
		yStart = pRect->y + (pRect->h/2) - (str_end_index * m_iYL[ftype])/2;   
	}
	else
	{
		yStart = pRect->y;
	}

	ResRect.y = yStart;
	ResRect.h = str_end_index * m_iYL[ftype];
	ResRect.x = minx;
	ResRect.w = maxx;

	if(m_bUseClipping)
		m_bClippingRoughTestResult = TRUE;		// rough test 안한다..
	else
		m_bClippingRoughTestResult = FALSE;

	float	curx;
	for(INT32 i = 0; i < str_end_index;++i)
	{
		xoffset = 0;
		curx = (float)str_startx[i];
		for(INT32 j = str_offset[i]; j < str_offset[i+1] ; ++j)
		{
			curx += xoffset;
			if( IsLeadByte( pstr[j] ) ) // DBCS
			{
				PrepareIMMode2D(curx-1,(float)yStart-1,pstr+j,2,ftype,alpha,bDrawShadow);

				xoffset = GetCharacterWidth( ftype, pstr + j );
				++j;
			}
			else
			{
				PrepareIMMode2D(curx,(float)yStart,pstr+j,1,ftype,alpha,bDrawShadow);
				xoffset = m_iXLE[ftype][ (unsigned char)pstr[j] ];
			}
		}

		yStart += m_iYL[ftype];
	}

	return ResRect;
}

void	AgcmFont::DrawTextImgFont(FLOAT x,FLOAT y,char*	pstr,UINT32 imgtype,UINT8 red,UINT8 green,UINT8 blue,UINT8 alpha)
{
	PROFILE("AgcmFont::DrawTextImgFont");

	if(!m_pImgFontTexture[imgtype]) return;			// NULL 체크  2005.4.2
	
	float	recip_z = m_fRecipZ;
	float	screen_z = m_fScreenZ;
	int		s = 0;
	int		xoffset = 0;

	My2DVertex		vert[120];		// 현재 20개 문자 수용

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;
	int c;
	int index = 0;
	int cindex;

	int fw,fh ; 

	DWORD	color = ARGB32_TO_DWORD(alpha,red,green,blue);
	
	while(*(pstr+s) != '\0')
	{
		c = *(pstr+s);

		if( c >= 46 && c < 58 )
		{
			cindex = c-46;
		}
		else if( c == 126 )		// '~'
		{
			cindex = 12;
		}
		else 
		{
			++s;
			continue;
		}

		xoffset = m_iImgFontW[imgtype][cindex];
		fw = m_iImgFontTexW[imgtype];
		fh = m_iImgFontTexH[imgtype];

		for(int i=0;i<6;++i)
		{
			vert[index + i].z = 0.0f;
			vert[index + i].rhw = 1.0f;
			vert[index + i].color = color;
		}

		vert[index+0].u = m_fImgFontUV[imgtype][cindex][2];    
		vert[index+0].v = m_fImgFontUV[imgtype][cindex][3];
			
		vert[index+1].u = m_fImgFontUV[imgtype][cindex][2];    
		vert[index+1].v = m_fImgFontUV[imgtype][cindex][1];

		vert[index+2].u = m_fImgFontUV[imgtype][cindex][0];    
		vert[index+2].v = m_fImgFontUV[imgtype][cindex][1];

		vert[index+3].u = m_fImgFontUV[imgtype][cindex][0];    
		vert[index+3].v = m_fImgFontUV[imgtype][cindex][3];

		vert[index+4].u = m_fImgFontUV[imgtype][cindex][2];    
		vert[index+4].v = m_fImgFontUV[imgtype][cindex][3];

		vert[index+5].u = m_fImgFontUV[imgtype][cindex][0];    
		vert[index+5].v = m_fImgFontUV[imgtype][cindex][1];

		vert[index+0].x = ChatPt.x+fw;
		vert[index+0].y = ChatPt.y+fh; 
		
		vert[index+1].x = ChatPt.x+fw;
		vert[index+1].y = ChatPt.y; 

		vert[index+2].x = ChatPt.x;
		vert[index+2].y = ChatPt.y; 

		vert[index+3].x = ChatPt.x;
		vert[index+3].y = ChatPt.y+fh; 

		vert[index+4].x = ChatPt.x+fw;
		vert[index+4].y = ChatPt.y+fh; 

		vert[index+5].x = ChatPt.x;
		vert[index+5].y = ChatPt.y; 
				
		index += 6;
		ChatPt.x += xoffset;

		if(index == 120)
		{
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pImgFontTexture[imgtype]));

			if (index)
			{
				RwD3D9SetFVF(MY2D_VERTEX_FLAG);
				RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLELIST , index/3 , vert , SIZE_MY2D_VERTEX );
			}

			index = 0;
		}
		
		++s;
	}
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pImgFontTexture[imgtype]));

	if (index)
	{
		RwD3D9SetFVF(MY2D_VERTEX_FLAG);
		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLELIST , index/3 , vert , SIZE_MY2D_VERTEX );
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Font Im2D Draw Funtions
//	
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void	AgcmFont::FontDrawStart(INT32	font_type)
{
	m_iAsciiNextIndex1 = 0;
	m_iAsciiNextBufferIndex1 = 0;
	m_iAsciiNextIndex2 = 0;
	m_iCurFontType = font_type;
	m_iFontBlockStatus = 0;

	RwRenderStateGet(rwRENDERSTATETEXTUREFILTER , (void *)&m_iBeforeFilter );
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERNEAREST );
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS , (void *)rwTEXTUREADDRESSCLAMP );
}

void	AgcmFont::FontDrawEnd()
{
	//. 2006. 3. 14. Nonstopdj
	//. m_pEnglishTextTexture array index validation check.
	if( m_iCurFontType > AGCM_FONT_MAX_TYPE_NUM)
	{
		MD_SetErrorMessage("AgcmFont::FontDrawEnd() m_pEnglishTextTexture array index(%d:%d) overflow.", AGCM_FONT_MAX_TYPE_NUM, m_iCurFontType);
		return;
	}

	if(!m_pEnglishTextTexture[m_iCurFontType]) 
	{
		MD_SetErrorMessage("AgcmFont::FontDrawEnd() m_pEnglishTextTexture data invalid.");
		return;
	}

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(m_pEnglishTextTexture[m_iCurFontType]));
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS , (void *)rwTEXTUREADDRESSCLAMP );
	
	if(m_iAsciiNextIndex1)
	{
		RwD3D9SetFVF(MY2D_VERTEX_FLAG);
		RwD3D9DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0 , m_iAsciiNextIndex1 , m_iAsciiNextIndex1/3, 
								m_pIndexAscii1, m_vBufferAscii1, SIZE_MY2D_VERTEX);
	}

	if(m_iAsciiNextIndex2)
		RwIm2DRenderPrimitive(rwPRIMTYPETRILIST, m_vTriListAscii2, m_iAsciiNextIndex2); 

	m_iFontBlockStatus = 1;
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER , (void *)m_iBeforeFilter );
}

void	AgcmFont::SetClippingArea(AgcdFontClippingBox*	area)
{
	if(area == NULL)
	{
		m_bUseClipping = FALSE;
	}
	else
	{
		m_bUseClipping = TRUE;

		m_stClipArea.x = area->x;
		m_stClipArea.y = area->y;
		m_stClipArea.w = area->w;
		m_stClipArea.h = area->h;
	}
}

RwRaster*	AgcmFont::CreateFontRaster(int ftype, char*	pstr, int len,FLOAT&	offset)
{
	int Wsize,Hsize ;
	Wsize = Hsize = m_iTexSize[ftype];
	
	FT_GlyphSlot  slot = m_pFTFaces[ftype]->glyph;
	WCHAR	strtemp[2];
	//MultiByteToWideChar( CP_ACP, 0, pstr, -1, strtemp, len );
	MultiByteToWideChar( AuInputComposer::GetInstance().GetCodePage(), 0, pstr, 2, strtemp, len );

	int error = FT_Load_Char( m_pFTFaces[ftype], strtemp[0], FT_LOAD_RENDER | FT_LOAD_NO_BITMAP);
		
	RwRaster*	new_raster = MakeFontRasterFromFTBits(slot,Wsize,Hsize);

	offset = (FLOAT)(m_astFonts[ftype].lSize - slot->bitmap_top); // 찍을때 y좌표 더해줌!
		
	return new_raster;
}

void	AgcmFont::RenderFont(float x,float y,float sz,UINT16	ccode,UINT32	ftype,UINT8	alpha,bool	bShadow)
{
	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	
	if(bShadow)
	{
		DWORD	shadow_color;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_color = AGCM_FONT_SHAODW_COLOR;
		}
		else
		{
			shadow_color = (alpha & 0xff) << 24;
		}

		m_vTriFan3[0].color = shadow_color;
		m_vTriFan3[1].color = shadow_color;
		m_vTriFan3[2].color = shadow_color;
		m_vTriFan3[3].color = shadow_color;

		m_vTriFan3[0].x = x+sz+1;
		m_vTriFan3[0].y = y+sz; 
		
		m_vTriFan3[1].x = x+sz+1;
		m_vTriFan3[1].y = y; 
		
		m_vTriFan3[2].x = x+1;
		m_vTriFan3[2].y = y; 
		
		m_vTriFan3[3].x = x+1;
		m_vTriFan3[3].y = y+sz; 

		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );

		m_vTriFan3[0].x = x+sz;
		m_vTriFan3[0].y = y+sz+1; 
		
		m_vTriFan3[1].x = x+sz;
		m_vTriFan3[1].y = y+1; 
		
		m_vTriFan3[2].x = x;
		m_vTriFan3[2].y = y+1; 
		
		m_vTriFan3[3].x = x;
		m_vTriFan3[3].y = y+sz+1; 

		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
	}

	for(int k=0;k<4;++k)
	{
		m_vTriFan3[k].color = m_iColor2;
	}

	m_vTriFan3[0].x = x+sz;
	m_vTriFan3[0].y = y+sz; 
	
	m_vTriFan3[1].x = x+sz;
	m_vTriFan3[1].y = y; 
	
	m_vTriFan3[2].x = x;
	m_vTriFan3[2].y = y; 
	
	m_vTriFan3[3].x = x;
	m_vTriFan3[3].y = y+sz; 

	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan3 , SIZE_MY2D_VERTEX );
}

void	AgcmFont::RenderFont_CLIP(float x,float y,float sz,UINT16	ccode,UINT32	ftype,UINT8	alpha,bool	bShadow)
{
	//float sz = (float)m_iTexSize[ftype];

	FLOAT		clip_endx = m_stClipArea.x + m_stClipArea.w;
	FLOAT		clip_endy = m_stClipArea.y + m_stClipArea.h;

	if(x >= clip_endx || x+sz <= m_stClipArea.x ||
		y >= clip_endy || y+sz <= m_stClipArea.y)	return;		// 완전 clip

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	My2DVertex		vert[4];

	for(int i=0;i<4;++i)
	{
		vert[i].z = 0.0f;
		vert[i].rhw = 1.0f;
	}

	FLOAT		x1,x2,y1,y2;
	FLOAT		u1,u2,v1,v2;
	FLOAT		temp;
	FLOAT		texel_offset = -(0.1f / sz);
	
	if(bShadow)
	{
		DWORD	shadow_color;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_color = AGCM_FONT_SHAODW_COLOR;
		}
		else
		{
			shadow_color = (alpha & 0xff) << 24;
		}

		vert[0].color = shadow_color;
		vert[1].color = shadow_color;
		vert[2].color = shadow_color;
		vert[3].color = shadow_color;

		x1 = x+1;
		x2 = x+sz+1;
		y1 = y;
		y2 = y+sz;

		if(x1 < m_stClipArea.x)
		{
			temp = m_stClipArea.x - x1; 
			u1 = temp/sz - texel_offset;
			x1 = m_stClipArea.x;
		}
		else 
			u1 = -texel_offset;
				
		if(x2 > clip_endx)
		{
			temp = clip_endx - x1;
			u2 = temp/sz - texel_offset;
			x2 = clip_endx;
		}
		else 
			u2 = 1.0f - texel_offset;

		if(y1 < m_stClipArea.y)
		{
			temp = m_stClipArea.y - y1;
			v1 = temp/sz - texel_offset;
			y1 = m_stClipArea.y;
		}
		else 
			v1 = -texel_offset;
				
		if(y2 > clip_endy)
		{
			temp = clip_endy - y1;
			v2 = temp/sz - texel_offset;
			y2 = clip_endy;
		}
		else 
			v2 = 1.0f - texel_offset;

		vert[0].x = x2;		vert[0].y = y2; 
		vert[1].x = x2;		vert[1].y = y1; 
		vert[2].x = x1;		vert[2].y = y1; 
		vert[3].x = x1;		vert[3].y = y2; 

		vert[0].u = u2;		vert[0].v = v2;
		vert[1].u = u2;		vert[1].v = v1;
		vert[2].u = u1;		vert[2].v = v1;
		vert[3].u = u1;		vert[3].v = v2;

		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , vert , SIZE_MY2D_VERTEX );

		x1 = x;
		x2 = x+sz;
		y1 = y+1;
		y2 = y+sz+1;

		if(x1 < m_stClipArea.x)
		{
			temp = m_stClipArea.x - x1; 
			u1 = temp/sz - texel_offset;
			x1 = m_stClipArea.x;
		}
		else 
			u1 = -texel_offset;
				
		if(x2 > clip_endx)
		{
			temp = clip_endx - x1;
			u2 = temp/sz - texel_offset;
			x2 = clip_endx;
		}
		else 
			u2 = 1.0f - texel_offset;

		if(y1 < m_stClipArea.y)
		{
			temp = m_stClipArea.y - y1;
			v1 = temp/sz - texel_offset;
			y1 = m_stClipArea.y;
		}
		else 
			v1 = -texel_offset;
				
		if(y2 > clip_endy)
		{
			temp = clip_endy - y1;
			v2 = temp/sz - texel_offset;
			y2 = clip_endy;
		}
		else 
			v2 = 1.0f - texel_offset;

		vert[0].x = x2;		vert[0].y = y2; 
		vert[1].x = x2;		vert[1].y = y1; 
		vert[2].x = x1;		vert[2].y = y1; 
		vert[3].x = x1;		vert[3].y = y2; 

		vert[0].u = u2;		vert[0].v = v2;
		vert[1].u = u2;		vert[1].v = v1;
		vert[2].u = u1;		vert[2].v = v1;
		vert[3].u = u1;		vert[3].v = v2;

		RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , vert , SIZE_MY2D_VERTEX );
	}

	for(int k=0;k<4;++k)
	{
		vert[k].color = m_iColor2;
	}

	x1 = x;
	x2 = x+sz;
	y1 = y;
	y2 = y+sz;

	if(x1 < m_stClipArea.x)
	{
		temp = m_stClipArea.x - x1; 
		u1 = temp /sz - texel_offset;
		x1 = m_stClipArea.x ;
	}
	else 
		u1 = -texel_offset ;
			
	if(x2 > clip_endx)
	{
		temp = clip_endx - x1;
		u2 = temp/sz - texel_offset;
		x2 = clip_endx;
	}
	else 
		u2 = 1.0f - texel_offset;

	if(y1 < m_stClipArea.y)
	{
		temp = m_stClipArea.y - y1;
		v1 = temp /sz - texel_offset;
		y1 = m_stClipArea.y ;
	}
	else 
		v1 = -texel_offset;
			
	if(y2 > clip_endy)
	{
		temp = clip_endy - y1;
		v2 = temp/sz - texel_offset;
		y2 = clip_endy;
	}
	else 
		v2 = 1.0f - texel_offset;

	vert[0].x = x2;	vert[0].y = y2; 
	vert[1].x = x2;	vert[1].y = y1; 
	vert[2].x = x1;	vert[2].y = y1; 
	vert[3].x = x1;	vert[3].y = y2; 

	vert[0].u = u2;	vert[0].v = v2;
	vert[1].u = u2;	vert[1].v = v1;
	vert[2].u = u1;	vert[2].v = v1;
	vert[3].u = u1;	vert[3].v = v2;

	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , vert , SIZE_MY2D_VERTEX );
}


void	AgcmFont::RenderAsciiFont(float x,float y,float fw,float fh,UINT16	ccode,UINT32	ftype,UINT8	alpha,bool	bShadow)
{
	int	cindex = ccode-32;

	if(bShadow)
	{
		DWORD	shadow_color;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_color = AGCM_FONT_SHAODW_COLOR;
		}
		else
		{
			shadow_color = (alpha & 0xff) << 24;
		}
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw+1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw+1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];

		// 아래쪽
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh+1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh+1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];
				
		m_iAsciiNextIndex1 += 12;
	}

	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x+fw;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][2];    
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][1];
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y+fh; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = m_fTexCoords[ftype][cindex][0];    
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = m_fTexCoords[ftype][cindex][3];
	
	m_iAsciiNextIndex1 += 6;
}

void	AgcmFont::RenderAsciiFont_CLIP(float x,float y,float fw,float fh,UINT16	ccode,UINT32	ftype,UINT8	alpha,bool	bShadow)
{
	FLOAT		clip_endx = m_stClipArea.x + m_stClipArea.w;
	FLOAT		clip_endy = m_stClipArea.y + m_stClipArea.h;

	if(x >= clip_endx || x+fw <= m_stClipArea.x ||
		y >= clip_endy || y+fh <= m_stClipArea.y)	return;		// 완전 clip

	int			cindex = ccode-32;

	FLOAT		x1,x2,y1,y2;
	FLOAT		u1,u2,v1,v2;
	FLOAT		temp;
	FLOAT		texel_offset = - (0.1f / m_iShareSize[ftype]);
		
	if(bShadow)
	{
		DWORD	shadow_color;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_color = AGCM_FONT_SHAODW_COLOR;
		}
		else
		{
			shadow_color = (alpha & 0xff) << 24;
		}

		x1 = x+1;
		x2 = x+fw+1;
		y1 = y;
		y2 = y+fh;

		if(x1 < m_stClipArea.x)
		{
			temp = m_stClipArea.x - x1;
			u1 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
			x1 = m_stClipArea.x;
		}
		else 
			u1 = m_fTexCoords[ftype][cindex][0] - texel_offset;
				
		if(x2 > clip_endx)
		{
			temp = clip_endx - x1;
			x2 = clip_endx;
			u2 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
		}
		else 
			u2 = m_fTexCoords[ftype][cindex][2] - texel_offset;

		if(y1 < m_stClipArea.y)
		{
			temp = m_stClipArea.y - y1;
			v1 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
			y1 = m_stClipArea.y;
		}
		else 
			v1 = m_fTexCoords[ftype][cindex][1] - texel_offset;
				
		if(y2 > clip_endy)
		{
			temp = clip_endy - y1;
			y2 = clip_endy;
			v2 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
		}
		else 
			v2 = m_fTexCoords[ftype][cindex][3] - texel_offset;

		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;

		x1 = x;
		x2 = x+fw;
		y1 = y+1;
		y2 = y+fh+1;

		if(x1 < m_stClipArea.x)
		{
			temp = m_stClipArea.x - x1;
			u1 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
			x1 = m_stClipArea.x;
		}
		else 
			u1 = m_fTexCoords[ftype][cindex][0] - texel_offset;
				
		if(x2 > clip_endx)
		{
			temp = clip_endx - x1;
			x2 = clip_endx;
			u2 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
		}
		else 
			u2 = m_fTexCoords[ftype][cindex][2] - texel_offset;

		if(y1 < m_stClipArea.y)
		{
			temp = m_stClipArea.y - y1;
			v1 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
			y1 = m_stClipArea.y;
		}
		else 
			v1 = m_fTexCoords[ftype][cindex][1] - texel_offset;
				
		if(y2 > clip_endy)
		{
			temp = clip_endy - y1;
			y2 = clip_endy;
			v2 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
		}
		else 
			v2 = m_fTexCoords[ftype][cindex][3] - texel_offset;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
		
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2; 
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = shadow_color;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
		m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;

		m_iAsciiNextIndex1 += 12;
	}

	x1 = x;
	x2 = x+fw;
	y1 = y;
	y2 = y+fh;

	if(x1 < m_stClipArea.x)
	{
		temp = m_stClipArea.x - x1;
		u1 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
		x1 = m_stClipArea.x;
	}
	else 
		u1 = m_fTexCoords[ftype][cindex][0] - texel_offset;
			
	if(x2 > clip_endx)
	{
		temp = clip_endx - x1;
		x2 = clip_endx;
		u2 = m_fTexCoords[ftype][cindex][0] + temp/m_iShareSize[ftype] - texel_offset;
	}
	else 
		u2 = m_fTexCoords[ftype][cindex][2] - texel_offset;

	if(y1 < m_stClipArea.y)
	{
		temp = m_stClipArea.y - y1;
		v1 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
		y1 = m_stClipArea.y;
	}
	else 
		v1 = m_fTexCoords[ftype][cindex][1] - texel_offset;
			
	if(y2 > clip_endy)
	{
		temp = clip_endy - y1;
		y2 = clip_endy;
		v2 = m_fTexCoords[ftype][cindex][1] + temp/m_iShareSize[ftype] - texel_offset;
	}
	else 
		v2 = m_fTexCoords[ftype][cindex][3] - texel_offset;

	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y1; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v1;
	
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].x = x1;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].y = y2; 
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].color = m_iColor2;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1].u = u1;
	m_vBufferAscii1[m_iAsciiNextBufferIndex1++].v = v2;
	
	m_iAsciiNextIndex1 += 6;
}

void	AgcmFont::PrepareIMMode2D(float x,float y,char* pstr,INT32 len,UINT32 ftype,UINT8 alpha,bool bShadow)
{
	ASSERT(ftype >= 0 || ftype < AGCM_FONT_MAX_TYPE_NUM);

	UINT16	ccode = (len == 2)? (*(pstr+1) << 8) + *(pstr) : *(pstr);

	if(ccode >= 32 && ccode < 127 && m_bUseShare[ftype]) // 공유 텍스쳐 사용
	{
		if(m_iFontBlockStatus != 0)
		{
			ASSERT(!"FontDrawStart 와 FontDrawEnd block안에 font그려주는 부분들이 모여 있어야 됩니다^^");
			return;
		}

		// 버퍼 플러쉬(그림자 있을 경우 가정)
		if(m_iAsciiNextIndex1 + 18 >= AGCM_FONT_ASCII_VERTS1_INDEX)
		{
			FontDrawEnd();
			FontDrawStart(m_iCurFontType);
		}
		
		if(m_bClippingRoughTestResult) 
			RenderAsciiFont_CLIP(x,y,(float)m_iTexXLE[ftype][ccode],(float)m_iTexYL[ftype],
									ccode,ftype,alpha,bShadow);
		else
			RenderAsciiFont(x,y,(float)m_iTexXLE[ftype][ccode],(float)m_iTexYL[ftype],
							ccode,ftype,alpha,bShadow);
	}
	else
	{
		INT32	hash_key = GetHashKey(ccode);

		TextRaster*		find_node = m_listTextRaster.hash[hash_key];
		RwRaster*		render_raster;
		BOOL			bFind = FALSE;
		FLOAT			offset = 0.0f;
		
		// 재검색 피하기 위해 삽입 위치까지 결정
		TextRaster*		before_node = m_listTextRaster.hash[hash_key],*next_node = NULL;
			
		while(find_node)
		{
			if(find_node->ccode == ccode && find_node->ftype == (UINT16)ftype)
			{
				bFind = TRUE;
				render_raster = find_node->Ras;
				find_node->count = 50;
				offset = find_node->offset;
				break;
			}
			else if(find_node->ccode == ccode && find_node->ftype > (UINT16)ftype)
			{
				next_node = find_node;
				break;
			}
			before_node = find_node;
			find_node = find_node->next;
		}

		if(!bFind)
		{
			TextRaster*		pTR = new TextRaster;

			pTR->ccode = ccode;
			pTR->count = 50;
			pTR->ftype = ftype;
			pTR->color = AGCM_FONT_WHITE_COLOR;

			render_raster = CreateFontRaster(ftype,pstr,len,pTR->offset);
			pTR->Ras = render_raster;
			offset = pTR->offset;

			pTR->next = next_node;
			if(before_node == m_listTextRaster.hash[hash_key])
			{
				pTR->next = m_listTextRaster.hash[hash_key];
				m_listTextRaster.hash[hash_key] = pTR;
			}
			else
			{
				pTR->next = next_node;
				before_node->next = pTR;
			}
		}

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
				(void *)render_raster);

		if(m_bClippingRoughTestResult) 
			RenderFont_CLIP(x,y+offset,(float)m_iTexSize[ftype],ccode,ftype,alpha,bShadow);
		else
			RenderFont(x,y+offset,(float)m_iTexSize[ftype],ccode,ftype,alpha,bShadow);
   	}
}

void	AgcmFont::RenderIMMode2DSCALE(float x,float y,char* pstr,INT32 len,UINT32 ftype,UINT8 alpha,bool bShadow,float size)
{
	ASSERT(ftype >= 0 || ftype < AGCM_FONT_MAX_TYPE_NUM);

	UINT16	ccode = (len == 2)? (*(pstr+1) << 8) + *(pstr) : *(pstr);

	if(ccode >= 32 && ccode < 127 && m_bUseShare[ftype]) // 공유 텍스쳐 사용
	{
		if(m_iFontBlockStatus != 0)
		{
			ASSERT(!"FontDrawStart 와 FontDrawEnd block안에 font그려주는 부분들이 모여 있어야 됩니다^^");
			return;
		}

		// 버퍼 플러쉬(그림자 있을 경우 가정)
		if(m_iAsciiNextIndex1 + 18 >= AGCM_FONT_ASCII_VERTS1_INDEX)
		{
			FontDrawEnd();
			FontDrawStart(m_iCurFontType);
		}

		float  fw = ((float)m_iTexXLE[ftype][ccode] * size);
		float  fh = (((float)m_iTexYL[ftype])* size); 

		if(m_bClippingRoughTestResult) 
			RenderAsciiFont_CLIP(x,y,fw,fh,ccode,ftype,alpha,bShadow);
		else
			RenderAsciiFont(x,y,fw,fh,ccode,ftype,alpha,bShadow);
   	}
	else
	{
		INT32	hash_key = GetHashKey(ccode);

		TextRaster*		find_node = m_listTextRaster.hash[hash_key];
		RwRaster*		render_raster;
		BOOL			bFind = FALSE;
		FLOAT			offset = 0.0f;
		
		// 재검색 피하기 위해 삽입 위치까지 결정
		TextRaster*		before_node = m_listTextRaster.hash[hash_key],*next_node = NULL;
			
		while(find_node)
		{
			if(find_node->ccode == ccode && find_node->ftype == (UINT16)ftype)
			{
				bFind = TRUE;
				render_raster = find_node->Ras;
				find_node->count = 50;
				offset = find_node->offset;
				break;
			}
			else if(find_node->ccode == ccode && find_node->ftype > (UINT16)ftype)
			{
				next_node = find_node;
				break;
			}
			before_node = find_node;
			find_node = find_node->next;
		}

		if(!bFind)
		{
			// 없으면 만든다
			TextRaster*		pTR = new TextRaster;
			pTR->ccode = ccode;
			pTR->count = 50;
			pTR->ftype = ftype;
			pTR->color = AGCM_FONT_WHITE_COLOR;
			
			render_raster = CreateFontRaster(ftype,pstr,len,pTR->offset);
			pTR->Ras = render_raster;
			offset = pTR->offset;

			pTR->next = next_node;
			if(before_node == m_listTextRaster.hash[hash_key])
			{
				pTR->next = m_listTextRaster.hash[hash_key];
				m_listTextRaster.hash[hash_key] = pTR;
			}
			else
			{
				pTR->next = next_node;
				before_node->next = pTR;
			}
		}

		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
				(void *)render_raster);
	   
		float sz = (float)m_iTexSize[ftype] * size;
		float ty = y - 1.0f*size + offset*size;

		if(m_bClippingRoughTestResult) 
			RenderFont_CLIP(x,ty,sz,ccode,ftype,alpha,bShadow);
		else
			RenderFont(x,ty,sz,ccode,ftype,alpha,bShadow);
	}
}

void	AgcmFont::RenderIMMode2DWORLD(float x,float y,float screen_z,float recip_z,char* pstr,INT32 len,UINT32 ftype,UINT8 alpha,bool bShadow,float size)
{
	ASSERT(ftype >= 0 || ftype < AGCM_FONT_MAX_TYPE_NUM);

	UINT16	ccode = (len == 2)? (*(pstr+1) << 8) + *(pstr) : *(pstr);

	if(ccode >= 32 && ccode < 127 && m_bUseShare[ftype]) // 공유 텍스쳐 사용
	{
		if(m_iFontBlockStatus != 0)
		{
			ASSERT(!"FontDrawStart 와 FontDrawEnd block안에 font그려주는 부분들이 모여 있어야 됩니다^^");
			return;
		}

		if(m_iAsciiNextIndex2 +18 >= AGCM_FONT_ASCII_VERTS2)
		{
			FontDrawEnd();
			FontDrawStart(m_iCurFontType);
		}

		int	index;
		int fw = m_iTexXLE[ftype][ccode] ;
		int fh = m_iTexYL[ftype]; //+ 1 ; 
		int i;

		float ty = y;// - 1.0f;
		int			cindex = ccode-32;
		FLOAT		texel_offset = - (0.1f / m_iShareSize[ftype]);
		FLOAT		u1 = m_fTexCoords[ftype][cindex][0] - texel_offset;
		FLOAT		u2 = m_fTexCoords[ftype][cindex][2] - texel_offset;
		FLOAT		v1 = m_fTexCoords[ftype][cindex][1] - texel_offset;
		FLOAT		v2 = m_fTexCoords[ftype][cindex][3] - texel_offset;

		if(bShadow)
		{
			BYTE	shadow_alpha;
			if(alpha > AGCM_FONT_SHADOW_ALPHA)
			{
				shadow_alpha = AGCM_FONT_SHADOW_ALPHA;
			}
			else
			{
				shadow_alpha = alpha;
			}
			
			index = m_iAsciiNextIndex2;
			for(i=index;i<index+6;++i)
			{
				RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
				RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
			}

			RwIm2DVertexSetU(&m_vTriListAscii2[index+0], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+0], v2, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+1], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+1], v1, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+2], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+2], v1, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+3], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+3], v2, recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+4], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+4], v2, recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+5], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+5], v1, recip_z);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2], 0, 0, 0, shadow_alpha);
			
			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5], 0, 0, 0, shadow_alpha);
			
			m_iAsciiNextIndex2 += 6;

			index = m_iAsciiNextIndex2;
			for(i=index;i<index+6;++i)
			{
				RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
				RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
			}

			RwIm2DVertexSetU(&m_vTriListAscii2[index+0], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+0], v2, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+1], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+1], v1, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+2], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+2], v1, recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+3], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+3], v2, recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+4], u2, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+4], v2, recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+5], u1, recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+5], v1, recip_z);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2],  0, 0, 0, shadow_alpha);
			
			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5], 0, 0, 0, shadow_alpha);
			
			m_iAsciiNextIndex2 += 6;
		}

		index = m_iAsciiNextIndex2;
		for(i=index;i<index+6;++i)
		{
			RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
			RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
		}

		RwIm2DVertexSetU(&m_vTriListAscii2[index+0], u2, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+0], v2, recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+1], u2, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+1], v1, recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+2], u1, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+2], v1, recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+3], u1, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+3], v2, recip_z);

		RwIm2DVertexSetU(&m_vTriListAscii2[index+4], u2, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+4], v2, recip_z);

		RwIm2DVertexSetU(&m_vTriListAscii2[index+5], u1, recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+5], v1, recip_z);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
		
		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5],m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
		
		m_iAsciiNextIndex2 += 6;
	}
	else
	{
	INT32	hash_key = GetHashKey(ccode);

	TextRaster*		find_node = m_listTextRaster.hash[hash_key];
	RwRaster*		render_raster;
	BOOL			bFind = FALSE;
	FLOAT			offset = 0.0f;
	
	// 재검색 피하기 위해 삽입 위치까지 결정
	TextRaster*		before_node = m_listTextRaster.hash[hash_key],*next_node = NULL;
		
	while(find_node)
	{
		if(find_node->ccode == ccode && find_node->ftype == (UINT16)ftype)
		{
			bFind = TRUE;
			render_raster = find_node->Ras;
			find_node->count = 50;
			offset = find_node->offset;
			break;
		}
		else if(find_node->ccode == ccode && find_node->ftype > (UINT16)ftype)
		{
			next_node = find_node;
			break;
		}
		before_node = find_node;
		find_node = find_node->next;
	}

	if(!bFind)
	{
		// 없으면 만든다
		TextRaster*		pTR = new TextRaster;
		pTR->ccode = ccode;
		pTR->count = 50;
		pTR->ftype = ftype;
		pTR->color = AGCM_FONT_WHITE_COLOR;

		render_raster = CreateFontRaster(ftype,pstr,len,pTR->offset);
		pTR->Ras = render_raster;
		offset = pTR->offset;

		pTR->next = next_node;
		if(before_node == m_listTextRaster.hash[hash_key])
		{
			pTR->next = m_listTextRaster.hash[hash_key];
			m_listTextRaster.hash[hash_key] = pTR;
		}
		else
		{
			pTR->next = next_node;
			before_node->next = pTR;
		}
	}
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
            (void *)render_raster);

	y += offset;
   
	float sz = (float)m_iTexSize[ftype] * size;
	if(bShadow)
	{
		BYTE	shadow_alpha;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_alpha = AGCM_FONT_SHADOW_ALPHA;
		}
		else
		{
			shadow_alpha = alpha;
		}

		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[0], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[1], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[2], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[3], 0, 0, 0, shadow_alpha);

		RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y); 
		 
		RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y); 
				
		RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz); 
		
		RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 

		RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz+1); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y+1); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y+1); 
				
		RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz+1); 
		
		RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 
	}
	
	RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[0], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[1], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[2], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
	
	RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[3], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 
	}
}

void	AgcmFont::OnCameraStateChange(CAMERASTATECHANGETYPE	ctype)
{
	CameraStatusChange();
}

void	AgcmFont::SetWorldCamera(RwCamera* pCam)
{
	m_pCamera = pCam; 

	CameraStatusChange();
}

INT32	AgcmFont::GetHashKey(WORD ccode)
{
	if(ccode == 0)
		return	0;
	else
		return	ccode % AGCM_FONT_HASH_SIZE;
}

// Parn님 작업
FontType	*AgcmFont::GetSequenceFont(INT32 *plIndex)
{
	for (; *plIndex < AGCM_FONT_MAX_TYPE_NUM; ++*plIndex)
	{
		if (m_pFTFaces[*plIndex])
		{
			++*plIndex;
			return (m_astFonts + *plIndex - 1);
		}
	}

	return NULL;
}

INT32		AgcmFont::GetSequenceImageFont(INT32 *plIndex)
{
	for (; *plIndex < AGCM_IMGFONT_MAX_TYPE_NUM; ++*plIndex)
	{
		if (m_pImgFontTexture[*plIndex])
		{
			++*plIndex;
			return *plIndex - 1;
		}
	}

	return -1;
}

void	AgcmFont::CameraStatusChange()
{
	RwCamera * pCamera = GetCamera();
	if (!m_pCamera || !pCamera || m_pCamera != pCamera )
		return;

	RwRaster*	pRas = RwCameraGetRaster(m_pCamera);

	if(!pRas)
	{
		MD_SetErrorMessage("AgcmFont::CameraStatusChange() m_pCamera Get Raster NULL.");
		MessageBox(NULL, "Please Check your Graphic Driver and DirectX version.", "Notify", MB_OK);

		RsEventHandler(rsTERMINATE, NULL);
		return;
	}

	m_iCameraRasterWidth = RwRasterGetWidth(pRas);
	m_iCameraRasterHeight = RwRasterGetHeight(pRas);
	
	m_fRecipZ = 1.0f / RwCameraGetNearClipPlane(m_pCamera);
	m_fScreenZ = RwIm2DGetNearScreenZ();
}

void	AgcmFont::DrawTextMapTool(float x,float y,char* pstr,UINT32 ftype,UINT8 alpha,DWORD color, bool bDrawShadow)
{
	int		s = 0;
	int		xoffset = 0;

	RwV2d	ChatPt;		
	ChatPt.x = x;
	ChatPt.y = y;

	memcpy(&m_stColor , &color, 4);
	
	while(*(pstr+s) != '\0')
	{
		ChatPt.x += xoffset;
		if( IsLeadByte( pstr[s] ) ) // DBCS
		{
			RenderMapTool(ChatPt.x,ChatPt.y,pstr+s,m_mbcsLetters,ftype,alpha,bDrawShadow);

			xoffset = GetCharacterWidth( ftype, pstr + s );
			s += 2;
		}
		else
		{
			RenderMapTool(ChatPt.x,ChatPt.y,pstr+s,1,ftype,alpha,bDrawShadow);
			xoffset = m_iXLE[ftype][ (unsigned char)pstr[s] ];
			++s;
		}
	}
}

void	AgcmFont::RenderMapTool(float x,float y,char* pstr,INT32 len,UINT32 ftype,UINT8 alpha,bool bShadow)
{
	RwReal	screen_z = RwIm2DGetNearScreenZ();
	RwReal  recip_z = 1.0f / RwCameraGetNearClipPlane(m_pCamera);

	ASSERT(ftype >= 0 || ftype < AGCM_FONT_MAX_TYPE_NUM);

	UINT16	ccode = (len == 2)? (*(pstr+1) << 8) + *(pstr) : *(pstr);

	if(ccode >= 32 && ccode < 127 && m_bUseShare[ftype]) // 공유 텍스쳐 사용
	{
		if(m_iFontBlockStatus != 0)
		{
			ASSERT(!"FontDrawStart 와 FontDrawEnd block안에 font그려주는 부분들이 모여 있어야 됩니다^^");
			return;
		}

		if(m_iAsciiNextIndex2 +18 >= AGCM_FONT_ASCII_VERTS2)
		{
			FontDrawEnd();
			FontDrawStart(m_iCurFontType);
		}

		int	index;
		int fw = m_iTexXLE[ftype][ccode] ;
		int fh = m_iTexYL[ftype] ;//+ 1 ; 
  
		float	ty = y;// - 1.0f;
		int		cindex = ccode-32;

		if(bShadow)
		{
			BYTE	shadow_alpha;
			if(alpha > AGCM_FONT_SHADOW_ALPHA)
			{
				shadow_alpha = AGCM_FONT_SHADOW_ALPHA;
			}
			else
			{
				shadow_alpha = alpha;
			}
			
			INT32	i;

			index = m_iAsciiNextIndex2;
			for(i=index;i<index+6;++i)
			{
				RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
				RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
			}

			RwIm2DVertexSetU(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][3],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][1],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][1],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][3],recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][3],recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][1],recip_z);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2], 0, 0, 0, shadow_alpha);
			
			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4], 0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x+1);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5], 0, 0, 0, shadow_alpha);
			
			m_iAsciiNextIndex2 += 6;

			index = m_iAsciiNextIndex2;
			for(i=index;i<index+6;++i)
			{
				RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
				RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
			}

			RwIm2DVertexSetU(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][3],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][1],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][1],recip_z);
			
			RwIm2DVertexSetU(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][3],recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][2],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][3],recip_z);

			RwIm2DVertexSetU(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][0],recip_z);    
			RwIm2DVertexSetV(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][1],recip_z);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2],  0, 0, 0, shadow_alpha);
			
			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4],  0, 0, 0, shadow_alpha);

			RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x);
			RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty+1); 
			RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5], 0, 0, 0, shadow_alpha);
			
			m_iAsciiNextIndex2 += 6;
		}

		index = m_iAsciiNextIndex2;
		for(int i=index;i<index+6;++i)
		{
			RwIm2DVertexSetScreenZ(&m_vTriListAscii2[i],screen_z);
			RwIm2DVertexSetRecipCameraZ(&m_vTriListAscii2[i], recip_z);
		}

		RwIm2DVertexSetU(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][2],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+0], m_fTexCoords[ftype][cindex][3],recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][2],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+1], m_fTexCoords[ftype][cindex][1],recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][0],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+2], m_fTexCoords[ftype][cindex][1],recip_z);
		
		RwIm2DVertexSetU(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][0],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+3], m_fTexCoords[ftype][cindex][3],recip_z);

		RwIm2DVertexSetU(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][2],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+4], m_fTexCoords[ftype][cindex][3],recip_z);

		RwIm2DVertexSetU(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][0],recip_z);    
		RwIm2DVertexSetV(&m_vTriListAscii2[index+5], m_fTexCoords[ftype][cindex][1],recip_z);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+0],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+0],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+0], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+1],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+1],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+1], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+2],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+2],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+2], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
		
		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+3],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+3],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+3], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+4],x+fw);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+4],ty+fh); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+4], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

		RwIm2DVertexSetScreenX(&m_vTriListAscii2[index+5],x);
		RwIm2DVertexSetScreenY(&m_vTriListAscii2[index+5],ty); 
		RwIm2DVertexSetIntRGBA(&m_vTriListAscii2[index+5],m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
		
		m_iAsciiNextIndex2 += 6;
	}
	else
	{
	INT32	hash_key = GetHashKey(ccode);

	TextRaster*		find_node = m_listTextRaster.hash[hash_key];
	RwRaster*		render_raster;
	BOOL			bFind = FALSE;
	FLOAT			offset = 0.0f;
	
	// 재검색 피하기 위해 삽입 위치까지 결정
	TextRaster*		before_node = m_listTextRaster.hash[hash_key],*next_node = NULL;
		
	while(find_node)
	{
		if(find_node->ccode == ccode && find_node->ftype == (UINT16)ftype)// && find_node->color == color)
		{
			bFind = TRUE;
			render_raster = find_node->Ras;
			find_node->count = 50;
			offset = find_node->offset;
			break;
		}
		else if(find_node->ccode == ccode && find_node->ftype > (UINT16)ftype)
		{
			next_node = find_node;
			break;
		}
		before_node = find_node;
		find_node = find_node->next;
	}

	if(!bFind)
	{
		// 없으면 만든다
		TextRaster*		pTR = new TextRaster;
		pTR->ccode = ccode;
		pTR->count = 50;
		pTR->ftype = ftype;
		pTR->color = AGCM_FONT_WHITE_COLOR;

		render_raster = CreateFontRaster(ftype,pstr,len,pTR->offset);
		pTR->Ras = render_raster;
		offset = pTR->offset;

		pTR->next = next_node;
		if(before_node == m_listTextRaster.hash[hash_key])
		{
			pTR->next = m_listTextRaster.hash[hash_key];
			m_listTextRaster.hash[hash_key] = pTR;
		}
		else
		{
			pTR->next = next_node;
			before_node->next = pTR;
		}
	}

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
            (void *)render_raster);

	for(int i=0;i<4;++i)
	{
		RwIm2DVertexSetScreenZ(&m_vTriFan2World[i],screen_z);
		RwIm2DVertexSetRecipCameraZ(&m_vTriFan2World[i], recip_z);

		RwIm2DVertexSetU(&m_vTriFan2World[0], 1.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[0], 1.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[1], 1.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[1], 0.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[2], 0.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[2], 0.0f,recip_z);
		
		RwIm2DVertexSetU(&m_vTriFan2World[3], 0.0f,recip_z);    
		RwIm2DVertexSetV(&m_vTriFan2World[3], 1.0f,recip_z);
	}

	y += offset;
   
	float sz = (float)m_iTexSize[ftype];
	if(bShadow)
	{
		BYTE	shadow_alpha;
		if(alpha > AGCM_FONT_SHADOW_ALPHA)
		{
			shadow_alpha = AGCM_FONT_SHADOW_ALPHA;
		}
		else
		{
			shadow_alpha = alpha;
		}

		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[0], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[1], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[2], 0, 0, 0, shadow_alpha);
		RwIm2DVertexSetIntRGBA(&m_vTriFan2World[3], 0, 0, 0, shadow_alpha);

		RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y); 
				
		RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x+1);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz); 
		
		RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 

		RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz+1); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y+1); 
		
		RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y+1); 
				
		RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x);
		RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz+1); 
		
		RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 
	}
	
	RwIm2DVertexSetScreenX(&m_vTriFan2World[0],x+sz);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[0],y+sz); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[0], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DVertexSetScreenX(&m_vTriFan2World[1],x+sz);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[1],y); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[1], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DVertexSetScreenX(&m_vTriFan2World[2],x);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[2],y); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[2], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);
	
	RwIm2DVertexSetScreenX(&m_vTriFan2World[3],x);
    RwIm2DVertexSetScreenY(&m_vTriFan2World[3],y+sz); 
	RwIm2DVertexSetIntRGBA(&m_vTriFan2World[3], m_stColor.red, m_stColor.green, m_stColor.blue, alpha);

	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, m_vTriFan2World, 4); 
	}
}

INT32 AgcmFont::GetMaxTexSize( int max )
{
	INT32	texsize = 1;
	while( texsize < max )
		texsize <<= 1;

	return texsize;
}

void AgcmFont::SetJapaneseTable()
{
	m_sjis2uniTable.resize( 0x8396 - 0x8140 + 1 );

	m_sjis2uniTable[0x8140 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8141 - 0x8140] = 0x3001;
	m_sjis2uniTable[0x8142 - 0x8140] = 0x3002;
	m_sjis2uniTable[0x8143 - 0x8140] = 0xFF0C;
	m_sjis2uniTable[0x8144 - 0x8140] = 0xFF0E;
	m_sjis2uniTable[0x8145 - 0x8140] = 0x30FB;
	m_sjis2uniTable[0x8146 - 0x8140] = 0xFF1A;
	m_sjis2uniTable[0x8147 - 0x8140] = 0xFF1B;
	m_sjis2uniTable[0x8148 - 0x8140] = 0xFF1F;
	m_sjis2uniTable[0x8149 - 0x8140] = 0xFF01;
	m_sjis2uniTable[0x814A - 0x8140] = 0x309B;
	m_sjis2uniTable[0x814B - 0x8140] = 0x309C;
	m_sjis2uniTable[0x814C - 0x8140] = 0x00B4;
	m_sjis2uniTable[0x814D - 0x8140] = 0xFF40;
	m_sjis2uniTable[0x814E - 0x8140] = 0x00A8;
	m_sjis2uniTable[0x814F - 0x8140] = 0xFF3E;
	m_sjis2uniTable[0x8150 - 0x8140] = 0xFFE3;
	m_sjis2uniTable[0x8151 - 0x8140] = 0xFF3F;
	m_sjis2uniTable[0x8152 - 0x8140] = 0x30FD;
	m_sjis2uniTable[0x8153 - 0x8140] = 0x30FE;
	m_sjis2uniTable[0x8154 - 0x8140] = 0x309D;
	m_sjis2uniTable[0x8155 - 0x8140] = 0x309E;
	m_sjis2uniTable[0x8156 - 0x8140] = 0x3003;
	m_sjis2uniTable[0x8157 - 0x8140] = 0x4EDD;
	m_sjis2uniTable[0x8158 - 0x8140] = 0x3005;
	m_sjis2uniTable[0x8159 - 0x8140] = 0x3006;
	m_sjis2uniTable[0x815A - 0x8140] = 0x3007;
	m_sjis2uniTable[0x815B - 0x8140] = 0xFF70;
	m_sjis2uniTable[0x815C - 0x8140] = 0x2014;
	m_sjis2uniTable[0x815D - 0x8140] = 0x2010;
	m_sjis2uniTable[0x815E - 0x8140] = 0xFF0F;
	m_sjis2uniTable[0x815F - 0x8140] = 0x005C;
	m_sjis2uniTable[0x8160 - 0x8140] = 0x301C;
	m_sjis2uniTable[0x8161 - 0x8140] = 0x2016;
	m_sjis2uniTable[0x8162 - 0x8140] = 0xFF5C;
	m_sjis2uniTable[0x8163 - 0x8140] = 0x2026;
	m_sjis2uniTable[0x8164 - 0x8140] = 0x2025;
	m_sjis2uniTable[0x8165 - 0x8140] = 0x2018;
	m_sjis2uniTable[0x8166 - 0x8140] = 0x2019;
	m_sjis2uniTable[0x8167 - 0x8140] = 0x201C;
	m_sjis2uniTable[0x8168 - 0x8140] = 0x201D;
	m_sjis2uniTable[0x8169 - 0x8140] = 0xFF08;
	m_sjis2uniTable[0x816A - 0x8140] = 0xFF09;
	m_sjis2uniTable[0x816B - 0x8140] = 0x3014;
	m_sjis2uniTable[0x816C - 0x8140] = 0x3015;
	m_sjis2uniTable[0x816D - 0x8140] = 0xFF3B;
	m_sjis2uniTable[0x816E - 0x8140] = 0xFF3D;
	m_sjis2uniTable[0x816F - 0x8140] = 0xFF5B;
	m_sjis2uniTable[0x8170 - 0x8140] = 0xFF5D;
	m_sjis2uniTable[0x8171 - 0x8140] = 0x3008;
	m_sjis2uniTable[0x8172 - 0x8140] = 0x3009;
	m_sjis2uniTable[0x8173 - 0x8140] = 0x300A;
	m_sjis2uniTable[0x8174 - 0x8140] = 0x300B;
	m_sjis2uniTable[0x8175 - 0x8140] = 0x300C;
	m_sjis2uniTable[0x8176 - 0x8140] = 0x300D;
	m_sjis2uniTable[0x8177 - 0x8140] = 0x300E;
	m_sjis2uniTable[0x8178 - 0x8140] = 0x300F;
	m_sjis2uniTable[0x8179 - 0x8140] = 0x3010;
	m_sjis2uniTable[0x817A - 0x8140] = 0x3011;
	m_sjis2uniTable[0x817B - 0x8140] = 0xFF0B;
	m_sjis2uniTable[0x817C - 0x8140] = 0x2212;
	m_sjis2uniTable[0x817D - 0x8140] = 0x00B1;
	m_sjis2uniTable[0x817E - 0x8140] = 0x00D7;
	m_sjis2uniTable[0x8180 - 0x8140] = 0x00F7;
	m_sjis2uniTable[0x8181 - 0x8140] = 0xFF1D;
	m_sjis2uniTable[0x8182 - 0x8140] = 0x2260;
	m_sjis2uniTable[0x8183 - 0x8140] = 0xFF1C;
	m_sjis2uniTable[0x8184 - 0x8140] = 0xFF1E;
	m_sjis2uniTable[0x8185 - 0x8140] = 0x2266;
	m_sjis2uniTable[0x8186 - 0x8140] = 0x2267;
	m_sjis2uniTable[0x8187 - 0x8140] = 0x221E;
	m_sjis2uniTable[0x8188 - 0x8140] = 0x2234;
	m_sjis2uniTable[0x8189 - 0x8140] = 0x2642;
	m_sjis2uniTable[0x818A - 0x8140] = 0x2640;
	m_sjis2uniTable[0x818B - 0x8140] = 0x00B0;
	m_sjis2uniTable[0x818C - 0x8140] = 0x2032;
	m_sjis2uniTable[0x818D - 0x8140] = 0x2033;
	m_sjis2uniTable[0x818E - 0x8140] = 0x2103;
	m_sjis2uniTable[0x818F - 0x8140] = 0xFFE5;
	m_sjis2uniTable[0x8190 - 0x8140] = 0xFF04;
	m_sjis2uniTable[0x8191 - 0x8140] = 0x00A2;
	m_sjis2uniTable[0x8192 - 0x8140] = 0x00A3;
	m_sjis2uniTable[0x8193 - 0x8140] = 0xFF05;
	m_sjis2uniTable[0x8194 - 0x8140] = 0xFF03;
	m_sjis2uniTable[0x8195 - 0x8140] = 0xFF06;
	m_sjis2uniTable[0x8196 - 0x8140] = 0xFF0A;
	m_sjis2uniTable[0x8197 - 0x8140] = 0xFF20;
	m_sjis2uniTable[0x8198 - 0x8140] = 0x00A7;
	m_sjis2uniTable[0x8199 - 0x8140] = 0x2606;
	m_sjis2uniTable[0x819A - 0x8140] = 0x2605;
	m_sjis2uniTable[0x819B - 0x8140] = 0x25CB;
	m_sjis2uniTable[0x819C - 0x8140] = 0x25CF;
	m_sjis2uniTable[0x819D - 0x8140] = 0x25CE;
	m_sjis2uniTable[0x819E - 0x8140] = 0x25C7;
	m_sjis2uniTable[0x819F - 0x8140] = 0x25C6;
	m_sjis2uniTable[0x81A0 - 0x8140] = 0x25A1;
	m_sjis2uniTable[0x81A1 - 0x8140] = 0x25A0;
	m_sjis2uniTable[0x81A2 - 0x8140] = 0x25B3;
	m_sjis2uniTable[0x81A3 - 0x8140] = 0x25B2;
	m_sjis2uniTable[0x81A4 - 0x8140] = 0x25BD;
	m_sjis2uniTable[0x81A5 - 0x8140] = 0x25BC;
	m_sjis2uniTable[0x81A6 - 0x8140] = 0x203B;
	m_sjis2uniTable[0x81A7 - 0x8140] = 0x3012;
	m_sjis2uniTable[0x81A8 - 0x8140] = 0x2192;
	m_sjis2uniTable[0x81A9 - 0x8140] = 0x2190;
	m_sjis2uniTable[0x81AA - 0x8140] = 0x2191;
	m_sjis2uniTable[0x81AB - 0x8140] = 0x2193;
	m_sjis2uniTable[0x81AC - 0x8140] = 0x3013;
	m_sjis2uniTable[0x81AD - 0x8140] = 0xFF07;
	m_sjis2uniTable[0x81AE - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81AF - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B0 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B1 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B2 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B3 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B4 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B5 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B6 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B7 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81B8 - 0x8140] = 0x2208;
	m_sjis2uniTable[0x81B9 - 0x8140] = 0x220B;
	m_sjis2uniTable[0x81BA - 0x8140] = 0x2286;
	m_sjis2uniTable[0x81BB - 0x8140] = 0x2287;
	m_sjis2uniTable[0x81BC - 0x8140] = 0x2282;
	m_sjis2uniTable[0x81BD - 0x8140] = 0x2283;
	m_sjis2uniTable[0x81BE - 0x8140] = 0x222A;
	m_sjis2uniTable[0x81BF - 0x8140] = 0x2229;
	m_sjis2uniTable[0x81C0 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C1 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C2 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C3 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C4 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C5 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C6 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C7 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81C8 - 0x8140] = 0x2227;
	m_sjis2uniTable[0x81C9 - 0x8140] = 0x2228;
	m_sjis2uniTable[0x81CA - 0x8140] = 0x00AC;
	m_sjis2uniTable[0x81CB - 0x8140] = 0x21D2;
	m_sjis2uniTable[0x81CC - 0x8140] = 0x21D4;
	m_sjis2uniTable[0x81CD - 0x8140] = 0x2200;
	m_sjis2uniTable[0x81CE - 0x8140] = 0x2203;
	m_sjis2uniTable[0x81CF - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D0 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D1 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D2 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D3 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D4 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D5 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D6 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D7 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D8 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81D9 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81DA - 0x8140] = 0x2220;
	m_sjis2uniTable[0x81DB - 0x8140] = 0x22A5;
	m_sjis2uniTable[0x81DC - 0x8140] = 0x2312;
	m_sjis2uniTable[0x81DD - 0x8140] = 0x2202;
	m_sjis2uniTable[0x81DE - 0x8140] = 0x2207;
	m_sjis2uniTable[0x81DF - 0x8140] = 0x2261;
	m_sjis2uniTable[0x81E0 - 0x8140] = 0x2252;
	m_sjis2uniTable[0x81E1 - 0x8140] = 0x226A;
	m_sjis2uniTable[0x81E2 - 0x8140] = 0x226B;
	m_sjis2uniTable[0x81E3 - 0x8140] = 0x221A;
	m_sjis2uniTable[0x81E4 - 0x8140] = 0x223D;
	m_sjis2uniTable[0x81E5 - 0x8140] = 0x221D;
	m_sjis2uniTable[0x81E6 - 0x8140] = 0x2235;
	m_sjis2uniTable[0x81E7 - 0x8140] = 0x222B;
	m_sjis2uniTable[0x81E8 - 0x8140] = 0x222C;
	m_sjis2uniTable[0x81E9 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81EA - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81EB - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81EC - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81ED - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81EE - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81EF - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81F0 - 0x8140] = 0x212B;
	m_sjis2uniTable[0x81F1 - 0x8140] = 0x2030;
	m_sjis2uniTable[0x81F2 - 0x8140] = 0x266F;
	m_sjis2uniTable[0x81F3 - 0x8140] = 0x266D;
	m_sjis2uniTable[0x81F4 - 0x8140] = 0x266A;
	m_sjis2uniTable[0x81F5 - 0x8140] = 0x2020;
	m_sjis2uniTable[0x81F6 - 0x8140] = 0x2021;
	m_sjis2uniTable[0x81F7 - 0x8140] = 0x00B6;
	m_sjis2uniTable[0x81F8 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81F9 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81FA - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81FB - 0x8140] = 0x3000;
	m_sjis2uniTable[0x81FC - 0x8140] = 0x25EF;
	m_sjis2uniTable[0x8240 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8241 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8242 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8243 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8244 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8245 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8246 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8247 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8248 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8249 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824A - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824B - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824C - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824D - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824E - 0x8140] = 0x3000;
	m_sjis2uniTable[0x824F - 0x8140] = 0xFF10;
	m_sjis2uniTable[0x8250 - 0x8140] = 0xFF11;
	m_sjis2uniTable[0x8251 - 0x8140] = 0xFF12;
	m_sjis2uniTable[0x8252 - 0x8140] = 0xFF13;
	m_sjis2uniTable[0x8253 - 0x8140] = 0xFF14;
	m_sjis2uniTable[0x8254 - 0x8140] = 0xFF15;
	m_sjis2uniTable[0x8255 - 0x8140] = 0xFF16;
	m_sjis2uniTable[0x8256 - 0x8140] = 0xFF17;
	m_sjis2uniTable[0x8257 - 0x8140] = 0xFF18;
	m_sjis2uniTable[0x8258 - 0x8140] = 0xFF19;
	m_sjis2uniTable[0x8259 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825A - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825B - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825C - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825D - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825E - 0x8140] = 0x3000;
	m_sjis2uniTable[0x825F - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8260 - 0x8140] = 0xFF21;
	m_sjis2uniTable[0x8261 - 0x8140] = 0xFF22;
	m_sjis2uniTable[0x8262 - 0x8140] = 0xFF23;
	m_sjis2uniTable[0x8263 - 0x8140] = 0xFF24;
	m_sjis2uniTable[0x8264 - 0x8140] = 0xFF25;
	m_sjis2uniTable[0x8265 - 0x8140] = 0xFF26;
	m_sjis2uniTable[0x8266 - 0x8140] = 0xFF27;
	m_sjis2uniTable[0x8267 - 0x8140] = 0xFF28;
	m_sjis2uniTable[0x8268 - 0x8140] = 0xFF29;
	m_sjis2uniTable[0x8269 - 0x8140] = 0xFF2A;
	m_sjis2uniTable[0x826A - 0x8140] = 0xFF2B;
	m_sjis2uniTable[0x826B - 0x8140] = 0xFF2C;
	m_sjis2uniTable[0x826C - 0x8140] = 0xFF2D;
	m_sjis2uniTable[0x826D - 0x8140] = 0xFF2E;
	m_sjis2uniTable[0x826E - 0x8140] = 0xFF2F;
	m_sjis2uniTable[0x826F - 0x8140] = 0xFF30;
	m_sjis2uniTable[0x8270 - 0x8140] = 0xFF31;
	m_sjis2uniTable[0x8271 - 0x8140] = 0xFF32;
	m_sjis2uniTable[0x8272 - 0x8140] = 0xFF33;
	m_sjis2uniTable[0x8273 - 0x8140] = 0xFF34;
	m_sjis2uniTable[0x8274 - 0x8140] = 0xFF35;
	m_sjis2uniTable[0x8275 - 0x8140] = 0xFF36;
	m_sjis2uniTable[0x8276 - 0x8140] = 0xFF37;
	m_sjis2uniTable[0x8277 - 0x8140] = 0xFF38;
	m_sjis2uniTable[0x8278 - 0x8140] = 0xFF39;
	m_sjis2uniTable[0x8279 - 0x8140] = 0xFF3A;
	m_sjis2uniTable[0x827A - 0x8140] = 0x3000;
	m_sjis2uniTable[0x827B - 0x8140] = 0x3000;
	m_sjis2uniTable[0x827C - 0x8140] = 0x3000;
	m_sjis2uniTable[0x827D - 0x8140] = 0x3000;
	m_sjis2uniTable[0x827E - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8280 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8281 - 0x8140] = 0xFF41;
	m_sjis2uniTable[0x8282 - 0x8140] = 0xFF42;
	m_sjis2uniTable[0x8283 - 0x8140] = 0xFF43;
	m_sjis2uniTable[0x8284 - 0x8140] = 0xFF44;
	m_sjis2uniTable[0x8285 - 0x8140] = 0xFF45;
	m_sjis2uniTable[0x8286 - 0x8140] = 0xFF46;
	m_sjis2uniTable[0x8287 - 0x8140] = 0xFF47;
	m_sjis2uniTable[0x8288 - 0x8140] = 0xFF48;
	m_sjis2uniTable[0x8289 - 0x8140] = 0xFF49;
	m_sjis2uniTable[0x828A - 0x8140] = 0xFF4A;
	m_sjis2uniTable[0x828B - 0x8140] = 0xFF4B;
	m_sjis2uniTable[0x828C - 0x8140] = 0xFF4C;
	m_sjis2uniTable[0x828D - 0x8140] = 0xFF4D;
	m_sjis2uniTable[0x828E - 0x8140] = 0xFF4E;
	m_sjis2uniTable[0x828F - 0x8140] = 0xFF4F;
	m_sjis2uniTable[0x8290 - 0x8140] = 0xFF50;
	m_sjis2uniTable[0x8291 - 0x8140] = 0xFF51;
	m_sjis2uniTable[0x8292 - 0x8140] = 0xFF52;
	m_sjis2uniTable[0x8293 - 0x8140] = 0xFF53;
	m_sjis2uniTable[0x8294 - 0x8140] = 0xFF54;
	m_sjis2uniTable[0x8295 - 0x8140] = 0xFF55;
	m_sjis2uniTable[0x8296 - 0x8140] = 0xFF56;
	m_sjis2uniTable[0x8297 - 0x8140] = 0xFF57;
	m_sjis2uniTable[0x8298 - 0x8140] = 0xFF58;
	m_sjis2uniTable[0x8299 - 0x8140] = 0xFF59;
	m_sjis2uniTable[0x829A - 0x8140] = 0xFF5A;
	m_sjis2uniTable[0x829B - 0x8140] = 0x3000;
	m_sjis2uniTable[0x829C - 0x8140] = 0x3000;
	m_sjis2uniTable[0x829D - 0x8140] = 0x3000;
	m_sjis2uniTable[0x829E - 0x8140] = 0x3000;
	m_sjis2uniTable[0x829F - 0x8140] = 0x3041;
	m_sjis2uniTable[0x82A0 - 0x8140] = 0x3042;
	m_sjis2uniTable[0x82A1 - 0x8140] = 0x3043;
	m_sjis2uniTable[0x82A2 - 0x8140] = 0x3044;
	m_sjis2uniTable[0x82A3 - 0x8140] = 0x3045;
	m_sjis2uniTable[0x82A4 - 0x8140] = 0x3046;
	m_sjis2uniTable[0x82A5 - 0x8140] = 0x3047;
	m_sjis2uniTable[0x82A6 - 0x8140] = 0x3048;
	m_sjis2uniTable[0x82A7 - 0x8140] = 0x3049;
	m_sjis2uniTable[0x82A8 - 0x8140] = 0x304A;
	m_sjis2uniTable[0x82A9 - 0x8140] = 0x304B;
	m_sjis2uniTable[0x82AA - 0x8140] = 0x304C;
	m_sjis2uniTable[0x82AB - 0x8140] = 0x304D;
	m_sjis2uniTable[0x82AC - 0x8140] = 0x304E;
	m_sjis2uniTable[0x82AD - 0x8140] = 0x304F;
	m_sjis2uniTable[0x82AE - 0x8140] = 0x3050;
	m_sjis2uniTable[0x82AF - 0x8140] = 0x3051;
	m_sjis2uniTable[0x82B0 - 0x8140] = 0x3052;
	m_sjis2uniTable[0x82B1 - 0x8140] = 0x3053;
	m_sjis2uniTable[0x82B2 - 0x8140] = 0x3054;
	m_sjis2uniTable[0x82B3 - 0x8140] = 0x3055;
	m_sjis2uniTable[0x82B4 - 0x8140] = 0x3056;
	m_sjis2uniTable[0x82B5 - 0x8140] = 0x3057;
	m_sjis2uniTable[0x82B6 - 0x8140] = 0x3058;
	m_sjis2uniTable[0x82B7 - 0x8140] = 0x3059;
	m_sjis2uniTable[0x82B8 - 0x8140] = 0x305A;
	m_sjis2uniTable[0x82B9 - 0x8140] = 0x305B;
	m_sjis2uniTable[0x82BA - 0x8140] = 0x305C;
	m_sjis2uniTable[0x82BB - 0x8140] = 0x305D;
	m_sjis2uniTable[0x82BC - 0x8140] = 0x305E;
	m_sjis2uniTable[0x82BD - 0x8140] = 0x305F;
	m_sjis2uniTable[0x82BE - 0x8140] = 0x3060;
	m_sjis2uniTable[0x82BF - 0x8140] = 0x3061;
	m_sjis2uniTable[0x82C0 - 0x8140] = 0x3062;
	m_sjis2uniTable[0x82C1 - 0x8140] = 0x3063;
	m_sjis2uniTable[0x82C2 - 0x8140] = 0x3064;
	m_sjis2uniTable[0x82C3 - 0x8140] = 0x3065;
	m_sjis2uniTable[0x82C4 - 0x8140] = 0x3066;
	m_sjis2uniTable[0x82C5 - 0x8140] = 0x3067;
	m_sjis2uniTable[0x82C6 - 0x8140] = 0x3068;
	m_sjis2uniTable[0x82C7 - 0x8140] = 0x3069;
	m_sjis2uniTable[0x82C8 - 0x8140] = 0x306A;
	m_sjis2uniTable[0x82C9 - 0x8140] = 0x306B;
	m_sjis2uniTable[0x82CA - 0x8140] = 0x306C;
	m_sjis2uniTable[0x82CB - 0x8140] = 0x306D;
	m_sjis2uniTable[0x82CC - 0x8140] = 0x306E;
	m_sjis2uniTable[0x82CD - 0x8140] = 0x306F;
	m_sjis2uniTable[0x82CE - 0x8140] = 0x3070;
	m_sjis2uniTable[0x82CF - 0x8140] = 0x3071;
	m_sjis2uniTable[0x82D0 - 0x8140] = 0x3072;
	m_sjis2uniTable[0x82D1 - 0x8140] = 0x3073;
	m_sjis2uniTable[0x82D2 - 0x8140] = 0x3074;
	m_sjis2uniTable[0x82D3 - 0x8140] = 0x3075;
	m_sjis2uniTable[0x82D4 - 0x8140] = 0x3076;
	m_sjis2uniTable[0x82D5 - 0x8140] = 0x3077;
	m_sjis2uniTable[0x82D6 - 0x8140] = 0x3078;
	m_sjis2uniTable[0x82D7 - 0x8140] = 0x3079;
	m_sjis2uniTable[0x82D8 - 0x8140] = 0x307A;
	m_sjis2uniTable[0x82D9 - 0x8140] = 0x307B;
	m_sjis2uniTable[0x82DA - 0x8140] = 0x307C;
	m_sjis2uniTable[0x82DB - 0x8140] = 0x307D;
	m_sjis2uniTable[0x82DC - 0x8140] = 0x307E;
	m_sjis2uniTable[0x82DD - 0x8140] = 0x307F;
	m_sjis2uniTable[0x82DE - 0x8140] = 0x3080;
	m_sjis2uniTable[0x82DF - 0x8140] = 0x3081;
	m_sjis2uniTable[0x82E0 - 0x8140] = 0x3082;
	m_sjis2uniTable[0x82E1 - 0x8140] = 0x3083;
	m_sjis2uniTable[0x82E2 - 0x8140] = 0x3084;
	m_sjis2uniTable[0x82E3 - 0x8140] = 0x3085;
	m_sjis2uniTable[0x82E4 - 0x8140] = 0x3086;
	m_sjis2uniTable[0x82E5 - 0x8140] = 0x3087;
	m_sjis2uniTable[0x82E6 - 0x8140] = 0x3088;
	m_sjis2uniTable[0x82E7 - 0x8140] = 0x3089;
	m_sjis2uniTable[0x82E8 - 0x8140] = 0x308A;
	m_sjis2uniTable[0x82E9 - 0x8140] = 0x308B;
	m_sjis2uniTable[0x82EA - 0x8140] = 0x308C;
	m_sjis2uniTable[0x82EB - 0x8140] = 0x308D;
	m_sjis2uniTable[0x82EC - 0x8140] = 0x308E;
	m_sjis2uniTable[0x82ED - 0x8140] = 0x308F;
	m_sjis2uniTable[0x82EE - 0x8140] = 0x3090;
	m_sjis2uniTable[0x82EF - 0x8140] = 0x3091;
	m_sjis2uniTable[0x82F0 - 0x8140] = 0x3092;
	m_sjis2uniTable[0x82F1 - 0x8140] = 0x3093;
	m_sjis2uniTable[0x82F2 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F3 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F4 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F5 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F6 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F7 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F8 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82F9 - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82FA - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82FB - 0x8140] = 0x3000;
	m_sjis2uniTable[0x82FC - 0x8140] = 0x3000;
	m_sjis2uniTable[0x8340 - 0x8140] = 0x30A1;
	m_sjis2uniTable[0x8341 - 0x8140] = 0x30A2;
	m_sjis2uniTable[0x8342 - 0x8140] = 0x30A3;
	m_sjis2uniTable[0x8343 - 0x8140] = 0x30A4;
	m_sjis2uniTable[0x8344 - 0x8140] = 0x30A5;
	m_sjis2uniTable[0x8345 - 0x8140] = 0x30A6;
	m_sjis2uniTable[0x8346 - 0x8140] = 0x30A7;
	m_sjis2uniTable[0x8347 - 0x8140] = 0x30A8;
	m_sjis2uniTable[0x8348 - 0x8140] = 0x30A9;
	m_sjis2uniTable[0x8349 - 0x8140] = 0x30AA;
	m_sjis2uniTable[0x834A - 0x8140] = 0x30AB;
	m_sjis2uniTable[0x834B - 0x8140] = 0x30AC;
	m_sjis2uniTable[0x834C - 0x8140] = 0x30AD;
	m_sjis2uniTable[0x834D - 0x8140] = 0x30AE;
	m_sjis2uniTable[0x834E - 0x8140] = 0x30AF;
	m_sjis2uniTable[0x834F - 0x8140] = 0x30B0;
	m_sjis2uniTable[0x8350 - 0x8140] = 0x30B1;
	m_sjis2uniTable[0x8351 - 0x8140] = 0x30B2;
	m_sjis2uniTable[0x8352 - 0x8140] = 0x30B3;
	m_sjis2uniTable[0x8353 - 0x8140] = 0x30B4;
	m_sjis2uniTable[0x8354 - 0x8140] = 0x30B5;
	m_sjis2uniTable[0x8355 - 0x8140] = 0x30B6;
	m_sjis2uniTable[0x8356 - 0x8140] = 0x30B7;
	m_sjis2uniTable[0x8357 - 0x8140] = 0x30B8;
	m_sjis2uniTable[0x8358 - 0x8140] = 0x30B9;
	m_sjis2uniTable[0x8359 - 0x8140] = 0x30BA;
	m_sjis2uniTable[0x835A - 0x8140] = 0x30BB;
	m_sjis2uniTable[0x835B - 0x8140] = 0x30BC;
	m_sjis2uniTable[0x835C - 0x8140] = 0x30BD;
	m_sjis2uniTable[0x835D - 0x8140] = 0x30BE;
	m_sjis2uniTable[0x835E - 0x8140] = 0x30BF;
	m_sjis2uniTable[0x835F - 0x8140] = 0x30C0;
	m_sjis2uniTable[0x8360 - 0x8140] = 0x30C1;
	m_sjis2uniTable[0x8361 - 0x8140] = 0x30C2;
	m_sjis2uniTable[0x8362 - 0x8140] = 0x30C3;
	m_sjis2uniTable[0x8363 - 0x8140] = 0x30C4;
	m_sjis2uniTable[0x8364 - 0x8140] = 0x30C5;
	m_sjis2uniTable[0x8365 - 0x8140] = 0x30C6;
	m_sjis2uniTable[0x8366 - 0x8140] = 0x30C7;
	m_sjis2uniTable[0x8367 - 0x8140] = 0x30C8;
	m_sjis2uniTable[0x8368 - 0x8140] = 0x30C9;
	m_sjis2uniTable[0x8369 - 0x8140] = 0x30CA;
	m_sjis2uniTable[0x836A - 0x8140] = 0x30CB;
	m_sjis2uniTable[0x836B - 0x8140] = 0x30CC;
	m_sjis2uniTable[0x836C - 0x8140] = 0x30CD;
	m_sjis2uniTable[0x836D - 0x8140] = 0x30CE;
	m_sjis2uniTable[0x836E - 0x8140] = 0x30CF;
	m_sjis2uniTable[0x836F - 0x8140] = 0x30D0;
	m_sjis2uniTable[0x8370 - 0x8140] = 0x30D1;
	m_sjis2uniTable[0x8371 - 0x8140] = 0x30D2;
	m_sjis2uniTable[0x8372 - 0x8140] = 0x30D3;
	m_sjis2uniTable[0x8373 - 0x8140] = 0x30D4;
	m_sjis2uniTable[0x8374 - 0x8140] = 0x30D5;
	m_sjis2uniTable[0x8375 - 0x8140] = 0x30D6;
	m_sjis2uniTable[0x8376 - 0x8140] = 0x30D7;
	m_sjis2uniTable[0x8377 - 0x8140] = 0x30D8;
	m_sjis2uniTable[0x8378 - 0x8140] = 0x30D9;
	m_sjis2uniTable[0x8379 - 0x8140] = 0x30DA;
	m_sjis2uniTable[0x837A - 0x8140] = 0x30DB;
	m_sjis2uniTable[0x837B - 0x8140] = 0x30DC;
	m_sjis2uniTable[0x837C - 0x8140] = 0x30DD;
	m_sjis2uniTable[0x837D - 0x8140] = 0x30DE;
	m_sjis2uniTable[0x837E - 0x8140] = 0x30DF;
	m_sjis2uniTable[0x8380 - 0x8140] = 0x30E0;
	m_sjis2uniTable[0x8381 - 0x8140] = 0x30E1;
	m_sjis2uniTable[0x8382 - 0x8140] = 0x30E2;
	m_sjis2uniTable[0x8383 - 0x8140] = 0x30E3;
	m_sjis2uniTable[0x8384 - 0x8140] = 0x30E4;
	m_sjis2uniTable[0x8385 - 0x8140] = 0x30E5;
	m_sjis2uniTable[0x8386 - 0x8140] = 0x30E6;
	m_sjis2uniTable[0x8387 - 0x8140] = 0x30E7;
	m_sjis2uniTable[0x8388 - 0x8140] = 0x30E8;
	m_sjis2uniTable[0x8389 - 0x8140] = 0x30E9;
	m_sjis2uniTable[0x838A - 0x8140] = 0x30EA;
	m_sjis2uniTable[0x838B - 0x8140] = 0x30EB;
	m_sjis2uniTable[0x838C - 0x8140] = 0x30EC;
	m_sjis2uniTable[0x838D - 0x8140] = 0x30ED;
	m_sjis2uniTable[0x838E - 0x8140] = 0x30EE;
	m_sjis2uniTable[0x838F - 0x8140] = 0x30EF;
	m_sjis2uniTable[0x8390 - 0x8140] = 0x30F0;
	m_sjis2uniTable[0x8391 - 0x8140] = 0x30F1;
	m_sjis2uniTable[0x8392 - 0x8140] = 0x30F2;
	m_sjis2uniTable[0x8393 - 0x8140] = 0x30F3;
	m_sjis2uniTable[0x8394 - 0x8140] = 0x30F4;
	m_sjis2uniTable[0x8395 - 0x8140] = 0x30F5;
	m_sjis2uniTable[0x8396 - 0x8140] = 0x30F6;
}

int AgcmFont::SJIS_TO_UNICODE( int charIndex )
{
	return m_sjis2uniTable[charIndex - 0x8140];
}

INT32 AgcmFont::GetTextExtentKr( UINT32 ftype, char* str, INT32 strLen )
{
	INT32 width = 0;
	const INT32 len = std::min<int>( strLen == -1 ? 0xffff : strLen, strlen(str));

	unsigned char ch;
	for( int i = 0; i < len;)
	{
		ch = str[i];

		if( ch > 0x7F )
		{
			width += m_iXLH[ftype];
			i += 2;
		}
		else 
		{
			width += m_iXLE[ftype][ch];
			++i;
		}
	}

	return width;
}

INT32 AgcmFont::GetTextExtentCn( UINT32 ftype, char* str, INT32 strLen )
{
	INT32 width = 0;
	const INT32 len = std::min<int>(strLen == -1 ? 0xffff : strLen , strlen(str));

	unsigned char ch;
	for( int i = 0; i < len; )
	{
		ch = str[i];
		if( ch > 0x7F )
		{
			width += m_iXLH[ftype];
			i += 2;
		}
		else 
		{
			width += m_iXLE[ftype][ch];
			++i;
		}
	}

	return width;
}

INT32 AgcmFont::GetTextExtentJp( UINT32 ftype, char* str, INT32 strLen )
{
	INT32 width = 0;
	const INT32 len = std::min<int>(strLen == -1 ? 0xffff : strLen, strlen(str));

	unsigned char ch = 0;
	for( int i = 0; i < len; )
	{
		ch = str[i];

		if ( ( 0x80 < ch && ch <= 0x9F ) || ( ch >= 0xE0 ) )
		{
			width += GetJapaneseCharWidth( ftype, str + i );
			i += 2;
		}
		else 
		{
			width += m_iXLE[ftype][ch];
			++i;
		}
	}

	return width;
}

INT32 AgcmFont::GetTextExtentEn( UINT32 ftype, char* str, INT32 strLen )
{
	INT32 width = 0;
	const INT32 len = std::min<int>(strLen == -1 ? 0xffff : strLen, strlen(str));

	for( int i = 0; i < len; ++i)
		width += m_iXLE[ftype][ unsigned char(str[i]) ];

	return width;
}

int AgcmFont::GetCharacterWidthKr( UINT32 ftype, char* /*str*/ )
{
	return m_iXLH[ftype];
}

int AgcmFont::GetCharacterWidthCn( UINT32 ftype, char* /*str*/ )
{
	return m_iXLH[ftype];
}

int AgcmFont::GetCharacterWidthJp( UINT32 ftype, char* str )
{
	return GetJapaneseCharWidth( ftype, str );
}

int AgcmFont::GetCharacterWidthEn( UINT32 ftype, char* str )
{
	return m_iXLE[ftype][ unsigned char(str[0]) ];
}

bool AgcmFont::IsLeadByteKr( unsigned char ch )
{
	if ( ch > 0x7F )
		return true;

	return false;
}

bool AgcmFont::IsLeadByteEn( unsigned char ch )
{
	return false;
}

bool AgcmFont::IsLeadByteJp( unsigned char ch )
{
	if ( ( 0x80 < ch && ch <= 0x9F ) || ( ch >= 0xE0 ) )
		return true;

	return false;
}

bool AgcmFont::IsLeadByteCn( unsigned char ch )
{
	if ( ch > 0x7F )
		return true;

	return false;
}
