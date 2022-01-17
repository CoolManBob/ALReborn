#ifndef	__AGSMAOIMAP_H__
#define	__AGSMAOIMAP_H__

#include "AgsmAOICell.h"

#define	AGSMAOIMAP_WORLD_INDEX_WIDTH	3200			// 전체맵 크기의 넓이.	x축 
#define	AGSMAOIMAP_WORLD_INDEX_HEIGHT	3200			// 전체맵 크기의 높이.	z축
#define	AGSMAOIMAP_CELL_WIDTH			(1600.0f)		// 한셀의 너비.
#define	AGSMAOIMAP_CELL_HEIGHT			(1600.0f)		// 한셀의 높이.

// Utility Inline Functions..
inline	INT32 ArrayIndexToCellIndexX	( INT32 arrayindex_X		) { return  arrayindex_X	- ( INT32 ) ( AGSMAOIMAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 ArrayIndexToCellIndexZ	( INT32 arrayindex_Z		) { return  arrayindex_Z	- ( INT32 ) ( AGSMAOIMAP_WORLD_INDEX_HEIGHT		>> 1 ); }
inline	INT32 CellIndexToArrayIndexX	( INT32 cellindex_X			) { return  cellindex_X		+ ( INT32 ) ( AGSMAOIMAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 CellIndexToArrayIndexZ	( INT32 cellindex_Z			) { return  cellindex_Z		+ ( INT32 ) ( AGSMAOIMAP_WORLD_INDEX_HEIGHT		>> 1 ); }

// 글로벌 위치로 섹터 인덱스 얻어내기..
inline	INT32 PosToCellIndexX			( FLOAT posX	)
{
	if( posX >= 0.0f )	return ( INT32 ) ( posX / AGSMAOIMAP_CELL_WIDTH  )							;
	else				return ( INT32 ) ( ( posX - AGSMAOIMAP_CELL_WIDTH + 1.0f	) / AGSMAOIMAP_CELL_WIDTH )	;
}
inline	INT32 PosToCellIndexX			( AuPOS pos		) { return PosToCellIndexX( pos.x ); }
inline	INT32 PosToCellIndexZ			( FLOAT posZ	)
{
	if( posZ >= 0.0f )	return ( INT32 ) ( posZ / AGSMAOIMAP_CELL_HEIGHT )							;
	else				return ( INT32 ) ( ( posZ - AGSMAOIMAP_CELL_HEIGHT + 1.0f	) / AGSMAOIMAP_CELL_HEIGHT )	;
}
inline	INT32 PosToCellIndexZ			( AuPOS pos		) { return PosToCellIndexZ( pos.z ); }

class AgsmAOIMap {
private:
	AgsmAOICell			*m_ppCell[AGSMAOIMAP_WORLD_INDEX_WIDTH][AGSMAOIMAP_WORLD_INDEX_HEIGHT];

public:
	AgsmAOIMap();
	~AgsmAOIMap();

	BOOL				AddCharacterToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit);
	BOOL				RemoveCharacterFromMap(AgsmCellUnit *pstUnit);
	AgsmAOICell*		UpdateCharacterPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit);

	BOOL				AddNPCToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit, BOOL bIsSiegeWarCharacter);
	BOOL				RemoveNPCFromMap(AgsmCellUnit *pstUnit);
	AgsmAOICell*		UpdateNPCPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit);

	BOOL				AddMonsterToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit, BOOL bIsSiegeWarCharacter);
	BOOL				RemoveMonsterFromMap(AgsmCellUnit *pstUnit);
	AgsmAOICell*		UpdateMonsterPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit);

	BOOL				AddItemToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit);
	BOOL				RemoveItemFromMap(AgsmCellUnit *pstUnit);

	inline AgsmAOICell*		GetCell(AuPOS *pstPos)
	{
		if (!pstPos)
			return NULL;

		return GetCell(CellIndexToArrayIndexX(PosToCellIndexX(pstPos->x)), CellIndexToArrayIndexZ(PosToCellIndexZ(pstPos->z)));
	}

	inline AgsmAOICell*		GetCell(INT32 lIndexX, INT32 lIndexZ)
	{
		if (lIndexX < 0 || lIndexZ < 0 ||
			lIndexX >= AGSMAOIMAP_WORLD_INDEX_WIDTH ||
			lIndexZ >= AGSMAOIMAP_WORLD_INDEX_HEIGHT)
			return NULL;

		if (!m_ppCell[lIndexX][lIndexZ])
		{
			m_ppCell[lIndexX][lIndexZ]	= new (AgsmAOICell);
			if (!m_ppCell[lIndexX][lIndexZ])
				return NULL;

			m_ppCell[lIndexX][lIndexZ]->Initialize(lIndexX, lIndexZ);
		}

		return m_ppCell[lIndexX][lIndexZ];
	}

	BOOL				NoticeActiveStatus(AgsmAOICell *pcsCell);
	BOOL				NoticeDisableStatus(AgsmAOICell *pcsCell);
};

#endif	//__AGSMAOIMAP_H__