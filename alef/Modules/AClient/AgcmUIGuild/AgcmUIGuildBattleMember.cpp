#include "AgcmUIGuildBattleMember.h"
#include "AgcmUIGuild.h"

// ----------------- CGuildBattleUnit ------------------
CGuildBattleUnit::CGuildBattleUnit( INT32 lMax )
{
	m_lListMax = lMax;

	m_listBattleChar.clear();
}

CGuildBattleUnit::~CGuildBattleUnit()
{
	m_listBattleChar.clear();
}

void	CGuildBattleUnit::Clear()
{
	m_listBattleChar.clear();
}

BOOL	CGuildBattleUnit::Insert( const string& strName, INT32 lLevel )
{
	if( -1 != GetPos( strName ) )	return FALSE;

	m_listBattleChar.push_back( CGuildBattleNode( strName, lLevel ) );
	
	return TRUE;
}

BOOL	CGuildBattleUnit::Delete( const string& strName )
{
	for( GuildBattleNodeListItr Itr = m_listBattleChar.begin(); Itr != m_listBattleChar.end(); ++Itr )
	{
		if( strName == (*Itr).strName )
		{
			m_listBattleChar.erase( Itr );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	CGuildBattleUnit::Delete( INT32 nPos )
{
	GuildBattleNodeListItr Itr = GetItr( nPos );
	if( Itr == m_listBattleChar.end() )		return FALSE;

	m_listBattleChar.erase( Itr );
	return TRUE;
}

void	CGuildBattleUnit::DeleteAll()
{
	m_listBattleChar.clear();
}

BOOL	CGuildBattleUnit::Swap( INT32 nPos, BOOL bUp )
{
	INT32 nNextPos;

	if( bUp )	nNextPos = nPos - 1;
	else		nNextPos = nPos + 1;

	if( 0 > nNextPos || nNextPos > (int)m_listBattleChar.size() )
		return FALSE;

	GuildBattleNodeListItr Itr = GetItr( nPos );
	if( Itr == m_listBattleChar.end() )		return FALSE;
	GuildBattleNodeListItr ItrNext = GetItr( nNextPos );
	if( ItrNext == m_listBattleChar.end() )	return FALSE;

	//아씨. ㅡㅡ;; 이건 아닌뎅..
	CGuildBattleNode cNode( (*Itr) );
	(*Itr) = (*ItrNext);
	(*ItrNext) = cNode;

	return TRUE;
}

BOOL	CGuildBattleUnit::Swap( const string& strName, BOOL bUp )
{
	if( m_listBattleChar.empty() )	return FALSE;

	return Swap( GetPos( strName ), bUp );
}

INT32	CGuildBattleUnit::GetPos( const string& strName )
{
	int i = 0;
	for( GuildBattleNodeListItr Itr = m_listBattleChar.begin(); Itr != m_listBattleChar.end(); ++Itr, ++i )
		if( strName == (*Itr).strName )
			return i;

	return -1;
}

GuildBattleNodeListItr	CGuildBattleUnit::GetItr( INT32 nPos )
{
	if( m_listBattleChar.empty() )	return m_listBattleChar.end();

	INT32 i = 0;
	for( GuildBattleNodeListItr Itr = m_listBattleChar.begin(); Itr != m_listBattleChar.end(); ++Itr, ++i )
	{
		if( i == nPos )
			return Itr;
	}

	return m_listBattleChar.end();
}

// ----------------- AgcmUIGuildBattleMember ------------------
AgcmUIGuildBattleMember::AgcmUIGuildBattleMember()
{
}

AgcmUIGuildBattleMember::~AgcmUIGuildBattleMember()
{
}

BOOL AgcmUIGuildBattleMember::AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild )
{
	m_pcsAgcmUIManager2	= pUIMgr;
	m_pcsAgcmUIGuild	= pUIGuild;

	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListUp", CBBattleListUp, 0 ) )					return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListDown", CBBattleListDown, 0 ) )				return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListRandomSelect", CBBattleListCancel, 0 ) )	return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListDelete", CBBattleListDelete, 0 ) )			return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListDeleteAll", CBBattleListDeleteAll, 0 ) )	return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListDone", CBBattleListDone, 0 ) )				return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListCancel", CBBattleListCancel, 0 ) )			return FALSE;

	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleListSelect", CBBattleListSelect, 0 ) )			return FALSE;

	m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberSelect", 0, CBDisplayGuildBattleListSelect, AGCDUI_USERDATA_TYPE_INT32 );
	m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberNumber", 0, CBDisplayGuildBattleMemberNumber, AGCDUI_USERDATA_TYPE_INT32 );
	m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberName", 0, CBDisplayGuildBattleMemberName, AGCDUI_USERDATA_TYPE_INT32 );
	m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberLevel", 0, CBDisplayGuildBattleMemberLevel, AGCDUI_USERDATA_TYPE_INT32 );
	m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberMax", 0, CBDisplayGuildBattleMemberMax, AGCDUI_USERDATA_TYPE_INT32 );
	
	for( int i=0; i<GUILD_BATTLE_MEMBER_MAX; ++i )
		m_aBattleMemberListIndex[i] = i;

	m_pBattleMemberList = m_pcsAgcmUIManager2->AddUserData( "GuildBattleMemberList", m_aBattleMemberListIndex, sizeof(INT32), GUILD_BATTLE_MEMBER_MAX, AGCDUI_USERDATA_TYPE_INT32 );
	m_pBattleMemberList->m_stUserData.m_lCount = 0;
	m_pBattleMemberMax = m_pcsAgcmUIManager2->AddUserData( "GuildBattleMemberMax", &m_lBattleMemberMax, sizeof(INT32), GUILD_BATTLE_MEMBER_MAX, AGCDUI_USERDATA_TYPE_INT32 );

	Init( pUIMgr, "UI_GuildBattleMember" );

	return TRUE;
}


