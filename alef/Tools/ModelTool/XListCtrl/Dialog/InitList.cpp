// InitList.cpp
//

#include "stdafx.h"
#include "resource.h"
#include VIEWCLASS_HEADER_FILE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int VIEWCLASS::m_nColWidths[6] = { 8, 14, 12, 9, 12, 9 };	// sixty-fourths

///////////////////////////////////////////////////////////////////////////////
// InitListCtrl
void VIEWCLASS::InitListCtrl(CXListCtrl * pList)
{
	ASSERT(pList);
	if (!pList)
		return;

	// set column width according to window rect
	CRect rect;
	pList->GetWindowRect(&rect);

	int w = rect.Width() - 2;

	TCHAR *	lpszHeaders[] = { _T("Enable"),
							  _T("Item"),
							  _T("Pct"),
							  _T("Status"),
							  _T("City"),
							  _T("Population"),
							  NULL };
	int i;
	int total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	w = w - ::GetSystemMetrics(SM_CXVSCROLL);	// width of vertical scroll bar

	// add columns
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = (i == 1) ? LVCFMT_LEFT : LVCFMT_CENTER;
		lvcolumn.fmt = (i == 5) ? LVCFMT_RIGHT : lvcolumn.fmt;
		lvcolumn.pszText = lpszHeaders[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = (lpszHeaders[i+1] == NULL) ? w - total_cx - 2 : (w * m_nColWidths[i]) / 64;
		total_cx += lvcolumn.cx;
		pList->InsertColumn(i, &lvcolumn);
	}

#if 0  // -----------------------------------------------------------
	// create the image list from bitmap resource
	VERIFY(pList->m_cImageList.Create(13, 13, ILC_COLOR24, 3, 1));
	CBitmap bm;
	bm.LoadBitmap(IDB_CHECKBOXES);
	pList->m_cImageList.Add(&bm, RGB(255, 0, 255));
	pList->m_HeaderCtrl.SetImageList(&pList->m_cImageList);
#endif // -----------------------------------------------------------

	// iterate through header items and attach the image list
	HDITEM hditem;

	for (i = 0; i < pList->m_HeaderCtrl.GetItemCount(); i++)
	{
		hditem.mask = HDI_IMAGE | HDI_FORMAT;
		pList->m_HeaderCtrl.GetItem(i, &hditem);
		hditem.fmt |=  HDF_IMAGE;
		if (i == 0 || i == 3)
			hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
		else
			hditem.iImage = XHEADERCTRL_NO_IMAGE;

		pList->m_HeaderCtrl.SetItem(i, &hditem);
	}

	memset(&lvcolumn, 0, sizeof(lvcolumn));

	// set the format again - must do this twice or first column does not get set
	for (i = 0; ; i++)
	{
		if (lpszHeaders[i] == NULL)
			break;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM;
		lvcolumn.fmt = (i == 1) ? LVCFMT_LEFT : LVCFMT_CENTER;
		lvcolumn.fmt = (i == 5) ? LVCFMT_RIGHT : lvcolumn.fmt;
		lvcolumn.iSubItem = i;
		pList->SetColumn(i, &lvcolumn);
	}
}

