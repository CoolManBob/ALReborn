#if !defined	(__AgcuEffUtil_h_20040522__)
#define			__AgcuEffUtil_h_20040522__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "rwcore.h"
#include "rpworld.h"
#include "rwplcore.h"
#include "RpSpline.h"
#include "RtAnim.h"
#include "AgcmResourceLoader.h"

#include <algorithm>

#include "AcuMathFunc.h"

#define EFF2_FILE_NAME_MAX				64		//파일이름 
#define	EFF2_BASE_TITLE_MAX_NUM			32		//Base Title MAX NUM
typedef RwInt32 (*fptrProcess)(const RwChar* szFName, void* lpOwerClass);

RpAtomic*			Eff2Ut_AtomicModifyColor		( RpAtomic	*atomic, void *data		);		// clump 의 매터리얼 색을 바꿀때 사용할 콜백함수.
RwInt32				Eff2Ut_RpClumpGetNumVertices	( RpClump	*clump					);
const RwV3d*		Eff2Ut_RpClumpGetVertex			( RpClump	*clump, RwInt32 index	);
RwInt32				Eff2Ut_RpAtomicGetNumVertices	( RpAtomic	*atomic					);
const RwV3d*		Eff2Ut_RpAtomicGetVertex		( RpAtomic	*atomic, RwInt32 index	);
RwInt32				Eff2Ut_RpClumpGetNumTri			( RpClump	*clump					);
RwInt32				Eff2Ut_RpAtomicGetNumTri		( RpAtomic	*atomic					);

RwInt32				Eff2Ut_ForAllFile				( const RwChar* path, const RwChar* exp, fptrProcess pProcess, void* lpOwerClass);

void				Eff2Ut_RwMatrixGetScale			( const RwMatrix& matSrc, RwMatrix* pMatScale	);
void				Eff2Ut_RwMatrixGetInvScale		( const RwMatrix& matSrc, RwMatrix* pMatScale	);
void				Eff2Ut_RwMatrixGetNoScale		( const RwMatrix& matSrc, RwMatrix* pMatNoScale	);
void				Eff2Ut_RwMatrixGetRot			( const RwMatrix& matSrc, RwMatrix* pMatTrans	);
void				Eff2Ut_RwMatrixGetTrans			( const RwMatrix& matSrc, RwMatrix* pMatRot		);

static inline RwReal Eff2Ut_RwV3dSqLength			( const RwV3d& vIn ){ return (vIn.x*vIn.x + vIn.y*vIn.y + vIn.z*vIn.z); };

RpHAnimHierarchy*	Eff2Ut_GetHierarchyFromRwFrame	( RwFrame* pFrm );
RpAtomic*			Eff2Ut_SetHierarchyForSkinAtomic( RpAtomic *atomic, void *data);
RpAtomic*			Eff2Ut_GetSkinHierarchy			( RpAtomic *atomic, void *data);

RpAtomic*			Eff2Ut_CalcBBox					( RpAtomic* atomic, void* pBBox);
RpClump*			Eff2Ut_CalcBBox					( RpClump* clump, void* pBBox);
void				Eff2Ut_CalcSphere				( RwSphere* pout, const RwBBox* pin);
RpAtomic*			Eff2Ut_CalcSphere				( RpAtomic* atomic, void* pSphere);
RpClump*			Eff2Ut_CalcSphere				( RpClump* clump, void* pSphere);

RwTexture*			Eff2Ut_FindEffTex				(  const char* szTex, const char* szMask, char* szPath, AgcmResourceLoader* pResLoader, RwUInt32 dwFillterMode=rwFILTERLINEAR );

//INT 2로 나누어서 쓸때
#define Eff2Ut_SET_LONG( low, hi )			MAKELONG( low, hi )
#define Eff2Ut_GET_LOWORD( val )			LOWORD(val)
#define Eff2Ut_GET_HIWORD( val )			HIWORD(val)

void				Eff2Ut_SAFE_DESTROY_FRAME		( RwFrame*& pFrm );
void				Eff2Ut_SAFE_DESTROY_TEX			( RwTexture*& pTex );
void				Eff2Ut_SAFE_DESTROY_SPLINE		( RpSpline*& pSpline );
void				Eff2Ut_SAFE_DESTROY_RTANIM		( RtAnimAnimation*& pRtAnim );
void				Eff2Ut_SAFE_DESTROY_CLUMP		( RpClump*& pClump );
void				Eff2Ut_SAFE_DESTROY_LIGHT		( RpLight*& pLight );

#define Eff2Ut_CLAMP(a, min, max)	{	if( a<min ) a = min; else if( a>max ) a = max; }
#define Eff2Ut_SWAP(a, b)			{	(a)^=(b)^=(a)^=(b);	}//wow~
#define Eff2Ut_FTODW(f)				(*(DWORD*)&f)
#define Eff2Ut_DWTOF(dw)			(*(FLOAT*)&dw)
#define Eff2Ut_ZEROBLOCK(t)			{	ZeroMemory( &t,sizeof(t) );	}

//rand() -> 0 ~ RAND_MAX, RAND_MAX : 0x7fff == 2^15-1 == 32767
static inline int Eff2Ut_RandomNumber(int iMin, int iMax)	
{
	return((rand() % (abs(iMax-iMin)+1))+iMin);
}

//반올림,음수일 경우 반내림을 한다.
static inline double Eff2Ut_Round( double x )
{ 
	return x > 0 ? floor(x + .5) : ceil(x - .5); 
};