//static Func
BOOL AgcmUIGuildBattleMember::CBBattleListUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;
	if( pThis->m_lSelectMember <= 1 )	return TRUE;			//길마를 이동 불가능하다

	if( pThis->Swap( pThis->m_lSelectMember, TRUE ) )
		--pThis->m_lSelectMember;

	pThis->RefreshList();
	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;
	if( pThis->m_lSelectMember >= pThis->Size() )	return TRUE;
	if( pThis->m_lSelectMember == 0 )				return TRUE;	//길마를 이동 불가능하다

	if( pThis->Swap( pThis->m_lSelectMember, FALSE ) )
		++pThis->m_lSelectMember;

	pThis->RefreshList();
	return TRUE;
}

void AgcmUIGuildBattleMember::SetRandomGuildMember()
{
	//임의선택 버튼은.. 버려버리자..
	return;

	//INT32 lCount = 0;
	//while( lCount < (INT32)m_pcsAgcmUIGuild->m_listAgcdGuildMember.size() )
	//{
	//	INT32 lRand = rand() % m_pcsAgcmUIGuild->m_listAgcdGuildMember.size();
	//	AgcdGuildMember* pMember = m_pcsAgcmUIGuild->GetGuildMember( lRand );
	//	if( pMember && AGPMGUILD_MEMBER_STATUS_ONLINE == pMember->m_cStatus )
	//	{
	//		if( PushList( pMember->m_szMemberID, pMember->m_lLevel ) )
	//			return;
	//	}

	//	++lCount;
	//}
}

