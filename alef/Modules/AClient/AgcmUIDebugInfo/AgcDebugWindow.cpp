// AgcDebugWindow.cpp: implementation of the AgcDebugWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcDebugWindow.h"
#include "AgcmUIDebugInfo.h"

#include "RwCore.h"
#include "RpWorld.h"
#include "RpLODAtm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcDebugWindow::AgcDebugWindow(AgcmUIDebugInfo *pcsAgcmUIDebugInfo)
{
	m_pcsAgcmUIDebugInfo = pcsAgcmUIDebugInfo;
	m_ulPrevClockCount = 0;

	m_lFilterMode = 0;

#ifdef _PROFILE_
	m_pCurNode = NULL;//AuProfileManager::Root;
	m_iCommand	= -1;
	m_iMyCurPage = 0;

	m_iCheckIndex = 0;
	for(int i=0;i<10;++i)
		m_pCheckedNode[i] = NULL;
	m_iProfileMode = 0;
#endif // _PROFILE_
}

AgcDebugWindow::~AgcDebugWindow()
{
}

BOOL	AgcDebugWindow::OnInit()
{
#ifdef _PROFILE_
	AuProfileManager::Reset();
#endif

	return TRUE;
}

VOID	AgcDebugWindow::OnWindowRender()
{
	if (m_pcsAgcmUIDebugInfo && m_pcsAgcmUIDebugInfo->m_pcsAgcmFont)
	{
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->FontDrawStart(0);

		switch (m_pcsAgcmUIDebugInfo->GetCurrentDebugMode())
		{
		case AGCM_UIDEBUG_MODE_PROFILE:
			RenderProfile();
			break;

		case AGCM_UIDEBUG_MODE_MEMORY:
			RenderMemory();
			break;

		case AGCM_UIDEBUG_MODE_OBJECT:
			RenderObject();
			break;

		case AGCM_UIDEBUG_MODE_GRAPHIC:
			RenderGraphic();
			break;

		case AGCM_UIDEBUG_MODE_NETWORK:
			RenderNetwork();
			break;
		}

		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->FontDrawEnd();
	}
}

BOOL	AgcDebugWindow::OnKeyDown(RsKeyStatus *ks)
{
    switch(ks->keyCharCode)
    {
	case '-':
		if (m_pcsAgcmUIDebugInfo)
		{
			if (g_pEngine->IsLAltDown())
				m_pcsAgcmUIDebugInfo->EnableDebugInfo(FALSE);
			else if (!g_pEngine->IsShiftDown() && !g_pEngine->IsAltDown() && !g_pEngine->IsCtrlDown())
				m_pcsAgcmUIDebugInfo->SetDebugMode((AgcmUIDebugMode) ((m_pcsAgcmUIDebugInfo->GetCurrentDebugMode() + 1) % AGCM_UIDEBUG_MAX_MODE));
		}
		break;

	case '=':
		ChangeTextureFilterMode();
		break;
	}

	if (m_pcsAgcmUIDebugInfo->GetCurrentDebugMode() == AGCM_UIDEBUG_MODE_PROFILE)
	{
		switch (ks->keyCharCode)
		{
#pragma warning ( disable : 4065 )
#ifdef _PROFILE_
		case '1':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 51;
			else							m_iCommand = 1;
			break;
		case '2':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 52;
			else							m_iCommand = 2;
			break;

		case '3':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 53;
			else							m_iCommand = 3;
			break;

		case '4':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 54;
			else							m_iCommand = 4;
			break;

		case '5':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 55;
			else							m_iCommand = 5;
			break;

		case '6':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 56;
			else							m_iCommand = 6;
			break;

		case '7':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 57;
			else							m_iCommand = 7;
			break;

		case '8':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 58;
			else							m_iCommand = 8;
			break;

		case '9':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 59;
			else							m_iCommand = 9;
			break;

		case '0':
			if(g_pEngine->IsLCtrlDown())	m_iCommand = 50;
			else							m_iCommand = 0;
			break;

		case rsPGUP:
			m_iCommand = 90;
			break;

		case rsPGDN:
			m_iCommand = 91;
			break;

		case rsHOME:
			m_iCommand = 98;
			break;

		case rsEND:
			m_iCommand = 99;
			break;

		case rsDEL:
			if(g_pEngine->IsLCtrlDown())	AuProfileManager::Reset();
			else			m_iCommand = 97;
			break;

		case rsINS:
			if(m_iProfileMode == 0) m_iProfileMode = 1;
			else m_iProfileMode = 0;
			break;
#endif // _PROFILE_
		default:
			break;
		}
	}

	return TRUE;
}

