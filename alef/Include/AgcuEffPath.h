#if !defined(AFX_AGCUEFFPATH_H__C7A0A63A_82B3_42A7_AFFE_4A09DB615BBF__INCLUDED_)
#define AFX_AGCUEFFPATH_H__C7A0A63A_82B3_42A7_AFFE_4A09DB615BBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"

enum {
	E_EFFPATH_INI		= 0,
	E_EFFPATH_TEX,
	E_EFFPATH_ANIM,
	E_EFFPATH_CLUMP,
	E_EFFPATH_SOUND,
	E_EFFPATH_SOUNDMONO,
	E_EFFPATH_SOUNDSTREAM,
	E_EFFPATH_NUM,
};

class AgcuEffPath  
{
private:
	AgcuEffPath()			{		}
	virtual ~AgcuEffPath()	{		}

public:
	static const RwChar* const	GetPath					( INT32 nIndex );

	static const RwChar* const	GetPath_Ini				( VOID );
	static const RwChar* const	GetPath_Tex				( VOID );
	static const RwChar* const	GetPath_Anim			( VOID );
	static const RwChar* const	GetPath_Clump			( VOID );
	static const RwChar* const	GetPath_Sound			( VOID );
	static const RwChar* const	GetPath_SoundMono		( VOID );
	static const RwChar* const	GetPath_SoundStream		( VOID );

	static void					SetPath_Ini				( const RwChar* szIni			);
	static void					SetPath_Tex				( const RwChar* szTex			);
	static void					SetPath_Anim			( const RwChar* szAnim			);
	static void					SetPath_Clump			( const RwChar* szClump			);
	static void					SetPath_Sound			( const RwChar* szSound			);
	static void					SetPath_SoundMono		( const RwChar* szSoundMono		);
	static void					SetPath_SoundStream		( const RwChar* szSoundStream	);

private:
	static const RwChar* m_szEffPath[E_EFFPATH_NUM];
};

#endif