BOOL AgcmUIGuildBattleMember::CBBattleListRandomSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;

	pThis->SetRandomGuildMember();

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListDelete(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )		return FALSE;

	//방장은 지울수 없게 만들어야 겠는뎅..
	char* szMember = pThis->GetSelectMember();
	if( !szMember )		return FALSE;

	AgpmGuild* pPGuildMng = pThis->m_pcsAgcmUIGuild->m_pcsAgpmGuild;
	if( !pPGuildMng )	return FALSE;

	AgcmGuild* pCGuildMng = pThis->m_pcsAgcmUIGuild->m_pcsAgcmGuild;
	if( !pCGuildMng )	return FALSE;

	AgpdGuild* pcsGuild = pPGuildMng->GetGuildLock( pCGuildMng->GetSelfGuildID() );
	if( !pcsGuild )		return FALSE;

	if( strcmp( pcsGuild->m_szMasterID, szMember ) )
	{
		pThis->Delete( pThis->m_lSelectMember );
		pThis->RefreshList();
	}
	else
	{
		char* szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMember_NotEraseMaster" );
		SystemMessage.ProcessSystemMessage( szMessage ? szMessage : "길드마스터는 지울수 없습니다" );
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListDeleteAll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;

	AgpmGuild* pPGuildMng = pThis->m_pcsAgcmUIGuild->m_pcsAgpmGuild;
	if( !pPGuildMng )	return FALSE;

	AgcmGuild* pCGuildMng = pThis->m_pcsAgcmUIGuild->m_pcsAgcmGuild;
	if( !pCGuildMng )	return FALSE;

	AgpdGuild* pcsGuild = pPGuildMng->GetGuildLock( pCGuildMng->GetSelfGuildID() );
	if( !pcsGuild )		return FALSE;

	AgpdGuildMember* pMaster = pcsGuild->GetMaster();
	if( !pMaster )		return FALSE;

	//길마의 Level은 어떻게 들고와야 할런지 @@
	pThis->DeleteAll();
	pThis->Insert( pMaster->m_szID, pMaster->m_lLevel );		//길드마스터는 넣어줘야징 ^^
	pThis->RefreshList();

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListDone(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;

	UINT32 lPerson = pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattlePersonSize();
	if( lPerson > (UINT32)pThis->Size() )
	{
		char* szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMember_LackMember" );
		SystemMessage.ProcessSystemMessage( szMessage ? szMessage : "길드전에 필요한 인원이 모자랍니다" );
		return FALSE;
	}

	UINT32 lSizeMAX = (( AGPACHARACTER_MAX_ID_STRING + 1 ) + sizeof(INT16) ) * lPerson;
	CHAR* pMemberList = new CHAR[ lSizeMAX ];
	memset( pMemberList, 0, lSizeMAX );
	INT16 lCurPos = 0;
	for( UINT32 i = 0; i < lPerson; ++i )
	{
		GuildBattleNodeListItr Itr = pThis->GetItr( i );
		if( Itr == pThis->GetStringList().end() )
			continue;

		//글자 사이즈 박고
		INT16 lStrLen = (INT16)(*Itr).strName.size();
		memcpy( pMemberList + lCurPos, &lStrLen, sizeof(INT16) );
		lCurPos += sizeof(INT16);
		strncpy( pMemberList + lCurPos, (*Itr).strName.c_str(), lStrLen );
		lCurPos += lStrLen;
	}

	pThis->m_pcsAgcmUIGuild->m_pcsAgcmGuild->SendBattlePerson( (INT16)lPerson, pMemberList, lCurPos );

	if( eOpenRequest == pThis->m_eOpenType )
		pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().SendRequest();
	else if( eOpenAccept == pThis->m_eOpenType )
		pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().SendAccept();
	else
		ASSERT(0);

	delete [] pMemberList;

	pThis->Clear();
	pThis->Close();

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )	return FALSE;

	if( eOpenAccept == pThis->m_eOpenType )
		pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().SendReject();
	pThis->Clear();
	pThis->Close();

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBBattleListSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis )			return FALSE;
	if( !pcsSourceControl )	return FALSE;

	if( 0 > pcsSourceControl->m_lUserDataIndex || pcsSourceControl->m_lUserDataIndex >= pThis->Size() )
		return FALSE;

	pThis->m_lSelectMember = pcsSourceControl->m_lUserDataIndex;
	pThis->RefreshList();

	return TRUE;
}

