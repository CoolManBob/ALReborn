#pragma once

#include <winsock2.h>

#include "AcDefine.h"
#include "AcuFileAdmin.h"
#include "AcuTexture.h"
#include "AcuObject.h"
#include "AcuExportDFF.h"

#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmSkill.h"
#include "AgcmSkill.h"
#include "AgcmPreLODManager.h"
#include "AgcmEff2.h"
#include "AgcuEffIniMng.h"
#include "AgcmUIManager2.h"
#include "AgcmEventEffect.h"

enum eExportTarget
{
	SET_INTERNAL,		// 내부 테스트용 ( Trunk Only 체크된것 모두 익스포트 )
	SET_TESTSERVER,		// 테스트 서버용 ( Reserved for next version up )
	SET_DISTRIBUTE		// 외부 배포용. ( Trunk Only 된것은 모두 제외함.
};

// AGCDE	: CHAR - race로 분류 
// FGHIJKL	: ITEM - type으로 분류(equip(armour, weapon...), usable, other)
// MNO		: OBJ
// P		: EFFECT
// S		: ETC TEX
// T		: WORLD TEX
// U		: UI TEX(ITEM)
// V		: UI TEX(SKILL)
// W		: EFFECT TEX
// X		: OBJ TEX
// Y		: ITEM TEX
// Z		: CAHR TEX
#define AER_FX_EXT								"FX"

#define D_IMAGE_EXTENSION_BMP					"BMP"
#define D_IMAGE_EXTENSION_PNG					"PNG"
#define D_IMAGE_EXTENSION_TIF					"TIF"

#define D_SKIP_FILE_A0001C0T_TIF				"A0001C0T.TIF"
#define D_SKIP_FILE_A010380T_PNG				"A010380T.PNG"
#define D_SKIP_FILE_A010280T_PNG				"A010280T.PNG"
#define D_SKIP_FILE_A010180T_PNG				"A010180T.PNG"

typedef map< string, string >		StringMap;
typedef StringMap::iterator			StringMapItr;
typedef StringMap::const_iterator	StringMapCItr;

class CStringFinder
{
public:
	CStringFinder()		{							}
	~CStringFinder()	{	m_mapString.clear();	}

	void	Add( char* szKey, char* szString )		{
		if( !szKey || !szKey[0] )		return;
		StringMapItr Itr = m_mapString.find( szKey );
		if( Itr == m_mapString.end() )
			m_mapString.insert( make_pair( szKey, szString ) );
	}

	void	Delete( char* szText )	{
		if( IsFind( szText ) )
			m_mapString.erase( szText );
	}

	char*	GetData( char* szText )	{
		StringMapItr Itr = m_mapString.find( szText );
		return Itr != m_mapString.end() ? (char*)(*Itr).second.c_str() : NULL;
	}

	BOOL	IsFind( char* szText )	{
		if( !szText || !szText[0] )		return FALSE;

		StringMapItr Itr = m_mapString.find( szText );
		return Itr != m_mapString.end() ? TRUE : FALSE;
	}

private:
	StringMap	m_mapString;
};

class functrExportSocailAnim;
class AgcmExportResource
{
	friend class functrExportSocailAnim;

public:
	AgcmExportResource();
	virtual ~AgcmExportResource();

	void	SetExportArea( ApServiceArea eServiceArea = AP_SERVICE_AREA_KOREA )		{	m_eExportAreaSetting = eServiceArea;	}
	void	SetExportTarget( eExportTarget eTarget = SET_INTERNAL )					{	m_eExportTarget = eTarget;				}
	ApServiceArea	GetExportArea()		{	return m_eExportAreaSetting;	}
	eExportTarget	GetExportTarget()	{	return m_eExportTarget;			}

