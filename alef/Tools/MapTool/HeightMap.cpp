// HeightMap.cpp: implementation of the CHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HeightMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeightMap::CHeightMap():pData( NULL ), nSize( 0 )
{

}

CHeightMap::~CHeightMap()
{
	CleanUp();
}

BOOL CHeightMap::Create( int nSize , UINT32 nInitialValue )
{
	CleanUp();

	ASSERT( nSize > 0 && nSize <= 0xffff );
	if( nSize <= 0 && nSize > 0xffff )
	{
		TRACE( "Out of Range\n" );
		return FALSE;
	}

	UINT32 * pHeightMap = new UINT32 [nSize*nSize];
	ASSERT( NULL != pHeightMap );
	if( NULL == pHeightMap )
	{
		TRACE( "Memmory Allocation Error\n" );
		return FALSE;
	}

	// Initialization 

	int x , z ;
	for( z = 0 ; z < nSize ; ++ z )
	{
		for( x = 0 ; x < nSize ; ++x )
		{
			pHeightMap[ x + z * nSize ] = nInitialValue;
		}
	}

	this->pData = pHeightMap	;
	this->nSize	= nSize			;

	return TRUE;
}

void CHeightMap::CleanUp()
{
	// Destruction

	if( pData != NULL )
	{
		delete [] pData;
		nSize = 0;
	}
}


void CHeightMap::NormalizeTerrain( float* fpHeightData , INT32 nSize )
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin= fpHeightData[0];
	fMax= fpHeightData[0];

	//find the min/max values of the height fTempBuffer
	for( i=1; i<nSize*nSize; i++ )
	{
		if( fpHeightData[i]>fMax ) 
			fMax= fpHeightData[i];

		else if( fpHeightData[i]<fMin ) 
			fMin= fpHeightData[i];
	}

	//find the range of the altitude
	if( fMax<=fMin )
		return;

	fHeight= fMax-fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for( i=0; i<nSize*nSize; i++ )
		fpHeightData[i]= ( ( fpHeightData[i]-fMin )/fHeight )*( ( FLOAT ) 0xff );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightBand - private
// Description:		Apply the erosion filter to an individual 
//					band of height values
// Arguments:		-fpBand: the band to be filtered
//					-iStride: how far to advance per pass
//					-iCount: Number of passes to make
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CHeightMap::FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter )
{
	float v= fpBand[0];
	int j  = iStride;
	int i;

	//go through the height band and apply the erosion filter
	for( i=0; i<iCount-1; i++ )
	{
		fpBand[j]= fFilter*v + ( 1-fFilter )*fpBand[j];
		
		v = fpBand[j];
		j+= iStride;
	}
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightfTempBuffer - private
// Description:		Apply the erosion filter to an entire buffer
//					of height values
// Arguments:		-fpHeightData: the height values to be filtered
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CHeightMap::FilterHeightField( float* fpHeightData, float fFilter , INT32 nSize )
{
	int i;

	//erode left to right
	for( i=0; i<nSize; i++ )
		FilterHeightBand( &fpHeightData[nSize*i], 1, nSize, fFilter );
	
	//erode right to left
	for( i=0; i<nSize; i++ )
		FilterHeightBand( &fpHeightData[nSize*i+nSize-1], -1, nSize, fFilter );

	//erode top to bottom
	for( i=0; i<nSize; i++ )
		FilterHeightBand( &fpHeightData[i], nSize, nSize, fFilter);

	//erode from bottom to top
	for( i=0; i<nSize; i++ )
		FilterHeightBand( &fpHeightData[nSize*(nSize-1)+i], -nSize, nSize, fFilter );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainFault - public
// Description:		Create a height data set using the "Fault Formation"
//					algorithm.  Thanks a lot to Jason Shankel for this code!
// Arguments:		-iSize: Desired size of the height map
//					-iIterations: Number of detail passes to make
//					-iMinDelta, iMaxDelta: the desired min/max heights
//					-iIterationsPerFilter: Number of passes per filter
//					-fFilter: Strength of the filter
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
bool CHeightMap::MakeTerrainFault( int nSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter )
{
	float* fTempBuffer;
	int iCurrentIteration;
	int iHeight;
	int iRandX1, iRandZ1;
	int iRandX2, iRandZ2;
	int iDirX1, iDirZ1;
	int iDirX2, iDirZ2;
	int x, z;
	int i;

	if( !Create( nSize ) )
	{
		return FALSE;
	}

	UINT32 * pHeightMap = this->pData;

	// Create Temporary buffer
	fTempBuffer= new float [nSize*nSize];

	//check to see if memory was successfully allocated
	if( fTempBuffer==NULL )
	{
		TRACE( "Temporary buffer creation fail\n" );
		return false;
	}

	//clear the height fTempBuffer
	for( i=0; i<nSize*nSize; i++ )
		fTempBuffer[i]= 0.0f;

	for( iCurrentIteration=0; iCurrentIteration<iIterations; iCurrentIteration++ )
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		iHeight= iMaxDelta - ( ( iMaxDelta-iMinDelta )*iCurrentIteration )/iIterations;
		
		//pick two points at random from the entire height map
		iRandX1= rand( )%nSize;
		iRandZ1= rand( )%nSize;
		
		//check to make sure that the points are not the same
		do
		{
			iRandX2= rand( )%nSize;
			iRandZ2= rand( )%nSize;
		} while ( iRandX2==iRandX1 && iRandZ2==iRandZ1 );

		
		//iDirX1, iDirZ1 is a vector going the same direction as the line
		iDirX1= iRandX2-iRandX1;
		iDirZ1= iRandZ2-iRandZ1;
		
		for( z=0; z<nSize; z++ )
		{
			for( x=0; x<nSize; x++ )
			{
				//iDirX2, iDirZ2 is a vector from iRandX1, iRandZ1 to the current point (in the loop)
				iDirX2= x-iRandX1;
				iDirZ2= z-iRandZ1;
				
				//if the result of ( iDirX2*iDirZ1 - iDirX1*iDirZ2 ) is "up" (above 0), 
				//then raise this point by iHeight
				if( ( iDirX2*iDirZ1 - iDirX1*iDirZ2 )>0 )
					fTempBuffer[( z*nSize )+x]+= ( float )iHeight;
			}
		}

		//erode terrain
		FilterHeightField( fTempBuffer, fFilter , nSize );
	}

	//normalize the terrain for our purposes
	NormalizeTerrain( fTempBuffer , nSize );

	//transfer the terrain into our class's unsigned char height buffer
	for( z=0; z<nSize; z++ )
	{
		for( x=0; x<nSize; x++ )
			pHeightMap[ x + z * nSize ] = ( DWORD )( fTempBuffer[( z*nSize )+x] );
	}
	
	//delete temporary buffer
	if( fTempBuffer )
	{
		//delete the data
		delete[] fTempBuffer;
	}

	return true;
}

BOOL CHeightMap::DrawDC( HDC hdc , int x , int y )
{
	ASSERT( IsAvailable() );
	if( !IsAvailable() ) return FALSE;

	ASSERT( NULL != hdc );
	if( NULL == hdc )
	{
		TRACE( "DC is null\n" );
		return FALSE;
	}
	// 비트맵으로 설정..
	int nx , ny;
	for( ny = 0 ; ny < nSize ; ++ ny )
	{
		for( nx = 0 ; nx < nSize ; ++ nx )
		{
			::SetPixel( hdc , x + nx , y + ny  ,
				RGB(	pData[ nx + ny * nSize ]	, 
						pData[ nx + ny * nSize ]	, 
						pData[ nx + ny * nSize ]	) );
		}
	}	

	return TRUE;
}