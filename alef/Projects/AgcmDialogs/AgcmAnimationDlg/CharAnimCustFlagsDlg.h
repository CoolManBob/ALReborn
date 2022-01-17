#pragma once

#include "../resource.h"

class CCharAnimCustFlagsDlg : public CDialog
{
public:
	CCharAnimCustFlagsDlg(UINT16 *pulCustFlags, INT32* pClumpShowOffsetTime, UINT32* pClumpShowFadeInTime, INT32* pClumpHideOffsetTime, UINT32* pClumpHideFadeOutTime, CWnd* pParent = NULL);

	enum { IDD = IDD_CHAR_ANIM_CUST_FLAGS };
	BOOL		m_bLoop;
	BOOL		m_bClumpHideEndTime;
	INT32*		m_plClumpHideOffsetTime;
	UINT32*		m_plClumpHideFadeOutTime;
	BOOL		m_bClumpShowStartTime;
	INT32*		m_plClumpShowOffsetTime;
	UINT32*		m_plClumpShowFadeInTime;

protected:
	UINT16	*m_pulCustFlags;


protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Message Map
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