	BOOL	Initialize( CHAR* pszSrcPath, CHAR* pszDestPath, AgcmCharacter* pcsAgcmCharacter, AgcmItem* pcsAgcmItem, AgcmObject* pcsAgcmObject, LogFunc funcLog, LogFormatFunc funcLogFormat );
	BOOL	SetSourcePath( CHAR* pszSrcPath, CHAR* pszDestPath, AgcmCharacter* pcsAgcmCharacter, AgcmItem* pcsAgcmItem, AgcmObject* pcsAgcmObject );

	BOOL	ExportCharacterTemplate( AgpmCharacter *pcsAgpmCharacter, AgcmCharacter *pcsAgcmCharacter, AgcmEventEffect *pcsAgcmEventEffect,
									CHAR *pszCharacterTemplatePublic, CHAR *pszCharacterTemplateClient, CHAR *pszCharacterTemplateAnimation, CHAR *pszCharacterTemplateCustomize,
									CHAR *pszCharacterTemplateSkill, CHAR *pszCharacterTemplateSkillSound, CHAR *pszCharacterTemplateEventEffect, CHAR *pszSocialPC, CHAR *pszSocialGM,
									AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszCharacterPreLOD, AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate );

	BOOL	ExportItemTemplates( AgpmItem* ppmItem, AgcmItem* pcmItem, char* pTargetPath, char* pEntryFileName, AgcmPreLODManager* pcmPreLODManager, char* pItemPreLODFileName );
	//BOOL	ExportItemTemplate( AgpmItem *pcsAgpmItem, AgcmItem *pcsAgcmItem, CHAR *pszItemTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD, vector<INT32>	*pvectorWishlist );

	BOOL	ExportObjectTemplate( ApmObject *pcsApmObject, AgcmObject* pcsAgcmObject, CHAR* pszObjectTemplate, AgcmPreLODManager* pcsAgcmPreLODManager, CHAR *pszObjectPreLOD );
	BOOL	ExportSkillTemplate( AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate );
	BOOL	ExportEffectResource(AgcmEff2 *pAgcmEff2 );
	BOOL	ExportUIResource();
	BOOL	ExportWorldResource();
	BOOL	ExportEtcResource();
	BOOL	ExportINIResource();
	BOOL	ExportDDSPath( AgcmResourceLoader *pcsAgcmResourceLoader );

protected:
	BOOL	ExportCharacterTemplate( AgpmCharacter *pcsAgpmCharacter, AgcmCharacter *pcsAgcmCharacter, AgcmEventEffect *pcsAgcmEventEffect, CHAR *pszCharacterTemplatePublic, CHAR *pszCharacterTemplateClient,
									CHAR *pszCharacterTemplateAnimation, CHAR *pszCharacterTemplateCustomize, CHAR *pszCharacterTemplateSkill, CHAR *pszCharacterTemplateSkillSound, CHAR *pszCharacterTemplateEventEffect, AgcmSkill *pcsAgcmSkill );
	BOOL	ExportCharacterClump( AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate ); 
	BOOL	ExportCharacterAnimation( AgcmSkill *pcsAgcmSkill, AgcmCharacter *pcsAgcmCharacter, AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAgcdSkillData );
	BOOL	ExportCharacterSocialAnimation( AgcmEventEffect *pcsAgcmEventEffect );

	//BOOL	ExportItemTemplate( AgpmItem *pcsAgpmItem, AgcmItem *pcsAgcmItem, CHAR *pszItemTemplate, vector<INT32> *pvectorWishlist );
	BOOL	ExportItemTemplates( AgpmItem* ppmItem, AgcmItem* pcmItem, char* pTargetPath, char* pEntryFileName );

