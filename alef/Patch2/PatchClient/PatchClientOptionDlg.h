#pragma once
#include "PatchOptionFile.h"

/////////////////////////////////////////////////////////////////////////////
// CPatchClientOptionDlg dialog
#include "KbcButton.h"
#include "afxcmn.h"
#include "afxwin.h"

class CPatchClientOptionDlg : public CDialog
{
public:
	CPatchClientOptionDlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_PATCHCLIENT_OPTION_DIALOG };
	CComboBox	m_cTextureCombo;
	CComboBox	m_cResolutionCombo;	
	CComboBox	m_cWindowedModeCombo;
	CComboBox	m_cLanguageCombo;
	int			m_iBeforeSelectSlider;
	int			m_iSelectSlider;
	CSliderCtrl	m_ctrlSlider;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnPaint();
	afx_msg void OnOptionButtonSave();
	afx_msg void OnOptionButtonCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeOptionComboResolution();
	afx_msg void OnSelchangeOptionComboTexture();
	afx_msg void OnCbnSelchangeOptionComboWindowedMode();
	afx_msg void OnNMThemeChangedOptionSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CPatchOptionFile*	m_pcPatchOptionFile;

    CFont				m_cDrawFont;
	CPoint				m_cOldPoint;

	CKbcButton			m_cKbcSaveButton;
	CKbcButton			m_cKbcCancelButton;

	CString				m_strCPU;
	CString				m_strGraphicCard;
	int					m_iPhysicalRam;

	CBitmap				m_cBMPBackground;

	void		SetOptionFile( CPatchOptionFile *pcPatchOptionFile )	{	m_pcPatchOptionFile = pcPatchOptionFile;	}

	void		InitResolution();
	void		InitTexture();
	void		InitWindowed();
	void		InitLanguage();


	INT			GetLanguageNumber( const CString&	strLanguage );

private:
	int			GetPhysicalMemory()	{	
		MEMORYSTATUS MemInfo;
		MemInfo.dwLength = sizeof( MemInfo );
		GlobalMemoryStatus( &MemInfo );
		//return MemInfo.dwTotalPhys + ONEM - 1) / ONEM);
		return (int)(MemInfo.dwTotalPhys / (1024*1024));
	}
	void		GetGraphicCardName( CString& strName )	{
		LPDIRECT3D9 pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
		if( pD3D9 )
		{
			D3DADAPTER_IDENTIFIER9 pIdentifier;
			pD3D9->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &pIdentifier );
			pD3D9->Release();

			strName = pIdentifier.Description;
		}
		else
			strName = "";
	}
};