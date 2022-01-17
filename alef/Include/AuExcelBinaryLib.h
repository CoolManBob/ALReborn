#include "AuExcelTxtLib.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ApUtil.h"

using namespace memory;

namespace AuExcel
{
	const	int	cTypeSize	= 1;	// 1 Byte

	#ifdef _M_X64
	typedef long INT_TYPE;
	#else
	typedef int INT_TYPE;
	#endif

	typedef double DOUBLE_TYPE;
	typedef std::string	STRING_TYPE;

	// 엘리번트 베이스..
	class	Element : public InstanceFrameMemory
	{
	public:
		enum	TYPE
		{
			ET_EMPTY,
			ET_INT,
			ET_DOUBLE,
			ET_STRING,
			ET_ERROR
		};

	public:
		Element():eType( ET_EMPTY ){}
		virtual ~Element(){}

		// 
		BYTE *	ReadType( BYTE	* pBuffer ) { return pBuffer + 1 ; }
		BYTE *	WriteType( BYTE * pBuffer )
		{
			// Type Writing
			*pBuffer	= static_cast< BYTE >( eType );
			pBuffer++;
			return pBuffer;
		}

		virtual BYTE *	WriteData( BYTE * pBuffer ) { return pBuffer; }

		// 타입 구분..
		static Element * DetermineType( BYTE	* pBuffer );	// Factory Function
		static Element * ReadTypeFromString( const char * pString );	// Factory Function

		static size_t	GetMaxBlockSize();	// 클래스중에 젤 큰거..
		
		virtual BYTE *	ReadData( BYTE * pBuffer ) { return pBuffer; }

		virtual	size_t	GetSize() { return cTypeSize; }

		// Get Functions..
		TYPE	GetType() { return eType; }

		// 하나만 쓰고.. 나머지 펑션은 호환용..
		virtual INT_TYPE	GetINT()	{ return 0;			}
		virtual DOUBLE_TYPE	GetDOUBLE() { return 0.0;		}
		virtual const char *	GetSTRING()	{ return NULL;	}

	protected:
		void	SetType( TYPE type ) { eType = type; }

	protected:
		TYPE	eType;
		STRING_TYPE	m_strData; // 밑에 애들이 쓴다.
	};

	class	ElementINT	: public Element
	{
	public:
		ElementINT():m_nData( 0 )
		{
			SetType( ET_INT );
		}
		ElementINT( INT_TYPE nData ):m_nData( nData )
		{
			SetType( ET_INT );
		}

		virtual BYTE *	WriteData( BYTE * pBuffer )
		{
			*( INT_TYPE * ) ( pBuffer ) = m_nData;
			return pBuffer + sizeof INT_TYPE;
		}
		virtual BYTE *	ReadData( BYTE * pBuffer )
		{
			m_nData	= *( INT_TYPE * ) ( pBuffer );
			return pBuffer + sizeof INT_TYPE;
		}

		virtual	size_t	GetSize() { return Element::GetSize() + sizeof INT_TYPE; }

		virtual INT_TYPE	GetINT()	{ return m_nData;			}

		// 호환용..
		virtual DOUBLE_TYPE	GetDOUBLE() { return static_cast< DOUBLE_TYPE > ( m_nData );	}
		virtual const char *	GetSTRING()	{ 
			char	str[ 65536 ]; sprintf( str , "%d" , m_nData );
			m_strData	= str;
			return m_strData.c_str();
		}

	protected:
		INT_TYPE	m_nData;
	};

	class	ElementDOUBLE	: public Element
	{
	public:
		ElementDOUBLE():m_dData( 0 )
		{
			SetType( ET_DOUBLE );
		}
		ElementDOUBLE( DOUBLE_TYPE dData ):m_dData( dData )
		{
			SetType( ET_DOUBLE );
		}

		virtual BYTE *	WriteData( BYTE * pBuffer )
		{
			*( DOUBLE_TYPE * )( pBuffer ) = m_dData;
			return pBuffer + sizeof DOUBLE_TYPE;
		}
		virtual BYTE *	ReadData( BYTE * pBuffer )
		{
			m_dData	= *( DOUBLE_TYPE * )( pBuffer );
			return pBuffer + sizeof DOUBLE_TYPE;
		}

		virtual	size_t	GetSize() { return Element::GetSize() + sizeof DOUBLE_TYPE; }

		virtual DOUBLE_TYPE	GetDOUBLE() { return m_dData;	}

		// 호환용..
		virtual INT_TYPE	GetINT()	{ return static_cast< INT_TYPE > ( m_dData );}
		virtual const char *	GetSTRING()	{ 
			char	str[ 65536 ]; sprintf( str , "%.2f" , m_dData );
			m_strData	= str;
			return m_strData.c_str();
		}
	protected:
		DOUBLE_TYPE	m_dData;
	};

