#include "stdafx.h"
#include "ModelTool.h"
#include "SaveDlg.h"
#include "ApModuleStream.h"
#include "AuFileFind.h"
#include "AgcEngineChild.h"

static BOOL	_CreateDirectory( char *pFormat , ... )
{
	char	strtmp[8192];
	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strtmp	,	pFormat,	vParams	);
	va_end		( vParams							);

	return CreateDirectory( strtmp , NULL );
}

BOOL CModelToolApp::SaveData()
{
	if (!InitStringDlg())
		return FALSE;

	char pMsgBuff[256];

	CSaveDlgSet	csSet;
	CSaveDlg	dlg(&csSet);

	if (dlg.DoModal() == IDOK)
	{		
		if (csSet.m_bCharOcTreeData)
		{
			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_CHAR_OCTREEDATA_STR);
		}

		if (csSet.m_bItemOcTreeData)
		{
			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_ITEM_OCTREEDATA_STR);
		}

		if (csSet.m_bObjtOcTreeData)
		{
			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_OBJT_OCTREEDATA_STR);

			if (!CalcObjtTemplateOctreeData())
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if (csSet.m_bCharGenHitRange)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			if (csSet.m_bCharGenHeight)
				m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_CHAR_HEIGHT_HIT_RANGE);
			else
				m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_CHAR_HIT_RANGE);

			if (!GenerateAllCharacterTemplateHitRange(csSet.m_bCharGenHeight))
			{
				ReleaseStringDlg();
				return FALSE;
			}

			// 이미 하지 않았나? 움헤헿~ -_-;;
			csSet.m_bCharGenHeight = FALSE;
		}

		if (csSet.m_bCharGenHeight)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_CHAR_HEIGHT);

			if (!GenerateAllCharacterTemplateHeight())
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if (csSet.m_bCharGenBSphere)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_BSPHERE);
			
			if (!GenerateAllCharacterTemplateBSphere())
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if (csSet.m_bItemGenBSphere)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_BSPHERE);

			if (!GenerateAllItemTemplateBSphere())
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if (csSet.m_bObjGenBSphere)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_BSPHERE);

			if (!GenerateAllObjectTemplateBSphere())
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if (csSet.m_bCharacerTemplate)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			if (csSet.m_bLODCheckLevel)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_CHECK_LOD_LEVEL_STR);
				if (!CheckCharacterTemplateLODLevel())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			if (csSet.m_bLODGenBoundary)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_LOD_BOUNDARY_STR);

				if (!GenerateCharacterTemplateLODBoundary())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_CHAR_LODDATA_STR);

			/*
			if (!m_bExport)
			{
				if (!m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamWrite(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME))
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}
			*/
		}

		if (csSet.m_bItemTemplate)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			if (csSet.m_bLODCheckLevel)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_CHECK_LOD_LEVEL_STR);
				if (!CheckItemTemplateLODLevel())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			if (csSet.m_bLODGenBoundary)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_LOD_BOUNDARY_STR);

				if (!GenerateItemTemplateLODBoundary())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_ITEM_LODDATA_STR);

			/*
			if (!m_bExport)
			{
				if (!m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamWrite(AMT_ITEM_PRE_LOD_INI_PATH_NAME))
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}
			*/
		}

		if (csSet.m_bObjectTemplate)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			if (csSet.m_bLODCheckLevel)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_CHECK_LOD_LEVEL_STR);
				if (!CheckObjectTemplateLODLevel())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			if (csSet.m_bLODGenBoundary)
			{
				m_pcsStingDlg->SetString(AMT_START_UP_GENERATE_LOD_BOUNDARY_STR);

				if (!GenerateObjectTemplateLODBoundary())
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_OBJT_LODDATA_STR);

			/*
			if (!m_bExport)
			{
				if (!m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamWrite(AMT_OBJECT_PRE_LOD_INI_PATH_NAME))
				{
					ReleaseStringDlg();
					return FALSE;
				}
			}
			*/
		}

		if (csSet.m_bCharacerTemplate)
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_CHAR_TEMPLATE_STR);

			if( csSet.m_bOldTypeSave )
			{
				if( !_SaveCharacterTemplateOld() )
				{
					return FALSE;
				}
			}
			else
			{
				// 새 쎄이브.
				if( !_SaveCharacterTemplate() )
				{
					return FALSE;
				}
			}

			//
			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_LABEL_TXT_PATH_NAME);
			m_pcsStingDlg->SetString(pMsgBuff);

			if (!CModelToolDlg::GetInstance()->GetResourceForm()->SaveCharacterLabel(AMT_CHARACTER_LABEL_TXT_PATH_NAME, 
				SetStringDlgTextCB, AMT_START_UP_SAVE_CHAR_TEMPLATE_STR))
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}
		else if( !csSet.vecCharacterTemplate.empty() )
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_CHAR_TEMPLATE_STR);

			TSO::CharacterLoader	cLoader( m_csAgcEngine );
			for( vector< AgpdCharacterTemplate *>::iterator iter = csSet.vecCharacterTemplate.begin() ;
				iter != csSet.vecCharacterTemplate.end();
				iter ++)
			{
				AgpdCharacterTemplate * pcsTemplate = *iter;
				if( cLoader.SaveTemplateOne( "ini\\CharacterTemplate" , pcsTemplate ) )
				{
					m_csAgcEngine.SetSaved( pcsTemplate );
				}
			}

			//
			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_LABEL_TXT_PATH_NAME);
			m_pcsStingDlg->SetString(pMsgBuff);

			if (!CModelToolDlg::GetInstance()->GetResourceForm()->SaveCharacterLabel(AMT_CHARACTER_LABEL_TXT_PATH_NAME, 
				SetStringDlgTextCB, AMT_START_UP_SAVE_CHAR_TEMPLATE_STR))
			{
				ReleaseStringDlg();
				return FALSE;
			}

		}

		if( csSet.m_bItemTemplate ) {
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_ITEM_TEMPLATE_STR);

			if( m_bExport ) {

				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_ITEM_TEMPLATE_STR, AMT_ITEM_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetAgpmItemModule()->StreamWriteTemplate(AMT_ITEM_TEMPLATE_INI_PATH_NAME, TRUE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
			else {

				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_ITEM_TEMPLATE_STR, AMT_ITEM_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetAgpmItemModule()->StreamWriteTemplate(AMT_ITEM_TEMPLATE_INI_PATH_NAME, FALSE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
				m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamWrite(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
				m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteItemGeomDataFile(AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);

				// ItemTemplate 새 형식으로 저장하지 않음..마고자 2008/04/24
				/*
				if( csSet.m_bOldTypeSave )
				{
				}
				else
				{
					_CreateDirectory( "ini\\ItemTemplate" );
					if( !TSO::SaveItemTemplateSeperated( m_csAgcEngine , "ini\\ItemTemplate" , MessageBoxLogger()  ) )
					{
						ReleaseStringDlg();
						return FALSE;
					}
				}
				*/
			}
		}
		/*
		else if( !csSet.vecItemTemplate.empty() )
		{
			if (!m_pcsStingDlg)
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_ITEM_TEMPLATE_STR);

			if( m_bExport ) {

				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_ITEM_TEMPLATE_STR, AMT_ITEM_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetAgpmItemModule()->StreamWriteTemplate(AMT_ITEM_TEMPLATE_INI_PATH_NAME, TRUE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
			else {
				_CreateDirectory( "ini\\ItemTemplate" );

				TSO::ItemLoader	cLoader( m_csAgcEngine );
				for( vector< AgpdItemTemplate *>::iterator iter = csSet.vecItemTemplate.begin() ;
					iter != csSet.vecItemTemplate.end();
					iter ++)
				{
					AgpdItemTemplate * pcsTemplate = *iter;
					if( cLoader.SaveTemplateOne( "ini\\ItemTemplate" , pcsTemplate ) )
					{
						m_csAgcEngine.SetSaved( pcsTemplate );
					}
				}
			}
		}
		*/

		if( csSet.m_bObjectTemplate ) {

			if( !m_pcsStingDlg )
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_OBJT_TEMPLATE_STR);

			if( m_bExport )	{

				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_TEMPLATE_STR, AMT_OBJECT_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetApmObjectModule()->StreamWriteTemplate(AMT_OBJECT_TEMPLATE_INI_PATH_NAME, TRUE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
			else
			{
				if( csSet.m_bOldTypeSave )
				{
					sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_TEMPLATE_STR, AMT_OBJECT_TEMPLATE_INI_PATH_NAME);
					m_pcsStingDlg->SetString(pMsgBuff);

					if( !m_csAgcEngine.GetApmObjectModule()->StreamWriteTemplate(AMT_OBJECT_TEMPLATE_INI_PATH_NAME, FALSE) ) {
						ReleaseStringDlg();
						return FALSE;
					}

					if( !m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteObjGeomDataFile( AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME ) ) {
						ReleaseStringDlg();
						return FALSE;
					}

					if (!m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamWrite(AMT_OBJECT_PRE_LOD_INI_PATH_NAME))
					{
						ReleaseStringDlg();
						return FALSE;
					}
				}
				else
				{
					_CreateDirectory( "ini\\ObjectTemplate" );
					if( !TSO::SaveObjectTemplateSeperated( m_csAgcEngine , "ini\\ObjectTemplate" , MessageBoxLogger()  ) )
					{
						ReleaseStringDlg();
						return FALSE;
					}
				}
			}

			if( (!CModelToolDlg::GetInstance()) || (!CModelToolDlg::GetInstance()->GetResourceForm()) ) {
				ReleaseStringDlg();
				return FALSE;
			}

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_TEMPLATE_STR, AMT_OBJECT_CATEGORY_PATH_NAME);
			m_pcsStingDlg->SetString( pMsgBuff );

			if( !CModelToolDlg::GetInstance()->GetResourceForm()->SaveObjectLabel(AMT_OBJECT_CATEGORY_PATH_NAME, 
					SetStringDlgTextCB, AMT_START_UP_SAVE_OBJT_TEMPLATE_STR) ) 
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}
		else if( !csSet.vecObjectTemplate.empty() )
		{
			if( !m_pcsStingDlg )
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_OBJT_TEMPLATE_STR);

			if( m_bExport )	{

				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_TEMPLATE_STR, AMT_OBJECT_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetApmObjectModule()->StreamWriteTemplate(AMT_OBJECT_TEMPLATE_INI_PATH_NAME, TRUE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
			else {
				_CreateDirectory( "ini\\ObjectTemplate" );

				TSO::ObjectLoader	cLoader( m_csAgcEngine );
				for( vector< ApdObjectTemplate *>::iterator iter = csSet.vecObjectTemplate.begin() ;
					iter != csSet.vecObjectTemplate.end();
					iter ++)
				{
					ApdObjectTemplate * pcsTemplate = *iter;
					if( cLoader.SaveTemplateOne( "ini\\ObjectTemplate" , pcsTemplate ) )
					{
						m_csAgcEngine.SetSaved( pcsTemplate );
					}
				}
			}

			if( (!CModelToolDlg::GetInstance()) || (!CModelToolDlg::GetInstance()->GetResourceForm()) ) {
				ReleaseStringDlg();
				return FALSE;
			}

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_TEMPLATE_STR, AMT_OBJECT_CATEGORY_PATH_NAME);
			m_pcsStingDlg->SetString( pMsgBuff );

			if( !CModelToolDlg::GetInstance()->GetResourceForm()->SaveObjectLabel(AMT_OBJECT_CATEGORY_PATH_NAME, 
					SetStringDlgTextCB, AMT_START_UP_SAVE_OBJT_TEMPLATE_STR) ) 
			{
				ReleaseStringDlg();
				return FALSE;
			}
		}

		if( csSet.m_bSkillTemplate ) {
			if( !m_pcsStingDlg )
				InitStringDlg();

			m_pcsStingDlg->SetString(AMT_START_UP_SAVE_SKIL_TEMPLATE_STR);

			if( m_bExport ) {
				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_SKIL_TEMPLATE_STR, AMT_SKILL_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetAgpmSkillModule()->StreamWriteTemplate(AMT_SKILL_TEMPLATE_INI_PATH_NAME, TRUE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
			else {
				sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_SKIL_TEMPLATE_STR, AMT_SKILL_TEMPLATE_INI_PATH_NAME);
				m_pcsStingDlg->SetString(pMsgBuff);

				if( !m_csAgcEngine.GetAgpmSkillModule()->StreamWriteTemplate(AMT_SKILL_TEMPLATE_INI_PATH_NAME, FALSE) ) {
					ReleaseStringDlg();
					return FALSE;
				}
			}
		}

		if( csSet.m_bCharGeometryData ) {
			m_pcsStingDlg->SetString( AMT_START_UP_SAVE_CHAR_GEOMDATA_STR );

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_GEOMDATA_STR , AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME);
			m_pcsStingDlg->SetString(pMsgBuff);

			/* // SaveCharaterTemplateOld 로 옮김
			if( !m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteCharGeomDataFile( AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME ) ) {
				ReleaseStringDlg();
				return FALSE;
			}
			*/
		}

		if( csSet.m_bItemGeometryData ) {
			m_pcsStingDlg->SetString( AMT_START_UP_SAVE_ITEM_GEOMDATA_STR );

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_ITEM_GEOMDATA_STR, AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);
			m_pcsStingDlg->SetString(pMsgBuff);

			/*
			if( !m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteItemGeomDataFile( AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME ) ) {
				ReleaseStringDlg();
				return FALSE;
			}
			*/
		}

		if( csSet.m_bObjGeometryData ) {
			m_pcsStingDlg->SetString( AMT_START_UP_SAVE_OBJT_GEOMDATA_STR );

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_OBJT_GEOMDATA_STR, AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME);
			m_pcsStingDlg->SetString(pMsgBuff);

			if( !m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteObjGeomDataFile( AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME ) ) {
				ReleaseStringDlg();
				return FALSE;
			}
		}

