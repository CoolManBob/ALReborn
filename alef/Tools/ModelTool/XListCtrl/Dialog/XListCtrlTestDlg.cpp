// XListCtrlTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XListCtrlTest.h"
#include "XListCtrlTestDlg.h"
#include "about.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestDlg dialog

BEGIN_MESSAGE_MAP(CXListCtrlTestDlg, CDialog)
//{{AFX_MSG_MAP(CXListCtrlTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClick)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblClick)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemChanged)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_CHECK1, OnGridlines)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_COMBO_SELECTION, OnComboSelection)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED, OnCheckbox)
	ON_REGISTERED_MESSAGE(WM_XLISTCTRL_EDIT_END, OnEditEnd)
	
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXListCtrlTestDlg::CXListCtrlTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CXListCtrlTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXListCtrlTestDlg)
	m_bGridlines = FALSE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRunning = FALSE;
	m_bGridlines = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CXListCtrlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//
	// comment out the DDX_Control line to create CXListCtrl dynamically -
	// remember also to include lines in OnInitDialog to create CXListCtrl,
	// and remove the list control from IDD_LISTCTRLTEST_DIALOG in the rc file
	//
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXListCtrlTestDlg)
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Control(pDX, IDC_LIST, m_List);	// comment out this line if creating dynamically
	DDX_Check(pDX, IDC_CHECK1, m_bGridlines);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXListCtrlTestDlg::OnInitDialog()
{
	TRACE(_T("in CXListCtrlTestDlg::OnInitDialog\n"));

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//	when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); 		// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//
	// include the following lines to create CXListCtrl dynamically -
	// remember also to comment out DDX line, and remove the list control
	// from IDD_LISTCTRLTEST_DIALOG in the rc file
	//
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

#if 0  // -----------------------------------------------------------
	DWORD dwStyle = LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS |
		WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;

	CRect rect, rectbutton;
	GetClientRect(&rect);
	GetDlgItem(IDC_BUTTON1)->GetClientRect(&rectbutton);
	rect.DeflateRect(5, 5);
	rect.top = rectbutton.bottom + 15;

	TRACE("   calling CreateEx\n");

	// you may also use Create(), but I prefer WS_EX_CLIENTEDGE style
	VERIFY(m_List.CreateEx(WS_EX_CLIENTEDGE, _T("SysListView32"), _T(""),
						dwStyle, rect, this, IDC_LIST, NULL));
	CFont *pFont = GetFont();
	if (pFont)
		m_List.SetFont(pFont);
#endif // -----------------------------------------------------------

	// XListCtrl must have LVS_EX_FULLROWSELECT if combo or edit boxes are used

	DWORD dwExStyle = LVS_EX_FULLROWSELECT
					/*| LVS_EX_TRACKSELECT*/;	// for hot tracking
	if (m_bGridlines)
		dwExStyle |= LVS_EX_GRIDLINES;

	m_List.SetExtendedStyle(dwExStyle);

	// call EnableToolTips to enable tooltip display
	m_List.EnableToolTips(TRUE);

	InitListCtrl(&m_List);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
// OnSysCommand
void CXListCtrlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlg;
		dlg.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXListCtrlTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnQueryDragIcon
HCURSOR CXListCtrlTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXListCtrlTestDlg::OnTimer(UINT nIDEvent)
{
	static int nPct = 0;
	static int nItem = -1;
	static int nBlink = 0;

	if (nIDEvent == 1)
	{
		KillTimer(nIDEvent);
		m_List.SetProgress(1, 2);
		nPct = 0;
		SetTimer(2, 80, NULL);
		SetTimer(4, 400, NULL);
	}
	else if (nIDEvent == 2)
	{
		nPct += 10;
		m_List.UpdateProgress(1, 2, nPct);
		if (nPct >= 100)
		{
			KillTimer(nIDEvent);
			SetTimer(3, 2000, NULL);
		}
	}
	else if (nIDEvent == 3)
	{
		CString str;
		str = m_List.GetComboText(0, 2);

		KillTimer(nIDEvent);
		m_List.DeleteProgress(1, 2);
		SetTimer(1, 2000, NULL);
	}
	else if (nIDEvent == 4)
	{
		if (nItem != -1)
		{
			// remove previous color & bold from columns 1 and 5
			m_List.SetItemText(nItem, 5, NULL, (COLORREF)-1, (COLORREF)-1);
			m_List.SetBold(nItem, 1, FALSE);
		}

		// blink item 3
		if (nItem == 3)
		{
			nBlink++;
			if (nBlink >= 12)
			{
				nBlink = 0;
				nItem++;
				m_List.SetItemText(nItem, 5, NULL, RGB(255,255,255), RGB(255,0,0));
				m_List.SetBold(nItem, 1, TRUE);
			}
			else
			{
				if (nBlink & 1)		// blink every other time
				{
					m_List.SetItemText(nItem, 5, NULL, RGB(255,255,255), RGB(255,0,0));
					m_List.SetBold(nItem, 1, TRUE);
				}
			}
		}
		else
		{
			nItem++;
			if (nItem >= 10)
				nItem = 0;
			m_List.SetItemText(nItem, 5, NULL, RGB(255,255,255), RGB(255,0,0));
			m_List.SetBold(nItem, 1, TRUE);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// OnButton1
void CXListCtrlTestDlg::OnButton1()
{
	if (m_bRunning)
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		KillTimer(1);
		KillTimer(2);
		KillTimer(3);
		KillTimer(4);
		m_bRunning = FALSE;
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Start"));
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		FillListCtrl(&m_List);

		// show a disabled item
		m_List.SetEnabled(5, FALSE);
		m_List.SetItemText(5, 1, _T("This row is disabled"));

#if 0  // -----------------------------------------------------------
		// demonstrate handling of WM_SYSCOLORCHANGE
		int nElement = COLOR_WINDOW;
		COLORREF crWindow = ::GetSysColor(nElement);
		if (::SetSysColors(1,				// number of elements
						   &nElement,		// array of elements
						   &crWindow))		// array of RGB values
		{
			TRACE(_T("SetSysColors ok\n"));
		}
		else
		{
			TRACE(_T("SetSysColors failed\n"));
		}
#endif // -----------------------------------------------------------

		SetTimer(1, 1000, NULL);

		m_bRunning = TRUE;
		GetDlgItem(IDC_BUTTON1)->SetWindowText(_T("Stop"));
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnButton2 - mark all subitems as unmodified
void CXListCtrlTestDlg::OnButton2() 
{
	m_List.SetListModified(FALSE);

	for (int nItem = 0; nItem < m_List.GetItemCount(); nItem++)
	{
		for (int nSubItem = 0; nSubItem < m_List.GetColumns(); nSubItem++)
		{
			m_List.SetModified(nItem, nSubItem, FALSE);

			COLORREF crText, crBackground;
			m_List.GetItemColors(nItem, nSubItem, crText, crBackground);
			m_List.SetItemColors(nItem, nSubItem, RGB(0,0,0), crBackground);

			// is this a checkbox?
			if (m_List.GetItemCheckedState(nItem, nSubItem) != -1)
			{
				if (nSubItem == 0)
					m_List.SetItemText(nItem, nSubItem, _T(" "));	// erase *
				else
					m_List.SetItemText(nItem, nSubItem, _T("OK"));
			}
		}
	}

	m_List.RedrawWindow();

	CString strCaption = _T("");
	strCaption.LoadString(AFX_IDS_APP_TITLE);
	SetWindowText(strCaption);
}

///////////////////////////////////////////////////////////////////////////////
// OnClear - clear m_Log listbox
void CXListCtrlTestDlg::OnClear() 
{
	m_Log.ResetContent();
}

///////////////////////////////////////////////////////////////////////////////
// OnGridlines
void CXListCtrlTestDlg::OnGridlines() 
{
	UpdateData(TRUE);

	DWORD dwExStyle = m_List.GetExtendedStyle();

	if (m_bGridlines)
		dwExStyle |= LVS_EX_GRIDLINES;
	else
		dwExStyle &= ~LVS_EX_GRIDLINES;

	m_List.SetExtendedStyle(dwExStyle);
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXListCtrlTestDlg::OnDestroy()
{
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	KillTimer(4);
	CDialog::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// OnClick
//
// This method shows how to handle NM_CLICK messages from XListCtrl
//
void CXListCtrlTestDlg::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
	}

	if (nItem >= 0 && nSubItem >= 0)
	{
		CString strText = m_List.GetItemText(nItem, nSubItem);
		Log(_T("OnClick at (%d,%d):  '%s'"), nItem, nSubItem, strText);
	}

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnDblClick
//
// This method shows how to handle NM_DBLCLK messages from XListCtrl
//
void CXListCtrlTestDlg::OnDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	if (pNMIA)
	{
		nItem = pNMIA->iItem;
		nSubItem = pNMIA->iSubItem;
	}

	if (nItem >= 0 && nSubItem >= 0)
	{
		CString strText = m_List.GetItemText(nItem, nSubItem);
		Log(_T("OnDblClick at (%d,%d):  '%s'"), nItem, nSubItem, strText);
	}

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnColumnClick
//
// This method shows how to handle LVN_COLUMNCLICK messages from XListCtrl
//
void CXListCtrlTestDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	if (pNMLV)
	{
		nItem = pNMLV->iItem;
		nSubItem = pNMLV->iSubItem;
	}

	Log(_T("OnColumnClick on column %d"), nSubItem);

	CString strCaption = _T("");
	strCaption.LoadString(AFX_IDS_APP_TITLE);
	strCaption += _T(" *");
	SetWindowText(strCaption);

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnItemChanged
//
// This method shows how to handle LVN_ITEMCHANGED messages from XListCtrl
//
void CXListCtrlTestDlg::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = -1;
	int nSubItem = -1;
	if (pNMLV)
	{
		nItem = pNMLV->iItem;
		nSubItem = pNMLV->iSubItem;
	}
	//TRACE(_T("in CXListCtrlTestDlg::OnItemChanged:  %d, %d\n"), nItem, nSubItem);

	if (pNMLV && (pNMLV->uNewState == (UINT)(LVIS_FOCUSED|LVIS_SELECTED)))
	{

		CString strText = m_List.GetItemText(nItem, nSubItem);
		Log(_T("Selection changed to item %d"), nItem);
	}

	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnEditEnd
//
// This method shows how to handle WM_XLISTCTRL_EDIT_END messages 
// from XListCtrl
//
LRESULT CXListCtrlTestDlg::OnEditEnd(WPARAM nItem, LPARAM nSubItem)
{
	if (nItem >= 0 && nSubItem >= 0)
	{
		CString strText = m_List.GetItemText(nItem, nSubItem);
		Log(_T("OnEditEnd at (%d,%d): '%s'"), nItem, nSubItem, strText);

		COLORREF crText, crBackground;
		m_List.GetItemColors(nItem, nSubItem, crText, crBackground);
		if (m_List.GetModified(nItem, nSubItem))
		{
			// subitem was modified - color it red
			m_List.SetItemText(nItem, nSubItem, strText, 
				RGB(255,0,0), crBackground);
			CString strCaption = _T("");
			strCaption.LoadString(AFX_IDS_APP_TITLE);
			strCaption += _T(" *");
			SetWindowText(strCaption);
		}
		else
		{
			// subitem not modified - color it black -
			// note that once modified, a subitem will remain
			// marked as modified
			m_List.SetItemText(nItem, nSubItem, strText, 
				RGB(0,0,0), crBackground);
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboSelection
//
// This method shows how to handle WM_XLISTCTRL_COMBO_SELECTION messages 
// from XListCtrl
//
LRESULT CXListCtrlTestDlg::OnComboSelection(WPARAM nItem, LPARAM nSubItem)
{
	if (nItem >= 0 && nSubItem >= 0)
	{
		CString strText = m_List.GetItemText(nItem, nSubItem);
		Log(_T("OnComboSelection at (%d,%d): '%s'"), nItem, nSubItem, strText);

		COLORREF crText, crBackground;
		m_List.GetItemColors(nItem, nSubItem, crText, crBackground);
		if (m_List.GetModified(nItem, nSubItem))
		{
			// subitem was modified - color it red
			m_List.SetItemText(nItem, nSubItem, strText, 
				RGB(255,0,0), crBackground);
			CString strCaption = _T("");
			strCaption.LoadString(AFX_IDS_APP_TITLE);
			strCaption += _T(" *");
			SetWindowText(strCaption);
		}
		else
		{
			// subitem not modified - color it black -
			// note that once modified, a subitem will remain
			// marked as modified
			m_List.SetItemText(nItem, nSubItem, strText, 
				RGB(0,0,0), crBackground);
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnCheckbox
//
// This method shows how to handle WM_XLISTCTRL_CHECKBOX_CLICKED messages 
// from XListCtrl
//
LRESULT CXListCtrlTestDlg::OnCheckbox(WPARAM nItem, LPARAM nSubItem)
{
	if (nItem >= 0 && nSubItem >= 0)
	{
		Log(_T("OnCheckbox at (%d,%d)"), nItem, nSubItem);

		COLORREF crText, crBackground;
		m_List.GetItemColors(nItem, nSubItem, crText, crBackground);
		if (m_List.GetModified(nItem, nSubItem))
		{
			// subitem was modified - color it red
			if (nSubItem == 0)
				m_List.SetItemText(nItem, nSubItem, _T("*"), 
					RGB(255,0,0), crBackground);
			else
				m_List.SetItemText(nItem, nSubItem, _T("Error"), 
					RGB(255,0,0), crBackground);

			CString strCaption = _T("");
			strCaption.LoadString(AFX_IDS_APP_TITLE);
			strCaption += _T(" *");
			SetWindowText(strCaption);
		}
		else
		{
			// subitem not modified - color it black -
			// note that once modified, a subitem will remain
			// marked as modified
			if (nSubItem == 0)
				m_List.SetItemText(nItem, nSubItem, _T(" "), 
					RGB(0,0,0), crBackground);
			else
				m_List.SetItemText(nItem, nSubItem, _T("OK"), 
					RGB(255,0,0), crBackground);
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Log - log a message to the m_Log listbox
void _cdecl CXListCtrlTestDlg::Log(LPCTSTR lpszFmt, ...)
{
	ASSERT(lpszFmt);

	TCHAR buf[2000], fmt[2000];
	va_list marker;

	// format string was passed as parameter
	_tcsncpy(fmt, lpszFmt, sizeof(fmt)/sizeof(TCHAR)-1);
	fmt[sizeof(fmt)/sizeof(TCHAR)-1] = _T('\0');

	// combine output string and variables
	va_start(marker, lpszFmt);
	_vsntprintf(buf, (sizeof(buf)/sizeof(TCHAR))-1, fmt, marker);
	va_end(marker);
	buf[sizeof(buf)/sizeof(TCHAR)-1] = _T('\0');

	CString strMsg(buf);

	// don't display \r or \n characters
	int i = 0;
	while ((i = strMsg.FindOneOf(_T("\r\n"))) != -1)
		strMsg.SetAt(i, _T(' '));

	i = m_Log.AddString(strMsg);

	TRACE(_T("%s\n"), strMsg);

	// scroll into view
	if (i >= 0)
	{
		m_Log.SetTopIndex(i);
	}

	m_Log.SetCurSel(-1);
}