BOOL	AgcDebugWindow::OnKeyUp		(RsKeyStatus *ks)
{
	return FALSE;
}

VOID	AgcDebugWindow::RenderProfile()
{
#ifdef _PROFILE_
	BOOL	bUpdate = FALSE;
	UINT32	ulCurrentClockCount = m_pcsAgcmUIDebugInfo->GetClockCount();

	if(m_ulPrevClockCount + 1000 <= ulCurrentClockCount) // 1초 간격
	{
		bUpdate = TRUE;
		m_ulPrevClockCount = ulCurrentClockCount;
	}
	
	if(bUpdate)
	{
		// profile manager update
		if(m_pCurNode == NULL)
		{
			m_pCurNode = AuProfileManager::Root;
		}
		else if(m_pCurNode == AuProfileManager::Roots[0])
		{
			CProfileNode* tnode = m_pCurNode->Child;
			if(tnode) m_pCurNode = tnode;
		}
		else if(m_pCurNode == AuProfileManager::Roots[1])
		{
			CProfileNode* tnode = m_pCurNode->Child;
			if(tnode) m_pCurNode = tnode;
		}

		// child로 이동
		if(m_iCommand >= 0 && m_iCommand <= 9)	
		{
			if(m_pCurNode)
			{
				CProfileNode* tnode = m_pCurNode->Get_Child(m_iCommand);
				if(tnode != NULL)
				{
					m_pCurNode = tnode;
					m_iMyCurPage = 0;
				}
			}

			m_iTotalItem = 0;
			CProfileNode* tnode = m_pCurNode;
			while(tnode)
			{
				tnode = tnode->Sibling;
				++m_iTotalItem;
			}
		}
		else if(m_iCommand >= 50 && m_iCommand <= 59)
		{
			if(m_pCurNode->Parent)
			{
				CProfileNode*	tnode = m_pCurNode->Parent->Child->Get_Sibling(m_iMyCurPage*10 + m_iCommand-50);
				if(tnode)
				{
					m_pCheckedNode[m_iCheckIndex++] = tnode;
					if(m_iCheckIndex >= 10 ) m_iCheckIndex = 0;
				}
			}
		}
		else if(m_iCommand == 90)
		{
			if(m_iMyCurPage > 0) --m_iMyCurPage;
			if(m_pCurNode->Parent)
			m_pCurNode = m_pCurNode->Parent->Child->Get_Sibling(m_iMyCurPage*10);
		}
		else if(m_iCommand == 91)
		{
			if(m_iTotalItem > 9) ++m_iMyCurPage;
			if(m_pCurNode->Parent)
			m_pCurNode = m_pCurNode->Parent->Child->Get_Sibling(m_iMyCurPage*10);
		}
		else if(m_iCommand == 98)
		{
			if(m_pCurNode)
			{
				CProfileNode* tnode = m_pCurNode->Get_Parent();
				if(tnode != NULL)	m_pCurNode = tnode;

				if(m_pCurNode->Parent)
				m_pCurNode = m_pCurNode->Parent->Child->Get_Sibling(0);

				m_iMyCurPage = 0;
			}
			m_iTotalItem = 0;

			CProfileNode* tnode = m_pCurNode;
			while(tnode)
			{
				tnode = tnode->Sibling;
				++m_iTotalItem;
			}
		}
		else if(m_iCommand == 99)
		{
			m_pCurNode = AuProfileManager::Roots[0];
			m_iMyCurPage = 0;

			m_iTotalItem = 0;
			CProfileNode* tnode = m_pCurNode;
			while(tnode)
			{
				tnode = tnode->Sibling;
				++m_iTotalItem;
			}
		}
		else if(m_iCommand == 97)
		{
			m_pCurNode = AuProfileManager::Roots[1];
			m_iMyCurPage = 0;

			m_iTotalItem = 0;
			CProfileNode* tnode = m_pCurNode;
			while(tnode)
			{
				tnode = tnode->Sibling;
				++m_iTotalItem;
			}
		}
		
		m_iCommand = -1;
		float		time = AuProfileManager::Get_Time_Since_Reset();
		float		time2 = AuProfileManager::Get_Time_Since_Start();

		// 가장 많은 시간 잡아 먹은 node검색
		CProfileNode*	search_node = AuProfileManager::Root->Child;
		CProfileNode*	maxnode = search_node;
		if(maxnode)
		{
			FindMaxItem(search_node,&maxnode);
		}

		memset(profile_max_str,'\0',200);

		if (maxnode)
		{
			float tf = maxnode->Get_Total_Time()/time;
			sprintf(profile_max_str,"■Max>> %40s ::  %3d  :: %4f :: %3d %% ▶ %5d :: %3d %%",
			maxnode->Get_Name(),maxnode->Get_Total_Calls(),
			maxnode->Get_Total_Time(),(int)(tf * 100.0f),
			maxnode->AccumulationCalls,(int)(maxnode->AccumulationTime / time2 * 100.0f));
		}

		CProfileNode* cnode = m_pCurNode;

		int index = 0;

		for(int j = 0; j<10;++j)
		memset(profile_str[j],'\0',200);
		
		while(cnode && index<10)
		{
			float tf = cnode->Get_Total_Time()/time;
			sprintf(profile_str[index],"%3d - %40s ::  %3d  :: %4f :: %3d %% ▶ %5d :: %3d %%",
			index,cnode->Get_Name(),cnode->Get_Total_Calls(),
			cnode->Get_Total_Time(),(int)(tf * 100.0f),
			cnode->AccumulationCalls,(int)(cnode->AccumulationTime / time2 * 100.0f));
						
			++index;
			cnode = cnode->Get_Sibling();
		}

		for(int i=0;i<10;++i)
			memset(profile_check_str[i],'\0',200);

		for(i=0;i<10;++i)
		{
			if(m_pCheckedNode[i])
			{
				float tf = m_pCheckedNode[i]->Get_Total_Time()/time;
				sprintf(profile_check_str[i],"%3d - %70s ::  %3d  :: %4f :: %3d %% ▶ %5d :: %3d %%",i,m_pCheckedNode[i]->Get_Name(),m_pCheckedNode[i]->Get_Total_Calls(),
				m_pCheckedNode[i]->Get_Total_Time(),(int)(tf * 100.0f),
				m_pCheckedNode[i]->AccumulationCalls,(int)(m_pCheckedNode[i]->AccumulationTime / time2 * 100.0f));
			}
		}

		if(m_iProfileMode == 0)
			AuProfileManager::Reset();
	}

	if(m_iMyCurPage >0) m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(50,10,"↑", 0 ,180,0xffffffff,false);
	for(int i=0;i<10;++i)
	{
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D( ( float ) 50, ( float ) ( 30 + i*30 ),profile_str[i], 0 ,180,0xffffffff,false);
	}

	if(m_iTotalItem >9) m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(60,10,"↓", 0 ,180,0xffffffff,false);

	m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(50,360,profile_max_str, 0 ,210,0xffffffff,false,false,true,0x7fff0000);

	for(i=0;i<10;++i)
	{
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D( ( float ) 50, ( float )  ( 400 + i*30 ) ,profile_check_str[i], 0 ,210,0xffffffff,false);
	}
#endif // _PROFILE_
}