	BOOL	ExportItemClump( AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate );
	BOOL	ExportItemUI( AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate );
	BOOL	ExportObjectTemplate( ApmObject *pcsApmObject, AgcmObject *pcsAgcmObject, CHAR *pszObjectTemplate );
	BOOL	ExportObjectData( ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate );
	BOOL	ExportSkillTemplate_( AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate );
	BOOL	ExportSkillTexture( AgpdSkillTemplate *pstAgpdSkillTemplate, AgcdSkillTemplate *pstAgcdSkillTemplate );
	BOOL	ExportEffectData(AgcmEff2 *pAgcmEff2 );
	BOOL	ExportFXData(CHAR *pszPath );
	BOOL	ExportUIData();
	BOOL	ExportWorldTexture();
	BOOL	ExportEtcTexture();
	BOOL	ExportDDSPath( CHAR *pszPath, AgcmResourceLoader *pcsAgcmResourceLoader );

	BOOL	ReadCharacterTemplate( AgpmCharacter *pcsAgpmCharacter, AgcmCharacter *pcsAgcmCharacter, AgcmEventEffect *pcsAgcmEventEffect,
								CHAR *pszCharacterTemplatePublic, CHAR *pszCharacterTemplateClient, CHAR *pszCharacterTemplateAnimation, CHAR *pszCharacterTemplateCustomize,
								CHAR *pszCharacterTemplateSkill, CHAR *pszCharacterTemplateSkillSound, CHAR *pszCharacterTemplateEventEffect,
								AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszCharacterPreLOD, AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate );
	BOOL	ReadCharacterPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszCharacterPreLOD);
	BOOL	ReadItemTemplate(AgpmItem *pcsAgpmItem, CHAR *pszItemTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD);
	BOOL	ReadItemPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD);
	BOOL	ReadObjectTemplate(ApmObject *pcsApmObject, CHAR *pszObjectTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszObjectPreLOD);
	BOOL	ReadObjectPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszObjectPreLOD);
	BOOL	ReadSkillTemplate(AgpmSkill *pcsAgpmSkill, CHAR *pszSkillTemplate);
	BOOL	ReadUIINI(AgcmUIManager2 *pcsAgcmUIManager2, CHAR *pszUIMessage, CHAR *pszUIINI);

	BOOL	ExportClump( CHAR *pszDffName, RpClump *pstClump, UINT32 ulID, CHAR *pszHeader, CHAR *pszExtension, CHAR *pszDffPath, CHAR *pszHTexturePath, CHAR *pszMTexturePath, CHAR *pszLTexturePath, CStringFinder& cStringFinder );
	BOOL	ExportFile( CHAR *pszFileName, CHAR *pszHeader, CHAR *pszExtension, CHAR *pszPath, UINT32 ulID, CStringFinder& cStringFinder );
	BOOL	ExportFile( CHAR *pszSrcPath, CHAR *pszDestPath, CHAR *pszSrcFileName , CHAR *pszDestFileName );
	BOOL	ExportDDSTexture( CHAR *pszTextureName, CHAR *pszPath, CHAR *pszHeader, UINT32 ulID );
	BOOL	ExportDDSTexture( CHAR *pszTextureName, CHAR *pszHeader, CHAR *pszHTexturePath, CHAR *pszMTexturePath, CHAR *pszLTexturePath, UINT32 ulID, CStringFinder& cStringFinder );
	BOOL	ExportTextureDictionary( CHAR *pszTextureName, CHAR *pszPath, CHAR *pszHeader, UINT32 ulID, CStringFinder& cStringFinder );
	BOOL	ExportTexDictFolder( CHAR *pszPath, BOOL bUseMipmap );
	BOOL	ExportTextureDictFolder( CHAR *pszPath, BOOL bUseMipmap, BOOL bUseDDS = FALSE, BOOL bMipmap = TRUE );
	BOOL	ExportTextureDDSFolder( CHAR *pszPath, CHAR *pszFindPath, INT32 lFormat, INT32 lReduceRatio, AcuTextureCopyType eCopyType, BOOL bMipmap = TRUE );
	BOOL	ExportFolder( CHAR* szRelativePath );
	
	BOOL	CheckLoadedClump( AgcdCharacterTemplate *pcsAgcdCharacterTemplate );
	BOOL	CheckLoadedClump( AgcdItemTemplate *pcsAgcdItemTemplate );
	BOOL	CheckLoadedClump( AgcdObjectTemplate *pcsAgcdObjectTemplate, AgcdObjectTemplateGroupList *pstList );

	BOOL	CreateDirectoryForCharacterTemplate();
	BOOL	CreateDirectoryForItemTemplate();
	BOOL	CreateDirectoryForObjectTemplate();
	BOOL	CreateDirectoryForSkillTemplate();
	BOOL	CreateDirectoryForEffectFile();
	BOOL	CreateDirectoryForUIData();
	BOOL	CreateDirectoryForWorldData();
	BOOL	CreateDirectoryForEtcTexture();
	BOOL	CreateDirectoryForINIResource();

