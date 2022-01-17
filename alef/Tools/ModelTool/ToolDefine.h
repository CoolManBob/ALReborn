#ifndef __TOOL_DEFINE_H__
#define __TOOL_DEFINE_H__

#define AMT_VERSION															102

#define AMT_CHAR_NUM														100
#define AMT_ITEM_NUM														100
#define AMT_OBJECT_NUM														10
#define AMT_SKILL_NUM														5000
#define AMT_CHAR_TEMPLATE_NUM												5000
//@{ 2007/01/17 burumal
//#define AMT_ITEM_TEMPLATE_NUM												5000
#define AMT_ITEM_TEMPLATE_NUM												10000
//@}
#define AMT_OBJECT_TEMPLATE_NUM												5000
#define AMT_SKILL_TEMPLATE_NUM												5000
#define AMT_CHAR_ANIMATION_NUM												30000
#define AMT_OBJ_ANIMATION_NUM												10000
#define AMT_EVENT_NUM														1000
#define AMT_EFFECT_DATA_NUM													10000
#define AMT_EFFECT_SOUND_NUM												2000
#define AMT_ITEM_CLUMP_NUM													AMT_ITEM_TEMPLATE_NUM
#define AMT_MAX_STR															256
#define AMT_RESET_CAMERA_OFFSET												3.0f
#define AMT_PROPERTY_BUTTON_X_TEMP											5
#define AMT_PROPERTY_BUTTON_Y_TEMP											5
#define AMT_PROPERTY_BUTTON_WIDTH											85
#define AMT_PROPERTY_BUTTON_HEIGHT											20
#define AMT_TOOL_BUTTON_X_TEMP												10
#define AMT_TOOL_BUTTON_Y_TEMP												10
#define AMT_TOOL_BUTTON_WIDTH												110
#define AMT_TOOL_BUTTON_HEIGHT												37
#define AMT_CHARACTER_HIT_RANGE_OFFSET										10
#define AMT_CHARACTER_TARGET_ID1											9999
#define AMT_CHARACTER_TARGET_ID2											9998
#define AMT_CHARACTER_TARGET_TID1											18
#define AMT_CHARACTER_TARGET_TID2											20
#define AMT_MAX_GEOMETRY_DATA_ARRAY											10
#define AMT_BOUNDARY_TARGET_ID												9000

#define AMT_BLOCKING_RENDER_CIRCLE_NUMPOINTS								15

typedef enum E_AMT_ID_RESOURCE_TREE
{
	E_AMT_ID_RESOURCE_TREE_DATA_STATIC_CHARACTER = 1000001,
	E_AMT_ID_RESOURCE_TREE_DATA_STATIC_OBJECT,
	E_AMT_ID_RESOURCE_TREE_DATA_OBJECT,
	E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR,
	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER,
	E_AMT_ID_RESOURCE_TREE_DATA_SKILL
} E_AMT_ID_RESOURCE_TREE;