/*		if( csSet.m_bRideList ) {
			m_pcsStingDlg->SetString( AMT_START_UP_SAVE_RIDE_TID_LIST_STR );

			CCharRideDlg* pCharRideDlg = CModelToolDlg::GetInstance()->GetRideDlg();
			AgcmRide* pAgcmRide		   = m_csAgcEngine.GetAgcmRideModule();

			pAgcmRide->SetRideData( pCharRideDlg->GetRideData() );
			if( !pAgcmRide->StreamWriteTemplate( AMT_RIDE_TID_LIST_INI_PATH_NAME, m_bExport ) ) {
				ReleaseStringDlg();
				return FALSE;
			}
		}*/

		//@{ 2006/05/03 burumal
		if ( m_bNationalCodeChanged )
		{
			m_bNationalCodeChanged = FALSE;
			
			InitializeTitle();

			// 툴정보 저장
			SaveToolData();
		}
		//@}
	}
	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::SaveToolData()
{
	CHAR szValue[AMT_MAX_STR];

	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))
		return FALSE;

	if(!csStream.SetSection(AMT_TOOL_DATA_SECTION_NAME))
		return FALSE;

	sprintf(szValue, "%f %f %f", m_csCameraOffset.m_fRotate, m_csCameraOffset.m_fZoom, m_csCameraOffset.m_fMove);
	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_CAMERA_OFFSET, szValue))
		return FALSE;

	sprintf(szValue, "%f %f %f", m_csEditOffset.m_fPosit, m_csEditOffset.m_fRotate, m_csEditOffset.m_fScale);
	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_EDIT_OFFSET, szValue))
		return FALSE;

	sprintf(szValue, "%d", m_csCharacterOffset.m_bCharacterTarget);
	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_CHARACTER_OFFSET, szValue))
		return FALSE;

	CRect csRect;
	CModelToolDlg::GetInstance()->GetWindowRect(csRect);
	sprintf(szValue, "%d %d %d %d", csRect.left, csRect.top, csRect.right, csRect.bottom);
	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_WINDOW_SIZE, szValue))
		return FALSE;

	sprintf(szValue, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
															m_csRenderOption.m_bShowBlocking,
															m_csRenderOption.m_bShowCameraPos,
															m_csRenderOption.m_bShowCollisionClump,
															m_csRenderOption.m_bShowPickClump,
															m_csRenderOption.m_bShowFPS,
															m_csRenderOption.m_bShowClumpPolygons,
															m_csRenderOption.m_bShowClumpVertices,
															m_csRenderOption.m_bShowTemp,
															m_csRenderOption.m_bShowBoundingSphere,
															m_csRenderOption.m_bMatD3DFx,
															m_csRenderOption.m_bShowHitRange,
															m_csRenderOption.m_bShowSiegeWarCollObj,
															m_csRenderOption.m_bEnableSound,
															m_csRenderOption.m_dwResourceTreeWidth );

	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_RENDER_OPTION, szValue))
		return FALSE;

	sprintf(szValue, "%d %d %f %f %f", m_lTargetTID, m_lNumTargets, m_v3dTargetPosOffset.x, m_v3dTargetPosOffset.y, m_v3dTargetPosOffset.z);
	if(!csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_TARGET_OPTION, szValue))
		return FALSE;

	//@{ 2006/05/03 burumal
	sprintf(szValue, "%d", m_uNationalCode);
	if ( !csStream.WriteValue(AMT_TOOL_DATA_INI_NAME_NATIONAL_CODE, szValue) )
		return FALSE;
	//@}

	return csStream.Write(AMT_TOOL_DATA_PATH_NAME);
}

