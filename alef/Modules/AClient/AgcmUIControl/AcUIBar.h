#ifndef	_ACUIBAR_H_
#define	_ACUIBAR_H_


#include "AcUIBase.h"


// Command Message
enum 
{
	UICM_BAR_MAX_MESSAGE					= UICM_BASE_MAX_MESSAGE
};

typedef enum
{
	ACUIBAR_TYPE_CUT_HEAD					= 0x01,
	ACUIBAR_TYPE_SMOOTH						= 0x02,
	ACUIBAR_MAX_TYPE,
} AcUIBarType;

// Bar 가 갖는 애니메이션 타입..
// NONE : 없음
// DECREASE : 감소 애니메이션
// INCREASE : 증가 애니메이션
typedef	enum
{
	ACUIBAR_ANIM_NONE						= 0,
	ACUIBAR_ANIM_DECREASE,
	ACUIBAR_ANIM_INCREASE
} AcUIBarAnimType;


// Bar 클래스
class AcUIBar : public AcUIBase
{
public:
	AcUIBar();
	virtual ~AcUIBar();

public:
	INT32									m_lTotalPoint;				// 총 Point
	INT32									m_lCurrentPoint;			// 현재 Point
	FLOAT									m_fCurrRate;				// 현재 진행률

	INT32									m_lEdgeImageID;				// 외곽을 표시할 이미지 ID
	INT32									m_lBodyImageID;				// Bar 중심부를 표시할 이미지 ID

	BOOL									m_bVertical;
	INT32									m_eBarType;

	AcUIBarAnimType							m_eAnim;					// 현재 Bar에 적용중인 애니메이션
	FLOAT									m_fAfterImageRate;			// Bar 에 진행중인 애니메이션의 목표수치
	FLOAT									m_fAfterImageMoveSpeed;		// Bar 에 진행중인 애니메이션의 진행속도

	UINT32									m_ulLastTick;				// 마지막으로 업데이트된 Tick 값

private :
	void			_CalculateNextRate		( INT32 eBarType, AcUIBarAnimType eAnimType, INT32 lTotalPoint, INT32 lCurrentPoint );
	BOOL			_IsTextureLoaded		( void ) { return m_csTextureList.GetCount() <= 0 ? FALSE : TRUE; }

	RwTexture*		_GetBodyTexture			( void );
	RwTexture*		_GetEdgeTexture			( void );
	BOOL			_IsValidTexture			( RwTexture* pTexture );

	BOOL			_DrawTexture			( RwTexture* pTexture, INT32 lPosX, INT32 lPosY, INT32 lWidth, INT32 lHeight, INT32 lUStart, INT32 lVStart, INT32 lUEnd, INT32 lVEnd, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawTexture			( RwTexture* pTexture, FLOAT fPosX, FLOAT fPosY, FLOAT fWidth, FLOAT fHeight, FLOAT fUStart, FLOAT fVStart, FLOAT fUEnd, FLOAT fVEnd, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawBody				( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodyNoAnim			( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodySmooth			( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	BOOL			_DrawBodySmoothShadow	( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawBodySmoothBody		( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue = 255.0f );
	BOOL			_DrawEdge				( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY );
	
public:
	void			SetPointInfo			( INT32 lTotalPoint, INT32 lCurrentPoint );	// Point Info 를 Setting 한다 - 여기서 이 Control의 Width가 결정된다
	void			SetEdgeImageID			( INT32 lEdgeImageID ) { m_lEdgeImageID = lEdgeImageID; }
	void			SetBodyImageID			( INT32 lBodyImageID ) { m_lBodyImageID = lBodyImageID; }
	INT32			GetEdgeImageID			( void ) { return m_lEdgeImageID;	}
	INT32			GetBodyImageID			( void ) { return m_lBodyImageID; }

//virtual fuction
public:
	virtual	void	OnWindowRender			( void );
};	

#endif			// _ACUIBAR_H_