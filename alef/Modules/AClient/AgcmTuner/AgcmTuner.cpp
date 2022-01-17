#include "AgcmTuner.h"
#include "AgcEngine.h"
#include "AgcmTextBoardMng.h"

AgcmTuner::AgcmTuner()
{
	SetModuleName("AgcmTuner");
	EnableIdle(TRUE);

	m_bStart = false;
	m_uiLastTick = 0;
	m_uiCurTickDiff = 0;
	
	m_iHighFrame = 0;
	m_iLowFrame = 0;
	
	m_iMapLoadMin = 0;
	m_iMapLoadMax = 0;
	m_iMapRoughMin = 0;
	m_iMapRoughMax = 0;
	m_iMapDetailMin = 0;
	m_iMapDetailMax = 0;
	
	m_fMapLoadRange = 0.0f;
	m_fMapRoughRange = 0.0f;
	m_fMapDetailRange = 0.0f;
	
	ZeroMemory(m_uiElapsedTime,sizeof(m_uiElapsedTime));
	ZeroMemory(m_uiUpdateTime,sizeof(m_uiUpdateTime));
	ZeroMemory(m_bUpdate,sizeof(m_bUpdate));
	ZeroMemory(m_uiTickDiff,sizeof(m_uiTickDiff));
	
	m_uiLODElapsedTime = 0;
	m_uiLODLastFramePerSec = 0;

	m_bAutoTuning = FALSE;
	m_uiLODRangeType = 0;
	m_uiMapLoadRangeType = 0;
	m_uiShadowRangeType = 0;

	ZeroMemory(m_fLODRangeTable,sizeof(m_fLODRangeTable));
	ZeroMemory(m_fMapLoadRangeTable,sizeof(m_fMapLoadRangeTable));
	ZeroMemory(m_fShadowRangeTable,sizeof(m_fShadowRangeTable));

	
	m_pcmRender = NULL;
	m_pcmMap = NULL;
	m_pcmLODManager = NULL;
	m_pcmFont = NULL;
	m_pcmWater = NULL;
	m_pcmLensFlare = NULL;
	m_pcmShadow = NULL;
	m_pcmTBoard = NULL;
}

AgcmTuner::~AgcmTuner()
{
	
}

BOOL AgcmTuner::OnAddModule()
{
	m_pcmLODManager = (AgcmLODManager*)GetModule("AgcmLODManager");
//@{ kday 20050113
//	m_pcmCamera = (AgcmCamera*)GetModule("AgcmCamera");
//@} kday
	m_pcmFont = (AgcmFont*)GetModule("AgcmFont");
	m_pcmLensFlare = (AgcmLensFlare*)GetModule("AgcmLensFlare");
	m_pcmShadow = (AgcmShadow*)GetModule("AgcmShadow");
	m_pcmTBoard = (AgcmTextBoardMng*)GetModule("AgcmTextBoardMng");
	m_pcmWater = (AgcmWater*)GetModule("AgcmWater");
	m_pcmRender = (AgcmRender*)GetModule("AgcmRender");
	m_pcmMap = (AgcmMap*)GetModule("AgcmMap");

	m_fMapLoadRange = 10.0f;
	m_fMapRoughRange = 10.0f;
	m_fMapDetailRange = 5.0f;

	return TRUE;
}

BOOL AgcmTuner::OnInit()
{
	for(int i=0;i<TUNER_HANDLE_MODULE_COUNT;++i)
	{
		m_uiElapsedTime[i] = 0;
		m_bUpdate[i] = true;
		m_uiTickDiff[i] = 0;
	}

	// 갱신 주기 
	m_uiUpdateTime[0] = 100;	// AgcmFont
	m_uiUpdateTime[1] = 40;	// LensFlare

	m_bAutoTuning = false;

	m_uiLODRangeType = 2;
	m_uiMapLoadRangeType = 2;
	m_uiShadowRangeType = 1;

	m_fLODRangeTable[0] = 0.5f;
	m_fLODRangeTable[1] = 1.0f;
	m_fLODRangeTable[2] = 2.0f;
	m_fLODRangeTable[3] = 99.0f;

	m_fMapLoadRangeTable[0] = 2.0f;
	m_fMapLoadRangeTable[1] = 5.0f;
	m_fMapLoadRangeTable[2] = 7.0f;
	m_fMapLoadRangeTable[3] = 10.0f;

	m_fShadowRangeTable[0] = 1.0f;
	m_fShadowRangeTable[1] = 0.5f;
	m_fShadowRangeTable[2] = 0.0f;

	return TRUE;
}

