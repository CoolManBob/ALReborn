// ApMapBlocking.h: interface for the ApMapBlocking class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_)
#define AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApModule.h"

////////////////////////////////////////////////////////////////////
// class	: ApMapBlocking
// Created  : 마고자 (2002-04-24 오후 3:24:10)
// Note     : 맵모듈에서 블러킹 요소 관리하기 위한 녀석.
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

enum	BlockingType
{
	TYPE_NONE		,	// 초기화 돼지 않았음;.
	TYPE_BOX		,
	TYPE_SPHERE		,
	TYPE_CYLINDER
};


class ApMapBlocking  
{
public:

public:
	// Attribute
	int		type;	// Blocking Type 저장함. 

	// 실제 블러킹에 대한 정보.
	union	data
	{
		AuBOX			box			;
		AuSPHERE		sphere		;
		AuCYLINDER		cylinder	;
	};

	// Operations
	ApMapBlocking();
	virtual ~ApMapBlocking();

	BOOL IsPassThis( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL );
	BOOL IsInTriangle( AuPOS pos );

};

#endif // !defined(AFX_APMAPBLOCKING_H__AFE7CD7A_73BF_447F_BECE_FD97DB52D333__INCLUDED_)
