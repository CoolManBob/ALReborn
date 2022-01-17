// ksBmpLoader.h: interface for the CksBmpLoader class.
// 2002.8.16 윤소장님도움으로 업버전.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KSBMPLOADER_H__012901EB_7B76_4C7F_8739_C0FC99C38092__INCLUDED_)
#define AFX_KSBMPLOADER_H__012901EB_7B76_4C7F_8739_C0FC99C38092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// include
#include <Windows.h>
#include <stdio.h>

typedef class CksBmpLoader  
{
	// variabels
private:	
	BOOL				m_bSuccessfulRead;
	FILE				*m_fp;
	BITMAPFILEHEADER	m_bmpfh;
	BITMAPINFOHEADER	m_bmpih;
	BYTE				*m_lpbyteBuff;
	char				*m_lpchFileName;
	static int			m_Count;
	BOOL				m_bRGBOrder; //1:rgb 0:brg 

public:
	// functions
private:
	BOOL		LoadData();
	static BOOL		Save(FILE* fp, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* pRgbQuad, LPBYTE pBuff);
public:
	BOOL		Load(LPCSTR fname);
	static BOOL		Save(LPCSTR fname, BITMAPFILEHEADER* pbfh, BITMAPINFOHEADER* pbih, RGBQUAD* pRgbQuad, LPBYTE pBuff);


public:
	BOOL		IsCorrectRGBOrder() {return m_bRGBOrder;}
	BOOL		IsRead()			{return m_bSuccessfulRead;}
	LONG		GetBmpWidth()		{return m_bmpih.biWidth;}
	LONG		GetBmpHeight()		{return m_bmpih.biHeight;}
	WORD		GetBitCount()		{return m_bmpih.biBitCount;}
	DWORD		GetSizeImage()		{return m_bmpih.biSizeImage;}
	BYTE*		GetBuffAddress()	{return m_lpbyteBuff;}

	CksBmpLoader();
	CksBmpLoader(LPCSTR fileName);
	virtual ~CksBmpLoader();

}BMPLOADER, *LPBMPLOADER;

#endif // !defined(AFX_KSBMPLOADER_H__012901EB_7B76_4C7F_8739_C0FC99C38092__INCLUDED_)
