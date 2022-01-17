#pragma once
#include "afxcmn.h"
#include "XListCtrl.h" 
#include "AgcEngineChild.h"

using namespace std;

class CSaveDlgSet
{
public:
	BOOL m_bCharacerTemplate;
	BOOL m_bItemTemplate;
	BOOL m_bObjectTemplate;
	BOOL m_bSkillTemplate;
	BOOL m_bRideList;

	BOOL m_bLODCharacter;
	BOOL m_bLODItem;
	BOOL m_bLODObject;

	BOOL m_bLODGenBoundary;
	BOOL m_bLODCheckLevel;

	BOOL m_bCharGenHeight;
	BOOL m_bCharGenHitRange;

	BOOL m_bCharGenBSphere;
	BOOL m_bItemGenBSphere;
	BOOL m_bObjGenBSphere;

	BOOL m_bCharGeometryData;
	BOOL m_bItemGeometryData;
	BOOL m_bObjGeometryData;

	BOOL m_bCharOcTreeData;
	BOOL m_bItemOcTreeData;
	BOOL m_bObjtOcTreeData;

	BOOL m_bOldTypeSave;

	vector< AgpdCharacterTemplate * >	vecCharacterTemplate;
	vector< AgpdItemTemplate * >		vecItemTemplate		;
	vector< ApdObjectTemplate * >		vecObjectTemplate	;
	
	CSaveDlgSet():
		m_bCharacerTemplate( FALSE ) ,
		m_bItemTemplate( FALSE ) ,
		m_bObjectTemplate( FALSE ) ,
		m_bSkillTemplate( FALSE ) ,
		m_bRideList( FALSE ) ,

		m_bLODCharacter( FALSE ) ,
		m_bLODItem( FALSE ) ,
		m_bLODObject( FALSE ) ,

		m_bLODGenBoundary( FALSE ) ,
		m_bLODCheckLevel( FALSE ) ,

		m_bCharGenHeight( FALSE ) ,
		m_bCharGenHitRange( FALSE ) ,

		m_bCharGenBSphere( FALSE ) ,
		m_bItemGenBSphere( FALSE ) ,
		m_bObjGenBSphere( FALSE ) ,

		m_bCharGeometryData( FALSE ) ,
		m_bItemGeometryData( FALSE ) ,
		m_bObjGeometryData( FALSE ) ,

		m_bCharOcTreeData( FALSE ) ,
		m_bItemOcTreeData( FALSE ) ,
		m_bObjtOcTreeData( FALSE ) ,

		m_bOldTypeSave( FALSE )
	{
		
	}
};

class CSaveDlg : public CDialog
{
// Construction
public:
	CSaveDlg(CSaveDlgSet *pcsSet, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveDlg)
	enum { IDD = IDD_SAVE };
	BOOL	m_bCharacterTemplate;
	BOOL	m_bItemTemplate;
	BOOL	m_bObjectTemplate;
	BOOL	m_bLODCharacter;
	BOOL	m_bLODItem;
	BOOL	m_bLODObject;
	BOOL	m_bSkillTemplate;
	BOOL	m_bCharGenHeight;
	BOOL	m_bCharGenHitRange;
	BOOL	m_bObjGenBSphere;
	BOOL	m_bItemGenBSphere;
	BOOL	m_bCharGenBSphere;
	BOOL	m_bGenLODBoundary;
	BOOL	m_bCheckLODLevel;
	BOOL	m_bSaveCharGeomData;
	BOOL	m_bSaveItemGeomData;
	BOOL	m_bSaveObjtGeomData;
	BOOL	m_bSaveCharOctreeData;
	BOOL	m_bSaveItemOctreeData;
	BOOL	m_bSaveObjtOctreeData;	
	BOOL	m_bOldTypeSave;	
	//}}AFX_DATA

protected:
	CSaveDlgSet	*m_pcsSaveDlgSet;

	CComboBox m_comboNationalCode;

	void	InitListControl();
	void	ListingUnsavedData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveDlg)
	afx_msg void OnButtonSaveCheckAll();
	afx_msg void OnButtonSaveCheckLod();
	afx_msg void OnButtonSaveCheckTemplate();
	virtual void OnOK();
	afx_msg void OnButtonSaveCheckCharacter();
	afx_msg void OnBUTTONSAVECHECKItem();
	afx_msg void OnButtonSaveCheckObject();
	afx_msg void OnButtonSaveCheckSkill();
	afx_msg void OnButtonSaveCheckBsphere();
	afx_msg void OnButtonSaveCheckGeometry();
	afx_msg void OnButtonSaveCheckOctree();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL m_bSaveRideList;
