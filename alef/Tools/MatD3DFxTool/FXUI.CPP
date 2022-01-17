//@{ Jaewon 20040826
// created.
// a fx user interface class for MatD3DFxTool.
//@} Jaewon

#include <rpcollis.h>
#include <rtpick.h>
#include <rpskin.h>
#include "RtSkinSp.h"
#include <tchar.h>
#include "FXUI.h"
#include "skel\skeleton.h"
#include "skel\win.h"
//@{ Jaewon 20041103
#include "MemoryManager\mmgr.h"
//@} Jaewon

#define IDC_SHEET_COMBOBOX	1
#define IDC_TECHNIC_COMBOBOX 1

extern bool _bAltKey;

//@{ Jaewon 20050708
// ;)
static bool _bApplyFxAll = false;
//@} Jaewon
static bool _bAtomicFxPipelineSuccess = false;
static FXUI *_pThis = NULL;
//@{ Jaewon 20040922
// bSkin -> pSkin
static RpSkin *_pSkin;
//@} Jaewon
static FXUI::Sheet *_pSheet = NULL;

//@{ Jaewon 20041102
// initial directory for texture loading
extern TCHAR _texturePath[MAX_PATH];
//@} Jaewon

//@{ Jaewon 20041110
// pack UI controls more tightly in space.
// a constant for vertical spacing
const int _vspace = 20;
//@} Jaewon

struct IntersectionParam
{
    RpIntersection intersection;
    RwReal minDistance;
    RpMaterial *material;
    RpAtomic *currAtomic;
};

//@{ Jaewon 20041102
// lpszInitialDir added.
int	doFileDlg(bool bOpenFileDlg, 
			  LPCTSTR lpszDefExt = NULL,
			  LPTSTR lpszFileName = NULL,
			  DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			  LPCTSTR lpszFilter = NULL,
			  HWND hParentWnd = NULL,
			  LPTSTR lpszFileTitle = NULL,
			  LPCTSTR lpszTitle = NULL,
			  LPTSTR lpszInitialDir = NULL)
{								
	OPENFILENAME	OpenFileStat			//typedef struct tagOFNW {		
		= {	sizeof(OPENFILENAME)			//   DWORD        lStructSize;	
			, hParentWnd					//   HWND         hwndOwner;
			, NULL							//   HINSTANCE    hInstance;
			, lpszFilter					//   LPCWSTR      lpstrFilter;	
			, NULL							//   LPWSTR       lpstrCustomFilter;
			, 0								//   DWORD        nMaxCustFilter;
			, 1								//   DWORD        nFilterIndex;
			, lpszFileName					//   LPWSTR       lpstrFile;	
			, MAX_PATH						//   DWORD        nMaxFile;
			, lpszFileTitle					//   LPWSTR       lpstrFileTitle;	
			, MAX_PATH						//   DWORD        nMaxFileTitle;			
			, lpszInitialDir				//   LPCWSTR      lpstrInitialDir;	
			, lpszTitle						//   LPCWSTR      lpstrTitle;		
			, dwFlags | OFN_NOCHANGEDIR		//   DWORD        Flags;	
			, 0								//   WORD         nFileOffset;	
			, 0								//   WORD         nFileExtension;	
			, lpszDefExt					//   LPCWSTR      lpstrDefExt;		
			, 0								//   LPARAM       lCustData;	
			, NULL							//   LPOFNHOOKPROC lpfnHook;		
			, NULL };						//   LPCWSTR      lpTemplateName;		
	
	BOOL result;

	if(bOpenFileDlg)
	// open dialog
	{
		result = GetOpenFileName(&OpenFileStat);
	}
	else
	// save dialog
	{
		result = GetSaveFileName(&OpenFileStat);
	}

	//@{ Jaewon 20041102
	// update the initial directory.
	if(result && lpszInitialDir)
	{
		_tcscpy(lpszInitialDir, lpszFileName);
		// discard the filename.
		int len = _tcslen(lpszInitialDir);
		while(--len >= 0)
		{
			if(lpszInitialDir[len] == L'\\')
			{
				lpszInitialDir[len+1] = L'\0';
				break;
			}
		}
	}
	//@} Jaewon

	return result;
}
//@} Jaewon

//@{ Jaewon 20041110
// pvData added. 
static void handleSliderControl(UINT nEvent, CDXUTSlider *pControl, CDXUTStatic* pStatic, float *pData, D3DXVECTOR4 *pvData)
//@} Jaewon
{
	void *pUserData = pControl->GetUserData();
	float step = *(float*)&pUserData;

	float value = float(pControl->GetValue()) * step;

	// update data.
	*pData = value;
	
	// update display.
	//@{ Jaewon 20050708
	// 64->128, 32->64
	TCHAR buf[128], tmp[64];
	//@} Jaewon
	_tcscpy(buf, pStatic->GetText());
	int len = _tcslen(buf);
	while(--len >= 0)
	{
		if(buf[len] == L' ')
		{
			buf[len] = L'\0';
			break;
		}
	}
	//@{ Jaewon 20041110
	// vector slider format changed
	if(pvData)
	{
		_stprintf(tmp, L" %1.3f,%1.3f,%1.3f,%1.3f", pvData->x, pvData->y, pvData->z, pvData->w);
	}
	else
	{
		_stprintf(tmp, L" %f", value);
	}
	//@} Jaewon
	_tcscat(buf, tmp);
	pStatic->SetText(buf);
}

