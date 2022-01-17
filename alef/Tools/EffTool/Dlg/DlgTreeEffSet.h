#pragma once

#include "DlgTree.h"
#include "dlgneweffbase.h"

class AgcdEffSet;
class AgcdEffBase;
class AgcdEffAnim;

class CDlgTreeEffSet : public CDlgTree  
{
public:
	struct Lparam
	{	
		enum	eLPARAM
		{
			E_LPARAM_EFFSET		= 0,
			E_LPARAM_EFFBASE,
			E_LPARAM_ANIM,
			E_LPARAM_NUM,
		};

		eLPARAM		m_eType;
		LPVOID		m_pEffSet;
		LPVOID		m_pEffBase;
		LPVOID		m_pEffAnim;

		explicit Lparam( eLPARAM eType, LPVOID pEffSet, LPVOID pEffBase = NULL, LPVOID pEffAnim = NULL ) : m_eType(eType), m_pEffSet(pEffSet), m_pEffBase(pEffBase), m_pEffAnim(pEffAnim)	{		};
		explicit Lparam(AgcdEffSet* pEffSet) : m_eType( E_LPARAM_EFFSET ), m_pEffSet( static_cast<LPVOID>(pEffSet) ), m_pEffBase(NULL), m_pEffAnim(NULL)									{		};

		bool operator == ( const Lparam& cmp )
		{
			 return cmp.m_eType == m_eType && cmp.m_pEffSet == m_pEffSet && cmp.m_pEffBase == m_pEffBase && cmp.m_pEffAnim == m_pEffAnim ? true : false;
		};

		AgcdEffSet* GetEffSet()		{	return reinterpret_cast<AgcdEffSet*>(m_pEffSet);	}
		AgcdEffBase* GetEffBase()	{	return reinterpret_cast<AgcdEffBase*>(m_pEffBase);	}
		AgcdEffAnim* GetEffAnim()	{	return reinterpret_cast<AgcdEffAnim*>(m_pEffAnim);	}
	};
	typedef Lparam					*LPLparam;
	typedef std::list<Lparam>		ListLparam;
	typedef ListLparam::iterator	ListLparamItr;
	
	struct EffSet
	{
		enum E_FLAG
		{
			FLAG_NOTSAVED	= 0x00000001,
			FLAG_NEW		= 0x00000002,
		};

		UINT	m_ulID;
		UINT	m_ulDelay;
		UINT	m_ulFlag;

		void	FlagOn(E_FLAG flag)		{	m_ulFlag |= (flag);		}
		void	FlagOff(E_FLAG flag)	{	m_ulFlag &= (~(flag));	}
		BOOL	FlagChk(E_FLAG flag)	{	return m_ulFlag & (flag) ? TRUE : FALSE; };

		explicit EffSet(UINT ulID, UINT ulFlag=0x0) : m_ulID(ulID),m_ulDelay(0LU),m_ulFlag(m_ulFlag)	{		}

		bool operator == (const EffSet& cmp) { return (cmp.m_ulID == m_ulID); };
	};
	typedef std::list<EffSet>		ListEffSet;
	typedef ListEffSet::iterator	ListEffSetItr;

private:
	ListLparam		m_listParam;		//LIST OF LPARAM
	LPLparam		m_pSelParam;		//SELECTED LPARAM
	ListEffSet		m_listStEffSet;		//LIST OF EffSet;

	LPEFFBASE		m_lpEffBaseCopy;	
	RwChar			m_szTexBaseCopy[EFF2_FILE_NAME_MAX];
	RwChar			m_szMaskBaseCopy[EFF2_FILE_NAME_MAX];

private:
	LPLparam	vGetLparam(HTREEITEM hItem);
	void		vDelLparam(LPLparam lpLparam);
	BOOL		vBeExistEffSet(UINT ulEffSetID);
	BOOL		vBeExistNotSaved(void);
	BOOL		vBeSavedEffset(UINT ulEffSetID);
	INT			vSaveEffSet(UINT ulEffSetID);
	AgcdEffSet*	vGetPtrEffSet(UINT ulEffSetID);

	INT			vInsStEffSet(UINT ulEffSetID);
	INT			vDelStEffSet(LPLparam pStLParam);
	INT			vDelEffBase(LPLparam pStLParam);
	INT			vDelEffAnim(LPLparam pStLParam);

	//dependancy
	INT			vInsEffBaseDpnd(LPLparam pStLParam);
public:
	INT			bDelEffBaseDpnd(INT delBaseDpndIndex);
	INT			bEffSetFlagUpdate(AgcdEffSet* pEffSet)		{	return vEffSetFlagUpdate(pEffSet);		};

private:	
	struct DpndInfo
	{
		DpndInfo() : parent(NULL), child(NULL), m_bdpnd(false), m_bdpndParent(false), m_bdpndToObj(false), m_bdpndToMissileObj(false)	{		}

		const AgcdEffBase*	parent;
		const AgcdEffBase*	child;

		bool	m_bdpnd;
		bool	m_bdpndParent;	// 0 : child, 1 : parent
		bool	m_bdpndToObj;
		bool	m_bdpndToMissileObj;
	};

private:
	bool		vGetDpndInfo(DpndInfo* pOut, AgcdEffSet* pEffSet, INT baseindex);
	INT			vEffBaseFlagUpdate(AgcdEffSet* pEffSet, INT baseindex);
	INT			vEffSetFlagUpdate(AgcdEffSet* pEffSet);
	INT			vEffBaseDpndUpdate(AgcdEffSet* pEffSet, INT delBaseIndex);

	HTREEITEM	vInsTrCtrlItem(LPCTSTR szBuff, LPARAM lparam, HTREEITEM hParent, HTREEITEM hInsertAfter=TVI_LAST);

	INT			vSetNewEffBase(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase, CDlgNewEffBase* pDlg);
	INT			vSetNewEffBase_Tex(AgcdEffSet* pEffSet, AgcdEffRenderBase* pEffBase, CDlgNewEffBase* pDlg);

public:
	INT			bClear();
	INT			bInsLparam(Lparam& lparam, HTREEITEM hParent=TVI_ROOT, HTREEITEM item=TVI_LAST);
	INT			bOnEditEffSet(UINT	ulEffID);
	INT			bOnNewEffSet(UINT	ulEffID);
	INT			bRefreshSelect(void);

	BOOL		bDpendancyValidation(LPEFFSET pEffSEt);

public:
	CDlgTreeEffSet(CWnd* pParent = NULL);

	afx_msg void OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteItemTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMenuItemTrCtrlIns();
	afx_msg void OnMenuItemTrCtrlDel();
	afx_msg void OnMenuItemTrCtrlEdit();
	afx_msg void OnMenuItemTrCtrltSave();
	afx_msg void OnTrctrleffsetInsertdependancy();
	afx_msg void OnMenuitemTrctrlBasecopy();
	afx_msg void OnMenuitemTrctrlBasepaste();
	DECLARE_MESSAGE_MAP()
	
};
