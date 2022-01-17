#include "stdafx.h"
#include "modeltool.h"
#include "PropertyTree.h"
#include ".\propertytree.h"

CPropertyTree::CPropertyTree(CWnd *pcsParent, RECT &stInitRect)
{
	InitailzeMember();
	_Create(pcsParent, stInitRect, TVS_DISABLEDRAGDROP);
	CreateTree();
}

CPropertyTree::~CPropertyTree()
{
}

BEGIN_MESSAGE_MAP(CPropertyTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CPropertyTree)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
	//ON_WM_KEYDOWN()
END_MESSAGE_MAP()

VOID CPropertyTree::InitailzeMember()
{
	m_hParentDefault			= NULL;
	m_hParentAnimation			= NULL;
	m_hParentEffect				= NULL;
	m_hParentEquip				= NULL;
	m_hParentEvent				= NULL;
	m_hParentFactor				= NULL;
	m_hParentGeometry			= NULL;
	m_hParentLOD				= NULL;
	m_hParentSkill				= NULL;
	m_hParentTool				= NULL;
	m_hParentUserDataArray		= NULL;
	m_hParentDefaultHead		= NULL;
	m_hParentRide				= NULL;
	m_hNationalCode				= NULL;

	m_hCreate					= NULL;
	m_hDefaultProperty			= NULL;
	m_hObjectType				= NULL;
	m_hAnimationProgress		= NULL;
	m_hAnimationDialog			= NULL;
	m_hEffectDialog				= NULL;
	m_hEffectReload				= NULL;
	m_hDefaultEquipments		= NULL;
	m_hTranslateEquipments		= NULL;
	m_hEventInitialize			= NULL;
	m_hEventWork				= NULL;
	m_hFactorsDialog			= NULL;
	m_hEditCollisionData		= NULL;
	m_hEditOcTreeData			= NULL;
	m_hEditPickingData			= NULL;
	m_hCreateClump				= NULL;
	m_hLODDialog				= NULL;
	m_hSkillDialog				= NULL;
	m_hToolOffset				= NULL;
	m_hToolRenderOption			= NULL;
	m_hToolResetView			= NULL;
	m_hToolTargetOption			= NULL;
	m_hToolLightOption			= NULL;
	m_hRenderType				= NULL;
	m_hEditFace					= NULL;
	m_hEditHair					= NULL;
	m_hAttachFace				= NULL;
	m_hAttachHair				= NULL;

	m_hFaceRenderType			= NULL;
	m_hHairRenderType			= NULL;

	m_hCustomizePreviewType		= NULL;

	m_hAttachRideDialog			= NULL;

	for ( UINT uIdx = 0; uIdx < nMAX_NATIONAL_CODES; uIdx++ )
		m_hNationalCodeState[uIdx] = NULL;
}

