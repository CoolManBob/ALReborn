#ifndef __CPROPERTYTREE_H__
#define __CPROPERTYTREE_H__

#include "TreeCtrlEx.h"

#define PT_PARENT_ITEM_NAME_DEFAULT				"[ ..DEFAULT.. ]"
#define PT_PARENT_ITEM_NAME_ANIMATION			"[ ANIMATION ]"
#define PT_PARENT_ITEM_NAME_EFFECT				"[ EFFECT ]"
#define PT_PARENT_ITEM_NAME_EQUIP				"[ EQUIP ]"
#define PT_PARENT_ITEM_NAME_EVENT				"[ EVENT ]"
#define PT_PARENT_ITEM_NAME_FACTOR				"[ FACTOR ]"
#define PT_PARENT_ITEM_NAME_GEOMETRY			"[ GEOMETRY ]"
#define PT_PARENT_ITEM_NAME_LOD					"[ LOD ]"
#define PT_PARENT_ITEM_NAME_SKILL				"[ SKILL ]"
#define PT_PARENT_ITEM_NAME_TOOL				"[ TOOL ]"
#define PT_PARENT_ITEM_NAME_USER_DATA_ARRAY		"[ USER DATA ARRAY ]"
#define PT_PARENT_ITEM_NAME_APPEARANCE			"[ APPEARANCE ]"
#define PT_PARENT_ITEM_NAME_DEFAULT_HEAD		"[ DEFAULT HEAD ]"
#define PT_PARENT_ITEM_NAME_RIDE				"[ RIDE ]"
//@{ 2006/05/29 burumal
#define PT_PARENT_ITEM_NAME_NATIONAL_CODE		"[ ...NCODE.. ]"
//@}

#define PT_ITEM_NAME_CREATE						" create"
#define PT_ITEM_NAME_DEFAULT_PROPERTY			" default property"
#define PT_ITEM_NAME_OBJECT_TYPE				" object type"
#define PT_ITEM_NAME_ANIMATION_PROGRESS			" animation progress"
#define PT_ITEM_NAME_ANIMATION_DIALOG			" animation dialog"
#define PT_ITEM_NAME_EFFECT_DIALOG				" effect dialog"
#define PT_ITEM_NAME_EFFECT_RELOAD				" effect reload"
#define PT_ITEM_NAME_DEFAULT_EQUIPMENTS			" default equipments"
#define PT_ITEM_NAME_TRANSLATE_EQUIPMENTS		" edit equipments"
#define PT_ITEM_NAME_EVENT_INITAILIZE			" initialize"
#define PT_ITEM_NAME_EVENT_WORK					" work"
#define PT_ITEM_NAME_FACTORS_DIALOG				" factors dialog"
#define PT_ITEM_NAME_EDIT_COLLISION_DATA		"  edit collision data"
#define PT_ITEM_NAME_EDIT_OCTREE_DATA			"  edit octree data"
#define PT_ITEM_NAME_EDIT_PICKING_DATA			"  edit picking data"
#define PT_ITEM_NAME_CREATE_CLUMP				" create clump"
#define PT_ITEM_NAME_LOD_DIALOG					" lod dialog"
#define PT_ITEM_NAME_SKILL_DIALOG				" skill dialog"
#define PT_ITEM_NAME_TOOL_OFFSET				" offset"
#define PT_ITEM_NAME_TOOL_RENDER_OPTION			" render option"
#define PT_ITEM_NAME_TOOL_TARGET_OPTION			" target option"
#define PT_ITEM_NAME_TOOL_LIGHT_OPTION			" Light option"
#define PT_ITEM_NAME_TOOL_RESET_VIEW			" reset view"
#define PT_ITEM_NAME_RENDER_TYPE				" render type"
#define PT_ITEM_NAME_SET_FACE					" set face"
#define PT_ITEM_NAME_EDIT_FACE					" edit face"
#define PT_ITEM_NAME_ATTACH_FACE				" attach face"
#define PT_ITEM_NAME_EDIT_HAIR					" edit hair"
#define PT_ITEM_NAME_ATTACH_HAIR				" attach hair"
#define PT_ITEM_NAME_FACE_RENDER_TYPE			" face render type "
#define PT_ITEM_NAME_HAIR_RENDER_TYPE			" hair render type "
#define PT_ITEM_NAME_CUSTOMIZE_PREVIEW_TYPE		" customize preview type "
#define PT_ITEM_NAME_ATTACH_RIDE_DIALOG			" attach ride"

