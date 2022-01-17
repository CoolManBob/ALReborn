#ifndef _AGPPITEM_H
#define _AGPPITEM_H

class PACKET_ITEM : public AuPacket
{
public:
	PACKET_ITEM()
	{
	};

	VOID Initialize()
	{
		SetFlagLength(sizeof(INT32));
		SetFieldType(
								AUTYPE_INT8,           1, // Operation
								AUTYPE_INT8,           1, // Status
								AUTYPE_INT32,          1, // Item ID
								AUTYPE_INT32,          1, // Item Template ID
								AUTYPE_INT32,		   1, // Item Owner CID
								AUTYPE_INT32,		   1, // Item Count
								AUTYPE_PACKET,         1, // Field
								AUTYPE_PACKET,         1, // Inventory
								AUTYPE_PACKET,         1, // Bank
								AUTYPE_PACKET,         1, // Equip
								AUTYPE_PACKET,		   1, // Factors
								AUTYPE_PACKET,		   1, // Percent Factors
								AUTYPE_INT32,		   1, // target character id (case usable (scroll) item)
								AUTYPE_PACKET,		   1, // Convert packet
								AUTYPE_PACKET,		   1, // restrict factor packet
								AUTYPE_PACKET,		   1, // Ego Item packet
								AUTYPE_PACKET,		   1, // Quest Item packet
								AUTYPE_INT32,		   1, // skill template id
								AUTYPE_UINT32,		   1, // reuse time for reverse orb
								AUTYPE_INT32,		   1, // status flag
								AUTYPE_PACKET,		   1, // option packet
								AUTYPE_PACKET,		   1, // skill plus packet
								AUTYPE_UINT32,		   1, // reuse time for transform
								AUTYPE_PACKET,		   1, // CashItem adding information
								AUTYPE_PACKET,			1, // Extra information
								AUTYPE_END,            0
								);
	};

	PVOID MakePacket(BOOL bPacket, INT16* pnPacketLength, ePacketType pPacketType, 
							INT8* cOperation,
   		                    INT8* Status,
							INT32* ItemID,
							INT32* ItemTID,
							INT32* CID,
							INT32* ItemCount,
							PVOID Field,
							PVOID Inventory,
							PVOID Bank,
							PVOID Equip,
							PVOID Factors,
							PVOID Percent,
							INT32* targetcid,
							PVOID Convert,
							PVOID restrictfactor,
							PVOID Ego,
							PVOID Quest,
							INT32* skilltid,
							UINT32* reusetimereverse,
							INT32* statusflag,
							PVOID option,
							PVOID skillplus,
							UINT32* reusetimetransform,
							PVOID CashItem,
							PVOID Extra
							)
	{
		return AuPacket::MakePacket(TRUE, pnPacketLength, pPacketType,
									cOperation,
									Status,
									ItemID,
									ItemTID,
									CID,
									ItemCount,
									Field,
									Inventory,
									Bank,
									Equip,
									Factors,
									Percent,
									targetcid,
									Convert,
									restrictfactor,
									Ego,
									Quest,
									skilltid,
									reusetimereverse,
									statusflag,
									option,
									skillplus,
									reusetimetransform,
									CashItem,
									Extra
									);
	}

	INT16 GetField(	BOOL bIsPacket, PVOID pvPacket, INT16 nPacketLength,
				INT8*		cOperation,
				INT8*      cStatus,
				INT32*		lIID,
				INT32*		lTID,
				INT32*       lCID,
				INT32*		lItemCount,
				PVOID       pField,
				PVOID       pInventory,
				PVOID		pBank,
				PVOID       pEquip,
				PVOID		pFactor,
				PVOID		pFactorPercent,
				INT32*		lTargetID,
				PVOID		pConvert,
				PVOID		pRestrictFactor,
				PVOID		pEgo,
				PVOID		pQuest,
				INT32*		lSkillTID,
				UINT32*		ulReuseTImeForReverseOrb,
				INT32*		lStatusFlag,
				PVOID		pOption,
				PVOID		pSkillPlus,
				UINT32*		ulReuseTimeForTransform,
				PVOID		pCashInformation,
				PVOID		pExtraInformation)
	{
		return AuPacket::GetField(bIsPacket, pvPacket, nPacketLength,  
						cOperation,
						cStatus,
						lIID,
						lTID,
						lCID,
						lItemCount,
						pField,
						pInventory,
						pBank,
						pEquip,
						pFactor,
						pFactorPercent,
						lTargetID,
						pConvert,
						pRestrictFactor,
						pEgo,
						pQuest,
						lSkillTID,
						ulReuseTImeForReverseOrb,
						lStatusFlag,
						pOption,
						pSkillPlus,
						ulReuseTimeForTransform,
						pCashInformation,
						pExtraInformation);
	}
};

struct PACKET_AGPMITEM_EXTRA 
{
	INT32 Type;

	union
	{
		AgpdSealData SealData; // Type 1
	} ExtraData;

	PACKET_AGPMITEM_EXTRA()
	{
		Type = 0;
		memset(&ExtraData, 0, sizeof(ExtraData));
	};
};

#endif // _AGPPITEM_H