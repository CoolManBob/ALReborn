// XTPGI_Sound.h: interface for the CXTPGI_Sound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_SOUND_H__FB43FCDF_00A2_4956_8A84_1ED64EFCED8D__INCLUDED_)
#define AFX_XTPGI_SOUND_H__FB43FCDF_00A2_4956_8A84_1ED64EFCED8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

#include "AgcdEffSound.h"


class CXTPGI_Sound : public CXTPGI_EffBase  
{
public:
	CXTPGI_Sound(CString strCaption, AgcdEffSound* pSound);
	virtual ~CXTPGI_Sound();

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffSound*	m_pSound;

	CXTPGI_Enum<AgcdEffSound::eEffSoundType>*	m_pItemType;
	CXTPGI_FileName*							m_pItemSoundFile;
	CXTPGI_FileName*							m_pItemMonoFile;
	CXTPGI_UINT*								m_pItemLoopCnt;
	CXTPGI_Float*								m_pItemVolume;
};

#endif // !defined(AFX_XTPGI_SOUND_H__FB43FCDF_00A2_4956_8A84_1ED64EFCED8D__INCLUDED_)