private:
	AcuTextureType _GetTextureType(CHAR *pszFile);
	VOID	_ToUpper(CHAR *szDest, CHAR *szSrc);
	VOID	_StringCpy(CHAR **ppszDest, CHAR *pszSrc, BOOL bSkipExt = FALSE);
	BOOL	_StringCmp(CHAR *pszSrcString, CHAR *pszCmpString);
	BOOL	_CopyFile(CHAR *pszPath, CHAR *pszName, BOOL bEncryption);
	BOOL	_CopyDirectory(CHAR *pszPath);
	BOOL	_CreateDirectory(CHAR *pszCurrentDirectory, CHAR *pszCheckDirectory);
	BOOL	_IsSkipFile(CHAR *pszFile);

public:
	CHAR			m_szSrcPath[1024];
	CHAR			m_szDestPath[1024];

protected:
	ApServiceArea	m_eExportAreaSetting;
	eExportTarget	m_eExportTarget;

	CStringFinder	m_cCharacterClump;
	CStringFinder	m_cCharacterAnimation;
	CStringFinder	m_cItemClump;
	CStringFinder	m_cTextureItemUI;
	CStringFinder	m_cObjectClump;	
	CStringFinder	m_cObjectAnimation;
	CStringFinder	m_cEffectClump;
	CStringFinder	m_cEffectAnimation;
	CStringFinder	m_cTextureEffect;
	CStringFinder	m_cTextureSkill;

	AcuExportDFF	m_csExportDFF;
	AuExcelTxtLib	m_csExcelTxtLib;

	LogFunc			m_funcLog;
	LogFormatFunc	m_funcLogFormat;
};


// 마고자
// 나뉘어진 템플릿 파일 읽는 펑션
// 페스로 ini\\ObjectTemplate  <- 끝에 \\없는 걸로 입력.

class ApModuleManager;
class AgcmPreLODManager;
class AgcmGeometryDataManager;	
class AgpdCharacterTemplate;
class AgpmCharacter;
class AgcmCharacter;
class AgcmSkill;
class AgcmEventEffect;
class ApmObject;
class ApdObjectTemplate;
class AgpmItem;
class AgpdItemTemplate;

class	SequenceNumberManager
{
private:
	SequenceNumberManager():
	  m_nAttachDataIndexCharacter	( -1 ),
	  m_nAttachDataIndexObject		( -1 ),
	  m_nAttachDataIndexItem		( -1 ),
	  m_pcsAgpmCharacter			( NULL ),
	  m_pcsApmObject				( NULL ),
	  m_pcsAgpmItem					( NULL )
	{}

public:

	// Singleton
	static SequenceNumberManager &GetInstance()
	{
		static SequenceNumberManager	stInstance;
		return stInstance;
	}

	// 툴에서 사용할 데이타들..
	struct	stSequenceNumber
	{
	protected:
		INT32	nSequenceNumber;	// 1-Based , 0 은 초기화되지 않은 값.