#define AMT_INI_PATH														"INI\\"
#define AMT_CHARACTER_DATA_PATH												"CHARACTER\\"
#define AMT_ITEM_DATA_PATH													"CHARACTER\\"
#define AMT_OBJECT_DATA_PATH												"OBJECT\\"
#define AMT_CHARACTER_ANIMATION_DATA_PATH									"CHARACTER\\ANIMATION\\"
#define AMT_OBJECT_ANIMATION_DATA_PATH										"OBJECT\\ANIMATION\\"
#define AMT_EFFECT_CLUMP_PATH												"EFFECT\\CLUMP\\"
#define AMT_EFFECT_ANIMATION_PATH											"EFFECT\\ANIMATION\\"
#define AMT_EFFECT_INI_PATH													"EFFECT\\INI\\"
#define AMT_EFFECT_SOUND_PATH												"SOUND\\EFFECT\\"
#define AMT_EFFECT_SOUND_MONO_PATH											"SOUND\\EFFECT\\"
#define AMT_EFFECT_TEXTURE_PATH												"TEXTURE\\EFFECT\\"
#define AMT_CHARACTER_TEXTURE_PATH											"TEXTURE\\CHARACTER\\"
#define AMT_OBJECT_TEXTURE_PATH												"TEXTURE\\OBJECT\\"
#define AMT_ITEM_TEXTURE_PATH												"TEXTURE\\ITEM\\"
#define AMT_SKILL_TEXTURE_PATH												"TEXTURE\\SKILL\\"
#define AMT_EFFECT_INI_FIND_PATH											"EFFECT\\INI\\*.INI"
#define AMT_CHARACTER_DATA_FIND_PATH1										"CHARACTER\\*.DFF"
#define AMT_CHARACTER_DATA_FIND_PATH2										"CHARACTER\\*.RWS"
#define AMT_OBJECT_DATA_FIND_PATH1											"OBJECT\\*.DFF"
#define AMT_OBJECT_DATA_FIND_PATH2											"OBJECT\\*.RWS"
#define AMT_OBJECT_ANIMATION_DATA_FIND_PATH1								"OBJECT\\ANIMATION\\*.ANM"
#define AMT_OBJECT_ANIMATINO_DATA_FIND_PATH2								"OBJECT\\ANIMATION\\*.RWS"
#define AMT_CHARACTER_ANIMATION_SOUND_FILE_FIND_PATH						"SOUND\\EFFECT\\*.WAV"
#define AMT_CHARACTER_ANIMATION_DATA_FIND_PATH1								"CHARACTER\\ANIMATION\\*.ANM"
#define AMT_CHARACTER_ANIMATION_DATA_FIND_PATH2								"CHARACTER\\ANIMATION\\*.RWS"
#define AMT_IMPORT_DATA_FIND_PATH											"INI\\*.TXT"
#define AMT_ITEM_TEXTURE_FIND_PATH1											"TEXTURE\\ITEM\\*.BMP"
#define AMT_ITEM_TEXTURE_FIND_PATH2											"TEXTURE\\ITEM\\*.TIF"
#define AMT_ITEM_TEXTURE_FIND_PATH3											"TEXTURE\\ITEM\\*.PNG"
#define AMT_SKILL_TEXTURE_FIND_PATH1										"TEXTURE\\SKILL\\*.BMP"
#define AMT_SKILL_TEXTURE_FIND_PATH2										"TEXTURE\\SKILL\\*.TIF"
#define AMT_SKILL_TEXTURE_FIND_PATH3										"TEXTURE\\SKILL\\*.PNG"
#define AMT_IMPORT_DATA_PATH												"INI\\"

#define AMT_CHARACTER_TEMPLATE_PUBLIC_INI_PATH_NAME							"INI\\CharacterTemplatePublic.ini"
#define AMT_CHARACTER_TEMPLATE_CLIENT_INI_PATH_NAME							"INI\\CharacterTemplateClient.ini"
#define AMT_CHARACTER_TEMPLATE_ANIMATION_INI_PATH_NAME						"INI\\CharacterTemplateAnimation.ini"
#define AMT_CHARACTER_TEMPLATE_CUSTOMIZEINI_INI_PATH_NAME					"INI\\CharacterTemplateCustomize.ini"
#define AMT_CHARACTER_TEMPLATE_SKILL_INI_PATH_NAME							"INI\\CharacterTemplateSkill.ini"
#define AMT_CHARACTER_TEMPLATE_SKILLSOUND_INI_PATH_NAME						"INI\\CharacterTemplateSkillSound.ini"
#define AMT_CHARACTER_TEMPLATE_EVENTEFFECT_INI_PATH_NAME					"INI\\CharacterTemplateEventEffect.ini"

