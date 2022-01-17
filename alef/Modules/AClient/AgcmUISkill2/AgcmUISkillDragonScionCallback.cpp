#include "AgcmUISkillDragonScion.h"
#include "AgcmUISkillDragonScion.inl"

#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgpmEventSKillMaster.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmSkill.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgpmSkill.h"
#include "AgcmUISkill2.h"



BOOL AgcmUISkillDragonScion::CBDisplaySkillScion0( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )			return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_0 ) );

	return TRUE;
}


BOOL AgcmUISkillDragonScion::CBDisplaySkillScion1_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_1_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion1_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_1_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion1_3( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;
	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_1_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion2_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_2_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion2_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_2_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion2_3( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_2_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion3_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_3_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion3_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_3_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion4_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_4_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion4_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_4_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion4_3( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_4_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion5_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_5_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion5_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_5_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion5_3( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_5_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion6_1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_6_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion6_2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_6_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillScion6_3( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetScionSkillPoint( SKILL_SCION_6_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter1_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_1_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter1_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_1_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter1_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_1_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter1_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_1_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter1_5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_1_5 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter2_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_2_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter2_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_2_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter2_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_2_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter3_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_3_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter3_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_3_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter3_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_3_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillObiter3_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetObiterSkillPoint( SKILL_OBITER_3_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer1_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_1_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer1_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_1_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer1_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_1_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer2_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_2_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer2_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_2_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer2_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_2_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer2_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_2_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer1_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_1_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer3_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_3_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer3_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" ,pThis->_GetSlayerSkillPoint( SKILL_SLAYER_3_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSlayer3_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSlayerSkillPoint( SKILL_SLAYER_3_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener1_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" ,pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_1_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener1_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_1_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener1_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_1_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener1_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_1_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener2_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_2_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener2_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_2_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener2_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_2_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener2_4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_2_4 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener1_5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_1_5 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener3_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_3_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener3_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_3_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillSummener3_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetSummernerSkillPoint( SKILL_SUMMENER_3_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive1_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_1_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive1_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_1_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive1_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_1_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive2_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_2_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive2_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_2_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive2_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_2_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive3_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_3_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive3_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_3_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive3_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue0)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_3_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive4_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_4_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive4_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_4_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive4_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_4_3 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive5_1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_5_1 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive5_2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_5_2 ) );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplaySkillPassive5_3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkillDragonScion* pThis			= static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis )		return FALSE;

	sprintf( szDisplay , "%d" , pThis->_GetPassiveSkillPoint( SKILL_PASSIVE_5_3 ) );

	return TRUE;
}


BOOL AgcmUISkillDragonScion::CBCloseScionSkillUI( VOID* pClass , VOID* pData1 , VOID* pData2 , VOID* pData3 , VOID* pData4 , VOID* pData5 , ApBase* pTarget , AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->CloseSkillUI();

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBCloseScionUPSkillUI( VOID* pClass , VOID* pData1 , VOID* pData2 , VOID* pData3 , VOID* pData4 , VOID* pData5 , ApBase* pTarget , AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->CloseSkillUpUI();

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSelectScionSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);
	INT32					nSkillTID			=	0;

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrSelectIndex[ SkillUI_Scion ]  = pThis->_GetSkillIndex( SkillUI_Scion , nSkillTID );
	}

	if( pThis->CheckRollbackSkill( nSkillTID ) )
		pThis->m_bScionActiveRollbackButton	=	TRUE;
	else
		pThis->m_bScionActiveRollbackButton	=	FALSE;

	pThis->UnSelectSkillUI( SkillUI_Scion );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventScionSelectedItem[ pThis->m_nArrSelectIndex[SkillUI_Scion] ] );
	pThis->GetUIManager()->SetUserDataRefresh( pThis->m_pScionActiveRollbackButtonUserData );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSelectBuyScionSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Scion ]  = pThis->_GetSkillIndex( SkillUI_Scion , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Scion );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventScionUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Scion] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectUpgradeScionSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Scion ]  = pThis->_GetSkillIndex( SkillUI_Scion , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Scion );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventScionUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Scion] ] );

	return TRUE;
	
}



