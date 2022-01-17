#include "stdafx.h"
#include "../resource.h"
#include "AgcmBlockingDlg.h"
#include "AgcmOpenEditBlockingDlg.h"
#include "AgcmInsertBlockingInfoDlg.h"
#include "AgcmEditBlockingDlg.h"
#include "AgcmEditBlockingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AgcmEditBlockingDlg		*g_pcsAgcmEditBlockingDlg		= NULL;
static AgcmBlockingDlg			*g_pcsAgcmBlockingDlgInst		= NULL;

AgcmBlockingDlg *AgcmBlockingDlg::GetInstance()
{
	return g_pcsAgcmBlockingDlgInst;
}

AgcmBlockingDlg::AgcmBlockingDlg()
{
	SetModuleName("AgcmBlockingDlg");

	g_pcsAgcmBlockingDlgInst	= this;

	m_eBlockingType				= AUBLOCKING_TYPE_NONE;
	m_bOpenEditBlocking			= FALSE;
}

AgcmBlockingDlg::~AgcmBlockingDlg()
{
	g_pcsAgcmBlockingDlgInst = NULL;
}

VOID AgcmBlockingDlg::SetBlockingType(UINT16 unType)
{
	switch(unType)
	{
	case 0:	m_eBlockingType = AUBLOCKING_TYPE_BOX;		break;
	case 1:	m_eBlockingType = AUBLOCKING_TYPE_SPHERE;	break;
	case 2:	m_eBlockingType = AUBLOCKING_TYPE_CYLINDER;	break;
	}
}