#define AMT_CHARACTER_LABEL_TXT_PATH_NAME									"INI\\CHAR_LABEL.TXT"
#define AMT_ITEM_TEMPLATE_INI_PATH_NAME										"INI\\ItemTemplate.ini"
#define AMT_OBJECT_TEMPLATE_INI_PATH_NAME									"INI\\ObjectTemplate.ini"
#define AMT_SKILL_TEMPLATE_INI_PATH_NAME									"INI\\SkillTemplate.ini"
#define AMT_CHARACTER_PRE_LOD_INI_PATH_NAME									"INI\\CharacterPreLod.ini"
#define AMT_ITEM_PRE_LOD_INI_PATH_NAME										"INI\\ItemPreLod.ini"
#define AMT_OBJECT_PRE_LOD_INI_PATH_NAME									"INI\\ObjectPreLod.ini"
#define AMT_OBJECT_CATEGORY_PATH_NAME										"INI\\ObjectList.txt"
#define AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME								"INI\\MODEL_CHAR_GEOM_DATA.INI"
#define AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME								"INI\\MODEL_ITEM_GEOM_DATA.INI"
#define AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME								"INI\\MODEL_OBJT_GEOM_DATA.INI"
#define AMT_ITEM_DATA_TABLE_TXT_PATH_NAME									"INI\\ITEMDATATABLE.TXT"
#define AMT_RIDE_TID_LIST_INI_PATH_NAME										"INI\\RideList.ini"
#define AMT_DUMYY_CLUMP_PATH_NAME											"Object\\Dummy.dff"
#define AMT_BOTTOM_CLUMP_PATH_NAME											"OBJECT\\BOTTOM.DFF"
#define AMT_RESOURCE_KEEPER_INFO_PATH_NAME									"INFO.RK"
#define AMT_ITEM_TEMPLATE_INI_NAME											"ItemTemplate.ini"
#define AMT_OBJECT_TEMPLATE_INI_NAME										"ObjectTemplate.ini"
#define AMT_SKILL_TEMPLATE_INI_NAME											"SkillTemplate.ini"
#define AMT_TOOL_DATA_PATH_NAME												"INI\\AMT5.INI"
#define AMT_IMPORT_DATA_CHARACTER											"CHARACTERSTATUS.TXT"
#define AMT_IMPORT_DATA_ITEM												"ITEMSTATUS.TXT"
#define AMT_WEAPON_SOUND_FILE_NAME											"WEAPONSOUND.TXT"
#define AMT_RUN_SOUND_FILE_NAME												"RUNSOUND.TXT"
#define AMT_LOD_DISTANCE_TYPE_FILE_NAME										"LODDISTANCETYPE.TXT"
#define AMT_RK_INFO_FILE_NAME												"INFO.RK"
#define AMT_ARMOUR_PART_ID_NAME												"PartID"
#define AMT_ARMOUR_PART_BODY_NAME											"Body"
#define AMT_ARMOUR_PART_HEAD_NAME											"Head"
#define AMT_ARMOUR_PART_ARMS_NAME											"Arms"
#define AMT_ARMOUR_PART_HANDS_NAME											"Hands"
#define AMT_ARMOUR_PART_LEGS_NAME											"Legs"
#define AMT_ARMOUR_PART_FOOT_NAME											"Foot"
#define AMT_ARMOUR_PART_ARMS2_NAME											"Arms2"
#define AMT_FILE_EXT_NAME_MERGE												"MER"
#define AMT_MERGE_LODLIST_INDEX												"LOD"
#define AMT_GEOM_DATA_PICKING												"PICKING"
#define AMT_GEOM_DATA_COLLISION												"COLLISION"
#define AMT_GEOM_DATA_OCTREE												"OCTREE"

