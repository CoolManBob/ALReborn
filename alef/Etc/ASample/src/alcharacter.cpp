#include "alcharacter.h"
#include "alclump.h"
#include "alcollision.h"
#include "alhanim.h"
#include "alcamera.h"

#include "alshadow.h"

#define SPEED (30.0f)

void AlCharacterUpdateSkin(RpGeometry *geometry, RpHAnimHierarchy *hierarchy);

stCharacterTemplate CharacterTemplates[MAX_CHARACTER_TEMPLATE_NUMBER];
stCharacter Characters[MAX_CHARACTER_NUMBER];
stArmor Armor;
RwFrame *PlayerFrame;
RwInt32 TemplateNumber = 0;
RwInt32 CharacterNumber = 0;
RwInt32 CurrentPlayer = 0;

extern RwBool ShadowEnabled;

static RwV3d Xaxis = {1.0f, 0.0f, 0.0f};
static RwV3d Yaxis = {0.0f, 1.0f, 0.0f};

RwInt32 CurrentPart;
RwInt32 Index;
RwInt32 NodeID;
RwInt32 PartID;

RwChar *part_name;

RwFrame *Frama;
RpSkinType SkinType;

RwReal totalTilt = 0.0f;
RwReal totalTurn = 0.0f;

RwReal currentTilt = 0.0f;
RwReal currentTurn = 0.0f;

RwV3d *PlayerPosition = NULL;

RwBool Equip = TRUE;

const RwChar *NodeIDName = "NodeID";
const RwChar *PartIDName = "PartID";
const RwChar *PartNameName = "PartName";

extern RpWorld *World;
extern RwInt32 WorldIndex;

RwInt32 I;

/*
 *****************************************************************************
 */
RwBool AlCharacterCreateOne(RpWorld *world, RwV3d *pos, RwInt32 type)
{
	RpHAnimHierarchyFlag flags;
	RwFrame *frame;
	const RwBBox *bboxWorld = NULL;
	RwV3d delta;
	RwV3d *position;
	RwMatrix *matrix;
	RwV3d rotate = {1.0f, 0.0f, 0.0f};
	RwV3d scale;

	if (CharacterNumber >= MAX_CHARACTER_NUMBER)
		return FALSE;

	if (type >= TemplateNumber)
		return FALSE;

	flags = (RpHAnimHierarchyFlag) (CharacterTemplates[type].pHierarchy)->flags;

	Characters[CharacterNumber].nTemplate = type;
	Characters[CharacterNumber].pClump = RpClumpClone(CharacterTemplates[type].pClump);
	Characters[CharacterNumber].pInHierarchy = AlHAnimGetHierarchy(Characters[CharacterNumber].pClump);
	RpClumpForAllAtomics(Characters[CharacterNumber].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *) Characters[CharacterNumber].pInHierarchy);