VOID	AgcDebugWindow::RenderMemory()
{
	CHAR	szTemp[256];

	ApMemoryManager &	csManager = ApMemoryManager::GetInstance();
	INT32	lIndex;
	for (lIndex = 0; lIndex < csManager.m_lTypeIndex; ++lIndex)
	{
#ifdef _CPPRTTI
		wsprintf( szTemp, "MemoryPool(%02d) : Size(%d)\t%d/%d [%d] - %s", lIndex, csManager.m_lAllocSize[lIndex], csManager.m_lAllocCount[lIndex], csManager.m_lArraySize[lIndex], csManager.m_lAllocCountPeak[lIndex], csManager.m_TypeName[lIndex]);
#else
		wsprintf( szTemp, "MemoryPool(%02d) : Size(%d)\t%d/%d [%d]", lIndex, csManager.m_lAllocSize[lIndex], csManager.m_lAllocCount[lIndex], csManager.m_lArraySize[lIndex], csManager.m_lAllocCountPeak[lIndex]);
#endif
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 100.f + lIndex * 14, szTemp, 0, 255);
	}
}

RwTexture *	CBTextureCount(RwTexture *pstTexture, PVOID pvData)
{
	INT32 *	plCount = (INT32 *) pvData;

	++*plCount;

	return pstTexture;
}