BOOL AgcmTuner::OnDestroy()
{

	return TRUE;
}

BOOL AgcmTuner::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmTuner::OnIdle");
	UINT32		tickdiff;
	
	if(!m_bStart)
	{
		m_bStart = true;
		m_uiLastTick = ulClockCount;
		return TRUE;
	}
	else
	{
		m_uiCurTickDiff = tickdiff = ulClockCount - m_uiLastTick;
		m_uiLastTick = ulClockCount;
	}

	for(int i=0;i<TUNER_HANDLE_MODULE_COUNT;++i)
	{
		m_uiElapsedTime[i] += tickdiff;

		if(m_uiElapsedTime[i] > m_uiUpdateTime[i]) 
		{
			m_uiTickDiff[i] = m_uiElapsedTime[i];
			m_uiElapsedTime[i] = 0;
			
			// 0 - font, 1 - lensflare
			if(i == 0 && m_pcmFont)
			{
				m_pcmFont->m_bUpdateThisFrame = true;
			}
			else if(i == 1 && m_pcmLensFlare)
			{
				m_pcmLensFlare->m_bUpdateThisFrame = true;
			}
		}
	}

	//m_pcmRender->m_fRenderTotalTick = 0.0f;
	//m_pcmRender->m_fMapTickDiff = 0.0f;	
	//m_pcmRender->m_fShadowTickDiff = 0.0f;		
	//m_pcmRender->m_fCharacterTickDiff = 0.0f;
	//m_pcmRender->m_fObjectTickDiff = 0.0f;

	return TRUE;		// 기능 잠시 껐습니다. by gemani 2004.3.17
	
