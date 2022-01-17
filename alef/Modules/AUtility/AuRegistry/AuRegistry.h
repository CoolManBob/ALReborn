/*=============================================================

	AuRegistry.h

=============================================================*/

#ifndef _ZZLIBRARY_REGISTRY_H_
	#define _ZZLIBRARY_REGISTRY_H_

#include "ApBase.h"

/********************************************/
/*		The Definition Registry class		*/
/********************************************/
//
class AuRegistry
	{
	protected:
		HKEY	m_hKey;
		BOOL	m_fWrite;

	public:
		static void	ModuleInitialize(CHAR *pszApplication);
				// pszApplication : "<company>\<application>"
		static void ModuleCleanup();

		AuRegistry() : m_hKey(NULL)		{}
		~AuRegistry();

		BOOL	Open(CHAR *pszSection, BOOL fWrite);
		BOOL	Open(HKEY hKey, CHAR *pszSection, BOOL fWrite);
		void	Close();

		BOOL	WriteString(CHAR *pszValueName, CHAR *pszValue, INT32 Value=-1);
		BOOL	ReadString(CHAR *pszValueName, CHAR *lpValue, UINT32 &ulSize);
		//BOOL	ReadString(CHAR pszValueName, CHAR CString& sz);
		BOOL	WriteNumber(CHAR *pszValueName, INT32 lValue);
		BOOL	ReadNumber(CHAR *pszValueName, INT32 *pValue);
		//BOOL	WriteStringArray(LPCTSTR pszValueName, const CStringArray& szs, const CString* pszDef=NULL);
		//BOOL	ReadStringArray(LPCTSTR pszValueName, CStringArray& szs, CString* pszDef=NULL);

	public:
		// helper
		BOOL	Int(CHAR *pszValueName, INT32& i);
		BOOL	Bool(CHAR *pszValueName, BOOL& f);
		//BOOL	String(LPCTSTR pszValueName, CString& sz);
		//BOOL	StringArray(LPCTSTR pszValueName, CStringArray& szs);
		//BOOL	Point(LPCTSTR pszValueName, LPPOINT lpPoint);
		//BOOL	Size(LPCTSTR pszValueName, LPSIZE lpSize);
		//BOOL	Rect(LPCTSTR pszValueName, LPRECT lprc);

	private:
		// internal helper
		HKEY	CreateRegistryKey(HKEY hKeyParent, CHAR *lpszSubKey, BOOL fWrite);
		void	DestroyRegistryKey(HKEY hKey);
	};

/*
inline BOOL IxRegBase::String(LPCTSTR pszValueName, CString& sz)
	{
	return (m_fWrite ? WriteString(pszValueName, sz, sz.GetLength()+1): ReadString(pszValueName, sz));
	}


inline BOOL IxRegBase::StringArray(LPCTSTR pszValueName, CStringArray& szs)
	{
	return (m_fWrite ? WriteStringArray(pszValueName, szs): ReadStringArray(pszValueName, szs));
	}
*/

#endif	// _ZZLIBRARY_REGISTRY_H_