#ifdef _USE_DPVS_
//	RpDPVSWorldAddNonOccludingClump(World, Characters[CharacterNumber].pClump, rpDPVS_USE_BOUNDING_BOX);
#else
//	RpWorldAddClump(world, Characters[CharacterNumber].pClump);
#endif _USE_DPVS_

	flags = (RpHAnimHierarchyFlag) (Characters[CharacterNumber].pInHierarchy)->flags;

	Characters[CharacterNumber].pOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(Characters[CharacterNumber].pInHierarchy,
		flags, 100);
	Characters[CharacterNumber].pInHierarchy2 = RpHAnimHierarchyCreateFromHierarchy(Characters[CharacterNumber].pInHierarchy,
		flags, 100);

	Characters[CharacterNumber].pOutHierarchy->parentFrame = Characters[CharacterNumber].pInHierarchy->parentFrame;
	Characters[CharacterNumber].pInHierarchy2->parentFrame = Characters[CharacterNumber].pInHierarchy->parentFrame;

	RpHAnimHierarchySetCurrentAnim(Characters[CharacterNumber].pInHierarchy, CharacterTemplates[type].pAnim[0]);
	RpHAnimUpdateHierarchyMatrices(Characters[CharacterNumber].pInHierarchy);
	RpHAnimHierarchySetCurrentAnimTime(Characters[CharacterNumber].pInHierarchy, 0);

	RpHAnimHierarchySetCurrentAnim(Characters[CharacterNumber].pInHierarchy2, CharacterTemplates[type].pAnim[0]);
	RpHAnimHierarchySetCurrentAnimTime(Characters[CharacterNumber].pInHierarchy2, 0);
	RpHAnimUpdateHierarchyMatrices(Characters[CharacterNumber].pInHierarchy2);

	RpHAnimHierarchySetKeyFrameCallBacks(Characters[CharacterNumber].pOutHierarchy, rpHANIMSTDKEYFRAMETYPEID);

	RpHAnimHierarchyAttach(Characters[CharacterNumber].pInHierarchy);
	RpHAnimHierarchyAttach(Characters[CharacterNumber].pInHierarchy2);
	RpHAnimHierarchyAttach(Characters[CharacterNumber].pOutHierarchy);

	frame = RpClumpGetFrame(Characters[CharacterNumber].pClump);

	if( !frame )
		return FALSE;

	RwFrameSetIdentity(frame);

	Characters[CharacterNumber].pFrame = RwFrameCreate();
	RwFrameAddChild(Characters[CharacterNumber].pFrame, frame);

	if (CharacterNumber == 0)
	{
		PlayerFrame = Characters[CharacterNumber].pFrame;

		PlayerPosition = RwMatrixGetPos(RwFrameGetMatrix(PlayerFrame));
	}

	bboxWorld = RpWorldGetBBox(world);

	RwFrameTranslate(Characters[CharacterNumber].pFrame, pos, rwCOMBINEREPLACE);

	matrix = RwFrameGetMatrix(Characters[CharacterNumber].pFrame);
	position = RwMatrixGetPos(matrix);

	RwV3dAdd(&(Characters[CharacterNumber].ClumpWorldBoundingBox.inf), &(CharacterTemplates[type].ClumpBoundingBox.inf), position);
	RwV3dAdd(&(Characters[CharacterNumber].ClumpWorldBoundingBox.sup), &(CharacterTemplates[type].ClumpBoundingBox.sup), position);

	if (pos->y)
	{
		Characters[CharacterNumber].ClumpWorldBoundingBox.inf.y -= 100.0f;
		Characters[CharacterNumber].ClumpWorldBoundingBox.sup.y += 100.0f;
	}
	else
	{
		Characters[CharacterNumber].ClumpWorldBoundingBox.inf.y = -1000000.0f;
		Characters[CharacterNumber].ClumpWorldBoundingBox.sup.y = 1000000.0f;
	}

	delta.x = delta.y = delta.z = 0.0f;
	if (AlCollisionLine(position, &delta, world, &Characters[CharacterNumber].ClumpWorldBoundingBox))
	{
		delta.y -= CharacterTemplates[type].ClumpBoundingBox.inf.y - CharacterTemplates[type].fOffset;

		RwFrameTranslate(Characters[CharacterNumber].pFrame, &delta, rwCOMBINEPOSTCONCAT);
	}

	frame = RpClumpGetFrame(Characters[CharacterNumber].pClump);

	if (type > 1)
	{
		if (type == 5)
		{
			scale.x = scale.y = scale.z = 1.5f;
			RwFrameScale(frame, &scale, rwCOMBINEPRECONCAT);
		}

		RwFrameRotate(frame, &rotate, 90, rwCOMBINEPRECONCAT);
	}

	CharacterNumber++;

	return TRUE;
}


/*
 *****************************************************************************
 */
RwBool AlCharacterTemplateCreate()
{
	RwChar fileName[256];
	RwChar prefixName[256];

	memset(CharacterTemplates, 0, sizeof(stCharacterTemplate) * MAX_CHARACTER_TEMPLATE_NUMBER);

	for (TemplateNumber = 0; TemplateNumber < MAX_CHARACTER_TEMPLATE_NUMBER; TemplateNumber++)
	{
		sprintf(prefixName, "models/char_%d", TemplateNumber + 1);
		sprintf(fileName, "%s.dff", prefixName);

		CharacterTemplates[TemplateNumber].pClump = AlClumpLoad(fileName);
		if (!CharacterTemplates[TemplateNumber].pClump)
		{
			break;
		}

		if (!AlHAnimCreate(CharacterTemplates[TemplateNumber].pClump,
			 &(CharacterTemplates[TemplateNumber].pHierarchy),
			 CharacterTemplates[TemplateNumber].pAnim,
			 prefixName,
			 &CharacterTemplates[TemplateNumber].nTotalAnim))
		{
			return FALSE;
		}

		if (TemplateNumber == 5)
		{
			CharacterTemplates[TemplateNumber].fOffset = 20;
		}

		AlClumpGetBBox(CharacterTemplates[TemplateNumber].pClump, &CharacterTemplates[TemplateNumber].ClumpBoundingBox);
	}

	return TRUE;
}

