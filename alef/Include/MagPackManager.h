// MagPackManager.h: interface for the CMagPackManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGPACKMANAGER_H__017DCB92_41BE_11D4_93B7_00E098783101__INCLUDED_)
#define AFX_MAGPACKMANAGER_H__017DCB92_41BE_11D4_93B7_00E098783101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AuList.h"
#include "MagUnpackManager.h"

BOOL PackFiles( char * outputfilename , AuList< CMagPackHeaderInfo * > *plistFiles ,
			   int (__cdecl *filecallback ) (unsigned int, unsigned int) = NULL ,
			   int (__cdecl *statecallback ) (unsigned int, unsigned int) = NULL );
// char * outputfilename : Specify the output MPF file name
// CMagList *plistFiles : Give the file info with list . this list must be contructed with CMagPackHeaderInfo
// int (__cdecl *filecallback ) (unsigned int, unsigned int) = NULL ,
// int (__cdecl *statecallback ) (unsigned int, unsigned int) = NULL );
//		Can be NULL when you need no info the packing process
// Filecallback is for file count info , ( Current file , total file )
// statecallback is for file compressing info. ( Processing Size , Compressed Size )
//  

class CMagPackHeaderBinaryInfo : public CMagPackHeaderInfo
{
public:
	BYTE	*	pSourceBinary	;
	UINT		nSourceSize		;
};

BOOL PackFilesBinary( char * outputfilename , AuList< CMagPackHeaderBinaryInfo * > *plistFiles ,
			   int (__cdecl *filecallback ) (unsigned int, unsigned int) = NULL ,
			   int (__cdecl *statecallback ) (unsigned int, unsigned int) = NULL );
// Compress , CMagPackHeaderInfo's Binary Infomation..

void	GetMagPackVersion( char * string , UINT & buildnumber );
// Get Version String and number

#endif // !defined(AFX_MAGPACKMANAGER_H__017DCB92_41BE_11D4_93B7_00E098783101__INCLUDED_)
