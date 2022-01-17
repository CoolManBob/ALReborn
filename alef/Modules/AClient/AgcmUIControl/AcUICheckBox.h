// AcUICheckBox.h: interface for the AcUICheckBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_)
#define AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AcUIBase.h"


#define	ACUICHECKEDBOX_H_MAX_STRING				256

enum 
{
	ACUICHECKBOX_MODE_UNCHECK_NORMAL			= UICM_BASE_MAX_MESSAGE,
	ACUICHECKBOX_MODE_UNCHECK_ACTIVE,
	ACUICHECKBOX_MODE_UNCHECK_DISABLE,
	ACUICHECKBOX_MODE_CHECK_NORMAL,
	ACUICHECKBOX_MODE_CHECK_ACTIVE,
	ACUICHECKBOX_MODE_CHECK_DISABLE		
};

// Command Message
enum 
{
	UICM_CHECKBOX_ON_CHECK						= 0,
	UICM_CHECKBOX_ON_UNCHECK,
	UICM_CHECKBOX_MAX_MESSAGE
};

class AcUICheckBox : public AcUIBase
{
public:
	AcUICheckBox( void );
	virtual ~AcUICheckBox( void );

// Virtual Functions
public:
	void				OnAddImage				( RwTexture* pTexture );
	void				OnPostRender			( RwRaster *raster );			// 월드 렌더한 후.
	void				OnWindowRender			( void );
	BOOL				OnLButtonDown			( RsMouseStatus *ms	);			// Left Button Down 시 
	void				OnSetFocus				( void );
	void				OnKillFocus				( void );
	BOOL				OnMouseMove				( RsMouseStatus *ms	);
	
// Button Controll
public :
	BOOL				GetCheckedValue			( void ) { return m_bChecked; }	// 체크되었는지의 여부를 돌려받는다. 

private:	
	BOOL										m_bChecked;						//체크되었는지의 여부
	INT8										m_cCheckedBoxMode;		
	INT32										m_lCheckedBoxIndex[ 6 ];		//Mode 위의 enum참조 
	char										m_szCheckedBoxText[ ACUICHECKEDBOX_H_MAX_STRING ];	//Button위에 Text쓰기 
	INT32										m_lTextPos_x;					//Text의 X좌표 
	INT32										m_lTextPos_y;					//Text의 Y좌표 
	BOOL										m_bTextWrite;					//Text를 사용할 것인지 

	float										m_fTextureWidth	;				// Check Box Texture 의 Width
	float										m_fTextureHeight;				// Check Box Texture 의 Height 

/**

	Not Use...
public :
	void				SetCheckedValue			( BOOL bChecked );				// 체크되어있어야 하는지의 여부를 Set한다. 
	void				SetTextUse				( BOOL bTextUse, char* szText, INT32 lTextPosX = 0, INT32 lTextPosY = 0 );
	void				SetCheckedBoxEnable		( BOOL bEnable );				//활성화 여부를 Set한다. 

*/
};

#endif // !defined(AFX_ACUICHECKBOX_H__1E6E05AF_CE71_4937_AA9D_998AFED4495C__INCLUDED_)