static bool handleTextureControl(UINT nEvent, CDXUTButton *pControl, RwTexture **ppTexture)
{
	TCHAR pathname[MAX_PATH];
	TCHAR filetitle[MAX_PATH];
	//@{ Jaewon 20040930
	_tcscpy(pathname, L"*.bmp");
	//@{ Jaewon 20041102
	// _texturePath
	if(doFileDlg(TRUE, L"bmp", pathname, NULL, 
				 L"Bitmap Files (*.bmp)\0*.bmp\0TIFF Files (*.tif)\0*.tif\0DDS Files (*.dds)\0*.dds\0All Files (*.*)\0*.*\0\0",
				 psWindowGetHandle(),
				 filetitle,
				 NULL,
				 _texturePath))
	//@} Jaewon
	//@} Jaewon
	{
		// Jaewon 20041005
		// discard the file extension.
		int len = _tcslen(filetitle);
		while(--len >= 0)
		{
			if(filetitle[len] == L'.')
			{
				filetitle[len] = L'\0';
				break;
			}
		}
		//@} Jaewon

		// update data.
		if(*ppTexture)
			RwTextureDestroy(*ppTexture);
		char buf[64];
		wcstombs(buf, filetitle, _tcslen(filetitle)+1);
		*ppTexture = RwTextureRead(buf, NULL);

		// update display.
		pControl->SetText(filetitle);
		
		return true;
	}	

	return false;
}

static bool handleColorControl(UINT nEvent, CDXUTButton *pControl, CDXUTStatic *pStatic, D3DXVECTOR4 *color)
{
	static COLORREF acrCustClr[16];
	COLORREF rgbCurrent;
	D3DCOLOR d3dCol = pStatic->GetTextColor();
	rgbCurrent = RGB(0xff & (d3dCol>>16), 
					 0xff & (d3dCol>>8),
					 0xff & d3dCol);

	CHOOSECOLOR chooseColorStruct = 
	{
		sizeof(CHOOSECOLOR),
		psWindowGetHandle(),
		NULL,
		rgbCurrent,
		acrCustClr,
		CC_FULLOPEN | CC_RGBINIT,
		NULL,
		NULL,
		NULL
	};

	if(ChooseColor(&chooseColorStruct))
	{
		BYTE R = GetRValue(chooseColorStruct.rgbResult);
		BYTE G = GetGValue(chooseColorStruct.rgbResult);
		BYTE B = GetBValue(chooseColorStruct.rgbResult);

		// update data.
		color->x = float(R)/255.0f;
		color->y = float(G)/255.0f;
		color->z = float(B)/255.0f;
		color->w = 1.0f;

		// update display.
		pStatic->SetTextColor(D3DCOLOR_RGBA(R, G, B, 255));

		return true;
	}	

	return false;
}

