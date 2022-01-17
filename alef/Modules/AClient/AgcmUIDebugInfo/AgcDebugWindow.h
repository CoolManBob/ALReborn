// AgcDebugWindow.h: interface for the AgcDebugWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCDEBUGWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_)
#define AFX_AGCDEBUGWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_

#include "AgcEngine.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AgcmUIDebugInfo;

class AgcDebugWindow : public AgcWindow  
{
private:
	AgcmUIDebugInfo *	m_pcsAgcmUIDebugInfo;

	UINT32				m_ulPrevClockCount;

	BOOL				m_bLCtrl;
	BOOL				m_bRCtrl;
	BOOL				m_bLAlt;
	BOOL				m_bLShift;

	INT32				m_lFilterMode;

// Profile 용
#ifdef	_PROFILE_
	int							m_iProfileMode;				// 0 - 1초마다 reset, 1 - reset 하지 않고 누적
	char						profile_str[10][200];
	CProfileNode*				m_pCurNode;
	int							m_iCommand;// 0 - 9 : Down to Num Child, 98 - up to parent,99 - reset
	int							m_iMyCurPage;// 0- 0~9, 1- 10~19
	int							m_iTotalItem;

	char						profile_check_str[10][200];
	char						profile_max_str[200];
	CProfileNode*				m_pCheckedNode[10];						// 주시하기 위해 설정하는 노드
	int							m_iCheckIndex;

	void						FindMaxItem(CProfileNode* node,CProfileNode** setnode);
#endif

public:
	AgcDebugWindow(AgcmUIDebugInfo *pcsAgcmUIDebugInfo = NULL);
	virtual ~AgcDebugWindow();

	void	SetUIWindow( AgcmUIDebugInfo *pcsAgcmUIDebugInfo ) { m_pcsAgcmUIDebugInfo = pcsAgcmUIDebugInfo; }
	
	BOOL	OnInit();

	VOID	OnWindowRender();

	BOOL	OnKeyDown(RsKeyStatus *ks);
	BOOL	OnKeyUp(RsKeyStatus *ks);

	VOID	RenderProfile();
	VOID	RenderMemory();
	VOID	RenderObject();
	VOID	RenderGraphic();
	VOID	RenderSound();
	VOID	RenderNetwork();

	VOID	ChangeTextureFilterMode();
};

#endif // !defined(AFX_AGCDEBUGWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_)
