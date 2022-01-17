// -----------------------------------------------------------------------------
//              _____                   _    _____                                                                
//             |_   _|                 | |  / ____|                                                               
//   __ _ _   _  | |  _ __  _ __  _   _| |_| |      ___  _ __ ___  _ __   ___  ___  ___ _ __      ___ _ __  _ __  
//  / _` | | | | | | | '_ \| '_ \| | | | __| |     / _ \| '_ ` _ \| '_ \ / _ \/ __|/ _ \ '__|    / __| '_ \| '_ \ 
// | (_| | |_| |_| |_| | | | |_) | |_| | |_| |____| (_) | | | | | | |_) | (_) \__ \  __/ |    _ | (__| |_) | |_) |
//  \__,_|\__,_|_____|_| |_| .__/ \__,_|\__|\_____|\___/|_| |_| |_| .__/ \___/|___/\___|_|   (_) \___| .__/| .__/ 
//                         | |                                    | |                                | |   | |    
//                         |_|                                    |_|                                |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/27/2005 by daeyoung Cha
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------


#include "auInputComposer.h"
#include "ImmLib.h"

#include "magdebug.h"

#include "skeleton.h"


// -----------------------------------------------------------------------------
// IME Version 관련 macro, Enum.
// gpgstudy.com 참고.
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ( (HKL)(INT_PTR)0xE0080404 ) // New Phonetic
#define _CHT_HKL2           ( (HKL)(INT_PTR)0xE0090404 ) // New Chang Jie
#define _CHS_HKL            ( (HKL)(INT_PTR)0xE00E0804 ) // MSPY
#define MAKEIMEVERSION( major, minor ) \
    ( (DWORD)( ( (BYTE)( major ) << 24 ) | ( (BYTE)( minor ) << 16 ) ) )

#define IMEID_CHT_VER42 ( LANG_CHT | MAKEIMEVERSION( 4, 2 ) )	// New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 ( LANG_CHT | MAKEIMEVERSION( 4, 3 ) )	// New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 ( LANG_CHT | MAKEIMEVERSION( 4, 4 ) )	// New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 ( LANG_CHT | MAKEIMEVERSION( 5, 0 ) )	// New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 ( LANG_CHT | MAKEIMEVERSION( 5, 1 ) )	// New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 ( LANG_CHT | MAKEIMEVERSION( 5, 2 ) )	// New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 ( LANG_CHT | MAKEIMEVERSION( 6, 0 ) )	// New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41	( LANG_CHS | MAKEIMEVERSION( 4, 1 ) )	// MSPY1.5	// SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42	( LANG_CHS | MAKEIMEVERSION( 4, 2 ) )	// MSPY2	// Win2k/WinME
#define IMEID_CHS_VER53	( LANG_CHS | MAKEIMEVERSION( 5, 3 ) )	// MSPY3	// WinXP

enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
enum { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

#pragma warning( push )
#pragma warning( disable : 4309 )
// -----------------------------------------------------------------------------
// Indicator에 표시된 글자배열
char g_aszIndicator[5][3] =  
{
	{ 'A', 0	 },		// non
	{ 0xBC, 0xF2 },		// chs
	{ 0xC1, 0x63 },		// cht
	{ 0xC7, 0xD1 },		// kor
	{ 0x82, 0xA0 },		// jpn
};
#pragma warning( pop )

// -----------------------------------------------------------------------------
// wide-character string to a new character string. 
// The new character string is not necessarily from a multibyte character set.
int	ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len)
{
	if(text == 0) 
	{
		return WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
	} 
	else
	{
		int tLen = WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);

		if(tLen > len) 
		{
			return 0;
		} 
		else 
		{
			return WideCharToMultiByte(codePage, 0, wText, wLen, text, tLen, NULL, NULL);
		}
	}
}

// -----------------------------------------------------------------------------
// private implementation (Pimpl idiom)
class WidgetImpl 
{
public:
	WidgetImpl(){};
	~WidgetImpl(){};

	CImmLib		m_ImmLib;
};