RwTexture *	CBTextureRefCount(RwTexture *pstTexture, PVOID pvData)
{
	INT32 *	plCount = (INT32 *) pvData;

	*plCount += pstTexture->refCount;

	return pstTexture;
}

VOID	AgcDebugWindow::RenderObject()
{
	INT32	lIndex;
	INT32	lCount;
	INT32	lCount2;
	INT32	lCount3;
	CHAR	szTemp[128];
	CHAR	szTemplates[256];

	if (m_pcsAgcmUIDebugInfo->m_pcsApmMap)
	{
		sprintf(szTemp, "Compact Data : %d", m_pcsAgcmUIDebugInfo->m_pcsApmMap->GetCurrentLoadedSectorCount());
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100, 100, szTemp, 0, 255);
	}

	if (m_pcsAgcmUIDebugInfo->m_pcsAgcmMap)
	{
		wsprintf( szTemp , "Sectors To Load: %03d", m_pcsAgcmUIDebugInfo->m_pcsAgcmMap->GetSectorsToLoad() );
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100, 115, szTemp, 0 ,230);
	}

	if (m_pcsAgcmUIDebugInfo->m_pcsApmObject && m_pcsAgcmUIDebugInfo->m_pcsAgcmObject)
	{
		sprintf(szTemp, "Object : %d", m_pcsAgcmUIDebugInfo->m_pcsApmObject->m_clObjects.m_csObject.m_lDataCount);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100, 200, szTemp, 0, 255);

		ApdObjectTemplate *		pcsTemplate;
		AgcdObjectTemplate *	pstTemplate;

		lIndex = lCount = lCount2 = lCount3 = 0;
		for (pcsTemplate = m_pcsAgcmUIDebugInfo->m_pcsApmObject->GetObjectTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsAgcmUIDebugInfo->m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
		{
			pstTemplate = m_pcsAgcmUIDebugInfo->m_pcsAgcmObject->GetTemplateData(pcsTemplate);

			if (pstTemplate->m_lRefCount > 0)
			{
				lCount2 += pstTemplate->m_lRefCount;
				++lCount;
			}
		}

		sprintf(szTemp, " Loaded Object Template : %d (RefCount : %d)", lCount, lCount2);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100, 215, szTemp, 0, 255);
	}

	if (m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter && m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter)
	{
		sprintf(szTemp, "Character : %d", m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->m_csACharacter.m_csObject.m_lDataCount);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100, 300, szTemp, 0, 255);

		AgpdCharacterTemplate *		pcsTemplate;
		AgcdCharacterTemplate *		pstTemplate;

		szTemplates[0] = 0;
		lIndex = lCount = lCount2 = lCount3 = 0;
		for (pcsTemplate = m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
		{
			pstTemplate = m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetTemplateData(pcsTemplate);

			if (pstTemplate->m_lRefCount > 0)
			{
				lCount2 += pstTemplate->m_lRefCount;
			}

			if (pstTemplate->m_bLoaded || pstTemplate->m_lRefCount)
			{
				++lCount;
				sprintf(szTemplates, "%s / %s (%d%s)", szTemplates, pcsTemplate->m_szTName, pstTemplate->m_lRefCount, (pstTemplate->m_lRefCount && !pstTemplate->m_bLoaded) ? " Loading" : "");

				if (lCount && (lCount % 8) == 0)
				{
					++lCount3;
					m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 315.f + 15.f * lCount3, szTemplates, 0, 255);
					szTemplates[0] = 0;
				}
			}
		}
		if ((lCount % 8) != 0)
		{
			++lCount3;
			m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 315.f + 15.f * lCount3, szTemplates, 0, 255);
			szTemplates[0] = 0;
		}

		sprintf(szTemp, " Loaded Character Template : %d (RefCount : %d)", lCount,  lCount2);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 330.f + 15.f * lCount3, szTemp, 0, 255);

		sprintf(szTemp, " Character Remove Pool : %d / %d", m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->m_csAdminCharacterRemove.GetObjectCount(), m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->m_csAdminCharacterRemove.GetCount());
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 345.f + 15.f * lCount3, szTemp, 0, 255);
	}

	if (m_pcsAgcmUIDebugInfo->m_pcsAgpmItem && m_pcsAgcmUIDebugInfo->m_pcsAgcmItem)
	{
		sprintf(szTemp, "Item : %d", m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->csItemAdmin.m_csObject.m_lDataCount);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 430.f, szTemp, 0, 255);

		AgpdItemTemplate *		pcsTemplate;
		AgcdItemTemplate *		pstTemplate;

		lIndex = lCount = lCount2 = lCount3 = 0;
		for(AgpaItemTemplate::iterator it = m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->csTemplateAdmin.begin(); it != m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->csTemplateAdmin.end(); ++it)
		{
			pcsTemplate = it->second;
			pstTemplate = m_pcsAgcmUIDebugInfo->m_pcsAgcmItem->GetTemplateData(pcsTemplate);

			if (pstTemplate->m_lRefCount > 0)
			{
				lCount2 += pstTemplate->m_lRefCount;
				++lCount;
			}

			if (!pstTemplate->m_bLoaded && pstTemplate->m_lRefCount)
			{
				++lCount;
				sprintf(szTemplates, "%s / %s (%d%s)", szTemplates, pcsTemplate->m_szName, pstTemplate->m_lRefCount, (pstTemplate->m_lRefCount && !pstTemplate->m_bLoaded) ? " Loading" : "");

				if (lCount && (lCount % 7) == 0)
				{
					++lCount3;
					m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 445.f + 15.f * lCount3, szTemplates, 0, 255);
					szTemplates[0] = 0;
				}
			}
		}
		if ((lCount % 7) != 0)
		{
			++lCount3;
			m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 445.f + 15.f * lCount3, szTemplates, 0, 255);
			szTemplates[0] = 0;
		}

		sprintf(szTemp, " Loaded Item Template : %d (RefCount : %d)", lCount, lCount2);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 460.f + 15.f * lCount3, szTemp, 0, 255);

		sprintf(szTemp, " Item Remove Pool : %d / %d", m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->m_csAdminItemRemove.GetObjectCount(), m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->m_csAdminItemRemove.GetCount());
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 475.f + 15.f * lCount3, szTemp, 0, 255);
	}

	if (m_pcsAgcmUIDebugInfo->m_pcsAgcmResourceLoader)
	{
		sprintf(szTemp, "Resource Loader Queue : BackThread (%d) ForeThread(%d)",
				m_pcsAgcmUIDebugInfo->m_pcsAgcmResourceLoader->GetBackThreadWaitingCount(),
				m_pcsAgcmUIDebugInfo->m_pcsAgcmResourceLoader->GetForeThreadWaitingCount());
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 530.f, szTemp, 0, 255);

		sprintf(szTemp, "Resource to Remove : %d",
				m_pcsAgcmUIDebugInfo->m_pcsAgcmResourceLoader->GetRemoveQueue());
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 545.f, szTemp, 0, 255);
	}
}