BOOL AgcmUISkillDragonScion::CBSelectSlayerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);
	INT						nSkillTID			=	0;

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrSelectIndex[ SkillUI_Slayer ]  = pThis->_GetSkillIndex( SkillUI_Slayer , nSkillTID );
	}

	if( pThis->CheckRollbackSkill( nSkillTID ) )
		pThis->m_bScionActiveRollbackButton	=	TRUE;
	else
		pThis->m_bScionActiveRollbackButton	=	FALSE;

	pThis->UnSelectSkillUI( SkillUI_Slayer );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSlayerSelectedItem[ pThis->m_nArrSelectIndex[SkillUI_Slayer] ] );
	pThis->GetUIManager()->SetUserDataRefresh( pThis->m_pScionActiveRollbackButtonUserData );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectBuySlayerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Slayer ]  = pThis->_GetSkillIndex( SkillUI_Slayer , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Slayer );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSlayerUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Slayer] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectUpgradeSlayerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Slayer ]  = pThis->_GetSkillIndex( SkillUI_Slayer , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Slayer );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSlayerUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Slayer] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectObiterSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);
	INT						nSkillTID			=	0;

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrSelectIndex[ SkillUI_Obiter ]  = pThis->_GetSkillIndex( SkillUI_Obiter , nSkillTID );
	}

	if( pThis->CheckRollbackSkill( nSkillTID ) )
		pThis->m_bScionActiveRollbackButton	=	TRUE;
	else
		pThis->m_bScionActiveRollbackButton	=	FALSE;

	pThis->UnSelectSkillUI( SkillUI_Obiter );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventObiterSelectedItem[ pThis->m_nArrSelectIndex[SkillUI_Obiter] ] );
	pThis->GetUIManager()->SetUserDataRefresh( pThis->m_pScionActiveRollbackButtonUserData );

	return TRUE;
}


BOOL AgcmUISkillDragonScion::CBSelectBuyObiterSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Obiter ]  = pThis->_GetSkillIndex( SkillUI_Obiter , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Obiter );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventObiterUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Obiter] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectUpgradeObiterSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Obiter ]  = pThis->_GetSkillIndex( SkillUI_Obiter , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Obiter );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventObiterUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Obiter] ] );

	return TRUE;

}

BOOL AgcmUISkillDragonScion::CBSelectPassiveSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);
	INT						nSkillTID			=	0;

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrSelectIndex[ SkillUI_Passive ]  = pThis->_GetSkillIndex( SkillUI_Passive , nSkillTID );
	}

	if( pThis->CheckRollbackSkill( nSkillTID ) )
		pThis->m_bScionActiveRollbackButton	=	TRUE;
	else
		pThis->m_bScionActiveRollbackButton	=	FALSE;

	pThis->UnSelectSkillUI( SkillUI_Passive );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventPassiveSelectedItem[ pThis->m_nArrSelectIndex[SkillUI_Passive] ] );
	pThis->GetUIManager()->SetUserDataRefresh( pThis->m_pScionActiveRollbackButtonUserData );

	return TRUE;	
}

BOOL AgcmUISkillDragonScion::CBSelectBuyPassiveSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Passive ]  = pThis->_GetSkillIndex( SkillUI_Passive , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Passive );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventPassiveUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Passive] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectUpgradePassiveSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Passive ]  = pThis->_GetSkillIndex( SkillUI_Passive , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Passive );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventPassiveUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Passive] ] );

	return TRUE;
	
}

BOOL AgcmUISkillDragonScion::CBSelectSummernerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);
	INT						nSkillTID			=	0;

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrSelectIndex[ SkillUI_Summerner ]  = pThis->_GetSkillIndex( SkillUI_Summerner , nSkillTID );
	}

	if( pThis->CheckRollbackSkill( nSkillTID ) )
		pThis->m_bScionActiveRollbackButton	=	TRUE;
	else
		pThis->m_bScionActiveRollbackButton	=	FALSE;

	pThis->UnSelectSkillUI( SkillUI_Summerner );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSummernerSelectedItem[ pThis->m_nArrSelectIndex[SkillUI_Summerner] ] );
	pThis->GetUIManager()->SetUserDataRefresh( pThis->m_pScionActiveRollbackButtonUserData );

	return TRUE;

}

