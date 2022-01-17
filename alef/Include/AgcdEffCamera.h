#pragma once


#include "AgcuEff2ApMemoryLog.h"
#include "AgcdEffBase.h"

enum	eEffCameraType
{

	EFF_CAMERA_ZOOM		,				//	Camera Zoom IN / OUT
	EFF_CAMERA_ROTATE	,				//	Camera Rotate
	EFF_CAMERA_OGIRIN	,				//	Camera 원래 위치로 돌려놓기

	EFF_CAMERA_COUNT	,

};

class AgcdEffCamera	
	:	public AgcdEffBase
{
	EFFMEMORYLOG_SMV;

public:
	AgcdEffCamera( VOID );
	AgcdEffCamera( RwCamera*	pCamera );
	~AgcdEffCamera( VOID );

	VOID				SetCameraMode	( eEffCameraType	eCameraType )		{	m_unCameraType	=	eCameraType;	}
	eEffCameraType		GetCameraMode	( VOID ) const							{	return (eEffCameraType)m_unCameraType;				}

	VOID				SetCamera		( RwCamera*	pCamera )					{	m_pCamera		=	pCamera;		}
	RwCamera*			GetCamera		( VOID )								{	return m_pCamera;					}

	INT					bToFile			( FILE* fp );
	INT					bFromFile		( FILE* fp );

public:
	UINT					m_unCameraType;
	RwCamera*				m_pCamera;

	RwV3d					m_vCameraPos;
	RwV3d					m_vCameraDir;

};