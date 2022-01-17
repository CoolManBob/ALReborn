#include "stdafx.h"

#include "PatchOptionFile.h"

#include "d3d9.h"
#include "ApModule.h"
#include "ApModuleStream.h"
#include "AutoDetectMemoryLeak.h"

#include "BasePatchClient.h"


CPatchOptionFile::CPatchOptionFile()
{
	m_lTextureQualityChar = 0;
	m_lTextureQualityObj = 0;
	m_lTextureQualityEff = 0;
	m_lTextureQualityMap = 0;

	m_lTextureQuality = 0;

	m_eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_eMap = AGCD_OPTION_VIEW_NORMAL;

	m_eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_eEffect = AGCD_OPTION_EFFECT_MEDIUM;
	m_eShadow = AGCD_OPTION_EFFECT_MEDIUM;
	m_eWater = AGCD_OPTION_EFFECT_MEDIUM;

	m_eWindowedMode = eWMODE_OFF;

	m_eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_eHighTexture = AGCD_OPTION_TOGGLE_OFF;
	m_eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_iPreferPresetID = 3;

	m_bPatchFirstStart = TRUE;
	m_iCurPresetID = -1;

	m_iMaxMouseSpeed = 20;
	m_iCurMouseSpeed = 10;
	m_iMaxBright = 100;
	m_iCurBright = 50;
	m_iGlowIntensity = 20;
	m_eMultiSampling = AGCD_OPTION_TOGGLE_OFF;
	m_eAutoTuning = AGCD_OPTION_TOGGLE_OFF;

	// 옵션 초기화
	m_stPresetInfo[0].eCharacter = AGCD_OPTION_VIEW_NEAR;
	m_stPresetInfo[0].eMap = AGCD_OPTION_VIEW_NEAR;
	m_stPresetInfo[0].eNature = AGCD_OPTION_EFFECT_OFF;
	m_stPresetInfo[0].eEffect = AGCD_OPTION_EFFECT_OFF;
	m_stPresetInfo[0].eShadow = AGCD_OPTION_EFFECT_OFF;
	m_stPresetInfo[0].eWater = AGCD_OPTION_EFFECT_OFF;
	m_stPresetInfo[0].eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[0].eHighTexture = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[0].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[0].eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_stPresetInfo[1].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[1].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[1].eNature = AGCD_OPTION_EFFECT_OFF;
	m_stPresetInfo[1].eEffect = AGCD_OPTION_EFFECT_LOW;
	m_stPresetInfo[1].eShadow = AGCD_OPTION_EFFECT_LOW;
	m_stPresetInfo[1].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[1].eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[1].eHighTexture = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[1].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[1].eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_stPresetInfo[2].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[2].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[2].eNature = AGCD_OPTION_EFFECT_LOW;
	m_stPresetInfo[2].eEffect = AGCD_OPTION_EFFECT_LOW;
	m_stPresetInfo[2].eShadow = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[2].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[2].eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[2].eHighTexture = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[2].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[2].eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_stPresetInfo[3].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[3].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[3].eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[3].eEffect = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[3].eShadow = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[3].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[3].eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[3].eHighTexture = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[3].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[3].eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_stPresetInfo[4].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[4].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[4].eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[4].eEffect = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[4].eShadow = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[4].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[4].eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[4].eHighTexture = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[4].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[4].eBloomType = AGCD_OPTION_EFFECT2_OFF;

	m_stPresetInfo[5].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[5].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[5].eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[5].eEffect = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[5].eShadow = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[5].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[5].eAutoExposure = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[5].eHighTexture = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[5].eImpact = AGCD_OPTION_TOGGLE_OFF;
	m_stPresetInfo[5].eBloomType = AGCD_OPTION_EFFECT2_TYPE1;

	m_stPresetInfo[6].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[6].eMap = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[6].eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[6].eEffect = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[6].eShadow = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[6].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[6].eAutoExposure = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[6].eHighTexture = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[6].eImpact = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[6].eBloomType = AGCD_OPTION_EFFECT2_TYPE1;

	m_stPresetInfo[7].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[7].eMap = AGCD_OPTION_VIEW_FAR;
	m_stPresetInfo[7].eNature = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[7].eEffect = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[7].eShadow = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[7].eWater = AGCD_OPTION_EFFECT_MEDIUM;
	m_stPresetInfo[7].eAutoExposure = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[7].eHighTexture = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[7].eImpact = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[7].eBloomType = AGCD_OPTION_EFFECT2_TYPE1;

	m_stPresetInfo[8].eCharacter = AGCD_OPTION_VIEW_NORMAL;
	m_stPresetInfo[8].eMap = AGCD_OPTION_VIEW_FAR;
	m_stPresetInfo[8].eNature = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[8].eEffect = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[8].eShadow = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[8].eWater = AGCD_OPTION_EFFECT_HIGH;
	m_stPresetInfo[8].eAutoExposure = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[8].eHighTexture = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[8].eImpact = AGCD_OPTION_TOGGLE_ON;
	m_stPresetInfo[8].eBloomType = AGCD_OPTION_EFFECT2_TYPE1;

	m_bSaveLoad		= TRUE;
	m_eStartupMode	= AGCM_OPTION_STARTUP_NORMAL;
	m_strRegistry	= REG_KEY_NAME_ARCHLORD;

	#ifdef _KOR
	{
		#ifdef _TEST_SERVER_
		m_strNoticeURL	= "http://cms.hangame.com/bbs.nhn?gameid=ARCHLORD_GAMENOTICE";
		m_strRegistry	= REG_KEY_NAME_ARCHLORD
		#endif


		FILE* pFile = fopen( ".\\ini\\archtest.ini", "r" );
		if( pFile )
		{
			char strBuffer[512];
			fscanf(pFile, "%s", strBuffer);			//. REG_KEY_NAME_ARCHLORD
			m_strRegistry = strBuffer;

			fscanf(pFile, "%s", strBuffer);			//. Patch2.archlord.com
			fscanf(pFile, "%s", strBuffer);			//. Login2.archlord.com
			fscanf(pFile, "%s", strBuffer);			//. http://archlord.naver.com/bbsList.nhn?bbsmode=testnotice&ver=3
			m_strNoticeURL.assign(strBuffer);

			fclose( pFile );
		}
		else
			m_strNoticeURL	= "http://cms.hangame.com/bbs.nhn?gameid=ARCHLORD_GAMENOTICE";
	}
	#elif _ENG
	{
		m_strNoticeURL = "http://www.archlordgame.com/launcher/right.php";
	}
	#elif _CHN
	{
		m_strNoticeURL = "http://ac.sdo.com/client/news/";
	}
	#elif _JPN
	{
		m_strNoticeURL = "http://archlord.hangame.co.jp/gamebbs.nhn?gameid=ARCHLORD_GM_NOTICE";
	}
	#elif _TIW
	{
		m_strNoticeURL = "http://naver.com";
	}
	#endif
}

