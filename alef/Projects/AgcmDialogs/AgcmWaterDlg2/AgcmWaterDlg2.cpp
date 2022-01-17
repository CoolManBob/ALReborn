// AgcmWaterDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmWaterDlg2.h"
#include "WaterDlg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmWaterDlg2::AgcmWaterDlg2()
{
	SetModuleName("AgcmWaterDlg2");

	m_pcmWater = NULL;
}

AgcmWaterDlg2::~AgcmWaterDlg2()
{
}

BOOL AgcmWaterDlg2::OnAddModule()
{
	m_pcmWater = (AgcmWater* ) GetModule( "AgcmWater" );
	return TRUE;
}

BOOL AgcmWaterDlg2::OnInit()
{
	return TRUE;
}

BOOL AgcmWaterDlg2::OnDestroy()
{
	return TRUE;
}

BOOL AgcmWaterDlg2::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

int  AgcmWaterDlg2::OpenWaterDlg2(int status_ID)
{
	int			file_limit = WMAP_FILE_NUM;

	WaterDlg2 dlg;

	dlg.m_iHwID = status_ID;
	dlg.m_fHeight0 = m_pcmWater->m_stHWaterStatus[status_ID].Height0;
	dlg.m_fHeight1 = m_pcmWater->m_stHWaterStatus[status_ID].Height1;
	dlg.m_fHeight2 = m_pcmWater->m_stHWaterStatus[status_ID].Height2;
	dlg.m_fHeight3 = m_pcmWater->m_stHWaterStatus[status_ID].Height3;
	dlg.m_fSpeed0 = m_pcmWater->m_stHWaterStatus[status_ID].Speed0;
	dlg.m_fSpeed1 = m_pcmWater->m_stHWaterStatus[status_ID].Speed1;
	dlg.m_fSpeed2 = m_pcmWater->m_stHWaterStatus[status_ID].Speed2;
	dlg.m_fSpeed3 = m_pcmWater->m_stHWaterStatus[status_ID].Speed3;
	dlg.m_fDirX0 = m_pcmWater->m_stHWaterStatus[status_ID].DirX0;
	dlg.m_fDirX1 = m_pcmWater->m_stHWaterStatus[status_ID].DirX1;
	dlg.m_fDirX2 = m_pcmWater->m_stHWaterStatus[status_ID].DirX2;
	dlg.m_fDirX3 = m_pcmWater->m_stHWaterStatus[status_ID].DirX3;
	dlg.m_fDirY0 = m_pcmWater->m_stHWaterStatus[status_ID].DirY0;
	dlg.m_fDirY1 = m_pcmWater->m_stHWaterStatus[status_ID].DirY1;
	dlg.m_fDirY2 = m_pcmWater->m_stHWaterStatus[status_ID].DirY2;
	dlg.m_fDirY3 = m_pcmWater->m_stHWaterStatus[status_ID].DirY3;
	dlg.m_fTexX0 = m_pcmWater->m_stHWaterStatus[status_ID].TexX0;
	dlg.m_fTexX1 = m_pcmWater->m_stHWaterStatus[status_ID].TexX1;
	dlg.m_fTexY0 = m_pcmWater->m_stHWaterStatus[status_ID].TexY0;
	dlg.m_fTexY1 = m_pcmWater->m_stHWaterStatus[status_ID].TexY1;
	dlg.m_fRed = m_pcmWater->m_stHWaterStatus[status_ID].Red;
	dlg.m_fGreen = m_pcmWater->m_stHWaterStatus[status_ID].Green;
	dlg.m_fBlue = m_pcmWater->m_stHWaterStatus[status_ID].Blue;
	dlg.m_fReflectionWeight = m_pcmWater->m_stHWaterStatus[status_ID].ReflectionWeight;
	
	dlg.m_strBump = m_pcmWater->m_stHQBumpFInfo[m_pcmWater->m_stHWaterStatus[status_ID].BumpTexID].strFileName;
	dlg.m_strFresnel = m_pcmWater->m_stHQFresnelFInfo[m_pcmWater->m_stHWaterStatus[status_ID].FresnelTexID].strFileName;

	dlg.m_fAlphaMin = m_pcmWater->m_stHWaterStatus[status_ID].Alpha_Min;
	dlg.m_fAlphaMax = m_pcmWater->m_stHWaterStatus[status_ID].Alpha_Max;
	dlg.m_fAlphaDecHeight = m_pcmWater->m_stHWaterStatus[status_ID].Alpha_DecreaseHeight;

	dlg.m_fWaveMin = m_pcmWater->m_stHWaterStatus[status_ID].Height_Min;
	dlg.m_fWaveMax = m_pcmWater->m_stHWaterStatus[status_ID].Height_Max;
	dlg.m_fWaveDecHeight = m_pcmWater->m_stHWaterStatus[status_ID].Height_DecreaseHeight;
	
	dlg.m_iVertexPerTile = m_pcmWater->m_stHWaterStatus[status_ID].VertexPerTile;
	
	//@{ Jaewon 20050706
	// ;)
	dlg.m_fBaseR = m_pcmWater->m_stHWaterStatus[status_ID].BaseR;
	dlg.m_fBaseG = m_pcmWater->m_stHWaterStatus[status_ID].BaseG;
	dlg.m_fBaseB = m_pcmWater->m_stHWaterStatus[status_ID].BaseB;
	dlg.m_iShader14ID = m_pcmWater->m_stHWaterStatus[status_ID].Shader14ID;
	dlg.m_iShader11ID = m_pcmWater->m_stHWaterStatus[status_ID].Shader11ID;
	//@} Jaewon

	int i;
	if(IDOK == dlg.DoModal())
	{
		if(strlen(dlg.m_strBump) >= 32 ||
			strlen(dlg.m_strFresnel) >= 32)
		{
			MessageBox(NULL,"파일길이 를 초과하였습니다(32)"," 경고",MB_OK);
			return 0;
		}

		int i, j;

		if(dlg.m_bDataChange)
		{
			int bump_TID = -1;
			int fresnel_TID = -1;
			bool	bfind = false;

			// BumpMap 파일 검사하자
			for(i=0;i<file_limit;++i)
			{
				if(!strcmp(m_pcmWater->m_stHQBumpFInfo[i].strFileName,dlg.m_strBump))
				{
					bump_TID = i;
					bfind = true;
					break;
				}
			}

			if(bfind == false)
			{
				// none 을 찾아보자..
				BOOL	bFindNone = FALSE;
				for(j=0;j<WMAP_FILE_NUM;++j)
				{
					if(!strcmp(m_pcmWater->m_stHQBumpFInfo[j].strFileName,"none"))
					{
						bump_TID = j;
						m_pcmWater->m_stHQBumpFInfo[j].bReload = TRUE;
						bFindNone = TRUE;

						strcpy(m_pcmWater->m_stHQBumpFInfo[j].strFileName,dlg.m_strBump);

						break;
					}
				}
				
				if(!bFindNone)
				{
					BOOL	bFindInStatus = FALSE;

					for(j=0;j<WMAP_STATUS_NUM;++j)
					{
						if(status_ID != j)
						{
							if(m_pcmWater->m_stHWaterStatus[status_ID].BumpTexID == m_pcmWater->m_stHWaterStatus[j].BumpTexID)
							{
								bFindInStatus = TRUE;
								break;
							}
						}
					}

					if(bFindInStatus)		// 새로 만듬 
					{
						if(m_pcmWater->m_iHQBumpFileNum == file_limit)
						{
							MessageBox(NULL,"BumpMap 파일수를 초과하였습니다"," 경고",MB_OK);
							return 0;
						}
						bump_TID = m_pcmWater->m_iHQBumpFileNum++;
					}
					else
					{
						bump_TID = m_pcmWater->m_stHWaterStatus[status_ID].BumpTexID;
					}

					m_pcmWater->m_stHQBumpFInfo[bump_TID].bReload = TRUE;
				}
			}

			bfind  = false;
			// FresnelMap 파일 검사하자
			for(i=0;i<file_limit;++i)
			{
				if(!strcmp(m_pcmWater->m_stHQFresnelFInfo[i].strFileName,dlg.m_strFresnel))
				{
					fresnel_TID = i;
					bfind = true;
					break;
				}
			}

			if(bfind == false)
			{
				// none 을 찾아보자..
				BOOL	bFindNone = FALSE;
				for(j=0;j<WMAP_FILE_NUM;++j)
				{
					if(!strcmp(m_pcmWater->m_stHQFresnelFInfo[j].strFileName,"none"))
					{
						fresnel_TID = j;
						m_pcmWater->m_stHQFresnelFInfo[j].bReload = TRUE;
						bFindNone = TRUE;

						strcpy(m_pcmWater->m_stHQFresnelFInfo[j].strFileName,dlg.m_strFresnel);

						break;
					}
				}

				if(!bFindNone)
				{
					BOOL	bFindInStatus = FALSE;

					for(int j=0;j<WMAP_STATUS_NUM;++j)
					{
						if(status_ID != j)
						{
							if(m_pcmWater->m_stHWaterStatus[status_ID].FresnelTexID == m_pcmWater->m_stHWaterStatus[j].FresnelTexID)
							{
								bFindInStatus = TRUE;
								break;
							}
						}
					}

					if(bFindInStatus)		// 새로 만듬 
					{
						if(m_pcmWater->m_iHQFresnelFileNum == file_limit)
						{
							MessageBox(NULL,"FresnelMap 파일수를 초과하였습니다"," 경고",MB_OK);
							return 0;
						}
						fresnel_TID = m_pcmWater->m_iHQFresnelFileNum++;
					}
					else
					{
						fresnel_TID = m_pcmWater->m_stHWaterStatus[status_ID].FresnelTexID;
					}

					m_pcmWater->m_stHQFresnelFInfo[fresnel_TID].bReload = TRUE;
				}
			}
			
			// data set
			m_pcmWater->m_stHWaterStatus[status_ID].Height0 = dlg.m_fHeight0;
			m_pcmWater->m_stHWaterStatus[status_ID].Height1 = dlg.m_fHeight1;
			m_pcmWater->m_stHWaterStatus[status_ID].Height2 = dlg.m_fHeight2;
			m_pcmWater->m_stHWaterStatus[status_ID].Height3 = dlg.m_fHeight3;
			m_pcmWater->m_stHWaterStatus[status_ID].Speed0 = dlg.m_fSpeed0;
			m_pcmWater->m_stHWaterStatus[status_ID].Speed1 = dlg.m_fSpeed1;
			m_pcmWater->m_stHWaterStatus[status_ID].Speed2 = dlg.m_fSpeed2;
			m_pcmWater->m_stHWaterStatus[status_ID].Speed3 = dlg.m_fSpeed3;
			m_pcmWater->m_stHWaterStatus[status_ID].DirX0 = dlg.m_fDirX0;
			m_pcmWater->m_stHWaterStatus[status_ID].DirX1 = dlg.m_fDirX1;
			m_pcmWater->m_stHWaterStatus[status_ID].DirX2 = dlg.m_fDirX2;
			m_pcmWater->m_stHWaterStatus[status_ID].DirX3 = dlg.m_fDirX3;
			m_pcmWater->m_stHWaterStatus[status_ID].DirY0 = dlg.m_fDirY0;
			m_pcmWater->m_stHWaterStatus[status_ID].DirY1 = dlg.m_fDirY1;
			m_pcmWater->m_stHWaterStatus[status_ID].DirY2 = dlg.m_fDirY2;
			m_pcmWater->m_stHWaterStatus[status_ID].DirY3 = dlg.m_fDirY3;
			m_pcmWater->m_stHWaterStatus[status_ID].TexX0 = dlg.m_fTexX0;
			m_pcmWater->m_stHWaterStatus[status_ID].TexX1 = dlg.m_fTexX1;
			m_pcmWater->m_stHWaterStatus[status_ID].TexY0 = dlg.m_fTexY0;
			m_pcmWater->m_stHWaterStatus[status_ID].TexY1 = dlg.m_fTexY1;
			m_pcmWater->m_stHWaterStatus[status_ID].Red = dlg.m_fRed;
			m_pcmWater->m_stHWaterStatus[status_ID].Green = dlg.m_fGreen;
			m_pcmWater->m_stHWaterStatus[status_ID].Blue = dlg.m_fBlue;
			
			m_pcmWater->m_stHWaterStatus[status_ID].ReflectionWeight = dlg.m_fReflectionWeight;
			
			m_pcmWater->m_stHWaterStatus[status_ID].BumpTexID = bump_TID;
			strcpy(m_pcmWater->m_stHQBumpFInfo[bump_TID].strFileName,dlg.m_strBump);

			m_pcmWater->m_stHWaterStatus[status_ID].FresnelTexID = fresnel_TID;
			strcpy(m_pcmWater->m_stHQFresnelFInfo[fresnel_TID].strFileName,dlg.m_strFresnel);

			m_pcmWater->m_stHWaterStatus[status_ID].Alpha_Min = dlg.m_fAlphaMin;
			m_pcmWater->m_stHWaterStatus[status_ID].Alpha_Max = dlg.m_fAlphaMax;
			m_pcmWater->m_stHWaterStatus[status_ID].Alpha_DecreaseHeight = dlg.m_fAlphaDecHeight;

			m_pcmWater->m_stHWaterStatus[status_ID].Height_Min = dlg.m_fWaveMin;
			m_pcmWater->m_stHWaterStatus[status_ID].Height_Max = dlg.m_fWaveMax;
			m_pcmWater->m_stHWaterStatus[status_ID].Height_DecreaseHeight = dlg.m_fWaveDecHeight;

			m_pcmWater->m_stHWaterStatus[status_ID].VertexPerTile = dlg.m_iVertexPerTile;

			//@{ Jaewon 20050706
			// ;)
			m_pcmWater->m_stHWaterStatus[status_ID].BaseR = dlg.m_fBaseR;
			m_pcmWater->m_stHWaterStatus[status_ID].BaseG = dlg.m_fBaseG;
			m_pcmWater->m_stHWaterStatus[status_ID].BaseB = dlg.m_fBaseB;
			m_pcmWater->m_stHWaterStatus[status_ID].Shader14ID = dlg.m_iShader14ID;
			m_pcmWater->m_stHWaterStatus[status_ID].Shader11ID = dlg.m_iShader11ID;
			//@} Jaewon
			
			return 1;
		}		
	}

	return 0;
}

