#pragma once


// CNewStatic
// 중국 Patch Client가 깨지는걸 수정한 스태틱 컨트롤
// (유니코드)기반으로 프로젝트를 바꿔도 되지만 엄청난 시간이 필요해서 이걸로 대체한다 ㅡ ㅡ
// Owner Draw 수정

class CNewStatic : public CStatic
{
	DECLARE_DYNAMIC(CNewStatic)

public:
	CNewStatic();
	virtual ~CNewStatic();

	VOID				SetText( wchar_t*	pszText )	{	m_TextData	=	pszText;	}
	const CStringW		GetText( VOID )					{	return m_TextData;			}

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnPaint			( VOID );
	afx_msg BOOL	OnEraseBkgnd	( CDC* pDC );

	CStringW	m_TextData;

	
};


