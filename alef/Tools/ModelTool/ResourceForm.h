#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "ResourceTree.h"
#include "EventButton.h"
#include "AgpdItem.h"
#include "ResourceSaver.h"

class AgcdItemTemplate;
class AgpdItemTemplate;

class CResourceForm : public CFormView
{
	CResourceForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResourceForm)

public:
	enum {
		CHAR_COMMON,			//케릭터 공통
		CHAR_MOONELF_SHA,		//문엘프 여자 법사
		CHAR_MOONELF_HUN,		//문엘프 여자 궁수
		CHAR_MOONELF_SB,		//문엘프 여자 전사
		CHAR_ORC_SHA,			//오크 남자마법사
		CHAR_ORC_WAR,			//오크 남자전사
		CHAR_ORC_HUN,			//오크 여자궁수
		CHAR_HUMAN_ARC,			//휴먼 남자궁수
		CHAR_HUMAN_KNI,			//휴먼 남자전사
		CHAR_HUMAN_WIZ,			//휴먼 여자마법사
		CHAR_DRAGONSCION_SCION,	//드레곤 시온
		CHAR_DRAGONSCION_SLA,	//드레곤 전사
		CHAR_DRAGONSCION_OBI,	//드레곤 궁수
		CHAR_DRAGONSCION_SUM,	//드레곤 마법사
		CHAR_NUM,
	};
	CResourceForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);

protected:
	HBRUSH			m_hBackBrush;

	CToolBarCtrl	*m_pcsResourceToolBar;
	CBitmap			*m_pcsResourceToolBarBitmap;
	CToolTipCtrl	*m_pcsResourceToolTip;

	CResourceTree	*m_pcsTreeCurrent;
	CResourceTree	*m_pcsTreeData;
	CResourceTree	*m_pcsTreeCharacter;
	CResourceTree	*m_pcsTreeItem;
	CResourceTree	*m_pcsTreeObject;
	CResourceTree	*m_pcsTreeSkill;

	HTREEITEM		m_hData1;
	HTREEITEM		m_hData2;

	HTREEITEM		m_hCharacter;

	HTREEITEM		m_hItem;
	HTREEITEM		m_ahItemChar[CHAR_NUM];

	HTREEITEM		m_ahEquip[CHAR_NUM];
	HTREEITEM		m_ahArmour[CHAR_NUM];
	HTREEITEM		m_ahBody[CHAR_NUM];
	HTREEITEM		m_ahHead[CHAR_NUM];
	HTREEITEM		m_ahArms[CHAR_NUM];
	HTREEITEM		m_ahHands[CHAR_NUM];
	HTREEITEM		m_ahLegs[CHAR_NUM];
	HTREEITEM		m_ahFoot[CHAR_NUM];
	HTREEITEM		m_ahArms2[CHAR_NUM];
	HTREEITEM		m_hShield;

	HTREEITEM		m_hUsable;
	HTREEITEM		m_hOther;

	HTREEITEM		m_hWeaponParent;
	HTREEITEM		m_hPcWeaponParent;
	HTREEITEM		m_hMonsterWeaponParent;
	HTREEITEM		m_ahWeapon[AGPMITEM_EQUIP_WEAPON_TYPE_NUM];
	HTREEITEM		m_ahMonsterWeapon[AGPMITEM_EQUIP_WEAPON_TYPE_NUM];
	HTREEITEM		m_hRing;
	HTREEITEM		m_hNecklace;
	HTREEITEM		m_hRide;

	HTREEITEM		m_hObject;

	HTREEITEM		m_hSkill;

	CResourceSaver	m_cResourceSaver;

