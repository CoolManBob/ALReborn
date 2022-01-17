#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "AgpmItem.h"
#include "ContainerUtil.h"
#include "AgpmEventSkillMaster.h"


enum eInventoryType
{
	Inventory_UnKnown = 0,
	Inventory_Normal,
	Inventory_Equip,
	Inventory_Cash,
	Inventory_Pet,
	Inventory_Bank,
	Inventory_Sales,
	Inventory_MailBox,
};


struct stItemEntry
{
	stAgpdAdminItem						m_Item;
	
	stItemEntry( void )
	{
		memset( &m_Item, 0, sizeof( stAgpdAdminItem ) );
	}
};


// AlefAdminItem 폼 뷰입니다.

class AlefAdminItem : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminItem)

protected:
	AlefAdminItem();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminItem();

public:
	enum { IDD = IDD_ITEM };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	AgpmItem* m_pcsAgpmItem;
	AgpmSkill* m_pcsAgpmSkill;
	AgpmEventSkillMaster* m_pcsAgpmEventSkillMaster;

	vector<INT32> m_vectorCategory1;
	vector<INT32> m_vectorCategory2;

	stAgpdAdminItem m_stSelectedItem;

public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* msg);

	BOOL InitList();
	BOOL InitGrid();
	BOOL InitControls();

	BOOL InitPosCB();
	BOOL InitConvertList(INT32 lMaxSocket = 0);
	BOOL InitConvertPhysicalCB();
	BOOL InitConvertSocketCB();
	BOOL InitConvertStoneCB();

	BOOL InitDeleteReasonCB();
	BOOL InitCreateReasonCB();

	//BOOL InitCreateFirstCB();
	//BOOL InitCreateSecondCB();
	//BOOL InitCreateThirdCB();

	BOOL InitItemListCB();
	BOOL InitCashItemCreateOption();
	BOOL InitExpireDate();
	
	BOOL SetCashItemControls(AgpdItemTemplate* pcsItemTemplate);
	BOOL SetEnableRemainTime(BOOL bEnable = TRUE);
	BOOL SetEnableExpireDate(BOOL bEnable = TRUE);

	BOOL SetDefaultRemainTime(INT64 lRemainTime);
	BOOL ClearRemainTime();

	BOOL InitOptionAllList();
	BOOL InitSkillAllList();

	// 다른 ItemTemplate 관련 함수는 Manager 로 이동
	//stAgpdAdminItemTemplate* GetItemTemplate(const char* szItemName);
	AgpdItemTemplate* GetItemTemplate(INT32 lTID);
	AgpdItemTemplate* GetItemTemplate(CHAR* szItemName);

	INT32 GetItemRank(stAgpdAdminItem* pstItem);
	CHAR* GetItemRankName(stAgpdAdminItem* pstItem);
	CHAR* GetItemRankName(INT32 lItemRank);

	BOOL OnReceive(AgpdItem* pcsItem);
	BOOL OnReceive(stAgpdAdminItem* pstItem);
	BOOL SetItemData(AgpdItem* pcsItem, stAgpdAdminItem* pstItem);
	BOOL SetItemTemplate(stAgpdAdminItem* pstItem);
	BOOL SelectItem(stAgpdAdminItem* pstItem);
	BOOL IsSameWithSelectedItem(stAgpdAdminItem* pstItem);

	INT32 GetListCtrlIndex(stAgpdAdminItem* pstItem);
	BOOL GetItemPosByIndex(stAgpdAdminItem* pstItem, int iLVIndex);
	BOOL GetEquipPosByIndex(stAgpdAdminItem* pstItem, int iLVIndex);

	BOOL ShowData(stAgpdAdminItem* pstItem);
	BOOL ShowData(INT32 lTypeIndex);
	BOOL ShowUpdateByPosition(stAgpdAdminItem* pstItem);
	BOOL ShowUpdateByItemDBID(stAgpdAdminItem* pstItem);
	BOOL InitItemList(INT32 lTypeIndex);
	BOOL ClearItemListCtrl();

	BOOL ShowDurability(stAgpdAdminItem* pstItem);
	BOOL ClearDurability();

	BOOL ShowSkillPlus(stAgpdAdminItem* pstItem);
	BOOL ClearSkillPlus();

	BOOL ShowConvertData(stAgpdAdminItem* pstItem);
	BOOL ShowConvertComboBox(stAgpdAdminItem* pstItem);
	BOOL ClearConvertData();
	INT32 GetMaxSocket(stAgpdAdminItem* pstItem);
	BOOL IsPhysicalConvertable(stAgpdAdminItem* pstItem);

	BOOL ShowAddedOptionData(stAgpdAdminItem* pstItem);
	BOOL ClearAddedOptionData();
	BOOL ClearAddedSkillData();
	BOOL ShowOptionDesc(INT32* pclOptionTID, INT32 lCount);
	BOOL InitSelectedOption();
	BOOL InitSelectedSkill();

	BOOL GetSelctedItemPosition(stAgpdAdminItemOperation* pstItemOperation);
	INT32 GetNowSetPhysicalConvertLevel();
	INT32 GetNowSetSocketNum();
	INT32 GetNowSetOption(stAgpdAdminItemOperation* pstItemOperation);
	INT32 GetNowSetSkill(stAgpdAdminItemOperation* pstItemOperation);

	BOOL GetCashUsingAndTimeDate(stAgpdAdminItemOperation* pstItemOperation);

	BOOL OnReceiveResult(stAgpdAdminItemOperation* pstItemOperation);

