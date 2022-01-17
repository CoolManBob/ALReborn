//	AgpdChatting.h
////////////////////////////////////////////////////////////////

#ifndef	__AGPDCHATTING_H__
#define	__AGPDCHATTING_H__

#include "ApBase.h"


struct AgpdChatCommand
{
	CHAR *						m_szCommand;
	PVOID						m_pvClass;
	ApModuleDefaultCallBack		m_fnCallback;

	AgpdChatCommand()
	{
		m_szCommand		= NULL;
		m_pvClass		= NULL;
		m_fnCallback	= NULL;
	}
};

class AgpdChattingADChar
{
public:
	INT32	m_lLastWhisperSenderID;			// 마지막으로 귓속말을 보낸사람
	
	BOOL	m_bIsBlockWhisper;				// 귓속말 차단 여부
};

#endif	//__AGPDCHATTING_H__
