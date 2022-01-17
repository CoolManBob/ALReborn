#pragma once

#include "resource.h"		// main symbols
#include "ToolDefine.h"
#include "ModelToolDlg.h"
#include "RenderWare.h"
#include "StringDlg.h"
#include "ObjectDlg.h"
#include "OffsetDlg.h"
#include "RenderOptionDlg.h"
#include "AnimationDlg.h"

#include "AuRandomNumber.h"
#include "AgcEngineChild.h"

#include "AcuObject.h"
#include "AcuIMDraw.h"

#include "AuRK_API.h"

#include "AcuFileAdmin.h"
#include "Util.h"

/////////////////////////////////////////////////////////////////////////////
// CModelToolApp:
// See ModelTool.cpp for the implementation of this class
//
class CCopyTransformSet
{
public:
	RwMatrix	*m_pstTransform;
	INT16		m_nPartID;
	INT16		m_nCBCount;
	INT16		m_nAtomicCount;

	CCopyTransformSet()
	{
		m_pstTransform	= NULL;
		m_nPartID		= 0;
		m_nCBCount		= 0;
		m_nAtomicCount	= 0;
	}
};

class CSaveTransformSet
{
public:
	INT16			m_nCBCount1;
	INT16			m_nCBCount2;
	INT32			m_lCTID;

	AgpdCharacter	*m_pcsAgpdCharacter;
	AgcdCharacter	*m_pcsAgcdCharacter;

	AgpmItem		*m_pcsAgpmItem;
	AgcmItem		*m_pcsAgcmItem;

	CSaveTransformSet()
	{
		m_lCTID				= 0;
		m_nCBCount1			= 0;
		m_nCBCount2			= 0;

		m_pcsAgpdCharacter	= NULL;
		m_pcsAgcdCharacter	= NULL;

		m_pcsAgpmItem		= NULL;
		m_pcsAgcmItem		= NULL;
	}
};

class CModelToolApp : public CWinApp
{
public:
	CModelToolApp();
	virtual ~CModelToolApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelToolApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CModelToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	AgcEngineChild			m_csAgcEngine;

	INT32					m_lStartTargetID;
	INT32					m_lTargetTID;
	INT32					m_lNumTargets;
	RwV3d					m_v3dTargetPosOffset;

	UINT					m_uNationalCode;
	BOOL					m_bNationalCodeChanged;

public:
	static CStringDlg*		m_pcsStingDlg;
	INT32					m_lResourceMode;
	BOOL					m_bExport;

	UINT32					m_aulLODDefaultDistance[5];

	eAmtObjectType			m_eObjectType;				// 현재 작업하고 있는 오브젝 타입
	RpClump					*m_pcsArmourClump1;
	RpClump					*m_pcsArmourClump2;
	RpClump					*m_pcsBottomClump;
	RpClump					*m_pcsStaticModel;
	RpAtomic				*m_pcsPickedAtomic;
	RwMatrix				m_pcsInitTransform;
	CHAR					m_szStaticModelDffName[AMT_MAX_STR];
	INT16					m_nNumBlocking;
	AuBLOCKING				m_astBlocking[AMT_MAX_GEOMETRY_DATA_ARRAY];
	HTREEITEM				m_hCurrentTreeItem;
	INT32					m_lID;
	INT32					m_lTID;
	std::vector<INT32>		m_vPolyNums;
	INT32					m_lVerNums;

	CHAR					m_szCameraPosBuffer[AMT_MAX_STR];

	CCameraOffset			m_csCameraOffset;
	CEditOffset				m_csEditOffset;
	CCharacterOffset		m_csCharacterOffset;
	CRenderOption			m_csRenderOption;	
	CRect					m_rtInitWindow;

	RwUtilClumpArray		m_stClumpArray;
	RwReal					m_fUpRotAngle;			// 카메라의 Up축 회전값
	BOOL					m_bActiveWindows;		// idle time
	MTRand					m_csRandom;
	CUtil					m_cUtil;
	CRwUtil					m_cRwUtil;

	BOOL					InitializeTitle();

protected:
	VOID					FirstInitMember();
	VOID					InitializeMember();
	BOOL					InitializeModule(CHAR *szDebug);	

