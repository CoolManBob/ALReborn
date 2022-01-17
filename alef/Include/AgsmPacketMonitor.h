
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AGSMPACKETMONITOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AGSMPACKETMONITOR_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AGSMPACKETMONITOR_EXPORTS
#define AGSMPACKETMONITOR_API __declspec(dllexport)
#else
#define AGSMPACKETMONITOR_API __declspec(dllimport)
#endif

#include "ApBase.h"
#include "AgsEngine.h"
#include "AuPacket.h"

typedef enum
{
	AGSMPACKETMONITOR_PACKET_TYPE_SEND = 0,
	AGSMPACKETMONITOR_PACKET_TYPE_RECV = 1,
} AgsmPMPacketType;

// This class is exported from the AgsmPacketMonitor.dll
class AGSMPACKETMONITOR_API AgsmPacketMonitor : public AgsModule
{
private:
//	AuProfileManager *	m_pcsProfile;

	INT32			m_ulLastTick;
	BOOL			m_bInit;

	HANDLE			m_hThread;
	DWORD			m_ulThreadID;

	static BOOL		m_bDestroy;

	static HWND		m_hDialog;
	static HWND		m_hList;
	static HWND		m_hRadioSend;
	static HWND		m_hRadioRecv;

	static AgsmPMPacketType		m_ePacketType;

	static AgsmPacketMonitor *	m_pThis;

public:
	static HINSTANCE		m_hInstance;

public:
	AgsmPacketMonitor();
	~AgsmPacketMonitor();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	static VOID				UpdateStatistics(BOOL bChangeEntry);

	static BOOL CALLBACK	MonitorProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI		DialogThread(PVOID pvParam);
	static BOOL				SetupDialog(HWND hDlg);
};

