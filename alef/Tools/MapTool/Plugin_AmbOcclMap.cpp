// -----------------------------------------------------------------------------
//  _____  _             _                                 _      ____            _ __  __                                  
// |  __ \| |           (_)                /\             | |    / __ \          | |  \/  |                                 
// | |__) | |_   _  __ _ _ _ __           /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __       ___ _ __  _ __  
// |  ___/| | | | |/ _` | | '_ \         / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \     / __| '_ \| '_ \ 
// | |    | | |_| | (_| | | | | |       / ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | (__| |_) | |_) |
// |_|    |_|\__,_|\__, |_|_| |_|      /_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_) \___| .__/| .__/ 
//                  __/ |        ______                                                         | |            | |   | |    
//                 |___/        |______|                                                        |_|            |_|   |_|    
//
// Plugin GUI for the ambient occlusion map
//
// -----------------------------------------------------------------------------
// Originally created on 01/13/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "stdafx.h"
#include "Plugin_AmbOcclMap.h"
#include "MainFrm.h"

#define IS_BUTTON_CLICKED(id, wParam) ((id)==LOWORD(wParam) && BN_CLICKED==HIWORD(wParam))
#define IS_COMBOBOX_SELCHANGED(id, wParam) ((id)==LOWORD(wParam) && CBN_SELCHANGE==HIWORD(wParam))

// -----------------------------------------------------------------------------
CPlugin_AmbOcclMap::CPlugin_AmbOcclMap()
				: created_(false)
{
	m_strShortName = "AOMap";
}

// -----------------------------------------------------------------------------
CPlugin_AmbOcclMap::~CPlugin_AmbOcclMap()
{
}

// -----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CPlugin_AmbOcclMap, CWnd)
	//{{AFX_MSG_MAP(CPlugin_AmbOcclMap)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------
int CPlugin_AmbOcclMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL result;

	result = createButton_.Create("Create...", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(75, 20, 225, 50), this, CTRLID_BUTTON_CREATE);
	ASSERT(result);

	result = clearButton_.Create("Clear", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(75, 60, 225, 90), this, CTRLID_BUTTON_CLEAR);
	ASSERT(result);
	clearButton_.EnableWindow(FALSE);

	result = computeButton_.Create("Compute...", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(75, 100, 225, 130), this, CTRLID_BUTTON_COMPUTE);
	ASSERT(result);
	computeButton_.EnableWindow(FALSE);

	result = saveButton_.Create("Save", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, CRect(75, 140, 225, 170), this, CTRLID_BUTTON_SAVE);
	ASSERT(result);
	saveButton_.EnableWindow(FALSE);

	result = technicComboBox_.Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, CRect(75, 190, 225, 310), this, CTRLID_COMBOBOX_TECHNIQUE);
	ASSERT(result);
	technicComboBox_.AddString("AmbientOcclusion");
	technicComboBox_.AddString("Simple");
	technicComboBox_.AddString("AmbOcclOnly");
	technicComboBox_.SetCurSel(0);

	result = objectProgress_.Create(WS_VISIBLE | WS_CHILD | PBS_SMOOTH, CRect(10, 240, 290, 270), this, CTRLID_PROGRESS_OBJECT);
	ASSERT(result);
	objectProgress_.SetRange(0, 100);
	objectProgress_.SetPos(0);

	result = sliceProgress_.Create(WS_VISIBLE | WS_CHILD | PBS_SMOOTH, CRect(10, 280, 290, 310), this, CTRLID_PROGRESS_SLICE);
	ASSERT(result);
	sliceProgress_.SetRange(0, 100);
	sliceProgress_.SetPos(0);

	return 0;
}

void CPlugin_AmbOcclMap::OnDestroy()
{
	CWnd::OnDestroy();
}

// A technique change callback for the "aom", ambient occlusion map command
static void* effectTechniqueCB(SharedD3dXEffectPtr *pEffect, void *data)
{
	int index = *(int*)data;

	for(int i=0; i<3; ++i)
	{
		D3DXHANDLE hDesc = pEffect->d3dxEffect[i]->GetParameterByName(NULL, "description");
		const char *desc = NULL;
		if(hDesc)
			pEffect->d3dxEffect[i]->GetString(hDesc, &desc);
		if(desc && strstr(desc, "Ambient Occlusion"))
		{
			D3DXHANDLE handle = pEffect->d3dxEffect[i]->GetTechnique(index);
			if(handle)
			{
				D3DXTECHNIQUE_DESC technicDesc;
				pEffect->d3dxEffect[i]->SetTechnique(handle);
				pEffect->d3dxEffect[i]->GetTechniqueDesc(handle, &technicDesc);
				DisplayMessage("Technique changed to \"%s\".", technicDesc.Name);
			}
		}
	}

	return pEffect;
}