RpAtomic *SetFilterMode(RpAtomic *atomic, void *data)
{
	RwTexture *texture = RpMaterialGetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0));

	if (texture)
		RwTextureSetFilterMode(texture, rwFILTERNEAREST);

	return atomic;
}

RpMaterial *MakeBlending(RpMaterial *material, void *data)
{
	RwRGBA rgba = {255, 255, 255, 255};
	RwSurfaceProperties *sp;

	sp = (RwSurfaceProperties *) RpMaterialGetSurfaceProperties(material);
	sp->ambient = 1.0;
	sp->specular = 0.0;
	sp->diffuse = 1.0;
	RpMaterialSetSurfaceProperties(material, sp);

	RpMaterialSetColor(material, &rgba);

	return material;
}

RpAtomic *UpdateSkin(RpAtomic *atomic, void *data)
{
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);

	RpGeometrySetFlags(geometry, RpGeometryGetFlags(geometry) | rpGEOMETRYMODULATEMATERIALCOLOR);

	RpGeometryForAllMaterials(geometry, MakeBlending, (void *) atomic);

//	PatchUtil(atomic);

	if (RpSkinGeometryGetSkin(geometry))
		AlCharacterUpdateSkin(geometry, Characters[0].pInHierarchy);

	return atomic;
}

/*
 *****************************************************************************
 */
RwBool AlCharacterCreate(RpWorld *world)
{
	RwFrame *pframe;
	RwV3d pos;
	const RwBBox *bboxWorld;

	CharacterNumber = 0;

	bboxWorld = RpWorldGetBBox(world);

	pos.x = pos.y = pos.z =1.0f;

	if (!AlCharacterCreateOne(world, &pos, 0))
		return FALSE;

	CurrentPart = 2;

	Armor.pClump = AlClumpLoad("models/armor.dff");
	if( !Armor.pClump)
		return FALSE;

	Armor.pHierarchy = AlHAnimGetHierarchy(Armor.pClump);
	if (!Armor.pHierarchy)
		return FALSE;

	pframe = RpClumpGetFrame(Armor.pClump);

	RpClumpForAllAtomics(Armor.pClump, UpdateSkin, NULL);

//	RpClumpForAllAtomics(Armor.pClump, SetFilterMode, NULL);

	return TRUE;
}

/*
 *****************************************************************************
 */
void AlCharacterTemplateDestroy()
{
	RwInt32 index;

	for (index = 0; index < MAX_CHARACTER_TEMPLATE_NUMBER; index++)
	{
		if( CharacterTemplates[index].pClump )
		{
			AlClumpDestroy(NULL, CharacterTemplates[index].pClump);
			CharacterTemplates[index].pClump = NULL;
		}
	}

	AlHAnimDestroy();
}

/*
 *****************************************************************************
 */
void AlCharacterDestroy(RpWorld *world)
{
	RwInt32 index;

	RwFrameRemoveChild(RwCameraGetFrame(Camera));

	for (index = 0; index < MAX_CHARACTER_NUMBER; index++)
	{
		if( Characters[index].pClump )
		{
			AlClumpDestroy(world, Characters[index].pClump);
			Characters[index].pClump = NULL;

			if( Characters[index].pOutHierarchy )
			{
				Characters[index].pOutHierarchy->parentFrame = NULL;

				RpHAnimHierarchyDestroy(Characters[index].pOutHierarchy);
			}

			if( Characters[index].pInHierarchy2 )
			{
				RpHAnimHierarchyDestroy(Characters[index].pInHierarchy2);
			}
		}
	}

	memset(Characters, 0, sizeof(stCharacter) * MAX_CHARACTER_NUMBER);

	if( Armor.pClump )
	{
//		AlClumpDestroy(world, Armor.pClump);

		Armor.pClump = NULL;
	}

	AlHAnimDestroy();
}