	public:
		void	Init()					{	nSequenceNumber = 0;	}
		void	SetNumber( INT32 nSeq )	{	nSequenceNumber = nSeq;	}
		INT32	GetNumber() const		{	return nSequenceNumber;	}
	};

	// OnAddModule 에서 불려 주어야함.
	BOOL	AttachTemplateSequenceData();
	bool	IsReady()
	{
		return	m_nAttachDataIndexCharacter != -1 &&
				m_nAttachDataIndexObject	!= -1 &&
				m_nAttachDataIndexItem		!= -1 ?
				true : false;
	}

	template< typename T >
	static	BOOL OnSequenceNumberCreate(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	stSequenceNumber	* GetAttachedData(	AgpdCharacterTemplate	* pcsTemplate );
	stSequenceNumber	* GetAttachedData(	ApdObjectTemplate		* pcsTemplate );
	stSequenceNumber	* GetAttachedData(	AgpdItemTemplate		* pcsTemplate );

	AgpmCharacter	*GetAgpmCharacter	()	;
	ApmObject		*GetApmObject		()	;
	AgpmItem		*GetAgpmItem		()	;

protected:
	INT16	m_nAttachDataIndexCharacter	;
	INT16	m_nAttachDataIndexObject	;
	INT16	m_nAttachDataIndexItem		;

	AgpmCharacter	*m_pcsAgpmCharacter	;
	ApmObject		*m_pcsApmObject		;
	AgpmItem		*m_pcsAgpmItem		;		
};

namespace TSO	// Template Separated Operating
{
	enum ReadError
	{
		LE_NOERROR,
		LE_PARAMETER_ERROR,
		LE_MODULE_NOT_FOUND,
		LE_READFAILD,
		LE_NO_SEPARATED_TEMPLATE
	};

	const	char	c_strTemplateFormat[]	= "%s\\%06d.%06d";
	// 템플릿ID,Sequence Number
	// Sequence Number가 익스포트 순서에 의해서 결정이 되므로 Unique 해야 한다.
	// 그리고 Template에서 Sequence Number가 변하지 않게 유지가 되어야한다.

	BOOL		FindIniFiles( const char * pPath , vector< string >	&vecFile );
	INT32		GetSequenceNumber( const char *pPath );

	struct	DummyLogger
	{ bool	operator()( const char * pStr ) { return true; } };

	// 템플릿 로더 베이스 클래스.
	template< typename T >
	class	LoaderBase
	{
	public:
		LoaderBase( ApModuleManager &csModuleManager ):m_csModuleManager( csModuleManager )
		{
			pcsAgcmPreLODManager		= dynamic_cast< AgcmPreLODManager		*>( m_csModuleManager.GetModule( "AgcmPreLODManager") );
			pcsAgcmGeometryDataManager	= dynamic_cast< AgcmGeometryDataManager	*>( m_csModuleManager.GetModule( "AgcmGeometryDataManager") );
		}

		// 통채로 로드& 쎄이브할때 씀
		template< typename StringLogger >
		ReadError	LoadTemplates( const char * pPath , StringLogger logger );
		template< typename StringLogger >		
		BOOL		SaveTemplates( const char * pPath , StringLogger logger );

		ReadError	LoadTemplates( const char * pPath ) { return LoadTemplates( pPath , DummyLogger() ); }
		ReadError	SaveTemplates( const char * pPath ) { return SaveTemplates( pPath , DummyLogger() ); }

		// 하나만 쎄이브함.
		BOOL	SaveTemplateOne( const char * pPath , T * pcsTemplate );

	private:
		// 각기 다른 이름의 펑션에 대한 인터페이스..
		virtual T * AddTemplate( INT32 lTID ) = 0;
		virtual	T *	GetSequence( INT32	&lIndex ) = 0;
		// 버퍼 초기화 ( 로딩시 )
		virtual BOOL	Initialize( int nAlloc ) = 0;
		// 각기 다른 로딩/쎄이브 에 대한 인터페이스.
		virtual BOOL		SaveTemplate( ApModuleStream	&csStream , T * pcsTemplate ) = 0;
		virtual BOOL		LoadTemplate( ApModuleStream	&csStream , T * pcsTemplate ) = 0;

