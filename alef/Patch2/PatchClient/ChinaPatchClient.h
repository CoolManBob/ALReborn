#pragma once

#include "BasePatchClient.h"
#include "DlgAgreement.h"

class ChinaPatchClient
	:	public BasePatchClient
{
public:
	ChinaPatchClient( CDialog* pDlg );
	virtual ~ChinaPatchClient(void);

	virtual BOOL			OnInitDialog		( VOID );
	virtual VOID			OnPaint				( VOID );
	virtual VOID			OnTimer				( UINT nIDEvent );
	virtual VOID			DoDataExchange		( CDataExchange* pDX );

	virtual VOID			OnBnClickedMenu1	( VOID );
	virtual VOID			OnBnClickedMenu2	( VOID );
	virtual VOID			OnBnClickedMenu3	( VOID );
	virtual VOID			OnBnClickedMenu4	( VOID );
	virtual VOID			OnBnClickedMenu5	( VOID );

	virtual VOID			OnBnClickedServer1	( VOID );
	virtual VOID			OnBnClickedServer2	( VOID );
	virtual VOID			OnBnClickedServer3	( VOID );

	void					OnCheckPVPAgreement	( void );

protected:
	BOOL					SetServerAreaList	( INT iCategory , BOOL bNewArea=FALSE );
	
	// Menu Button
	CKbcButton						m_ckbcMenu1Button;
	CKbcButton						m_ckbcMenu2Button;
	CKbcButton						m_ckbcMenu3Button;
	CKbcButton						m_ckbcMenu4Button;
	CKbcButton						m_ckbcMenu5Button;

	// Server Button
	CKbcButton						m_ckbcServerSelect1Btn;
	CKbcButton						m_ckbcServerSelect2Btn;
	CKbcButton						m_ckbcServerSelect3Btn;

	CTransparentListBox				m_cServerListBox;
	CKbcButton						m_cScrollThumbBtn;
	BOOL							m_bIsWaitPatch;

	CNewStatic						m_StaticStatus;
	CNewStatic						m_StaticDetailInfo;

	CDlgAgreement					m_DlgAgreement;
};
