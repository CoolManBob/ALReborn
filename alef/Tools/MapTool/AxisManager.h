#pragma once

class CAxisManager
{
public:
	enum AXIS
	{
		AXIS_NOSELECTED,
		AXISX,
		AXISY,
		AXISZ
	};

protected:
	// 축에 대한 정보..
	UINT32			m_uVertexCount;
	RwIm3DVertex *	m_pImVertex; // 폴리건 갯수 * 버택스량 * 축갯수
	RwMatrix		m_matTransform	;


	INT32			m_nSelectedAxis;

	RwV3d			m_vPos		;
	FLOAT			m_fScale	;
public:
	INT32			GetSelectedAxis(){ return m_nSelectedAxis; }
	INT32			SetSelectedAxis( INT32 nAxis );

	INT32			GetCollisionAxis( RwCamera * pCamera , RsMouseStatus *ms );

	BOOL			SetScale	( FLOAT fScale );
	BOOL			SetPosition	( RwV3d * pPos );

	BOOL	Init();

	void	Render();

	CAxisManager(void);
	~CAxisManager(void);
};
