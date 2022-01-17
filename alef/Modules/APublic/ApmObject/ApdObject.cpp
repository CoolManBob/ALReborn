#include "ApdObject.h"
#include "AuMath.h"

//---------------------- ApdObject ----------------------
void	ApdObject::Init()
{
	m_Mutex.Init((PVOID)this);

	m_eType		= APBASE_TYPE_OBJECT;

	m_stScale.x	= 1.0f;
	m_stScale.y	= 1.0f;
	m_stScale.z	= 1.0f;

	m_fDegreeX	= 0.0f;
	m_fDegreeY	= 0.0f;

	m_nObjectType		= 1;// ACUOBJECT_TYPE_OBJECT;
	m_lCurrentStatus	= APDOBJ_STATUS_NORMAL;
	m_listOcTreeID		= NULL;
	m_ulRemoveTimeMSec	= 0;
	m_nDimension		= 0;
}

void	ApdObject::CalcWorldBlockInfo( int nBlockCount , AuBLOCKING * pBlockingTemplate , AuBLOCKING * pBlock )
{
	ASSERT( pBlock );
	if( !pBlock ) return;
	
	for( int i = 0 ; i < nBlockCount ; ++i )
	{
		pBlock[i] = pBlockingTemplate[i];

		switch( pBlock[ i ].type )
		{
		case	AUBLOCKING_TYPE_NONE:
			break;
		case	AUBLOCKING_TYPE_BOX	:
			{
				AuV3D	v3d		;
				AuV3D	v3dout	;

				// 스케일링..
				pBlock[ i ].data.box.inf.x	*= m_stScale.x;
				pBlock[ i ].data.box.inf.y	*= m_stScale.y;
				pBlock[ i ].data.box.inf.z	*= m_stScale.z;

				pBlock[ i ].data.box.sup.x	*= m_stScale.x;
				pBlock[ i ].data.box.sup.y	*= m_stScale.y;
				pBlock[ i ].data.box.sup.z	*= m_stScale.z;


				// 로테이트..
				AuMATRIX	matrix;
				AuMath::MatrixRotateX	( & matrix , m_fDegreeX );
				AuMath::MatrixRotateY	( & matrix , m_fDegreeY );
				AuMath::MatrixTranslate	( & matrix , m_stPosition.x , m_stPosition.y , m_stPosition.z );

				v3d.x	= pBlock[ i ].data.box.inf.x;
				v3d.y	= pBlock[ i ].data.box.inf.y;
				v3d.z	= pBlock[ i ].data.box.inf.z;

				AuMath::V3DTransform	( &v3dout , &v3d , & matrix );
				
				pBlock[ i ].data.box.inf.x = v3dout.x;
				pBlock[ i ].data.box.inf.y = v3dout.y;
				pBlock[ i ].data.box.inf.z = v3dout.z;

				v3d.x	= pBlock[ i ].data.box.sup.x;
				v3d.y	= pBlock[ i ].data.box.sup.y;
				v3d.z	= pBlock[ i ].data.box.sup.z;

				AuMath::V3DTransform	( &v3dout , &v3d , & matrix );
				
				pBlock[ i ].data.box.sup.x = v3dout.x;
				pBlock[ i ].data.box.sup.y = v3dout.y;
				pBlock[ i ].data.box.sup.z = v3dout.z;
			}
			break;
		case	AUBLOCKING_TYPE_SPHERE		:
			{
				AuV3D	v3d		;
				AuV3D	v3dout	;

				// 스케일링..
				v3d.x	= pBlock[ i ].data.sphere.center.x;
				v3d.y	= pBlock[ i ].data.sphere.center.y;
				v3d.z	= pBlock[ i ].data.sphere.center.z;

				v3d.x	*= m_stScale.x;
				v3d.y	*= m_stScale.y;
				v3d.z	*= m_stScale.z;

				pBlock[ i ].data.sphere.radius	*=
					( FLOAT ) sqrt( m_stScale.x * m_stScale.x + m_stScale.y * m_stScale.y + m_stScale.z * m_stScale.z );

				// 로테이트..

				AuMATRIX	matrix;
				AuMath::MatrixRotateX	( & matrix , m_fDegreeX );
				AuMath::MatrixRotateY	( & matrix , m_fDegreeY );
				AuMath::MatrixTranslate	( & matrix , m_stPosition.x , m_stPosition.y , m_stPosition.z );
				AuMath::V3DTransform	( &v3dout , &v3d , & matrix );
				
				pBlock[ i ].data.sphere.center.x = v3dout.x;
				pBlock[ i ].data.sphere.center.y = v3dout.y;
				pBlock[ i ].data.sphere.center.z = v3dout.z;
			}
			break;
		case	AUBLOCKING_TYPE_CYLINDER	:
			{
				AuV3D	v3d		;
				AuV3D	v3dout	;

				// 스케일링..
				v3d.x	= pBlock[ i ].data.cylinder.center.x;
				v3d.y	= pBlock[ i ].data.cylinder.center.y;
				v3d.z	= pBlock[ i ].data.cylinder.center.z;

				v3d.x	*= m_stScale.x;
				v3d.y	*= m_stScale.y;
				v3d.z	*= m_stScale.z;

				pBlock[ i ].data.cylinder.radius *= ( FLOAT ) sqrt( m_stScale.x * m_stScale.x + m_stScale.y * m_stScale.y + m_stScale.z * m_stScale.z );
				pBlock[ i ].data.cylinder.height *= ( FLOAT ) sqrt( m_stScale.x * m_stScale.x + m_stScale.y * m_stScale.y + m_stScale.z * m_stScale.z );

				// 로테이트..

				AuMATRIX	matrix;
				AuMath::MatrixRotateX	( & matrix , m_fDegreeX );
				AuMath::MatrixRotateY	( & matrix , m_fDegreeY );
				AuMath::MatrixTranslate	( & matrix , m_stPosition.x , m_stPosition.y , m_stPosition.z );
				AuMath::V3DTransform	( &v3dout , &v3d , & matrix );
				
				pBlock[ i ].data.cylinder.center.x = v3dout.x;
				pBlock[ i ].data.cylinder.center.y = v3dout.y;
				pBlock[ i ].data.cylinder.center.z = v3dout.z;
			}
			break;
		}
	}
}

//---------------------- ApdObjectTemplate ----------------------
void	ApdObjectTemplate::Init()
{
	m_Mutex.Init((PVOID) this);
	m_eType			= APBASE_TYPE_OBJECT_TEMPLATE;
	m_nObjectType	= 1;//ACUOBJECT_TYPE_OBJECT;
}