VOID	AgcDebugWindow::RenderGraphic()
{
	INT32	lCount;
	INT32	lCount2;
	CHAR	szTemp[128];

	//wsprintf(szTemp,"PNum:%d",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_iPolygonNum);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800.f, 60.f, szTemp, 0 );

	//sprintf(szTemp,"RenderTotal:%f",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_fRenderTotalTick);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800, 90, szTemp, 0 );
	//sprintf(szTemp,"Object:%f",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_fObjectTickDiff);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800, 120, szTemp, 0 );
	//sprintf(szTemp,"Character(item):%f",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_fCharacterTickDiff);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800, 150, szTemp, 0 );
	//sprintf(szTemp,"Map:%f",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_fMapTickDiff);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800, 180, szTemp, 0 );
	//sprintf(szTemp,"Shadow:%f",m_pcsAgcmUIDebugInfo->m_pcsAgcmRender->m_fShadowTickDiff);
	//m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(800, 210, szTemp, 0 );

	if(m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetSelfCharacter() && m_pcsAgcmUIDebugInfo->m_pcsAgcmShadow2)
	{
		BOOL	isShadow = m_pcsAgcmUIDebugInfo->m_pcsAgcmShadow2->GetShadowBuffer((RwV3d*)&m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos);
		sprintf(szTemp,"POS: %.3f , %.3f , %.3f , %d", m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.x,
		m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.y,
		m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos.z,isShadow);
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(700.f, 350.f, szTemp, 0 );
	}

	if(m_pcsAgcmUIDebugInfo->m_pcsAgcmGrass)
	{
		wsprintf( szTemp , "Grass Draw: %d", m_pcsAgcmUIDebugInfo->m_pcsAgcmGrass->m_iDrawGrassCount + m_pcsAgcmUIDebugInfo->m_pcsAgcmGrass->m_iDrawGrassCountNear );
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(900.f, 380.f, szTemp, 0 ,230);
	}

	if(m_pcsAgcmUIDebugInfo->m_pcsAgcmEff2)
	{
		wsprintf( szTemp , "Effect Count : %d" , m_pcsAgcmUIDebugInfo->m_pcsAgcmEff2->GetCtrlMng()->GetCurrCnt() );
		m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D( 870.0f , 410.0f , szTemp , 0 );
	}

	RwTexDictionary *	pstTexDictionary = RwTexDictionaryGetCurrent();
	if (pstTexDictionary)
	{
		RwTexDictionaryForAllTextures(RwTexDictionaryGetCurrent(), CBTextureCount, &lCount);
		RwTexDictionaryForAllTextures(RwTexDictionaryGetCurrent(), CBTextureRefCount, &lCount2);
	}

	lCount = lCount2 = 0;

	sprintf(szTemp, " Texture Count : %d", lCount);
	m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 255.f, szTemp, 0, 255);
	sprintf(szTemp, " Texture Ref Count : %d", lCount2);
	m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 270.f, szTemp, 0, 255);
}

