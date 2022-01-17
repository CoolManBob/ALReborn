#include "stdafx.h"
#include "ModelTool.h"

void	CModelToolApp::RegistryKeyboard()
{
	m_cKeyboardMng.Insert( AniToggle, this, 's', eKeyboardPress, 0, 1, 0 );
	m_cKeyboardMng.Insert( AniToggle, this, 'S', eKeyboardPress, 0, 1, 0 );

	m_cKeyboardMng.Insert( ChangeBackGround, this, VK_F2, eKeyboardPress, 0, 0, 0 );

	m_cKeyboardMng.Insert( LeftClick, this, VK_LEFT, eKeyboardPress, 0, 0, 0 );
	m_cKeyboardMng.Insert( RightClick, this, VK_RIGHT, eKeyboardPress, 0, 0, 0 );
	m_cKeyboardMng.Insert( UpClick, this, VK_UP, eKeyboardPress, 0, 0, 0 );
	m_cKeyboardMng.Insert( DownClick, this, VK_DOWN, eKeyboardPress, 0, 0, 0 );

	m_cKeyboardMng.Insert( ShiftLeftClick, this, VK_LEFT, eKeyboardPress, 1, 0, 0 );
	m_cKeyboardMng.Insert( ShiftRightClick, this, VK_RIGHT, eKeyboardPress, 1, 0, 0 );
	m_cKeyboardMng.Insert( ShiftUpClick, this, VK_UP, eKeyboardPress, 1, 0, 0 );
	m_cKeyboardMng.Insert( ShiftDownClick, this, VK_DOWN, eKeyboardPress, 1, 0, 0 );

	m_cKeyboardMng.Insert( CtrlLeftClick, this, VK_LEFT, eKeyboardPress, 0, 1, 0 );
	m_cKeyboardMng.Insert( CtrlRightClick, this, VK_RIGHT, eKeyboardPress, 0, 1, 0  );
	m_cKeyboardMng.Insert( CtrlUpClick, this, VK_UP, eKeyboardPress, 0, 1, 0 );
	m_cKeyboardMng.Insert( CtrlDownClick, this, VK_DOWN, eKeyboardPress, 0, 1, 0 );

	m_cKeyboardMng.Insert( AltLeftClick, this, VK_LEFT, eKeyboardPress, 0, 0, 1 );
	m_cKeyboardMng.Insert( AltRightClick, this, VK_RIGHT, eKeyboardPress, 0, 0, 1 );
	m_cKeyboardMng.Insert( AltUpClick, this, VK_UP, eKeyboardPress, 0, 0, 1 );
	m_cKeyboardMng.Insert( AltDownClick, this, VK_DOWN, eKeyboardPress, 0, 0, 1 );
}

BOOL	CModelToolApp::AniToggle( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);
	pApp->StopCharacterAnim( pApp->IsPlayCharacterAnim() ); 

	return TRUE;
}

BOOL	CModelToolApp::ChangeBackGround( PVOID pThis )
{
	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	RwRGBA color = pApp->GetRenderWare()->GetBackColor();
	CColorDialog cDialog;
	cDialog.m_cc.Flags		= cDialog.m_cc.Flags | CC_RGBINIT | CC_FULLOPEN;
	cDialog.m_cc.rgbResult	= RGB( color.red, color.green, color.blue );
	if( cDialog.DoModal() == IDOK )
		pApp->GetRenderWare()->SetClearColor( GetRValue( cDialog.GetColor() ), GetGValue( cDialog.GetColor() ), GetBValue( cDialog.GetColor() ) );

	return TRUE;
}

//------------------------- Normal -----------------------
BOOL	CModelToolApp::LeftClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::UpClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::RightClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::DownClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

//------------------------- Shift -----------------------
BOOL	CModelToolApp::ShiftLeftClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::ShiftUpClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::ShiftRightClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::ShiftDownClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

//------------------------- Ctrl -----------------------
BOOL	CModelToolApp::CtrlLeftClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	if( CModelToolDlg::GetInstance()->IsOpenEditEquipmentsDlg() && 
		CModelToolDlg::GetInstance()->EditEquipmentsDlgIsMouseMode() )
	{
		RwV2d	vOffset =  { -2.f, 0.f };
		pApp->TransformPickedAtomic( &vOffset );
	}

	return TRUE;
}

BOOL	CModelToolApp::CtrlUpClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	if( CModelToolDlg::GetInstance()->IsOpenEditEquipmentsDlg() && 
		CModelToolDlg::GetInstance()->EditEquipmentsDlgIsMouseMode() )
	{
		RwV2d	vOffset =  { 0.f, 2.f };
		pApp->TransformPickedAtomic( &vOffset );
	}

	return TRUE;
}

BOOL	CModelToolApp::CtrlRightClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	if( CModelToolDlg::GetInstance()->IsOpenEditEquipmentsDlg() && 
		CModelToolDlg::GetInstance()->EditEquipmentsDlgIsMouseMode() )
	{
		RwV2d	vOffset =  { 2.f, 0.f };
		pApp->TransformPickedAtomic( &vOffset );
	}
	

	return TRUE;
}

BOOL	CModelToolApp::CtrlDownClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	if( CModelToolDlg::GetInstance()->IsOpenEditEquipmentsDlg() && 
		CModelToolDlg::GetInstance()->EditEquipmentsDlgIsMouseMode() )
	{
		RwV2d	vOffset =  { 0.f, -2.f };
		pApp->TransformPickedAtomic( &vOffset );
	}

	return TRUE;
}

//------------------------- Alt -----------------------
BOOL	CModelToolApp::AltLeftClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::AltUpClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::AltRightClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}

BOOL	CModelToolApp::AltDownClick( PVOID pThis )
{
	if( !pThis )	return FALSE;

	CModelToolApp* pApp = static_cast<CModelToolApp*>(pThis);

	return TRUE;
}