public:
	virtual BOOL OnInitDialog();
	CXListCtrl m_ListCtrl;

	enum Column
	{
		COLUMN_TYPE	,
		COLUMN_TID	,
		COLUMN_NAME	,
		COLUMN_SAVE	,
		COLUMN_STATE
	};
};

// 세이브 체크~
template< typename T >
class	ForEachTemplate
{
public:
	ForEachTemplate( AgcEngineChild &csModuleManager , CXListCtrl &csCtrl , int &nSeq):
	  m_csModuleManager( csModuleManager ), _csCtrl( csCtrl ), _nSequence( nSeq ){}

	BOOL	operator()()
	{
		// 작업..
		INT32	lIndex = 0;
		for (T	*pcsTemplate = GetSequence(lIndex); pcsTemplate; pcsTemplate = GetSequence(lIndex))
			if( !_DoForEach( pcsTemplate) ) return FALSE;
		return TRUE;
	}
private:
	// 각기 다른 이름의 펑션에 대한 인터페이스..
	virtual	T *	GetSequence( INT32	&lIndex ) = 0;
	virtual	const char * GetTypeName() = 0;
	virtual DWORD	GetTypeColor() = 0;

	BOOL _DoForEach( T * pcsTemplate );

protected:
	AgcEngineChild			&m_csModuleManager			;
	CXListCtrl &_csCtrl;
	int	&_nSequence;
};

class	ForEachCharacterTemplate : public ForEachTemplate< AgpdCharacterTemplate >
{
public:
	ForEachCharacterTemplate( AgcEngineChild &csModuleManager , CXListCtrl &csCtrl , int &nSeq ):
		ForEachTemplate< AgpdCharacterTemplate >( csModuleManager , csCtrl , nSeq )
	{_pcsAgpmCharacter= dynamic_cast< AgpmCharacter *>( m_csModuleManager.GetModule( "AgpmCharacter") );}

private:
	virtual	AgpdCharacterTemplate *	GetSequence( INT32	&lIndex	){ return _pcsAgpmCharacter->GetTemplateSequence(&lIndex); }
	virtual	const char * GetTypeName() { return "C"; }
	virtual DWORD	GetTypeColor() { return  RGB( 255 , 128 , 128 ); }

protected:
	AgpmCharacter * _pcsAgpmCharacter;

};

class	ForEachObjectTemplate : public ForEachTemplate< ApdObjectTemplate >
{
public:
	ForEachObjectTemplate( AgcEngineChild &csModuleManager , CXListCtrl &csCtrl , int &nSeq ):
		ForEachTemplate< ApdObjectTemplate >( csModuleManager , csCtrl , nSeq )
	{_pcsApmObject= dynamic_cast< ApmObject *>( m_csModuleManager.GetModule( "ApmObject") );}

private:
	virtual	ApdObjectTemplate *	GetSequence( INT32	&lIndex	){ return _pcsApmObject->GetObjectTemplateSequence(&lIndex); }
	virtual	const char * GetTypeName() { return "O"; }
	virtual DWORD	GetTypeColor() { return  RGB( 128 , 255 , 128 ); }

protected:
	ApmObject * _pcsApmObject;
};

class	ForEachItemTemplate : public ForEachTemplate< AgpdItemTemplate >
{
public:
	ForEachItemTemplate( AgcEngineChild &csModuleManager , CXListCtrl &csCtrl , int &nSeq ):
		ForEachTemplate< AgpdItemTemplate >( csModuleManager , csCtrl , nSeq )
	{_pcsAgpmItem= dynamic_cast< AgpmItem *>( m_csModuleManager.GetModule( "AgpmItem") );}

private:
	virtual	AgpdItemTemplate *	GetSequence( INT32	&lIndex	){ return _pcsAgpmItem->GetTemplateSequence(&lIndex); }
	virtual	const char * GetTypeName() { return "I"; }
	virtual DWORD	GetTypeColor() { return  RGB( 128 , 128 , 255 ); }

protected:
	AgpmItem * _pcsAgpmItem;
};