// -----------------------------------------------------------------------------
AuInputComposer::AuInputComposer() : _pImpl(new WidgetImpl), m_ulStart(0), m_ulEnd(0), m_ImeState(IMEUI_STATE_OFF) , m_bUseRenderedIMEWindow( TRUE )
{
	SetEditPosition( 0 , 0 );

	switch( g_eServiceArea )
	{
	case	AP_SERVICE_AREA_CHINA	:
		SetRenderedIMEWindow( FALSE );
		break;
	default:
	case	AP_SERVICE_AREA_KOREA	:
	case	AP_SERVICE_AREA_WESTERN	:
	case	AP_SERVICE_AREA_JAPAN	:
		SetRenderedIMEWindow( TRUE );
		break;
	}

	m_sentence = 0;
	m_conversion = 0;
}

// -----------------------------------------------------------------------------
AuInputComposer::~AuInputComposer()
{
	delete _pImpl;
}

// -----------------------------------------------------------------------------
//singleton
AuInputComposer& AuInputComposer::GetInstance()
{
	static AuInputComposer _AuInputComposer;

	return _AuInputComposer;
}

// -----------------------------------------------------------------------------
void AuInputComposer::InitInputComposer()
{
	ASSERT(_pImpl);

	//"imm32.dll" load, funtion pointer binding
	_pImpl->m_ImmLib.Init();
}

// -----------------------------------------------------------------------------
// WM_INPUTLANGCHANGE 메세지 처리
// 사용자가 입력할 언어를 바꾸었을 때 발생. (eq. 한글->중국어, 중국어->일어)
// wParam : Specifies the character set of the new locale. 
// lParam : Input locale identifier
void AuInputComposer::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	_pImpl->m_ImmLib.SetHKL(lParam);
	m_langId = LOWORD((HKL)lParam);
	
	// AgcmFont::CreateFontRaster()에서 코드페이지 사용..
	// 몇몇글자가 깨져서 출력되던 문제 해결.. 
	m_codePage = GetCodePageFromLang(m_langId);
	//m_codePage = CP_ACP;
	m_input.resize(0);

	// Check Property
	DWORD property = _pImpl->m_ImmLib._ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicodeIME = (property & IME_PROP_UNICODE) ? true : false;

	// Update Indicator
	CheckToggleState(hWnd);

	// Update m_dwId[]
	GetImeId();

	// Bind Proc
	_pImpl->m_ImmLib.SetupImeApi(hWnd);
}