typedef enum ePropertyTreeItemData
{
	E_PT_ITEM_DATA_DEFAULT = 1,
	E_PT_ITEM_DATA_ANIMATION,
	E_PT_ITEM_DATA_EFFECT,
	E_PT_ITEM_DATA_EQUIP,
	E_PT_ITEM_DATA_EVENT,
	E_PT_ITEM_DATA_FACTOR,
	E_PT_ITEM_DATA_GEOMETRY,
	E_PT_ITEM_DATA_LOD,
	E_PT_ITEM_DATA_SKILL,
	E_PT_ITEM_DATA_TOOL,
	E_PT_ITEM_DATA_USER_DATA_ARRAY,
	E_PT_ITEM_DATA_APPEARANCE,
	E_PT_ITEM_DATA_DEFAULT_HEAD,
	E_PT_ITEM_DATA_RIDE,
	E_PT_ITEM_DATA_NATIONAL_CODE,

	E_PT_ITEM_DATA_SEPERATE_PARENT = 100,
	E_PT_ITEM_DATA_CREATE,
	E_PT_ITEM_DATA_DEFAULT_PROPERTY,
	E_PT_ITEM_DATA_OBJECT_TYPE,
	E_PT_ITEM_DATA_ANIMATION_PROGRESS,
	E_PT_ITEM_DATA_ANIMATION_DIALOG,
	E_PT_ITEM_DATA_EFFECT_DIALOG,
	E_PT_ITEM_DATA_EFFECT_RELOAD,
	E_PT_ITEM_DATA_DEFAULT_EQUIPMENTS,
	E_PT_ITEM_DATA_TRANSLATE_EQUIPMENTS,
	E_PT_ITEM_DATA_EVENT_INITAILIZE,
	E_PT_ITEM_DATA_EVENT_WORK,
	E_PT_ITEM_DATA_FACTORS_DIALOG,
	E_PT_ITEM_DATA_EDIT_COLLISION_DATA,
	E_PT_ITEM_DATA_EDIT_OCTREE_DATA,
	E_PT_ITEM_DATA_EDIT_PICKING_DATA,
	E_PT_ITEM_DATA_CREATE_CLUMP,
	E_PT_ITEM_DATA_LOD_DIALOG,
	E_PT_ITEM_DATA_SKILL_DIALOG,
	E_PT_ITEM_DATA_TOOL_OFFSET,
	E_PT_ITEM_DATA_TOOL_RENDER_OPTION,
	E_PT_ITEM_DATA_TOOL_RESET_VIEW,
	E_PT_ITEM_DATA_TOOL_TARGET_OPTION,
	E_PT_ITEM_DATA_TOOL_LIGHT_OPTION,
	E_PT_ITEM_DATA_RENDER_TYPE,
	E_PT_ITEM_DATA_SET_FACE,
	E_PT_ITEM_DATA_EDIT_FACE,
	E_PT_ITEM_DATA_ATTACH_FACE,
	E_PT_ITEM_DATA_EDIT_HAIR,
	E_PT_ITEM_DATA_ATTACH_HAIR,
	E_PT_ITEM_DATA_FACE_RENDER_TYPE,
	E_PT_ITEM_DATA_HAIR_RENDER_TYPE,
	E_PT_ITEM_DATA_CUSTOMIZE_PREVIEW,
	E_PT_ITEM_DATA_ATTACH_RIDE_DIALOG,
	
	E_PT_ITEM_DATA_NATIONAL_CODE_KOREA,
	E_PT_ITEM_DATA_NATIONAL_CODE_CHINA,
	E_PT_ITEM_DATA_NATIONAL_CODE_WESTERN,
	E_PT_ITEM_DATA_NATIONAL_CODE_JAPAN,
};


class CPropertyTree : public CTreeCtrlEx
{
public:
	CPropertyTree(CWnd *pcsParent, RECT &stInitRect);
	virtual ~CPropertyTree();

protected:
	HTREEITEM		m_hParentDefault;
	HTREEITEM		m_hParentAnimation;
	HTREEITEM		m_hParentEffect;
	HTREEITEM		m_hParentEquip;
	HTREEITEM		m_hParentEvent;
	HTREEITEM		m_hParentFactor;
	HTREEITEM		m_hParentGeometry;
	HTREEITEM		m_hParentLOD;
	HTREEITEM		m_hParentSkill;
	HTREEITEM		m_hParentTool;
	HTREEITEM		m_hParentUserDataArray;
	HTREEITEM		m_hParentAppearance;
	HTREEITEM		m_hParentDefaultHead;
	HTREEITEM		m_hParentRide;
	HTREEITEM		m_hNationalCode;

	HTREEITEM		m_hCreate;
	HTREEITEM		m_hDefaultProperty;
	HTREEITEM		m_hObjectType;
	HTREEITEM		m_hAnimationProgress;
	HTREEITEM		m_hAnimationDialog;
	HTREEITEM		m_hEffectDialog;
	HTREEITEM		m_hEffectReload;
	HTREEITEM		m_hDefaultEquipments;
	HTREEITEM		m_hTranslateEquipments;
	HTREEITEM		m_hEventInitialize;
	HTREEITEM		m_hEventWork;
	HTREEITEM		m_hFactorsDialog;
	HTREEITEM		m_hEditCollisionData;
	HTREEITEM		m_hEditOcTreeData;
	HTREEITEM		m_hEditPickingData;
	HTREEITEM		m_hCreateClump;
	HTREEITEM		m_hLODDialog;
	HTREEITEM		m_hSkillDialog;
	HTREEITEM		m_hToolOffset;
	HTREEITEM		m_hToolRenderOption;
	HTREEITEM		m_hToolResetView;
	HTREEITEM		m_hToolTargetOption;
	HTREEITEM		m_hToolLightOption;
	HTREEITEM		m_hRenderType;
	HTREEITEM		m_hSetFace;
	HTREEITEM		m_hEditFace;
	HTREEITEM		m_hEditHair;
	HTREEITEM		m_hAttachFace;
	HTREEITEM		m_hAttachHair;
	HTREEITEM		m_hFaceRenderType;
	HTREEITEM		m_hHairRenderType;
	HTREEITEM		m_hCustomizePreviewType;
	HTREEITEM		m_hAttachRideDialog;

	//@{ 2006/05/30 burumal
	HTREEITEM		m_hNationalCodeState[nMAX_NATIONAL_CODES];
	//@}

protected:
	VOID			InitailzeMember();
	VOID			CreateTree();

	//{{AFX_MSG(CPropertyTree)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:	
};

#endif // __CPROPERTYTREE_H__