VOID CPropertyTree::CreateTree()
{
	m_hParentDefault			= _InsertItem(PT_PARENT_ITEM_NAME_DEFAULT,			TVI_ROOT,				E_PT_ITEM_DATA_DEFAULT);
	m_hParentAnimation			= _InsertItem(PT_PARENT_ITEM_NAME_ANIMATION,		TVI_ROOT,				E_PT_ITEM_DATA_ANIMATION);
	m_hParentEffect				= _InsertItem(PT_PARENT_ITEM_NAME_EFFECT,			TVI_ROOT,				E_PT_ITEM_DATA_EFFECT);
	m_hParentEquip				= _InsertItem(PT_PARENT_ITEM_NAME_EQUIP,			TVI_ROOT,				E_PT_ITEM_DATA_EQUIP);
	m_hParentEvent				= _InsertItem(PT_PARENT_ITEM_NAME_EVENT,			TVI_ROOT,				E_PT_ITEM_DATA_EVENT);
	m_hParentFactor				= _InsertItem(PT_PARENT_ITEM_NAME_FACTOR,			TVI_ROOT,				E_PT_ITEM_DATA_FACTOR);
	m_hParentGeometry			= _InsertItem(PT_PARENT_ITEM_NAME_GEOMETRY,			TVI_ROOT,				E_PT_ITEM_DATA_GEOMETRY);
	m_hParentLOD				= _InsertItem(PT_PARENT_ITEM_NAME_LOD,				TVI_ROOT,				E_PT_ITEM_DATA_LOD);
	m_hParentSkill				= _InsertItem(PT_PARENT_ITEM_NAME_SKILL,			TVI_ROOT,				E_PT_ITEM_DATA_SKILL);
	m_hParentTool				= _InsertItem(PT_PARENT_ITEM_NAME_TOOL,				TVI_ROOT,				E_PT_ITEM_DATA_TOOL);
	m_hParentUserDataArray		= _InsertItem(PT_PARENT_ITEM_NAME_USER_DATA_ARRAY,	TVI_ROOT,				E_PT_ITEM_DATA_USER_DATA_ARRAY);
	m_hParentAppearance			= _InsertItem(PT_PARENT_ITEM_NAME_APPEARANCE,		TVI_ROOT,				E_PT_ITEM_DATA_APPEARANCE);
	m_hParentDefaultHead		= _InsertItem(PT_PARENT_ITEM_NAME_DEFAULT_HEAD,		TVI_ROOT,				E_PT_ITEM_DATA_DEFAULT_HEAD);
	m_hParentRide				= _InsertItem(PT_PARENT_ITEM_NAME_RIDE,				TVI_ROOT,				E_PT_ITEM_DATA_RIDE);
	m_hNationalCode				= _InsertItem(PT_PARENT_ITEM_NAME_NATIONAL_CODE,	TVI_ROOT,				E_PT_ITEM_DATA_NATIONAL_CODE);
	
	m_hCreate					= _InsertItem(PT_ITEM_NAME_CREATE,					m_hParentDefault,		E_PT_ITEM_DATA_CREATE);
	m_hDefaultProperty			= _InsertItem(PT_ITEM_NAME_DEFAULT_PROPERTY,		m_hParentDefault,		E_PT_ITEM_DATA_DEFAULT_PROPERTY);
	m_hObjectType				= _InsertItem(PT_ITEM_NAME_OBJECT_TYPE,				m_hParentDefault,		E_PT_ITEM_DATA_OBJECT_TYPE);

	m_hAnimationProgress		= _InsertItem(PT_ITEM_NAME_ANIMATION_PROGRESS,		m_hParentAnimation,		E_PT_ITEM_DATA_ANIMATION_PROGRESS);
	m_hAnimationDialog			= _InsertItem(PT_ITEM_NAME_ANIMATION_DIALOG,		m_hParentAnimation,		E_PT_ITEM_DATA_ANIMATION_DIALOG);

	m_hEffectReload				= _InsertItem(PT_ITEM_NAME_EFFECT_RELOAD,			m_hParentEffect,		E_PT_ITEM_DATA_EFFECT_RELOAD);
	m_hEffectDialog				= _InsertItem(PT_ITEM_NAME_EFFECT_DIALOG,			m_hParentEffect,		E_PT_ITEM_DATA_EFFECT_DIALOG);

	m_hDefaultEquipments		= _InsertItem(PT_ITEM_NAME_DEFAULT_EQUIPMENTS,		m_hParentEquip,			E_PT_ITEM_DATA_DEFAULT_EQUIPMENTS);
	m_hTranslateEquipments		= _InsertItem(PT_ITEM_NAME_TRANSLATE_EQUIPMENTS,	m_hParentEquip,			E_PT_ITEM_DATA_TRANSLATE_EQUIPMENTS);

	m_hEventInitialize			= _InsertItem(PT_ITEM_NAME_EVENT_INITAILIZE,		m_hParentEvent,			E_PT_ITEM_DATA_EVENT_INITAILIZE);
	m_hEventWork				= _InsertItem(PT_ITEM_NAME_EVENT_WORK,				m_hParentEvent,			E_PT_ITEM_DATA_EVENT_WORK);

	m_hFactorsDialog			= _InsertItem(PT_ITEM_NAME_FACTORS_DIALOG,			m_hParentFactor,		E_PT_ITEM_DATA_FACTORS_DIALOG);

	m_hEditCollisionData		= _InsertItem(PT_ITEM_NAME_EDIT_COLLISION_DATA,		m_hParentGeometry,		E_PT_ITEM_DATA_EDIT_COLLISION_DATA);
	m_hEditOcTreeData			= _InsertItem(PT_ITEM_NAME_EDIT_OCTREE_DATA,		m_hParentGeometry,		E_PT_ITEM_DATA_EDIT_OCTREE_DATA);
	m_hEditPickingData			= _InsertItem(PT_ITEM_NAME_EDIT_PICKING_DATA,		m_hParentGeometry,		E_PT_ITEM_DATA_EDIT_PICKING_DATA);
	m_hCreateClump				= _InsertItem(PT_ITEM_NAME_CREATE_CLUMP,			m_hParentGeometry,		E_PT_ITEM_DATA_CREATE_CLUMP);

	m_hLODDialog				= _InsertItem(PT_ITEM_NAME_LOD_DIALOG,				m_hParentLOD,			E_PT_ITEM_DATA_LOD_DIALOG);

	m_hSkillDialog				= _InsertItem(PT_ITEM_NAME_SKILL_DIALOG,			m_hParentSkill,			E_PT_ITEM_DATA_SKILL_DIALOG);

	m_hToolOffset				= _InsertItem(PT_ITEM_NAME_TOOL_OFFSET,				m_hParentTool,			E_PT_ITEM_DATA_TOOL_OFFSET);
	m_hToolRenderOption			= _InsertItem(PT_ITEM_NAME_TOOL_RENDER_OPTION,		m_hParentTool,			E_PT_ITEM_DATA_TOOL_RENDER_OPTION);
	m_hToolResetView			= _InsertItem(PT_ITEM_NAME_TOOL_RESET_VIEW,			m_hParentTool,			E_PT_ITEM_DATA_TOOL_RESET_VIEW);
	m_hToolTargetOption			= _InsertItem(PT_ITEM_NAME_TOOL_TARGET_OPTION,		m_hParentTool,			E_PT_ITEM_DATA_TOOL_TARGET_OPTION);
	m_hToolLightOption			= _InsertItem(PT_ITEM_NAME_TOOL_LIGHT_OPTION,		m_hParentTool,			E_PT_ITEM_DATA_TOOL_LIGHT_OPTION);

	m_hRenderType				= _InsertItem(PT_ITEM_NAME_RENDER_TYPE,				m_hParentUserDataArray,	E_PT_ITEM_DATA_RENDER_TYPE);

	m_hSetFace					= _InsertItem(PT_ITEM_NAME_SET_FACE,				m_hParentAppearance,	E_PT_ITEM_DATA_SET_FACE);

	m_hEditFace					= _InsertItem(PT_ITEM_NAME_EDIT_FACE,				m_hParentDefaultHead,	E_PT_ITEM_DATA_EDIT_FACE);
	m_hEditHair					= _InsertItem(PT_ITEM_NAME_EDIT_HAIR,				m_hParentDefaultHead,	E_PT_ITEM_DATA_EDIT_HAIR );

	m_hAttachFace				= _InsertItem(PT_ITEM_NAME_ATTACH_FACE,				m_hParentDefaultHead,	E_PT_ITEM_DATA_ATTACH_FACE);
	m_hAttachHair				= _InsertItem(PT_ITEM_NAME_ATTACH_HAIR,				m_hParentDefaultHead,	E_PT_ITEM_DATA_ATTACH_HAIR );

	m_hFaceRenderType			= _InsertItem(PT_ITEM_NAME_FACE_RENDER_TYPE,		m_hParentDefaultHead,	E_PT_ITEM_DATA_FACE_RENDER_TYPE );
	m_hHairRenderType			= _InsertItem(PT_ITEM_NAME_HAIR_RENDER_TYPE,		m_hParentDefaultHead,	E_PT_ITEM_DATA_HAIR_RENDER_TYPE );

	m_hCustomizePreviewType		= _InsertItem(PT_ITEM_NAME_CUSTOMIZE_PREVIEW_TYPE,	m_hParentDefaultHead,	E_PT_ITEM_DATA_CUSTOMIZE_PREVIEW );

	m_hAttachRideDialog			= _InsertItem(PT_ITEM_NAME_ATTACH_RIDE_DIALOG,		m_hParentRide,			E_PT_ITEM_DATA_ATTACH_RIDE_DIALOG );

	char pNCodeStr[256];		
	for ( UINT uIdx = 0; uIdx < nMAX_NATIONAL_CODES; uIdx++ )
	{
		sprintf( pNCodeStr, "%d. %s", uIdx + 1, g_pNationalCodeStr[uIdx] );
		m_hNationalCodeState[uIdx]	= _InsertItem(strlwr(pNCodeStr),				m_hNationalCode,		E_PT_ITEM_DATA_NATIONAL_CODE_KOREA + uIdx);
	}

	Expand(m_hParentDefault, TVE_EXPAND);		SetItemBold(m_hParentDefault, TRUE);
	Expand(m_hParentAnimation, TVE_EXPAND);		SetItemBold(m_hParentAnimation, TRUE);
	Expand(m_hParentEffect, TVE_EXPAND);		SetItemBold(m_hParentEffect, TRUE);
	Expand(m_hParentEquip, TVE_EXPAND);			SetItemBold(m_hParentEquip, TRUE);
	Expand(m_hParentEvent, TVE_EXPAND);			SetItemBold(m_hParentEvent, TRUE);
	Expand(m_hParentFactor, TVE_EXPAND);		SetItemBold(m_hParentFactor, TRUE);
	Expand(m_hParentGeometry, TVE_EXPAND);		SetItemBold(m_hParentGeometry, TRUE);
	Expand(m_hParentLOD, TVE_EXPAND);			SetItemBold(m_hParentLOD, TRUE);
	Expand(m_hParentSkill, TVE_EXPAND);			SetItemBold(m_hParentSkill, TRUE);
	Expand(m_hParentTool, TVE_EXPAND);			SetItemBold(m_hParentTool, TRUE);
	Expand(m_hParentUserDataArray, TVE_EXPAND);	SetItemBold(m_hParentUserDataArray, TRUE);
	Expand(m_hParentAppearance, TVE_EXPAND);	SetItemBold(m_hParentAppearance, TRUE);
	Expand(m_hParentDefaultHead, TVE_EXPAND);	SetItemBold(m_hParentDefaultHead, TRUE);
	Expand(m_hParentRide, TVE_EXPAND);			SetItemBold(m_hParentRide, TRUE);
	Expand(m_hNationalCode, TVE_EXPAND);		SetItemBold(m_hNationalCode, TRUE);
}

void CPropertyTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ( !CModelToolApp::GetInstance() )		return;

	*pResult = 0;

	HTREEITEM hSelected	= GetSelectedItem();
	if( !hSelected )		return;
		
	DWORD dwData = GetItemData( hSelected );
	if( dwData < E_PT_ITEM_DATA_SEPERATE_PARENT)	return;

	BOOL			bFuncResult	= TRUE;
	switch ( dwData )
	{
	case E_PT_ITEM_DATA_CREATE:				bFuncResult = CModelToolApp::GetInstance()->F_CreateObject();		break;
	case E_PT_ITEM_DATA_DEFAULT_PROPERTY:	bFuncResult = CModelToolApp::GetInstance()->F_SetDefaultProperty();	break;
	case E_PT_ITEM_DATA_OBJECT_TYPE:		bFuncResult = CModelToolApp::GetInstance()->F_SetObjectType();		break;
	case E_PT_ITEM_DATA_ANIMATION_PROGRESS:	bFuncResult = CModelToolApp::GetInstance()->F_SetAnimationOption();	break;
	case E_PT_ITEM_DATA_ANIMATION_DIALOG:	bFuncResult = CModelToolApp::GetInstance()->F_SetAnimation();		break;
	case E_PT_ITEM_DATA_EFFECT_DIALOG:		bFuncResult = CModelToolApp::GetInstance()->F_SetEffect();			break;
	case E_PT_ITEM_DATA_EFFECT_RELOAD:		bFuncResult = CModelToolApp::GetInstance()->F_ReloadEffect();		break;
	case E_PT_ITEM_DATA_DEFAULT_EQUIPMENTS:	bFuncResult = CModelToolApp::GetInstance()->F_SetDefaultEquipments();break;
	case E_PT_ITEM_DATA_TRANSLATE_EQUIPMENTS:bFuncResult = CModelToolApp::GetInstance()->F_EditEquipments();	break;
	case E_PT_ITEM_DATA_EVENT_INITAILIZE:	bFuncResult = CModelToolApp::GetInstance()->F_SetEventInitialize();	break;
	case E_PT_ITEM_DATA_EVENT_WORK:			bFuncResult = CModelToolApp::GetInstance()->F_SetEventWork();		break;
	case E_PT_ITEM_DATA_FACTORS_DIALOG:		bFuncResult = CModelToolApp::GetInstance()->F_SetFactors();			break;
	case E_PT_ITEM_DATA_EDIT_COLLISION_DATA:bFuncResult = CModelToolApp::GetInstance()->F_SetGeometry(AMT_GEOM_DATA_COLLISION);	break;
	case E_PT_ITEM_DATA_EDIT_OCTREE_DATA:	bFuncResult = CModelToolApp::GetInstance()->F_SetGeometry(AMT_GEOM_DATA_OCTREE);	break;
	case E_PT_ITEM_DATA_EDIT_PICKING_DATA:	bFuncResult = CModelToolApp::GetInstance()->F_SetGeometry(AMT_GEOM_DATA_PICKING);	break;
	case E_PT_ITEM_DATA_CREATE_CLUMP:		bFuncResult = CModelToolApp::GetInstance()->F_CreateClump();		break;
	case E_PT_ITEM_DATA_LOD_DIALOG:			bFuncResult = CModelToolApp::GetInstance()->F_SetLOD();				break;
	case E_PT_ITEM_DATA_SKILL_DIALOG:		bFuncResult = CModelToolApp::GetInstance()->F_SetSkill();			break;
	case E_PT_ITEM_DATA_TOOL_OFFSET:		bFuncResult = CModelToolApp::GetInstance()->F_SetOffset();			break;
	case E_PT_ITEM_DATA_TOOL_RENDER_OPTION:	bFuncResult = CModelToolApp::GetInstance()->F_SetRenderOption();	break;
	case E_PT_ITEM_DATA_TOOL_RESET_VIEW:	bFuncResult = CModelToolApp::GetInstance()->F_ResetView();			break;
	case E_PT_ITEM_DATA_TOOL_TARGET_OPTION:	bFuncResult = CModelToolApp::GetInstance()->F_SetTargetOption();	break;
	case E_PT_ITEM_DATA_TOOL_LIGHT_OPTION:	bFuncResult = CModelToolDlg::GetInstance()->OpenLightOptionDlg();	break;
	case E_PT_ITEM_DATA_RENDER_TYPE:		bFuncResult = CModelToolApp::GetInstance()->F_SetRenderType();		break;
	case E_PT_ITEM_DATA_SET_FACE:			bFuncResult = CModelToolApp::GetInstance()->F_SetFace();			break;
	case E_PT_ITEM_DATA_EDIT_FACE:			bFuncResult = CModelToolApp::GetInstance()->F_EditFace();			break;
	case E_PT_ITEM_DATA_ATTACH_FACE:		bFuncResult = CModelToolApp::GetInstance()->F_AttachFace();			break;
	case E_PT_ITEM_DATA_EDIT_HAIR:			bFuncResult = CModelToolApp::GetInstance()->F_EditHair();			break;
	case E_PT_ITEM_DATA_ATTACH_HAIR:		bFuncResult = CModelToolApp::GetInstance()->F_AttachHair();			break;
	case E_PT_ITEM_DATA_FACE_RENDER_TYPE:	bFuncResult = CModelToolApp::GetInstance()->F_FaceRenderType();		break;
	case E_PT_ITEM_DATA_HAIR_RENDER_TYPE:	bFuncResult = CModelToolApp::GetInstance()->F_HairRenderType();		break;
	case E_PT_ITEM_DATA_CUSTOMIZE_PREVIEW:	bFuncResult = CModelToolApp::GetInstance()->F_CustomizePreviewType();	break;
	case E_PT_ITEM_DATA_ATTACH_RIDE_DIALOG:	bFuncResult = CModelToolApp::GetInstance()->F_RideDialog();			break;
	case E_PT_ITEM_DATA_NATIONAL_CODE_KOREA:
		CModelToolApp::GetInstance()->SetNationalCode(0);
		CModelToolApp::GetInstance()->InitializeTitle();
		break;
	case E_PT_ITEM_DATA_NATIONAL_CODE_CHINA:
		CModelToolApp::GetInstance()->SetNationalCode(1);
		CModelToolApp::GetInstance()->InitializeTitle();
		break;
	case E_PT_ITEM_DATA_NATIONAL_CODE_WESTERN:
		CModelToolApp::GetInstance()->SetNationalCode(2);
		CModelToolApp::GetInstance()->InitializeTitle();
		break;
	case E_PT_ITEM_DATA_NATIONAL_CODE_JAPAN :
		CModelToolApp::GetInstance()->SetNationalCode(3);
		CModelToolApp::GetInstance()->InitializeTitle();
		break;
	default:
		return;
	}

	if( !bFuncResult )
		MessageBox("해당 작업을 지원하지 않거나\n내부 에러입니다.", "WARNING");
}