/*
 *****************************************************************************
 */
void AlCharacterUpdate(RpWorld *world, RwReal deltaTime)
{
	RwReal MoveSpeed;
	RwInt32 index;

	RwMatrix		   *_matrix = RwFrameGetMatrix(PlayerFrame);
	RwBBox				bboxChar;
	RwBBox				bboxCharWorld;

	if( bMove && Characters[CurrentPlayer].nCurrentAnim == 0 )
	{
		MoveSpeed = SPEED * deltaTime;
	}
	else
	{
		MoveSpeed = 0.0f;
	}

	if( MoveSpeed != 0.0f )
	{
		RwMatrix *matrix;
		RwV3d delta;

		matrix = RwFrameGetMatrix(PlayerFrame);
		PlayerPosition = RwMatrixGetPos(matrix);

		delta = *RwMatrixGetAt(matrix);

		RwV3dScale(&delta, &delta, MoveSpeed);

		AlCollisionConfine(PlayerPosition, &delta, world);
	
		AlClumpGetBBox(Characters[CurrentPlayer].pClump, &bboxChar);
		RwV3dAdd(&(bboxCharWorld.inf), &(bboxChar.inf), PlayerPosition);
		RwV3dAdd(&(bboxCharWorld.sup), &(bboxChar.sup), PlayerPosition);

		bboxCharWorld.inf.y -= (bboxChar.sup.y - bboxChar.inf.y) / 2;
		bboxCharWorld.sup.y += (bboxChar.sup.y - bboxChar.inf.y) / 2;

		if (AlCollisionLine(PlayerPosition, &delta, world, &bboxCharWorld))
		{
			delta.y -= bboxChar.inf.y - CharacterTemplates[Characters[CurrentPlayer].nTemplate].fOffset;
			RwFrameTranslate(PlayerFrame, &delta, rwCOMBINEPOSTCONCAT);
		}

		AlCharacterDirection(deltaTime);
	}

	for( index = 0; index < CharacterNumber; ++index )
	{
		RpClumpForAllAtomics(Characters[index].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)Characters[index].pInHierarchy);

		if (index == CurrentPlayer)
			AlHAnimUpdateAnimation(Characters[index].pInHierarchy, deltaTime, bMove, Characters[index].nCurrentAnim);
		else
			AlHAnimUpdateAnimation(Characters[index].pInHierarchy, deltaTime, 1, 0);

		RwFrameUpdateObjects(RpClumpGetFrame(Characters[index].pClump));
	}

	if( ShadowEnabled )
	{
		for( index = 0; index < CharacterNumber; ++index )
		{
			RwV3dTransformPoints(&Characters[index].ClumpWorldBoundingSphere.center,
								 &CharacterTemplates[Characters[index].nTemplate].ClumpBoundingSphere.center, 1, _matrix);

			UpdateShadow(deltaTime, Characters[index].pClump);
		}
	}

	return;
}

/*
 *****************************************************************************
 */
void AlCharacterDirection(RwReal deltaTime)
{
	RwV3d delta, pos, *right;
	RwFrame *cameraFrame;

	/*
	 * Limit the camera's tilt so that it never quite reaches
	 * exactly +90 or -90 degrees...
	 */
	if( totalTilt + currentTilt > 89.0f )
	{
		currentTilt = 89.0f - totalTilt;

	}
	else if( totalTilt + currentTilt < -89.0f )
	{
		currentTilt = -89.0f - totalTilt;
	}

	totalTurn += currentTurn;
	totalTilt += currentTilt;

	cameraFrame = RwCameraGetFrame(Camera);

	/*
	 * Remember where the camera is...
	 */
	pos = *RwMatrixGetPos(RwFrameGetMatrix(PlayerFrame));

	/*
	 * Remove the translation component...
	 */
	RwV3dScale(&delta, &pos, -1.0f);
	RwFrameTranslate(PlayerFrame, &delta, rwCOMBINEPOSTCONCAT);

	/*
	 * Rotate to the new direction...
	 */
	right = RwMatrixGetRight(RwFrameGetMatrix(cameraFrame));
	RwFrameRotate(cameraFrame, right, currentTilt, rwCOMBINEPOSTCONCAT);
	RwFrameRotate(PlayerFrame, &Yaxis, totalTurn, rwCOMBINEPOSTCONCAT);

	currentTilt = 0;
	currentTurn = 0;

	/*
	 * Put the camera back to where it was...
	 */
	RwFrameTranslate(PlayerFrame, &pos, rwCOMBINEPOSTCONCAT);

	return;
}

