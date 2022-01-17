#pragma once

typedef std::vector< std::string >	StringVec;
typedef StringVec::iterator			StringVecItr;

enum eResourceSaver
{
	eResourceSaverCharacter,
	eResourceSaverItem,
	eResourceSaverObject,
	eResourceSaverMax,
};

enum eSaveClump
{
	eSaveClumpCharacter,
	eSaveClumpObject,
	eSaveClumpEffect,
	eSaveClumpMax,
};

class ApBase;
class CModelToolApp;
class AgcEngineChild;
class AgpmCharacter;
class AgcmCharacter;
class ApmObject;
class AgcmObject;
class AgpmItem;
class AgcmItem;
class AgcmPreLODManager;
class AgpmSkill;
class AgcmSkill;
class AgcmEventEffect;
class AgpdCharacterTemplate;
class AgcdCharacterTemplate;
class CResourceSaver
{
public:
	CResourceSaver();
	~CResourceSaver();

	BOOL	Initialize( AgcEngineChild* pEngine );
	void	Destory();

	BOOL	Create( int nID, eResourceSaver eMode = eResourceSaverCharacter );

protected:
	BOOL	CreateRoot( char* szName, char* szSaveDir );
	BOOL	CreateFolder( char* szSaveDir );

	void	SaveObjectGroup( AgcdObjectTemplateGroupList* pStart, char* szSrcDir, char* szDstDir );

	void	SaveIni( int nID, char* szSrcDir, char* szDstDir, eResourceSaver eMode );
	void	SaveAnimation( AgcdCharacterTemplate* pClientTemplate, char* szSrcDir, char* szDstDir );
	void	SaveSkill( AgpdCharacterTemplate* pPublicTemplate, AgcdCharacterTemplate* pClientTemplate, char* szSrcDir, char* szDstDir );
	void	SaveLod( ApBase* pBase, char* szSrcDir, char* szDstDir, eSaveClump eMode );
	void	SaveEffect( ApBase* pBase, char* szSrcDir, char* szDstDir );

	void	SaveFile( char* szName, char* szCurDir, char* szSrcDir, char* szSprintf );
	void	SaveClumpFile( char* szFilename, char* szCurDir, char* szSaveDir, eSaveClump eMode );
	void	SaveEffectFile( int nID, char* szCurDir, char* szSaveDir );

	void	SaveAniSound( AgcdAnimData2* pAniData, char* szSrcDir, char* szDstDir );
	void	SaveEffectSound( const char* szSoundName, const char* szSrcDir, const char* szDstDir );

	int		GetNumVisualInfo( AgcdCharacterTemplate* pTemplate );
	BOOL	GetTextureName( char* szClump, StringVec& vecString );
	BOOL	GetEffectTextureName( char* szClump, StringVec& vecString );	//Gloss Texture를 얻기위해서..

private:
	void	MessageBox( char* szName );

private:
	char				m_szDir[512];

	AgcEngineChild*		m_pEngine;
	AgpmCharacter*		m_pPublicCharacterMng;
	AgcmCharacter*		m_pClientCharacterMng;
	ApmObject*			m_pPublicObjectMng;
	AgcmObject*			m_pClientObjectMng;
	AgpmItem*			m_pPublicItemMng;
	AgcmItem*			m_pClientItemMng;
	AgcmPreLODManager*	m_pLodMng;
	AgpmSkill*			m_pPublicSkillMng;
	AgcmSkill*			m_pClientSkillMng;
	AgcmEventEffect*	m_pClientEffectMng;
	AgcaAnimation2*		m_pClientAnimationMng;
};