	protected:
		ApModuleManager			&m_csModuleManager			;
		AgcmPreLODManager		*pcsAgcmPreLODManager		;
		AgcmGeometryDataManager	*pcsAgcmGeometryDataManager	;
	};

	class	CharacterLoader : public LoaderBase< AgpdCharacterTemplate >
	{
	public:
		CharacterLoader( ApModuleManager &csModuleManager );

	private:
		virtual BOOL		SaveTemplate( ApModuleStream	&csStream , AgpdCharacterTemplate * pcsTemplate );
		virtual BOOL		LoadTemplate( ApModuleStream	&csStream , AgpdCharacterTemplate * pcsTemplate );

		virtual BOOL	Initialize( int nKeyNum ) ;

		virtual AgpdCharacterTemplate * AddTemplate( INT32 lTID	);
		virtual	AgpdCharacterTemplate *	GetSequence( INT32	&lIndex	);

	private:
		AgpmCharacter		*pcsAgpmCharacter		;
		AgcmCharacter		*pcsAgcmCharacter		;
		AgcmSkill			*pcsAgcmSkill			;
		AgcmEventEffect		*pcsAgcmEventEffect		;
	};

	class	ObjectLoader : public LoaderBase< ApdObjectTemplate >
	{
	public:
		ObjectLoader( ApModuleManager &csModuleManager );

	private:
		virtual BOOL		SaveTemplate( ApModuleStream	&csStream , ApdObjectTemplate * pcsTemplate );
		virtual BOOL		LoadTemplate( ApModuleStream	&csStream , ApdObjectTemplate * pcsTemplate );

		virtual BOOL	Initialize( int nKeyNum );

		virtual ApdObjectTemplate * AddTemplate( INT32 lTID	);
		virtual	ApdObjectTemplate * GetSequence( INT32	&lIndex	);

	private:
		ApmObject				*pcsApmObject				;
	};

	class	ItemLoader : public LoaderBase< AgpdItemTemplate >
	{
	public:
		ItemLoader( ApModuleManager &csModuleManager );

	private:
		virtual BOOL		SaveTemplate( ApModuleStream	&csStream , AgpdItemTemplate * pcsTemplate );
		virtual BOOL		LoadTemplate( ApModuleStream	&csStream , AgpdItemTemplate * pcsTemplate );

		virtual BOOL	Initialize( int nKeyNum );

		virtual AgpdItemTemplate * AddTemplate( INT32 lTID	);
		virtual	AgpdItemTemplate * GetSequence( INT32	&lIndex	)	{ return NULL;}

	private:
		AgpmItem				*pcsAgpmItem				;
	};

	// Implementation

	template< typename T >
	template< typename StringLogger >		
	BOOL	LoaderBase< T >::SaveTemplates( const char * pPath , StringLogger logger )
	{
		INT32	lIndex = 0;
		const BOOL	bEncryption = FALSE;

		SequenceNumberManager &cSeqManager = SequenceNumberManager::GetInstance();

		INT32	nSequence = 1;
		for (T	*pcsTemplate = GetSequence(lIndex); pcsTemplate; pcsTemplate = GetSequence(lIndex))
		{
			ApModuleStream		csStream;
			csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
			if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

			CHAR				szTID[32];
			sprintf(szTID, "%d", pcsTemplate->GetID() );

			// TID로 Section을 설정하고
			csStream.SetSection(szTID);

			if( !SaveTemplate( csStream , pcsTemplate ) )
			{
				std::string	str = szTID;
				str += " SaveTemplate Failed";
				logger( str.c_str() );
				return FALSE;
			}

			char	szFile[ MAX_PATH ] ;

			// 원래 시퀀스 넘버가 있을경우 저장함..
			// 없으면 여기서 지정함..
			SequenceNumberManager::stSequenceNumber	* pSeq = cSeqManager.GetAttachedData( pcsTemplate );
			if( !pSeq->GetNumber() )	pSeq->SetNumber( nSequence++ );

			sprintf( szFile , c_strTemplateFormat , pPath , pcsTemplate->GetID() , pSeq->GetNumber() );
			if( !csStream.Write(szFile, 0, bEncryption) )
			{
				std::string	str = szTID;
				str += " SaveTemplate Write Failed";
				logger( str.c_str() );
				return FALSE;
			}
		}

		return TRUE;
	}