RpAtomic *AlCharacterCallbackAtomic(RpAtomic *atomic)
{
	return atomic;
}

void AlCharacterUpdateSkin(RpGeometry *geometry, RpHAnimHierarchy *hierarchy)
{
	RwInt32 boneIndex[256];
	RwInt32 i;
	RpSkin *skin = RpSkinGeometryGetSkin(geometry);
	RwInt32 vertices = RpGeometryGetNumVertices(geometry);
	RwUInt32 *boneIndices = (RwUInt32 *) RpSkinGetVertexBoneIndices(skin);
	RwUInt32 index;
	RwInt32 j, k;

	memset(boneIndex, 0, sizeof(RwInt32) * 256);
	for (i = 0; i < 256; i++)
	{
		j = RpHAnimIDGetIndex(hierarchy, i);
		k = RpHAnimIDGetIndex(Armor.pHierarchy, i);

		if (j >= 0 && k >= 0 && j < 256 && k < 256)
			boneIndex[k] = j;
	}

	for (i = 0; i < vertices; i++)
	{
		index = boneIndices[i] & 0xff;
		boneIndices[i] = (boneIndices[i] & 0xffffff00) | boneIndex[index];

		index = (boneIndices[i] >> 8) & 0xff;
		boneIndices[i] = (boneIndices[i] & 0xffff00ff) | (boneIndex[index] << 8);

		index = (boneIndices[i] >> 16) & 0xff;
		boneIndices[i] = (boneIndices[i] & 0xff00ffff) | (boneIndex[index] << 16);

		index = (boneIndices[i] >> 24) & 0xff;
		boneIndices[i] = (boneIndices[i] & 0x00ffffff) | (boneIndex[index] << 24);
	}
}

RpGeometry *CreateGeometry(RpAtomic *atomic, RwFrame *frame)
{
/*
	RpGeometry *geometry;
	RpGeometry *geometry2 = RpAtomicGetGeometry(atomic);
	RpSkin *skin = RpSkinGeometryGetSkin(geometry2);
	RpTriangle *tlist;
	RwV3d *vlist;
	RwV3d *normals;
	RwInt32 numVert = RpGeometryGetNumVertices(geometry2);
	RwInt32 numTri = RpGeometryGetNumTriangles(geometry2);
	RpMorphTarget *morphTarget;
	RwInt32 i;
	RpMaterial *material = RpMaterialCreate();

	geometry = RpGeometryCreate(numVert, numTri, geometry2->flags);
	morphTarget = RpGeometryGetMorphTarget(geometry, 0);
	tlist = RpGeometryGetTriangles(geometry);
	vlist = RpMorphTargetGetVertices(morphTarget);
	normals = RpMorphTargetGetVertexNormals(morphTarget);

	memcpy(tlist, RpGeometryGetTriangles(geometry2), sizeof(RpTriangle) *numTri);
	memcpy(vlist, RpMorphTargetGetVertices(RpGeometryGetMorphTarget(geometry, 0)), sizeof(RwV3d) * numVert);
	memcpy(normals, RpMorphTargetGetVertexNormals(RpGeometryGetMorphTarget(geometry, 0)), sizeof(RwV3d) * numVert);
*/
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	RwInt32 numVert = RpGeometryGetNumVertices(geometry);
	RwInt32 numTri = RpGeometryGetNumTriangles(geometry);
	RpSkin *skin = RpSkinGeometryGetSkin(geometry);
    RwMatrix *LTM = RwFrameGetLTM(frame);
	RwMatrix inverseMatrix;

	RwMatrixInvert(&inverseMatrix, LTM);

	RpSkin *skin2 = RpSkinCreate(numVert, numTri, (struct RwMatrixWeights *)RpSkinGetVertexBoneWeights(skin), (RwUInt32 *) RpSkinGetVertexBoneIndices(skin), NULL);
	const RwUInt32 *boneIndex = RpSkinGetVertexBoneIndices(skin);
	const RwUInt32 *boneIndex2 = RpSkinGetVertexBoneIndices(skin2);

	RpSkinGeometrySetSkin(geometry, skin2);

	return geometry;
}

