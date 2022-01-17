#include "AgcdEffCamera.h"

AgcdEffCamera::AgcdEffCamera( VOID )
: AgcdEffBase( AgcdEffBase::E_EFFBASE_CAMERA )
{
	m_unCameraType		=	EFF_CAMERA_ZOOM;
	m_pCamera			=	NULL;

	ZeroMemory( &m_vCameraPos , sizeof(RwV3d) );
	ZeroMemory( &m_vCameraDir , sizeof(RwV3d) );
}

AgcdEffCamera::~AgcdEffCamera( VOID )
{

}

INT			AgcdEffCamera::bToFile( FILE* fp )
{

	return 0;
}	

INT			AgcdEffCamera::bFromFile( FILE* fp )
{

	return 0;
}