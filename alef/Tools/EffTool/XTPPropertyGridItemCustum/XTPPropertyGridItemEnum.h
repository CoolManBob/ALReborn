#pragma once

#include <vector>
#pragma warning(disable: 4786)

//-------------------------- CXTPGI_Enum --------------------------
template <class T>
class CXTPGI_Enum : public CXTPPropertyGridItem  
{
	int		m_nEmax;
	T		m_eVal;
	T*	m_pBindVal;

	const TCHAR * const * const		STRENUMTYPE;

	void	InitCombo(void)
	{
		if( !STRENUMTYPE )		return;

		//m_eVal 요넘은 반드시 0부터 시작하며 순차적으로 증가하는 enum type 이어야 한다.
		CXTPPropertyGridItemConstraints*	pList = CXTPPropertyGridItem::GetConstraints();
		for( int i=0; i<m_nEmax; ++i )
			pList->AddConstraint( STRENUMTYPE[i] );

		SetFlags( xtpGridItemHasComboButton | xtpGridItemHasEdit );
	};

	T		ChkVal( CString strVal )
	{
		if( !STRENUMTYPE )		return (T)m_nEmax;

		for( int i=0; i<m_nEmax; ++i )
		{
			if( strVal == STRENUMTYPE[i] )
				return (T)i;
		}

		return (T)m_nEmax;
	}

public:
	CXTPGI_Enum(CString strCaption, T eMax = (T)0, T eVal = (T)0, T* pBindVal=NULL, const TCHAR*const*const strEnumType=NULL)
		: CXTPPropertyGridItem(strCaption)
		, m_nEmax(eMax)
		, m_pBindVal(pBindVal)
		, STRENUMTYPE(strEnumType)
	{
		InitCombo();
		SetEVal(eVal);
	};

	CXTPGI_Enum(UINT nID, T eMax = (T)0, T eVal = (T)0, T* pBindVal=NULL, const TCHAR*const*const strEnumType=NULL)
		: CXTPPropertyGridItem(nID)
		, m_nEmax(eMax)
		, m_pBindVal(pBindVal)
		, STRENUMTYPE(strEnumType)
	{
		InitCombo();
		SetEVal(eVal);
	};

	virtual ~CXTPGI_Enum(){};

public:
	void	SetEVal(T eVal)
	{
		m_eVal	= eVal;
		if( m_pBindVal )
			*m_pBindVal = eVal;

		CString strVal;
		if(STRENUMTYPE && m_nEmax > (int)eVal )
			strVal.Format( _T("%s"), STRENUMTYPE[eVal] );
		else
			strVal.Format( _T("%i, UNKNOWN"), eVal );

		CXTPPropertyGridItem::SetValue( strVal );
	};

	T		GetEVal(void) const
	{	
		return m_eVal;	
	}

	void	BindToEVal(T* pBindVal)
	{
		m_pBindVal	= pBindVal;
		if( m_pBindVal )
			*m_pBindVal	= m_eVal;
	};

protected:
	virtual void	SetValue(CString strValue)
	{
		SetEVal( ChkVal( strValue ) );
	};
		
#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue)
	{
		if( varValue->vt == VT_I4 )			SetEVal( (T)varValue->lVal ); 
		else if( varValue->vt == VT_I2 )	SetEVal( (T)varValue->iVal ); 
	};

	afx_msg const VARIANT OleGetValue()
	{
		return CComVariant((int)m_eVal);
	};
#endif
};

//-------------------------- CXTPGI_EnumGen --------------------------
template<class T>
class CXTPGI_EnumGen : public CXTPPropertyGridItem
{
public:	
	struct	ValAndString
	{
		INT		m_num;
		LPCTSTR	m_str;
	};

typedef typename std::vector<ValAndString>	STLVEC_ENUM;
typedef typename STLVEC_ENUM::iterator		STLVEC_ENUM_ITR;

private:
	int			m_nNum;
	T			m_val;
	T*			m_pVal;
	STLVEC_ENUM	m_vecEnum;

	void InitCombo(void)
	{
		CXTPPropertyGridItemConstraints* pList = GetConstraints();

		for( int i=0; i<m_nNum; ++i )
			pList->AddConstraint( m_vecEnum[i].m_str );

		SetFlags( xtpGridItemHasComboButton );
	};

	T ChkVal( CString strVal )
	{
		for( int i=0; i<m_nNum; ++i )
		{
			if( strVal == m_vecEnum[i].m_str )
				return (T)(m_vecEnum[i].m_num);
		}

		return (T)m_nNum;
	}

	LPCTSTR ChkVal( T val )
	{
		for( int i=0; i<m_nNum; ++i )
		{
			if( (INT)val == m_vecEnum[i].m_num )
				return (LPCTSTR)(m_vecEnum[i].m_str);
		}
		return (_T("Unknown"));
	}

public:
	CXTPGI_EnumGen(CString strCaption, int nNum, T eVal, T* pBindVal, const INT number[], const TCHAR*const*const strEnumType )
		: CXTPPropertyGridItem(strCaption)
		, m_nNum(nNum)
		, m_pVal(pBindVal)
		, m_vecEnum(m_nNum)
	{
		ASSERT( m_pVal && number && strEnumType );
		for( int i=0; i<nNum; ++i )
		{
			m_vecEnum[i].m_num = number[i];
			m_vecEnum[i].m_str = strEnumType[i];
		}

		InitCombo();
		SetEVal(eVal);
	};
	CXTPGI_EnumGen(UINT nID, int nNum, T eVal, T* pBindVal, const INT number[], const TCHAR*const*const strEnumType )
		: CXTPPropertyGridItem(nID)
		, m_num(nNum)
		, m_pVal(pBindVal)
		, m_vecEnum(m_nNum)
	{
		ASSERT( m_pVal && number && strEnumType );
		for( int i=0; i<nNum; ++i )
		{
			m_vecEnum[i].m_num = number[i];
			m_vecEnum[i].m_str = strEnumType[i];
		}

		InitCombo();
		SetEVal(eVal);
	};

	virtual ~CXTPGI_EnumGen(){};

public:
	void	SetEVal(T eVal){
		m_val	= eVal;
		if( m_pVal )
			*m_pVal = eVal;

		CString strVal = ChkVal(eVal);
		CXTPPropertyGridItem::SetValue(strVal);
	};
	T		GetEVal(void) const
	{
		return m_val;
	}

	void	BindToEVal(T* pBindVal)
	{
		m_pVal	= pBindVal;
		if( m_pVal )
			*m_pVal	= m_val;
	};

protected:
	virtual void	SetValue(CString strValue)
	{
		SetEVal( ChkVal( strValue ) );
	};
		
#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue)
	{
		if( varValue->vt == VT_I4 )	SetEVal( (T)varValue->lVal ); else
		if( varValue->vt == VT_I2 )	SetEVal( (T)varValue->iVal ); 
	};
	afx_msg const VARIANT OleGetValue()
	{
		return CComVariant((int)m_val);
	};
#endif
};
