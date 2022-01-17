#ifndef	__AGPD_EVENT_NPCDIALOG_H__
#define __AGPD_EVENT_NPCDIALOG_H__

#include "ApBase.h"

enum eAgpdEventNPCDialogType
{
	AGPM_NPCDIALOG_TYPE_MOB = 0,
	AGPM_NPCDIALOG_TYPE_NPC
};

enum eAgpdEventNPCDialogNPC
{
	AGPD_NPCDIALOG_NPC_NORMAL = 0, //NPC를 찍으면 나오는 일반적인 대화. 현재는 NORMAL만 있음.
	AGPD_NPCDIALOG_NPC_MAX
};

enum eAgpdEventNPCDialogMob
{
	AGPD_NPCDIALOG_MOB_COMMON = 0, //NPC를 찍으면 나오는 일반적인 대화.
	AGPD_NPCDIALOG_MOB_WANDERING,
	AGPD_NPCDIALOG_MOB_ATTACK,
/*	AGPD_NPCDIALOG_MOB_TARGET_PC,
	AGPD_NPCDIALOG_MOB_DIE,
	AGPD_NPCDIALOG_MOB_SPAWN,
	AGPD_NPCDIALOG_MOB_CHASE,
	AGPD_NPCDIALOG_MOB_PC_MISS,*/ //2004.10.25일 대사 통합되면서 사라졌음.
	AGPD_NPCDIALOG_MOB_MAX
};

class AgpdEventMobDialogKeyTemplate
{
public:
	INT32			m_lMobTID;	//DialogID의 키값은 MOB의 TID이다.
	INT32			m_lDialogID;
};

class AgpdEventMobDialogData
{
public:
	int				m_iDialogTextNum;
	char			**m_ppstrDialogText;
	float			m_fProbable;
};

class AgpdEventMobDialogTemplate
{
public:
	//AgpdEventMobDialogData m_csEventMobDialog[AGPD_NPCDIALOG_MOB_MAX];
	ApSafeArray<AgpdEventMobDialogData, AGPD_NPCDIALOG_MOB_MAX>	m_csEventMobDialog;

	INT32			m_lMobDialogTID;
	char			m_strTemplateName[80];
};

class AgpdEventNPCDialogTemplate
{
public:
	INT32			m_lNPCDialogTID;
	int				m_iDialogTextNum;
	char			**m_ppstrDialogText;

	AgpdEventNPCDialogTemplate()
	{
		m_lNPCDialogTID = 0;
		m_iDialogTextNum = 0;
		m_ppstrDialogText = NULL;
	}
};

//현재는 대화용 텍스트이외의 데이터는 없다.
class AgpdEventNPCDialogData
{
public:
	INT32			m_lNPCDialogTextID;
};

#endif