	class	ElementSTRING	: public Element
	{
	public:
		ElementSTRING()
		{
			SetType( ET_STRING );
		}
		ElementSTRING( const char * pStr )
		{
			m_strData = pStr;
			SetType( ET_STRING );
		}

		virtual BYTE *	WriteData( BYTE * pBuffer )
		{
			*( size_t * ) ( pBuffer ) = m_strData.length() + 1;	// +1은 NULL
			pBuffer += sizeof size_t;
			strcpy( ( char * ) pBuffer , m_strData.c_str() );
			pBuffer += m_strData.length() + 1;

			return pBuffer;
		}
		virtual BYTE *	ReadData( BYTE * pBuffer )
		{
			size_t nLength = *( size_t * )( pBuffer );
			pBuffer += sizeof size_t;

			char	strBuffer[ 65536 ];
			ASSERT( nLength < 65536 );

			strncpy( strBuffer , ( char * ) pBuffer , nLength );

			pBuffer += nLength;

			m_strData = strBuffer;
			return pBuffer;
		}

		virtual	size_t	GetSize()
		{
			return	Element::GetSize()
				+ sizeof size_t			// length가 들어갈 자리..
				+ m_strData.length()	// 실제 스트링 길이..
				+ 1;					// 작업 편이를 위한 NULL 
		}

		virtual const char *	GetSTRING()	{ return m_strData.c_str();	}
		// 호환용..
		virtual INT_TYPE	GetINT()	{ return atoi( m_strData.c_str() ); }
		virtual DOUBLE_TYPE	GetDOUBLE()	{ return atof( m_strData.c_str() ); }
	};


	// 메인클래스
	class AuExcelBinaryLib : public InstanceMemoryBlock , public AuExcelLib
	{
	public:
		static const	INT_TYPE	cIdentifier	= 0x00010010;	// 대충 만든 ID..
		static const	INT_TYPE	cVersion	= 0x00000001;	// 프로토타입..

		struct	Header
		{
			INT_TYPE	nIdentifier		;
			INT_TYPE	nVersion		;
			INT_TYPE	nColumn			;
			INT_TYPE	nRow			;
			size_t		nDataSize		;
			size_t		nPackedSize		;

			Header():nIdentifier( cIdentifier ) , nVersion( cVersion ) , nColumn ( 0 ) , nRow( 0 ) , nDataSize( 0 ) , nPackedSize( 0 ) {}
		};
	public:
		AuExcelBinaryLib(): m_iColumn(0) , m_iRow(0) {}

		// 복사과정..
		AuExcelBinaryLib(AuExcelTxtLib &txtlib);

		~AuExcelBinaryLib()
		{
			Clear();
		}

		BOOL ReadExcelFile	( const char *pstrFileName);
		BOOL WriteExcelFile	( const char *pstrFileName);
		BOOL WriteExcelFIleText	( const char *pstrFileName , bool bShowType = false);

		bool	IsEmpty( int iColumn, int iRow)	{ return GetElement( iColumn , iRow )->GetType() == Element::ET_EMPTY ? true : false; }
		INT_TYPE	GetINT		(int iColumn, int iRow)	{ return GetElement( iColumn , iRow )->GetINT();	}
		DOUBLE_TYPE	GetDOUBLE	(int iColumn, int iRow) { return GetElement( iColumn , iRow )->GetDOUBLE();	}
		const char *GetSTRING	(int iColumn, int iRow)	{ return GetElement( iColumn , iRow )->GetSTRING();	}

		int	GetColumn() { return m_iColumn; }
		int GetRow() { return m_iRow; }

		// 호환용
		virtual char *GetData( int iColumn, int iRow ) { return ( char * ) GetSTRING( iColumn , iRow ); }
		virtual INT32 GetDataToInt( int iColumn, int iRow ) { return GetINT( iColumn , iRow ); }
		virtual float GetDataToFloat( int iColumn, int iRow ) { return ( float ) GetDOUBLE( iColumn , iRow ); }

	protected:
		struct	deleter
		{ void	operator()( Element * pElement ) { delete pElement; } };

		void	Clear()
		{
			for_each( vecElement.begin() , vecElement.end() , deleter() );
		}
		Element *	GetElement( int iColumn, int iRow ) { return vecElement[ GetColumn() * iRow + iColumn ]; }
	protected:
		std::vector< Element * >	vecElement;
		int			m_iColumn;
		int			m_iRow;
	};

	inline AuExcelLib * LoadExcelFile( const char * pFileName , BOOL bDecryption = TRUE )
	{
		AuExcelBinaryLib	* pExcelBinary = new AuExcelBinaryLib;
		if( pExcelBinary->ReadExcelFile( pFileName ) )
		{
			return pExcelBinary;
		}
		else
		{
			delete pExcelBinary;
			AuExcelTxtLib	* pExcelTxt = new AuExcelTxtLib;
			if(!pExcelTxt->OpenExcelFile(pFileName, true, bDecryption))
			{
				delete pExcelTxt;
				return FALSE;
			}

			return pExcelTxt;
		}
	}
}