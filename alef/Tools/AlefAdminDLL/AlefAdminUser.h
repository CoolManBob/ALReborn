#pragma once
#include "afxcmn.h"
#include "afxwin.h"

enum	eRegionType
{
	REGION_MAIN			,
	REGION_BATTLE		,
	REGION_VILLAGE		,
	REGION_SPIRIT		,
	REGION_DUNGEON		,

	REGION_COUNT		,
};

class AlefAdminUser : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminUser)


protected:
	AlefAdminUser();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminUser();

public:
	enum { IDD = IDD_USER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnBnClickedUserRequest();
	virtual void	OnInitialUpdate();

	static BOOL		CBUserInfoUpdate( PVOID pData , PVOID pClass , PVOID pCustData );

	VOID			InitRegion			( VOID );
	VOID			UpdateRegion		( INT nRegionIndex , INT nRegionUserCount );
	VOID			UpdateUserCount		( VOID );

	const char*		GetRegionName		( INT nRegionIndex );
	eRegionType		GetRegionType		( INT nRegionIndex );

	VOID			AddRegionUserCount	( eRegionType eType , INT nRegionUserCount  );

	BOOL			SendUserRequest		( VOID );

protected:

	CListCtrl	m_ListMain;
	CListCtrl	m_listDungeon;
	CListCtrl	m_listBattle;
	CListCtrl	m_listSpirit;
	CListCtrl	m_listVillage;

	CStatic		m_staticMainCount;
	CStatic		m_staticDungeonCount;
	CStatic		m_staticBattleCount;
	CStatic		m_staticSpiritCount;
	CStatic		m_staticVillageCount;
	CStatic		m_staticAllUser;

	INT			m_arrRegionUserCount[ REGION_COUNT ];
};