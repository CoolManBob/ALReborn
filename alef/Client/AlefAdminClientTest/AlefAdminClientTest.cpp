#include <windows.h>
#include <stdio.h>

//#include "AgcmAdmin.h"
#include "ApDefine.h"
#include "AgpmAdminDefine.h"
#include "AgcmAdminDlg.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPSTR lpszClass = "AlefAdminClientTest";

BOOL SearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData);


// From Lib
//AgcmAdmin g_csAgcmAdmin;

// From Lib
//AgcmAdminDlgManager g_csAgcmAdminDlgManager;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass,
										lpszClass,
										WS_OVERLAPPEDWINDOW,
										CW_USEDEFAULT,
										CW_USEDEFAULT,
										CW_USEDEFAULT,
										CW_USEDEFAULT,
										NULL,
										(HMENU)NULL,
										hInstance,
										NULL);
	ShowWindow(hWnd, nCmdShow);

	while(GetMessage(&Message, 0, 0, 0))
	{
		if(ADMIN_Common_IsDialogMessage(&Message))
		{}
		else
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_LBUTTONDOWN:
			//g_csAgcmAdmin.OpenMainDlg();
			//g_csAgcmAdminDlgManager.OpenMainDlg();
			ADMIN_Common_SetCallbackClass(NULL);

			ADMIN_Main_SetCallbackSearch(SearchCharacter);
			ADMIN_Search_SetCallbackSearch(SearchCharacter);
			ADMIN_CharacterData_SetCallbackEdit(SearchCharacter);

			ADMIN_Main_OpenDlg();

			ADMIN_Main_AddObject("울트라때이", 1111);
			ADMIN_Main_AddObject("울트라작살때이", 22222);
			ADMIN_Main_AddObject("쓰봉알뿌지직", 3333333);
			
			return 0;

		case WM_RBUTTONDOWN:
			ADMIN_Main_SetInfoText("오른쪽 버튼 눌렀을 때 메시지 가는 지..");
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL SearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpmAdminSearch* pstSearchData = (stAgpmAdminSearch*)pData;

	char szOut[255];
	sprintf(szOut, "Type : %d, Field : %d, Name : %s\n", pstSearchData->m_iType, pstSearchData->m_iField, pstSearchData->m_szSearchName);

	MessageBox(NULL, szOut, "되라!!!", MB_OK);

	return TRUE;
}