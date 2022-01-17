#include "stdafx.h"
#include "AuExcelLib.h"

CExcelLib::CExcelLib()
{
	m_paFieldTypes = NULL;
	m_pSafeArrary = NULL;
	m_lNumRows = 0;
	m_lNumCols = 0;
	m_iCurrentSheet = 1; //디폴트로 열리는 시트는 첫번째 시트이다.
	m_lSheetCount = 0;

	m_iCurrentRow  = 0;
	m_iCurrentCol = 0;
}

CExcelLib::~CExcelLib()
{
}

long CExcelLib::GetSheet()
{
	return m_lSheetCount;
}

long CExcelLib::GetRow()
{
	return  m_lNumRows;
}

long CExcelLib::GetColumn()
{
	return m_lNumCols;
}

bool CExcelLib::OpenExcelFile( char *pstrFileName )
{
	COleVariant		VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	bool			bResult;

	bResult = false;

    // Instantiate Excel and open an existing workbook.
    if( m_ObjApp.CreateDispatch("Excel.Application") == TRUE )
	{
		if( (m_ObjBooks = m_ObjApp.GetWorkbooks()) != NULL )
		{
			if( (m_ObjBook = m_ObjBooks.Open( pstrFileName,
											VOptional, VOptional, VOptional, VOptional,
											VOptional, VOptional, VOptional, VOptional,
											VOptional, VOptional, VOptional, VOptional) ) != NULL )
			{
				if( (m_ObjSheets = m_ObjBook.GetWorksheets()) != NULL )
				{
					m_lSheetCount = m_ObjSheets.GetCount();

					//시트가 하나라도 있어야 읽지......
					if( m_lSheetCount > 0 )
					{
						bResult = SetSheet( 1 );
					}
				}
			}
		}
	}

	return bResult;
}

bool CExcelLib::SetSheet( int iSheet )
{
	bool			bResult = false;

	m_iCurrentSheet = iSheet;

	if( (0 < m_iCurrentSheet) && (m_iCurrentSheet <= m_lSheetCount) )
	{
		if( (m_ObjSheet = m_ObjSheets.GetItem(COleVariant((short)m_iCurrentSheet))) != NULL )
		{
			if( m_pSafeArrary != NULL )
			{
				delete m_pSafeArrary;

				m_pSafeArrary = NULL;
			}

			if( (m_ObjRange = m_ObjSheet.GetUsedRange()) != NULL )
			{
				VARIANT			ret;

				ret = m_ObjRange.GetValue();

				if( (m_pSafeArrary = new COleSafeArray( ret )) != NULL )
				{
					m_pSafeArrary->GetUBound(1, &m_lNumRows);
					m_pSafeArrary->GetUBound(2, &m_lNumCols);

					bResult = true;
				}
			}
		}
	}

	return bResult;
}


VARIANT CExcelLib::GetElement( int iRow, int iColumn )
{
	VARIANT			vReturn;

	vReturn.vt = VT_NULL;

	if( m_pSafeArrary != NULL )
	{
		if( (1 <= iRow) && (iRow <= m_lNumRows ) && (1 <= iColumn) && (iColumn <= m_lNumCols ) )
		{
			long index[2];

			index[0] = iRow;
			index[1] = iColumn;

			m_pSafeArrary->GetElement(index, &vReturn);
		}
	}

	return vReturn;
}

bool CExcelLib::SetFieldType( VARENUM iFirst, ... )
{
	va_list		Marker;
	VARENUM		iFieldData;
	int			iFieldCount = 0;
	bool		bResult = false;

	iFieldData = iFirst;

	//인자의 갯수를 확인해본다. 하나라도 있어야 읽지~
	va_start( Marker, iFirst );

	while( iFieldData != VT_NULL )
	{
		iFieldCount++;

		iFieldData = va_arg( Marker, VARENUM );
	}

	va_end( Marker );

	//뭔가 인자가 있다면?
	if( iFieldCount != 0 )
	{
		int			i=0;

		if( m_paFieldTypes != NULL )
			delete [] m_paFieldTypes;

		m_paFieldTypes = new VARENUM[iFieldCount];

		//갯수를 얻었으니..... 실제 Type를 저장한다.
		iFieldData = iFirst;

		va_start( Marker, iFirst );

		while( iFieldData != VT_NULL )
		{
			m_paFieldTypes[i] = iFieldData;
			i++;

			iFieldData = va_arg( Marker, VARENUM );
		}

		va_end( Marker );

		bResult = true;
	}

	return bResult;
}