#define AMT_START_UP_ADD_MODULE												"Initializing tool.........................."
#define AMT_START_UP_RESOURCE_KEEPER_CHK_NEW_FILE_STR						"Checking new files........................."
#define AMT_START_UP_RESOURCE_KEEPER_CHK_UDT_FILE_STR						"Checking lastest version..................."
#define AMT_START_UP_RESOURCE_KEEPER_GET_LST_FILE_STR						"Downloading files.........................."
#define AMT_START_UP_RESOURCE_KEEPER_UPL_LST_FILE_STR						"Uploading files............................"
#define AMT_START_UP_UPDATE_TEMPLATE										"Updating templates........................."
#define AMT_START_UP_DATA_STR												"Reading Data..............................."
#define AMT_START_UP_CHARACTER_TEMPLATE_STR									"Reading character templates................"
#define AMT_START_UP_ITEM_TEMPLATE_STR										"Reading item templates....................."
#define AMT_START_UP_OBJECT_TEMPLATE_STR									"Reading object templates..................."
#define AMT_START_UP_SKILL_TEMPLATE_STR										"Reading skill templates...................."
#define AMT_START_UP_SAVE_CHAR_TEMPLATE_STR									"Saving character templates................."
#define AMT_START_UP_SAVE_ITEM_TEMPLATE_STR									"Saving item templates......................"
#define AMT_START_UP_SAVE_OBJT_TEMPLATE_STR									"Saving object templates...................."
#define AMT_START_UP_SAVE_SKIL_TEMPLATE_STR									"Saving skill templates....................."
#define AMT_START_UP_SAVE_CHAR_LODDATA_STR									"Saving character lod data.................."
#define AMT_START_UP_SAVE_ITEM_LODDATA_STR									"Saving item lod data......................."
#define AMT_START_UP_SAVE_OBJT_LODDATA_STR									"Saving object lod data....................."
#define AMT_START_UP_SAVE_CHAR_GEOMDATA_STR									"Saving character geom data................."
#define AMT_START_UP_SAVE_ITEM_GEOMDATA_STR									"Saving item geom data......................"
#define AMT_START_UP_SAVE_OBJT_GEOMDATA_STR									"Saving object geom data...................."
#define AMT_START_UP_SAVE_CHAR_OCTREEDATA_STR								"Saving character octree data..............."
#define AMT_START_UP_SAVE_ITEM_OCTREEDATA_STR								"Saving item octree data...................."
#define AMT_START_UP_SAVE_OBJT_OCTREEDATA_STR								"Saving object octree data.................."
#define AMT_START_UP_SAVE_RIDE_TID_LIST_STR									"Saving ride tid list data.................."
#define AMT_START_UP_GENERATE_LOD_BOUNDARY_STR								"Generating lod boundary...................."
#define AMT_START_UP_CHECK_LOD_LEVEL_STR									"Check LOD level...................."
#define AMT_START_UP_GENERATE_CHAR_HEIGHT									"Generating character height................"
#define AMT_START_UP_GENERATE_CHAR_HIT_RANGE								"Generating character hit-range............."
#define AMT_START_UP_GENERATE_CHAR_HEIGHT_HIT_RANGE							"Generating character height, hit-range....."
#define AMT_START_UP_GENERATE_BSPHERE										"Generating bounding-sphere.........."
#define AMT_START_UP_RESOURCE_KEEPER_LOCK_FILE_STR							"Locking files.............................."
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_UIIMAGE_STR						"Uploading UI images........................"
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_CLUMP_STR						"Uploading clump files......................"
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_LOD_STR							"Uploading LOD files........................"
#define	AMT_START_UP_RESOURCE_KEEPER_UPLOAD_ANIM_STR						"Uploading animation files.................."
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_SKILL_STR						"Uploading skill datas......................"
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_EFFECT_STR						"Uploading effect datas....................."
#define AMT_START_UP_RESOURCE_KEEPER_SAVE_TEMPLATE_STR						"Saving template files......................"
#define AMT_START_UP_RESOURCE_KEEPER_UPLOAD_TEMPLATE_STR					"Uploading template files..................."
#define AMT_TITLE_NAME_INITIALIZE											"<<MODEL TOOL>>"
#define AMT_TITLE_NAME_STATIC_MODEL											"<<STATIC MODEL>>"
#define AMT_TITLE_NAME_CHARACTER											"<<CHARACTER>>"
#define AMT_TITLE_NAME_ITEM													"<<ITEM>>"
#define AMT_TITLE_NAME_OBJECT												"<<OBJECT>>"
#define AMT_TITLE_NAME_SKILL												"<<SKILL>>"