void AlCharacterAddArmor(RpAtomic *atomic, RpSkinType skinType, RwInt32 nodeID)
{
	RpAtomic *atomic2;
	RpGeometry *geometry2;
	RpHAnimNodeInfo *nodeInfo;
	RwMatrix *matrix;
	RwFrame *pFrame2;
	RwFrame *pFrame;

	if( atomic )
	{
#ifdef _USE_DPVS_
//		RpDPVSWorldRemoveClump(World, Characters[CurrentPlayer].pClump);
#else
//		RpWorldRemoveClump(World, Characters[CurrentPlayer].pClump);
#endif _USE_DPVS_

		atomic2 = RpAtomicClone(atomic);
		geometry2 = RpAtomicGetGeometry(atomic2);

		pFrame2 = RpAtomicGetFrame(atomic);
		matrix = RwFrameGetMatrix(pFrame2);

		RpClumpAddAtomic(Characters[CurrentPlayer].pClump, atomic2);

		if (skinType != rpNASKINTYPE)
		{
/*
			AlCharacterUpdateSkin(geometry2, Characters[CurrentPlayer].pInHierarchy);
			RpSkinAtomicSetHAnimHierarchy(atomic, Characters[CurrentPlayer].pInHierarchy);

			RpAtomicSetGeometry(atomic, geometry2, 0);
*/
//			geometry2 = CreateGeometry(atomic2, Characters[CurrentPlayer].pFrame);

			RpAtomicSetFrame(atomic2, RpClumpGetFrame(Characters[CurrentPlayer].pClump));

			RpSkinAtomicSetHAnimHierarchy(atomic2, Characters[CurrentPlayer].pInHierarchy);
			RpSkinAtomicSetType(atomic2, skinType);

			RwFrameUpdateObjects(RpClumpGetFrame(Characters[CurrentPlayer].pClump));
			RpHAnimHierarchyUpdateMatrices(Characters[CurrentPlayer].pInHierarchy);
		}
		else
		{
			pFrame = RwFrameCreate();

			RpAtomicSetFrame(atomic2, pFrame);

			if (nodeID)
			{
				nodeInfo = &(Characters[CurrentPlayer].pInHierarchy->pNodeInfo[RpHAnimIDGetIndex(Characters[CurrentPlayer].pInHierarchy, NodeID)]);
				RwFrameAddChild(nodeInfo->pFrame, pFrame);
				RwFrameTransform(pFrame, matrix, rwCOMBINEREPLACE);
			}
			else
			{
				RwFrameAddChild(Characters[CurrentPlayer].pFrame, pFrame);
				RwFrameTransform(pFrame, matrix, rwCOMBINEREPLACE);
			}
		}

#ifdef _USE_DPVS_
//		RpDPVSWorldAddNonOccludingClump(World, Characters[CurrentPlayer].pClump, rpDPVS_USE_BOUNDING_BOX);
#else
//		RpWorldAddClump(World, Characters[CurrentPlayer].pClump);
#endif _USE_DPVS_
	}

}