	BOOL					LoadToolData();
	BOOL					LoadStaticData();
	BOOL					LoadTemplateData();
	BOOL					LoadEtcFile();
	BOOL					LoadLODList();
	BOOL					LoadGeometryDataFile();
	BOOL					LoadCharacterTemplate();
	BOOL					LoadItemTemplate();
	BOOL					LoadObjectTemplate();
	BOOL					LoadSkillTemplate();

	VOID					EnableSound(BOOL bValue);

	BOOL					SaveToolData();

	BOOL					InitStringDlg();
	VOID					ReleaseStringDlg();

	BOOL					ReleaseAllObject(BOOL bCloseEffectDlg = TRUE);
	BOOL					ReleaseClump(RpClump **ppstClump);
	BOOL					ReleaseStaticModel();
	BOOL					ReleaseCharacter();
	BOOL					ReleaseItem();
	BOOL					ReleaseObject();
	BOOL					ReleaseModalessDlg(BOOL bCloseEffectDlg = TRUE);
	BOOL					ReleaseCharacterTemplate(INT32 lTID);
	BOOL					ReleaseItemTemplate(INT32 lTID);
	BOOL					ReleaseObjectTemplate(INT32 lTID);
	BOOL					ReleaseSkillTemplate(INT32 lTID);

	VOID					UpdateApp();
	VOID					UpdateClumpPolygons();
	VOID					UpdateClumpVertices();
	VOID					UpdateCameraPosBuffer(CRenderWare *pcsRenderWare);
	VOID					UpdateAnimationDlg();
	BOOL					UpdateCharacterTemplate(AgpdCharacterTemplate *ppTemplate, AgcdCharacterTemplate *pcTemplate);
	BOOL					UpdateItemTemplate(AgpdItemTemplate *ppTemplate, AgcdItemTemplate *pcTemplate);
	BOOL					UpdateObjectTemplate(ApdObjectTemplate *ppTemplate, AgcdObjectTemplate *pcTemplate);

	VOID					RenderCameraPosBuffer(CRenderWare *pcsRenderWare);
	VOID					RenderClumpPolygons(CRenderWare *pcsRenderWare);
	VOID					RenderClumpVertices(CRenderWare *pcsRenderWare);
	VOID					RenderBlocking();
	BOOL					RenderBlocking(AuBLOCKING *pstBlocking, INT16 nIndex, RwMatrix *pLTM);
	BOOL					RenderBlocking(AuBOX *pstBox, RwMatrix *pLTM, RwRGBA *pstRGBA);
	BOOL					RenderBlocking(AuSPHERE *pstSphere, RwMatrix *pLTM, RwRGBA *pstRGBA);
	BOOL					RenderBlocking(AuCYLINDER *pstCylinder, RwMatrix *pLTM, RwRGBA *pstRGBA);
	BOOL					RenderObjectCollisionAtomic();
	BOOL					RenderPickingAtomic();
	BOOL					RenderHitRange();
	BOOL					RenderSiegeWarCollisionObject();
	BOOL					RenderTemp();
	VOID					RenderScene();
	VOID					RenderBSphere(RwSphere *pBS);
	VOID					RenderBShpere();
	VOID					RenderPickedAtomicInfo(CRenderWare *pcsRenderWare);

public:
	BOOL					RemoveTargetCharacter();
	BOOL					MakeTargetCharacter();
	BOOL					SetTarget(AgpdCharacter *pcsAgpdCharacter = NULL, AgcdCharacter *pcsAgcdCharacter = NULL);
	BOOL					LoadTerrain( BOOL bChange = FALSE );

public:
	BOOL					ApplyObject();

	INT32					GenerateCID();
	INT32					GenerateIID();
	INT32					GenerateOID();

	INT32					GenerateCTID(DWORD dwID);
	INT32					GenerateITID(DWORD dwID);
	INT32					GenerateOTID(DWORD dwID);
	INT32					GenerateSTID(DWORD dwID);

	BOOL					ReloadAllTemplate(BOOL bCurSave = TRUE);
	BOOL					ReloadStaticData();
	BOOL					ReloadCharacterTemplate(BOOL bCurSave = TRUE);
	BOOL					ReloadItemTemplate(BOOL bCurSave = TRUE);
	BOOL					ReloadObjectTemplate(BOOL bCurSave = TRUE);
	BOOL					ReloadSkillTemplate(BOOL bCurSave = TRUE);

