#ifndef __AGCM_PROCESS_INFO_DLG_H__
#define __AGCM_PROCESS_INFO_DLG_H__

#ifdef AGCM_EXPORT
#define AGCM_DLL __declspec(dllexport)
#else
#define AGCM_DLL __declspec(dllimport)
#endif

#ifndef	_WINDLL
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmProcessInfoDlgD" )
#else
#pragma comment ( lib , "AgcmProcessInfoDlg" )
#endif // 	_DEBUG
#endif
#endif //  _WINDLL

//class AFX_EXT_CLASS AgcmProcessInfoDlg
class AGCM_DLL AgcmProcessInfoDlg
{
public:
	AgcmProcessInfoDlg();
	virtual ~AgcmProcessInfoDlg();

	static AgcmProcessInfoDlg *GetInstance();

public:
	BOOL OpenProcessInfoDlg(CHAR *szInitInfo);
	BOOL CloseProcessInfoDlg();
	BOOL IsOpenProcessInfoDlg();

	BOOL SaveInfo(CHAR *szName);

	BOOL SetProcessInfoDlg(CHAR *szInfo);
	BOOL SetEmptyProcessInfoDlg(INT32 lMax);
};

#endif // __AGCM_PROCESS_INFO_DLG_H__