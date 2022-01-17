#ifndef __PATCH_GET_DISPLAY_INFO__
#define __PATCH_GET_DISPLAY_INFO__

#pragma once

// GetDisplayInfo.h
//
// Get a display adapter & driver information and screen resolution
//
// nhn corporation
// 20041013, lqez / engine development unit
//

#include <stdlib.h>
#include <windows.h>
#include <shlwapi.h>
#include <ddraw.h>
#include <math.h>

#include "PatchClientRegistry.h"

// *** NOTICE ***
// add version.lib, ddraw.lib and dxguid.lib into project settings, please.

namespace Hangame
{
	#define MAX_LENGTH_OF_VIDEOINFO	256

	class DisplayInfo
	{
	public:
		DisplayInfo()
		{
			Check();
		}

	public:
		unsigned int m_nScreenWidth;
		unsigned int m_nScreenHeight;
		unsigned int m_nVideoMemory;

		char m_szVendor			[MAX_LENGTH_OF_VIDEOINFO];
		char m_szChipType		[MAX_LENGTH_OF_VIDEOINFO];
		char m_szAdapterName	[MAX_LENGTH_OF_VIDEOINFO];
		char m_szDACType		[MAX_LENGTH_OF_VIDEOINFO];
		char m_szDriverName		[MAX_LENGTH_OF_VIDEOINFO];
		char m_szDriverDate		[MAX_LENGTH_OF_VIDEOINFO];
		char m_szDriverVersion	[MAX_LENGTH_OF_VIDEOINFO];

	protected:
		int GetVRAMwithDXInterface()
		{
			HRESULT hr;
			DDSCAPS2 ddsCaps;
			ZeroMemory(&ddsCaps, sizeof(ddsCaps));
			ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
			DWORD dwUsedVRAM = 0;
			DWORD dwTotal = 0;
			DWORD dwFree = 0;

			// lp_DD points to the IDirectDraw object	
			LPDIRECTDRAW7 lp_DD;
			hr = ::DirectDrawCreateEx( NULL, (VOID**)&lp_DD, IID_IDirectDraw7, NULL );
			
			if ( hr != S_OK )
				return 0;

			hr = lp_DD->GetAvailableVidMem(&ddsCaps, &dwTotal, &dwFree);

			dwTotal /= 1024*1024;

			// snap to 2^n
			int n = 0;
			while( pow( (double) 2, n++ ) < dwTotal );

			dwTotal = static_cast<int>(pow( (double) 2, (n-1) ));

			return( dwTotal );
		}

		bool GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, BOOL bPrimaryEnough/*= FALSE*/)
		{
			LPWORD lpwData;

			for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
			{
				if (*lpwData == wLangId)
				{
					dwId = *((DWORD*)lpwData);
					return TRUE;
				}
			}

			if (!bPrimaryEnough)
				return FALSE;

			for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
			{
				if (((*lpwData)&0x00FF) == (wLangId&0x00FF))
				{
					dwId = *((DWORD*)lpwData);
					return TRUE;
				}
			}

			return FALSE;
		}