public:
	CResourceTree*	GetTreeCurrent()								const	{	return m_pcsTreeCurrent;	}
	CResourceTree*	GetTreeData()									const	{	return m_pcsTreeData;		}
	CResourceTree*	GetTreeCharacter()								const	{	return m_pcsTreeCharacter;	}
	CResourceTree*	GetTreeItem()									const	{	return m_pcsTreeItem;		}
	CResourceTree*	GetTreeObject()									const	{	return m_pcsTreeObject;		}
	CResourceTree*	GetTreeSkill()									const	{	return m_pcsTreeSkill;		}
	
	HTREEITEM		GetTreeHandleData1()							const	{	return m_hData1;		}
	HTREEITEM		GetTreeHandleData2()							const	{	return m_hData2;		}
	HTREEITEM		GetTreeHandleCharacter()						const	{	return m_hCharacter;	}

	HTREEITEM		GetTreeHandleItem()								const	{	return m_hItem;}
	HTREEITEM		GetTreeHandleItemChar( INT32 nCharID )			const	{	return m_ahItemChar[ nCharID ];	}
	HTREEITEM		GetTreeHandleEquip( INT32 nCharID )				const	{	return m_ahEquip[ nCharID ];	}
	HTREEITEM		GetTreeHandleArmour( INT32 nCharID )			const	{	return m_ahArmour[ nCharID ];	}
	HTREEITEM		GetTreeHandleBody( INT32 nCharID )				const	{	return m_ahBody[ nCharID ];		}
	HTREEITEM		GetTreeHandleHead( INT32 nCharID )				const	{	return m_ahHead[ nCharID ];		}
	HTREEITEM		GetTreeHandleArms( INT32 nCharID )				const	{	return m_ahArms[ nCharID ];		}
	HTREEITEM		GetTreeHandleHands( INT32 nCharID )				const	{	return m_ahHands[ nCharID ];	}
	HTREEITEM		GetTreeHandleLegs( INT32 nCharID )				const	{	return m_ahLegs[ nCharID ];		}
	HTREEITEM		GetTreeHandleFoot( INT32 nCharID )				const	{	return m_ahFoot[ nCharID ];		}
	HTREEITEM		GetTreeHandleArms2( INT32 nCharID )				const	{	return m_ahArms2[ nCharID ];	}
	HTREEITEM		GetTreeHandleShield()							const	{	return m_hShield;				}
	HTREEITEM		GetTreeHandleWeaponParent()						const	{	return m_hWeaponParent;			}
	HTREEITEM		GetTreeHandlePcWeaponParent()					const	{	return m_hPcWeaponParent;		}
	HTREEITEM		GetTreeHandleMonsterWeaponParent()				const	{	return m_hMonsterWeaponParent;			}
	HTREEITEM		GetTreeHandlePcWeapon( INT32 nWeaponType )		const	{	return m_ahWeapon[ nWeaponType ];		}
	HTREEITEM		GetTreeHandleMonsterWeapon( INT32 nWeaponType )	const	{	return m_ahMonsterWeapon[ nWeaponType ];}
	HTREEITEM		GetTreeHandleRing()								const	{	return m_hRing;		}
	HTREEITEM		GetTreeHandleNecklace()							const	{	return m_hNecklace;	}
	HTREEITEM		GetTreeHandleRide()								const	{	return m_hRide;		}

	HTREEITEM		GetTreeHandleUsable()							const	{	return m_hUsable;	}
	HTREEITEM		GetTreeHandleOther()							const	{	return m_hOther;	}

	HTREEITEM		GetTreeHandleObject()							const	{	return m_hObject;	}
	HTREEITEM		GetTreeHandleSkill()							const	{	return m_hSkill;	}

	HTREEITEM		GetTreeHandleItemParent( AgpdItemTemplate* pAgpdItemTemplate, AgcdItemTemplate* pAgcdItemTemplate );

	VOID			SetTreeHandleData1(HTREEITEM hSetHandle)		{	m_hData1 = hSetHandle;		}
	VOID			SetTreeHandleData2(HTREEITEM hSetHandle)		{	m_hData2 = hSetHandle;		}
	VOID			SetTreeHandleCharacter(HTREEITEM hSetHandle)	{	m_hCharacter = hSetHandle;	}

	VOID			SetTreeHandleItem( HTREEITEM hSetHandle )		{	m_hItem = hSetHandle;	}
	VOID			SetTreeHandleItemChar( INT32 nCharID, HTREEITEM hSetHandle )	{	m_ahItemChar[nCharID] = hSetHandle;	}
	VOID			SetTreeHandleEquip( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahEquip[nCharID] = hSetHandle;	}
	VOID			SetTreeHandleArmour( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahArmour[nCharID] = hSetHandle;	}
	VOID			SetTreeHandleBody( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahBody[nCharID] = hSetHandle;		}
	VOID			SetTreeHandleHead( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahHead[nCharID] = hSetHandle;		}
	VOID			SetTreeHandleArms( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahArms[nCharID] = hSetHandle;		}
	VOID			SetTreeHandleHands( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahHands[nCharID] = hSetHandle;	}
	VOID			SetTreeHandleLegs( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahLegs[nCharID] = hSetHandle;		}
	VOID			SetTreeHandleFoot( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahFoot[nCharID] = hSetHandle;		}
	VOID			SetTreeHandleArms2( INT32 nCharID, HTREEITEM hSetHandle )		{	m_ahArms2[nCharID] = hSetHandle;	}

	VOID			SetTreeHandleShield( HTREEITEM hSetHandle )				{	m_hShield = hSetHandle;			}
	VOID			SetTreeHandleWeaponParent( HTREEITEM hSetHandle )		{	m_hWeaponParent = hSetHandle;	}
	VOID			SetTreeHandlePcWeaponParent( HTREEITEM hSetHandle )		{	m_hPcWeaponParent = hSetHandle;	}
	VOID			SetTreeHandleMonsterWeaponParent( HTREEITEM hSetHandle ){	m_hMonsterWeaponParent = hSetHandle;	}
	VOID			SetTreeHandlePcWeapon( INT32 nWeaponType, HTREEITEM hSetHandle )		{	m_ahWeapon[nWeaponType] = hSetHandle;	}
	VOID			SetTreeHandleMonsterWeapon( INT32 nWeaponType, HTREEITEM hSetHandle )	{	m_ahMonsterWeapon[nWeaponType] = hSetHandle;	}
	VOID			SetTreeHandleUsable(HTREEITEM hSetHandle)				{	m_hUsable = hSetHandle;		}
	VOID			SetTreeHandleOther(HTREEITEM hSetHandle)				{	m_hOther = hSetHandle;		}
	VOID			SetTreeHandleRing(HTREEITEM hSetHandle)					{	m_hRing = hSetHandle;		}
	VOID			SetTreeHandleNecklace(HTREEITEM hSetHandle)				{	m_hNecklace = hSetHandle;	}
	VOID			SetTreeHandleRide(HTREEITEM hSetHandle)					{	m_hRide = hSetHandle;		}

	VOID			SetTreeHandleObject(HTREEITEM hSetHandle)				{	m_hObject = hSetHandle;		}
	VOID			SetTreeHandleSkill(HTREEITEM hSetHandle)				{	m_hSkill = hSetHandle;		}

	VOID			InsertItemEx(CResourceTree *pcsCurTree, CHAR *szFindPathName, HTREEITEM hParent, DWORD dwData = 0, int nImage = -1, int nSelectedImage = -1);

	VOID			UpdateCharacterResourceTree();
	VOID			UpdateItemResourceTree();
	VOID			UpdateObjectResourceTree();
	VOID			UpdateSkillResourceTree();

	BOOL			IsEquipments(HTREEITEM hItem);

	BOOL			ReadCharacterLabel(CHAR *szPathName);
	BOOL			SaveCharacterLabel(CHAR *szPathName, VOID (*pMsgFuncCB) (const CHAR* pMsg, VOID*) = NULL, CHAR* pParam1 = NULL);
	BOOL			ReadObjectLabel(CHAR *szPathName);
	BOOL			SaveObjectLabel(CHAR *szPathName, VOID (*pMsgFuncCB) (const CHAR* pMsg, VOID*) = NULL, CHAR* pParam1 = NULL);

	BOOL			ChangeLabel(HTREEITEM hParent);

	CResourceSaver&	GetResourceSaver()		{	return m_cResourceSaver;	}

protected:
	VOID InitializeMember();
	BOOL CreateMember();

	VOID FocusDataControl();
	VOID FocusCharacterControl();
	VOID FocusItemControl();
	VOID FocusObjectControl();
	VOID FocusSkillControl();

// Form Data
public:
	//{{AFX_DATA(CResourceForm)
	enum { IDD = IDD_RESOURCE_FORM };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResourceForm)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResourceForm();

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CResourceForm)
	afx_msg void OnDestroy();
	afx_msg void OnCommandPressDataButton();
	afx_msg void OnCommandPressCharacterButton();
	afx_msg void OnCommandPressItemButton();
	afx_msg void OnCommandPressObjectButton();
	afx_msg void OnCommandPressSkillButton();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
