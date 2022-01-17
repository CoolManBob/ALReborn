#pragma once

#include "SoundInstance.h"

//-----------------------------------------------------------------------
//

class Sound3D : public SoundInstance
{
private :
	SoundNode*												m_pSceneNode;
	SoundVector												m_vPos;
	SoundVector												m_vPrevPos;

public :
	Sound3D( void );
	Sound3D( SoundResource* pResource );
	virtual ~Sound3D( void );

	virtual bool 					Update					( float fTimeElapsed, SoundNode* pListenerNode = NULL );

	SoundNode*						GetSceneNode			( void ) { return m_pSceneNode; }
	void							SetSceneNode			( SoundNode* pNode ) { m_pSceneNode = pNode; }

	SoundVector						GetPos					( void ) { return m_vPos; }
	void							SetPos					( SoundVector vPos ) { m_vPos = vPos; }

	SoundVector						GetPrevPos				( void ) { return m_vPrevPos; }
	void							SetPrevPos				( SoundVector vPos ) { m_vPrevPos = vPos; }

	bool 							UpdatePosition			( SoundVector* pPos = NULL );

	float							GetActiveDistanceMin	( void );
	float							GetActiveDistanceMax	( void );
	void							SetActiveDistanceMinMax	( const float fDistanceMin, const float fDistanceMax );
};

//-----------------------------------------------------------------------