static void CALLBACK onGUIEvent(UINT nEvent, int nControlID, CDXUTControl *pControl)
{
	FXUI::Sheet *pSheet = _pThis->m_sheets[_pThis->m_iSelectedMaterial];

	if(nControlID == IDC_TECHNIC_COMBOBOX)
	{		
		//@{ Jaewon 20041216
		// if an alt key is pressed, apply the change to all materials of the same kind.
		for(int n=0; n<_pThis->m_sheets.size(); ++n)
		{
			if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name)))
				|| (_pThis->m_sheets[n] == pSheet))
			{
				// ensure the selection.
				_pThis->m_sheets[n]->m_dlg.GetComboBox(nControlID)->SetSelectedByText(((CDXUTComboBox*)pControl)->GetSelectedItem()->strText);
				//@{ Jaewon 20041102
				// lighting optimization
				HRESULT hr = _pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->d3dxEffect[0]
								->SetTechnique(D3DXHANDLE(_pThis->m_sheets[n]->m_dlg.GetComboBox(nControlID)->GetSelectedItem()->pData));
				assert(SUCCEEDED(hr));
				//@} Jaewon
			}
		}
		//@} Jaewon
	}
	else if(nControlID == pSheet->m_rwControls[0])
	// rw base texture
	{
		RwTexture *pTexture = NULL;
		if(handleTextureControl(nEvent, (CDXUTButton *)pControl, &pTexture))
		{
			RpMaterialSetTexture(pSheet->m_pMaterial, pTexture);
		}
	}
	else if(nControlID == pSheet->m_rwControls[1])
	// rw material ambient
	{
		D3DXVECTOR4 color;
		if(handleColorControl(nEvent, (CDXUTButton *)pControl, pSheet->m_dlg.GetStatic(nControlID-1), &color))
		{
			//@{ Jaewon 20040909
			// if an alt key is pressed, apply the change to all materials of the same kind.
			for(int n=0; n<_pThis->m_sheets.size(); ++n)
			{
				if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name))) 
					|| (_pThis->m_sheets[n] == pSheet))
				{
					float ambient = 0.3*color.x + 0.59*color.y + 0.11*color.z;
					const RwSurfaceProperties *pProp = RpMaterialGetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial);
					RwSurfaceProperties prop;
					prop.ambient = ambient;
					prop.diffuse = pProp->diffuse;
					prop.specular = pProp->specular;
					RpMaterialSetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial, &prop);
					RwRGBAReal rwColor, rwAmbient;
					RwRGBARealFromRwRGBA(&rwColor, (RwRGBA *)RpMaterialGetColor(_pThis->m_sheets[n]->m_pMaterial));
					RwRGBARealScale(&rwAmbient, &rwColor, prop.ambient);
					_pThis->m_sheets[n]->m_dlg.GetStatic(nControlID-1)->SetTextColor(D3DCOLOR_COLORVALUE(rwAmbient.red, rwAmbient.green, rwAmbient.blue, 1.0f));
				}
			}
			//@} Jaewon
		}
	}
	else if(nControlID == pSheet->m_rwControls[2])
	// rw material diffuse
	{
		D3DXVECTOR4 color;
		if(handleColorControl(nEvent, (CDXUTButton *)pControl, pSheet->m_dlg.GetStatic(nControlID-1), &color))
		{
			//@{ Jaewon 20040909
			// if an alt key is pressed, apply the change to all materials of the same kind.
			for(int n=0; n<_pThis->m_sheets.size(); ++n)
			{
				if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name))) 
					|| (_pThis->m_sheets[n] == pSheet))
				{
					const RwSurfaceProperties *pProp = RpMaterialGetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial);
					RwSurfaceProperties prop;
					prop.ambient = pProp->ambient;
					prop.diffuse = 1.0f;
					prop.specular = pProp->specular;
					RpMaterialSetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial, &prop);
					RwRGBA rgba;
					rgba.red = RwUInt8(255.0f * color.x);
					rgba.green = RwUInt8(255.0f * color.y);
					rgba.blue = RwUInt8(255.0f * color.z);
					rgba.alpha = RwUInt8(255.0f * color.w);
					RpMaterialSetColor(_pThis->m_sheets[n]->m_pMaterial, &rgba);
					_pThis->m_sheets[n]->m_dlg.GetStatic(nControlID-1)->SetTextColor(D3DCOLOR_RGBA(rgba.red, rgba.green, rgba.blue, rgba.alpha));
				}
			}
			//@} Jaewon
		}
	}
	else if(nControlID == pSheet->m_rwControls[3])
	// rw material specular
	{
		D3DXVECTOR4 color;
		if(handleColorControl(nEvent, (CDXUTButton *)pControl, pSheet->m_dlg.GetStatic(nControlID-1), &color))
		{
			//@{ Jaewon 20040909
			// if an alt key is pressed, apply the change to all materials of the same kind.
			for(int n=0; n<_pThis->m_sheets.size(); ++n)
			{
				if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name))) 
					|| (_pThis->m_sheets[n] == pSheet))
				{
					float specular = 0.3*color.x + 0.59*color.y + 0.11*color.z;
					const RwSurfaceProperties *pProp = RpMaterialGetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial);
					RwSurfaceProperties prop;
					prop.ambient = pProp->ambient;
					prop.diffuse = pProp->diffuse;
					prop.specular = specular;
					RpMaterialSetSurfaceProperties(_pThis->m_sheets[n]->m_pMaterial, &prop);
					RwRGBAReal rwColor, rwSpecular;
					RwRGBARealFromRwRGBA(&rwColor, (RwRGBA *)RpMaterialGetColor(_pThis->m_sheets[n]->m_pMaterial));
					RwRGBARealScale(&rwSpecular, &rwColor, prop.specular);
					_pThis->m_sheets[n]->m_dlg.GetStatic(nControlID-1)->SetTextColor(D3DCOLOR_COLORVALUE(rwSpecular.red, rwSpecular.green, rwSpecular.blue, 1.0f));
				}
			}
			//@} Jaewon
		}
	}
	else
	{
		for(int i=0; i<pSheet->m_controls.size(); ++i)
		{
			if(nControlID == pSheet->m_controls[i]->m_id)
			{
				if(strcmp(pSheet->m_controls[i]->m_widget, "Slider") == 0)
				{
					//@{ Jaewon 20041110
					// vector slider format changed
					int nStaticID = nControlID-1;
					if(pSheet->m_controls[i]->m_vdata)
					{
						nStaticID = nControlID - 1 -
									(RwUInt32(pSheet->m_controls[i]->m_data) - RwUInt32(pSheet->m_controls[i]->m_vdata))/4;
					}

					handleSliderControl(nEvent, (CDXUTSlider *)pControl, pSheet->m_dlg.GetStatic(nStaticID), (float*)(pSheet->m_controls[i]->m_data), (D3DXVECTOR4*)(pSheet->m_controls[i]->m_vdata));

					//@{ Jaewon 20041109
					// if an alt key is pressed, apply the change to all materials of the same kind.
					for(int n=0; n<_pThis->m_sheets.size(); ++n)
					{
						if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name))) 
							|| (_pThis->m_sheets[n] == pSheet))
						{
							if(pSheet->m_controls[i]->m_vdata)
							// an element of a vector
								RpMaterialD3DFxSetVector(_pThis->m_sheets[n]->m_pMaterial, _pThis->m_sheets[n]->m_controls[i]->m_name, pSheet->m_controls[i]->m_vdata);
							else
							// just float
								RpMaterialD3DFxSetFloat(_pThis->m_sheets[n]->m_pMaterial, _pThis->m_sheets[n]->m_controls[i]->m_name, *(float*)(pSheet->m_controls[i]->m_data));
	
							// update display.
							_pThis->m_sheets[n]->m_dlg.GetSlider(nControlID)->SetValue(((CDXUTSlider *)pControl)->GetValue());
							_pThis->m_sheets[n]->m_dlg.GetStatic(nStaticID)->SetText(pSheet->m_dlg.GetStatic(nStaticID)->GetText());
						}
					}
					//@} Jaewon
					//@} Jaewon
				}
				else if(strcmp(pSheet->m_controls[i]->m_widget, "Texture") == 0)
				{
					RwTexture *pTexture = NULL;
					handleTextureControl(nEvent, (CDXUTButton *)pControl, &pTexture);
					if(pTexture)
					{
						RpMaterialD3DFxSetTexture(pSheet->m_pMaterial, pSheet->m_controls[i]->m_name, pTexture);
						RwTextureDestroy(pTexture);
					}
				}
				else if(strcmp(pSheet->m_controls[i]->m_widget, "Color") == 0)
				{
					if(handleColorControl(nEvent, (CDXUTButton *)pControl, pSheet->m_dlg.GetStatic(nControlID-1),
									   (D3DXVECTOR4*)(pSheet->m_controls[i]->m_data)))
					{
						//@{ Jaewon 20040909
						// if an alt key is pressed, apply the change to all materials of the same kind.
						for(int n=0; n<_pThis->m_sheets.size(); ++n)
						{
							if((_bAltKey && (0 == strcmp(_pThis->m_sheets[n]->m_pEffect->pSharedD3dXEffect->name, pSheet->m_pEffect->pSharedD3dXEffect->name))) 
								|| (_pThis->m_sheets[n] == pSheet))
							{
								RpMaterialD3DFxSetVector(_pThis->m_sheets[n]->m_pMaterial, _pThis->m_sheets[n]->m_controls[i]->m_name, (D3DXVECTOR4*)(pSheet->m_controls[i]->m_data));
								D3DXVECTOR4* pvCol = (D3DXVECTOR4*)(pSheet->m_controls[i]->m_data);
								_pThis->m_sheets[n]->m_dlg.GetStatic(nControlID-1)->SetTextColor(D3DCOLOR_COLORVALUE(pvCol->x, pvCol->y, pvCol->z, pvCol->w));
							}
						}
						//@} Jaewon
					}
				}
				break;
			}
		}
	}

	//@{ Jaewon 20040909
	//@{ Jaewon 20041109
	//if(_bAltKey)
	//	_bAltKey = false;
	//@} Jaewon
	//@} Jaewon
}