BOOL AgcmBlockingDlg::OpenInsertBlocking(UINT16 unNewIndex)
{
	AgcmInsertBlockingInfoDlg dlg(unNewIndex);
	if(dlg.DoModal() == IDOK)
	{
		m_pstBlocking[unNewIndex].type = m_eBlockingType;

		EnumCallbackInitBlocking((PVOID)(m_pstBlocking + unNewIndex));

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmBlockingDlg::OpenEditBlocking(BOOL bChangeType)
{
	if(bChangeType)
	{
		if(!OpenInsertBlocking(m_nCurBlockingIndex))
			return FALSE;
	}

	g_pcsAgcmEditBlockingDlg = new AgcmEditBlockingDlg();

	g_pcsAgcmEditBlockingDlg->Create(NULL);
	g_pcsAgcmEditBlockingDlg->ShowWindow(SW_SHOW);

	m_bOpenEditBlocking = TRUE;

	return TRUE;
}

BOOL AgcmBlockingDlg::Open(AuBLOCKING *pstOutBlocking, INT16 *pnBlockingNum, INT16 nMaxBlocking, PVOID pvFinishUpCBData)
{
	m_pstBlocking		= pstOutBlocking;
	m_pnBlockingNum		= pnBlockingNum;
	m_nMaxBlocking		= nMaxBlocking;	

	AgcmOpenEditBlockingDlg	dlg;
	INT16 nReturn = dlg.DoModal();

	if(nReturn == IDCANCEL)
		return FALSE; // skip

	if(!(OpenEditBlocking(((nReturn == IDOK) ? FALSE : TRUE))))
		return FALSE; // SKIP

	INT16 nCount;
	for( nCount = 0; nCount < m_nMaxBlocking; ++nCount)
	{
		if(m_pstBlocking[nCount].type == AUBLOCKING_TYPE_NONE)
			break;
	}

	*(m_pnBlockingNum)	= nCount;

	m_pvFinishUpCBData	= pvFinishUpCBData;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 022703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::DeleteBlocking(INT16 nDeleteIndex)
{
	INT16 nIndex;

	memset(m_pstBlocking + nDeleteIndex, 0, sizeof(AuBLOCKING));
	m_pstBlocking[nDeleteIndex].type = AUBLOCKING_TYPE_NONE;

	for(nIndex = nDeleteIndex; nIndex < GetBlockingNum(); ++nIndex)
	{
		memcpy(m_pstBlocking + nIndex, m_pstBlocking + (nIndex + 1), sizeof(AuBLOCKING));
	}

	if(nIndex > nDeleteIndex)
	{
		memset(m_pstBlocking + nIndex, 0, sizeof(AuBLOCKING));
	}

	--(*m_pnBlockingNum);
	if ((*(m_pnBlockingNum)) > 0)
	{
		if (m_nCurBlockingIndex > 0)
			--m_nCurBlockingIndex;
	}
	else
	{
		CloseEditBlocking();
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::SetNewBlocking(INT16 nCopyIndex)
{
	CloseEditBlocking();

	m_nCurBlockingIndex = *(m_pnBlockingNum);

	if(nCopyIndex == -1)
	{
		if(!OpenInsertBlocking(m_nCurBlockingIndex))
			return FALSE; // skip
	}
	else
	{
		memcpy(m_pstBlocking + m_nCurBlockingIndex, m_pstBlocking + nCopyIndex, sizeof(AuBLOCKING));		
	}

	OpenEditBlocking(FALSE);

	++(*(m_pnBlockingNum));

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
//VOID AgcmBlockingDlg::CloseEditBlocking(AuBLOCKING *pstBlocking, INT16 nMaxBlockInfo)
VOID AgcmBlockingDlg::CloseEditBlocking()
{
	if(g_pcsAgcmEditBlockingDlg)
	{
//		memcpy(m_pstBlocking, pstBlocking, sizeof(AuBLOCKING) * nMaxBlockInfo);

		g_pcsAgcmEditBlockingDlg->ShowWindow(SW_HIDE);
		g_pcsAgcmEditBlockingDlg->DestroyWindow();

		delete g_pcsAgcmEditBlockingDlg;
		g_pcsAgcmEditBlockingDlg = NULL;

		m_bOpenEditBlocking = FALSE;

		INT16 nCount;
		for(nCount = 0; nCount < m_nMaxBlocking; ++nCount)
		{
			if(m_pstBlocking[nCount].type == AUBLOCKING_TYPE_NONE)
				break;
		}

		*(m_pnBlockingNum) = nCount;

		EnumCallback(AGCMBLOCKINGDLG_CB_ID_FINISH_UP_EDITING_BLOCKING, m_pvFinishUpCBData, NULL);
	}
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsScale()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsScale();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsTranslate()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsTranslate();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsRight()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsRight();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsUp()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsUp();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsAt()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsAt();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsAll()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsAll();
}

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsHeight()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsHeight();
}

/******************************************************************************
* Purpose :
*
* 122702. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::EditIsNone()
{
	if(!g_pcsAgcmEditBlockingDlg)
		return FALSE;

	return g_pcsAgcmEditBlockingDlg->IsNone();
}

BOOL AgcmBlockingDlg::IsUpdateClick()
{
	if( g_pcsAgcmEditBlockingDlg )
		return g_pcsAgcmEditBlockingDlg->IsClick();

	return FALSE;
}

void AgcmBlockingDlg::ClearEdit()
{
	if( g_pcsAgcmEditBlockingDlg )
		g_pcsAgcmEditBlockingDlg->ClearEdit();

}

void AgcmBlockingDlg::GetVector( float& fX, float& fY, float& fZ )
{
	fX = g_pcsAgcmEditBlockingDlg->m_fEditX;
	fY = g_pcsAgcmEditBlockingDlg->m_fEditY;
	fZ = g_pcsAgcmEditBlockingDlg->m_fEditZ;
}

void AgcmBlockingDlg::ScaleBlock( const float& fScale )
{
	AuBLOCKING* pBlocking = GetCurrentBlocking();
	if( !pBlocking )		return;

	RwV3d	vScale	= { 0.0f, 0.0f, 0.0f };
	if( EditIsAll() || EditIsRight() )	vScale.x += fScale;
	if( EditIsAll() || EditIsUp() )		vScale.y += fScale;
	if( EditIsAll() || EditIsAt() )		vScale.z += fScale;

	ScaleBlock( vScale );
}

void AgcmBlockingDlg::ScaleBlock( const RwV3d& vScale )
{
	AuBLOCKING* pBlocking = GetCurrentBlocking();
	if( !pBlocking )		return;

	switch( pBlocking->type )
	{
	case AUBLOCKING_TYPE_BOX:
		{
			RwV3d	vSup, vInf;
			
			AuBOX* pBox = &pBlocking->data.box;
			RwV3dAdd( &vSup, &pBox->sup, &vScale );
			RwV3dSub( &vInf, &pBox->inf, &vScale );

			if((vSup.x <= vInf.x) || (vSup.y <= vInf.y) || (vSup.z <= vInf.z))
				return; // skip

			memcpy( &pBox->sup, &vSup, sizeof(AuPOS) );
			memcpy( &pBox->inf, &vInf, sizeof(AuPOS) );
		}
		break;

	case AUBLOCKING_TYPE_SPHERE:
		{
			pBlocking->data.sphere.radius += vScale.x; // x, y, z중에 하나
		}
		break;

	case AUBLOCKING_TYPE_CYLINDER:
		{
			AuCYLINDER* pCylinder = &pBlocking->data.cylinder;
			if( EditIsHeight() )
			{
				if( vScale.y != 0.f )
					pCylinder->height += vScale.y;
				else if( vScale.z != 0.f )
					pCylinder->height += vScale.z;
			}
			else	pCylinder->radius += vScale.x;
		}
		break;
	}
}

void AgcmBlockingDlg::TransBlock( const RwV2d& vec, const RwCamera* pCamera )
{
	if( !pCamera )		return;

	AuBLOCKING* pBlocking = GetCurrentBlocking();
	if( !pBlocking )	return ;

	RwV3d	vTrans;
	if( EditIsAll() )
	{
		RwMatrix* pMatCamera = RwFrameGetMatrix( RwCameraGetFrame( pCamera ) );
		RwV3d pVecAxis[2];

		pVecAxis[0] = *(RwMatrixGetRight(pMatCamera));
		pVecAxis[1] = *(RwMatrixGetUp(pMatCamera));

		RwV3dScale( pVecAxis+0, pVecAxis+0, vec.x * -1.f );
		RwV3dScale( pVecAxis+1, pVecAxis+1, vec.y * -1.f );
		RwV3dAdd( &vTrans, pVecAxis+0, pVecAxis+1 );
	}
	else
	{
		RwV3d	vDir = { 0.f, 0.f, 0.f };
		if( EditIsRight() )		vDir.x = 1.f;
		else if( EditIsUp() )	vDir.y = 1.f;
		else if( EditIsAt() )	vDir.z = 1.f;

		RwV3dScale( &vTrans, &vDir, vec.y );
	}

	TransBlock( vTrans, TRUE );
}

RwV3d	GetCenter( const RwV3d* lhs, const RwV3d* rhs )
{
	RwV3d	center;
	RwV3dAdd( &center, lhs, rhs );
	RwV3dScale( &center, &center, 0.5f );
	return center;
}


void AgcmBlockingDlg::TransBlock( const RwV3d& vTrans, BOOL bOffset )
{
	AuBLOCKING* pBlocking = GetCurrentBlocking();
	if( !pBlocking )		return;

	switch( pBlocking->type )
	{
	case AUBLOCKING_TYPE_BOX:
		{
			if( bOffset )
			{
				RwV3dAdd( (RwV3d*)&pBlocking->data.box.inf, (RwV3d*)&pBlocking->data.box.inf, &vTrans );
				RwV3dAdd( (RwV3d*)&pBlocking->data.box.sup, (RwV3d*)&pBlocking->data.box.sup, &vTrans );
			}
			else
			{
				RwV3d center = GetCenter( (RwV3d*)&pBlocking->data.box.inf, (RwV3d*)&pBlocking->data.box.sup );

				RwV3d	vOffset;
				RwV3dSub( &vOffset, &vTrans, &center );
				RwV3dAdd( (RwV3d*)&pBlocking->data.box.inf, (RwV3d*)&pBlocking->data.box.inf, &vOffset );
				RwV3dAdd( (RwV3d*)&pBlocking->data.box.sup, (RwV3d*)&pBlocking->data.box.sup, &vOffset );				
			}

			RwV3d center = GetCenter( (RwV3d*)&pBlocking->data.box.inf, (RwV3d*)&pBlocking->data.box.sup );
			g_pcsAgcmEditBlockingDlg->SetPos( center.x, center.y, center.z );
		}
		break;
	case AUBLOCKING_TYPE_SPHERE:
		{
			AuPOS& center = pBlocking->data.sphere.center;
			if( bOffset )
				RwV3dAdd( (RwV3d*)&center, (RwV3d*)&center, &vTrans );
			else
				RwV3dAssign( (RwV3d*)&center, &vTrans );
			
			g_pcsAgcmEditBlockingDlg->SetPos( center.x, center.y, center.z );
		}
		break;
	case AUBLOCKING_TYPE_CYLINDER:
		{
			AuPOS& center = pBlocking->data.cylinder.center;
			if( bOffset )
				RwV3dAdd( (RwV3d*)&center, (RwV3d*)&center, &vTrans );
			else
				RwV3dAssign( (RwV3d*)&center, &vTrans );
			g_pcsAgcmEditBlockingDlg->SetPos( center.x, center.y, center.z );
		}
		break;
	}
}

BOOL AgcmBlockingDlg::UpdateBlock( const RwV3d& vec )
{
	if( EditIsNone() )	return FALSE;
	if( vec.x == 0.f && vec.y == 0.f && vec.z == 0.f )
		return FALSE;

	if( EditIsScale() )
		ScaleBlock( vec );
	else if( EditIsTranslate() )
		TransBlock( vec, TRUE );

	return TRUE;
}

BOOL AgcmBlockingDlg::UpdateBlock( const RwV2d& vec, const RwCamera* pCamera )
{
	if( EditIsNone() )	return FALSE;
	if( !pCamera )		return FALSE;
	if( vec.x == 0.f && vec.y == 0.f )
		return FALSE;

	if( EditIsScale() )
		ScaleBlock( vec.y );
	else if( EditIsTranslate() )
		TransBlock( vec, pCamera );

	return TRUE;
}


/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::OnInit()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::OnAddModule()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::OnDestroy()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::SetCallbackInitBlocking(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMBLOCKINGDLG_CB_ID_INIT_BLOCKING, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 150604. Bob Jung
******************************************************************************/
BOOL AgcmBlockingDlg::SetCallbackFinishUpEditingBlocking(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMBLOCKINGDLG_CB_ID_FINISH_UP_EDITING_BLOCKING, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 021703. Bob Jung
******************************************************************************/
VOID AgcmBlockingDlg::EnumCallbackInitBlocking(PVOID pData)
{
	EnumCallback(AGCMBLOCKINGDLG_CB_ID_INIT_BLOCKING, pData, NULL);
}

/******************************************************************************
******************************************************************************/
