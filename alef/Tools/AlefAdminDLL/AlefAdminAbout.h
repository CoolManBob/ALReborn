#pragma once


// AlefAdminAbout dialog

#include "Resource.h"

class AlefAdminAbout : public CDialog
{
	DECLARE_DYNAMIC(AlefAdminAbout)

public:
	AlefAdminAbout(CWnd* pParent = NULL);   // standard constructor
	virtual ~AlefAdminAbout();

// Dialog Data
	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
