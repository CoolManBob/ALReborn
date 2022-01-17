#pragma once

#include "resource.h"       // main symbols

class CEffToolApp : public CWinApp
{
public:
	CEffToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffToolApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEffToolApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
protected:
	CRenderWare			m_cRenderWare;
	BOOL				m_bExistToolView;
	CMultiDocTemplate*	m_pDocTemplateToolView;

public:
	void			bCalcBound(UINT32 ulEffID);
	CRenderWare&	bGetRW()	{	return m_cRenderWare;	}


public:
	afx_msg void OnPacking();
	afx_msg void OnUpdatePacking(CCmdUI *pCmdUI);
	afx_msg void OnStopCalcbound();
	afx_msg void OnCalcboundAlleff();
	afx_msg void OnAmbientLight();
	afx_msg void OnEngineLight();
};

extern CEffToolApp theApp;