// Eff2Ut_ShowBinaryVal
template<class T>
int	Eff2Ut_ShowBinaryVal( char* szOut, const T& tData, BOOL bHasVTable=FALSE, BOOL bBigEndian=FALSE )
{
	unsigned char CHECKBITMASK	= 128;	//binary:1000_0000, hex:0x80, octal:0200

	int		i	= 0,
		j	= 0;

	int		nSizeByte	= sizeof(T);
	int		nSizeBit	= nSizeByte << 3;

	const unsigned char* pSrcBegin	= (const unsigned char*)&tData;
	if( bHasVTable )	pSrcBegin	+= 4;

	const unsigned char* pSrcCurr	= NULL;


	for( i=0; i<nSizeByte; ++i ){
		CHECKBITMASK	= 128;//1000_0000

		if( bBigEndian )
			pSrcCurr	= pSrcBegin + i;
		else//little endian
			pSrcCurr	= pSrcBegin + (nSizeByte - (i+1));

		for( j=0; j<8; ++j, ++szOut, CHECKBITMASK >>= 1 )
			if( (*pSrcCurr) & CHECKBITMASK )	*szOut	= '1';
			else								*szOut	= '0';

			*(szOut++)	= ' ';//checking 8 bit
	}
	*szOut	= '\0';

	//extra info
	strcat( szOut, (bBigEndian ? "- BIG ENDIAN" : "- LITTLE ENDIAN") );
	if( bHasVTable )
		strcat( szOut, " - there is vfptable");

	return 0;
}

char*		Eff2Ut_ShowRwMat( const RwMatrix* pMat, LPCSTR szMatName=NULL );
char*		Eff2Ut_ShowRwV3d( const RwV3d* pV3d, LPCSTR szVecName=NULL );
void		Eff2Ut_DbgToFile( const char* outputfile, const char* srcfile, int srcline, const char* msg, const char* where=NULL );
const char*	Eff2Ut_GetTimeStamp(void);
int			Eff2Ut_TimeStampToFile(FILE* fp);
int			Eff2Ut_TimeStampToFile(const char* fname);
void		Eff2Ut_ToFile( const char* fname, const char* msg );
void		Eff2Ut_ToConsol( const char* msg );
void		Eff2Ut_ToMsgBox( const char* msg, HWND hWnd=NULL );
void		Eff2Ut_ToWnd( const char* msg, int nx=10, int ny=10, HWND hWnd=NULL );
const char*	Eff2Ut_DxErrMSG(HRESULT hr);
const char*	Eff2Ut_FmtMsg( const char* szFmt, ... );

///////////////////////////////////////////////////////////////////////////////
// effect error_file
#ifdef	EFF2_SHOWERR
#define EFF2_ERRORFILE "Effect_ERR.txt"
#define Eff2Ut_ERR( msg )											{	\
	Eff2Ut_DbgToFile( EFF2_ERRORFILE, __FILE__, __LINE__, msg, NULL );	\
	ASSERT(!"Eff2Ut_ERR");/**/												}
#else
#define Eff2Ut_ERR( msg )
#endif//EFF2_SHOWERR

///////////////////////////////////////////////////////////////////////////////
// effect log_file
#ifdef	EFF2_SHOWLOG
#define EFF2_LOGFILE	"Effect_LOG.txt"
#define Eff2Ut_LOG( msg )	Eff2Ut_DbgToFile( EFF2_LOGFILE, __FILE__, __LINE__, msg, NULL )
#else
#define Eff2Ut_LOG( msg )
#endif//EFF2_SHOWLOG

///////////////////////////////////////////////////////////////////////////////
// effect show_info

#ifdef EFF2_SHOWINFO
#define Eff2Ut_TOCON( msg )					Eff2Ut_ToConsol(msg)
#define Eff2Ut_TOWND( msg, nx, ny, hWnd )	Eff2Ut_ToWnd(msg,nx,ny,hWnd)
#define Eff2Ut_TOFILE( fname, msg )			Eff2Ut_ToFile( fname, msg )
#else
#define Eff2Ut_TOCON( msg )
#define Eff2Ut_TOWND( msg, nx, ny, hWnd )
#define Eff2Ut_TOFILE( fname, msg )
#endif//__EFF2_SHOWINFO__

RwInt32	Eff2Ut_RenderBBox( const RwBBox& bbox, const RwFrame* pFrm );
RwInt32	Eff2Ut_RenderBSphere( const RwSphere& bsphere, const RwUInt32 colr=0xffffff00, const RwFrame* pFrm=NULL );
RwInt32	Eff2Ut_RenderSphereXZ(const RwSphere& stSphere, const RwUInt32 nColor=0xffffff00, const RwFrame* pFrm=NULL);
RwInt32	Eff2Ut_RenderBoxXZ(const RwBBox& stBox, const RwUInt32 nColor=0xffffff00, const RwFrame* pFrm=NULL);

class AuAutoSetDirectory
{
public:
	explicit AuAutoSetDirectory(LPCSTR path = NULL)	{
		memset( oldpath, 0, sizeof(oldpath) );
		BOOL chk = ::GetCurrentDirectory( MAX_PATH, oldpath );
		ASSERT( chk && "GetCurrentDirectory" );

		if(path)
		{
			chk	= ::SetCurrentDirectory( path );
			ASSERT( chk && "SetCurrentDirectory" );
		}
	}
	~AuAutoSetDirectory()			{	::SetCurrentDirectory( oldpath );	}

	LPCSTR	bGetSavedPath()const	{	return oldpath;						}		

private:
	char	oldpath[MAX_PATH];
};

RwChar *Eff2Ut_EffectPathnameCreate(const RwChar *srcBuffer);
void	Eff2Ut_EffectPathnameDestroy(RwChar *buffer);

#endif