	template< typename T >
	template< typename StringLogger >
	ReadError	LoaderBase< T >::LoadTemplates( const char * pPath , StringLogger logger )
	{
		vector< string >	vecFile;
		if( !FindIniFiles( pPath , vecFile ) )
		{
			logger( "No Separated Templates" );
			return LE_NO_SEPARATED_TEMPLATE;
		}

		INT32	nNumKeys = static_cast<int>( vecFile.size() );;

		if( !Initialize( nNumKeys ) )
		{
			logger( "Initialize Failed" );
			return LE_READFAILD;
		}

		SequenceNumberManager &cSeqManager = SequenceNumberManager::GetInstance();
		if( !cSeqManager.IsReady() )
		{
			logger( "Sequence Number Attach Data not found!" );
			return LE_MODULE_NOT_FOUND;
		}

		const BOOL	bDecryption	 = FALSE;

		vector< INT32 >	vecSeq;

		// 각 Section에 대해서...
		// 각 Section에 대해서
		for( vector< string >::iterator iter = vecFile.begin(); iter != vecFile.end(); ++iter )
		{
			char	szFile[ MAX_PATH ];
			sprintf( szFile , "%s\\%s" , pPath , iter->c_str() );

			INT32	nSequence = GetSequenceNumber( szFile );
			if( !nSequence )		continue;

			// Sequence 는 Unique 해야한다.
			if( vecSeq.end() != find( vecSeq.begin() , vecSeq.end() , nSequence ) )
			{
				std::string	str = szFile;
				str += " Sequence Number Corrupted ";
				logger( str.c_str() );
				return LE_READFAILD;
			}

			vecSeq.push_back( nSequence );

			ApModuleStream		csStream;
			// Template을 추가한다.
			if( !csStream.Open(szFile, 0, bDecryption) )
			{
				std::string	str = szFile;
				str += "Stream Open Failed";
				logger( str.c_str() );
				return LE_READFAILD;
			}

			INT32	nTID = atoi(csStream.ReadSectionName(0));

			T	* pcsTemplate = AddTemplate( nTID );
			if (!pcsTemplate)
			{
				std::string	str = szFile;
				str += "AddTemplate Failed";
				logger( str.c_str() );
				return LE_READFAILD;
			}

			if( !LoadTemplate( csStream , pcsTemplate ) )
			{
				std::string	str = szFile;
				str += " LoadTemplate Failed";
				logger( str.c_str() );
				return LE_READFAILD;
			}

			// Sequence Number 설정한다.
			{
				SequenceNumberManager::stSequenceNumber	* pSeq = cSeqManager.GetAttachedData( pcsTemplate );
				pSeq->SetNumber( nSequence );
			}
		}

		return LE_NOERROR;
	}

