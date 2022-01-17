#include "headers.h"

#define MAX_CHARACTER_NUMBER			40
#define MAX_CHARACTER_TEMPLATE_NUMBER	10
#define MAX_ANIMATION_NUMBER			5

RwBool AlCharacterTemplateCreate();
RwBool AlCharacterCreateOne(RpWorld *world, RwV3d *pos, RwInt32 type);
RwBool AlCharacterCreate(RpWorld *world);
void AlCharacterTemplateDestroy();
void AlCharacterDestroy(RpWorld *world);
void AlCharacterUpdate(RpWorld *world, RwReal deltaTime);
void AlCharacterDirection(RwReal deltaTime);
RwBool AlCharacterChangePart();
void AlCharacterAdd(RwV3d position);
void AlCharacterSetAnim(RwInt32 index);
void AlCharacterSetPlayer(RwBool next);
void AlCharacterMove(RwReal x, RwReal y, RwReal z);
void AlCharacterRender(RpWorld *world);

extern RwInt32 CharacterNumber;

typedef struct _stCharacterTemplate
{
	RpClump *pClump;

	RpHAnimHierarchy *pHierarchy;

	RpHAnimAnimation *pAnim[MAX_ANIMATION_NUMBER];

	RwSphere ClumpBoundingSphere;
	RwBBox ClumpBoundingBox;

	RwInt32 nTotalAnim;
	RwReal fOffset;
} stCharacterTemplate;

typedef struct _stCharacter
{
	RwInt32 nTemplate;

	RpClump *pClump;
	RwFrame *pFrame;

	RpHAnimHierarchy *pInHierarchy;
	RpHAnimHierarchy *pInHierarchy2;
	RpHAnimHierarchy *pOutHierarchy;

	RwSphere ClumpWorldBoundingSphere;
	RwBBox ClumpWorldBoundingBox;

	RwInt32 nCurrentAnim;
	RwInt32 nNextAnim;
} stCharacter;

typedef struct _stArmor
{
	RpClump *pClump;
	RpHAnimHierarchy *pHierarchy;
} stArmor;

extern stCharacterTemplate CharacterTemplates[MAX_CHARACTER_TEMPLATE_NUMBER];
extern stCharacter Characters[MAX_CHARACTER_NUMBER];
extern stArmor Armor;
extern RwFrame *PlayerFrame;
extern RwInt32 CurrentPlayer;
extern RwInt32 TemplateNumber;