//=============================================================================
//    File : NPGameLib.h
//     Use : Handle Communication between Game and GameMon
//  Author : Kim Chan Ho (chkim@inca.co.kr)
//    Date : 2003/07/01 - 03
//=============================================================================

#ifndef __NPGAMELIB_H__
#define __NPGAMELIB_H__


/// Definitions
#ifdef NPGAMEDLL_EXPORTS
#define NPGAMEDLL_API __declspec(dllexport)
#else
#define NPGAMEDLL_API
#endif


// Callback Message
#define NPGAMEMON_UNDEFINED            1000        // Undefined message
#define NPGAMEMON_COMM_ERROR           1001        // Communication error
#define NPGAMEMON_COMM_CLOSE           1002        // Communication closing

#define NPGAMEMON_SPEEDHACK            1011        // SpeedHack detected
#define NPGAMEMON_GAMEHACK_KILLED      1012        // GameHack killed
#define NPGAMEMON_GAMEHACK_DETECT      1013        // GameHack detected
#define NPGAMEMON_INIT_ERROR           1014        // GameMon Init Error
#define NPGAMEMON_GAMEHACK_DOUBT       1015        // GameHack doubt
#define NPGAMEMON_CHECK_CSAUTH         1016        // CSAuth
#define NPGAMEMON_CHECK_CSAUTH2        1017        // CSAuth2

// Error Code  110 - 300
#define NPGAMEMON_ERROR_EXIST          110         // GameMon Already Exist
#define NPGAMEMON_ERROR_CREATE         111         // GameGuard Directory Create Error
#define NPGAMEMON_ERROR_NPSCAN         112         // npscan.des Error
#define NPGAMEMON_ERROR_THREAD         113         // CreateThread Error
#define NPGAMEMON_ERROR_INIT           114         // GameMon Initialize Error
#define NPGAMEMON_ERROR_GAME_EXIST     115         // Game Instance Already Exist
#define NPGAMEMON_ERROR_AUTH_INI       120         // .ini Authentication Fail
#define NPGAMEMON_ERROR_AUTH_NPGMUP    121         // npgmup.des Authentication Fail
#define NPGAMEMON_ERROR_AUTH_GAMEMON   122         // GameMon.des Authentication Fail
#define NPGAMEMON_ERROR_AUTH_NEWUP     123         // npgmup.des.new Auth Fail
#define NPGAMEMON_ERROR_AUTH_GAMEGUARD 124         // GameGuard.des Authentication Fail
#define NPGAMEMON_ERROR_AUTH_DLL       125         // NPGameDLL.dll Authentication Fail
#define NPGAMEMON_ERROR_DECRYPT        130         // .ini File Decryption Fail
#define NPGAMEMON_ERROR_CORRUPT_INI    141         // Corrupt ini file Error
#define NPGAMEMON_ERROR_CORRUPT_INI2   142         // Not match GameName in ini file Error
#define NPGAMEMON_ERROR_NFOUND_INI     150         // ini File not Found
#define NPGAMEMON_ERROR_NFOUND_NPGMUP  151         // npgmup.des not found
#define NPGAMEMON_ERROR_NFOUND_NEWUP   152         // npgmup.des.new not found
#define NPGAMEMON_ERROR_NFOUND_GG      153         // GameGuard.des not found
#define NPGAMEMON_ERROR_NFOUND_GM      154         // GameMon.des not found
#define NPGAMEMON_ERROR_CRYPTOAPI      155         // rsabase.dll is corrupted
#define NPGAMEMON_ERROR_COMM           160         // Communication Init Error
#define NPGAMEMON_ERROR_EXECUTE        170         // GameMon Execute Error
#define NPGAMEMON_ERROR_EVENT          171         // GameMon Event Create Error
#define NPGAMEMON_ERROR_NPGMUP         180         // npgmup.dll Error
#define NPGAMEMON_ERROR_MOVE_INI       191         // Move ini Error
#define NPGAMEMON_ERROR_MOVE_NEWUP     192         // Move npgmup.des.new Error

#define NPGAMEMON_ERROR_ILLEGAL_PRG    200         // Detected a illegal program

#define NPGAMEMON_ERROR_GAMEMON        210         // GameMon Init Error
#define NPGAMEMON_ERROR_SPEEDCHECK     220         // SpeedCheck Init Error
#define NPGAMEMON_ERROR_GAMEGUARD      230         // GameGuard Init Error

// Error Code  310 - 400
// Update Error Code
#define NPGMUP_ERROR_DOWNCFG           340         // Download Error
#define NPGMUP_ERROR_ABORT             350         // Canceled by User
#define NPGMUP_ERROR_AUTH              360         // File Authentication Error
#define NPGMUP_ERROR_AUTH_INI          361         // .ini Authentication Error
#define NPGMUP_ERROR_DECRYPT           370         // .ini Decrypt Error
#define NPGMUP_ERROR_CONNECT           380         // Connect to update server Fail
#define NPGMUP_ERROR_INI               390         // .ini is corrupted

#define NPGAMEMON_SUCCESS              0x755       // GameMon Initialize Success


// USER_POLYBUFFER (NPGE L5 only)
#define USER_POLYBUFFER \
	__asm _emit 0xEB \
	__asm _emit 0x04 \
	__asm _emit 0xEB \
	__asm _emit 0x05 \
	__asm _emit 0x39 \
	__asm _emit 0x19


// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	DWORD dwIndex;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;