CPatchOptionFile::~CPatchOptionFile()
{
}

void CPatchOptionFile::Init()
{
	// preset 정보 초기화
	LPDIRECT3D9 pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !pD3D9 )			return;

	D3DADAPTER_IDENTIFIER9	pIdentifier;
	pD3D9->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &pIdentifier );
	pD3D9->Release();

	int MyVendorID	= pIdentifier.VendorId;
	int	MyDeviceID	= pIdentifier.DeviceId;
	int MyProduct	= HIWORD(pIdentifier.DriverVersion.HighPart);
	int MyVersion	= LOWORD(pIdentifier.DriverVersion.HighPart);
	int MySubVersion	= HIWORD(pIdentifier.DriverVersion.LowPart);
	int MyBuild		= LOWORD(pIdentifier.DriverVersion.LowPart);

	if( MyBuild <= 0 )		return;

	ApModuleStream	csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );
	csStream.ReadSectionName(0);
	csStream.SetValueID(-1);

#if defined(_TIW) || defined(_CHN)
	BOOL bIsEncrypt = FALSE;
#else
	BOOL bIsEncrypt = TRUE;
#endif

	BOOL bOpenResult = csStream.Open( "Ini\\OptionInit.Ini", 0 , bIsEncrypt );
	if( !bOpenResult )		return;

	BOOL	bCorrect;
	int		temp;
	CHAR	szValue[256];

	while( csStream.ReadNextValue() )
	{
		const CHAR* szValueName = csStream.GetValueName();

		if( !strcmp( szValueName, "Vendor ID " ) )
		{
			csStream.GetValue( szValue, 256) ;
			sscanf( szValue, "%d", &temp );

			bCorrect = ( temp == MyVendorID ) ? TRUE : FALSE;
		}
		else if( !strcmp( szValueName, "Device ID " ) )
		{
			csStream.GetValue( szValue, 256 );
			sscanf( szValue, "%d", &temp );

			bCorrect = ( temp == MyDeviceID ) ? TRUE : FALSE;
		}
		else if( !strcmp( szValueName, "Setting " ) )
		{
			csStream.GetValue( szValue, 256 );
			sscanf( szValue,"%d",&temp );

			if( bCorrect )
				m_iPreferPresetID = max( 0, min( temp, 8 ) );
		}
	}
}