// -----------------------------------------------------------------------------
// WM_IME_COMPOSITION 메세지 처리
// 글자 조합처리
// wParam : DBCS character
// lParam : change indicator
bool AuInputComposer::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;

	//IMM context 얻기
	hImc = ImmGetContext(hWnd);
	
	if(lParam & GCS_COMPSTR)	// 글자가 조합중일 때
	{	
		char	strComposing[256];

		if(hImc) 
		{
			int tempSize = _pImpl->m_ImmLib._ImmGetCompositionStringW( hImc, GCS_COMPSTR, NULL, 0 );

			if( tempSize > 0 )
			{
				wchar_t* temp = (wchar_t*)alloca(tempSize + sizeof(wchar_t));

				_pImpl->m_ImmLib._ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);

				// Convert size in byte to size in char
				// Force terminate
				temp[tempSize / sizeof(wchar_t)] = NULL;
				m_comp.assign(temp, temp+tempSize/sizeof(wchar_t));

				// WBCS -> MBCS
				// 변환전에 char[]을 클리어한다. 없는 문자가 있을 시에 뒤에 gabage값이 붙는 버그가 있음.
				memset(strComposing, 0, sizeof(strComposing));
				ConvertString(m_codePage, &m_comp[0], m_comp.size(), strComposing, sizeof(strComposing));

				// Force terminate
				strComposing[tempSize] = NULL;

				// 조합중이던 문자 넘겨줌....
				RsKeyboardEventHandler(rsIMECOMPOSING , (void *) strComposing);
			}
			else
			{
				strComposing[tempSize] = NULL;

				// 조합중이던 문자 넘겨줌....
				RsKeyboardEventHandler(rsIMECOMPOSING , (void *) strComposing);
			}
		}
	}

	if(lParam & GCS_RESULTSTR)	// 글자가 완성되었을 때
	{
		char	strCompleted[256];

		if(hImc) 
		{ 
			 int tempSize = _pImpl->m_ImmLib._ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0);

			if(tempSize > 0)
			{
				wchar_t* temp = (wchar_t*)alloca(tempSize + sizeof(wchar_t));

				_pImpl->m_ImmLib._ImmGetCompositionStringW(hImc, GCS_RESULTSTR, temp, tempSize);
				
				// Convert size in byte to size in char
				// Force terminate
				temp[tempSize / sizeof(wchar_t)] = NULL;
				
				// 계속 덧붙이지 않고 새롭게 복사한다.
				//m_input.append(temp, temp+tempSize/sizeof(wchar_t));
				m_input.assign(temp, temp+tempSize/sizeof(wchar_t));

				// WBCS -> MBCS
				// 변환전에 char[]을 클리어한다. 없는 문자가 있을 시에 뒤에 gabage값이 붙는 버그가 있음.
				memset(strCompleted, 0, sizeof(strCompleted));
				ConvertString(m_codePage, &m_input[0], m_input.size(), strCompleted, sizeof(strCompleted));
				
				// Force terminate
				strCompleted[tempSize] = NULL;

				// 조합중이던 문자 제거..
				RsKeyboardEventHandler(rsIMECOMPOSING , "");

				// 윈도우로 글자를 넘긴다..안에서 처리해야함..
				RsKeyboardEventHandler(rsCHAR , (void *) strCompleted);
			}
			else
			{
				//no update
			}
		}
	}
	if(lParam & GCS_COMPATTR)	// Retrieves or updates the attribute of the composition string
	{
		if(hImc) 
		{ 
			int tempSize = _pImpl->m_ImmLib._ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);
			//BYTE temp[10] = {0,};

			BYTE* temp = (BYTE*)alloca(tempSize);

			_pImpl->m_ImmLib._ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, tempSize);


			int start, end;
			for(start=0; start < tempSize; ++start) 
			{
				if(temp[start] == ATTR_TARGET_CONVERTED || temp[start] == ATTR_TARGET_NOTCONVERTED) 
					break;
			}
			for(end=start; end < tempSize; ++end)
			{
				if(temp[end]!=temp[start]) 
					break;
			}

			m_ulStart	= start;
			m_ulEnd		= end;
		}
	}

	ImmReleaseContext(hWnd, hImc);

	return true;
}

// -----------------------------------------------------------------------------
// WM_IME_ENDCOMPOSITION
bool AuInputComposer::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);

	RsEventHandler( rsIMEEndComposition , NULL );

	return true;
}


