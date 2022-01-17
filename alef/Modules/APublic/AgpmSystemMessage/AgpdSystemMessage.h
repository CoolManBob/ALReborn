// AgpmSystemMessage.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 20050906

#ifndef _AGPD_SYSTEM_MESSAGE_H_
#define _AGPD_SYSTEM_MESSAGE_H_

typedef struct _stAgpdSystemMessage
{

	INT32	m_lOperation;
	INT32	m_lCode;
	INT32	m_alData[2];
	CHAR*	m_aszData[2];
	DWORD	m_dwColor;

	BOOL	m_bIsSendWhisper;
	BOOL	m_bTranslateMessage;

} AgpdSystemMessage;

#endif // _AGPD_SYSTEM_MESSAGE_H_