TSO::ReadError	CModelToolApp::_LoadCharacterTemplate()
{
	return TSO::LoadCharacterTemplateSeperated( m_csAgcEngine , "ini\\CharacterTemplate" , MessageBoxLogger()  );	
}

BOOL	CModelToolApp::_SaveCharacterTemplate()
{
	_CreateDirectory( "ini\\CharacterTemplate" );

	return TSO::SaveCharacterTemplateSeperated( m_csAgcEngine , "ini\\CharacterTemplate" , MessageBoxLogger()  );
}

BOOL	CModelToolApp::_SaveCharacterTemplateOld()
{
	char pMsgBuff[256];
	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_PUBLIC_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgpmCharacterModule()->StreamWriteTemplate(AMT_CHARACTER_TEMPLATE_PUBLIC_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_CLIENT_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmCharacterModule()->StreamWriteTemplate(AMT_CHARACTER_TEMPLATE_CLIENT_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_ANIMATION_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmCharacterModule()->StreamWriteTemplateAnimation(AMT_CHARACTER_TEMPLATE_ANIMATION_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_CUSTOMIZEINI_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmCharacterModule()->StreamWriteTemplateCustomize(AMT_CHARACTER_TEMPLATE_CUSTOMIZEINI_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_SKILL_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmSkillModule()->StreamWriteTemplateSkill(AMT_CHARACTER_TEMPLATE_SKILL_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_SKILLSOUND_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmSkillModule()->StreamWriteTemplateSkillSound(AMT_CHARACTER_TEMPLATE_SKILLSOUND_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	//
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SAVE_CHAR_TEMPLATE_STR, AMT_CHARACTER_TEMPLATE_EVENTEFFECT_INI_PATH_NAME);
	m_pcsStingDlg->SetString(pMsgBuff);

	if (!m_csAgcEngine.GetAgcmEventEffectModule()->StreamWriteTemplate(AMT_CHARACTER_TEMPLATE_EVENTEFFECT_INI_PATH_NAME, m_bExport))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	if (!m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamWrite(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME))
	{
		ReleaseStringDlg();
		return FALSE;
	}

	if( !m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteCharGeomDataFile( AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME ) ) {
		ReleaseStringDlg();
		return FALSE;
	}

	return TRUE;
}


BOOL	CModelToolApp::_LoadCharacterTemplateOld()
{
	CHAR szIniPathNamePublic[AMT_MAX_STR];
	CHAR szIniPathNameClient[AMT_MAX_STR];
	CHAR szIniPathNameAnimation[AMT_MAX_STR];
	CHAR szIniPathNameCustomize[AMT_MAX_STR];
	CHAR szIniPathNameSkill[AMT_MAX_STR];
	CHAR szIniPathNameSkillSound[AMT_MAX_STR];
	CHAR szIniPathNameEventEffect[AMT_MAX_STR];

	sprintf( szIniPathNamePublic,		AMT_CHARACTER_TEMPLATE_PUBLIC_INI_PATH_NAME			);
	sprintf( szIniPathNameClient,		AMT_CHARACTER_TEMPLATE_CLIENT_INI_PATH_NAME			);
	sprintf( szIniPathNameAnimation,	AMT_CHARACTER_TEMPLATE_ANIMATION_INI_PATH_NAME		);
	sprintf( szIniPathNameCustomize,	AMT_CHARACTER_TEMPLATE_CUSTOMIZEINI_INI_PATH_NAME	);
	sprintf( szIniPathNameSkill,		AMT_CHARACTER_TEMPLATE_SKILL_INI_PATH_NAME			);
	sprintf( szIniPathNameSkillSound,	AMT_CHARACTER_TEMPLATE_SKILLSOUND_INI_PATH_NAME		);
	sprintf( szIniPathNameEventEffect,	AMT_CHARACTER_TEMPLATE_EVENTEFFECT_INI_PATH_NAME	);

	CHAR szOutput[AMT_MAX_STR];

	bool bError = false;
	CFileFind	csFind;
	if(csFind.FindFile(szIniPathNamePublic) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNamePublic);
	}
	if(csFind.FindFile(szIniPathNameClient) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameClient);
	}
	if(csFind.FindFile(szIniPathNameAnimation) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameAnimation);
	}
	if(csFind.FindFile(szIniPathNameCustomize) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameCustomize);
	}
	if(csFind.FindFile(szIniPathNameSkill) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameSkill);
	}
	if(csFind.FindFile(szIniPathNameSkillSound) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameSkillSound);
	}
	if(csFind.FindFile(szIniPathNameEventEffect) == false ) {
		bError = true;
		sprintf(szOutput, "%s가 없습니다!!!\n", szIniPathNameEventEffect);
	}
	csFind.Close();

	char pMsgBuff[256];
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNamePublic);
	m_pcsStingDlg->SetString( pMsgBuff );

	CHAR szErrorCode[AMT_MAX_STR];
	if( !m_csAgcEngine.GetAgpmCharacterModule()->StreamReadTemplate( szIniPathNamePublic, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplatePublic.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameClient);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmCharacterModule()->StreamReadTemplate( szIniPathNameClient, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateClient.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameAnimation);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmCharacterModule()->StreamReadTemplateAnimation( szIniPathNameAnimation, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateAnimation.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameCustomize);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmCharacterModule()->StreamReadTemplateCustomize( szIniPathNameCustomize, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateCustomize.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameSkill);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmSkillModule()->StreamReadTemplateSkill( szIniPathNameSkill, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateSkill.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	// 
	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameSkillSound);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmSkillModule()->StreamReadTemplateSkillSound( szIniPathNameSkillSound, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateSkillSound.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, szIniPathNameEventEffect);
	m_pcsStingDlg->SetString(pMsgBuff);

	if( !m_csAgcEngine.GetAgcmEventEffectModule()->StreamReadTemplate( szIniPathNameEventEffect, szErrorCode, m_bExport ) )
	{
		CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
		CModelToolDlg::GetInstance()->MessageBox("CharacterTemplateEventEffect.ini 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
		csFind.Close();
		return FALSE;
	}

	// 추가..
	m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamRead(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadCharGeomDataFile(AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME);

	return TRUE;
}




std::string CModelToolApp::_FindTemplateFile( CHAR* szPath, INT32 lTID )
{
	// 지정 경로의 모든 파일리스트를 작성한다.
	vector< string > vecFile;
	if( !TSO::FindIniFiles( szPath, vecFile ) )
	{
		// 파일이 존재하지 않거나 읽는데 실패
		return "";
	}

	for( vector< string >::iterator iter = vecFile.begin() ; iter != vecFile.end() ; iter++ )
	{
		char szFile[ MAX_PATH ];
		sprintf( szFile , "%s\\%s", szPath, iter->c_str() );

		ApModuleStream csStream;
		if( csStream.Open( szFile, 0, FALSE ) )
		{
			// 스트림의 0번 섹션에 TID 값이 있다.
			INT32 nTID = atoi( csStream.ReadSectionName( 0 ) );
			if( nTID == lTID )
			{
				// 찾고자 하는 TID 값과 동일하면 파일명 리턴
				return szFile;
			}
		}
	}

	// 결국 못찾았다.. 경로지정이 잘못되었거나 실제로 해당 파일이 없는 경우
	return "";
}
