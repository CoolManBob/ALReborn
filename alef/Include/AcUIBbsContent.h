#ifndef		_ACUIBBSCONTENT_H_
#define		_ACUIBBSCONTENT_H_

#include "AcUIBase.h"
#include "AcUIButton.h"

#define ACUIBBSCONTENT_CLOSE_BUTTON		1

enum 
{
	UICM_BBSCONTENT_MAX_MESSAGE
};

class AcUIBbsContent : public AcUIBase
{
public:
	AcUIBbsContent();
	virtual ~AcUIBbsContent();
		
//Member 
private:
	char			*m_pszContentText	;			//Content Text 
	RwRect			m_rectDrawContent	;			//글이 써질 위치 Rect

public:
	AcUIButton		m_clUIButton		;			//버튼 

//fuction
public:
	BOOL	SetContentText( char* pszContent, UINT16 nContentLength );
	void	SetDrawRect( RwRect rect );
	void	ReleaseContentTextMemory();
	
private:
	BOOL	DrawContentText( );

//virtual function
public:
	virtual void	OnPostRender	( RwRaster* raster	);
	virtual	void	OnWindowRender	(					);
	virtual void	OnClose			(					);
	virtual BOOL	OnInit			(					);
	virtual BOOL OnCommand		( INT32	nID , PVOID pParam	);
};

#endif		//_ACUIBBSCONTENT_H_