#ifdef __cplusplus
extern "C" {
#endif

NPGAMEDLL_API DWORD __cdecl CheckNPGameMon();
NPGAMEDLL_API BOOL  __cdecl CloseNPGameMon();
NPGAMEDLL_API DWORD __cdecl InitNPGameMon();
NPGAMEDLL_API void  __cdecl SetHwndToGameMon(HWND hWnd);
NPGAMEDLL_API DWORD __cdecl PreInitNPGameMonA(LPCSTR szGameName);
NPGAMEDLL_API DWORD __cdecl PreInitNPGameMonW(LPCWSTR szGameName);
NPGAMEDLL_API BOOL  __cdecl SendUserIDToGameMonA(LPCSTR szUserID);
NPGAMEDLL_API BOOL  __cdecl SendUserIDToGameMonW(LPCWSTR szUserID);
NPGAMEDLL_API BOOL  __cdecl SendCSAuthToGameMon(DWORD dwAuth);
NPGAMEDLL_API BOOL  __cdecl SendCSAuth2ToGameMon(PGG_AUTH_DATA pAuth);
NPGAMEDLL_API LPSTR __cdecl NPDect(LPSTR lpszString);
NPGAMEDLL_API BOOL  __cdecl NPGuardData(LPVOID lpData, DWORD dwSize);
NPGAMEDLL_API BOOL  __cdecl NPReleaseData(LPVOID lpData, DWORD dwSize);
NPGAMEDLL_API LPCSTR __cdecl GetInfoFromGameMon();
NPGAMEDLL_API void  __cdecl RunFAQWebPage(DWORD dwErrcode);
NPGAMEDLL_API BOOL  __cdecl ResourceAuthA(LPCSTR lpszFilename, bool bCheckFilename /* = true */ );
NPGAMEDLL_API BOOL  __cdecl ResourceAuthW(LPCWSTR lpszFilename, bool bCheckFilename /* = true */ );

NPGAMEDLL_API DWORD __cdecl InitPacketProtect(LPCTSTR lpszUserKey);
NPGAMEDLL_API DWORD __cdecl EncryptPacket(LPVOID lpData, DWORD dwLength);
NPGAMEDLL_API DWORD __cdecl DecryptPacket(LPVOID lpData, DWORD dwLength);
NPGAMEDLL_API DWORD __cdecl EncryptPeerPacket(LPVOID lpData, DWORD dwLength);
NPGAMEDLL_API DWORD __cdecl DecryptPeerPacket(LPVOID lpData, DWORD dwLength);
NPGAMEDLL_API DWORD __cdecl PPGetLastError();

typedef BOOL (CALLBACK *NPGMCALLBACK)(DWORD dwMsg, DWORD dwArg);
NPGAMEDLL_API void  __cdecl SetCallbackToGameMon(NPGMCALLBACK fnCallback);

#ifdef __cplusplus
}
#endif


#ifdef _UNICODE
#define PreInitNPGameMon		PreInitNPGameMonW
#define SendUserIDToGameMon	SendUserIDToGameMonW
#define ResourceAuth				ResourceAuthW
#else
#define PreInitNPGameMon		PreInitNPGameMonA
#define SendUserIDToGameMon	SendUserIDToGameMonA
#define ResourceAuth				ResourceAuthA
#endif


#ifndef NPGAMELIB_C

/// CNPGameLib Wrapper Class ///
class CNPGameLib
{
public:
   explicit CNPGameLib(LPCTSTR lpszGameName)
   {
#ifdef NO_GAMEGUARD
      return;
#endif
      PreInitNPGameMon(lpszGameName);
   }
   ~CNPGameLib()
   {
#ifdef NO_GAMEGUARD
      return;
#endif
      CloseNPGameMon();
   }

public:
   DWORD Init()
   {
#ifdef NO_GAMEGUARD
      return NPGAMEMON_SUCCESS;
#endif
      return (InitNPGameMon());
   }

   void SetHwnd(HWND hWnd)
   {
#ifdef NO_GAMEGUARD
      return;
#endif
      SetHwndToGameMon(hWnd);
   }

   DWORD Check()
   {
#ifdef NO_GAMEGUARD
      return NPGAMEMON_SUCCESS;
#endif
      return (CheckNPGameMon());
   }

   DWORD Send(LPCTSTR lpszUserId)
   {
#ifdef NO_GAMEGUARD
      return NPGAMEMON_SUCCESS;
#endif
      return (SendUserIDToGameMon(lpszUserId));
   }

   LPCSTR GetInfo()
   {
#ifdef NO_GAMEGUARD
      return NULL;
#endif
      return (GetInfoFromGameMon());
   }

   DWORD Auth(DWORD dwAuth)
   {
#ifdef NO_GAMEGUARD
      return NPGAMEMON_SUCCESS;
#endif
      return (SendCSAuthToGameMon(dwAuth));
   }

   DWORD Auth2(PGG_AUTH_DATA pAuth)
   {
#ifdef NO_GAMEGUARD
      return NPGAMEMON_SUCCESS;
#endif
      return (SendCSAuth2ToGameMon(pAuth));
   }

	void RunFAQ(DWORD dwErrcode)
	{
#ifdef NO_GAMEGUARD
      return;
#endif
		RunFAQWebPage(dwErrcode);
	}

   BOOL ResAuth(LPCTSTR lpszFilename, bool bCheckFilename = true)
   {
#ifdef NO_GAMEGUARD
      return TRUE;
#endif
      return (ResourceAuth(lpszFilename, bCheckFilename));
   }

private:
   // No copies
   CNPGameLib(const CNPGameLib &rhs);
   CNPGameLib &operator=(const CNPGameLib &rhs);
};


/// 
class CNPDword
{
public:
   CNPDword();
   //CNPDword(DWORD dwSize);
   ~CNPDword();
   DWORD Get();
   void Set(DWORD dwValue);

private:
   void Guard();
   void Release();

public:
   LPDWORD m_pValue;
   DWORD m_dwSize;
};

#endif // NPGAMELIB_C


#endif // __NPGAMELIB_H__
