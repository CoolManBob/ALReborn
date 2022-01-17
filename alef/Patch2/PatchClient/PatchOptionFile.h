#ifndef __PATCH_OPTION_FILE__
#define __PATCH_OPTION_FILE__

#include <string>

#include "AgcmUIOptionDefine.h"

class CPatchOptionResolution
{
public:
	CPatchOptionResolution()
	{
		Reset();
	}

	CPatchOptionResolution( int nWidth, int nHeight, int nBit ) : m_iWidth(nWidth), m_iHeight(nHeight), m_iBit(nBit)
	{
	}

	bool operator == ( CPatchOptionResolution& lhs )	{
		return m_iWidth == lhs.m_iWidth && m_iHeight == lhs.m_iHeight && m_iBit == lhs.m_iBit ? true : false;
	}

	void Reset()
	{
		m_iWidth	= 1024;
		m_iHeight	= 768;
		m_iBit		= 32;
	}

	int		m_iWidth;
	int		m_iHeight;
	int		m_iBit;

};

enum	eInterfaceStyle
{
	eInterfaceStyle1 = 0,
	eInterfaceStyle2
};

enum	eWindowedMode
{	
	eWMODE_OFF = 0,
	eWMODE_ON
};

struct	OptionPreSet
{
	AgcdUIOptionView			eCharacter;
	AgcdUIOptionView			eMap;

	AgcdUIOptionEffect			eNature;
	AgcdUIOptionEffect			eEffect;
	AgcdUIOptionEffect			eShadow;
	AgcdUIOptionEffect			eWater;

	AgcdUIOptionToggle			eAutoExposure;
	AgcdUIOptionToggle			eHighTexture;
	AgcdUIOptionToggle			eImpact;
	AgcdUIOptionEffect2			eBloomType;
};

struct stOptionSound
{
	BOOL						m_bIsEnableBGM;
	int							m_nVolumeBGM;

	BOOL						m_bIsEnableEnv;
	int							m_nVolumeEnv;

	BOOL						m_bIsEnableEffect;
	int							m_nVolumeEffect;

	int							m_nSpeakerType;
	char						m_strProvider[ 128 ];

	BOOL						m_bIsEnableEAX;

	stOptionSound( void )
	{
		m_bIsEnableBGM = TRUE;
		m_nVolumeBGM = 100;

		m_bIsEnableEnv = TRUE;
		m_nVolumeEnv = 100;

		m_bIsEnableEffect = TRUE;
		m_nVolumeEffect = 100;

		m_nSpeakerType = 0;
		memset( m_strProvider, 0, sizeof( char ) * 128 );

		m_bIsEnableEAX = TRUE;
	}
};

struct stOptionView
{
	BOOL					m_bIsViewSelf;
	BOOL					m_bIsViewMonster;
	BOOL					m_bIsViewGuildMember;
	BOOL					m_bIsViewPartyMember;
	BOOL					m_bIsViewAllPlayer;

	BOOL					m_bIsViewHPBar;
	BOOL					m_bIsViewPartySkill;
	BOOL					m_bIsViewHelmet;
	BOOL					m_bIsViewDropItem;

	BOOL					m_bViewBalloonHelp;
	BOOL					m_bViewBalloonChat;
	BOOL					m_bViewGameTip;

	BOOL					m_bViewVillen;
	BOOL					m_bViewHostilGuild;
	BOOL					m_bViewAttacker;

	stOptionView( void )
	{
		m_bIsViewSelf = TRUE;
		m_bIsViewMonster = TRUE;
		m_bIsViewGuildMember = TRUE;
		m_bIsViewPartyMember = TRUE;
		m_bIsViewAllPlayer = TRUE;

		m_bIsViewHPBar = TRUE;
		m_bIsViewPartySkill = TRUE;
		m_bIsViewHelmet = TRUE;
		m_bIsViewDropItem = TRUE;

		m_bViewBalloonHelp = TRUE;
		m_bViewBalloonChat = TRUE;
		m_bViewGameTip = TRUE;

		m_bViewVillen = TRUE;
		m_bViewHostilGuild = TRUE;
		m_bViewAttacker = TRUE;
	}
};