LRESULT CPlugin_AmbOcclMap::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(WM_COMMAND == message)
	{
		if(IS_BUTTON_CLICKED(CTRLID_BUTTON_CREATE, wParam))
		{
			if(false == created_)
			// create
			{
				// Open a prefix dialog.
				//@{ Jaewon 20050713
				// dlg -> createDlg_
				if(IDOK == createDlg_.DoModal())
				//@} Jaewon
				{
					//@{ Jaewon 20050414
					// In an indoor scene, turn the dynamic lightmap off(ambientOcclusionMVL.fx will do the lighting).
					//@{ Jaewon 20050713
					// dlg -> createDlg_
					if(createDlg_.rayLength_ != 0)
					//@} Jaewon
					{
						AgcmDynamicLightmap *pAgcmDynamicLightmap = (AgcmDynamicLightmap*)(g_MyEngine.GetModule("AgcmDynamicLightmap"));
						if(pAgcmDynamicLightmap)
							pAgcmDynamicLightmap->Off();
					}
					//@} Jaewon

					// Create maps.
					//@{ Jaewon 20050203
					//@{ Jaewon 20050207
					// dlg.rayCount_
					//@{ Jaewon 20050713
					// dlg -> createDlg_
					// 'createDlg_.recreateAll_' added.
					VERIFY( g_MyEngine.m_ambOcclMap.create(createDlg_.rayCount_, createDlg_.rayLength_, createDlg_.recreateAll_) );
					//@} Jaewon
					//@} Jaewon
					//@} Jaewon

					created_ = true;

					DisplayMessage("Successfully created.");

					// Change the label of this button.
					createButton_.SetWindowText("Destroy");

					// Enable processing buttons.
					clearButton_.EnableWindow(TRUE);
					computeButton_.EnableWindow(TRUE);
					saveButton_.EnableWindow(TRUE);
				}
			}
			else
			// destroy
			{
				//@{ Jaewon 20050713
				// ;)
				if(IDOK == destroyDlg_.DoModal())
				//@} Jaewon
				{
					//@{ Jaewon 20050713
					// 'destroyDlg_.destroyAll_' added.
					VERIFY( g_MyEngine.m_ambOcclMap.destroy(destroyDlg_.destroyAll_) );
					//@} Jaewon

					created_= false;

					DisplayMessage("Successfully destroyed.");

					// change the label of this button.
					createButton_.SetWindowText("Create...");

					// disable processing buttons.
					clearButton_.EnableWindow(FALSE);
					computeButton_.EnableWindow(FALSE);
					saveButton_.EnableWindow(FALSE);
				}
			}
		}
		else if(IS_BUTTON_CLICKED(CTRLID_BUTTON_CLEAR, wParam))
		{
			bool ok = g_MyEngine.m_ambOcclMap.clear();
			ASSERT(ok);

			DisplayMessage("Successfully cleared.");
		}
		else if(IS_BUTTON_CLICKED(CTRLID_BUTTON_COMPUTE, wParam))
		{
			if(false == g_MyEngine.m_computingAmbOcclMaps)
			// compute
			{
				// Open a supersample dialog.
				//@{ Jaewon 20050713
				// dlg -> computeDlg_
				if(IDOK == computeDlg_.DoModal())
				//@} Jaewon
				{
					// Set the supersampling value.
					g_MyEngine.m_ambOcclMap.setSuperSample((unsigned int)computeDlg_.supersample_);
					DisplayMessage("Set supersample to %d.", g_MyEngine.m_ambOcclMap.getSuperSample());

					// Start(or restart) computing.
					g_MyEngine.m_computingAmbOcclMaps = true;
					DisplayMessage("Start computing...");

					// Change the label of this button.
					computeButton_.SetWindowText("Pause");

					// Disable other controls.
					createButton_.EnableWindow(FALSE);
					clearButton_.EnableWindow(FALSE);
					saveButton_.EnableWindow(FALSE);
				}
			}
			else
			// pause
			{
				// Pause computing.
				g_MyEngine.m_computingAmbOcclMaps = false;
				DisplayMessage("Computing paused.");

				// reset the slice progress bar.
				setSliceProgress(0);

				// Change the label of this button.
				computeButton_.SetWindowText("Compute...");

				// Enable other controls.
				createButton_.EnableWindow(TRUE);
				//@{ Jaewon 20050204
				//clearButton_.EnableWindow(TRUE);
				//@{ Jaewon 20050713
				// Support saving in pause.
				saveButton_.EnableWindow(TRUE);
				//@} Jaewon
				//@} Jaewon
			}
		}
		else if(IS_BUTTON_CLICKED(CTRLID_BUTTON_SAVE, wParam))
		{
			char buf[MAX_PATH];
			sprintf(buf, "%s\\Texture\\AmbOccl\\", ALEF_CURRENT_DIRECTORY);
			bool ok = g_MyEngine.m_ambOcclMap.save(buf);

			if(ok)
				DisplayMessage("Successfully saved.");
			else
				DisplayMessage("Write failure!");
		}
		else if(IS_COMBOBOX_SELCHANGED(CTRLID_COMBOBOX_TECHNIQUE, wParam))
		{
			// change the active technique of "ambientOcclusion.fx".
			int index = 0;
			index = technicComboBox_.GetCurSel();
			RpMaterialD3DFxForAllEffects(effectTechniqueCB, (void*)&index);
		}
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

// -----------------------------------------------------------------------------
//@{ Jaewon 20050204
void CPlugin_AmbOcclMap::endComputing()
{
	DisplayMessage("End computing.");

	// Reset the label of the pause button.
	computeButton_.SetWindowText("Compute...");

	// Enable other controls.
	createButton_.EnableWindow(TRUE);
	saveButton_.EnableWindow(TRUE);
}
//@} Jaewon

// -----------------------------------------------------------------------------
// Plugin_AmbOcclMap.cpp - End of file
// -----------------------------------------------------------------------------
// D:\Alef\Tools\MapTool\Plugin_AmbOcclMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Plugin_AmbOcclMap.h"


// CAmbOcclMapPrefixDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAmbOcclMapPrefixDlg, CDialog)
CAmbOcclMapPrefixDlg::CAmbOcclMapPrefixDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmbOcclMapPrefixDlg::IDD, pParent)
	, rayLength_(0)
	//@{ Jaewon 20050713
	// ;)
	, rayCount_(64)
	//@} Jaewon
	, recreateAll_(FALSE)
{
}

