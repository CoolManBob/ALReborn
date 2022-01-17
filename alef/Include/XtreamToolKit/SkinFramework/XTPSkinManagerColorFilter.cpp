// XTPSkinImage.cpp: implementation of the CXTPSkinImage class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "XTPSkinManagerColorFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerColorFilterColorize

CXTPSkinManagerColorFilterColorize::CXTPSkinManagerColorFilterColorize(BYTE hue, BYTE sat, float blend)
{
	m_hue = hue;
	m_sat = sat;
	m_blend = blend;

	if (m_blend < 0.0f) m_blend = 0.0f;
	if (m_blend > 1.0f) m_blend = 1.0f;
}

#define  HSLMAX   255   /* H,L, and S vary over 0-HSLMAX */
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */
                        /* HSLMAX BEST IF DIVISIBLE BY 6 */
                        /* RGBMAX, HSLMAX must each fit in a BYTE. */
#define HSLUNDEFINED (HSLMAX*2/3)

RGBQUAD RGBtoHSL(RGBQUAD lRGBColor)
{
	BYTE R,G,B;                 /* input RGB values */
	BYTE H,L,S;                 /* output HSL values */
	BYTE cMax,cMin;             /* max and min RGB values */
	WORD Rdelta,Gdelta,Bdelta;  /* intermediate value: % of spread from max*/

	R = lRGBColor.rgbRed;   /* get R, G, and B out of DWORD */
	G = lRGBColor.rgbGreen;
	B = lRGBColor.rgbBlue;

	cMax = max( max(R,G), B);   /* calculate lightness */
	cMin = min( min(R,G), B);
	L = (BYTE)((((cMax+cMin)*HSLMAX)+RGBMAX)/(2*RGBMAX));

	if (cMax == cMin)
	{
		S = 0;                  /* saturation */
		H = HSLUNDEFINED;       /* hue */
	}
	else
	{                   /* chromatic case */
		if (L <= (HSLMAX/2))    /* saturation */
			S = (BYTE)((((cMax-cMin)*HSLMAX)+((cMax+cMin)/2))/(cMax+cMin));
		else
			S = (BYTE)((((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2))/(2*RGBMAX-cMax-cMin));
		/* hue */
		Rdelta = (WORD)((((cMax-R)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
		Gdelta = (WORD)((((cMax-G)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
		Bdelta = (WORD)((((cMax-B)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));

		if (R == cMax)
			H = (BYTE)(Bdelta - Gdelta);
		else if (G == cMax)
			H = (BYTE)((HSLMAX/3) + Rdelta - Bdelta);
		else /* B == cMax */
			H = (BYTE)(((2*HSLMAX)/3) + Gdelta - Rdelta);

		if (H > HSLMAX) H -= HSLMAX;
	}

	RGBQUAD hsl={L,S,H,0};
	return hsl;
}
////////////////////////////////////////////////////////////////////////////////
float HueToRGB(float n1,float n2, float hue)
{
	//<F. Livraghi> fixed implementation for HSL2RGB routine
	float rValue;

	if (hue > 360)
		hue = hue - 360;
	else if (hue < 0)
		hue = hue + 360;

	if (hue < 60)
		rValue = n1 + (n2-n1)*hue/60.0f;
	else if (hue < 180)
		rValue = n2;
	else if (hue < 240)
		rValue = n1+(n2-n1)*(240-hue)/60;
	else
		rValue = n1;

	return rValue;
}

RGBQUAD RGBtoRGBQUAD(COLORREF cr)
{
	RGBQUAD c;
	c.rgbRed = GetRValue(cr);   /* get R, G, and B out of DWORD */
	c.rgbGreen = GetGValue(cr);
	c.rgbBlue = GetBValue(cr);
	c.rgbReserved=0;
	return c;
}
////////////////////////////////////////////////////////////////////////////////
COLORREF RGBQUADtoRGB (RGBQUAD c)
{
	return RGB(c.rgbRed,c.rgbGreen,c.rgbBlue);
}

RGBQUAD HSLtoRGB(RGBQUAD lHSLColor)
{
	float h,s,l;
	float m1,m2;
	BYTE r,g,b;

	h = (float)lHSLColor.rgbRed * 360.0f / 255.0f;
	s = (float)lHSLColor.rgbGreen / 255.0f;
	l = (float)lHSLColor.rgbBlue / 255.0f;

	if (l <= 0.5) m2 = l * (1+s);
	else m2 = l + s - l*s;

	m1 = 2 * l - m2;

	if (s == 0)
	{
		r = g = b = (BYTE)(l * 255.0f);
	}
	else
	{
		r = (BYTE)(HueToRGB(m1, m2, h+120) * 255.0f);
		g = (BYTE)(HueToRGB(m1, m2, h) * 255.0f);
		b = (BYTE)(HueToRGB(m1, m2, h-120) * 255.0f);
	}

	RGBQUAD rgb = {b, g, r, 0};
	return rgb;
}

void CXTPSkinManagerColorFilterColorize::ApplyColorFilter(COLORREF& clr)
{
	int a0 = (int)(256 * m_blend);
	int a1 = 256 - a0;

	bool bFullBlend = false;
	if (m_blend > 0.999f) bFullBlend = true;

	RGBQUAD color = RGBtoRGBQUAD(clr);


	if (bFullBlend)
	{
		color = RGBtoHSL(color);
		color.rgbRed = m_hue;
		color.rgbGreen = m_sat;
		clr = RGBQUADtoRGB(HSLtoRGB(color));
	}
	else
	{
		RGBQUAD hsl = RGBtoHSL(color);
		hsl.rgbRed=m_hue;
		hsl.rgbGreen=m_sat;
		hsl = HSLtoRGB(hsl);

		color.rgbRed = (BYTE)((hsl.rgbRed * a0 + color.rgbRed * a1) >> 8);
		color.rgbBlue = (BYTE)((hsl.rgbBlue * a0 + color.rgbBlue * a1) >> 8);
		color.rgbGreen = (BYTE)((hsl.rgbGreen * a0 + color.rgbGreen * a1) >> 8);

		clr = RGBQUADtoRGB(color);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerColorFilterColorize

CXTPSkinManagerColorFilterShiftRGB::CXTPSkinManagerColorFilterShiftRGB(long r, long g, long b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}

void CXTPSkinManagerColorFilterShiftRGB::ApplyColorFilter(COLORREF& clr)
{
	int rgbRed = (BYTE)max(0, min(255, (int)(GetRValue(clr) + m_r)));
	int rgbGreen = (BYTE)max(0, min(255, (int)(GetGValue(clr) + m_g)));
	int rgbBlue = (BYTE)max(0, min(255, (int)(GetBValue(clr) + m_b)));

	clr = RGB(rgbRed, rgbGreen, rgbBlue);
}