///////////////////////////////////////////////////////////////////////////////
// FillListCtrl
void VIEWCLASS::FillListCtrl(CXListCtrl * pList)
{
	static TCHAR * szCityNames[] = 
	{
		_T("Tokyo"),
		_T("Mexico City"),
		_T("Seoul"),
		_T("New York"),
		_T("Sao Paulo"),
		_T("Bombay"),
		_T("Delhi"),
		_T("Shanghai"),
		_T("Los Angeles"),
		_T("Osaka"),
		_T("Jakarta"),
		_T("Calcutta"),
		_T("Cairo"),
		_T("Manila"),
		_T("Karachi"),
		_T("Moscow"),
		_T("Buenos Aires"),
		_T("Dacca"),
		_T("Rio de Janeiro"),
		_T("Beijing"),
		_T("London"),
		_T("Tehran"),
		_T("Istanbul"),
		_T("Lagos"),
		_T("Shenzhen"),
		_T("Paris"),
		_T("Chicago"),
		_T("Canton"),
		_T("Chungking"),
		_T("Wuhan"),
		_T("Lima"),
		_T("Bangkok"),
		_T("Bogota"),
		_T("Washington"),
		_T("Nagoya"),
		_T("Madras"),
		_T("Lahore"),
		_T("Hong Kong"),
		_T("Johannesburg"),
		_T("San Francisco"),
		_T("Bangalore"),
		_T("Kinshasa"),
		_T("Taipei"),
		_T("Hyderabad"),
		_T("Tientsin"),
		_T("Dallas"),
		_T("Philadelphia"),
		_T("Santiago"),
		_T("Detroit"),
		_T("Ruhr"),
		_T("Boston"),
		_T("Khartoum"),
		_T("Ahmadabad"),
		_T("Belo Horizonte"),
		_T("Madrid"),
		_T("Baghdad"),
		_T("Miami"),
		_T("Houston"),
		_T("Ho Chi Minh City"),
		_T("Toronto"),
		_T("St. Petersburg"),
		_T("Atlanta"),
		_T("Alexandria"),
		_T("Caracas"),
		_T("Singapore"),
		_T("Riyadh"),
		_T("Shenyang"),
		_T("Rangoon"),
		_T("Poona"),
		_T("Guadalajara"),
		_T("Sydney"),
		_T("Chittagong"),
		_T("Kuala Lumpur"),
		_T("Berlin"),
		_T("Algiers"),
		_T("Porto Alegre"),
		_T("Abidjan"),
		_T("Monterrey"),
		_T("Phoenix"),
		_T("Casablanca"),
		_T("Milan"),
		_T("Barcelona"),
		_T("Ankara"),
		_T("Recife"),
		_T("Seattle"),
		_T("Busan"),
		_T("Melbourne"),
		_T("Surat"),
		_T("Montreal"),
		_T("Brasília"),
		_T("Pyongyang"),
		_T("Sian"),
		_T("Athens"),
		_T("Durban"),
		_T("Fortaleza"),
		_T("Nanking"),
		_T("Salvador"),
		_T("Medellin"),
		_T("Harbin"),
		_T("Minneapolis"),
		NULL
	};

	static TCHAR * szCityPopulations[] = 
	{
		_T("34,200,000"),
		_T("22,800,000"),
		_T("22,300,000"),
		_T("21,900,000"),
		_T("20,200,000"),
		_T("19,850,000"),
		_T("19,700,000"),
		_T("18,150,000"),
		_T("18,000,000"),
		_T("16,800,000"),
		_T("16,550,000"),
		_T("15,650,000"),
		_T("15,600,000"),
		_T("14,950,000"),
		_T("14,300,000"),
		_T("13,750,000"),
		_T("13,450,000"),
		_T("13,250,000"),
		_T("12,150,000"),
		_T("12,100,000"),
		_T("12,000,000"),
		_T("11,850,000"),
		_T("11,500,000"),
		_T("11,100,000"),
		_T("10,700,000"),
		_T("9,950,000"),
		_T("9,750,000"),
		_T("9,550,000"),
		_T("9,350,000"),
		_T("9,100,000"),
		_T("8,550,000"),
		_T("8,450,000"),
		_T("8,350,000"),
		_T("8,150,000"),
		_T("8,050,000"),
		_T("7,600,000"),
		_T("7,550,000"),
		_T("7,400,000"),
		_T("7,400,000"),
		_T("7,250,000"),
		_T("7,100,000"),
		_T("7,000,000"),
		_T("6,950,000"),
		_T("6,700,000"),
		_T("6,350,000"),
		_T("6,000,000"),
		_T("6,000,000"),
		_T("5,900,000"),
		_T("5,800,000"),
		_T("5,800,000"),
		_T("5,700,000"),
		_T("5,650,000"),
		_T("5,600,000"),
		_T("5,600,000"),
		_T("5,600,000"),
		_T("5,550,000"),
		_T("5,550,000"),
		_T("5,400,000"),
		_T("5,400,000"),
		_T("5,400,000"),
		_T("5,250,000"),
		_T("5,100,000"),
		_T("5,000,000"),
		_T("4,700,000"),
		_T("4,650,000"),
		_T("4,550,000"),
		_T("4,550,000"),
		_T("4,500,000"),
		_T("4,450,000"),
		_T("4,350,000"),
		_T("4,350,000"),
		_T("4,300,000"),
		_T("4,300,000"),
		_T("4,200,000"),
		_T("4,150,000"),
		_T("4,150,000"),
		_T("4,100,000"),
		_T("3,950,000"),
		_T("3,900,000"),
		_T("3,850,000"),
		_T("3,850,000"),
		_T("3,800,000"),
		_T("3,750,000"),
		_T("3,750,000"),
		_T("3,750,000"),
		_T("3,700,000"),
		_T("3,700,000"),
		_T("3,700,000"),
		_T("3,650,000"),
		_T("3,600,000"),
		_T("3,600,000"),
		_T("3,600,000"),
		_T("3,500,000"),
		_T("3,500,000"),
		_T("3,500,000"),
		_T("3,500,000"),
		_T("3,500,000"),
		_T("3,450,000"),
		_T("3,400,000"),
		_T("3,400,000"),
		NULL
	};

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	//
	// Note - the string arrays must persist for life of list control
	//
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////

	// a list of some of my favorite things - note that this list
	// is not sorted
	m_sa1.RemoveAll();
	m_sa1.Add(_T("banana"));
	m_sa1.Add(_T("strawberry"));
	m_sa1.Add(_T("tangerine"));
	m_sa1.Add(_T("cherry"));
	m_sa1.Add(_T("plum"));
	m_sa1.Add(_T("cranberry"));
	m_sa1.Add(_T("blueberry"));
	m_sa1.Add(_T("lime"));
	m_sa1.Add(_T("pear"));
	m_sa1.Add(_T("lemon"));
	m_sa1.Add(_T("orange"));
	m_sa1.Add(_T("peach"));
	m_sa1.Add(_T("apple"));
	m_sa1.Add(_T("raspberry"));
	m_sa1.Add(_T("grape"));

	// a list of some more of my favorite things
	m_sa2.RemoveAll();
	m_sa2.Add(_T("Aidi"));
	m_sa2.Add(_T("Akbas"));
	m_sa2.Add(_T("Ariegeois"));
	m_sa2.Add(_T("Boerboel"));
	m_sa2.Add(_T("Boolomo"));
	m_sa2.Add(_T("Borzoi"));
	m_sa2.Add(_T("Broholmer"));
	m_sa2.Add(_T("Caniche"));
	m_sa2.Add(_T("Charplaninatz"));
	m_sa2.Add(_T("Drok"));
	m_sa2.Add(_T("Elo"));
	m_sa2.Add(_T("Farou"));
	m_sa2.Add(_T("Godo"));
	m_sa2.Add(_T("Groenendael"));
	m_sa2.Add(_T("Hahoavu"));
	m_sa2.Add(_T("Heidewachtel"));
	m_sa2.Add(_T("Hovawart"));
	m_sa2.Add(_T("Juzak"));
	m_sa2.Add(_T("Kai"));
	m_sa2.Add(_T("Kangal"));
	m_sa2.Add(_T("Karabash"));
	m_sa2.Add(_T("Kishu"));
	m_sa2.Add(_T("Kuvasz"));
	m_sa2.Add(_T("Landseer"));
	m_sa2.Add(_T("Levesque"));
	m_sa2.Add(_T("Licaon"));
	m_sa2.Add(_T("Maliki"));
	m_sa2.Add(_T("Mechelaar"));
	m_sa2.Add(_T("Mirigung"));
	m_sa2.Add(_T("Mudi"));
	m_sa2.Add(_T("Noggum"));
	m_sa2.Add(_T("Ocherese"));
	m_sa2.Add(_T("Phalene"));
	m_sa2.Add(_T("Pocadan"));
	m_sa2.Add(_T("Podhalan"));
	m_sa2.Add(_T("Poitevin"));
	m_sa2.Add(_T("Saluki"));
	m_sa2.Add(_T("Samojedskaja"));
	m_sa2.Add(_T("Samoyed"));
	m_sa2.Add(_T("Spion"));
	m_sa2.Add(_T("Stichelhaar"));
	m_sa2.Add(_T("Taygan"));
	m_sa2.Add(_T("Telomian"));
	m_sa2.Add(_T("Tornjak"));
	m_sa2.Add(_T("Virelade"));
	m_sa2.Add(_T("Warrigal"));


	pList->LockWindowUpdate();	// ***** lock window updates while filling list *****

	pList->DeleteAllItems();

	CString str = _T("");
	int nItem, nSubItem;

	// insert the items and subitems into the list
	for (nItem = 0; nItem < 50; nItem++)
	{
		for (nSubItem = 0; nSubItem < 6; nSubItem++)
		{
			str = _T("");

			if (nSubItem == 0)				// checkbox
				str = _T(" ");
			else if (nSubItem == 1)			// combo
				str = _T("");
			else if (nSubItem == 2)			// progress
				str = _T("Complete");
			else if (nSubItem == 3)			// checkbox
				str = _T("OK");
			else if (nSubItem == 4)			// city name
				str = szCityNames[nItem];
			else if (nSubItem == 5)			// city population
				str = szCityPopulations[nItem];

			// show background coloring
			if (nSubItem == 0)
				pList->InsertItem(nItem, str);
			else if (nItem == 0 && nSubItem == 4)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(210,221,244));
			else if (nItem == 3 && nSubItem == 1)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(255,230,162));
			else if (nItem == 8 && nSubItem == 3)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(194,209,166));
			else if (nItem == 1 && nSubItem == 2)
				pList->SetItemText(nItem, nSubItem, str, RGB(0,0,0), RGB(247,202,202));
			else
				pList->SetItemText(nItem, nSubItem, str);

			if (nSubItem == 1 && ((nItem & 1) == 1))
				pList->SetComboBox(nItem, nSubItem, 
								   TRUE,		// enable combobox
								   &m_sa1,		// pointer to persistent CStringArray
								   5,			// size of dropdown
								   0,			// initial combo selection
								   TRUE);		// sort CStringArray
			if (nSubItem == 1 && ((nItem & 1) == 0))
				pList->SetComboBox(nItem, nSubItem, 
								   TRUE,		// enable combobox
								   &m_sa2,		// pointer to persistent CStringArray
								   10,			// size of dropdown
								   3,			// initial combo selection
								   TRUE);		// sort CStringArray
			if (nSubItem == 0 || nSubItem == 3)
				pList->SetCheckbox(nItem, nSubItem, 1);
			if (nSubItem == 4)
				pList->SetEdit(nItem, nSubItem);
		}
	}

#ifndef NO_XLISTCTRL_TOOL_TIPS

	// set tool tips in second row
	nItem = 1;
	for (nSubItem = 0; nSubItem < 6; nSubItem++)
	{
		str.Format(_T("This is %d,%d"), nItem, nSubItem);
		pList->SetItemToolTipText(nItem, nSubItem, str);
	}

#endif

	pList->UnlockWindowUpdate();	// ***** unlock window updates *****
}