#define AMT_RESOURCE_TREE_NAME_DATA1										"[CHARACTER]"
#define AMT_RESOURCE_TREE_NAME_DATA2										"[OBJECT]"
#define AMT_RESOURCE_TREE_NAME_CHARACTER									"[CHARACTER]"
#define AMT_RESOURCE_TREE_NAME_ITEM											"[ITEM]"
#define AMT_RESOURCE_TREE_NAME_OBJECT										"[OBJECT]"
#define AMT_RESOURCE_TREE_NAME_EQUIP										"[EQUIP]"
#define AMT_RESOURCE_TREE_NAME_USABLE										"[USABLE]"
#define AMT_RESOURCE_TREE_NAME_OTHER										"[OTHER]"
#define AMT_RESOURCE_TREE_NAME_ARMOUR										"[ARMOUR]"
#define AMT_RESOURCE_TREE_NAME_WEAPON										"[WEAPON]"
#define AMT_RESOURCE_TREE_NAME_PC_WEAPON									"[PC_WEAPON]"
#define AMT_RESOURCE_TREE_NAME_MONSTER_WEAPON								"[MONSTER_WEAPON]"
#define AMT_RESOURCE_TREE_NAME_SHIELD										"[SHIELD]"
#define AMT_RESOURCE_TREE_NAME_RING											"[RING]"
#define AMT_RESOURCE_TREE_NAME_NECKLACE										"[NECKLACE]"
#define AMT_RESOURCE_TREE_NAME_RIDE											"[RIDE]"
#define AMT_RESOURCE_TREE_NAME_BODY											"[BODY]"
#define AMT_RESOURCE_TREE_NAME_HEAD											"[HEAD]"
#define AMT_RESOURCE_TREE_NAME_ARMS											"[ARMS]"
#define AMT_RESOURCE_TREE_NAME_HANDS										"[HANDS]"
#define AMT_RESOURCE_TREE_NAME_LEGS											"[LEGS]"
#define AMT_RESOURCE_TREE_NAME_FOOT											"[FOOT]"
#define AMT_RESOURCE_TREE_NAME_ARMS2										"[ARMS2]"
#define AMT_RESOURCE_TREE_NAME_SKILL										"[SKILL]"
#define AMT_RESOURCE_TREE_NAME_FACE											"[FACE]"
#define AMT_RESOURCE_TREE_NAME_HAIR											"[HAIR]"

#define AMT_RESOURCE_TREE_NAME_COMMON										"Common"	//공통.
#define AMT_RESOURCE_TREE_NAME_MOONELF_SHA									"M_SHA_W"	//문엘프 여자 법사
#define AMT_RESOURCE_TREE_NAME_MOONELF_HUN									"M_HUN_W"	//문엘프 여자 궁수
#define AMT_RESOURCE_TREE_NAME_MOONELF_SB									"M_SB_W"	//문엘프 여자 전사
#define AMT_RESOURCE_TREE_NAME_ORC_SHA										"O_SHA_M"	//오크 남자마법사
#define AMT_RESOURCE_TREE_NAME_ORC_WAR										"O_WAR_M"	//오크 남자전사
#define AMT_RESOURCE_TREE_NAME_ORC_HUN										"O_HUN_W"	//오크 여자궁수
#define AMT_RESOURCE_TREE_NAME_HUMAN_ARC									"H_ARC_M"	//휴먼 남자궁수
#define AMT_RESOURCE_TREE_NAME_HUMAN_KNI									"H_KNI_M"	//휴먼 남자전사
#define AMT_RESOURCE_TREE_NAME_HUMAN_WIZ									"H_WIZ_W"	//휴먼 여자마법사
#define AMT_RESOURCE_TREE_NAME_DRAGONSCION_SCION							"D_SCI_M"	//드레곤 시온 -> 궁수와 같다. 
#define AMT_RESOURCE_TREE_NAME_DRAGONSCION_SLA								"D_SLA_M"	//드레곤 전사
#define AMT_RESOURCE_TREE_NAME_DRAGONSCION_OBI								"D_OBI_M"	//드레곤 궁수
#define AMT_RESOURCE_TREE_NAME_DRAGONSCION_SUM								"D_SUM_M"	//드레곤 법사

