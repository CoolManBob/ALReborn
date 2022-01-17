#if !defined(AFX_SKYSETTINGDLG_H__B9FED664_F5CE_4F64_B8AF_D8766E97140D__INCLUDED_)
#define AFX_SKYSETTINGDLG_H__B9FED664_F5CE_4F64_B8AF_D8766E97140D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkySettingDlg.h : header file
//

#include "AgcmEventNatureDlg.h"
#include "HSVControlStatic.h"
#include "TimeSelectStatic.h"
#include "TextureListStatic.h"
#include "ImagePreviewStatic.h"
#include "HSVInputDlg.h"
#include "PortionStatic.h"
#include "afxwin.h"

#define SKYSETTING_TEXTURE_FILE_NAME	"map\\skytexture.ini"
/////////////////////////////////////////////////////////////////////////////
// CSkySettingDlg dialog

class CSkySettingDlg : public CDialog
{
// Construction
public:
	// 데이타들 카피..
	CString				m_strTitle			;
	BOOL				m_bPrevTimerSetting	;
	UINT8				m_uPrevSpeedRate	;
	AgcmEventNatureDlg *m_pEventNatureDlg	;

	INT32				m_nPrevTemplateID	;
	INT32				m_nTemplateID		;	// 템플릿 아이디..
	AgpdSkySet *		m_pAgpdSkySet		;
	AgcdSkySet *		m_pAgcdSkySet		;

	ASkySetting			m_aSkySetting[ ASkySetting::TS_MAX ]	; // 24시간.. 빽업용..

	BOOL	GetData();
	BOOL	SetData();
	BOOL	RestoreData();
	
	CSkySettingDlg(CWnd* pParent = NULL);   // standard constructor

	int		DoModal( int tid , AgcmEventNatureDlg * pEventNatureDlg );

public:
	// Operations..
	BOOL	SetTime( INT32 time );	// 0~23..
	void	ProcessLButtonUp( POINT point );

	void	Export();

// Dialog Data
	//{{AFX_DATA(CSkySettingDlg)
	enum { IDD = IDD_SKYSETTING };
	CColorStatic	m_ctlCloud2Color;
	CColorStatic	m_ctlCloud1Color;
	CColorStatic	m_ctlCircumstanceColor;
	CHSVControlStatic	m_ctlCircumstanceAlpha;
	CHSVControlStatic	m_ctlCloud2Alpha;
	CHSVControlStatic	m_ctlCloud1Alpha;
	CListCtrl	m_ctlEffectList;
	CImagePreviewStatic	m_ctlCircumstanceTexture;
	CImagePreviewStatic	m_ctlCloud1ImagePreview	;
	CImagePreviewStatic	m_ctlCloud2ImagePreview	;
	CColorStatic		m_ctlSkyTop				;
	CColorStatic		m_ctlSkyBottom			;
	CColorStatic		m_ctlFog				;
	CColorStatic		m_ctlDirectional		;
	CColorStatic		m_ctlAmbient			;
	CTextureListStatic	m_ctlTextureList		;
	CTimeSelectStatic	m_ctlTimeSelectStatic	;
	float				m_fSkyRadius			;
	float	m_fCloudSpeed1;
	float	m_fCloudSpeed2;
	int		m_nEffectInterval;
	BOOL	m_bUseCloud;
	BOOL	m_bUseColor;
	BOOL	m_bUseEffect;
	BOOL	m_bUseFog;
	CString	m_strBackgroundMusic;
	BOOL	m_bUseBGM;
	float	m_fFogDistance;
	float	m_fFogFarClip;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkySettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSkySettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSetfog();
	virtual void OnCancel();
	afx_msg void OnEffectAdd();
	afx_msg void OnEffectDelete();
	afx_msg void OnEffectEdit();
	afx_msg void OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeDataFlag();
	afx_msg void OnBackgroundMusicBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bUseWeather;
	afx_msg void OnBnClickedSetWeather();

	BOOL m_bWeaLensFlare	;
	BOOL m_bWeaMoon			;
	BOOL m_bWeaBreath		;
	BOOL m_bWeaStar			;
	BOOL m_bWeaPLightOff	;
	BOOL m_bWeaNEDisable	; // Nature Effect Disable
	BOOL m_bWea_Reserved1	;
	BOOL m_bWea_Reserved2	;
	BOOL m_bWea_Reserved3	;

	CImagePreviewStatic	m_ctlSunTexture	;
	CColorStatic		m_ctlSunColor	;
	CHSVControlStatic	m_ctlSunAlpha	;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKYSETTINGDLG_H__B9FED664_F5CE_4F64_B8AF_D8766E97140D__INCLUDED_)