CAmbOcclMapPrefixDlg::~CAmbOcclMapPrefixDlg()
{
}

void CAmbOcclMapPrefixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AMBOCCLMAP_RAYLENGTH, rayLength_);
	DDX_Text(pDX, IDC_EDIT_AMBOCCLMAP_RAYCOUNT, rayCount_);
	DDV_MinMaxUInt(pDX, rayCount_, 4, 1024);
	DDX_Check(pDX, IDC_CHECK_RECREATE_ALL, recreateAll_);
}


BEGIN_MESSAGE_MAP(CAmbOcclMapPrefixDlg, CDialog)
END_MESSAGE_MAP()


// CAmbOcclMapPrefixDlg 메시지 처리기입니다.
// D:\Alef\Tools\MapTool\Plugin_AmbOcclMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Plugin_AmbOcclMap.h"


// CAmbOcclMapSupersampleDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAmbOcclMapSupersampleDlg, CDialog)
CAmbOcclMapSupersampleDlg::CAmbOcclMapSupersampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmbOcclMapSupersampleDlg::IDD, pParent)
	//@{ Jaewon 20050713
	// ;)
	, supersample_(2)
	//@} Jaewon
{
}

CAmbOcclMapSupersampleDlg::~CAmbOcclMapSupersampleDlg()
{
}

void CAmbOcclMapSupersampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AMBOCCLMAP_SUPERSAMPLE, supersample_);
	DDV_MinMaxByte(pDX, supersample_, 1, 16);
}


BEGIN_MESSAGE_MAP(CAmbOcclMapSupersampleDlg, CDialog)
END_MESSAGE_MAP()


// CAmbOcclMapSupersampleDlg 메시지 처리기입니다.
//@{ Jaewon 20050713
// ;)
// G:\svn\Alef\Tools\MapTool\Plugin_AmbOcclMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Plugin_AmbOcclMap.h"


// CAmbOcclMapDestroyDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAmbOcclMapDestroyDlg, CDialog)
CAmbOcclMapDestroyDlg::CAmbOcclMapDestroyDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAmbOcclMapDestroyDlg::IDD, pParent)
, destroyAll_(FALSE)
{
}

CAmbOcclMapDestroyDlg::~CAmbOcclMapDestroyDlg()
{
}

void CAmbOcclMapDestroyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_DESTROY_ALL, destroyAll_);
}


BEGIN_MESSAGE_MAP(CAmbOcclMapDestroyDlg, CDialog)
END_MESSAGE_MAP()


// CAmbOcclMapDestroyDlg 메시지 처리기입니다.
//@} Jaewon