bool CExcelLib::SetRow( int iRow )
{
	bool			bResult;

	if( (1 <= iRow) && (iRow <= m_lNumRows) ) 
	{
		m_iCurrentRow = iRow;
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

bool CExcelLib::MoveToPrevRow()
{
	bool			bResult;

	if( (m_iCurrentRow - 1) < 1 )
	{
		bResult = false;
	}
	else
	{
		m_iCurrentRow--;
		bResult = true;
	}

	return bResult;
}

bool CExcelLib::MoveToNextRow()
{
	bool			bResult;

	if( (m_iCurrentRow + 1) > m_lNumRows )
	{
		bResult = false;
	}
	else
	{
		m_iCurrentRow++;
		bResult = true;
	}

	return bResult;
}

bool CExcelLib::ReadRow( void *pvArg, ... )
{
	//값을 넘겨받을 인자의 갯수를 센다.
	va_list		Marker;
	void		*pvFieldData;
	int			iFieldCount = 0;
	bool		bResult = false;

	pvFieldData = pvArg;

	//인자의 갯수를 확인해본다. 하나라도 있어야읽지~
	va_start( Marker, pvArg );

	while( pvFieldData != NULL )
	{
		iFieldCount++;

		pvFieldData = va_arg( Marker, void * );
	}

	va_end( Marker );

	//인자의 수가 0개가 아니고 칼럼의 수와 일치할때!!
	if( (iFieldCount != 0) && (iFieldCount == m_lNumCols) )
	{
		m_iCurrentRow++;
		
		if( m_iCurrentRow > m_lNumRows )
		{
			m_iCurrentRow--;
			bResult = false;
		}
		else
		{
			VARIANT			varRet;

			pvFieldData = pvArg;

			//인자의 갯수를 확인해본다. 하나라도 있어야읽지~
			va_start( Marker, pvArg );

			for( int iCol = 1; iCol <= m_lNumCols; ++iCol )
			{
				varRet = GetElement( m_iCurrentRow, iCol );

				if( varRet.vt == VT_R8 )
				{
					if( m_paFieldTypes[iCol-1] == VT_BOOL )
					{
						pvFieldData = (void *)&varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_I1 )
					{
						*((char *)pvFieldData) = (char)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_I2 )
					{
						*((short *)pvFieldData) = (short)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_I4 )
					{
						*((int *)pvFieldData) = (int)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_UI1 )
					{
						*((unsigned char *)pvFieldData) = (unsigned char)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_UI2 )
					{
						*((unsigned short *)pvFieldData) = (unsigned short)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_UI4 )
					{
						*((unsigned int *)pvFieldData) = (unsigned int)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_R4 )
					{
						*((float *)pvFieldData) = (float)varRet.dblVal;
					}
					else if( m_paFieldTypes[iCol-1] == VT_R8 )
					{
						*((double *)pvFieldData) = (double)varRet.dblVal;
					}
					else
					{
						//뭐가 더있지?
					}
				}
				else if( varRet.vt == VT_BSTR )
				{
					(*(CString *)pvFieldData) = (CString)varRet.bstrVal;
				}
				else if( varRet.vt == VT_EMPTY )
				{
					; //아무일 없음.
				}
				else
				{
					; //우선은 R8, BSTR만......
					; //나머지는 필요하면 추가하자~
				}

				pvFieldData = va_arg( Marker, void * ); //그냥 포인터니까 4바이트씩 증가시킨다.
			}

			va_end( Marker );

			bResult = true;
		}
	}

	return bResult;
}

bool CExcelLib::CloseFile()
{
	bool			bResult;
    COleVariant		VOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	bResult = true;

	m_lNumRows = 0;
	m_lNumCols = 0;
	m_iCurrentSheet = 1; //디폴트로 열리는 시트는 첫번째 시트이다.
	m_lSheetCount = 0;

	if( m_paFieldTypes != NULL )
	{
		delete [] m_paFieldTypes;
		m_paFieldTypes = NULL;
	}

	if( m_pSafeArrary != NULL )
	{
		delete m_pSafeArrary;
		m_pSafeArrary = NULL;
	}

	m_ObjBook.Close(COleVariant((short)FALSE), VOptional, VOptional);
	m_ObjApp.Quit();

	return bResult;
}