	BOOL					SetCharacterHeight(AgcdCharacter *pstCharacter);
	BOOL					GenerateAllCharacterTemplateHeight();
	BOOL					GenerateAllCharacterTemplateHitRange(BOOL bGenHeight = FALSE);
	BOOL					GenerateAllCharacterTemplateBSphere();
	BOOL					GenerateAllItemTemplateBSphere();
	BOOL					GenerateAllObjectTemplateBSphere();
	BOOL					GenerateCharacterTemplateLODBoundary();
	BOOL					GenerateItemTemplateLODBoundary();
	BOOL					GenerateObjectTemplateLODBoundary();
	BOOL					CalcObjtTemplateOctreeData();

	BOOL					CheckCharacterTemplateLODLevel();
	BOOL					CheckItemTemplateLODLevel();
	BOOL					CheckObjectTemplateLODLevel();

	INT32					GetCharacterHitRange(RpClump *pstClump);	

	static RpAtomic*		ArmourCheckCB(RpAtomic *atomic, void *data);
	static RpAtomic*		SaveTransformCB(RpAtomic *atomic, void *data);
	static RpAtomic*		CopyTransformCB(RpAtomic *atomic, void *data);
	static RpAtomic*		ClumpForAllAtomicsCB_AddClumpTextureFileUseResourceKeeper(RpAtomic *atomic, void *data);
	static RpMaterial*		GeometryForAllMaterialsCB_AddClumpTextureFileUseResourceKeeper(RpMaterial *geometry, void *data);
	static BOOL				ReloadObjectCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				InitBlockingCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				FinishUpEditingBlockingCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				OpenStaticModelInfoCB(PVOID pClass, PVOID pCustClass);
	static BOOL				OpenCharacterInfoCB(PVOID pClass, PVOID pCustClass);
	static BOOL				OpenItemInfoCB(PVOID pClass, PVOID pCustClass);
	static BOOL				OpenObjectInfoCB(PVOID pClass, PVOID pCustClass);
	static BOOL				OpenObjectListCB(PVOID pClass, PVOID pCustClass);
	static BOOL				ObjectGetAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				StartAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				StartSkillAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				StartSkillEffectCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				GetAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				GetSkillAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				SetCharacterHeightCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				SetCharacterDepthCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				SetCharacterHitRangeCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CharacterChangeAnimCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				GenerateBoundingSphereCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ReadRtAnimCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				AddAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				AddAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				RemoveAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				RemoveAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				RemoveAllAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				StartPureAnimCB( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				StartAnimCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static RpAtomic*		MatD3DFxCB( RpAtomic* pstAtomic, void* pvData );
	static VOID				SetStringDlgTextCB(const CHAR* pMessage, VOID* pParam1);
	static VOID				WriteDirtyFileListCB(const CHAR* pMessage, VOID* pFileHandle);

public:
	static CModelToolApp	*GetInstance();

	
	VOID					Fin();		// 내부적으로 사용!(모달리스 다이얼로그 해제작업)

	BOOL					LoadResourceData();
	
	BOOL					OpenDetailInfo();
	BOOL					SaveData();

	BOOL					_SaveCharacterTemplate();
	BOOL					_SaveCharacterTemplateOld();


	TSO::ReadError			_LoadCharacterTemplate();
	BOOL					_LoadCharacterTemplateOld();

	BOOL					CursorInRenderView();

	BOOL					SetEmpty();
	RpClump*				SetStaticModel( const CHAR *szDffName, BOOL bObject);
	AgpdCharacter			*SetCharacter(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera = TRUE, BOOL bReleaseAll = TRUE, INT32 lForceCID = 0, RwV3d *pv3dForcePos = NULL);
	AgpdItem				*SetItem(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera = TRUE, BOOL bReleaseAll = TRUE);
	AgpdItem				*EquipItem(INT32 lCID, INT32 lTID);
	ApdObject				*SetObject(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera = TRUE, BOOL bReleaseAll = TRUE);
	BOOL					SetSkill(INT32 lTID, HTREEITEM hSelected);

	BOOL					ResetCamera();
	BOOL					TranslateCamera(RwV2d *pv2dDelta);
	BOOL					RotateCamera(RwV2d *pv2dDelta);
	BOOL					ZoomCamera(FLOAT fDelta);

	BOOL					F_CreateObject();
	BOOL					F_SetAnimation();
	BOOL					F_SetDefaultEquipments();
	BOOL					F_SetFactors();
	BOOL					F_SetLOD();
	BOOL					F_SetGeometry(CHAR *szGeometryKeyName);
	BOOL					F_SetEffect();
	BOOL					F_ReloadEffect();
	BOOL					F_SetObjectType();
	BOOL					F_SetDefaultProperty();
	BOOL					F_EditEquipments();
	BOOL					F_SetSkill();
	BOOL					F_SetEventInitialize();
	BOOL					F_SetEventWork();
	BOOL					F_SetRenderType();
	BOOL					F_SetFace();
	BOOL					F_CreateClump();
	BOOL					F_SetOffset();
	BOOL					F_SetRenderOption();
	BOOL					F_SetAnimationOption();
	BOOL					F_ResetView();
	BOOL					F_SetTargetOption();
	BOOL					F_SetLightOption();
	BOOL					F_EditFace();
	BOOL					F_AttachFace();
	BOOL					F_EditHair();
	BOOL					F_AttachHair();
	BOOL					F_FaceRenderType();
	BOOL					F_HairRenderType();
	BOOL					F_CustomizePreviewType();
	BOOL					F_RideDialog();
	BOOL					F_CreateOTIDFile();

	BOOL					SetCharacterLabel(INT32 lTID, CHAR *szLabel);
	BOOL					SetObjectLabel(INT32 lTID, CHAR *szLabel);

	VOID					SetMatD3DFxCB();

	BOOL					ScaleBlocking(RwV2d *pv2dDelta);
	BOOL					ScaleBlocking(AuBLOCKING *pstBlocking, RwV3d *pstScale, BOOL bHeight);
	BOOL					TranslateBlocking(RwV2d *pv2dDelta);
	BOOL					TranslateBlocking(AuBLOCKING *pstBlocking, RwV3d *pstTranslate);
	BOOL					MakeClumpBlocking(RwUtilClumpArray *pstClumpArray, AuBLOCKING *pstBlocking);
	BOOL					MakeClumpBlocking(RpClump *pstClump, AuBOX *pstBox);
	BOOL					MakeClumpBlocking(RpClump *pstClump, AuSPHERE *pstSphere);
	BOOL					MakeClumpBlocking(RpClump *pstClump, AuCYLINDER *pstCylinder);

	// util
	RpAtomic*				RenderWorldForAllIntersections(RwV2d *pixel);

	AgcEngineChild*			GetEngine()							{	return &m_csAgcEngine;	}
	CRenderWare*			GetRenderWare();
	INT32					GetCurrentID()						{	return m_lID;	}
	INT32					GetCurrentTID()						{	return m_lTID;	}
	RwUtilClumpArray*		GetRenderClump();

	CHAR*					GetStaticModelDffName()				{	return m_szStaticModelDffName;	}
	RpClump*				GetStaticModel()					{	return m_pcsStaticModel;	}
	RpAtomic*				GetPickedAtomic()					{	return m_pcsPickedAtomic;	}
	VOID					SetPickedAtomic(RpAtomic *atomic)	{	m_pcsPickedAtomic = atomic;	}
	HTREEITEM				GetCurrentTreeItem()				{	return m_hCurrentTreeItem;	}

	BOOL					CheckExport()						{	return m_bExport;	}

	AgpdCharacterTemplate*	AddCharacterTemplate(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate, DWORD dwID = 0 );
	AgpdItemTemplate*		AddItemTemplate(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, DWORD dwID = 0, BOOL bEnsureVisible = FALSE);
	AgpdItemTemplate*		MakeItemTemplate(CHAR *szName, AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, INT32 lArmourID = 0, DWORD dwID = 0, BOOL bEnsureVisible = FALSE);
	ApdObjectTemplate*		AddObjectTemplate(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate, DWORD dwID = 0);
	AgpdSkillTemplate*		AddSkillTemplate(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate, DWORD dwID = 0, BOOL bEnsureVisible = FALSE);

	BOOL					OpenBrowseForFolder(CHAR *szGetSelectedDirectory);
	BOOL					OpenObjectCategory(CHAR *szDest);
	BOOL					OpenObjectTemplate(CHAR *szDest);

	BOOL					CheckArmour(RpAtomic *pstAtomic);
	BOOL					CheckItemArmour();
	BOOL					CheckItemWeapon();
	VOID					SetInitTransform();
	VOID					InitTransformPickedAtomic();
	BOOL					ApplyTransform(RwV3d *pos, INT32 axis, RwReal degree, RwV3d *scale);
	BOOL					SaveTransform();
	void					TransformPickedAtomic( const RwV2d& vec );

	BOOL					DeleteResourceData(eAmtObjectType eType, INT32 lTID);

	BOOL					CopyLODData(AgcdLOD *pcsSrcLOD, AgcdLOD *pcsDestLOD, AgcdPreLOD *pcsSrcPreLOD, AgcdPreLOD *pcsDestPreLOD);
	BOOL					CopyEffectData(ApBase *pSrc, ApBase *pDest);
	BOOL					CopyCharacterTemplate(INT32 lTID);
	BOOL					CopyItemTemplate(INT32 lTID);
	BOOL					CopyObjectTemplate(INT32 lTID);
	BOOL					CopySkillTemplate(INT32 lTID);

	INT32					GetCharacterCurrentAnimDuration();
	VOID					StopCharacterAnim(BOOL bStop = TRUE);
	BOOL					SetCharacterAnimTime(INT32 lAnimTime);
	BOOL					IsPlayCharacterAnim();

	BOOL					SetFaceAtomic(RpAtomic *pstAtomic);
	BOOL					SetUDAData(RpAtomic *pstAtomic);
	AgcdClumpRenderType *	GetCurrentRenderType(RpAtomic *pstAtomic);

	static BOOL				CBSetUDAData(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL				CBApplyUDA(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL				CBSetFaceAtomic(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL				CBApplyFace(PVOID pvData, PVOID pvClass, PVOID pvCustData);	

	UINT					GetNationalCode()			{ return m_uNationalCode; }
	void					SetNationalCode(UINT uCode);
	BOOL					IsNationalCodeChanged()		{ return m_bNationalCodeChanged; }

	void					ClearAllTreeState(CResourceTree* pResTree);
	
	void					SetCharacterTemplateDNF(INT32 nTID, BOOL bDNF);
	void					SetObjectTemplateDNF(INT32 nTID, BOOL bDNF);
	void					SetItemTemplateDNF(INT32 nTID, BOOL bDNF);
	void					SetSkillTemplateDNF(INT32 nTID, BOOL bDNF);

	BOOL					GetCharacterTemplateDNF(INT32 nTID, INT32* pnDNF);
	BOOL					GetObjectTemplateDNF(INT32 nTID, INT32* pnDNF);
	BOOL					GetItemTemplateDNF(INT32 nTID, INT32* pnDNF);
	BOOL					GetSkillTemplateDNF(INT32 nTID, INT32* pnDNF);

	void					ResetCharacterResourceTreeDNF(UINT32 uCurNationalCode);
	void					ResetItemResourceTreeDNF(UINT32 uCurNationalCode);
	void					ResetObjectResourceTreeDNF(UINT32 uCurNationalCode);
	void					ResetSkillResourceTreeDNF(UINT32 uCurNationalCode);

	void					ResetAllResourceTreeDNF(UINT32 uCurNationalCode);
	BOOL					RefreshRenderType();


	// 2008. 06. 10. 성일추가
	// 지정 폴더에서 지정 lTID 에 해당하는 템플릿파일을 찾아 파일명을 리턴한다.
	// 리턴된 파일명이 "" 인 경우 해당하는 파일을 찾지 못한 것이다.
	std::string				_FindTemplateFile( CHAR* szPath, INT32 lTID );

	////////////////////
	void	SetSaveDataCharacter()
	{
		AgpdCharacterTemplate *ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
		m_csAgcEngine.SetSaveData( ppTemplate );
	}

	void	SetSaveDataObject()
	{
		ApdObjectTemplate *ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
		m_csAgcEngine.SetSaveData( ppTemplate );
	}

	void	SetSaveDataItem()
	{
		AgpdItemTemplate *ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
		m_csAgcEngine.SetSaveData( ppTemplate );
	}

	void	SetSaveData()
	{
		switch( m_eObjectType  )
		{
		case AMT_OBJECT_TYPE_CHARACTER	:	SetSaveDataCharacter(); break;
		case AMT_OBJECT_TYPE_ITEM		:	SetSaveDataItem(); break;
		case AMT_OBJECT_TYPE_OBJECT		:	SetSaveDataObject(); break;
		}
	}
};