// MagUnpackManager.h: interface for the CMagUnpackManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGUNPACKMANAGER_H__017DCB93_41BE_11D4_93B7_00E098783101__INCLUDED_)
#define AFX_MAGUNPACKMANAGER_H__017DCB93_41BE_11D4_93B7_00E098783101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AuList.h"

#define	MAGPACKTMPFILENAME	"$$MP%02d$$.magtmp"
#define	MAGPACKFILEHEADERTXT	"MagPack Ver 0.1a"
#define	MAGPACKFILEHEADERSIZE	50 // reserved
#define	MAGPACKFILEHEADERBUFFERSIZE	( 100 * 1024 )
#define	MAGPACKFILE_MAXFILENAME	256

#define	MAGPACK_USE_ARRAY
#define	MAGPACK_MAX_ELEMENT_SIZE		256

// for Store header info
class CMagPackHeaderInfo
{
public:
		char	filename[ MAGPACKFILE_MAXFILENAME ]	; // Filename
		UINT	num					; // Sequence number in the file
		UINT	size				; // Packed size
		BYTE *	pPackedData			;	

		CMagPackHeaderInfo(){}
		~CMagPackHeaderInfo()
		{
		}

		void	Clean()
		{
			// 여기서 처리하지 않음.
			//if( pPackedData ) delete [] pPackedData;
			//pPackedData	= NULL;
		}
};

class CMagUnpackManager  
{
protected:
	UINT	m_nFileCount			;	// Total File Count;
	char	m_strFilename[ 256 ]	;	// The main packed file name.

	BYTE	* m_pMemoryBuffer		;

	struct	ReturnMemory
	{
	protected:
		BYTE	* pMemory	;
		int		nSize		;

	public:

		ReturnMemory():pMemory( NULL ),nSize( 0 ){}
		~ReturnMemory(){ if( pMemory ) delete [] pMemory; }

		BYTE	* GetMemory( int nRequestSize )
		{
			if( nSize < nRequestSize )
			{
				if( pMemory ) delete [] pMemory;
				pMemory = new BYTE[ nRequestSize ];
				nSize = nRequestSize;
			}
			return pMemory;
		}
	};

	static	ReturnMemory	m_sReturnMemory;
	
	BOOL	AllocMemory( int nSize );
	void	FreeMemory();

#ifdef MAGPACK_USE_ARRAY
	CMagPackHeaderInfo	m_arrayList[ MAGPACK_MAX_ELEMENT_SIZE ];
#else
	AuList< CMagPackHeaderInfo * >	m_listFile; // File info list , get from the MPF file
#endif

public:
	CMagUnpackManager();
	virtual ~CMagUnpackManager();


	// Initialization..
	BOOL	SetFile			( char * filename	);	// Set Main packed file
	BOOL	CloseFile		();						// Close the file stream.

	// Return currently loaded pack file
	char *	GetPackFileName	() { return m_strFilename; }

	UINT	GetFileCount	();
	int		GetFileNum		( char *filename );
	// if errors , return -1;

	// Get File Size..
	BOOL	GetFileSize		( UINT	num			, UINT	*pSize	);
	BOOL	GetFileSize		( char	*filename	, UINT	*pSize	){ return GetFileSize( GetFileNum( filename ) , pSize ); }
		
	UINT	GetFileBinary	( char *filename	, BYTE *&ptr			);
	UINT	GetFileBinary	( UINT num			, BYTE *&ptr			); 
	// Get File to binary pointer , the pointer does not take anything valuable,
	// This Function create with new operator to get memory for you
	// Return the alloc size of the pointer.

	BOOL	GetFile			( char *filename	, char *pdirdest = NULL	);
	BOOL	GetFile			( UINT num			, char *pdirdest = NULL	);

	BOOL	GetAllFiles		( char *pdirdest = NULL ,int (__cdecl *filecallback ) (unsigned int, unsigned int) = NULL);

	char *	GetFileName		( UINT num			);
};

void	GetMagPackVersion	( char * string , UINT & buildnumber	);
UINT	XORReturn			( UINT data								); // 암호화 중요 자료 암호화 저장용
UINT	GetParity			( BYTE * pdata , UINT size				);

#endif // !defined(AFX_MAGUNPACKMANAGER_H__017DCB93_41BE_11D4_93B7_00E098783101__INCLUDED_)
