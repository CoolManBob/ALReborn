#pragma once


// CNewListBox

class CNewListBox : public CListBox
{
	DECLARE_DYNAMIC(CNewListBox)

public:
	CNewListBox();
	virtual ~CNewListBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


