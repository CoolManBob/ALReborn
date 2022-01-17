#ifndef		_PLUGIN_NATURE_EFFECT_H_
#define		_PLUGIN_NATURE_EFFECT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyEngine.h"
#include "UITileList_PluginBase.h"

// CPlugin_NatureEffect

enum
{
	IDC_PLUGIN_NEFFECT_SET_BUTTON   =1258,
	IDC_PLUGIN_NEFFECT_TREE         =1259,
	IDC_PLUGIN_START		        =1260,
	IDC_PLUGIN_STOP			        =1261
};

class CPlugin_NatureEffect : public CUITileList_PluginBase
{
	DECLARE_DYNAMIC(CPlugin_NatureEffect)

public:
	CPlugin_NatureEffect();
	virtual ~CPlugin_NatureEffect();

	CTreeCtrl		m_TreeNEffect		;
	CButton			m_buttonNEffectSet	;
	CButton			m_buttonStart		;
	CButton			m_buttonEnd			;

	INT32			m_iCurSelectNEffectID;

	virtual BOOL OnIdle		( UINT32 ulClockCount );

	void			ChangeNEffectSetVal();

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
