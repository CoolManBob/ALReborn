//////////////////////////////////////////////////////////////////////
// libHGPSHELP.hpp
//
// Copyright (C) 2008 NHN Japan Corp. All rights reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// <usage language="c++">
// #include "libHGPSHELP.hpp"
//
// extern int RealMain(); // Your function.
//
// int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPTSTR, int){
//    HGPSHELP hgpshelp;
//    return RealMain();
// }
// </usage>
//
//////////////////////////////////////////////////////////////////////
#ifndef libHGPSHELP_hpp
#define libHGPSHELP_hpp

#include <windows.h>

/*
 * HGPSHELP loader.
 */
class HGPSHELP
{
public:
	HGPSHELP() throw();
	~HGPSHELP();

	operator HMODULE() const { return module_; }

	const char* GameIdA() const;
	const wchar_t* GameIdW() const;

private:
	const HMODULE module_;
};

/*
 * Constructor
 */
inline HGPSHELP::HGPSHELP() throw()
:	module_(LoadLibraryW(L"HGPSHELP.DLL"))
{
	if(!module_)
	{
		OutputDebugStringW(L"HGPSHELP: Loading error.\n");
	}
}

/*
 * Destructor
 */
inline HGPSHELP::~HGPSHELP()
{
	if(module_)
	{
		FreeLibrary(module_);
	}
}

/*
 * Get the GAME-ID string.
 *
 * The string is null terminated.
 * The buffer is allocated in global scope when the module is initialized.
 */
inline const char* HGPSHELP::GameIdA() const
{
	typedef const char* (WINAPI *FpGameID)();
	FpGameID fp_game_id = (FpGameID)GetProcAddress(module_, (LPCSTR)1);
	if(!fp_game_id)
	{
		return "";
	}

	return fp_game_id();
}

/*
 * Get the GAME-ID string.
 * The string is null terminated.
 * The buffer is allocated in global scope when the module is initialized.
 */
inline const wchar_t* HGPSHELP::GameIdW() const
{
	typedef const wchar_t* (WINAPI *FpGameID)();
	FpGameID fp_game_id = (FpGameID)GetProcAddress(module_, (LPCSTR)2);
	if(!fp_game_id)
	{
		return L"";
	}

	return fp_game_id();
}

#endif