void AlCharacterRemoveArmor(RpAtomic *atomic, RpSkinType skinType, RwInt32 nodeID)
{
	RpGeometry *geometry;
	RwFrame *pFrame;

	if( atomic )
	{
#ifdef _USE_DPVS_
//		RpDPVSWorldRemoveClump(World, Characters[CurrentPlayer].pClump);
#else
//		RpWorldRemoveClump(World, Characters[CurrentPlayer].pClump);
#endif _USE_DPVS_

		geometry = RpAtomicGetGeometry(atomic);

		pFrame = RpAtomicGetFrame(atomic);

		RpClumpRemoveAtomic(Characters[CurrentPlayer].pClump, atomic);

		if (skinType == rpNASKINTYPE)
		{
			RwFrameRemoveChild(pFrame);
			RpAtomicSetFrame(atomic, NULL);
			RwFrameDestroy(pFrame);
		}

		RwFrameUpdateObjects(RpClumpGetFrame(Characters[CurrentPlayer].pClump));
		RpHAnimHierarchyUpdateMatrices(Characters[CurrentPlayer].pInHierarchy);

#ifdef _USE_DPVS_
//		RpDPVSWorldAddNonOccludingClump(World, Characters[CurrentPlayer].pClump, rpDPVS_USE_BOUNDING_BOX);
#else
//		RpWorldAddClump(World, Characters[CurrentPlayer].pClump);
#endif _USE_DPVS_
	}
}

RpAtomic *AlCharacterEquipPart(RpAtomic *atomic, void *data)
{
	RwFrame *frame;
	RpUserDataArray *userdata;
	RwInt32 count;
	RwInt32 i;
	RpSkin *skin;
	RpGeometry *geometry;
	RwChar *name;
	RwChar *partName;

	NodeID = PartID = 0;
	part_name = NULL;

	frame = RpAtomicGetFrame(atomic);
	geometry = RpAtomicGetGeometry(atomic);
	if (RpSkinGeometryGetSkin(geometry))
		SkinType = RpSkinAtomicGetType(atomic);
	else
		SkinType = rpNASKINTYPE;

	skin = RpSkinGeometryGetSkin(geometry);
	if (skin)
		SkinType = RpSkinAtomicGetType(atomic);

	count = RpGeometryGetUserDataArrayCount(geometry);
	for (i = 0; i < count; i++)
	{
		userdata = RpGeometryGetUserDataArray(geometry, i);
		name = RpUserDataArrayGetName(userdata);

		if (!strcmp(name, PartIDName))
		{
			PartID = RpUserDataArrayGetInt(userdata, 0);
		}
		else if (!strcmp(name, NodeIDName))
		{
			NodeID = RpUserDataArrayGetInt(userdata, 0);
		}
		else if (!strcmp(name, PartNameName))
		{
			partName = RpUserDataArrayGetString(userdata, 0);
		}
	}

	if (PartID == CurrentPart)
	{
		part_name = partName;
		AlCharacterAddArmor(atomic, SkinType, NodeID);
	}

	return atomic;
}

RpAtomic *AlCharacterRemovePart(RpAtomic *atomic, void *data)
{
	RwFrame *frame;
	RpUserDataArray *userdata;
	RwInt32 count;
	RwInt32 i;
	RpSkin *skin;
	RpGeometry *geometry;
	RwChar *name;
	RwChar *partName;

	NodeID = PartID = 0;
	part_name = NULL;

	frame = RpAtomicGetFrame(atomic);
	geometry = RpAtomicGetGeometry(atomic);
	if (RpSkinGeometryGetSkin(geometry))
		SkinType = RpSkinAtomicGetType(atomic);
	else
		SkinType = rpNASKINTYPE;

	skin = RpSkinGeometryGetSkin(geometry);
	if (skin)
		SkinType = RpSkinAtomicGetType(atomic);

	count = RpGeometryGetUserDataArrayCount(geometry);
	for (i = 0; i < count; i++)
	{
		userdata = RpGeometryGetUserDataArray(geometry, i);
		name = RpUserDataArrayGetName(userdata);

		if (!strcmp(name, PartIDName))
		{
			PartID = RpUserDataArrayGetInt(userdata, 0);
		}
		else if (!strcmp(name, NodeIDName))
		{
			NodeID = RpUserDataArrayGetInt(userdata, 0);
		}
		else if (!strcmp(name, PartNameName))
		{
			partName = RpUserDataArrayGetString(userdata, 0);
		}
	}

	if (PartID == CurrentPart)
	{
		part_name = partName;
		AlCharacterRemoveArmor(atomic, SkinType, NodeID);
	}

	return atomic;
}