		bool GetDLLInfo()
		{
			DWORD dwFileVersionMS = 0;
			DWORD dwFileVersionLS = 0;
			BOOL bRet;

			DWORD dwHandle;
			DWORD dwLen = GetFileVersionInfoSize ( m_szDriverName, &dwHandle );
			if ( dwLen == 0 )
			{
				return FALSE;
			}

			BYTE* pVerInfo = new BYTE[dwLen];
			bRet = GetFileVersionInfo ( m_szDriverName, dwHandle, dwLen, pVerInfo );
			if ( !bRet )
			{
				return FALSE;
			}

			UINT uLen;
			BYTE *pBuffer;
			bRet = VerQueryValue ( pVerInfo, "\\", (LPVOID*)&pBuffer, &uLen );
			if ( !bRet )
			{
				delete[] pVerInfo;
				return FALSE;
			}
		    
			dwFileVersionMS = ((VS_FIXEDFILEINFO*)pBuffer)->dwFileVersionMS; 
			dwFileVersionLS = ((VS_FIXEDFILEINFO*)pBuffer)->dwFileVersionLS; 

		    
			sprintf( m_szDriverVersion, "%d.%d.%d.%d", HIWORD(dwFileVersionMS), LOWORD(dwFileVersionMS),
													   HIWORD(dwFileVersionLS), LOWORD(dwFileVersionLS) );


			bRet = VerQueryValue(pVerInfo, "\\VarFileInfo\\Translation", (LPVOID*)&pBuffer, &uLen);
			if ( !bRet )
			{
				delete[] pVerInfo;
				return FALSE;
			}
			
			DWORD	dwLangCode = 0;
			if (!GetTranslationId(pBuffer, uLen, GetUserDefaultLangID(), dwLangCode, FALSE))
			{
				if (!GetTranslationId(pBuffer, uLen, GetUserDefaultLangID(), dwLangCode, TRUE))
				{
					if (!GetTranslationId(pBuffer, uLen, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), dwLangCode, TRUE))
					{
						if (!GetTranslationId(pBuffer, uLen, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL), dwLangCode, TRUE))
							dwLangCode = *((DWORD*)pBuffer);
					}
				}
			}
			
			TCHAR szSubBlock[512]; 
			TCHAR szSubBlockString[512]; 
			sprintf( szSubBlock, "\\StringFileInfo\\%04X%04X\\", dwLangCode&0x0000FFFF, (dwLangCode&0xFFFF0000)>>16);
			sprintf( szSubBlockString, "%sCompanyName", szSubBlock );
			
			if (VerQueryValue(pVerInfo, (LPTSTR)(LPCTSTR)(szSubBlockString), (LPVOID*)&pBuffer, &uLen))
				sprintf( m_szVendor, "%s", (LPCTSTR)pBuffer );


			delete[] pVerInfo;


			
			TCHAR szFullPathAndFilename[512]; 
			WIN32_FILE_ATTRIBUTE_DATA fInfo;
			SYSTEMTIME filetime;
			sprintf( szFullPathAndFilename, "%s\\system32\\%s", getenv("windir"), m_szDriverName ); // WIN NT

			bRet = GetFileAttributesEx( szFullPathAndFilename, GetFileExInfoStandard, &fInfo );

			if( !bRet ) 
			{
				sprintf( szFullPathAndFilename, "%s\\system\\%s", getenv("windir"), m_szDriverName );	// WIN 95, 98
				bRet = GetFileAttributesEx( szFullPathAndFilename, GetFileExInfoStandard, &fInfo );

				if( !bRet )
					return FALSE;
			}

			FileTimeToSystemTime( &fInfo.ftLastWriteTime, &filetime );

			sprintf( m_szDriverDate, "%4d-%02d-%02d %02d:%02d:%02d",filetime.wYear,
																	filetime.wMonth,
																	filetime.wDay,
																	filetime.wHour,
																	filetime.wMinute,
																	filetime.wSecond );

			return TRUE;
		}

		bool GetNTInfo()
		{
			HKEY hRegKey; 
			TCHAR szKey[512]; 
			TCHAR szQueryValue[512]; 
			DWORD dwType; 
			DWORD dwLen; 
			LONG lResult; 

			sprintf( szKey, "HARDWARE\\DEVICEMAP\\VIDEO" ); 

			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE, &hRegKey ) ) 
				return false;

			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "\\Device\\Video0", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;

			RegCloseKey( hRegKey );

            sprintf( szKey, "%s", &szQueryValue[18] );

			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE, &hRegKey ) ) 
				return false;
		
			// Device Description
			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "Device Description", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;
			
			sprintf( m_szAdapterName, "%s", szQueryValue );
		
			// HardwareInformation.MemorySize
			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "HardwareInformation.MemorySize", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;
			
			m_nVideoMemory = szQueryValue[3] * 16;
		
			// HardwareInformation.ChipType
			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "HardwareInformation.ChipType", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;

			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szQueryValue, -1, m_szChipType, MAX_LENGTH_OF_VIDEOINFO, NULL, NULL);

			// InstalledDisplayDrivers
			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "InstalledDisplayDrivers", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;

			sprintf( m_szDriverName, "%s.dll", szQueryValue );

			// HardwareInformation.DacType
			dwLen = sizeof(szQueryValue);
			lResult = RegQueryValueEx( hRegKey, "HardwareInformation.DacType", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

			if( lResult != ERROR_SUCCESS ) 
				return false;

			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szQueryValue, -1, m_szDACType, MAX_LENGTH_OF_VIDEOINFO, NULL, NULL);

			RegCloseKey( hRegKey );
			
			return true;
		}

		bool Get9XInfo()
		{
			HKEY hRegKey; 
			HKEY hRegKey2; 
			HKEY hRegKey3; 
			HKEY hRegKey4; 
			TCHAR szKey[512]; 
			TCHAR szKey2[512]; 
			TCHAR szKey3[512]; 
			TCHAR szKey4[512]; 
			TCHAR szQueryValue[512];
			DWORD dwType; 
			DWORD dwLen;
			DWORD dwIndexEnum;
			LONG lResult;
			FILETIME MyFileTime;

			sprintf( szKey, "Config Manager\\Enum" ); 

			if( lResult = RegOpenKeyEx( HKEY_DYN_DATA, szKey, 0, KEY_READ, &hRegKey ) != ERROR_SUCCESS )
				return false;

			{
				dwLen = 255;
				dwIndexEnum = 0;

				// enum system devices
				while ((lResult = RegEnumKeyEx( hRegKey, dwIndexEnum++, szQueryValue, &(dwLen = sizeof(szQueryValue)), 0, NULL, 0, &MyFileTime)) == ERROR_SUCCESS )
				{
					sprintf( szKey2, "%s\\%s", szKey, szQueryValue );

					if ( lResult = RegOpenKeyEx( HKEY_DYN_DATA, szKey2, 0, KEY_READ, &hRegKey2) != ERROR_SUCCESS )
						return false;

					// get hardward key
					dwLen = sizeof(szQueryValue);
					if ( lResult = RegQueryValueEx( hRegKey2, "HardWareKey", 0, &dwType, (LPBYTE)szQueryValue, &dwLen) != ERROR_SUCCESS )
						continue;

					sprintf( szKey3, "Enum\\%s", szQueryValue ); 

					// open device enum key
					if( lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey3, 0, KEY_READ, &hRegKey3 ) != ERROR_SUCCESS ) 
						continue;

					// get class id and compare it
					dwLen = sizeof(szQueryValue);
					if ( lResult = RegQueryValueEx( hRegKey3, "Class", 0, &dwType, (LPBYTE)szQueryValue, &dwLen) != ERROR_SUCCESS )
						continue;
					
					if ( _stricmp( "DISPLAY", szQueryValue ) != 0 )
						continue;

					// Device Description
					dwLen = sizeof(szQueryValue);
					lResult = RegQueryValueEx( hRegKey3, "DeviceDesc", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

					if( lResult != ERROR_SUCCESS ) 
						continue;
					
					sprintf( m_szAdapterName, "%s", szQueryValue );

					// get driver key
					dwLen = sizeof(szQueryValue);
					lResult = RegQueryValueEx( hRegKey3, "Driver", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

					if( lResult != ERROR_SUCCESS ) 
						continue;
					
					// open driver key
					sprintf( szKey3, "System\\CurrentControlSet\\Services\\Class\\%s", szQueryValue ); 

					if ( lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey3, 0, KEY_READ, &hRegKey4) != ERROR_SUCCESS )
						continue;	

					// Chip ( maybe same as AdapterName )
					dwLen = sizeof(szQueryValue);
					lResult = RegQueryValueEx( hRegKey4, "DriverDesc", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

					if( lResult != ERROR_SUCCESS ) 
						continue;
					
					sprintf( m_szChipType, "%s", szQueryValue );

					// open Default key
					RegCloseKey( hRegKey4 );

					sprintf( szKey4, "%s\\DEFAULT", szKey3 ); 

					if ( lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, szKey4, 0, KEY_READ, &hRegKey4) != ERROR_SUCCESS )
						continue;	

					// Driver Name
					dwLen = sizeof(szQueryValue);
					lResult = RegQueryValueEx( hRegKey4, "drv", 0, &dwType, (LPBYTE)&szQueryValue, &dwLen ); 

					if( lResult != ERROR_SUCCESS ) 
						continue;
	
					sprintf( m_szDriverName, "%s", szQueryValue );

					RegCloseKey( hRegKey3 );
					RegCloseKey( hRegKey2 );
					RegCloseKey( hRegKey );

					return true;
				}
				RegCloseKey( hRegKey );
			}
		
			return false;
		}

		void Check()
		{
			m_nVideoMemory = 0;
			m_szVendor[0] = '\0';
			m_szAdapterName[0] = '\0';
			m_szChipType[0] = '\0';
			m_szDACType[0] = '\0';
			m_szDriverName[0] = '\0';
			m_szDriverDate[0] = '\0';
			m_szDriverVersion[0] = '\0';

			m_nScreenWidth = GetSystemMetrics ( SM_CXSCREEN );
			m_nScreenHeight = GetSystemMetrics ( SM_CYSCREEN );

			bool bSuccess = false;

			bSuccess = GetNTInfo();

			if( !bSuccess )
			{
				bSuccess = Get9XInfo();
				m_nVideoMemory = GetVRAMwithDXInterface();	// because win9X has no vram information in registry
			}

			if ( bSuccess )
				GetDLLInfo();

		}
	};
};

#endif