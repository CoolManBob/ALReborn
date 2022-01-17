/******************************************************************************
Module:  AuType.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/

#if !defined(__AUTYPE_H__)
#define __AUTYPE_H__

#include "ApBase.h"

typedef enum _eAUTYPE {
	AUTYPE_CHAR			= 0,
	AUTYPE_INT8,
	AUTYPE_UINT8,
	AUTYPE_INT16,
	AUTYPE_UINT16,
	AUTYPE_INT32,
	AUTYPE_UINT32,
	AUTYPE_INT64,
	AUTYPE_UINT64,
	AUTYPE_FLOAT,
	AUTYPE_POS,
	AUTYPE_MATRIX,
	AUTYPE_PACKET,
	AUTYPE_MEMORY_BLOCK,
	AUTYPE_POS_BASEMETER,
	AUTYPE_WCHAR,
	AUTYPE_MAX
} eAUTYPE;
const UINT8 AUTYPE_END		= (UINT8) 0xFF;

const int AuTypeSize[AUTYPE_MAX]	= { 1, 1, 1, 2, 2, 4, 4, 8, 8, sizeof(FLOAT), sizeof(AuPOS), sizeof(AuMATRIX), 8, 0, sizeof(AuPOSBaseMeter), sizeof(wchar_t) };

#endif //__AUTYPE_H__