struct stOptionPrivate
{
	BOOL					m_bRejectTrade;
	BOOL					m_bRejectInviteParty;
	BOOL					m_bRejectInviteGuild;
	BOOL					m_bRejectInviteBuddy;
	BOOL					m_bRejectGuildBattle;
	BOOL					m_bRejectGuildRelation;

	stOptionPrivate( void )
	{
		m_bRejectTrade = FALSE;
		m_bRejectInviteParty = FALSE;
		m_bRejectInviteGuild = FALSE;
		m_bRejectInviteBuddy = FALSE;
		m_bRejectGuildBattle = FALSE;
		m_bRejectGuildRelation = FALSE;
	}
};

class CPatchOptionFile
{
public:
	CPatchOptionResolution	m_cResolution;

	int						m_lTextureQualityChar;
	int						m_lTextureQualityObj;
	int						m_lTextureQualityEff;
	int						m_lTextureQualityMap;

	char					m_lTextureQuality;

	eWindowedMode			m_eWindowedMode;

	AgcdUIOptionView		m_eCharacter;
	AgcdUIOptionView		m_eMap;

	AgcdUIOptionEffect		m_eNature;
	AgcdUIOptionEffect		m_eEffect;
	AgcdUIOptionEffect		m_eShadow;
	AgcdUIOptionEffect		m_eWater;

	AgcdUIOptionToggle		m_eAutoExposure;
	AgcdUIOptionToggle		m_eHighTexture;
	AgcdUIOptionToggle		m_eImpact;
	AgcdUIOptionEffect2		m_eBloomType;

	bool					m_bPatchFirstStart;			// Patch가 처음 시작하는 건지..

	int						m_iCurPresetID;
	int						m_iPreferPresetID;			// Card에 맞는 권장 preset option id
	OptionPreSet			m_stPresetInfo[9];

	// 저장을 위해서 갖고 있자
	int						m_iMaxMouseSpeed;
	int						m_iCurMouseSpeed;
	int						m_iMaxBright;
	int						m_iCurBright;
	int						m_iGlowIntensity;
	AgcdUIOptionToggle		m_eMultiSampling;
	AgcdUIOptionToggle		m_eAutoTuning;

	
	BOOL					m_bSaveLoad;		// CommandLine에서 넘어오는 옵션에 관련된 내용
	std::string				m_strNoticeURL;
	std::string				m_strRegistry;

protected:
	AgcmOptionStartup		m_eStartupMode;

private :
	stOptionSound			m_OptionSound;
	stOptionView			m_OptionView;
	stOptionPrivate			m_OptionPrivate;

public:
	CPatchOptionFile();
	~CPatchOptionFile();

	void Init();
	bool Load( char *pstrFileName );
	bool Save( char *pstrFileName );

	void	SetDefaultOption( char *pstrFileName, BOOL bSaveFile = TRUE );
	void	SetResolution( CPatchOptionResolution* pcResolution )	{
		if( !pcResolution )			return;

		m_cResolution.m_iWidth	= pcResolution->m_iWidth;
		m_cResolution.m_iHeight	= pcResolution->m_iHeight;
		m_cResolution.m_iBit	= pcResolution->m_iBit;
	}
	void	SetTextureQuality( int lTextureQuality )	{	m_lTextureQuality = lTextureQuality;	}
	void	SetWindowedMode( eWindowedMode eWinMode)	{	m_eWindowedMode = eWinMode;	}
	void	SetStartupMode(AgcmOptionStartup eMode);
	void	SetNoticeURL(const char* strURL)			{	m_strNoticeURL = strURL;	}

	CPatchOptionResolution*	GetResolution()	{	return &m_cResolution;		}
	int				GetTextureQuality()		{	return m_lTextureQuality;	}
	eWindowedMode	GetWindowedMode()		{	return m_eWindowedMode;		}
	AgcmOptionStartup GetStartupMode()		{	return m_eStartupMode;		}
	const char*		GetNoticeURL()			{	return m_strNoticeURL.c_str();	}

private :
	void			_LoadOptionSound		( char* pFileName );
	void			_LoadOptionView			( char* pFileName );
	void			_LoadOptionPrivate		( char* pFileName );

	void			_SaveOptionSound		( char* pFileName );
	void			_SaveOptionView			( char* pFileName );
	void			_SaveOptionPrivate		( char* pFileName );
};

#endif
