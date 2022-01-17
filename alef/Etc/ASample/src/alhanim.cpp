#include "alhanim.h"
#include "alcharacter.h"

typedef struct RpHAnimStdKeyFrame_ RpHAnimStdKeyFrame_;

struct RpHAnimStdKeyFrame_
{
    RpHAnimStdKeyFrame_ *prevFrame;  /**< Previous keyframe for particular hierarchy node  */
    RwReal              time;       /**< Time at keyframe  */
    RtQuat              q;          /**< Quaternion rotation at keyframe  */
    RwV3d               t;          /**< Translation at keyframe  */
};

RwReal BlendTime = 1.0f;
RwReal BlendDuration = 0.2f;
/*
 *****************************************************************************
 */
RpHAnimAnimation *AlHAnimLoad(RwChar *file)
{
	RpHAnimAnimation *anim = NULL;
	RwChar *path = NULL;

	path = RsPathnameCreate(file);
	anim = RpHAnimAnimationRead(path);
	RsPathnameDestroy(path);

	return anim;
}


/*
 *****************************************************************************
 */
RwFrame *AlHAnimGetChildFrameHierarchy(RwFrame *frame, void *data)
{	
	RpHAnimHierarchy **hierarchy = (RpHAnimHierarchy **) data;

	/*
	 * Return the first hierarchy found that is attached to one of the atomic
	 * frames...
	 */

	*hierarchy = RpHAnimFrameGetHierarchy(frame);
	if( *hierarchy == NULL )
	{
		RwFrameForAllChildren(frame, AlHAnimGetChildFrameHierarchy, data);

		return frame;
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpHAnimHierarchy *AlHAnimGetHierarchy(RpClump *clump)
{
	RpHAnimHierarchy *hierarchy = NULL;

	RwFrameForAllChildren(RpClumpGetFrame(clump), AlHAnimGetChildFrameHierarchy, (void *)&hierarchy);

	return hierarchy;
}


/*
 *****************************************************************************
 */
RpAtomic *AlHAnimSetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
	RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
	
	return atomic;
}


/*
 *****************************************************************************
 */
RwBool AlHAnimCreate(RpClump *clump, RpHAnimHierarchy **hierarchy, RpHAnimAnimation *anim[], RwChar *path, RwInt32 *number)
{
	RwChar file[256];

	*hierarchy = AlHAnimGetHierarchy(clump);

	RpClumpForAllAtomics(clump, AlHAnimSetHierarchyForSkinAtomic, (void *)*hierarchy);

	for (*number = 0; *number < MAX_ANIMATION_NUMBER; (*number)++)
	{
		sprintf(file, "%s_%d.anm", path, *number + 1);
		anim[*number] = AlHAnimLoad(file);
		if (!anim[*number])
			break;
	}

	RpHAnimHierarchySetFlags(*hierarchy,
							 (RpHAnimHierarchyFlag)
							 ( RpHAnimHierarchyGetFlags(*hierarchy) | 
							   rpHANIMHIERARCHYUPDATELTMS |
							   rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYLOCALSPACEMATRICES) );

	RpHAnimHierarchyAttach(*hierarchy);

	if (anim[0])
	{
		RpHAnimHierarchySetCurrentAnim(*hierarchy, anim[0]);
		RpHAnimUpdateHierarchyMatrices(*hierarchy);
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
void AlHAnimDestroy()
{
	RwInt32 index;
	RwInt32 index2;

	for (index = 0; index < MAX_CHARACTER_TEMPLATE_NUMBER; index++)
	{
		for (index2 = 0; index2 < MAX_ANIMATION_NUMBER; index2++)
		{
			if( CharacterTemplates[index].pAnim[index2] )
			{
//				RpHAnimAnimationDestroy(CharacterTemplates[index].pAnim[index2]);
			}
		}
	}

	return;
}


/*
 *****************************************************************************
 */
RpAtomic *AlHAnimGetSkinHierarchy(RpAtomic *atomic, void *data)
{
	*(void **)data = (void *)RpSkinAtomicGetHAnimHierarchy(atomic);
	
	return NULL;
}

RpHAnimHierarchy *SetNextAnimation(RpHAnimHierarchy *hierarchy, void *data)
{
	stCharacter *character = (stCharacter *) data;

	RpHAnimHierarchySetAnimLoopCallBack(character->pInHierarchy, NULL, NULL);

	RpHAnimHierarchySetCurrentAnim(character->pInHierarchy, CharacterTemplates[character->nTemplate].pAnim[character->nNextAnim]);
	RpHAnimUpdateHierarchyMatrices(character->pInHierarchy);
	RpHAnimHierarchySetCurrentAnimTime(character->pInHierarchy, 0);

	character->nCurrentAnim = character->nNextAnim;

	RpClumpForAllAtomics(character->pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)character->pInHierarchy);

	return hierarchy;
}


/*
 *****************************************************************************
 */
void AlHAnimSetNextAnimation(void *character, RwInt32 index)
{
	stCharacter *ch = (stCharacter *) character;
	ch->nNextAnim = index;

//	RpHAnimHierarchySetAnimLoopCallBack(ch->pInHierarchy, SetNextAnimation, (void *) ch);
}

/*
 *****************************************************************************
 */
void AlHAnimUpdateAnimation(RpHAnimHierarchy *hierarchy, RwReal deltaTime, RwBool move, RwBool additional)
{
	if( hierarchy )
	{
		if (additional)
		{
			RpHAnimHierarchyAddAnimTime(hierarchy, deltaTime);
			RpHAnimHierarchyUpdateMatrices(hierarchy);
		}
		else
		{
			if( move )
			{
				if (hierarchy == Characters[CurrentPlayer].pInHierarchy)
				{
					if (BlendTime)
						RpClumpForAllAtomics(Characters[CurrentPlayer].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)hierarchy);

					BlendTime = 0;
				}

				RpHAnimHierarchyAddAnimTime(hierarchy, deltaTime);
				RpHAnimHierarchyUpdateMatrices(hierarchy);
			}
			else
			{
				if (hierarchy == Characters[CurrentPlayer].pInHierarchy && Characters[CurrentPlayer].pInHierarchy2 && Characters[CurrentPlayer].pOutHierarchy && BlendTime < BlendDuration)
				{
					RpClumpForAllAtomics(Characters[CurrentPlayer].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)Characters[CurrentPlayer].pOutHierarchy);

					RpHAnimHierarchyBlend(Characters[CurrentPlayer].pOutHierarchy, hierarchy, Characters[CurrentPlayer].pInHierarchy2, BlendTime / BlendDuration);
					BlendTime += deltaTime;

					RpHAnimHierarchyUpdateMatrices(Characters[CurrentPlayer].pOutHierarchy);
				}
				else
				{
					if (BlendTime)
						RpClumpForAllAtomics(Characters[CurrentPlayer].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)hierarchy);

//					BlendTime = 0;

					RpHAnimHierarchySetCurrentAnimTime (hierarchy, 0);
					RpHAnimHierarchyUpdateMatrices(hierarchy);
				}
			}
		}
	}
	
	return;
}

/*
 *****************************************************************************
 */