static void CALLBACK onGUIEventSS(UINT nEvent, int nControlID, CDXUTControl *pControl)
{
	switch(nControlID)
	{
	case IDC_SHEET_COMBOBOX:
		if(_pThis->m_sheets[_pThis->m_iSelectedMaterial]->m_pEffect)
			_pThis->m_sheets[_pThis->m_iSelectedMaterial]->m_pEffect->selected = FALSE;
		_pThis->m_iSelectedMaterial = int(((CDXUTComboBox*)pControl)->GetSelectedItem()->pData);
		if(_pThis->m_bWireframeOnSelection && _pThis->m_sheets[_pThis->m_iSelectedMaterial]->m_pEffect)
			_pThis->m_sheets[_pThis->m_iSelectedMaterial]->m_pEffect->selected = TRUE;
		break;
	}
}

FXUI::FXUI()
{
	m_w = m_h = 0;
	m_iSelectedMaterial = 0;

	m_sheetSelectionDlg.SetCallback(onGUIEventSS);

	m_bWireframeOnSelection = true;
	
	assert(_pThis == NULL);
	_pThis = this;
}

FXUI::~FXUI()
{
	_pThis = NULL;

	m_bWireframeOnSelection = true;

	clear();

	m_w = m_h = 0;
}

void FXUI::clear()
{
	for(int i=0; i<m_sheets.size(); ++i)
	{
		for(int j=0; j<m_sheets[i]->m_controls.size(); ++j)
			delete m_sheets[i]->m_controls[j];

		m_sheets[i]->m_controls.clear();

		m_sheets[i]->m_dlg.RemoveAllControls();

		delete m_sheets[i];
	}

	m_sheets.clear();
	
	m_sheetSelectionDlg.RemoveAllControls();
	
	m_iSelectedMaterial = 0;
}

static RpCollisionTriangle *triangleIntersectionCB(RpIntersection *intersection, 
												   RpCollisionTriangle *triangle,
												   RwReal distance, void *data)
{
    IntersectionParam *params;

    params = (IntersectionParam *)data;

    if (distance < params->minDistance)
    {
        RpGeometry *geom;
        RpTriangle *tris;
        params->minDistance = distance;

        geom = RpAtomicGetGeometry(params->currAtomic);
        tris = RpGeometryGetTriangles(geom);
        params->material = geom->matList.materials[tris[triangle->index].matIndex];
    }

    return triangle;
}


static RpAtomic *atomicIntersectionCB(RpIntersection *intersection, 
									  RpWorldSector *sector, RpAtomic *atomic,
									  RwReal distance, void *data)
{
    IntersectionParam *params;
    RwReal oldDistance;

    params = (IntersectionParam *)data;
    oldDistance = params->minDistance;
    params->currAtomic = atomic;

    RpAtomicForAllIntersections(atomic, &params->intersection, triangleIntersectionCB, data);

    return atomic;
}

void FXUI::findClosestMaterialToPosition(RpClump *pClump, RwCamera *pCamera, RwV2d *pPos)
{
	int	i;
	RwLine line;
	IntersectionParam params;

	if(pClump)
	{
		RwCameraCalcPixelRay(pCamera, &line, pPos);    

		params.intersection.type = rpINTERSECTLINE;
		params.intersection.t.line = line;
		params.minDistance = RwRealMAXVAL;
		params.material = NULL;

		RpWorldForAllAtomicIntersections(RpClumpGetWorld(pClump), &params.intersection,
			atomicIntersectionCB, (void *)&params);

		if(params.material)
		{
			for(i=0; i<m_sheets.size(); ++i)
				if(m_sheets[i]->m_pMaterial == params.material)
				{
					if(m_sheets[m_iSelectedMaterial]->m_pEffect)
						m_sheets[m_iSelectedMaterial]->m_pEffect->selected = FALSE;
					m_iSelectedMaterial = i;
					if(m_bWireframeOnSelection && m_sheets[m_iSelectedMaterial]->m_pEffect)
						m_sheets[m_iSelectedMaterial]->m_pEffect->selected = TRUE;
					break;
				}

			assert(i<m_sheets.size());
	
			// update the sheet combo box by the selection.
			m_sheetSelectionDlg.GetComboBox(IDC_SHEET_COMBOBOX)->SetSelectedByIndex(m_iSelectedMaterial);
		}
	}

	return;
}

static RpMaterial *materialBuildSheetCB(RpMaterial *material, void *data)
{
	int *pCount = (int *)data;

	assert(_pThis->m_sheets.size() == *pCount);
	_pThis->m_sheets.push_back(new FXUI::Sheet);
	_pThis->m_sheets[*pCount]->m_pMaterial = material;
	_pThis->m_sheets[*pCount]->m_bNeedUpdate = true;
	_pThis->m_sheets[*pCount]->m_pEffect = RpMaterialD3DFxGetEffect(material);
	memset(_pThis->m_sheets[*pCount]->m_rwControls, 0, sizeof(_pThis->m_sheets[*pCount]->m_rwControls)); 
	_pThis->m_sheets[*pCount]->m_dlg.SetCallback(onGUIEvent);
	_pThis->m_sheets[*pCount]->m_dlg.SetSize(200, _pThis->m_h-50);
	_pThis->m_sheets[*pCount]->m_dlg.SetLocation(_pThis->m_w - 200, 50);

	CDXUTComboBox *pComboBox = _pThis->m_sheetSelectionDlg.GetComboBox(IDC_SHEET_COMBOBOX);
	DxEffect *pEffect = RpMaterialD3DFxGetEffect(material);
	wchar_t wbuf[64];
	if(pEffect)
		mbstowcs(wbuf, pEffect->pSharedD3dXEffect->name, 
				strlen(pEffect->pSharedD3dXEffect->name)+1);
	else
		wcscpy(wbuf, L"no fx yet");
	pComboBox->AddItem(wbuf, LPVOID(*pCount));

	(*pCount) += 1;
    
    return material;
}