/*	if(m_bAutoTuning)
	{
		m_uiLODElapsedTime	+=	tickdiff;
		if(m_uiLODElapsedTime > TUNER_LOD_CHANGE_TIME)			// Detail 변경
		{
			m_uiLODElapsedTime = 0;

			AgcEngine	* pEngine = ( AgcEngine * ) GetModuleManager();
			UINT32	fps = pEngine->GetFps();

			if(fps < m_iLowFrame)
			{
				if(m_pcmRender->m_bActiveClass)
				{
					float perCharacter = m_pcmRender->m_fCharacterTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perObject = m_pcmRender->m_fObjectTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perMap = m_pcmRender->m_fMapTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perShadow = m_pcmRender->m_fShadowTickDiff / m_pcmRender->m_fRenderTotalTick;

					m_pcmLODManager->m_fLODDistFactor[0] -= 0.001f * perObject;
					if(m_pcmLODManager->m_fLODDistFactor[0] < AGCMLOD_LOWER_DIST)
					{
						m_pcmLODManager->m_fLODDistFactor[0] = AGCMLOD_LOWER_DIST;

						m_pcmLODManager->m_uiLODTriCutLine[0] -= 50;
						if(m_pcmLODManager->m_uiLODTriCutLine[0] <  AGCMLOD_LOWER_TRINUM )
							m_pcmLODManager->m_uiLODTriCutLine[0] = AGCMLOD_LOWER_TRINUM;	// 다른 방법을 강구하자 ㅡㅡ;
					}

					m_pcmLODManager->m_fLODDistFactor[1] -= 0.001f * perCharacter;
					if(m_pcmLODManager->m_fLODDistFactor[1] < AGCMLOD_LOWER_DIST)
					{
						m_pcmLODManager->m_fLODDistFactor[1] = AGCMLOD_LOWER_DIST;

						m_pcmLODManager->m_uiLODTriCutLine[1] -= 100;
						if(m_pcmLODManager->m_uiLODTriCutLine[1] <  AGCMLOD_LOWER_TRINUM )
							m_pcmLODManager->m_uiLODTriCutLine[1] = AGCMLOD_LOWER_TRINUM;	// 다른 방법을 강구하자 ㅡㅡ;
					}

					m_fMapLoadRange -=  0.01f * perMap;
					if(m_fMapLoadRange < (float)m_iMapLoadMin)	m_fMapLoadRange = (float)m_iMapLoadMin;
					m_fMapRoughRange -=  0.01f * perMap;
					if(m_fMapRoughRange < (float)m_iMapRoughMin)	m_fMapRoughRange = (float)m_iMapRoughMin;
					m_fMapDetailRange -=  0.01f * perMap;
					if(m_fMapDetailRange < (float)m_iMapDetailMin)	m_fMapDetailRange = (float)m_iMapDetailMin;

					//m_pcmMap->SetAutoLoadRange(m_fMapLoadRange,m_fMapRoughRange,m_fMapDetailRange);

					if(m_pcmShadow)
					{
						m_pcmShadow->m_fShadowDistFactor -= 0.001 * perShadow;
						if(m_pcmShadow->m_fShadowDistFactor < AGCMLOD_LOWER_DIST)
						{
							m_pcmShadow->m_fShadowDistFactor = AGCMLOD_LOWER_DIST;
						}
					}
				}
			}
			else if(fps > m_iHighFrame)
			{
				if(m_pcmRender->m_bActiveClass)
				{
					float perCharacter = m_pcmRender->m_fCharacterTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perObject = m_pcmRender->m_fObjectTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perMap = m_pcmRender->m_fMapTickDiff / m_pcmRender->m_fRenderTotalTick;
					float perShadow = m_pcmRender->m_fShadowTickDiff / m_pcmRender->m_fRenderTotalTick;

					m_pcmLODManager->m_fLODDistFactor[0] += 0.001f * perObject;
					if(m_pcmLODManager->m_fLODDistFactor[0] > 1.0f) 
					{
						m_pcmLODManager->m_fLODDistFactor[0] = 1.0f;
						m_pcmLODManager->m_uiLODTriCutLine[0] += 50;
						if(m_pcmLODManager->m_uiLODTriCutLine[0] >  AGCMLOD_MAX_TRINUM )
							m_pcmLODManager->m_uiLODTriCutLine[0] = AGCMLOD_MAX_TRINUM;	
					}

					m_pcmLODManager->m_fLODDistFactor[1] += 0.001f * perCharacter;
					if(m_pcmLODManager->m_fLODDistFactor[1] > 1.0f) 
					{
						m_pcmLODManager->m_fLODDistFactor[1] = 1.0f;
						m_pcmLODManager->m_uiLODTriCutLine[1] += 50;
						if(m_pcmLODManager->m_uiLODTriCutLine[1] >  AGCMLOD_MAX_TRINUM )
							m_pcmLODManager->m_uiLODTriCutLine[1] = AGCMLOD_MAX_TRINUM;	
					}

					//m_fMapLoadRange += (m_fMapLoadRange * 0.1f * perMap);
					//m_fMapRoughRange += (m_fMapRoughRange * 0.1f * perMap);
					//m_fMapDetailRange += (m_fMapDetailRange * 0.1f * perMap);

					m_fMapLoadRange += 0.01f * perMap;
					if(m_fMapLoadRange > (float)m_iMapLoadMax)	m_fMapLoadRange = (float)m_iMapLoadMax;
					m_fMapRoughRange += 0.01f * perMap;
					if(m_fMapRoughRange > (float)m_iMapRoughMax)	m_fMapRoughRange = (float)m_iMapRoughMax;
					m_fMapDetailRange += 0.01f * perMap;
					if(m_fMapDetailRange > (float)m_iMapDetailMax)	m_fMapDetailRange = (float)m_iMapDetailMax;
					
					//m_pcmMap->SetAutoLoadRange(m_fMapLoadRange,m_fMapRoughRange,m_fMapDetailRange);
					
					if(m_pcmShadow)
					{
						m_pcmShadow->m_fShadowDistFactor += 0.001 * perShadow;
						if(m_pcmShadow->m_fShadowDistFactor > 1.0f)
						{
							m_pcmShadow->m_fShadowDistFactor = 1.0f;
						}
					}
				}
			}

			m_uiLODLastFramePerSec = fps;
		}
	}*/

	return TRUE;
}

void	AgcmTuner::SetVariableByOption(bool bAuto,UINT32 LODType,UINT32 MapLODType,UINT32 ShadowType)
{
	m_bAutoTuning = bAuto;
	m_uiLODRangeType = LODType;
	m_uiMapLoadRangeType = MapLODType;
	m_uiShadowRangeType = ShadowType;

	if(!bAuto)
	{
		if(m_pcmLODManager)
		{
			m_pcmLODManager->m_fLODDistFactor[0] = m_fLODRangeTable[m_uiLODRangeType];
			m_pcmLODManager->m_fLODDistFactor[1] = m_fLODRangeTable[m_uiLODRangeType];
		}
		
		m_fMapDetailRange = m_fMapLoadRangeTable[m_uiMapLoadRangeType];
		if(m_pcmMap)
			m_pcmMap->SetAutoLoadRange( ( INT32 ) m_fMapLoadRange, ( INT32 ) m_fMapRoughRange, ( INT32 ) m_fMapDetailRange);

		if(m_pcmShadow)
			m_pcmShadow->m_fShadowDistFactor = m_fShadowRangeTable[m_uiShadowRangeType];
	}
}