BOOL AgcmUISkillDragonScion::CBSelectBuySummernerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Summerner ]  = pThis->_GetSkillIndex( SkillUI_Summerner , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Summerner );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSummernerUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Summerner] ] );

	return TRUE;

}

BOOL AgcmUISkillDragonScion::CBSelectUpgradeSummernerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmUISkillDragonScion	*pThis				=	static_cast< AgcmUISkillDragonScion *	>(pClass);
	AcUIGrid				*pGrid				=	static_cast< AcUIGrid*					>(pControl->m_pcsBase);

	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->m_nArrUpgradeSelectIndex[ SkillUI_Summerner ]  = pThis->_GetSkillIndex( SkillUI_Summerner , nSkillTID );
	}

	pThis->UnSelectUpgradeSkillUI( SkillUI_Summerner );	
	pThis->GetUIManager()->ThrowEvent( pThis->m_nEventSummernerUpgradeSelectedItem[ pThis->m_nArrUpgradeSelectIndex[SkillUI_Summerner] ] );

	return TRUE;

}

BOOL AgcmUISkillDragonScion::CBSelectArchlordSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkillDragonScion	*pThis			=	static_cast< AgcmUISkillDragonScion*	>(pClass);
	AgcdUIControl			*pcsUIControl	=	static_cast< AgcdUIControl*				>(pData1);
	AcUIGrid				*pGrid			=	static_cast< AcUIGrid*					>(pcsUIControl->m_pcsBase);

	if (pThis->m_nArchlordSkillSelectIndex >= 0)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_nArchlordSkillUnSelect, pThis->m_nArchlordSkillSelectIndex);

	pThis->m_nArchlordSkillSelectIndex		= pControl->m_lUserDataIndex;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_nArchlordSkillSelect, pThis->m_nArchlordSkillSelectIndex);

	return TRUE;
}