VOID	AgcDebugWindow::RenderNetwork()
{
	CHAR	szTemp[128];

	sprintf(szTemp, "Receive Queue : %d", g_AcReceiveQueue.GetCount());
	m_pcsAgcmUIDebugInfo->m_pcsAgcmFont->DrawTextIM2D(100.f, 100.f, szTemp, 0, 255);

	if (g_pEngine)
	{
		//sprintf(szTemp, "GameServer : 
	}
}

#ifdef _PROFILE_
VOID	AgcDebugWindow::FindMaxItem(CProfileNode* node, CProfileNode**		setnode)
{
	if(node->Child)
	{
		FindMaxItem(node->Child,setnode);
	}
	
	if(node->Sibling)
	{
		FindMaxItem(node->Sibling,setnode);
	}

	if(node->TotalTime > (*setnode)->TotalTime)
	{
		(*setnode) = node;
	}
}
#endif // _PROFILE_


RpMaterial *	CBMaterialFilter(RpMaterial *pstMaterial, PVOID pvData)
{
	if (RpMaterialGetTexture(pstMaterial))
		RwTextureSetFilterMode(RpMaterialGetTexture(pstMaterial), (RwTextureFilterMode) (INT32) pvData);

	return pstMaterial;
}

RpGeometry *	CBGeometryFilter(RpGeometry *pstGeometry, PVOID pvData)
{
	RpGeometryForAllMaterials(pstGeometry, CBMaterialFilter, pvData);

	return pstGeometry;
}

