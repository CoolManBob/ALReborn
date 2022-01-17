#pragma once


// CListBoxDrag

class CListBoxDrag : public CListBox
{
	DECLARE_DYNAMIC(CListBoxDrag)

public:
	CListBoxDrag();
	virtual ~CListBoxDrag();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


