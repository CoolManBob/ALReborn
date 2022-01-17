#pragma once
#include "DefineSoundMacro.h"

//-----------------------------------------------------------------------
//

namespace SoundUtil
{
	SoundVector					GetWorldPositionFromNode	( SoundNode* pNode );
	BOOL						IsSameSoundVector			( SoundVector * vVec1, SoundVector * vVec2 );
	SoundVector					NormalizeVector				( SoundVector * vVec );

	SoundVector					CalcVectorVelocity			( SoundVector * vCurr, SoundVector * vPrev, float fElapsedTime );
	SoundVector					CalcVectorDistance			( SoundVector * vVec1, SoundVector * vVec2 );
	float						CalcScalarDistance			( SoundVector * vVec1, SoundVector * vVec2 );
	SoundVector					CalcVectorScale				( SoundVector * vVec, float fScaleFactor );

	SoundVector					GetAtFromNode				( SoundNode* pNode );
	SoundVector					GetUpFromNode				( SoundNode* pNode );
	int WINAPI					GetOSVersionType			();
};

//-----------------------------------------------------------------------