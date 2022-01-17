#pragma once

#include <stdio.h>
#include "ApBase.h"

//@{ 2006/07/31 burumal

// 참고 : 이 정의문을 disable 시키면 AuExcelTxtLibHelper 클래스를 사용하지 않는다
#define __USE_EXCEL_TXT_LIB_HELPER__

#include <string>
#include <unordered_map>
using namespace std;
//@}

// pure virtual class;
class	AuExcelLib
{
public:
	AuExcelLib(){}
	virtual ~AuExcelLib(){}

	virtual int GetColumn()=0;
	virtual int GetRow() =0;

	virtual	bool IsEmpty( int iColumn, int iRow ) = 0;
	virtual char *GetData( int iColumn, int iRow ) = 0;
	virtual INT32 GetDataToInt( int iColumn, int iRow ) = 0;
	virtual float GetDataToFloat( int iColumn, int iRow ) = 0;

	virtual BOOL CloseFile() { return TRUE; }
};

class AuExcelTxtLib : public AuExcelLib
{
//	FILE		*m_pFile;
	char		***m_pppiData;
	char		*m_pBuffer;
	int			m_iBufferSize;
	int			m_iBufferOffset;

	int			m_iColumn;
	int			m_iRow;

	int GetColumnFromFile();
	int GetRowFromFile();

public:
	AuExcelTxtLib();
	~AuExcelTxtLib();
	BOOL OpenExcelFile( const char *pstrFileName, BOOL bLoadAll = FALSE, BOOL bDecryption = FALSE);
	BOOL OpenExcelFile( char *pstrFileName, BOOL bLoadAll = FALSE, BOOL bDecryption = FALSE)
		{ return OpenExcelFile( ( const char * ) pstrFileName , bLoadAll , bDecryption ); }
	BOOL CloseFile();

	int GetColumn();
	int GetRow();

	char *GetData( char **ppiData, int iIndex );
	char *GetData( int iColumn, int iRow );
	INT32 GetDataToInt( int iColumn, int iRow );
	float GetDataToFloat( int iColumn, int iRow );
	bool IsEmpty( int iColumn, int iRow );

	void Reset();
	void ResetRowData( char **ppiData );
	BOOL ReadRow( char **ppiData );
	
	bool EncryptSave(LPCTSTR pszPathName);
};

//@{ 2006/07/31 burumal
#define AU_EXCEL_TXT_LIB_HELPER_VERSION				0x1234
#define AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG			"<@!#_AeTlHhT_#!@>"

typedef struct tagEXCEL_TXT_FILE_INFO
{	
	char*	pFileName;
	int		lFileSize;
	int		lRow;
	int		lColumn;
	
	void Clear()
	{		
		pFileName = NULL;
		lFileSize = 0;
		lRow = 0;
		lColumn = 0;
	}

} EXCEL_TXT_FILE_INFO;

typedef unordered_map<string, EXCEL_TXT_FILE_INFO*>	hmapExcelTxtFileInfo;
typedef hmapExcelTxtFileInfo::iterator			hmapExcelTxtFileInfoItr;

class AuExcelTxtLibHelper
{	
public :
	AuExcelTxtLibHelper();
	~AuExcelTxtLibHelper();
	
	// member funcs...	
	BOOL	IsEnable() { return (m_hmapFiles.size() > 0); }

	VOID	SetIniFile(const char* pIniFileName);
	char*	GetIniFile() { return m_pIniFileName; }

	BOOL	Insert(EXCEL_TXT_FILE_INFO* pExcelTxt);
	BOOL	Delete(const char* pFullPathFileName);	
	VOID	DeleteAll();

	EXCEL_TXT_FILE_INFO* Find(const char* pFullPathFileName);
	EXCEL_TXT_FILE_INFO* NewElement(const char* pFullPathFileName);	
	EXCEL_TXT_FILE_INFO* NewElement(const char* pPath, const char* pFileName);

	int		GetRow(const char* pFullPathFileName);
	int		GetColumn(const char* pFullPathFileName);
	VOID	GetRowColumn(const char* pFullPathFileName, int* pRow, int* pColumn);

	void	SetModified(BOOL bVal) { m_bModified = bVal; }
	BOOL	IsModified() { return m_bModified; }

	BOOL	LoadIniFile();
	BOOL	SaveIniFile();
	BOOL	DeleteIniFile();

	// singleton funcs...
	static AuExcelTxtLibHelper* MakeInstance();
	static AuExcelTxtLibHelper* GetSingletonPtr();
	static void					DestroyInstance();

protected :

	BOOL	WriteHeaderTag(FILE* fpIni);
	BOOL	CheckHeaderTag(const char* pDataBuffer, INT32 nSize);


private :	
	
	BOOL m_bModified;

	INT32 m_nLoadedFileInfoCount;

	char m_pIniFileName[MAX_PATH];

	hmapExcelTxtFileInfo m_hmapFiles;

	static AuExcelTxtLibHelper* m_pThis;
};
//@}