RwBool AlCharacterChangePart()
{

	Frama = RpClumpGetFrame(Armor.pClump);

	if (Equip)
	{
		RpClumpForAllAtomics(Armor.pClump, AlCharacterEquipPart, (void *) &CurrentPart);
		CurrentPart++;

		if (CurrentPart >= 8)
			Equip = FALSE;
	}
	else
	{
		RpClumpForAllAtomics(Characters[CurrentPlayer].pClump, AlCharacterRemovePart, (void *) &CurrentPart);
		CurrentPart--;

		if (CurrentPart < 2)
			Equip = TRUE;
	}


	return TRUE;
}

void AlCharacterAdd(RwV3d pos)
{
	const RwBBox *bbox = NULL;
	RwInt32 type;

	type = rand() % TemplateNumber;

	if (!AlCharacterCreateOne(World, &pos, type))
		return;

	return;
}

void AlCharacterSetAnim(RwInt32 index)
{
	if (index >= CharacterTemplates[Characters[CurrentPlayer].nTemplate].nTotalAnim)
		return;

	Characters[CurrentPlayer].nCurrentAnim = index;
	RpHAnimHierarchySetCurrentAnim(Characters[CurrentPlayer].pInHierarchy, CharacterTemplates[Characters[CurrentPlayer].nTemplate].pAnim[index]);
	RpHAnimUpdateHierarchyMatrices(Characters[CurrentPlayer].pInHierarchy);
	RpHAnimHierarchySetCurrentAnimTime(Characters[CurrentPlayer].pInHierarchy, 0);

	RpClumpForAllAtomics(Characters[CurrentPlayer].pClump, AlHAnimSetHierarchyForSkinAtomic, (void *)Characters[CurrentPlayer].pInHierarchy);

	AlHAnimSetNextAnimation(&Characters[CurrentPlayer], 0);
}

void AlCharacterSetPlayer(RwBool next)
{
	if (next)
		CurrentPlayer++;
	else
		CurrentPlayer--;

	if (CurrentPlayer >= CharacterNumber)
		CurrentPlayer = 0;

	if (CurrentPlayer < 0)
		CurrentPlayer = CharacterNumber - 1;

	RwFrameRemoveChild(RwCameraGetFrame(Camera));

	PlayerFrame = Characters[CurrentPlayer].pFrame;

	RwFrameAddChild(PlayerFrame, RwCameraGetFrame(Camera));

	AlCameraReset(Camera);
}

void AlCharacterMove(RwReal x, RwReal y, RwReal z)
{
	RwFrame *frame;
	RwMatrix *matrix;
	RwV3d *position;
	RwV3d pos = {x, y, z};
	RwV3d delta;
	RwInt32 type = Characters[CurrentPlayer].nTemplate;

	frame = Characters[CurrentPlayer].pFrame;

	RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);

	matrix = RwFrameGetMatrix(frame);
	position = RwMatrixGetPos(matrix);

	RwV3dAdd(&(Characters[CurrentPlayer].ClumpWorldBoundingBox.inf), &(CharacterTemplates[type].ClumpBoundingBox.inf), position);
	RwV3dAdd(&(Characters[CurrentPlayer].ClumpWorldBoundingBox.sup), &(CharacterTemplates[type].ClumpBoundingBox.sup), position);

	if (y != 0.0f)
	{
		Characters[CurrentPlayer].ClumpWorldBoundingBox.inf.y -= 100.0f;
		Characters[CurrentPlayer].ClumpWorldBoundingBox.sup.y += 100.0f;
	}
	else
	{
		Characters[CurrentPlayer].ClumpWorldBoundingBox.inf.y = -1000000.0f;
		Characters[CurrentPlayer].ClumpWorldBoundingBox.sup.y = 1000000.0f;
	}

	delta.x = delta.y = delta.z = 0.0f;
	if (AlCollisionLine(position, &delta, World, &Characters[CurrentPlayer].ClumpWorldBoundingBox))
	{
		delta.y -= CharacterTemplates[type].ClumpBoundingBox.inf.y;

		RwFrameTranslate(frame, &delta, rwCOMBINEPOSTCONCAT);
	}
}

void AlCharacterRender(RpWorld *world)
{
	RwInt32 index;

	for( index = 0; index < CharacterNumber; index++ )
	{
		RpClumpRender(Characters[index].pClump);
	}
}