RpAtomic *		CBAtomicFilter(RpAtomic *pstAtomic, PVOID pvData)
{
	CBGeometryFilter(RpAtomicGetGeometry(pstAtomic), pvData);

	RpLODAtomicForAllLODGeometries(pstAtomic, CBGeometryFilter, pvData);

	return pstAtomic;
}

VOID	AgcDebugWindow::ChangeTextureFilterMode()
{
	INT32	lIndex;

	AgpdCharacterTemplate *	pcsCharacter;
	AgcdCharacterTemplate *	pstCharacter;
	AgpdItemTemplate *		pcsItem;
	AgcdItemTemplate *		pstItem;
	ApdObjectTemplate *		pcsObject;
	AgcdObjectTemplate *	pstObject;

	lIndex = 0;
	for (pcsCharacter = m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsCharacter; pcsCharacter = m_pcsAgcmUIDebugInfo->m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		pstCharacter = m_pcsAgcmUIDebugInfo->m_pcsAgcmCharacter->GetTemplateData(pcsCharacter);
		if (pstCharacter->m_pClump)
			RpClumpForAllAtomics(pstCharacter->m_pClump, CBAtomicFilter, (PVOID) m_lFilterMode);

		if (pstCharacter->m_pDefaultArmourClump)
			RpClumpForAllAtomics(pstCharacter->m_pDefaultArmourClump, CBAtomicFilter, (PVOID) m_lFilterMode);
	}

	lIndex = 0;
	for(AgpaItemTemplate::iterator it = m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->csTemplateAdmin.begin(); it != m_pcsAgcmUIDebugInfo->m_pcsAgpmItem->csTemplateAdmin.end(); ++it)
	{
		pcsItem = it->second;
		pstItem = m_pcsAgcmUIDebugInfo->m_pcsAgcmItem->GetTemplateData(pcsItem);
		if (pstItem->m_pstBaseClump)
			RpClumpForAllAtomics(pstItem->m_pstBaseClump, CBAtomicFilter, (PVOID) m_lFilterMode);

		if (pstItem->m_pstFieldClump)
			RpClumpForAllAtomics(pstItem->m_pstFieldClump, CBAtomicFilter, (PVOID) m_lFilterMode);

		if (pstItem->m_pstSecondClump)
			RpClumpForAllAtomics(pstItem->m_pstSecondClump, CBAtomicFilter, (PVOID) m_lFilterMode);
	}

	lIndex = 0;
	for (pcsObject = m_pcsAgcmUIDebugInfo->m_pcsApmObject->GetObjectTemplateSequence(&lIndex); pcsObject; pcsObject = m_pcsAgcmUIDebugInfo->m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
	{
		pstObject = m_pcsAgcmUIDebugInfo->m_pcsAgcmObject->GetTemplateData(pcsObject);
		if (pstObject->m_stGroup.m_pstList->m_csData.m_pstClump)
			RpClumpForAllAtomics(pstObject->m_stGroup.m_pstList->m_csData.m_pstClump, CBAtomicFilter, (PVOID) m_lFilterMode);
	}

	++m_lFilterMode;
	if (m_lFilterMode > rwFILTERLINEARMIPLINEAR)
		m_lFilterMode = 0;
}
