// AgcmWaterDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmWaterDlg.h"
#include "WaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


AgcmWaterDlg::AgcmWaterDlg()
{
	SetModuleName("AgcmWaterDlg");
}

AgcmWaterDlg::~AgcmWaterDlg()
{
}

BOOL AgcmWaterDlg::OnAddModule()
{
	m_pcmWater = (AgcmWater* ) GetModule( "AgcmWater" );
	return TRUE;
}

BOOL AgcmWaterDlg::OnInit()
{
	return TRUE;
}

BOOL AgcmWaterDlg::OnDestroy()
{
	return TRUE;
}

BOOL AgcmWaterDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

int AgcmWaterDlg::OpenWaterDlg(int status_ID)
{
	int			file_limit = WMAP_FILE_NUM;

	CWaterDlg dlg;

	dlg.m_iStatusID = status_ID;
	dlg.m_iWaterType = m_pcmWater->m_stWaterStatus[status_ID].WaterType;

	dlg.m_iRed_L1 = m_pcmWater->m_stWaterStatus[status_ID].Red_L1;
	dlg.m_iGreen_L1 = m_pcmWater->m_stWaterStatus[status_ID].Green_L1;
	dlg.m_iBlue_L1 = m_pcmWater->m_stWaterStatus[status_ID].Blue_L1;
	dlg.m_iAlpha_L1 = m_pcmWater->m_stWaterStatus[status_ID].Alpha_L1;
	dlg.m_iSelBMode_L1 = m_pcmWater->m_stWaterStatus[status_ID].BMode_L1;
	dlg.m_iSelBOP_L1 = m_pcmWater->m_stWaterStatus[status_ID].BOP_L1;
	dlg.m_fU_L1 = m_pcmWater->m_stWaterStatus[status_ID].U_L1;
	dlg.m_fV_L1 = m_pcmWater->m_stWaterStatus[status_ID].V_L1;
	dlg.m_iTileSize_L1 = m_pcmWater->m_stWaterStatus[status_ID].TileSize_L1;
	dlg.m_strWaterFile_L1 = m_pcmWater->m_stWaterFInfo[m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L1].strFileName;

	dlg.m_iSelBMode_L2 = m_pcmWater->m_stWaterStatus[status_ID].BMode_L2;
	dlg.m_fU_L2 = m_pcmWater->m_stWaterStatus[status_ID].U_L2;
	dlg.m_fV_L2 = m_pcmWater->m_stWaterStatus[status_ID].V_L2;
	dlg.m_strWaterFile_L2 = m_pcmWater->m_stWaterFInfo[m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L2].strFileName;
	
	int wave_tid= m_pcmWater->m_stWaterStatus[status_ID].WaveTexID;

	dlg.m_strWaveFile = m_pcmWater->m_stWaveFInfo[wave_tid].strFileName;
	dlg.m_iWaveWidth = m_pcmWater->m_stWaterStatus[status_ID].WaveWidth;
	dlg.m_iWaveHeight = m_pcmWater->m_stWaterStatus[status_ID].WaveHeight;
	dlg.m_fWaveLifeTime = (float)m_pcmWater->m_stWaterStatus[status_ID].WaveLifeTime / 1000.0f;
	dlg.m_fWaveScaleX = m_pcmWater->m_stWaterStatus[status_ID].WaveScaleX;
	dlg.m_fWaveScaleZ = m_pcmWater->m_stWaterStatus[status_ID].WaveScaleZ;
	dlg.m_iWaveRed = m_pcmWater->m_stWaterStatus[status_ID].WaveRed;
	dlg.m_iWaveGreen = m_pcmWater->m_stWaterStatus[status_ID].WaveGreen;
	dlg.m_iWaveBlue = m_pcmWater->m_stWaterStatus[status_ID].WaveBlue;

	dlg.m_iWaveMinNum = m_pcmWater->m_stWaterStatus[status_ID].WaveMinNum;
	dlg.m_iWaveMaxNum = m_pcmWater->m_stWaterStatus[status_ID].WaveMaxNum;

	if(IDOK == dlg.DoModal())
	{
		if(strlen(dlg.m_strWaterFile_L1) >= 32 ||
			strlen(dlg.m_strWaterFile_L2) >= 32 ||
			strlen(dlg.m_strWaveFile) >= 32	)
		{
			MessageBox(NULL,"파일길이 를 초과하였습니다(32)"," 경고",MB_OK);
			return 0;
		}

		if(dlg.m_bDataChange)
		{
			int water_TID1 = -1;
			int water_TID2 = -1;
			int wave_TID = -1;
			bool	bfind = false;

			// Water 파일 검사하자
			int i;
			int j;

			for(i=0;i<file_limit;++i)
			{
				if(!strcmp(m_pcmWater->m_stWaterFInfo[i].strFileName,dlg.m_strWaterFile_L1))
				{
					water_TID1 = i;
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
					if(!strcmp(m_pcmWater->m_stWaterFInfo[j].strFileName,"none"))
					{
						water_TID1 = j;
						m_pcmWater->m_stWaterFInfo[j].bReload = TRUE;
						bFindNone = TRUE;

						strcpy(m_pcmWater->m_stWaterFInfo[j].strFileName,dlg.m_strWaterFile_L1);

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
							if(m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L1 == m_pcmWater->m_stWaterStatus[j].WaterTexID_L1)
							{
								bFindInStatus = TRUE;
								break;
							}
						}

						if(m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L1 == m_pcmWater->m_stWaterStatus[j].WaterTexID_L2)
						{
							bFindInStatus = TRUE;
							break;
						}
					}

					if(bFindInStatus)		// 새로 만듬 
					{
						if(m_pcmWater->m_iWaterFileNum == file_limit)
						{
							MessageBox(NULL,"Water파일수를 초과하였습니다"," 경고",MB_OK);
							return 0;
						}
						water_TID1 = m_pcmWater->m_iWaterFileNum++;
					}
					else
					{
						water_TID1 = m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L1;
					}

					m_pcmWater->m_stWaterFInfo[water_TID1].bReload = TRUE;
				}
			}

			bfind  = false;
			// Water 파일 검사하자
			for( i=0;i<file_limit;++i)
			{
				if(!strcmp(m_pcmWater->m_stWaterFInfo[i].strFileName,dlg.m_strWaterFile_L2))
				{
					water_TID2 = i;
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
					if(!strcmp(m_pcmWater->m_stWaterFInfo[j].strFileName,"none"))
					{
						water_TID2 = j;
						m_pcmWater->m_stWaterFInfo[j].bReload = TRUE;
						bFindNone = TRUE;

						strcpy(m_pcmWater->m_stWaterFInfo[j].strFileName,dlg.m_strWaterFile_L2);

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
							if(m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L2 == m_pcmWater->m_stWaterStatus[j].WaterTexID_L2)
							{
								bFindInStatus = TRUE;
								break;
							}
						}

						if(m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L2 == m_pcmWater->m_stWaterStatus[j].WaterTexID_L1)
						{
							bFindInStatus = TRUE;
							break;
						}
					}

					if(bFindInStatus)		// 새로 만듬 
					{
						if(m_pcmWater->m_iWaterFileNum == file_limit)
						{
							MessageBox(NULL,"Water파일수를 초과하였습니다"," 경고",MB_OK);
							return 0;
						}
						water_TID2 = m_pcmWater->m_iWaterFileNum++;
					}
					else
					{
						water_TID2 = m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L2;
					}

					m_pcmWater->m_stWaterFInfo[water_TID2].bReload = TRUE;
				}
			}

			bfind  = false;
			// Wave 파일 검사하자
			for(i = 0;i<file_limit;++i)
			{
				if(!strcmp(m_pcmWater->m_stWaveFInfo[i].strFileName,dlg.m_strWaveFile))
				{
					wave_TID = i;
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
					if(!strcmp(m_pcmWater->m_stWaveFInfo[j].strFileName,"none"))
					{
						wave_TID = j;
						m_pcmWater->m_stWaveFInfo[j].bReload = TRUE;
						bFindNone = TRUE;

						strcpy(m_pcmWater->m_stWaveFInfo[j].strFileName,dlg.m_strWaveFile);

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
							if(m_pcmWater->m_stWaterStatus[status_ID].WaveTexID == m_pcmWater->m_stWaterStatus[j].WaveTexID)
							{
								bFindInStatus = TRUE;
								break;
							}
						}
					}

					if(bFindInStatus)		// 새로 만듬 
					{
						if(m_pcmWater->m_iWaterFileNum == file_limit)
						{
							MessageBox(NULL,"Water파일수를 초과하였습니다"," 경고",MB_OK);
							return 0;
						}
						wave_TID = m_pcmWater->m_iWaveFileNum++;
					}
					else
					{
						wave_TID = m_pcmWater->m_stWaterStatus[status_ID].WaveTexID;
					}

					m_pcmWater->m_stWaveFInfo[wave_TID].bReload = TRUE;
				}
			}

			// data set
			m_pcmWater->m_stWaterStatus[status_ID].WaterType = dlg.m_iWaterType;

			m_pcmWater->m_stWaterStatus[status_ID].Red_L1 = dlg.m_iRed_L1;
			m_pcmWater->m_stWaterStatus[status_ID].Green_L1 = dlg.m_iGreen_L1;
			m_pcmWater->m_stWaterStatus[status_ID].Blue_L1 = dlg.m_iBlue_L1;
			m_pcmWater->m_stWaterStatus[status_ID].Alpha_L1 = dlg.m_iAlpha_L1;
			m_pcmWater->m_stWaterStatus[status_ID].BMode_L1 = dlg.m_iSelBMode_L1;
			m_pcmWater->m_stWaterStatus[status_ID].BOP_L1 = dlg.m_iSelBOP_L1;
			m_pcmWater->m_stWaterStatus[status_ID].RenderBMode_L1 = MapBMode(dlg.m_iSelBMode_L1,dlg.m_iSelBOP_L1);
			m_pcmWater->m_stWaterStatus[status_ID].TileSize_L1 = dlg.m_iTileSize_L1;
			m_pcmWater->m_stWaterStatus[status_ID].U_L1 = dlg.m_fU_L1;
			m_pcmWater->m_stWaterStatus[status_ID].V_L1 = dlg.m_fV_L1;
			m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L1 = water_TID1;
			strcpy(m_pcmWater->m_stWaterFInfo[water_TID1].strFileName,dlg.m_strWaterFile_L1);

			m_pcmWater->m_stWaterStatus[status_ID].BMode_L2 = dlg.m_iSelBMode_L2;
			m_pcmWater->m_stWaterStatus[status_ID].U_L2 = dlg.m_fU_L2;
			m_pcmWater->m_stWaterStatus[status_ID].V_L2 = dlg.m_fV_L2;
			m_pcmWater->m_stWaterStatus[status_ID].WaterTexID_L2 = water_TID2;
			strcpy(m_pcmWater->m_stWaterFInfo[water_TID2].strFileName,dlg.m_strWaterFile_L2);

			if(dlg.m_iSelBMode_L2 == 0 || dlg.m_iSelBMode_L2 == 3)
			{
				m_pcmWater->m_stWaterStatus[status_ID].BModeSrc_L2 = rwBLENDSRCALPHA;
				m_pcmWater->m_stWaterStatus[status_ID].BModeDest_L2 = rwBLENDINVSRCALPHA;
			}
			else if(dlg.m_iSelBMode_L2 == 1)
			{
				m_pcmWater->m_stWaterStatus[status_ID].BModeSrc_L2 = rwBLENDONE;
				m_pcmWater->m_stWaterStatus[status_ID].BModeDest_L2 = rwBLENDONE;
			}
			else if(dlg.m_iSelBMode_L2 == 2)
			{
				m_pcmWater->m_stWaterStatus[status_ID].BModeSrc_L2 = rwBLENDSRCALPHA;
				m_pcmWater->m_stWaterStatus[status_ID].BModeDest_L2 = rwBLENDONE;
			}
			else if(dlg.m_iSelBMode_L2 == 4)
			{
				m_pcmWater->m_stWaterStatus[status_ID].BModeSrc_L2 = rwBLENDSRCCOLOR;
				m_pcmWater->m_stWaterStatus[status_ID].BModeDest_L2 = rwBLENDINVSRCCOLOR;
			}
			
			strcpy(m_pcmWater->m_stWaveFInfo[wave_TID].strFileName,dlg.m_strWaveFile);
			m_pcmWater->m_stWaterStatus[status_ID].WaveTexID = wave_TID;
			m_pcmWater->m_stWaterStatus[status_ID].WaveWidth = dlg.m_iWaveWidth;
			m_pcmWater->m_stWaterStatus[status_ID].WaveHeight = dlg.m_iWaveHeight;
			m_pcmWater->m_stWaterStatus[status_ID].WaveLifeTime = dlg.m_fWaveLifeTime * 1000;
			m_pcmWater->m_stWaterStatus[status_ID].WaveScaleX = dlg.m_fWaveScaleX;
			m_pcmWater->m_stWaterStatus[status_ID].WaveScaleZ = dlg.m_fWaveScaleZ;
			m_pcmWater->m_stWaterStatus[status_ID].WaveRed = dlg.m_iWaveRed;
			m_pcmWater->m_stWaterStatus[status_ID].WaveGreen = dlg.m_iWaveGreen;
			m_pcmWater->m_stWaterStatus[status_ID].WaveBlue = dlg.m_iWaveBlue;
			m_pcmWater->m_stWaterStatus[status_ID].WaveMinNum = dlg.m_iWaveMinNum;
			m_pcmWater->m_stWaterStatus[status_ID].WaveMaxNum = dlg.m_iWaveMaxNum;
			
			return 1;
		}		
	}

	return 0;
}

int		AgcmWaterDlg::MapBMode(int BlendMode,int BOP)
{
	if(BlendMode == 0)
		return -1;
	else if(BlendMode == 4)
	{
		return 0 + BOP;
	}
	else if(BlendMode == 1)
	{
		return 3 + BOP;
	}
	else if(BlendMode == 5)
	{
		return 6 + BOP;
	}
	else if(BlendMode == 2)
	{
		return 9 + BOP;
	}
	else if(BlendMode == 3)
	{
		return 12 + BOP;
	}

	return -1;
}