#ifndef	__EXCEL_LIB_H__
#define __EXCEL_LIB_H__

#include <afxdisp.h>
#include <ks.h>

#include "AuExcel9.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
#pragma comment ( lib, "AuExcelLibD" )
#else 
#pragma comment ( lib, "AuExcelLib" )
#endif
#endif

class CExcelLib
{
	_Application	m_ObjApp;
	_Workbook		m_ObjBook;
	Workbooks		m_ObjBooks;
	Worksheets		m_ObjSheets;
	_Worksheet		m_ObjSheet;
	Range			m_ObjRange;

	COleSafeArray	*m_pSafeArrary;
	VARENUM			*m_paFieldTypes;

	short			m_iCurrentSheet;
	long			m_lSheetCount;
	long			m_lNumRows;
	long			m_lNumCols;

	int				m_iCurrentRow;
	int				m_iCurrentCol;

public:
	CExcelLib();
	~CExcelLib();

	//Excel File
	bool OpenExcelFile( char *pstrFileName );
	bool CloseFile();
	bool SetSheet( int iSheet );

	VARIANT GetElement( int iRow, int iColumn );

	bool SetFieldType( VARENUM iFirst, ... );
	bool ReadRow( void *pvArg, ... );
	bool SetRow( int iRow );
	bool MoveToPrevRow();
	bool MoveToNextRow();

	long GetSheet();
	long GetRow();
	long GetColumn();
};

#endif