static RpAtomic *atomicBuildSheetCB(RpAtomic *atomic, void *data)
{
    RpGeometry *geometry;

    geometry = RpAtomicGetGeometry(atomic);
    RpGeometryForAllMaterials(geometry, materialBuildSheetCB, data);
    return atomic;
}

static void *addParamControlCB(DxEffect *effect, RwUInt32 type, RwChar *name, void *data, void *callbackData)
{
	FXUI::Control *pControl;
	EffectGUIParams params;
	RwChar guiName[FX_HANDLE_NAME_LENGTH];
	RwChar widget[FX_HANDLE_NAME_LENGTH];

	int id = *(int*)callbackData; 

	params.guiName = guiName;
	params.widget = widget;

	EffectParameterGetGUI(effect, name, &params);

	switch(type)
	{
		case EFFECT_TWEAKABLE_PARAM_FLOAT:
		{
			pControl = new FXUI::Control;
			pControl->m_data = data;
			strcpy(pControl->m_name, name);
			pControl->m_vdata = NULL;
			pControl->m_id = id+1;
			//strcpy(pControl->m_widget, params.widget);
			strcpy(pControl->m_widget, "Slider");
			_pSheet->m_controls.push_back(pControl);

			//@{ Jaewon 20050708
			// 64->128, 32->64
			wchar_t wbuf[128], tmp[64];
			//@} Jaewon
			mbstowcs(wbuf, params.guiName, strlen(params.guiName)+1);
			// display the current value.
			_stprintf(tmp, L" %f", *(float*)data);
			_tcscat(wbuf, tmp);
			_pSheet->m_dlg.AddStatic(id, wbuf, 30, -20+id*_vspace+10, 150, 20);
			CDXUTSlider *pSlider;
			_pSheet->m_dlg.AddSlider(id+1, 30, -20+(id+1)*_vspace+10-5, 150, 20, 
									 int(params.minRange/params.step), int(params.maxRange/params.step), 
									 int((*(float*)data)/params.step), false, &pSlider);
			// a dirty hack for saving the step value
			pSlider->SetUserData(*(void**)&(params.step));
			
			*(int*)callbackData = id + 2;
			break;
		}

		case EFFECT_TWEAKABLE_PARAM_VECTOR:
		{
			D3DXVECTOR4 *vector = (D3DXVECTOR4 *)data;

			if(0 ==strcmp(params.widget, "Color"))
			// color
			{
				pControl = new FXUI::Control;
				pControl->m_data = data;
				pControl->m_vdata = NULL;
				strcpy(pControl->m_name, name);
				pControl->m_id = id+1;
				strcpy(pControl->m_widget, "Color");
				_pSheet->m_controls.push_back(pControl);
		
				wchar_t wbuf[64];
				mbstowcs(wbuf, params.guiName, strlen(params.guiName)+1);
				CDXUTStatic *pStatic;
				_pSheet->m_dlg.AddStatic(id, wbuf, 30, -20+id*_vspace+10, 150, 20, false, &pStatic);
				_pSheet->m_dlg.AddButton(id+1, L"COLOUR", 30, -20+(id+1)*_vspace+10-5, 150, 20);
				pStatic->SetTextColor(D3DCOLOR_RGBA(int(vector->x*255.0f), int(vector->y*255.0f), 
													int(vector->z*255.0f), int(vector->w*255.0f)));

				*(int*)callbackData = id + 2;
			}
			else
			// float4
			{
				//@{ Jaewon 20041110
				// pack elements of a vector more tightly in space.
				wchar_t wbuf[128], tmp[64];
				mbstowcs(wbuf, params.guiName, strlen(params.guiName)+1);
				_stprintf(tmp, L" %1.3f,%1.3f,%1.3f,%1.3f", vector->x, vector->y, vector->z, vector->w);
				_tcscat(wbuf, tmp);
				CDXUTStatic *pStatic;
				_pSheet->m_dlg.AddStatic(id, wbuf, 30, -20+id*_vspace+10, 150, 20, false, &pStatic);
				pStatic->GetElement(0)->dwTextFormat = DT_NOCLIP|DT_RIGHT;
				for(int i=0; i<4; ++i)
				{
					pControl = new FXUI::Control;
					pControl->m_id = id+i+1;
					if(0==i)
					{
						pControl->m_data = &vector->x;
						//_tcscat(wbuf, L".x");
					}
					else if(1==i)
					{
						pControl->m_data = &vector->y;
						//_tcscat(wbuf, L".y");
					}
					else if(2==i)
					{
						pControl->m_data = &vector->z;
						//_tcscat(wbuf, L".z");
					}
					else
					{
						pControl->m_data = &vector->w;
						//_tcscat(wbuf, L".w");
					}
					strcpy(pControl->m_name, name);
					pControl->m_vdata = vector;
					strcpy(pControl->m_widget, "Slider");
					_pSheet->m_controls.push_back(pControl);

					// display the current value.
					CDXUTSlider *pSlider;
					_pSheet->m_dlg.AddSlider(id+i+1, 30, -20+(id+i+1)*_vspace+10-5, 150, 20, 
											 int(params.minRange/params.step), int(params.maxRange/params.step), 
											 int((*(float*)(pControl->m_data))/params.step), false, &pSlider);
					
					// a dirty hack for saving the step value
					pSlider->SetUserData(*(void**)&(params.step));
				}
				*(int*)callbackData = id + 1 + 4;
				//@} Jaewon
			}
			break;
		}

		case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
		{
			/* editing a 4x4 matrix with the skeleton menu will be a bit of a pain so I won`t write. Anyway
			   the skeleton menu is just being used for testing the tweakable parameter API and will be
			   replaced by a set of Windows controls in Studio */
			break;
		}

		case EFFECT_TWEAKABLE_PARAM_TEXTURE:
		{
			RwTexture **pTexture = (RwTexture **)data;
			
			pControl = new FXUI::Control;
			pControl->m_data = data;
			strcpy(pControl->m_name, name);
			pControl->m_vdata = NULL;
			pControl->m_id = id+1;
			strcpy(pControl->m_widget, "Texture");
			_pSheet->m_controls.push_back(pControl);
	
			wchar_t wbuf[64];
			mbstowcs(wbuf, params.guiName, strlen(params.guiName)+1);
			_pSheet->m_dlg.AddStatic(id, wbuf, 30, -20+id*_vspace+10, 150, 20);
			char *name = *pTexture?RwTextureGetName(*pTexture):NULL;
			if(name)
				mbstowcs(wbuf, name, strlen(name)+1);
			else
				_tcscpy(wbuf, L"no texture");
			_pSheet->m_dlg.AddButton(id+1, wbuf, 30, -20+(id+1)*_vspace+10-5, 150, 20);

			*(int*)callbackData = id + 2;

			break;
		}
	}

	return effect;
}

