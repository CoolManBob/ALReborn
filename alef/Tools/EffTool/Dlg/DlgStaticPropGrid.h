#pragma once

#include "DlgStatic.h"

//------------------------- CDlgStaticPropGrid -----------------------------
class CDlgStaticPropGrid : public CDlgStatic  
{
public:
	CDlgStaticPropGrid(CWnd* pParent = NULL);

protected:
	virtual BOOL	t1stInitGrid()	{return TRUE;};

protected:
	virtual	BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

protected:
	CXTPPropertyGrid*	m_pGrid;
};

//------------------------- CDlgGridEffSet -----------------------------
class AgcdEffSet;
class AgcdEffBase;
class AgcdEffAnim;
class AgcdEffBoard;
class AgcdEffLight;
class AgcdEffMFrm;
class AgcdEffObj;
class AgcdEffParticleSystem;
class AgcdEffParticleSys_SimpleBlackHole;
class AgcdEffSound;
class AgcdEffTail;

class AgcdEffAnim_Colr;
class AgcdEffAnim_TuTv;
class AgcdEffAnim_Missile;
class AgcdEffAnim_Linear;
class AgcdEffAnim_Rev;
class AgcdEffAnim_Rot;
class AgcdEffAnim_RpSpline;
class AgcdEffAnim_RtAnim;
class AgcdEffAnim_Scale;

class CDlgGridEffSet : public CDlgStaticPropGrid
{
public:
	CDlgGridEffSet(CWnd* pParent = NULL);

protected:
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP();

public:
	INT		bSetForEffSet(AgcdEffSet* pEffSet);
	INT		bSetForEffBase(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase);
	INT		bSetForEffAnim(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase, AgcdEffAnim* pEffAnim);
	INT		bSetBlank(void);

protected:
	BOOL	t1stInitGrid();
	UINT	m_ulEffSetID;

private:
	BOOL	vIsVisible();
	INT		vNewGrid();
};

///////////////////////////////////////////////////////////////////////////////
// CDlgGridToolOption
///////////////////////////////////////////////////////////////////////////////
class CDlgGridToolOption : public CDlgStaticPropGrid
{
	//class CXTPGI_EffUseInfo;
public:
	CDlgGridToolOption(CWnd* pParent = NULL);

protected:
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP();

public:
	INT		bSetUseEffSetInfo(STEFFUSEINFO& stUseEffSet);

protected:
	BOOL	t1stInitGrid();

};
