//@{ Jaewon 20040826
// created.
// a fx user interface class for MatD3DFxTool.
//@} Jaewon

#ifndef __MATD3DFXTOOL_FXUI_H__
#define __MATD3DFXTOOL_FXUI_H__

#include "DXUTgui\DXUTgui.h"
#include "vector"
#include <rwcore.h>
#include <rpworld.h>
#include "AcuRpMatD3DFx.h"

//@{ Jaewon 20041102
// lpszInitialDir added.
int	doFileDlg(bool bOpenFileDlg, 
			  LPCTSTR lpszDefExt,
			  LPTSTR lpszFileName,
			  DWORD dwFlags,
			  LPCTSTR lpszFilter,
			  HWND hParentWnd,
			  LPTSTR lpszFileTitle,
			  LPCTSTR lpszTitle,
			  LPTSTR lpszInitialDir);
//@} Jaewon

class FXUI
{
public:
	// constructor
	FXUI();
	// destructor
	~FXUI();

	void clear();

	void findClosestMaterialToPosition(RpClump *pClump, RwCamera *pCamera, RwV2d *pPos);

	//@{ Jaewon 20050825
	// This returns true if the clump has fx.
	bool rebuild(RpClump *clump);
	//@} Jaewon

	void render(float fElapsedTime);

	//@{ Jaewon 20050708
	// A parameter 'all' added
	bool applyFx(RpClump *pClump, RwChar *path, bool all);
	//@} Jaewon

	bool msgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void resize(int w, int h);

	void setWireframeOnSelection(bool bWireframeOnSelection);
	
	struct Control
	{
		// DXUT control id
		INT m_id;
		// widget type
		char m_widget[32];
		// real data to be tweaked
		void *m_data;
		// parameter name to be tweaked
		char m_name[FX_HANDLE_NAME_LENGTH];
		// the whole vector data if m_data represents an element of a vector.
		D3DXVECTOR4 *m_vdata;
	};
	struct Sheet
	{
		CDXUTDialog m_dlg;
		RpMaterial *m_pMaterial;
		DxEffect* m_pEffect;
		// controls
		std::vector<Control*> m_controls;
		// this sheet should be rebuilt?
		bool m_bNeedUpdate;
		// control ids for the tweakable parameter which has a RenderWare semantic.
		// 0 - base texture
		// 1 - material ambient
		// 2 - material diffuse
		// 3 - material specular
		INT m_rwControls[4];
	};

	// dialog sheets for each material
	std::vector<Sheet*> m_sheets;

	// index of currently selected material
	int m_iSelectedMaterial;

	// viewport dimension
	int m_w, m_h;

	// a main dialog of the sheet selection
	CDXUTDialog m_sheetSelectionDlg;

	bool m_bWireframeOnSelection;

	// // add controls for tweakable RenderWare parameters, if any.
	int addRwParamControls(Sheet *pSheet);
};

#endif