int FXUI::addRwParamControls(Sheet *pSheet)
{
	int nAdded = 0;

	int idBase = IDC_TECHNIC_COMBOBOX + 1;
	
	//@{ Jaewon 20041102
	// lighting optimization
	if(pSheet->m_pEffect->rw[ID_BASE_TEXTURE].handle[0])
	{
		pSheet->m_rwControls[0] = idBase+2*nAdded+1;

		wchar_t wbuf[64];
		pSheet->m_dlg.AddStatic(pSheet->m_rwControls[0]-1, L"Base texture", 30, -20+(pSheet->m_rwControls[0]-1)*_vspace+10, 150, 20);
		RwTexture *pTexture = RpMaterialGetTexture(pSheet->m_pMaterial);
		char *name = pTexture?RwTextureGetName(pTexture):NULL;
		if(name)
			mbstowcs(wbuf, name, strlen(name)+1);
		else
			_tcscpy(wbuf, L"no texture");
		pSheet->m_dlg.AddButton(pSheet->m_rwControls[0], wbuf, 30, -20+pSheet->m_rwControls[0]*_vspace+10-5, 150, 20);

		++nAdded;
	}
	else
		pSheet->m_rwControls[0] = 0;

	if(pSheet->m_pEffect->rw[ID_AMBIENT].handle[0])
	{
		pSheet->m_rwControls[1] = idBase+2*nAdded+1;
		
		CDXUTStatic *pStatic;
		pSheet->m_dlg.AddStatic(pSheet->m_rwControls[1]-1, L"Material ambient", 30, -20+(pSheet->m_rwControls[1]-1)*_vspace+10, 150, 20, false, &pStatic);
		pSheet->m_dlg.AddButton(pSheet->m_rwControls[1], L"COLOUR", 30, -20+pSheet->m_rwControls[1]*_vspace+10-5, 150, 20);

		RwRGBAReal color;
		RwRGBAReal ambient;
		const RwSurfaceProperties *pSurfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(pSheet->m_pMaterial));
		pSurfaceProp = RpMaterialGetSurfaceProperties(pSheet->m_pMaterial);
		RwRGBARealScale(&ambient, &color, pSurfaceProp->ambient);
		pStatic->SetTextColor(D3DCOLOR_RGBA(int(ambient.red*255.0f), int(ambient.green*255.0f), 
											int(ambient.blue*255.0f), int(1.0*255.0f)));

		++nAdded;
	}
	else
		pSheet->m_rwControls[1] = 0;

	if(pSheet->m_pEffect->rw[ID_DIFFUSE].handle[0])
	{
		pSheet->m_rwControls[2] = idBase+2*nAdded+1;
		
		CDXUTStatic *pStatic;
		pSheet->m_dlg.AddStatic(pSheet->m_rwControls[2]-1, L"Material diffuse", 30, -20+(pSheet->m_rwControls[2]-1)*_vspace+10, 150, 20, false, &pStatic);
		pSheet->m_dlg.AddButton(pSheet->m_rwControls[2], L"COLOUR", 30, -20+pSheet->m_rwControls[2]*_vspace+10-5, 150, 20);

		RwRGBAReal color;
		RwRGBAReal diffuse;
		const RwSurfaceProperties *pSurfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(pSheet->m_pMaterial));
		pSurfaceProp = RpMaterialGetSurfaceProperties(pSheet->m_pMaterial);
		RwRGBARealScale(&diffuse, &color, pSurfaceProp->diffuse);
		pStatic->SetTextColor(D3DCOLOR_RGBA(int(diffuse.red*255.0f), int(diffuse.green*255.0f), 
											int(diffuse.blue*255.0f), int(diffuse.alpha*255.0f)));

		++nAdded;
	}
	else
		pSheet->m_rwControls[2] = 0;

	if(pSheet->m_pEffect->rw[ID_SPECULAR].handle[0])
	{
		pSheet->m_rwControls[3] = idBase+2*nAdded+1;
		
		CDXUTStatic *pStatic;
		pSheet->m_dlg.AddStatic(pSheet->m_rwControls[3]-1, L"Material specular", 30, -20+(pSheet->m_rwControls[3]-1)*_vspace+10, 150, 20, false, &pStatic);
		pSheet->m_dlg.AddButton(pSheet->m_rwControls[3], L"COLOUR", 30, -20+pSheet->m_rwControls[3]*_vspace+10-5, 150, 20);

		RwRGBAReal color;
		RwRGBAReal specular;
		const RwSurfaceProperties *pSurfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(pSheet->m_pMaterial));
		pSurfaceProp = RpMaterialGetSurfaceProperties(pSheet->m_pMaterial);
		RwRGBARealScale(&specular, &color, pSurfaceProp->specular);
		pStatic->SetTextColor(D3DCOLOR_RGBA(int(specular.red*255.0f), int(specular.green*255.0f), 
											int(specular.blue*255.0f), int(1.0*255.0f)));

		++nAdded;
	}
	else
		pSheet->m_rwControls[3] = 0;
	//@} Jaewon

	return nAdded;
}

