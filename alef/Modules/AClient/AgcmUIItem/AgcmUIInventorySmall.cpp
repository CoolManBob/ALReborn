#include "AgcmUIItem.h"

BOOL AgcmUIItem::CBInventorySmallLayer0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AcUIGrid			*pcsUIGrid;
	AgcdUIControl		*pcsControl;
	AcUIButton			*pcsLayer0Button;
	AcUIButton			*pcsLayer1Button;
	AcUIButton			*pcsLayer2Button;
	AcUIButton			*pcsLayer3Button;

	BOOL				bResult;

	bResult = FALSE;

	pcsControl = (AgcdUIControl *) pData1;
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pcsControl = (AgcdUIControl *) pData2;
	if( pcsControl != NULL )
	{
		pcsLayer0Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData3;
	if( pcsControl != NULL )
	{
		pcsLayer1Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData4;
	if( pcsControl != NULL )
	{
		pcsLayer2Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData5;
	if( pcsControl != NULL )
	{
		pcsLayer3Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	if( pcsUIGrid != NULL )
	{
		pThis->m_pcsInventory->m_lSelectedIndex = 0;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventory );
//		pcsUIGrid->SetNowLayer( 0 );

		pcsLayer0Button->SetButtonMode( ACUIBUTTON_MODE_CLICK );
		pcsLayer1Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer2Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer3Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventorySmallLayer1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AcUIGrid			*pcsUIGrid;
	AgcdUIControl		*pcsControl;
	AcUIButton			*pcsLayer0Button;
	AcUIButton			*pcsLayer1Button;
	AcUIButton			*pcsLayer2Button;
	AcUIButton			*pcsLayer3Button;

	BOOL				bResult;

	bResult = FALSE;

	pcsControl = (AgcdUIControl *) pData1;
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pcsControl = (AgcdUIControl *) pData2;
	if( pcsControl != NULL )
	{
		pcsLayer0Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData3;
	if( pcsControl != NULL )
	{
		pcsLayer1Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData4;
	if( pcsControl != NULL )
	{
		pcsLayer2Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData5;
	if( pcsControl != NULL )
	{
		pcsLayer3Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	if( pcsUIGrid != NULL )
	{
		pThis->m_pcsInventory->m_lSelectedIndex = 1;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventory );
//		pcsUIGrid->SetNowLayer( 1 );

		pcsLayer0Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer1Button->SetButtonMode( ACUIBUTTON_MODE_CLICK );
		pcsLayer2Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer3Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventorySmallLayer2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AcUIGrid			*pcsUIGrid;
	AgcdUIControl		*pcsControl;
	AcUIButton			*pcsLayer0Button;
	AcUIButton			*pcsLayer1Button;
	AcUIButton			*pcsLayer2Button;
	AcUIButton			*pcsLayer3Button;

	BOOL				bResult;

	bResult = FALSE;

	pcsControl = (AgcdUIControl *) pData1;

	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pcsControl = (AgcdUIControl *) pData2;
	if( pcsControl != NULL )
	{
		pcsLayer0Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData3;
	if( pcsControl != NULL )
	{
		pcsLayer1Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData4;
	if( pcsControl != NULL )
	{
		pcsLayer2Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData5;
	if( pcsControl != NULL )
	{
		pcsLayer3Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	if( pcsUIGrid != NULL )
	{
		pThis->m_pcsInventory->m_lSelectedIndex = 2;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventory );
//		pcsUIGrid->SetNowLayer( 2 );

		pcsLayer0Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer1Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer2Button->SetButtonMode( ACUIBUTTON_MODE_CLICK );
		pcsLayer3Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventorySmallLayer3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AcUIGrid			*pcsUIGrid;
	AgcdUIControl		*pcsControl;
	AcUIButton			*pcsLayer0Button;
	AcUIButton			*pcsLayer1Button;
	AcUIButton			*pcsLayer2Button;
	AcUIButton			*pcsLayer3Button;

	BOOL				bResult;

	bResult = FALSE;

	pcsControl = (AgcdUIControl *) pData1;

	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pcsControl = (AgcdUIControl *) pData2;
	if( pcsControl != NULL )
	{
		pcsLayer0Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData3;
	if( pcsControl != NULL )
	{
		pcsLayer1Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData4;
	if( pcsControl != NULL )
	{
		pcsLayer2Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	pcsControl = (AgcdUIControl *) pData5;
	if( pcsControl != NULL )
	{
		pcsLayer3Button = (AcUIButton * )pcsControl->m_pcsBase;
	}
	else
	{
		return bResult;
	}

	if( pcsUIGrid != NULL )
	{
		pThis->m_pcsInventory->m_lSelectedIndex = 3;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventory );
//		pcsUIGrid->SetNowLayer( 3 );

		pcsLayer0Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer1Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer2Button->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
		pcsLayer3Button->SetButtonMode( ACUIBUTTON_MODE_CLICK );

		bResult = TRUE;
	}

	return bResult;
}