// ApmEventManagerDlg.h : main header file for the APMEVENTMANAGERDLG DLL
//

#if !defined(AFX_APMEVENTMANAGERDLG_H__BAFC5590_659A_472B_A341_887DBFCCC226__INCLUDED_)
#define AFX_APMEVENTMANAGERDLG_H__BAFC5590_659A_472B_A341_887DBFCCC226__INCLUDED_


#include "ApModule.h"
#include "ApmEventManager.h"

class AFX_EXT_CLASS ApmEventDialog : public ApModule
{
public:
	ApmEventDialog();
	virtual ~ApmEventDialog();

	virtual BOOL	Open(ApdEvent *pstEvent)	{ return TRUE; };
};

class  AFX_EXT_CLASS ApmEventManagerDlg : public ApModule  
{
private:
	ApmEventManager *		m_pcsApmEventManager;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;

public:
	ApmEventManagerDlg();
	virtual ~ApmEventManagerDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	OnInit();

	BOOL	Open(ApBase *pcsBase, ApdEventAttachData *pstEventAD = NULL);
	BOOL	Open(ApdEventAttachData *pstEvents);
	BOOL	OpenCondition(ApdEvent *pstEvent);
	BOOL	AddEventDialog(ApmEventDialog *pcsDialog, ApdEventFunction eFunction);
	void	Close();
	void	CloseCondition();
};


#endif // !defined(AFX_APMEVENTMANAGERDLG_H__BAFC5590_659A_472B_A341_887DBFCCC226__INCLUDED_)
