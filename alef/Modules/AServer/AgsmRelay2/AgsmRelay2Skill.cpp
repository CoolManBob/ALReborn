/*===============================================================

	AgsmRelay2Skill.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include "AgsmRelay2Skill.h"
#include "AgspSkill.h"

//
//	==========		Skill		==========
//
void AgsmRelay2::InitPacketSkill()
	{
	m_csPacketSkill.SetFlagLength(sizeof(INT8));
	m_csPacketSkill.SetFieldType(AUTYPE_INT16,		1,								// eAgsmRelay2Operation
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,		// Owner ID
								 AUTYPE_CHAR,		_MAX_TREENODE_LENGTH + 1,		// Skill tree node
								 AUTYPE_CHAR,		_MAX_TREENODE_LENGTH + 1,		// product(compose)
								 AUTYPE_END,		0
								 );
	}


BOOL AgsmRelay2::OnParamSkill(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Skill *pcsRelaySkill = new AgsdRelay2Skill;

	CHAR*	pszCharName = NULL;
	CHAR*	pszTreeNode = NULL;
	//CHAR*	pszSpecialize = NULL;
	CHAR*	pszProduct = NULL;

	m_csPacketSkill.GetField(FALSE, pvPacket, 0,
							&pcsRelaySkill->m_eOperation,
							&pszCharName,
							&pszTreeNode,
							//&pszSpecialize,
							&pszProduct
							);
	pcsRelaySkill->m_ulNID = ulNID;

	if (!pszCharName || _T('\0') == *pszCharName)
		{
		pcsRelaySkill->Release();
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : CharID is NULL in OnParamSkill()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

		return FALSE;
		}

	_tcsncpy(pcsRelaySkill->m_szCharName, pszCharName, _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsRelaySkill->m_szTreeNode, pszTreeNode ? pszTreeNode : _T(""), _MAX_TREENODE_LENGTH);
	_tcsncpy(pcsRelaySkill->m_szProduct, pszProduct ? pszProduct : _T(""), _MAX_TREENODE_LENGTH);

	return EnumCallback(AGSMRELAY_PARAM_SKILL, (PVOID)pcsRelaySkill, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Invalid Parameter for Update Skill in AgsmRelay2::CBSkillUpdate()");

	AgsmRelay2*	pThis			= (AgsmRelay2 *)pClass;
	ApBase*			pcsBase			= (ApBase*)pData;

	return pThis->SendSkillUpdate(pcsBase);
	}


BOOL AgsmRelay2::SendSkillUpdate(ApBase *pcsBase)
	{
	if (!pcsBase || APBASE_TYPE_CHARACTER != pcsBase->m_eType ||
		!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pcsBase;
	AgpdSkillAttachData		*pcsAttachData	= m_pAgpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendSkillUpdate"));

	INT16	nPacketLength	= 0;
	CHAR	szTreeBuffer[_MAX_TREENODE_LENGTH + 1];
	CHAR	szProduct[_MAX_TREENODE_LENGTH + 1];

	ZeroMemory(szTreeBuffer, sizeof(szTreeBuffer));
	ZeroMemory(szProduct, sizeof(szProduct));

	m_pAgsmEventSkillMaster->EncodingSkillList((AgpdCharacter *) pcsBase, szTreeBuffer, sizeof(szTreeBuffer));
	if (m_pAgsmProduct)
		m_pAgsmProduct->EncodeCompose((AgpdCharacter *) pcsBase, szProduct, sizeof(szProduct));

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;

	PVOID	pvPacketSkill	= m_csPacketSkill.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														pcsCharacter->m_szID,
														szTreeBuffer,
														//NULL,			//szSpecializeBuffer
														szProduct
														);

	if (!pvPacketSkill)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketSkill, AGSMRELAY_PARAM_SKILL);	
	m_csPacketSkill.FreePacket(pvPacketSkill);

	return bResult;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Skill class		*/
/********************************************************/
//
AgsdRelay2Skill::AgsdRelay2Skill()
	{
	ZeroMemory(m_szCharName, sizeof(m_szCharName));
	ZeroMemory(m_szTreeNode, sizeof(m_szTreeNode));
	ZeroMemory(m_szProduct, sizeof(m_szProduct));
	}


BOOL AgsdRelay2Skill::SetParamUpdate(AuStatement* pStatement)
	{
	INT i = 0;
	pStatement->SetParam(i++, m_szTreeNode, sizeof(m_szTreeNode));
	pStatement->SetParam(i++, m_szProduct, sizeof(m_szProduct));
	pStatement->SetParam(i++, m_szCharName, sizeof(m_szCharName));

	return TRUE;
	}


void AgsdRelay2Skill::Dump(CHAR *pszOp)
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_SKILL.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s]"
						"ID[%s] Skill[%s] Product[%s]\n",
						st.wHour, st.wMinute, st.wSecond, pszOp,
						m_szCharName, m_szTreeNode, m_szProduct);
	AuLogFile_s(szFile, strCharBuff);*/
	}

//////////////////////////////////////////////////////////////////////////
// skill save - arycoat 2008.7
BOOL AgsmRelay2::OnParamSkillSave(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMSKILL_RELAY_SKILLSAVE* pPacket = (PACKET_AGSMSKILL_RELAY_SKILLSAVE*)pvPacket;

	AgsdRelay2SkillSave* pcsRelaySkill = new AgsdRelay2SkillSave(ulNID, pPacket->strCharName);
	pcsRelaySkill->m_eOperation = pPacket->nOperation;

	switch(pPacket->nOperation)
	{
		case AGSMDATABASE_OPERATION_INSERT:
			{
				pcsRelaySkill->m_nSkillTID = pPacket->m_pSaveData.SkillTID;
				pcsRelaySkill->m_nRemainTime = pPacket->m_pSaveData.RemainTime;
				string strExpireDate = pPacket->m_pSaveData.ExpireDate.Format(AUTIMESTAMP_ORACLE_TIME_FORMAT);
				strcpy(pcsRelaySkill->m_szExpireDate, strExpireDate.c_str());
				pcsRelaySkill->m_nAttribute = (UINT32)pPacket->m_pSaveData.Attribute;
			} break;
		case AGSMDATABASE_OPERATION_DELETE:
			{
				pcsRelaySkill->m_nSkillTID = pPacket->m_pSaveData.SkillTID;
			} break;
		default:
			return FALSE;
	}

	INT16 nIndex = pcsRelaySkill->m_eOperation + pPacket->nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pcsRelaySkill;

	return m_pAgsmDatabasePool->Execute(pQuery);
}
