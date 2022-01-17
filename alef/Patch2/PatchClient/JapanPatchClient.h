#pragma once

#ifdef _JPN

#include "BasePatchClient.h"

class JapanPatchClient
	:	public BasePatchClient
{
public:
	JapanPatchClient( CDialog* pDlg );
	virtual ~JapanPatchClient(void);

	virtual BOOL			OnInitDialog		( VOID );
	virtual VOID			OnPaint				( VOID );
	virtual VOID			OnTimer				( UINT nIDEvent );
	virtual VOID			DoDataExchange		( CDataExchange* pDX );

	virtual VOID			StartPatch			( VOID );

protected:

	HBITMAP						m_cBMPBackgroundEx;	
};


#endif