//@{ Jaewon 20050825
// This returns true if the clump has fx.
bool FXUI::rebuild(RpClump *clump)
{
	bool hasFX = false;

	// if there is no sheet, build sheets first.
	int count = 0;
	if(m_sheets.size() == 0)
	{
		m_sheetSelectionDlg.AddComboBox(IDC_SHEET_COMBOBOX, 0, 10, 150, 30);
		m_sheetSelectionDlg.GetComboBox(IDC_SHEET_COMBOBOX)->SetDropHeight(100);

		RpClumpForAllAtomics(clump, atomicBuildSheetCB, (void *)&count);

		if(m_bWireframeOnSelection && m_sheets[0]->m_pEffect)
			m_sheets[0]->m_pEffect->selected = TRUE;
	}

	for(int i=0; i<m_sheets.size(); ++i)
	{
		if(m_sheets[i]->m_bNeedUpdate == false)
			continue;
		
		// clear, if any.
		m_sheets[i]->m_dlg.RemoveAllControls();
		for(int j=0; j<m_sheets[i]->m_controls.size(); ++j)
			delete m_sheets[i]->m_controls[j];	
		
		m_sheets[i]->m_controls.clear();
	
		if(m_sheets[i]->m_pEffect)
		{
			hasFX = true;

			// technique combo box
			CDXUTComboBox *pTechicComboBox;
			m_sheets[i]->m_dlg.AddComboBox(IDC_TECHNIC_COMBOBOX, 30, 0, 150, 30, 0, false, &pTechicComboBox);
			//@{ Jaewon 20041102
			// lighting optimization
			LPD3DXEFFECT pEffect = m_sheets[i]->m_pEffect->pSharedD3dXEffect->d3dxEffect[0];
			//@} Jaewon
			HRESULT hr;
			D3DXHANDLE technique = NULL;
			hr = pEffect->FindNextValidTechnique(NULL, &technique);
			assert(SUCCEEDED(hr));
			do 
			{
				D3DXTECHNIQUE_DESC desc;
				pEffect->GetTechniqueDesc(technique, &desc);
				wchar_t wbuf[64];
				mbstowcs(wbuf, desc.Name, strlen(desc.Name)+1);
				pTechicComboBox->AddItem(wbuf, LPVOID(technique));				

				hr = pEffect->FindNextValidTechnique(technique, &technique);
				assert(SUCCEEDED(hr));
			} while(technique);

			// add controls for tweakable RenderWare parameters, if any.
			int nAdded = addRwParamControls(m_sheets[i]);
			
			// for each tweakable parameter, add a proper control.
			int count = IDC_TECHNIC_COMBOBOX + 2*nAdded + 1;
			_pSheet = m_sheets[i];
			RpMaterialD3DFxForAllTweakableParameters(m_sheets[i]->m_pMaterial, addParamControlCB, &count);

			// add a fx description text, if any.
			D3DXHANDLE hDesc = pEffect->GetParameterByName(NULL, "description");
			if(hDesc)
			{
				const char *pstrDesc;
				pEffect->GetString(hDesc, &pstrDesc);
				wchar_t wbuf[512];
				mbstowcs(wbuf, pstrDesc, strlen(pstrDesc)+1);
				CDXUTStatic *pStatic;
				m_sheets[i]->m_dlg.AddStatic(count, wbuf, 30, -20+count*_vspace+10, 150, 60, false, &pStatic);
				pStatic->GetElement(0)->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
				pStatic->SetTextColor(D3DCOLOR_RGBA(255,0,0,255));
			}
		}
	}

	return hasFX;
}
//@} Jaewon

void FXUI::render(float fElapsedTime)
{
	if(m_sheets.size())
		m_sheets[m_iSelectedMaterial]->m_dlg.OnRender(fElapsedTime);

	m_sheetSelectionDlg.OnRender(fElapsedTime);
}

bool FXUI::msgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bNoFurtherProcessing 
		= m_sheetSelectionDlg.MsgProc(hWnd, uMsg, wParam, lParam);
	if(bNoFurtherProcessing)
		return true;

	if(m_sheets.size() > 0)
		return m_sheets[m_iSelectedMaterial]->m_dlg.MsgProc(hWnd, uMsg, wParam, lParam);
	else
		return false;
}