public :
	ContainerVector< stItemEntry >					m_vecItemInventory_Normal;
	ContainerVector< stItemEntry >					m_vecItemInventory_Equip;
	ContainerVector< stItemEntry >					m_vecItemInventory_Cash;
	ContainerVector< stItemEntry >					m_vecItemInventory_Pet;
	ContainerVector< stItemEntry >					m_vecItemInventory_Bank;
	ContainerVector< stItemEntry >					m_vecItemInventory_Sales;
	ContainerVector< stItemEntry >					m_vecItemInventory_MailBox;

public :
	BOOL				OnAddItemToInventory		( eInventoryType eInventory, stAgpdAdminItem* ppdItem );
	BOOL				OnShowInventory				( eInventoryType eInventory );
	BOOL				OnClearInventory			( eInventoryType eInventory );
	BOOL				OnUpdateItemPosition		( eInventoryType eInventory, stAgpdAdminItem* ppdItem );

public :
	stAgpdAdminItem*	GetItem						( eInventoryType eInventory, stAgpdAdminItem* ppdItem );
	stAgpdAdminItem*	GetItem						( eInventoryType eInventory, INT16 nLayer, INT16 nRow, INT16 nCol );

private :
	void*				_SelectInventory			( eInventoryType eInventory );
	BOOL				_IsSameItem					( stAgpdAdminItem* ppdItem1, stAgpdAdminItem* ppdItem2 );
	BOOL				_FindEmptySlot				( eInventoryType eInventory, stAgpdAdminItemOperation* ppdItemOperation );

	BOOL				_UpdateItemPosition_Normal	( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_Equip	( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_Cash	( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_Pet		( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_Bank	( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_Sales	( stAgpdAdminItem* ppdItem );
	BOOL				_UpdateItemPosition_MailBox	( stAgpdAdminItem* ppdItem );

	INT16				_GetInventoryMaxLayer		( eInventoryType eInventory );
	INT16				_GetInventoryMaxRow			( eInventoryType eInventory );
	INT16				_GetInventoryMaxCol			( eInventoryType eInventory );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CXTFlatComboBox m_csItemPosCB;
	CXTButton m_csRefreshBtn;
	CXTListCtrl m_csItemList;
	CXTFlatHeaderCtrl m_flatHeader;

	CXTListCtrl m_csConvertList;
	CXTFlatHeaderCtrl m_flatHeader2;
	CXTButton m_csConvertRefreshBtn;
	CXTButton m_csItemDeleteBtn;
	CXTFlatComboBox m_csConvertPhysicalCB;
	CXTFlatComboBox m_csConvertSocketCB;
	CXTFlatComboBox m_csConvertSpiritStoneCB;
	CXTFlatComboBox m_csConvertStoneCB;
	CXTButton m_csConvertBtn;
	CXTFlatComboBox m_csDeleteReasonCB;
	CXTButton m_csItemUpdateBtn;

	//CXTFlatComboBox m_csCreateFirstCB;
	//CXTFlatComboBox m_csCreateSecondCB;
	//CXTFlatComboBox m_csCreateThirdCB;
	CXTButton m_csItemSearchBtn;
	CXTFlatComboBox m_csItemListCB;
	CButton m_csChkUsingBtn;
	CStatic m_csRemainTimeStatic;
	CEdit m_csRemainTimeDayEdit;
	CEdit m_csRemainTimeHourEdit;
	CEdit m_csRemainTimeMinEdit;
	CEdit m_csRemainTimeSecEdit;
	CXTFlatComboBox m_csExpireDateDefaultCB;
	CXTFlatComboBox m_csExpireDateYearCB;
	CXTFlatComboBox m_csExpireDateMonthCB;
	CXTFlatComboBox m_csExpireDateDayCB;
	CXTFlatComboBox m_csExpireDateHourCB;
	CXTFlatComboBox m_csExpireDateMinCB;
	CXTButton m_csCreateBtn;
	CXTFlatComboBox m_csCreateReasonCB;

	CListBox m_csOptionAddedList;
	CListBox m_csOptionAllList;
	CXTButton m_csOptionAddBtn;
	CXTButton m_csOptionRemoveBtn;
	CXTButton m_csOptionSearchBtn;

	CXTFlatComboBox m_csSkillPlusCB;

	CListBox m_csSkillAddedList;
	CListBox m_csSkillAllList;
	CXTButton m_csSkillAddBtn;
	CXTButton m_csSkillRemoveBtn;
	CXTButton m_csSkillSearchBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	
	afx_msg void OnCbnSelchangeCbPos();
	//afx_msg void OnCbnSelchangeCbCreateFirst();
	//afx_msg void OnCbnSelchangeCbCreateSecond();
	//afx_msg void OnCbnSelchangeCbCreateThird();
	//afx_msg void OnCbnEditchangeCbCreateThird();
	afx_msg void OnBnClickedBCreate();
	afx_msg void OnNMClickLcItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeCbConvertSocket();
	afx_msg void OnCbnSelchangeCbConvertPhysical();
	afx_msg void OnBnClickedBConvert();
	afx_msg void OnCbnSelchangeCbConvertSpiritStone();
	afx_msg void OnCbnSelchangeCbConvertStone();
	afx_msg void OnBnClickedBRefreshConvert();
	afx_msg void OnBnClickedBDelete();
	afx_msg void OnBnClickedBRefresh();
	afx_msg void OnNMClickLItemConvert(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeLOptionAdded();
	afx_msg void OnLbnSelchangeLOptionAll();
	afx_msg void OnBnClickedBOptionAdd();
	afx_msg void OnBnClickedBOptionRemove();
	afx_msg void OnBnClickedOptionSearch();
	afx_msg void OnCbnSelchangeCbItemList();
	afx_msg void OnBnClickedBItemSearch();
	afx_msg void OnBnClickedChkUsing();
	afx_msg void OnCbnSelchangeCbExpireDateDefault();
	afx_msg void OnBnClickedBItemUpdate();
	afx_msg void OnBnClickedBSkillAdd();
	afx_msg void OnBnClickedBSkillRemove();
	afx_msg void OnBnClickedSkillSearch();
};


