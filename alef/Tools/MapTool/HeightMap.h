// HeightMap.h: interface for the CHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEIGHTMAP_H__B67F0696_F0F3_4EBA_9B64_A4BED601F7D7__INCLUDED_)
#define AFX_HEIGHTMAP_H__B67F0696_F0F3_4EBA_9B64_A4BED601F7D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHeightMap  
{
public:
	UINT32	*pData	;
	INT32	nSize	;	// 넓이 .. 가로 세로 동일함.
public:
	CHeightMap();
	virtual ~CHeightMap();

public:
	BOOL IsAvailable() { return pData ? TRUE : FALSE; }
	// Operation
	bool MakeTerrainFault	( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter );

	BOOL Create( int nSize , UINT32 nInitialValue = 0 );
	void CleanUp();

	BOOL DrawDC( HDC hdc , int x = 0 , int y = 0 );

	int		GetValue( int x , int z );
	int		SetValue( int x , int z );
protected:
	
	void FilterHeightField	( float* fpHeightData, float fFilter , INT32 nSize );
	void FilterHeightBand	( float* fpBand, int iStride, int iCount, float fFilter );
	void NormalizeTerrain	( float* fpHeightData , INT32 nSize );


};

#endif // !defined(AFX_HEIGHTMAP_H__B67F0696_F0F3_4EBA_9B64_A4BED601F7D7__INCLUDED_)
