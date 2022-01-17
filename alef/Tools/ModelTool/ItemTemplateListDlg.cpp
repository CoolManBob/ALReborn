// ItemTemplateListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "ItemTemplateListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemTemplateListDlg dialog

CItemTemplateListDlg::CItemTemplateListDlg(INT32 *plTID, CWnd* pParent /*=NULL*/)
	: CDialog(CItemTemplateListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemTemplateListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_plTID = plTID;
}

void CItemTemplateListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemTemplateListDlg)
	DDX_Control(pDX, IDC_ITEM_TEMPLATE_LIST, m_csList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemTemplateListDlg, CDialog)
	//{{AFX_MSG_MAP(CItemTemplateListDlg)
	ON_LBN_DBLCLK(IDC_ITEM_TEMPLATE_LIST, OnDblclkItemTemplateList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemTemplateListDlg message handlers

void CItemTemplateListDlg::OnOK() 
{
	*(m_plTID) = m_csList.GetItemData(m_csList.GetCurSel());

	CDialog::OnOK();
}

void CItemTemplateListDlg::OnDblclkItemTemplateList() 
{
	OnOK();
}

BOOL CItemTemplateListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	if(!CModelToolApp::GetInstance())
		return FALSE;

	AgpmItem *pcsAgpmItem = CModelToolApp::GetInstance()->GetEngine()->GetAgpmItemModule();
	if(!pcsAgpmItem)
		return FALSE;

	INT16 nItem;
	INT32 lIndex = 0;
	for(AgpdItemTemplate *pcsAgpdItemTemplate = pcsAgpmItem->GetTemplateSequence(&lIndex); pcsAgpdItemTemplate; pcsAgpdItemTemplate = pcsAgpmItem->GetTemplateSequence(&lIndex))
	{
		//char strDebug[ 256 ] = { 0, };
		//sprintf( strDebug, "Add List Item : Name = %s", pcsAgpdItemTemplate->GetName() );
		//OutputDebugString( strDebug );

		if( _IsAcceptFilter( pcsAgpdItemTemplate ) )
		{
			nItem = m_csList.AddString(pcsAgpdItemTemplate->m_szName);
			m_csList.SetItemData(nItem, pcsAgpdItemTemplate->m_lID);

			//OutputDebugString( "  -->  Added" );
		}

		//OutputDebugString( "\n" );
	}

	return TRUE;
}


BOOL CItemTemplateListDlg::_IsAcceptFilter( AgpdItemTemplate* pcsAgpdItemTemplate )
{
	if( !pcsAgpdItemTemplate ) return FALSE;

	// 장착 불가능한 물건은 안됨
	if( pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;

	AgpdItemTemplateEquip* pcsAgpdItemTemplateEquip = ( AgpdItemTemplateEquip* )pcsAgpdItemTemplate;

	//char strDebug[ 256 ] = { 0, };
	//sprintf( strDebug, ", PartsType = %d", pcsAgpdItemTemplateEquip->m_nPart );
	//OutputDebugString( strDebug );

	// 지정된 타입이 아닌경우..
	if( pcsAgpdItemTemplateEquip->m_nPart != m_lEquipType )
	{
		// 특정 타입은 해당 타입의 _V_ 타입을 커버해야 함
		// 동일 타입은 위의 if 문에서 걸러지니 비교대상이 _V_ 타입인가만 검사
		switch( pcsAgpdItemTemplateEquip->m_nPart )
		{
		case AGPMITEM_PART_BODY :				if( m_lEquipType != AGPMITEM_PART_V_BODY ) return FALSE;				break;
		case AGPMITEM_PART_HEAD :				if( m_lEquipType != AGPMITEM_PART_V_HEAD ) return FALSE;				break;
		case AGPMITEM_PART_ARMS :				if( m_lEquipType != AGPMITEM_PART_V_ARMS ) return FALSE;				break;
		case AGPMITEM_PART_HANDS :				if( m_lEquipType != AGPMITEM_PART_V_HANDS ) return FALSE;				break;
		case AGPMITEM_PART_LEGS :				if( m_lEquipType != AGPMITEM_PART_V_LEGS ) return FALSE;				break;
		case AGPMITEM_PART_FOOT :				if( m_lEquipType != AGPMITEM_PART_V_FOOT ) return FALSE;				break;
		case AGPMITEM_PART_ARMS2 :				if( m_lEquipType != AGPMITEM_PART_V_ARMS2 ) return FALSE;				break;
		case AGPMITEM_PART_HAND_LEFT :			if( m_lEquipType != AGPMITEM_PART_V_HAND_LEFT ) return FALSE;			break;
		case AGPMITEM_PART_HAND_RIGHT :			if( m_lEquipType != AGPMITEM_PART_V_HAND_RIGHT ) return FALSE;			break;
		case AGPMITEM_PART_ACCESSORY_RING1 :
		case AGPMITEM_PART_ACCESSORY_RING2 :
			{
				// 반지의 경우 1,2 를 동일타입으로 간주하여 처리
				if( m_lEquipType == AGPMITEM_PART_ACCESSORY_RING1 || m_lEquipType == AGPMITEM_PART_ACCESSORY_RING2 ||
					m_lEquipType == AGPMITEM_PART_V_ACCESSORY_RING1 || m_lEquipType == AGPMITEM_PART_V_ACCESSORY_RING2 )
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;
		case AGPMITEM_PART_ACCESSORY_NECKLACE :	if( m_lEquipType != AGPMITEM_PART_V_ACCESSORY_NECKLACE ) return FALSE;	break;
		case AGPMITEM_PART_RIDE :				if( m_lEquipType != AGPMITEM_PART_V_RIDE ) return FALSE;				break;
		case AGPMITEM_PART_LANCER :				if( m_lEquipType != AGPMITEM_PART_V_LANCER ) return FALSE;				break;
		default :								return FALSE;															break;
		}
	}

	return TRUE;
}
