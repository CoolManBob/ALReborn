// AgcuEffPath.cpp: implementation of the AgcuEffPath class.
//
//////////////////////////////////////////////////////////////////////
#include "AgcuEffUtil.h"
#include "AgcuEffPath.h"

#include "ApMemoryTracker.h"

const RwChar* AgcuEffPath::m_szEffPath[]	= {
	  "EFFECT\\INI\\NEW\\"			//E_EFFPATH_INI		
	, "TEXTURE\\EFFECT\\"		//E_EFFPATH_TEX		
	, "EFFECT\\ANIMATION\\"		//E_EFFPATH_ANIM		
	, "EFFECT\\CLUMP\\"			//E_EFFPATH_CLUMP		
	, "SOUND\\EFFECT\\"			//E_EFFPATH_SOUND		
	, "SOUND\\EFFECT\\"			//E_EFFPATH_SOUNDMONO	
	, "SOUND\\STREAM\\"			//E_EFFPATH_SOUNDSTREAM
};

const RwChar* const	AgcuEffPath::GetPath(int nIndex)
{
	return nIndex < E_EFFPATH_NUM ? m_szEffPath[nIndex] : NULL;
}

const RwChar* const	AgcuEffPath::GetPath_Ini(void) 
{
	return m_szEffPath[E_EFFPATH_INI];
}

const RwChar* const	AgcuEffPath::GetPath_Tex(void) 
{
	return m_szEffPath[E_EFFPATH_TEX];
}

const RwChar* const AgcuEffPath::GetPath_Anim(void) 
{
	return m_szEffPath[E_EFFPATH_ANIM];
}

const RwChar* const AgcuEffPath::GetPath_Clump(void) 
{
	return m_szEffPath[E_EFFPATH_CLUMP];
}

const RwChar* const AgcuEffPath::GetPath_Sound(void) 
{
	return m_szEffPath[E_EFFPATH_SOUND];
}

const RwChar* const AgcuEffPath::GetPath_SoundMono(void) 
{
	return m_szEffPath[E_EFFPATH_SOUNDMONO];
}

const RwChar* const AgcuEffPath::GetPath_SoundStream(void) 
{
	return m_szEffPath[E_EFFPATH_SOUNDSTREAM];
}

void AgcuEffPath::SetPath_Ini(const RwChar* szIni)
{
	m_szEffPath[E_EFFPATH_INI] = szIni;
}

void AgcuEffPath::SetPath_Tex(const RwChar* szTex)
{
	m_szEffPath[E_EFFPATH_TEX] = szTex;
}

void AgcuEffPath::SetPath_Anim(const RwChar* szAnim)
{
	m_szEffPath[E_EFFPATH_ANIM] = szAnim;
}

void AgcuEffPath::SetPath_Clump(const RwChar* szClump)
{
	m_szEffPath[E_EFFPATH_CLUMP] = szClump;
}

void AgcuEffPath::SetPath_Sound(const RwChar* szSound)
{
	m_szEffPath[E_EFFPATH_SOUND] = szSound;
}

void AgcuEffPath::SetPath_SoundMono(const RwChar* szSoundMono)
{
	m_szEffPath[E_EFFPATH_SOUNDMONO] = szSoundMono;
}
