#ifndef			_AgcmUIHelp_H_
#define			_AgcmUIHelp_H_

#include "AuRandomNumber.h"
#include "AgcmUIManager2.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIHelpD" )
#else
#pragma comment ( lib , "AgcmUIHelp" )
#endif
#endif

class AgcmUIHelp : public AgcModule
{
public:
	AgcmUIManager2		*	m_pAgcmUIManager2		;

	INT32					m_lNumHelp;
	CHAR **					m_pszHelpItem;
	CHAR **					m_pszHelpContent;
	INT32 *					m_plHelpDepth;

	INT32					m_lCurrentItem;

	AgcdUIUserData *		m_pstUDItem;
	AgcdUIUserData *		m_pstUDItemDepth;
	AgcdUIUserData *		m_pstUDItemCurrent;

	BOOL					m_bEditMode;

	AgcdUIControl *			m_pcsContentControl;

	//@{ 2006/07/04 burumal
	BOOL					m_bDecryptionFlag;
	BOOL					m_bDecryption;
	//@}

public:
	AgcmUIHelp()	;
	~AgcmUIHelp()	;

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();

	VOID			SetEditMode(bool bMode = TRUE);

	BOOL			ReadHelp(CHAR *szFile, BOOL bDecryption = FALSE);
	VOID			WriteHelp(string szFile);
	
	//@{ 2006/07/04 burumal
	VOID			RefreshUI();

	VOID			ReleaseHelp();

	INT32			GetHelpNum() { return m_lNumHelp; }
	CHAR**			GetHelpItem() { return m_pszHelpItem; }
	CHAR**			GetHelpContent() { return m_pszHelpContent; }
	INT32*			GetHelpDepth() { return m_plHelpDepth; }

	BOOL			IsDecrypted()  { return m_bDecryption; }
	//@}

	static	BOOL	CBDisplayContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static	BOOL	CBChangeItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
};

#endif			// _AgcmUIHelp_H_

