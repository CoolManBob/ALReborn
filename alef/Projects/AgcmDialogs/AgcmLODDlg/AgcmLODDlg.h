#pragma once

#include "ApModule.h"
#include "AgcmLODManager.h"
#include "AgcmPreLODManager.h"

#define AGCM_LODDLG_MAX_INFO							20

enum eAgcmLODDlgCallbackPoint
{
	AGCMLODDLG_CB_ID_APPLY_LOD = 0,
	AGCMLODDLG_CB_ID_NUM
};

struct AgcdDefaultLODInfo
{
	CHAR*	m_paszDFFInfo[AGCM_LODDLG_MAX_INFO];
	UINT32	m_aaulDistance[AGCM_LODDLG_MAX_INFO][AGPDLOD_MAX_NUM];
	INT32	m_lDistanceType;
	UINT32	m_aulMaxDistanceRatio[AGCM_LODDLG_MAX_INFO];
};

class AFX_EXT_CLASS AgcmLODDlg : public ApModule
{
public:
	AgcmLODDlg();
	virtual ~AgcmLODDlg();

	static AgcmLODDlg *GetInstance();

	// pstLODInfo는 pstLODInfo->m_paszDFFInfo만 셋팅한다.(m_paszDFFInfo = Base DFF Name)
	BOOL			OpenLODDlg(AgcdLOD *pstLOD, AgcdPreLOD *pstAgcdPreLOD, CHAR *szDataDirectory, AgcdDefaultLODInfo *pstLODInfo);
	BOOL			CloseLODDlg(AgcdPreLOD *pstPreLOD = NULL);
	BOOL			ApplyLODDlg(AgcdPreLOD *pstPreLOD);


	BOOL			FindLODDataName(CHAR *szDest);
	BOOL			OpenLODDistance(UINT32 *pulDistance);	
	
	VOID			SetDefaultLODRange(UINT32 *pulRange);									// 초기화시 불러준다.
	BOOL			SetCallbackApplyLOD(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// CALLBACK 등록 함수

	AgcmPreLODManager	*GetAgcmPreLODManager()	{	return m_pcsAgcmPreLODManager;	}

protected:
	BOOL			OnAddModule();

public:
	AgcmLODManager		*m_pcsAgcmLODManager;
	AgcmPreLODManager	*m_pcsAgcmPreLODManager;

	UINT32				m_aulDefaultDistance[AGPDLOD_MAX_NUM];	

	CHAR				m_szDataDirectory[256];

	AgcdDefaultLODInfo	m_stLODInfo;
	AgcdLOD				*m_pcsLOD;
	AgcdPreLOD			*m_pcsPreLOD;
};