//static Display
BOOL AgcmUIGuildBattleMember::CBDisplayGuildBattleListSelect( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl )
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if(*pcsIndex >= pThis->Size() || *pcsIndex < 0)
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	if( pThis->m_lSelectMember == pcsSourceControl->m_lUserDataIndex )
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);
	else
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID);

	return TRUE;
}


BOOL AgcmUIGuildBattleMember::CBDisplayGuildBattleMemberNumber( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )		return FALSE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	INT32 nIndex = *(INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if( 0 > nIndex || nIndex >= pThis->Size() )
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	sprintf( szDisplay, "%d", nIndex + 1 );

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::CBDisplayGuildBattleMemberName( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )		return FALSE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	INT32 nIndex = *(INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if( 0 > nIndex || nIndex >= pThis->Size() )
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	GuildBattleNodeListItr Itr = pThis->GetItr( nIndex );
	if( Itr == pThis->GetStringList().end() )
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	strcpy( szDisplay, (*Itr).strName.c_str() );

	return TRUE; 
}

BOOL AgcmUIGuildBattleMember::CBDisplayGuildBattleMemberLevel( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pClass || !pData )		return FALSE;

	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	INT32 nIndex = *(INT32*)pData;

	// 넘어온 인덱스가 맞지 않는다.
	if( 0 > nIndex || nIndex >= pThis->Size() )
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	GuildBattleNodeListItr Itr = pThis->GetItr( nIndex );
	if( Itr == pThis->GetStringList().end() )
	{
		szDisplay[0] = NULL;
		return TRUE;
	}

	sprintf( szDisplay, "%d", (*Itr).lLevel );

	return TRUE; 
}

BOOL AgcmUIGuildBattleMember::CBDisplayGuildBattleMemberMax( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pClass )					return FALSE;
	AgcmUIGuildBattleMember* pThis = (AgcmUIGuildBattleMember*)pClass;
	if( !pThis->m_pcsAgcmUIGuild )	return FALSE;

	CHAR* szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Member_Max" );
	sprintf( szDisplay, szMessage ? szMessage : "%d번까지 길드전에 참여합니다.",  pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattlePersonSize() );

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::Open( AgpdGuild* pGuild )
{
	AgpdGuildMember* pMaster = pGuild->GetMaster();
	if( !pMaster )		return FALSE;

	Insert( pMaster->m_szID, pMaster->m_lLevel );

	INT32 lIndex = 0;
	AgpdGuildMember** ppcsMember = NULL;
	for( ppcsMember = (AgpdGuildMember**)pGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pGuild->m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppcsMember )		break;
		if( !strcmp( pMaster->m_szID, (*ppcsMember)->m_szID ) )				continue;
		if( AGPMGUILD_MEMBER_STATUS_ONLINE != (*ppcsMember)->m_cStatus )	continue;
		if( AGPMGUILD_MEMBER_RANK_JOIN_REQUEST == (*ppcsMember)->m_lRank )	continue;
		if( AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST == (*ppcsMember)->m_lRank )	continue;
	
		Insert( (*ppcsMember)->m_szID, (*ppcsMember)->m_lLevel );
	}

	RefreshList();

	CUIOpenCloseSystem::Open();

	return TRUE;
}

BOOL AgcmUIGuildBattleMember::PushList( CHAR* szMember, INT32 lLevel )
{
	if( !szMember )		return FALSE;
	if( Insert( szMember, lLevel ) )
		RefreshList();

	return TRUE;
}

void	AgcmUIGuildBattleMember::RefreshList()
{
	m_pBattleMemberList->m_stUserData.m_lCount = Size();
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pBattleMemberList );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pBattleMemberMax );
}

char* AgcmUIGuildBattleMember::GetSelectMember()
{
	GuildBattleNodeListItr Itr = GetItr( m_lSelectMember );
	if( Itr == GetStringList().end() )		return NULL;

	return (char*)(*Itr).strName.c_str();
}