// -----------------------------------------------------------------------------
// WM_IME_NOTIFY
bool AuInputComposer::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;

	if( m_bUseRenderedIMEWindow )
	{
		switch (wParam) 
		{
		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
			//candidate window character update & trigger event
			hImc = _pImpl->m_ImmLib._ImmGetContext(hWnd);
			if(hImc) 
			{ 
				m_reading.resize(0);

				int candidateLen = _pImpl->m_ImmLib._ImmGetCandidateListW(hImc, 0, NULL, 0);

				if(candidateLen > 0) 
				{ 
					m_candidate.resize(candidateLen);

					_pImpl->m_ImmLib._ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)&m_candidate[0], candidateLen);
				}

				_pImpl->m_ImmLib._ImmReleaseContext(hWnd, hImc);
			}

			RsKeyboardEventHandler( rsIMEShowCandidate,	" ");

			return true;

		case IMN_CLOSECANDIDATE:
			m_candidate.resize(0);

			//reading window 감추기
			RsKeyboardEventHandler( rsIMEHideReadingWindow,	" ");
			RsKeyboardEventHandler( rsIMEHideCandidate,	" ");

			return true;

		case IMN_SETCONVERSIONMODE:
		case IMN_SETOPENSTATUS:
			CheckToggleState(hWnd);
			return false;

		case IMN_PRIVATE:

			//reading string update & trigger event
			GetPrivateReadingString(hWnd);

			// Trap some messages to hide reading window
			switch( m_dwId[0] )
			{
				case IMEID_CHT_VER42:
				case IMEID_CHT_VER43:
				case IMEID_CHT_VER44:
				case IMEID_CHS_VER41:
				case IMEID_CHS_VER42:
					if((lParam==1)||(lParam==2)) 
					{
						RsKeyboardEventHandler( rsIMEShowReadingWindow,	" ");
						return true;
					}
					break;

				case IMEID_CHT_VER50:
				case IMEID_CHT_VER51:
				case IMEID_CHT_VER52:
				case IMEID_CHT_VER60:
				case IMEID_CHS_VER53:
					if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) 
					{
						RsKeyboardEventHandler( rsIMEShowReadingWindow,	" ");
						return true;
					}
					break;
			}
			break;
		}
	}
	else
	{
	switch (wParam) 
	{
		case IMN_OPENCANDIDATE:

			// IME창의 위치를 조절한다.
			hImc = _pImpl->m_ImmLib._ImmGetContext(hWnd);
			if(hImc) 
			{ 
				CANDIDATEFORM cf;
				ImmGetCandidateWindow( hImc , 0 , &cf );
				cf.ptCurrentPos = m_ptEditPosition;
				ImmSetCandidateWindow( hImc , &cf );

				COMPOSITIONFORM compositionform;

				ImmGetCompositionWindow( hImc , &compositionform );
				compositionform.ptCurrentPos = m_ptEditPosition;
				ImmSetCompositionWindow( hImc , &compositionform );
			}
			return false;
		case IMN_OPENSTATUSWINDOW:
			// IME창의 위치를 조절한다.
			hImc = _pImpl->m_ImmLib._ImmGetContext(hWnd);
			if(hImc) 
			{ 
				ImmSetStatusWindowPos( hImc , &m_ptEditPosition );
			}
			return false;

		case IMN_CHANGECANDIDATE:
			//candidate window character update & trigger event
			return false;

		case IMN_CLOSECANDIDATE:
			m_candidate.resize(0);

			//reading window 감추기
			RsKeyboardEventHandler( rsIMEHideReadingWindow,	" ");
			RsKeyboardEventHandler( rsIMEHideCandidate,	" ");

			return false;

		case IMN_SETCONVERSIONMODE:
		case IMN_SETOPENSTATUS:
			CheckToggleState(hWnd);
			return false;

		case IMN_PRIVATE:

			//reading string update & trigger event
			GetPrivateReadingString(hWnd);

			// Trap some messages to hide reading window
			switch( m_dwId[0] )
			{
				case IMEID_CHT_VER42:
				case IMEID_CHT_VER43:
				case IMEID_CHT_VER44:
				case IMEID_CHS_VER41:
				case IMEID_CHS_VER42:
					if((lParam==1)||(lParam==2)) 
					{
						RsKeyboardEventHandler( rsIMEShowReadingWindow,	" ");
						return false;
					}
					break;

				case IMEID_CHT_VER50:
				case IMEID_CHT_VER51:
				case IMEID_CHT_VER52:
				case IMEID_CHT_VER60:
				case IMEID_CHS_VER53:
					if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) 
					{
						RsKeyboardEventHandler( rsIMEShowReadingWindow,	" ");
						return false;
					}
					break;
			}
			break;
		}
		
	}

	return false;
}

