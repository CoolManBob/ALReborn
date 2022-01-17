// -----------------------------------------------------------------------------
//  _____  _             _                                 _      ____            _ __  __                 _     
// |  __ \| |           (_)                /\             | |    / __ \          | |  \/  |               | |    
// | |__) | |_   _  __ _ _ _ __           /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __     | |__  
// |  ___/| | | | |/ _` | | '_ \         / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \    | '_ \ 
// | |    | | |_| | (_| | | | | |       / ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | | | |
// |_|    |_|\__,_|\__, |_|_| |_|      /_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_)|_| |_|
//                  __/ |        ______                                                         | |              
//                 |___/        |______|                                                        |_|              
//
// Plugin GUI for the ambient occlusion map
//
// -----------------------------------------------------------------------------
// Originally created on 01/13/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_PLUGIN_AMBOCCLMAP
#define _H_PLUGIN_AMBOCCLMAP

#include "MyEngine.h"
#include "resource.h"
#include "UITileList_PluginBase.h"

#pragma once


// CAmbOcclMapPrefixDlg 대화 상자입니다.

class CAmbOcclMapPrefixDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmbOcclMapPrefixDlg)

public:
	CAmbOcclMapPrefixDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAmbOcclMapPrefixDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_AMBOCCLMAP_PREFIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// Ray length for dungeon
	float rayLength_;
	// Number of rays used per sample
	UINT rayCount_;
	BOOL recreateAll_;
};
#pragma once


// CAmbOcclMapSupersampleDlg 대화 상자입니다.

class CAmbOcclMapSupersampleDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmbOcclMapSupersampleDlg)

public:
	CAmbOcclMapSupersampleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAmbOcclMapSupersampleDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_AMBOCCLMAP_SUPERSAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// A supersampling value for the ambient occlusion map computation
	BYTE supersample_;
};
//@{ Jaewon 20050713
// ;)
#pragma once


// CAmbOcclMapDestroyDlg 대화 상자입니다.

class CAmbOcclMapDestroyDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmbOcclMapDestroyDlg)

public:
	CAmbOcclMapDestroyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAmbOcclMapDestroyDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_AMBOCCLMAP_DESTROY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL destroyAll_;
};
//@} Jaewon

// -----------------------------------------------------------------------------

class CPlugin_AmbOcclMap : public CUITileList_PluginBase
{
public:
	// Construction/Destruction
	CPlugin_AmbOcclMap();
	virtual	~CPlugin_AmbOcclMap();

	enum ControlID
	{
		CTRLID_BUTTON_CREATE = 0,
		CTRLID_BUTTON_CLEAR,
		CTRLID_BUTTON_COMPUTE,
		CTRLID_BUTTON_SAVE,
		CTRLID_COMBOBOX_TECHNIQUE,
		CTRLID_PROGRESS_OBJECT,
		CTRLID_PROGRESS_SLICE
	};

	// Operators

	// Accessors
	void setObjectProgress(int percent)
	{ objectProgress_.SetPos(percent); }
	void setSliceProgress(int percent)
	{ sliceProgress_.SetPos(percent); }

	// Interface methods
	//@{ Jaewon 20050204
	void endComputing();
	//@} Jaewon

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//{{AFX_MSG(CPlugin_AmbOcclMap)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// Data members
	// Controls
	CButton createButton_;
	CButton clearButton_;
	CButton computeButton_;
	CButton saveButton_;
	CComboBox technicComboBox_;
	CProgressCtrl objectProgress_;
	CProgressCtrl sliceProgress_;

	// A flag telling whether maps are created
	bool created_;

	//@{ Jaewon 20050713
	// Have it as members so that its setting can be saved.
	CAmbOcclMapPrefixDlg createDlg_;
	CAmbOcclMapSupersampleDlg computeDlg_;
	CAmbOcclMapDestroyDlg destroyDlg_;
	//@} Jaewon

	// Implementation methods

};

#endif // _H_PLUGIN_AMBOCCLMAP
// -----------------------------------------------------------------------------
// Plugin_AmbOcclMap.h - End of file
// -----------------------------------------------------------------------------