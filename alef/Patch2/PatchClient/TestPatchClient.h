#pragma once

#include "BasePatchClient.h"

class TestPatchClient
	:	public BasePatchClient
{
public:
	TestPatchClient( CDialog* pDlg );
	virtual ~TestPatchClient(void);

	virtual BOOL			OnInitDialog		( VOID );
	virtual VOID			OnPaint				( VOID );
	virtual VOID			DoDataExchange		( CDataExchange* pDX );

protected:
	HBITMAP						m_cBMPBackgroundEx;	

};