// -----------------------------------------------------------------------------
// WM_CHAR
bool AuInputComposer::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam) 
	{
	case '\r':
	case '\n':
		m_input.resize(0);
		break;
	case '\b':
		if(m_input.size()) m_input.resize(m_input.size() - 1);
		break;
	case '\t':
	case 27:
		break;
	default:

		if(wParam > 31) 
		{
			wchar_t temp;
			MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);

			m_input.push_back(temp);
		}

		break;
	}

	// 문자열 포인터를 넘긴다..
	// 기존에 사용되었던 방식으로도 처리한다.
	//char	strCompleted[ 256 ];
	//memset(strCompleted, 0, sizeof(strCompleted));
	//wsprintf( strCompleted , "%c" , wParam );

	char	strCompleted[ 2 ] = { 0, };
	//memset(strCompleted, 0, sizeof(strCompleted));
	//wsprintf( strCompleted , "%c" , wParam );
	strCompleted[0] = (char)wParam;

	RsKeyboardEventHandler( rsIMECOMPOSING , "" );
	RsKeyboardEventHandler( rsCHAR, (void *) strCompleted );

	return true;
}

// -----------------------------------------------------------------------------
int		AuInputComposer::GetInput(char* text, int len)
{
	return ConvertString(m_codePage, m_input.c_str(), m_input.size(), text, len);
}

// -----------------------------------------------------------------------------
int		AuInputComposer::GetComp(char* text, int len)
{
	return ConvertString(m_codePage, m_comp.c_str(), m_comp.size(), text, len);
}

// -----------------------------------------------------------------------------
// 현재 화면에 뿌려질 reading window의 내용 변환 WBCS -> MBCS
// AcUIEdit::OnIMEReadingWDraw()에서 사용.
int	AuInputComposer::GetReading(char* text, int len)
{
	return ConvertString(m_codePage, m_reading.c_str(), m_reading.size(), text, len);
}

// -----------------------------------------------------------------------------
// Index에 따른 Candidate의 문자 얻기.
int	AuInputComposer::GetCandidate(DWORD index, char* text, int len) 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) 
		{
			return 0;
		} 
		else 
		{
			if(m_bUnicodeIME) //유니코드를 사용한다면
			{	
				wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);

				return ConvertString(m_codePage, wText, wcslen(wText), text, len);

			} else 
			{
				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) 
				{
					return strlen(temp);
				} 
				else 
				{
					int tempLen = strlen(temp);

					if(len < tempLen) 
					{
						return 0;
					} 
					else 
					{
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------
// candidate의 갯수 반환
int	AuInputComposer::GetCandidateCount() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
	}
}

// -----------------------------------------------------------------------------
// 선택된 candidate 반환. PRIMARYLANGID가 LANG_KOREAN이면 count반환
int	AuInputComposer::GetCandidateSelection() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		if(PRIMARYLANGID(m_langId) == LANG_KOREAN)
			return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
		else
			return ((CANDIDATELIST*)&m_candidate[0])->dwSelection;
	}
}

// -----------------------------------------------------------------------------
int	AuInputComposer::GetCandidatePageSize() 
{
	if(m_candidate.empty())
	{
		return 0;
	} 
	else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageSize;
	}
}

// -----------------------------------------------------------------------------
int	AuInputComposer::GetCandidatePageStart() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageStart;
	}
}

// -----------------------------------------------------------------------------
char*	AuInputComposer::GetNextChar(const char* pTemp)
{
	return CharNextExA(m_codePage, pTemp, 0);
}