#define AMT_TOOL_DATA_SECTION_NAME											"TOOL_DATA"
#define AMT_TOOL_DATA_INI_NAME_CAMERA_OFFSET								"CAMERA_OFFSET"
#define AMT_TOOL_DATA_INI_NAME_EDIT_OFFSET									"EDIT_OFFSET"
#define AMT_TOOL_DATA_INI_NAME_CHARACTER_OFFSET								"CHARACTER_OFFSET"
#define AMT_TOOL_DATA_INI_NAME_WINDOW_SIZE									"WIN_SIZE"
#define AMT_TOOL_DATA_INI_NAME_RENDER_OPTION								"RENDER_OPTION"
#define AMT_TOOL_DATA_INI_NAME_TARGET_OPTION								"TARGET_OPTION"
//@{ 2006/05/03 burumal
#define AMT_TOOL_DATA_INI_NAME_NATIONAL_CODE								"NATIONAL_CODE"
//@}

#define AMT_MENU_TOOL_TIP_TEXT_SAVE											"SAVE"
#define AMT_MENU_TOOL_TIP_TEXT_EXPORT										"EXPORT"
#define AMT_MENU_TOOL_TIP_TEXT_IMPORT										"IMPORT"
#define AMT_MENU_TOOL_TIP_TEXT_MERGE_DATA									"MERGE DATA"
#define AMT_MENU_TOOL_TIP_TEXT_MERGE										"MERGE"
#define AMT_MENU_TOOL_TIP_TEXT_RK											"RESOURCE KEEPER"
#define AMT_MENU_TOOL_TIP_TEXT_LOCK_OBJECT									"LOCK"
#define AMT_MENU_TOOL_TIP_TEXT_UNLOCK										"UNLOCK"
#define AMT_RESOURCE_TOOL_TIP_TEXT_STATIC									"STATIC"
#define AMT_RESOURCE_TOOL_TIP_TEXT_CHARACTER								"CHARACTER"
#define AMT_RESOURCE_TOOL_TIP_TEXT_ITME										"ITEM"
#define AMT_RESOURCE_TOOL_TIP_TEXT_OBJECT									"OBJECT"
#define AMT_RESOURCE_TOOL_TIP_TEXT_SKILL									"SKILL"

#define AMT_SAVE_TREE_NAME_ROOT												"_ROOT"

#define AMT_RENDER_NAME_CLUMP_POLYGONS										"Clump polygons"
#define AMT_RENDER_NAME_CLUMP_VERTICES										"Clump vertices"

typedef enum
{
	AMT_OBJECT_TYPE_NONE			= 0x00,
	AMT_OBJECT_TYPE_STATIC_MODEL	= 0x01,
	AMT_OBJECT_TYPE_CHARACTER		= 0x02,
	AMT_OBJECT_TYPE_ITEM			= 0x04,
	AMT_OBJECT_TYPE_OBJECT			= 0x08,
	AMT_OBJECT_TYPE_SKILL			= 0x10
} eAmtObjectType;

class CAmtArmourCheck
{
public:
	BOOL	m_bHasBody;
	BOOL	m_bHasHead;
	BOOL	m_bHasArms;
	BOOL	m_bHasHands;
	BOOL	m_bHasLegs;
	BOOL	m_bHasFoot;
	BOOL	m_bHasArms2;

public:
	CAmtArmourCheck()
	{
		memset(this, 0, sizeof(CAmtArmourCheck));
	}

	virtual ~CAmtArmourCheck() {}
};

//@{ 2006/05/03
#define nMAX_NATIONAL_CODES			4

	//"KOREA",
	//"CHINA",
	//"WESTERN",
	//"JAPAN"

extern char* g_pNationalCodeStr[nMAX_NATIONAL_CODES];
//@}


#endif // __TOOL_DEFINE_H__