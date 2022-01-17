#if !defined(AFX_ACUEXPORTDFF_H__3BF653A8_3508_4F97_80ED_BBA1F9EFB0DD__INCLUDED_)
#define AFX_ACUEXPORTDFF_H__3BF653A8_3508_4F97_80ED_BBA1F9EFB0DD__INCLUDED_

#include "ApBase.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "rwcore.h"
#include "rpworld.h"
#include "rpusrdat.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "AcuRpMatD3DFx.h"

#include "AcDefine.h"

#include <vector>
#include <string>

using namespace std;
#define	ACUEXPORTDFF_BUFFER_LENGTH		256

typedef void (*LogFunc)( char* szLog );
typedef void (*LogFormatFunc)( char* szFormat, ... );

typedef vector< pair< string, string > >	StringPairVec;
typedef StringPairVec::iterator				StringPairVecItr;

class CFileNameRule
{
public:
	void	InitNameRule( char* szHeader, DWORD dwID, char* szExtension, WORD wHeaderLen, WORD wIDLen, WORD wExtenceLen );
	char*	GetName( char* szName = NULL );
	void	GetFileName( char* szName, int nCount );

protected:
	char	m_szHeader[16];
	DWORD	m_dwID;
	char	m_szExtension[4];

	WORD	m_wHeaderLen;
	WORD	m_wIDLen;
	WORD	m_wExtenceLen;

	char	m_szName[256];
};

class AcuExportDFF : public CFileNameRule
{
public:
	AcuExportDFF();
	virtual ~AcuExportDFF();

	void	Init( CHAR* szLog, LogFunc funcLog = NULL);
	void	SetPath( CHAR* szSrcPath, CHAR* szDestPath, CHAR* szDffPath, CHAR* szTexturePathHQ, CHAR* szTexturePathMQ = NULL, CHAR* szTexturePathLQ = NULL );
	BOOL	ExportDFF( RpClump *pstClump, CHAR* szDestDffName = NULL );	// DFF와 Texture들을 Export한다. (Texture Name은 다 바뀐다)

private:
	void	ProcessTexture( RwTexture *pstTexture );
	void	TextureWrite( RwTexture* pstTexture, CHAR* szTexturePath, CHAR* szSrcName, CHAR* szDstName, INT32 lFormat );

	void	RegisterName( CHAR* szSrc, CHAR* szDst )	{
		ASSERT( szSrc && szDst );
		m_vecStringPair.push_back( make_pair( szSrc, szDst ) );
	}
	CHAR*	FindSrcName( CHAR* szSrc );
	CHAR*	FindDstName( CHAR* szSrc );
	
	// internal use
	static RpAtomic*	ForAllAtomics(RpAtomic *pstAtomic, PVOID pvData);
	static PVOID		ForAllParameters(DxEffect *effect , RwUInt32 type, RwChar *name, void *data, void *callbackData);
	static RpMaterial*	ForAllMaterials(RpMaterial *pstMaterial, PVOID pvData);
	static RpGeometry*	ForAllGeometries(RpGeometry *pstGeometry, PVOID pvData);

private:
	CHAR	m_szSrcPath[ACUEXPORTDFF_BUFFER_LENGTH];
	CHAR	m_szDestPath[ACUEXPORTDFF_BUFFER_LENGTH];
	CHAR	m_szDffPath[ACUEXPORTDFF_BUFFER_LENGTH];
	CHAR	m_szTexturePathHQ[ACUEXPORTDFF_BUFFER_LENGTH];		// High Quality Texture
	CHAR	m_szTexturePathMQ[ACUEXPORTDFF_BUFFER_LENGTH];		// Medium Quality Texture
	CHAR	m_szTexturePathLQ[ACUEXPORTDFF_BUFFER_LENGTH];		// Low Quality Texture

	string			m_strLogFile;

	INT16			m_nMaterialCount;
	StringPairVec	m_vecStringPair;
	LogFunc			m_funcLog;	//to show debugging info
};

#endif // !defined(AFX_ACUEXPORTDFF_H__3BF653A8_3508_4F97_80ED_BBA1F9EFB0DD__INCLUDED_)
