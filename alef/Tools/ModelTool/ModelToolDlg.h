#pragma once

#include "PropertyForm.h"
#include "ResourceForm.h"
#include "MenuForm.h"
#include "RenderForm.h"
#include "ToolForm.h"
#include "TitleForm.h"
#include "EditEquipmentsDlg.h"
#include "AnimationDlg.h"
#include "AttachFaceDlg.h"
#include "AttachHairDlg.h"
#include "CustomizePreviewDlg.h"
#include "LightOptionDlg.h"

#include "CharRideDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CModelToolDlg dialog

class CModelToolDlg : public CDialog
{
protected:
	CRenderForm				*m_pcsRenderForm;
	CTitleForm				*m_pcsTitleForm;
	CMenuForm				*m_pcsMenuForm;
	CPropertyForm			*m_pcsPropertyForm;
	CToolForm				*m_pcsToolForm;
	CResourceForm			*m_pcsResourceForm;
	CFormView				*m_pcsTempForm;

	CEditEquipmentsDlg*		m_pcsEditEquipmentsDlg;
	CAnimationDlg*			m_pcsAnimationDlg;
	CAttachFaceDlg*			m_pcsAttachFaceDlg;
	CAttachHairDlg*			m_pcsAttachHairDlg;
	CCustomizePreviewDlg*	m_pcsCustomizePreviewDlg;
	CCharRideDlg*			m_pcsCharRideDlg;

	HBRUSH					m_hWhiteBrush;
	HBRUSH					m_hBlackBrush;

public:
	CRect					m_rtToolDlg;
	CRect					m_rtRenderForm;
	CRect					m_rtTitleForm;
	CRect					m_rtMenuForm;
	CRect					m_rtPropertyForm;
	CRect					m_rtToolForm;
	CRect					m_rtResourceForm;

public:
	static CModelToolDlg*	GetInstance();

	VOID					ResizeResourceForm( DWORD dwWidth );

	BOOL					UpdateAll();
	VOID					ReleaseAll();

	CRenderForm*			GetRenderForm()			{	return m_pcsRenderForm;		}
	CTitleForm*				GetTitleForm()			{	return m_pcsTitleForm;		}
	CMenuForm*				GetMenuForm()			{	return m_pcsMenuForm;		}
	CPropertyForm*			GetPropertyForm()		{	return m_pcsPropertyForm;	}
	CResourceForm*			GetResourceForm()		{	return m_pcsResourceForm;	}

	BOOL					IsOpenEditEquipmentsDlg()	{	return m_pcsEditEquipmentsDlg ? TRUE : FALSE;	}
	VOID					OpenEditEquipmentsDlg();
	VOID					CloseEditEquipmentsDlg();
	BOOL					EditEquipmentsDlgIsPickMode();
	BOOL					EditEquipmentsDlgIsMouseMode();
	INT32					EditEquipmentsDlgGetMouseMode();
	INT32					EditEquipmentsDlgGetMouseModeAxis();

	BOOL					IsOpenAttachFaceDlg()		{	return m_pcsAttachFaceDlg ? TRUE : FALSE;	}
	VOID					OpenAttachFaceDlg();
	VOID					CloseAttachFaceDlg();
	CAttachFaceDlg*			GetAttachFaceDlg()			{	return m_pcsAttachFaceDlg;	}

	BOOL					IsOpenAttachHairDlg()		{	return m_pcsAttachHairDlg ? TRUE : FALSE;	}
	VOID					OpenAttachHairDlg();
	VOID					CloseAttachHairDlg();
	CAttachHairDlg*			GetAttachHairDlg()			{	return m_pcsAttachHairDlg;	}

	BOOL					IsOpenCustomizePreviewDlg()	{	return m_pcsCustomizePreviewDlg ? TRUE : FALSE;	}
	VOID					OpenCustomizePreviewDlg();
	VOID					CloseCustomizePreviewDlg();
	CCustomizePreviewDlg*	GetCustomizePreviewDlg()	{	return m_pcsCustomizePreviewDlg;	}

	BOOL					IsOpenAnimationOptionDlg()	{	return m_pcsAnimationDlg ? TRUE : FALSE;	}
	BOOL					OpenAnimationOptionDlg();
	BOOL					CloseAnimationOptionDlg();
	VOID					UpdateAnimTime(INT32 lTime);
	VOID					SetAnimRange(INT32 lMin, INT32 lMax);

	BOOL					IsOpenRideDlg()				{	return m_pcsCharRideDlg ? TRUE : FALSE;	}
	VOID					OpenRideDlg();
	VOID					CloseRideDlg();
	CCharRideDlg*			GetRideDlg()				{	return m_pcsCharRideDlg;	}

	BOOL					OpenLightOptionDlg()		{	
		CLightOptionDlg		cDlg;
		//cDlg.UpdateControl();
		cDlg.DoModal();
		return TRUE;
	}
	
protected:
	VOID	InitializeMember();
	BOOL	CreateMember();

// Construction
public:
	CModelToolDlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_MODELTOOL_DIALOG };

protected:
	HICON m_hIcon;

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};