BOOL AgcmUISkillDragonScion::CBOpenScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	pThis->m_pAgcmUISkill2->OpenSkillToolTip( pControl , AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL );
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBCloseScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	pThis->m_pAgcmUISkill2->CloseSkillToolTip();
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenUpgradeScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	INT32		(AgcmUISkillDragonScion::*GetEventSkillFunc)	( INT32 );
				
	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AcUIGrid*	pGrid	=	static_cast< AcUIGrid* >(pControl->m_pcsBase);

	if( !pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	INT32	SkillIndex	=	pThis->_GetSkillIndex( pThis->m_eCurrentUpUI , pGrid->m_pToolTipAgpdGridItem->m_lItemTID );

	switch( pThis->m_eCurrentUpUI )
	{
	case SkillUI_Scion:			GetEventSkillFunc	=	&AgcmUISkillDragonScion::_GetScionSkillPoint;		break;
	case SkillUI_Slayer:		GetEventSkillFunc	=	&AgcmUISkillDragonScion::_GetSlayerSkillPoint;		break;
	case SkillUI_Obiter:		GetEventSkillFunc	=	&AgcmUISkillDragonScion::_GetObiterSkillPoint;		break;
	case SkillUI_Summerner:		GetEventSkillFunc	=	&AgcmUISkillDragonScion::_GetSummernerSkillPoint; 	break;
	case SkillUI_Passive:		GetEventSkillFunc	=	&AgcmUISkillDragonScion::_GetPassiveSkillPoint;		break;
	default:					return FALSE;
	}

	AgcmUISkill2_TooltipType		eToolTipType;

	if( ( (pThis->*GetEventSkillFunc)(SkillIndex)) )		eToolTipType	=	AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE;
	else										eToolTipType	=	AGCMUI_SKILL2_TOOLTIP_TYPE_BUY;

	pThis->m_pAgcmUISkill2->OpenSkillToolTip( pControl , eToolTipType );
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBCloseUpgradeScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	pThis->m_pAgcmUISkill2->CloseSkillToolTip();
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenBuyScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	pThis->m_pAgcmUISkill2->OpenSkillToolTip( pControl , AGCMUI_SKILL2_TOOLTIP_TYPE_BUY );
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBCloseBuyScionSkillToolTip( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	return FALSE;

	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	pThis->m_pAgcmUISkill2->CloseSkillToolTip();
	return TRUE;
}


BOOL AgcmUISkillDragonScion::CBScionSkillRollBack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR						szUIMessage [ MAX_PATH ];
	AgcmUISkillDragonScion*		pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdEventSkillHighLevel		stHighLevel		=	pThis->_GetScionSkill( pThis->m_nArrSelectIndex[ SkillUI_Scion ] );
	AgpdSkill*					pcsSkill		=	NULL;
	AgpdSkillTemplate*			pcsTemplate		=	NULL;

	// 있는 스킬인지 확인
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), stHighLevel.m_lSkillTID );
	if(!pcsSkill)		return FALSE;

	pcsTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pcsSkill->m_pcsTemplate->GetID() );

	// 변신을 안한 상태에서만 사용 가능
	if( pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;

	// Condition 체크
	if( !pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll
		(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsTemplate->m_szName );	
	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );

	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->m_nArrSelectIndex[ SkillUI_Scion ]		=	-1;
		return pThis->m_pcsAgcmSkill->SendRequestRollback(pcsSkill->m_lID);
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSlayerSkillRollBack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR						szUIMessage [ MAX_PATH ];
	AgcmUISkillDragonScion*		pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdEventSkillHighLevel		stHighLevel		=	pThis->_GetSlayerSkill( pThis->m_nArrSelectIndex[ SkillUI_Slayer ] );
	AgpdSkill*					pcsSkill		=	NULL;
	AgpdSkillTemplate*			pcsTemplate		=	NULL;

	// 고렙스킬
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), stHighLevel.m_lSkillTID );
	if(!pcsSkill)		return FALSE;

	pcsTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pcsSkill->m_pcsTemplate->GetID() );

	// 변신을 안한 상태에서만 사용 가능
	if( pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;

	// Condition 체크
	if( !pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsTemplate->m_szName );	
	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );


	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->m_nArrSelectIndex[ SkillUI_Slayer ]		=	-1;
		return pThis->m_pcsAgcmSkill->SendRequestRollback(pcsSkill->m_lID );
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBObiterSkillRollBack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR						szUIMessage [ MAX_PATH ];	
	AgcmUISkillDragonScion*		pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdEventSkillHighLevel		stHighLevel		=	pThis->_GetObiterSkill( pThis->m_nArrSelectIndex[ SkillUI_Obiter ] );
	AgpdSkill*					pcsSkill		=	NULL;
	AgpdSkillTemplate*			pcsTemplate		=	NULL;

	// 고렙스킬
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), stHighLevel.m_lSkillTID );
	if(!pcsSkill)		return FALSE;

	pcsTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pcsSkill->m_pcsTemplate->GetID() );

	// 변신을 안한 상태에서만 사용 가능
	if( pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;

	// Condition 체크
	if( !pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsTemplate->m_szName );	
	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );

	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->m_nArrSelectIndex[ SkillUI_Obiter ]		=	-1;
		return pThis->m_pcsAgcmSkill->SendRequestRollback( pcsSkill->m_lID );
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBPassiveSkillRollBack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR						szUIMessage [ MAX_PATH ];	
	AgcmUISkillDragonScion*		pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdEventSkillHighLevel		stHighLevel		=	pThis->_GetPassiveSkill( pThis->m_nArrSelectIndex[ SkillUI_Passive ] );
	AgpdSkill*					pcsSkill		=	NULL;
	AgpdSkillTemplate*			pcsTemplate		=	NULL;

	// 고렙스킬
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), stHighLevel.m_lSkillTID );
	if(!pcsSkill)		return FALSE;

	pcsTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pcsSkill->m_pcsTemplate->GetID() );

	// 변신을 안한 상태에서만 사용 가능
	if( pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;
	
	// Condition 체크
	if( !pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsTemplate->m_szName );	
	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );

	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->m_nArrSelectIndex[ SkillUI_Passive ]		=	-1;
		return pThis->m_pcsAgcmSkill->SendRequestRollback( pcsSkill->m_lID );
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSummernerSkillRollBack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR						szUIMessage [ MAX_PATH ];	
	AgcmUISkillDragonScion*		pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdEventSkillHighLevel		stHighLevel		=	pThis->_GetSummernerSkill( pThis->m_nArrSelectIndex[ SkillUI_Summerner ] );
	AgpdSkill*					pcsSkill		=	NULL;
	AgpdSkillTemplate*			pcsTemplate		=	NULL;

	// 고렙스킬
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), stHighLevel.m_lSkillTID );
	if(!pcsSkill)		return FALSE;

	pcsTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pcsSkill->m_pcsTemplate->GetID() );

	// 변신을 안한 상태에서만 사용 가능
	if( pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;

	// Condition 체크
	if( !pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsTemplate->m_szName );	
	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );

	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->m_nArrSelectIndex[ SkillUI_Summerner ]		=	-1;
		return pThis->m_pcsAgcmSkill->SendRequestRollback( pcsSkill->m_lID );
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBScionSkillUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmUISkillDragonScion*			pThis				=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdCharacter*					pAgpdCharacter		=	pThis->GetAgcmCharacter()->GetSelfCharacter();
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;
	AgpdSkill*						pcsSkill			=	NULL;

	if( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Scion ] == -1 )
		return FALSE;

	AgpdEventSkillHighLevel	stHighLevel	=	pThis->_GetScionSkill( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Scion ] );

	pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill		=	pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	// 업그레이드가 불가능하면 그냥 종료한다
	if( pThis->IsActiveUpgradeItem( stHighLevel.m_lSkillTID ) == SKILL_STATUS_FAIL )
		return FALSE;
	
	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );
	
	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT32	nAnswer						=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	if( !nAnswer )		
		return FALSE;

	pThis->m_pcsEvent					=	pThis->m_pAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}
	
	pThis->m_nArrUpgradeSelectIndex[ SkillUI_Scion ]	=	-1;
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSlayerSkillUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmUISkillDragonScion*			pThis				=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdCharacter*					pAgpdCharacter		=	pThis->GetAgcmCharacter()->GetSelfCharacter();
	AuCharClassType					eClassType			=	pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pAgpdCharacter->m_lTID1 );
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;
	AgpdSkill*						pcsSkill			=	NULL;


	if( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Slayer ] == -1 )
		return FALSE;

	if( eClassType	!=  AUCHARCLASS_TYPE_KNIGHT	)
		return FALSE;

	AgpdEventSkillHighLevel	stHighLevel	=	pThis->_GetSlayerSkill( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Slayer ] );

	pcsSkillTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill			=	pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	// 업그레이드가 불가능하면 그냥 종료한다
	if( pThis->IsActiveUpgradeItem( stHighLevel.m_lSkillTID ) != SKILL_STATUS_SUCCESS )
		return FALSE;

	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );

	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT32	nAnswer						=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	if( !nAnswer )				return FALSE;

	pThis->m_pcsEvent					=	pThis->m_pAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}

	pThis->m_nArrUpgradeSelectIndex[ SkillUI_Slayer ]	=	-1;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBObiterSkillUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmUISkillDragonScion*			pThis				=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdCharacter*					pAgpdCharacter		=	pThis->GetAgcmCharacter()->GetSelfCharacter();
	AuCharClassType					eClassType			=	pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pAgpdCharacter->m_lTID1 );
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;	
	AgpdSkill*						pcsSkill			=	NULL;

	if( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Obiter ] == -1 )
		return FALSE;

	if( eClassType	!=	AUCHARCLASS_TYPE_RANGER	)
		return FALSE;

	AgpdEventSkillHighLevel	stHighLevel	=	pThis->_GetObiterSkill( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Obiter ] );

	pcsSkillTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill			=	pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );
	
	// 업그레이드가 불가능하면 그냥 종료한다
	if( pThis->IsActiveUpgradeItem( stHighLevel.m_lSkillTID ) != SKILL_STATUS_SUCCESS )
		return FALSE;

	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );

	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT32	nAnswer						=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	if( !nAnswer )		
		return FALSE;

	pThis->m_pcsEvent					=	pThis->m_pAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}

	pThis->m_nArrUpgradeSelectIndex[ SkillUI_Obiter ]	=	-1;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBPassiveSkillUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmUISkillDragonScion*			pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdCharacter*					pAgpdCharacter	=	pThis->GetAgcmCharacter()->GetSelfCharacter();
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;
	AgpdSkill*						pcsSkill			=	NULL;

	if( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Passive ] == -1 )
		return FALSE;

	AgpdEventSkillHighLevel	stHighLevel	=	pThis->_GetPassiveSkill( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Passive ] );

	pcsSkillTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill			=	pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	// 업그레이드가 불가능하면 그냥 종료한다
	if( pThis->IsActiveUpgradeItem( stHighLevel.m_lSkillTID ) == SKILL_STATUS_FAIL )
		return FALSE;
		
	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );

	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT32	nAnswer						=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	if( !nAnswer )			
		return FALSE;

	pThis->m_pcsEvent					=	pThis->m_pAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}
	
	pThis->m_nArrUpgradeSelectIndex[ SkillUI_Passive ]	=	-1;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBSummernerSkillUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmUISkillDragonScion*			pThis			=	static_cast< AgcmUISkillDragonScion* >(pClass);
	AgpdCharacter*					pAgpdCharacter	=	pThis->GetAgcmCharacter()->GetSelfCharacter();
	AuCharClassType					eClassType		=	pThis->m_pcsAgcmCharacter->GetClassTypeByTID( pAgpdCharacter->m_lTID1 );
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;
	AgpdSkill*						pcsSkill			=	NULL;

	if( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Summerner ] == -1 )
		return FALSE;

	if( eClassType	!= AUCHARCLASS_TYPE_MAGE	)
		return FALSE;

	AgpdEventSkillHighLevel	stHighLevel	=	pThis->_GetSummernerSkill( pThis->m_nArrUpgradeSelectIndex[ SkillUI_Summerner ] );

	pcsSkillTemplate		=		pThis->m_pcsAgpmSkill->GetSkillTemplate( stHighLevel.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill				=		pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	// 업그레이드가 불가능하면 그냥 종료한다
	if( pThis->IsActiveUpgradeItem( stHighLevel.m_lSkillTID ) != SKILL_STATUS_SUCCESS )
		return FALSE;

	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );

	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT32	nAnswer						=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	if( !nAnswer )			return FALSE;

	pThis->m_pcsEvent					=	pThis->m_pAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}
	
	pThis->m_nArrUpgradeSelectIndex[ SkillUI_Summerner ]	= -1;
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBDisplayScionSkillPoint( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	AgcmUISkillDragonScion*		pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	if( !pThis )	return FALSE;

	INT32		nSkillPoint		=	pThis->GetAgpmCharacter()->GetSkillPoint( pThis->GetAgcmCharacter()->GetSelfCharacter() );

	sprintf( szDisplay , "%d" , nSkillPoint );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBBuyScionSkillResult			(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkillDragonScion		*pThis			= (AgcmUISkillDragonScion *)		pClass;


	for( INT i = 1 ; i < SkillUI_Count  ; ++i )
	{
		pThis->LoadSkill( (eDragonScionSkillUIType)i );
		pThis->LoadBuySkill( (eDragonScionSkillUIType)i );
	}

	

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBUpgradeScionSkillResult		(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkillDragonScion	*pThis			= (AgcmUISkillDragonScion *)		pClass;
	PVOID					*ppvBuffer		= (PVOID *)				pCustData;

	AgpmEventSkillUpgradeResult	eResult	= (AgpmEventSkillUpgradeResult) (INT32) ppvBuffer[2];
	INT32			lSkillID		= (INT32) ppvBuffer[1];

	switch (eResult) {
	case AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS:
		{
			for( INT i = 1 ; i < SkillUI_Count  ; ++i )
			{
				pThis->LoadSkill( (eDragonScionSkillUIType)i );
				pThis->LoadBuySkill( (eDragonScionSkillUIType)i );
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBUpdateScionSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{

	AgcmUISkillDragonScion	*pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	for( INT i = 1 ; i < SkillUI_Count ; ++i )
	{
		pThis->LoadBuySkill( (eDragonScionSkillUIType) i );
		pThis->LoadSkill( (eDragonScionSkillUIType) i );
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBUpdateScionCost(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass )
		return FALSE;

	AgcmUISkillDragonScion	*pThis		=	static_cast< AgcmUISkillDragonScion* >(pClass);

	for( INT i = 1 ; i < SkillUI_Count ; ++i )
	{
		pThis->LoadBuySkill( (eDragonScionSkillUIType) i );
		pThis->LoadSkill( (eDragonScionSkillUIType) i );
	}

	return TRUE;
}