// -----------------------------------------------------------------------------
// Reading window가 horison인지 아닌지 반환
// DirectX 9.0 summer2004 CustomUI예제에서 발췌..
bool AuInputComposer::GetReadingWindowOrientation()
{
	HKL hkl = _pImpl->m_ImmLib.GetHKL();

    bool bHorizontalReading = ( hkl == _CHS_HKL ) || ( hkl == _CHT_HKL2 ) || ( m_dwId[0] == 0 );
    if( !bHorizontalReading && ( m_dwId[0] & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = m_dwId[0] & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );

        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

// -----------------------------------------------------------------------------
void AuInputComposer::GetPrivateReadingString(HWND hWnd)
{
    if( !m_dwId[0] ) 
	{
        m_reading.resize(0);
        return;
    }

	HIMC hImc = _pImpl->m_ImmLib._ImmGetContext(hWnd);
    if( !hImc ) 
	{
        m_reading.resize(0);
        return;
    }

    
    DWORD dwErr = 0;

    if( _pImpl->m_ImmLib._GetReadingString ) 
	{

        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        int wstrLen = _pImpl->m_ImmLib._GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );

		if( wstrLen == 0 ) 
		{
			m_reading.resize(0);
		} 
		else 
		{
			wchar_t *wstr = (wchar_t*)alloca(sizeof(wchar_t) * wstrLen);
            _pImpl->m_ImmLib._GetReadingString( hImc, wstrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
			m_reading.assign(wstr, wstr+wstrLen);
		}

		m_bVerticalReading = bVertical ? true : false;

		_pImpl->m_ImmLib._ImmReleaseContext(hWnd, hImc);

    } 
	else 
	{
        // IMEs that doesn't implement Reading String API
		//@{ 2006/05/02 burumal
		/*
		wchar_t* temp;
		DWORD tempLen;
		*/
		wchar_t* temp = NULL;
		DWORD tempLen = 0;
		//@}
	    bool bUnicodeIme = false;
		INPUTCONTEXT *lpIC = _pImpl->m_ImmLib._ImmLockIMC(hImc);

		if(lpIC == NULL) 
		{
			temp = NULL;
			tempLen = 0;
		} 
		else 
		{
			LPBYTE p = 0;
			switch( m_dwId[0] )
			{
				case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
				case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
				case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
					p = *(LPBYTE *)((LPBYTE)_pImpl->m_ImmLib._ImmLockIMCC( lpIC->hPrivate ) + 24 );
					if( !p ) break;
					tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
					dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
					temp = (wchar_t *)( p + 56 );
					bUnicodeIme = true;
					break;

				case IMEID_CHT_VER50: // 5.0.x.x // WinME
					p = *(LPBYTE *)( (LPBYTE)_pImpl->m_ImmLib._ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
					if( !p ) break;
					p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
					temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = false;
					break;

				case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
				case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
				case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
					p = *(LPBYTE *)((LPBYTE)_pImpl->m_ImmLib._ImmLockIMCC( lpIC->hPrivate ) + 4);
					if( !p ) break;
					p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
					temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = true;
					break;

				// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
				case IMEID_CHS_VER41:
					{
						int nOffset;
						nOffset = ( m_dwId[1] >= 0x00000002 ) ? 8 : 7;

						p = *(LPBYTE *)((LPBYTE)_pImpl->m_ImmLib._ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
						dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
						dwErr = min( dwErr, tempLen );
						temp = (wchar_t *)(p + 6*4 + 16*2*1);
						bUnicodeIme = true;
					}
					break;

				case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
					{
						OSVERSIONINFOA osi;
						osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
						GetVersionExA( &osi );

						int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
						p = *(LPBYTE *)((LPBYTE)_pImpl->m_ImmLib._ImmLockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
					}
					break;

				default:
					temp = NULL;
					tempLen = 0;
					break;
			}
		}

		if(tempLen == 0)
		{
			m_reading.resize(0);
		} 
		else 
		{
			if( bUnicodeIme ) 
			{
				m_reading.assign(temp, tempLen);
			} 
			else 
			{
				int wstrLen = MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0); 
				wchar_t* wstr = (wchar_t*)alloca(sizeof(wchar_t)*wstrLen);
				MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, wstr, wstrLen); 
				m_reading.assign(wstr, wstrLen);
			}
		}

		_pImpl->m_ImmLib._ImmUnlockIMCC(lpIC->hPrivate);
		_pImpl->m_ImmLib._ImmUnlockIMC(hImc);

		m_bVerticalReading = !GetReadingWindowOrientation();
    }

	_pImpl->m_ImmLib._ImmReleaseContext(hWnd, hImc);
}

// -----------------------------------------------------------------------------
void AuInputComposer::GetImeId()
{
    char    szTmp[1024];

	m_dwId[0] = m_dwId[1] = 0;

	HKL& _hkl = _pImpl->m_ImmLib.GetHKL();

    if(!((_hkl==_CHT_HKL) || (_hkl==_CHT_HKL2) || (_hkl==_CHS_HKL)))
		return;
        
	if ( ! _pImpl->m_ImmLib._ImmGetIMEFileNameA( _hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
        return;

    if ( ! _pImpl->m_ImmLib._GetReadingString ) 
	{
        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) 
		{
	        return;
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSize( szTmp, &dwVerHandle );

    if( dwVerSize )
	{
        LPVOID  lpVerBuffer = alloca( dwVerSize );

        if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) 
		{
			LPVOID  lpVerData;
			UINT    cbVerData;

            if( VerQueryValue( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) 
			{
                DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
                dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
                if(  _pImpl->m_ImmLib._GetReadingString
                    ||
                    ( m_langId == LANG_CHT &&
                        ( dwVer == MAKEIMEVERSION(4, 2) || 
                        dwVer == MAKEIMEVERSION(4, 3) || 
                        dwVer == MAKEIMEVERSION(4, 4) || 
                        dwVer == MAKEIMEVERSION(5, 0) ||
                        dwVer == MAKEIMEVERSION(5, 1) ||
                        dwVer == MAKEIMEVERSION(5, 2) ||
                        dwVer == MAKEIMEVERSION(6, 0) ) )
                    ||
                    ( m_langId == LANG_CHS &&
                        ( dwVer == MAKEIMEVERSION(4, 1) ||
                        dwVer == MAKEIMEVERSION(4, 2) ||
                        dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

                    m_dwId[0] = dwVer | m_langId;
                    m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
                }
            }
        }
    }
}


// -----------------------------------------------------------------------------
void AuInputComposer::CheckToggleState(HWND hWnd)
{
	//Update Indicator
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

		// toggle vertical status
		m_bVerticalCandidate = !m_bVerticalCandidate;

		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            //m_bVerticalCandidate = m_dwId[0] == 0;
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

    default:
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == g_aszIndicator[INDICATOR_NON_IME] )
    {
        char szLang[10];
        GetLocaleInfoA( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, sizeof(szLang) );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = ( char ) towlower( szLang[1] );
    }


	//Check Toggle State
	bool bIme = ImmIsIME( _pImpl->m_ImmLib.GetHKL() ) != 0;

	HIMC hImc = _pImpl->m_ImmLib._ImmGetContext(hWnd);
    if( hImc )
	{
        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) 
		{
            DWORD dwConvMode, dwSentMode;
			_pImpl->m_ImmLib._ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

        } 
		else 
		{
            m_ImeState = ( bIme && _pImpl->m_ImmLib._ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }
		_pImpl->m_ImmLib._ImmReleaseContext(hWnd, hImc);
        
	} 
	else 
	{

        m_ImeState = IMEUI_STATE_OFF;
	}
}

// -----------------------------------------------------------------------------
// 현재 IME에 설정된 언어에 따른 character set을 반환
int	AuInputComposer::GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

// -----------------------------------------------------------------------------
// character set으로 codepage를 반환한다.
int	AuInputComposer::GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	default:
		return 1252;
	}
}

// -----------------------------------------------------------------------------
// language ID로 codepage를 반환한다.
int	AuInputComposer::GetCodePageFromLang( LANGID langid )
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}

void AuInputComposer::ToAlphaNumeric( HWND hWnd )
{
	HIMC hImc = ImmGetContext(hWnd);
	
	ImmGetConversionStatus( hImc, &m_conversion, &m_sentence );

	ImmSetConversionStatus( hImc, 0, IME_SMODE_NONE );
	
	ImmReleaseContext( hWnd, hImc );
}

void AuInputComposer::ToNative( HWND hWnd )
{
	HIMC hImc = ImmGetContext(hWnd);
	
	ImmSetConversionStatus( hImc, m_conversion, m_sentence );

	ImmReleaseContext( hWnd, hImc );
}

// -----------------------------------------------------------------------------
// auInputComposer.cpp - End of file
// -----------------------------------------------------------------------------
