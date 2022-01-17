// MaterialList.h: interface for the AcMaterialList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_)
#define AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcTextureList.h"
#include "ApmMap.h"
#include "AuList.h"

#define	ALEF_MATERIAL_LIST_NO_MATERIAL	(0)

#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_AMBIENT	0.3f
#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_DIFFUSE	0.7f
#define	ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_SPECULAR	0.0f

struct RtWorldImport;

class AcMaterialList  
{
public:
	static AcTextureList *	m_pTextureList	;
	RpGeometry *	m_pGeometry		;
	
	struct	MaterialData
	{
		bool			bDeleted			;	// 사용하지 않음을 체크하는 플래그.
		RpMaterial *	pMateterial			;
		int				matindex			;
		UINT32			pIndex[ TD_DEPTH ]	;	// 익덱스 저정 정보 변경.
	};

	AuList< MaterialData >	list;	// 데이타는 여기다가 저장함.
										// 여기 들어가는 데이타는 월드를 생성하면서 Material 을 추가하면서 인덱스를 얻어야하기때문에
										// WorldImport를 생성하는 MainWindow::CreateMapSectorImport 에서 작성한다.

	INT32	GetMatIndex		(	UINT32 firsttextureindex								,
								UINT32 secondtextureindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 thirdtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 fourthtextureindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 fifthtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	,
								UINT32 sixthtexutreindex	= ALEF_TEXTURE_NO_TEXTURE	);

	INT32	GetMatIndex		(	UINT32 * pIndex	);

	void	RemoveTexture	( int matindex		);
	void	RemoveTexture	( RpMaterial * pMat	);
	void	RemoveAll		(					);

	AcMaterialList();
	virtual ~AcMaterialList();

protected:

	MaterialData *	Find			(	int			matindex		);									// 머티리얼 인덱스로 찾기.
	MaterialData *	Find			(	RpMaterial *	pMat		);									// 머티리얼 포인터로 찾기.
	MaterialData *	Find			(	UINT32 firsttextureindex		,
										UINT32 secondtextureindex		,
										UINT32 thirdtexutreindex		,
										UINT32 fourthtextureindex		,
										UINT32 fifthtexutreindex		,
										UINT32 sixthtextureindex		);// 텍스쳐 인덱스로 찾기.
	MaterialData *	Find			(	UINT32 * pIndex		);// 텍스쳐 인덱스로 찾기.
	MaterialData *	GetEmptyMaterial( void							);									// 비어있는 Material을 얻어냄.
};

#endif // !defined(AFX_MATERIALLIST_H__A54B2F06_7695_41FA_8970_1FDB56636CD4__INCLUDED_)
