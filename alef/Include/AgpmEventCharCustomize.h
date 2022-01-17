#ifndef	__AGPMEVENTCHARCUSTOMIZE_H__
#define	__AGPMEVENTCHARCUSTOMIZE_H__

#include "ApDefine.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventCharCustomizeD" )
#else
#pragma comment ( lib , "AgpmEventCharCustomize" )
#endif
#endif

typedef enum _AgpmEventCharCustomizeOperation {
	AGPMEVENT_CHARCUSTOMIZE_REQUEST				= 0,
	AGPMEVENT_CHARCUSTOMIZE_RESPONSE,
	AGPMEVENT_CHARCUSTOMIZE_BUY,
} AgpmEventCharCustomizeOperation;

typedef enum _AgpmEventCharCustomizeCBID {
	AGPMEVENT_CHARCUSTOMIZE_GRANT				= 0,
	AGPMEVENT_CHARCUSTOMIZE_AFTER_UPDATE
} AgpmEventCharCustomizeCBID;

#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_TYPE			"Type"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_NUMBER		"Number"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_NAME			"Sell Name"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_CASE			"Case"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_CHRACTERTID	"CharacterTID"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_USELEVEL		"UseLevel"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_PRICE_MONEY	"Price(Money)"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_PRICE_SKULL	"Price(Skull)"

#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_HAIR			"Hair"
#define	AGPMEVENTCHARCUSTOMIZE_STREAM_NAME_FACE			"Face"

#define	CHARCUSTOMIZE_MAX_NAME							64
#define	CHARCUSTOMIZE_MAX_LIST							800
	// ¸¶°íÀÚ : Ä¿½ºÅÍ¸¶ÀÌÂ¡ °¹¼ö°¡ ´Ã¾î¼­ ¹è¿­À» ´Ã·ÁÁÜ..

typedef enum _CharCustomizeCase {
	CHARCUSTOMIZE_CASE_ALL				= 0,
	CHARCUSTOMIZE_CHAR_INGAME			= 1,
	CHARCUSTOMIZE_CHAR_LOGIN			= 2,
	CHARCUSTOMIZE_NONE,
} CharCustomizeCase;

typedef enum _CharCustomizeType {
	CHARCUSTOMIZE_TYPE_HAIR				= 1,
	CHARCUSTOMIZE_TYPE_FACE,
	CHARCUSTOMIZE_TYPE_MAX
} CharCustomizeType;

typedef struct _CharCustomizeList {
	CharCustomizeType		m_eType;
	INT32					m_lNumber;
	CHAR					m_szName[CHARCUSTOMIZE_MAX_NAME];
	AgpdCharacterTemplate	*m_pcsCharacterTemplate;
	INT32					m_lUseLevel;
	INT32					m_lPriceMoney;
	INT32					m_lPriceSkull;
	CharCustomizeCase		m_eCase;
} CharCustomizeList;

class AgpmItem;
class AgpmGrid;

class AgpmEventCharCustomize : public ApModule {
private:
	ApmEventManager		*m_pcsApmEventManager;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmGrid			*m_pcsAgpmGrid;

public:
	AuPacket			m_csPacket;

	ApSafeArray<CharCustomizeList, CHARCUSTOMIZE_MAX_LIST> m_astCustomizeList;

private:
	INT32	GetPriceWithTax(INT32 taxRatio, INT32 price);
	BOOL	OnOperationRequest(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent);
	BOOL	OnOperationResponse(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent);
	BOOL	OnOperationBuy(AgpdCharacter *pcsCharacter, ApdEvent *pcsEvent, INT8 cNewFaceIndex, INT8 cNewHairIndex);

	BOOL	CheckCharacterStatus(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter);
	BOOL	CheckValidRange(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter, AuPOS *pstDestPos);

public:
	AgpmEventCharCustomize();
	virtual ~AgpmEventCharCustomize();

	BOOL	OnAddModule();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID	MakePacketRequestEvent(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength);
	PVOID	MakePacketResponseEvent(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength);

	PVOID	MakePacketRequestCustomize(INT32 lCID, ApdEvent *pcsEvent, INT32 lFaceIndex, INT32 lHairIndex, INT16 *pnPacketLength);

	static BOOL	CBActionEventCharCustomize(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAfterUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	StreamReadCustomizeList(CHAR *szFile, BOOL bDecryption);

	CharCustomizeCase	GetCustomizeCase(INT32 lRace, INT32 lClass, CharCustomizeType eType, INT32 lNumber);

	int		GetItemCount(AgpdCharacter* pcsCharacter, int tid);
	bool	UsingCustomizingItem(AgpdCharacter* pcsCharacter, const int tid, const int count);
};

#endif	//__AGPMEVENTCHARCUSTOMIZE_H__