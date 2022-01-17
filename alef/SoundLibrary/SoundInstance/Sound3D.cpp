#include "Sound3D.h"
#include "Util/SoundUtil.h"

Sound3D::Sound3D( void ) : m_pSceneNode( NULL )
{
	m_vPos = vZeroVector;
	m_vPrevPos = vZeroVector;
}

Sound3D::Sound3D( SoundResource* pResource )
{
	m_pSceneNode	=	NULL;
	m_vPos			=	vZeroVector;
	m_vPrevPos		=	vZeroVector;

	if( pResource != NULL )
	{
		SoundInstance::Create( pResource );

		if( channel_ != NULL )
		{
			//m_pChannel->set3DSpread( 40.0f );
		}
	}
}

Sound3D::~Sound3D( void )
{
}

bool Sound3D::Update( float fTimeElapsed, SoundNode* pListenerNode )
{
	SoundInstance::Update( fTimeElapsed, pListenerNode );

	SoundVector vVelocity = vZeroVector;
	if( m_pSceneNode )
		m_vPos = SoundUtil::GetWorldPositionFromNode( m_pSceneNode );

	if( fTimeElapsed > 0 && !SoundUtil::IsSameSoundVector( &m_vPos, &m_vPrevPos ) )
	{
		vVelocity = SoundUtil::CalcVectorVelocity( &m_vPos, &m_vPrevPos, fTimeElapsed );
	}
	else
	{
		vVelocity = vZeroVector;
	}

	FMOD_VECTOR vChannelPos;
	FMOD_VECTOR vChannelVelocity;

	VECTOR3_SET_FMODVECTOR( vChannelPos, m_vPos );
	VECTOR3_SET_FMODVECTOR( vChannelVelocity, vVelocity );

	channel_->set3DAttributes( &vChannelPos, &vChannelVelocity );
	m_vPrevPos = m_vPos;

	return true;
}

bool Sound3D::UpdatePosition( SoundVector* pPos )
{
	if( pPos )
	{
		m_vPrevPos = m_vPos;
		m_vPos = *pPos;
	}
	
	return true;
}

float Sound3D::GetActiveDistanceMin( void )
{
	float distMin = 0.0f;
	float distMax = 0.0f;

	if( channel_ != NULL )
	{
		channel_->get3DMinMaxDistance( &distMin, &distMax );
	}

	return distMin;
}

float Sound3D::GetActiveDistanceMax( void )
{
	float distMin = 0.0f;
	float distMax = 0.0f;

	if( channel_ != NULL )
	{
		channel_->get3DMinMaxDistance( &distMin, &distMax );
	}

	return distMax;
}

void Sound3D::SetActiveDistanceMinMax( const float fDistanceMin, const float fDistanceMax )
{
	if( channel_ != NULL )
	{
		channel_->set3DMinMaxDistance( fDistanceMin, fDistanceMax );
	}
}