static RpMaterial *materialSetEffectCB(RpMaterial *material, void *data)
{
	int	i;
	RwBool success;

	//@{ Jaewon 20050708
	// _bApplyFxAll condition added
	if(material == _pThis->m_sheets[_pThis->m_iSelectedMaterial]->m_pMaterial
	|| NULL == RpMaterialD3DFxGetEffect(material)
	|| _bApplyFxAll)
	//@} Jaewon
	{
		//@{ Jaewon 20050907
		// If the material has got an effect which has specific material parameters previously,
		// pass their value to a new effect if possible.
		bool passAmbient=false, passDiffuse=false, passSpecular=false;
		D3DXVECTOR4 ambient, diffuse, specular;
		if(RpMaterialD3DFxGetEffect(material))
		{
			if(RpMaterialD3DFxGetVector(material, "materialAmbient", &ambient))
				passAmbient = true;
			if(RpMaterialD3DFxGetVector(material, "materialDiffuse", &diffuse))
				passDiffuse = true;
			if(RpMaterialD3DFxGetVector(material, "materialSpecular", &specular))
				passSpecular = true;
		}
		//@} Jaewon

		//@{ Jaewon 20040922
		// bSkin -> pSkin
		success = RpMaterialD3DFxSetEffect(material, (RwChar *)data, _pSkin);
		//@} Jaewon
		if(success)
		{
			//@{ Jaewon 20050907
			// Pass material colors to the new effect if possible.
			if(passAmbient)
				RpMaterialD3DFxSetVector(material, "materialAmbient", &ambient);
			if(passDiffuse)
				RpMaterialD3DFxSetVector(material, "materialDiffuse", &diffuse);
			if(passSpecular)
				RpMaterialD3DFxSetVector(material, "materialSpecular", &specular);
			//@} Jaewon

			for(i=0; i<_pThis->m_sheets.size(); ++i)
				if(_pThis->m_sheets[i]->m_pMaterial == material)
				{
					break;
				}

			assert(i < _pThis->m_sheets.size());
			_pThis->m_sheets[i]->m_pEffect = RpMaterialD3DFxGetEffect(material);
			_pThis->m_sheets[i]->m_bNeedUpdate = true;

			// change the corresponding combo box entry.
			mbstowcs(_pThis->m_sheetSelectionDlg.GetComboBox(IDC_SHEET_COMBOBOX)->GetItem(i)->strText, 
					RpMaterialD3DFxGetEffect(material)->pSharedD3dXEffect->name,
					strlen(RpMaterialD3DFxGetEffect(material)->pSharedD3dXEffect->name)+1);

			//@{ Jaewon 20041004
			// some dirty hacks for setting a default gloss map.
			//@{ Jaewon 20050708
			// "perPixelGloss3.fx" condition added
			if(0 == strcmp(_pThis->m_sheets[i]->m_pEffect->pSharedD3dXEffect->name, "perPixelGloss2.fx")
			|| 0 == strcmp(_pThis->m_sheets[i]->m_pEffect->pSharedD3dXEffect->name, "perPixelGloss3.fx"))
			//@} Jaewon
			{
				RwTexture *pTexture = RpMaterialGetTexture(material);
				if(pTexture)
				{
					char glossMapName[64];
					strcpy(glossMapName, RwTextureGetName(pTexture));
					strcat(glossMapName, "_glo");
					RwTexture *pGlossMap = RwTextureRead(glossMapName, NULL);
					if(pGlossMap)
					{
						RpMaterialD3DFxSetTexture(material, "glossMap", pGlossMap);
						RwTextureDestroy(pGlossMap);
					}
				}
			}
			//@} Jaewon

			return material;
		}
	}
	else
	{
		success = TRUE;
	}

	if(!success)
	{
		_bAtomicFxPipelineSuccess = FALSE;
		return NULL;
	}

	return material;
}

static RpAtomic *atomicSetEffectCB(RpAtomic *atomic, void *data)
{
	_bAtomicFxPipelineSuccess = TRUE;

	RpAtomicFxEnable(atomic);
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	assert(geometry);
	//@{ Jaewon 20040922
	// bSkin -> pSkin
	_pSkin = RpSkinGeometryGetSkin(geometry);
	//@} Jaewon
	RpGeometryForAllMaterials(geometry, materialSetEffectCB, data);
	if(!_bAtomicFxPipelineSuccess)
	{
		return NULL;
	}

	return atomic;
}

static RpAtomic *atomicDisableEffectCB(RpAtomic *atomic, void *data)
{
    RpAtomicFxDisable(atomic);
    return atomic;
}

//@{ Jaewon 20050708
// A parameter 'all' added
bool FXUI::applyFx(RpClump *pClump, RwChar *path, bool all)
//@} Jaewon
{
	RwChar fxName[MAX_PATH], searchPath[MAX_PATH];
	RwUInt32 len;

	//@{ Jaewon 20050708
	// ;)
	_bApplyFxAll = all;
	//@} Jaewon

	strcpy(searchPath, path);
	len = strlen(searchPath);
	while(len >= 0)
	{
		if (searchPath[len] == '\\')
		{
			searchPath[len+1] = '\0';
			break;
		}
		len--;
	}

	strcpy(fxName, path+len+1);

	RpClumpForAllAtomics(pClump, atomicSetEffectCB, (void *)fxName);

	if(_bAtomicFxPipelineSuccess)
	{
		rebuild(pClump);

		return true;
	}
	else
	{
		RwChar msg[] = "There was an error setting the effect. Disabling all fxs. See rwdebug log for errors.";

//		RpClumpForAllAtomics(pClump, atomicDisableEffectCB, NULL);
		RsErrorMessage(msg);

		return false;
	}
}

void FXUI::resize(int w, int h)
{
	m_w = w;
	m_h = h;

	m_sheetSelectionDlg.SetSize(200, 50);
	m_sheetSelectionDlg.SetLocation(m_w - 200, 0);

	for(int i=0; i<m_sheets.size(); ++i)
	{
		m_sheets[i]->m_dlg.SetSize(200, m_h-50);
		m_sheets[i]->m_dlg.SetLocation(m_w - 200, 50);
	}
}

void FXUI::setWireframeOnSelection(bool bWireframeOnSelection)
{
	m_bWireframeOnSelection = bWireframeOnSelection;

	// if on, turn the selected flag of the current material on.
	if(m_bWireframeOnSelection && 
	   m_sheets.size() &&
	   m_sheets[m_iSelectedMaterial]->m_pEffect)
		m_sheets[m_iSelectedMaterial]->m_pEffect->selected = TRUE;

	// if off, turn the selected flag of the current material off.
	if(!m_bWireframeOnSelection && 
	   m_sheets.size() &&
	   m_sheets[m_iSelectedMaterial]->m_pEffect)
		m_sheets[m_iSelectedMaterial]->m_pEffect->selected = FALSE;
}