	template< typename T >
	BOOL	LoaderBase< T >::SaveTemplateOne( const char * pPath , T * pcsTemplate )
	{
		// Sequence Number가 없으면 저장 할 수 없음.
		// Sequence Number는 최초에 Seperated로 저장후 , 다시 로드한경우 생성됨.
		SequenceNumberManager &cSeqManager = SequenceNumberManager::GetInstance();
		SequenceNumberManager::stSequenceNumber	* pSeq = cSeqManager.GetAttachedData( pcsTemplate );
		if( !pSeq->GetNumber() )
			return FALSE;

		const BOOL	bEncryption = FALSE;

		ApModuleStream		csStream;
		csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
		if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

		CHAR				szTID[32];
		sprintf(szTID, "%d", pcsTemplate->GetID() );

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		if( !SaveTemplate( csStream , pcsTemplate ) )
			return FALSE;

		char	szFile[ MAX_PATH ] ;
		sprintf( szFile , c_strTemplateFormat , pPath , pcsTemplate->GetID() , pSeq->GetNumber() );
		if( !csStream.Write(szFile, 0, bEncryption) ) return FALSE;

		return TRUE;
	}


	// Utility Functions
	template< typename StringLogger >
	inline ReadError	LoadObjectTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		ObjectLoader	cLoader( stModuleManager );
		return cLoader.LoadTemplates( pPath , logger );
	}
	template< typename StringLogger >
	inline BOOL			SaveObjectTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		ObjectLoader	cLoader( stModuleManager );
		return cLoader.SaveTemplates( pPath , logger );
	}
	template< typename StringLogger >
	inline ReadError	LoadCharacterTemplateSeperated	( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		CharacterLoader	cLoader( stModuleManager );
		return cLoader.LoadTemplates( pPath , logger );
	}
	template< typename StringLogger >
	inline BOOL			SaveCharacterTemplateSeperated	( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		CharacterLoader	cLoader( stModuleManager );
		return cLoader.SaveTemplates( pPath , logger );
	}
	template< typename StringLogger >
	inline ReadError	LoadItemTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		ItemLoader	cLoader( stModuleManager );
		return cLoader.LoadTemplates( pPath , logger );
	}
	template< typename StringLogger >
	inline BOOL			SaveItemTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath , StringLogger logger )
	{
		ItemLoader	cLoader( stModuleManager );
		return cLoader.SaveTemplates( pPath , logger );
	}

	inline ReadError	LoadObjectTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath ) { return LoadObjectTemplateSeperated		( stModuleManager , pPath , DummyLogger() ); }
	inline BOOL			SaveObjectTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath ) { return SaveObjectTemplateSeperated		( stModuleManager , pPath , DummyLogger() ); }
	inline ReadError	LoadCharacterTemplateSeperated	( ApModuleManager &stModuleManager , const char * pPath ) { return LoadCharacterTemplateSeperated	( stModuleManager , pPath , DummyLogger() ); }
	inline BOOL			SaveCharacterTemplateSeperated	( ApModuleManager &stModuleManager , const char * pPath ) { return SaveCharacterTemplateSeperated	( stModuleManager , pPath , DummyLogger() ); }
	inline ReadError	LoadItemTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath ) { return LoadItemTemplateSeperated		( stModuleManager , pPath , DummyLogger() ); }
	inline BOOL			SaveItemTemplateSeperated		( ApModuleManager &stModuleManager , const char * pPath ) { return SaveItemTemplateSeperated		( stModuleManager , pPath , DummyLogger() ); }

	template< typename T >
	void SetDefaultSequenceNumber( T * pcsTemplate )
	{
		SequenceNumberManager &cSeqManager = SequenceNumberManager::GetInstance();
		SequenceNumberManager::stSequenceNumber	* pSeq = cSeqManager.GetAttachedData( pcsTemplate );
		pSeq->SetNumber( pcsTemplate->m_lID );	// TID로 디폴트 설정..
	}
};

class	CompatibleStringLogger
{
public:
	CompatibleStringLogger( AcCallbackData1 pfOutputCallback ): _pCallback( pfOutputCallback ) {}

	bool	operator()( const char * pMessage )
	{
		if( _pCallback ) _pCallback( ( void * ) pMessage );
		return true;
	}

protected:
	AcCallbackData1	_pCallback;
};