bool CPatchOptionFile::Load( char *pstrFileName )
{
	if( !m_bSaveLoad )		return true;

	FILE* file = fopen( pstrFileName, "rb" );
	if( !file )
	{
		SetDefaultOption( PATCHCLIENT_OPTION_FILENAME );
		return false;
	}

	fclose( file );

	bool	bResult = true;
	char	strBuffer[80];

	if( GetPrivateProfileString( "Patch_Option", "Width", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_cResolution.m_iWidth = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "Height", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_cResolution.m_iHeight = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "Bit", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_cResolution.m_iBit = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "OptionAutoInitialized", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
	{
		int temp = atoi(strBuffer);
		if(temp) m_bPatchFirstStart = FALSE;
	}
	else
		bResult = false;

	//Texure Quality Setting
	if( GetPrivateProfileString( "Patch_Option", "TexCharacter", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
	{
		m_lTextureQualityChar = atoi(strBuffer);
		m_lTextureQuality = atoi(strBuffer);
	}
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "TexObject", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_lTextureQualityObj = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "TexEffect", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_lTextureQualityEff = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "TexWorld", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_lTextureQualityMap = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Patch_Option", "Windowed", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eWindowedMode = (eWindowedMode) atoi(strBuffer);
	else
		bResult = false;

	// 게임내 옵션 읽기
	if( GetPrivateProfileString( "Video_Option", "CharView", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eCharacter = (AgcdUIOptionView)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "MapView", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eMap = (AgcdUIOptionView)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Nature", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eNature = (AgcdUIOptionEffect)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Shadow", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eShadow = (AgcdUIOptionEffect)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Effect", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eEffect = (AgcdUIOptionEffect)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Water", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eWater = (AgcdUIOptionEffect)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "AutoExposure", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eAutoExposure = (AgcdUIOptionToggle)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "MatEffect", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eHighTexture = (AgcdUIOptionToggle)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "ImpactFX", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eImpact = (AgcdUIOptionToggle)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Bloom", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eBloomType = (AgcdUIOptionEffect2)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "BRIGHT", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		sscanf(strBuffer,"%d:%d",&m_iMaxBright,&m_iCurBright);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "GlowIntensity", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_iGlowIntensity = atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "Multisampling", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eMultiSampling = (AgcdUIOptionToggle)atoi(strBuffer);
	else
		bResult = false;

	if( GetPrivateProfileString( "Video_Option", "AUTOTUNING", NULL, strBuffer, sizeof(strBuffer), pstrFileName ) )
		m_eAutoTuning = (AgcdUIOptionToggle)atoi(strBuffer);
	else
		bResult = false;
	
	_LoadOptionSound( pstrFileName );
	_LoadOptionView( pstrFileName );
	_LoadOptionPrivate( pstrFileName );
	
	if( !bResult || m_bPatchFirstStart )		//뭐라도 하나 읽다가 뻑나면?
		SetDefaultOption( PATCHCLIENT_OPTION_FILENAME );		//Patch_Option 관련 부분만 초기화한다.

	return bResult;
}

bool CPatchOptionFile::Save( char *pstrFileName )
{
	if( !m_bSaveLoad )		return true;

	CreateDirectory( "ini", NULL );

	char	strBuffer[255];

	//Patch Option Section.
	WritePrivateProfileString( "Patch_Option", NULL, NULL, pstrFileName );

	sprintf( strBuffer, "%d", m_cResolution.m_iWidth );
	WritePrivateProfileString( "Patch_Option", "Width", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_cResolution.m_iHeight );
	WritePrivateProfileString( "Patch_Option", "Height", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_cResolution.m_iBit );
	WritePrivateProfileString( "Patch_Option", "Bit", strBuffer, pstrFileName );
	
	m_lTextureQualityChar	= m_lTextureQuality;
	m_lTextureQualityObj	= m_lTextureQuality;
	m_lTextureQualityEff	= m_lTextureQuality;
	m_lTextureQualityMap	= m_lTextureQuality;

	sprintf( strBuffer, "%d", m_lTextureQualityChar );
	WritePrivateProfileString( "Patch_Option", "TexCharacter", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_lTextureQualityObj );
	WritePrivateProfileString( "Patch_Option", "TexObject", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_lTextureQualityEff );
	WritePrivateProfileString( "Patch_Option", "TexEffect", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_lTextureQualityMap );
	WritePrivateProfileString( "Patch_Option", "TexWorld", strBuffer, pstrFileName );

	sprintf( strBuffer, "1");
	WritePrivateProfileString( "Patch_Option", "OptionAutoInitialized", strBuffer, pstrFileName );

	sprintf( strBuffer, "%d", m_eWindowedMode);
	WritePrivateProfileString( "Patch_Option", "Windowed", strBuffer, pstrFileName );	
	
	// 게임내 옵션 저장
	if( m_iCurPresetID != -1 )
	{
		WritePrivateProfileString( "Video_Option", NULL, NULL, pstrFileName );

		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eCharacter );
		WritePrivateProfileString( "Video_Option", "CharView", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eMap );
		WritePrivateProfileString( "Video_Option", "MapView", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eNature );
		WritePrivateProfileString( "Video_Option", "Nature", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eEffect );
		WritePrivateProfileString( "Video_Option", "Effect", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eShadow );
		WritePrivateProfileString( "Video_Option", "Shadow", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eWater );
		WritePrivateProfileString( "Video_Option", "Water", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eAutoExposure );
		WritePrivateProfileString( "Video_Option", "AutoExposure", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eHighTexture );
		WritePrivateProfileString( "Video_Option", "MatEffect", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eImpact );
		WritePrivateProfileString( "Video_Option", "ImpactFX", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_stPresetInfo[m_iCurPresetID].eBloomType );
		WritePrivateProfileString( "Video_Option", "Bloom", strBuffer, pstrFileName );

		// 기타 Video_Option 저장
		sprintf( strBuffer, "%d:%d",m_iMaxBright,m_iCurBright  );
		WritePrivateProfileString( "Video_Option", "BRIGHT", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d:%d", m_iMaxMouseSpeed,m_iCurMouseSpeed );
		WritePrivateProfileString( "Video_Option", "GlowIntensity", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eMultiSampling );
		WritePrivateProfileString( "Video_Option", "Multisampling", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eAutoTuning );
		WritePrivateProfileString( "Video_Option", "AUTOTUNING", strBuffer, pstrFileName );

		m_eCharacter= m_stPresetInfo[m_iCurPresetID].eCharacter;
		m_eMap		= m_stPresetInfo[m_iCurPresetID].eMap;

		m_eNature	= m_stPresetInfo[m_iCurPresetID].eNature;
		m_eEffect	= m_stPresetInfo[m_iCurPresetID].eEffect;
		m_eShadow	= m_stPresetInfo[m_iCurPresetID].eShadow;
		m_eWater	= m_stPresetInfo[m_iCurPresetID].eWater;

		m_eAutoExposure	= m_stPresetInfo[m_iCurPresetID].eAutoExposure;
		m_eHighTexture	= m_stPresetInfo[m_iCurPresetID].eHighTexture;
		m_eImpact		= m_stPresetInfo[m_iCurPresetID].eImpact;
		m_eBloomType	= m_stPresetInfo[m_iCurPresetID].eBloomType;
	}

	_SaveOptionSound( pstrFileName );
	_SaveOptionView( pstrFileName );
	_SaveOptionPrivate( pstrFileName );

	return true;
}

void CPatchOptionFile::SetDefaultOption( char *pstrFileName, BOOL bSaveFile )
{
	CreateDirectory( "ini", NULL );

	//우선 디폴트값을 세팅한다.
	m_cResolution.m_iWidth	= 1024;
	m_cResolution.m_iHeight	= 768;
	m_cResolution.m_iBit	= 32;

	m_eCharacter	= m_stPresetInfo[m_iPreferPresetID].eCharacter;
	m_eMap			= m_stPresetInfo[m_iPreferPresetID].eMap;

	m_eNature		= m_stPresetInfo[m_iPreferPresetID].eNature;
	m_eEffect		= m_stPresetInfo[m_iPreferPresetID].eEffect;
	m_eShadow		= m_stPresetInfo[m_iPreferPresetID].eShadow;
	m_eWater		= m_stPresetInfo[m_iPreferPresetID].eWater;

	m_eAutoExposure	= m_stPresetInfo[m_iPreferPresetID].eAutoExposure;
	m_eHighTexture	= m_stPresetInfo[m_iPreferPresetID].eHighTexture;
	m_eImpact		= m_stPresetInfo[m_iPreferPresetID].eImpact;
	m_eBloomType	= m_stPresetInfo[m_iPreferPresetID].eBloomType;

	if( bSaveFile && m_bSaveLoad )
	{
		char	strBuffer[255];

		//Patch Section
		WritePrivateProfileString( "Patch_Option", NULL, NULL, pstrFileName );

		sprintf( strBuffer, "%d", m_cResolution.m_iWidth );
		WritePrivateProfileString( "Patch_Option", "Width", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_cResolution.m_iHeight );
		WritePrivateProfileString( "Patch_Option", "Height", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_cResolution.m_iBit );
		WritePrivateProfileString( "Patch_Option", "Bit", strBuffer, pstrFileName );

		//텍스쳐 퀄리티 세팅.
		sprintf( strBuffer, "%d", m_lTextureQualityChar );
		WritePrivateProfileString( "Patch_Option", "TexCharacter", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_lTextureQualityObj );
		WritePrivateProfileString( "Patch_Option", "TexObject", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_lTextureQualityEff );
		WritePrivateProfileString( "Patch_Option", "TexEffect", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_lTextureQualityMap );
		WritePrivateProfileString( "Patch_Option", "TexWorld", strBuffer, pstrFileName );

		sprintf( strBuffer, "1");
		WritePrivateProfileString( "Patch_Option", "OptionAutoInitialized", strBuffer, pstrFileName );

		// 게임내 옵션 저장
		sprintf( strBuffer, "%d", m_eCharacter );
		WritePrivateProfileString( "Video_Option", "CharView", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eMap );
		WritePrivateProfileString( "Video_Option", "MapView", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_eNature );
		WritePrivateProfileString( "Video_Option", "Nature", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eEffect );
		WritePrivateProfileString( "Video_Option", "Effect", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eShadow );
		WritePrivateProfileString( "Video_Option", "Shadow", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eWater );
		WritePrivateProfileString( "Video_Option", "Water", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_eAutoExposure );
		WritePrivateProfileString( "Video_Option", "AutoExposure", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eHighTexture );
		WritePrivateProfileString( "Video_Option", "MatEffect", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eImpact );
		WritePrivateProfileString( "Video_Option", "ImpactFX", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eBloomType );
		WritePrivateProfileString( "Video_Option", "Bloom", strBuffer, pstrFileName );

		// 기타 Video_Option 저장
		sprintf( strBuffer, "%d:%d",m_iMaxBright,m_iCurBright  );
		WritePrivateProfileString( "Video_Option", "BRIGHT", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d:%d", m_iMaxMouseSpeed,m_iCurMouseSpeed );
		WritePrivateProfileString( "Video_Option", "GlowIntensity", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eMultiSampling );
		WritePrivateProfileString( "Video_Option", "Multisampling", strBuffer, pstrFileName );
		sprintf( strBuffer, "%d", m_eAutoTuning );
		WritePrivateProfileString( "Video_Option", "AUTOTUNING", strBuffer, pstrFileName );

		sprintf( strBuffer, "%d", m_eWindowedMode);
		WritePrivateProfileString( "Patch_Option", "Windowed", strBuffer, pstrFileName );
	}
}

void CPatchOptionFile::SetStartupMode(AgcmOptionStartup eMode)
{
	m_eStartupMode = eMode;

	switch (m_eStartupMode)
	{
	case AGCM_OPTION_STARTUP_LOW:
		m_iPreferPresetID = 0;
		m_lTextureQuality = m_lTextureQualityChar = m_lTextureQualityObj = m_lTextureQualityEff = m_lTextureQualityMap = 2;
		SetDefaultOption(PATCHCLIENT_OPTION_FILENAME, FALSE);
		break;

	case AGCM_OPTION_STARTUP_HIGH:
		m_iPreferPresetID = 8;
		m_lTextureQuality = m_lTextureQualityChar = m_lTextureQualityObj = m_lTextureQualityEff = m_lTextureQualityMap = 0;
		SetDefaultOption(PATCHCLIENT_OPTION_FILENAME, FALSE);
		break;
	}
}

void CPatchOptionFile::_LoadOptionSound( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;

	char strBuffer[ 128 ] = { 0, };

	int nCur = 0;
	int nMax = 100;

	if( ::GetPrivateProfileString( "Sound_Option", "BGM_ENABLE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionSound.m_bIsEnableBGM = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "BGM_VOLUME", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		sscanf( strBuffer, "%d:%d", &nCur, &nMax );
		m_OptionSound.m_nVolumeBGM = nCur;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "ES_ENABLE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionSound.m_bIsEnableEffect = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "ES_VOLUME", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		sscanf( strBuffer, "%d:%d", &nCur, &nMax );
		m_OptionSound.m_nVolumeEffect = nCur;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "ENV_ENABLE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionSound.m_bIsEnableEnv = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "ENV_VOLUME", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		sscanf( strBuffer, "%d:%d", &nCur, &nMax );
		m_OptionSound.m_nVolumeEnv = nCur;
	}

	if( ::GetPrivateProfileString( "Sound_Option", "SPEAKER_TYPE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionSound.m_nSpeakerType = atoi( strBuffer );
	}

	if( ::GetPrivateProfileString( "Sound_Option", "3DPROVIDER", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		memset( m_OptionSound.m_strProvider, 0, sizeof( char ) * 128 );
		strcpy_s( m_OptionSound.m_strProvider, sizeof( char ) * 128, strBuffer );
	}

	if( ::GetPrivateProfileString( "Sound_Option", "EAX_ENABLE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionSound.m_bIsEnableEAX = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}
}

void CPatchOptionFile::_LoadOptionView( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;
	char strBuffer[ 128 ] = { 0, };

	if( ::GetPrivateProfileString( "ETC_Option", "VN_MINE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewSelf = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VN_MONSTER", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewMonster = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VN_GUILD", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewGuildMember = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VN_PARTY", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewPartyMember = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VN_OTHERS", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewAllPlayer = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VH_TIP", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewGameTip = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VH_BALOONCHAT", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewBalloonChat = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VH_UCLIE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewBalloonHelp = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VS_BARS", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewHPBar = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VS_PARTY", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewPartySkill = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VS_HELMET", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewHelmet = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VS_ITEM", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bIsViewDropItem = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VR_CRIMINAL", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewVillen = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VR_WANTED", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewHostilGuild = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "ETC_Option", "VS_MURDERER", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionView.m_bViewAttacker = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}
}

void CPatchOptionFile::_LoadOptionPrivate( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;
	char strBuffer[ 128 ] = { 0, };

	if( ::GetPrivateProfileString( "Option_Option", "REF_TRADE", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectTrade = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Option_Option", "REF_PIN", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectInviteParty = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Option_Option", "REF_GIN", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectInviteGuild = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Option_Option", "REF_GBAT", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectGuildBattle = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Option_Option", "REF_GR", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectGuildRelation = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}

	if( ::GetPrivateProfileString( "Option_Option", "REF_BUDDY", NULL, strBuffer, sizeof( char ) * 128, pFileName ) )
	{
		m_OptionPrivate.m_bRejectInviteBuddy = atoi( strBuffer ) != 0 ? TRUE : FALSE;
	}
}

void CPatchOptionFile::_SaveOptionSound( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;
	char strBuffer[ 128 ] = { 0, };

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionSound.m_bIsEnableBGM ? 1 : 0 );
	WritePrivateProfileString( "Sound_Option", "BGM_ENABLE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d:%d",	m_OptionSound.m_nVolumeBGM, 100 );
	WritePrivateProfileString( "Sound_Option", "BGM_VOLUME", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionSound.m_bIsEnableEffect ? 1 : 0 );
	WritePrivateProfileString( "Sound_Option", "ES_ENABLE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d:%d",	m_OptionSound.m_nVolumeEffect, 100 );
	WritePrivateProfileString( "Sound_Option", "ES_VOLUME", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionSound.m_bIsEnableEnv ? 1 : 0 );
	WritePrivateProfileString( "Sound_Option", "ENV_ENABLE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d:%d",	m_OptionSound.m_nVolumeEnv, 100 );
	WritePrivateProfileString( "Sound_Option", "ENV_VOLUME", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionSound.m_nSpeakerType );
	WritePrivateProfileString( "Sound_Option", "SPEAKER_TYPE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%s",	m_OptionSound.m_strProvider );
	WritePrivateProfileString( "Sound_Option", "3DPROVIDER", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionSound.m_bIsEnableEAX ? 1 : 0 );
	WritePrivateProfileString( "Sound_Option", "EAX_ENABLE", strBuffer, pFileName );
}

void CPatchOptionFile::_SaveOptionView( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;
	char strBuffer[ 128 ] = { 0, };

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewSelf ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VN_MINE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewMonster ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VN_MONSTER", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewGuildMember ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VN_GUILD", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewPartyMember ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VN_PARTY", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewAllPlayer ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VN_OTHERS", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewGameTip ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VH_TIP", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewBalloonChat ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VH_BALOONCHAT", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewBalloonHelp ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VH_UCLIE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewHPBar ? 1 : 0 );
	WritePrivateProfileString( "Sound_Option", "VS_BARS", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewPartySkill ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VS_PARTY", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewHelmet ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VS_HELMET", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bIsViewDropItem ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VS_ITEM", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewVillen ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VR_CRIMINAL", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewHostilGuild ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VR_WANTED", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionView.m_bViewAttacker ? 1 : 0 );
	WritePrivateProfileString( "ETC_Option", "VS_MURDERER", strBuffer, pFileName );
}

void CPatchOptionFile::_SaveOptionPrivate( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return;
	char strBuffer[ 128 ] = { 0, };

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectTrade ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_TRADE", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectInviteParty ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_PIN", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectInviteGuild ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_GIN", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectGuildBattle ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_GBAT", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectGuildRelation ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_GR", strBuffer, pFileName );

	memset( strBuffer, 0, sizeof( char ) * 128 );
	sprintf_s( strBuffer, "%d",	m_OptionPrivate.m_bRejectInviteBuddy ? 1 : 0 );
	WritePrivateProfileString( "Option_Option", "REF_BUDDY", strBuffer, pFileName );
}

