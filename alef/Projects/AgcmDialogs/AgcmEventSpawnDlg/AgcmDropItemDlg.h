#if !defined(AFX_AGCMDROPITEMDLG_H__419D768A_46FE_4AEA_BF64_E0A849F3A39D__INCLUDED_)
#define AFX_AGCMDROPITEMDLG_H__419D768A_46FE_4AEA_BF64_E0A849F3A39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmDropItemDlg.h : header file
//

#include "../Resource.h"
#include "AgpmItem.h"
#include "AgpmDropItem.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmDropItemDlg dialog

class AgcmDropItemDlg : public CDialog
{
//	AgpdDropItemTemplate		*m_pcsAgpdDropItemTemplate;
	AgpmItem					*m_pcsAgpmItem;

//	eAgpmDropItemBag	m_lCurrentEditBag;
	INT32				m_lFreeForAllCurrentEditRow;
	INT32				m_lOldFreeForAllCurrentEditRow;
	INT32				m_lCommonCurrentEditRow;
	INT32				m_lOldCommonCurrentEditRow;
	INT32				m_lRareCurrentEditRow;
	INT32				m_lOldRareCurrentEditRow;
	INT32				m_lSpecialCurrentEditRow;
	INT32				m_lOldSpecialCurrentEditRow;

	void DisableAllRuneEditBox();
	void EnableWeaponRuneEditBox();
	void EnableShieldRuneEditBox();
	void EnableArmourRuneEditBox();
	void UpdateRuneOptionEditBox( AgpdItemTemplate *pcsItemTemplate );

// Construction
public:
	void SetDlgData( AgpmItem *pcsAgpmItem, AgpdDropItemTemplate *pcsAgpdDropItemTemplate );
	void SetDropItemTemplate( AgpdDropItemTemplate *pcsAgpdDropItemTemplate );
//	void UpdateDropItemListRow( int iRow, int iItemTID, int iDropRate, int iEquipRate, int iMinQuantity, int iMaxQuantity, int iJackpotRate, int iJackpotPiece );
	void UpdateDropItemListRow( int iRow, AgpdDropItemInfo *pcsDropItemInfo );
	void EditDropItemListRow( int iRow );
	void CleanDropItemList();
	void CleanDropItemListRow( int iRow );
//	eAgpmDropItemBag GetCurrentBag();
//	AgpdDropItemBag	*GetCurrentBag( eAgpmDropItemBag eBag );
//	void SetDropDataFromDropTemplate( eAgpmDropItemBag eBag, int iRow );
//	void GetDropDataFromDialog( int iRow ); 

	AgcmDropItemDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmDropItemDlg)
	enum { IDD = IDD_DIALOG_DROP_ITEM };
	CListBox	m_cDropItemSelectList;
	CEdit	m_cDropItemSelectEdit;
	CEdit	m_cEquipItemCount9Edit;
	CEdit	m_cEquipItemCount8Edit;
	CEdit	m_cEquipItemCount7Edit;
	CEdit	m_cEquipItemCount6Edit;
	CEdit	m_cEquipItemCount5Edit;
	CEdit	m_cEquipItemCount4Edit;
	CEdit	m_cEquipItemCount3Edit;
	CEdit	m_cEquipItemCount2Edit;
	CEdit	m_cEquipItemCount1Edit;
	CEdit	m_cEquipItemCount10Edit;
	CEdit	m_cEquipItemCount0Edit;
	CEdit	m_cRuneOptDefensePoint;
	CEdit	m_cRuneOptAttackPoint;
	CEdit	m_cRuneOptAttackSpeed;
	CEdit	m_cRuneOptDamage;
	CEdit	m_cRuneOptDefenseRate;
	CEdit	m_cRuneOptHP;
	CEdit	m_cRuneOptHPRegen;
	CEdit	m_cRuneOptMaxAttackPoint;
	CEdit	m_cRuneOptMaxAttackSpeed;
	CEdit	m_cRuneOptMaxDamage;
	CEdit	m_cRuneOptMaxDefensePoint;
	CEdit	m_cRuneOptMaxDefenseRate;
	CEdit	m_cRuneOptMaxHP;
	CEdit	m_cRuneOptMaxHPRegen;
	CEdit	m_cRuneOptMaxMP;
	CEdit	m_cRuneOptMaxMPRegen;
	CEdit	m_cRuneOptMaxSP;
	CEdit	m_cRuneOptMaxSPRegen;
	CEdit	m_cRuneOptMinAttackSpeed;
	CEdit	m_cRuneOptMinAttackPoint;
	CEdit	m_cRuneOptMinDamageEdit;
	CEdit	m_cRuneOptMinDefensePoint;
	CEdit	m_cRuneOptMinDefenseRate;
	CEdit	m_cRuneOptMinHP;
	CEdit	m_cRuneOptMinHPRegen;
	CEdit	m_cRuneOptMinMP;
	CEdit	m_cRuneOptMinMPRegen;
	CEdit	m_cRuneOptMinSP;
	CEdit	m_cRuneOptMinSPRegen;
	CEdit	m_cRuneOptMP;
	CEdit	m_cRuneOptMPRegen;
	CEdit	m_cRuneOptSP;
	CEdit	m_cRuneOptSPRegen;
	CListCtrl	m_cDropItemList;
	CEdit	m_cRuneCount9Edit;
	CEdit	m_cRuneCount8Edit;
	CEdit	m_cRuneCount7Edit;
	CEdit	m_cRuneCount6Edit;
	CEdit	m_cRuneCount5Edit;
	CEdit	m_cRuneCount4Edit;
	CEdit	m_cRuneCount3Edit;
	CEdit	m_cRuneCount2Edit;
	CEdit	m_cRuneCount1Edit;
	CEdit	m_cRuneCount10Edit;
	CEdit	m_cRuneCount0Edit;
	CEdit	m_cItemCount9Edit;
	CEdit	m_cItemCount8Edit;
	CEdit	m_cItemCount7Edit;
	CEdit	m_cItemCount6Edit;
	CEdit	m_cItemCount5Edit;
	CEdit	m_cItemCount4Edit;
	CEdit	m_cItemCount3Edit;
	CEdit	m_cItemCount2Edit;
	CEdit	m_cItemCount1Edit;
	CEdit	m_cItemCount10Edit;
	CEdit	m_cItemCount0Edit;
	CButton	m_cSpecialRadio;
	CButton	m_cRareRadio;
	CButton	m_cFreeForAllRadio;
	CButton	m_cCommonRadio;
	CEdit	m_cJackpotPieceEdit;
	CEdit	m_cJackpotRateEdit;
	CEdit	m_cMaxQuantityEdit;
	CEdit	m_cMinQuantityEdit;
	CEdit	m_cEquipRateEdit;
	CEdit	m_cDropRateEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmDropItemDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmDropItemDlg)
	afx_msg void OnDropItemDeleteButton();
	afx_msg void OnItemDropAddButton();
	afx_msg void OnItemDropUpdateButton();
	afx_msg void OnDoubleclickedItemDropCloseButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemDropAddUpdateButton();
	afx_msg void OnFreeForAllRadio();
	afx_msg void OnCommonRadio();
	afx_msg void OnRareRadio();
	afx_msg void OnSpecialRadio();
	afx_msg void OnItemDropCloseButton();
	afx_msg void OnClickDropItemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeDropItemSelectEdit();
	afx_msg void OnSelchangeDropItemSelectList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMDROPITEMDLG_H__419D768A_46FE_4AEA_BF64_E0A849F3A39D__INCLUDED_)
