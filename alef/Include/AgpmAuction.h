/*====================================================================

	AgpmAuction.h
	
====================================================================*/


#ifndef  _AGPM_AUCTION_H_
	#define _AGPM_AUCTION_H_


#include "ApBase.h"
#include "ApModule.h"
#include "AgpdAuction.h"
#include "AgpaAuction.h"
#include "ApmMap.h"
#include "ApmEventManager.h"
#include "AgpmGrid.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define	MAX_QUERY_LENGTH				512
#define	MAX_HEADER_STRING_LENGTH		512
#define	MAX_COLUMN_SIZE					32

#define AGPMAUCTION_MAX_ROW_PER_PAGE				15

#define AGPMEVENT_AUCTION_INI_NAME_START			"AuctionStart"
#define AGPMEVENT_AUCTION_INI_NAME_END				"AuctionEnd"
#define AGPMEVENT_AUCTION_INI_HAVE_EVENT			"HaveAuctionEvent"

#define AGPMAUCTION_SELECT_COLUMN_DOCID				0
#define AGPMAUCTION_SELECT_COLUMN_SELLER			1
#define AGPMAUCTION_SELECT_COLUMN_ITEMSEQ			2
#define AGPMAUCTION_SELECT_COLUMN_PRICE				3
#define AGPMAUCTION_SELECT_COLUMN_QUANTITY			4
#define AGPMAUCTION_SELECT_COLUMN_ITEMTID			5
#define AGPMAUCTION_SELECT_COLUMN_STACKCOUNT		6
#define AGPMAUCTION_SELECT_COLUMN_NEEDLEVEL			7
#define AGPMAUCTION_SELECT_COLUMN_CONVHIST			8
#define AGPMAUCTION_SELECT_COLUMN_DURABILITY		9
#define AGPMAUCTION_SELECT_COLUMN_MAX_DURABILITY	10
#define AGPMAUCTION_SELECT_COLUMN_OPTION			11
#define AGPMAUCTION_SELECT_COLUMN_SKILLPLUS			12

enum eAGPMAUCTION_DATATYPE
	{
	AGPMAUCTION_DATATYPE_SALES = 0,
	};

enum eAGPMAUCTION_OPERATION
	{
	AGPMAUCTION_OPERATION_NONE = 0,
	AGPMAUCTION_OPERATION_ADD_SALES,
	AGPMAUCTION_OPERATION_REMOVE_SALES,
	AGPMAUCTION_OPERATION_UPDATE_SALES,
	AGPMAUCTION_OPERATION_SELECT,
	AGPMAUCTION_OPERATION_SELL,
	AGPMAUCTION_OPERATION_CANCEL,
	AGPMAUCTION_OPERATION_CONFIRM,
	AGPMAUCTION_OPERATION_BUY,
	AGPMAUCTION_OPERATION_NOTIFY,
	AGPMAUCTION_OPERATION_EVENT_REQUEST,
	AGPMAUCTION_OPERATION_EVENT_GRANT,
	AGPMAUCTION_OPERATION_LOGIN,
	AGPMAUCTION_OPERATION_SELECT2,
	AGPMAUCTION_OPERATION_REQUEST_ALL_SALES,
	AGPMAUCTION_OPERATION_OPEN_ANYWHERE,
	AGPMAUCTION_OPERATION_MAX
	};

enum AGPMAUCTION_CB
	{
	AGPMAUCTION_CB_NONE = 0,
	AGPMAUCTION_CB_ADD_SALES,
	AGPMAUCTION_CB_REMOVE_SALES,
	AGPMAUCTION_CB_UPDATE_SALES,
	AGPMAUCTION_CB_SELECT,
	AGPMAUCTION_CB_SELL,
	AGPMAUCTION_CB_CANCEL,
	AGPMAUCTION_CB_CONFIRM,
	AGPMAUCTION_CB_BUY,
	AGPMAUCTION_CB_NOTIFY,
	AGPMAUCTION_CB_OPEN_AUCTION,
	AGPMAUCTION_CB_LOGIN,
	AGPMAUCTION_CB_SELECT2,
	AGPMAUCTION_CB_REQUEST_ALL_SALES,
	AGPMAUCTION_CB_OPEN_ANYWHERE,
	AGPMAUCTION_CB_MAX
	};

enum eAGPMAUCTION_EXCPT
	{
	AGPMAUCTION_EXCPT_SUCCESS = 0,					// 0 성공
	AGPMAUCTION_EXCPT_UNKNOWN,						// 1 몰라
	AGPMAUCTION_EXCPT_NOT_AVAILABLE_SERVER,			// 2 옥션서버 사용불가
	AGPMAUCTION_EXCPT_INVALID_CHARACTER,			// 3 캐릭터 정보가 틀림. (버그에 가까움)
	AGPMAUCTION_EXCPT_INVALID_ITEM,					// 4 아이템 정보를 찾을 수 없음. (버그에 가까움)
	AGPMAUCTION_EXCPT_INSUFFICIENT_QTY,				// 5 팔거나 살 아이템 수량 부족
	AGPMAUCTION_EXCPT_EXCEED_REGISTRATION_LIMIT,	// 6 최대 등록 게시물 수 초과
	AGPMAUCTION_EXCPT_FULL_INVENTORY,				// 7 인벤토리가 다참
	AGPMAUCTION_EXCPT_NOT_EXIST_ITEM,				// 8 아이템이 존재하지 않음.
	AGPMAUCTION_EXCPT_INVALID_DOCID,				// 9 게시물 번호가 잘못됨( 중간에 지워지거나 한 경우 )
	AGPMAUCTION_EXCPT_INSUFFICIENT_MONEY,			// 10 돈 부족(나뿐넘)
	AGPMAUCTION_EXCPT_IDENTIC_CHARACTER,			// 11 파는넘 사는넘 동일하다. 우낀넘.
	AGPMAUCTION_EXCPT_NOT_EXIST,					// 12 이미 다른사람에게 팔려서 존재하지 않는 아이템
	AGPMAUCTION_EXCPT_INVALID_STATUS,				// 13
	AGPMAUCTION_EXCPT_NO_TICKET,					// 14 등록에 필요한 ticket이 없다.
	AGPMAUCTION_EXCPT_MAX,
	};


struct stRowset
	{
	public:
		INT32	m_lQueryIndex;						// query index, if ne 0
		CHAR	m_szQuery[MAX_QUERY_LENGTH];		// custom query string

		CHAR	m_szHeaders[MAX_HEADER_STRING_LENGTH];		// column headers
		UINT32	m_ulRows;									// row count
		UINT32	m_ulCols;									// column count

		PVOID	m_pBuffer;							// data buffer ptr.
		UINT32	m_ulRowBufferSize;					// size of 1 row
		UINT32	m_ulTotalBufferSize;				// total buffer size. m_ulRowBufferSize * m_ulCols
		//INT32	m_lOffsets[MAX_COLUMN_SIZE];		// offsets between columns
		ApSafeArray<INT32, MAX_COLUMN_SIZE>		m_lOffsets;
		
	public:
		stRowset()
			{
			m_lQueryIndex = 0;
			ZeroMemory(m_szQuery, sizeof(m_szQuery));

			ZeroMemory(m_szHeaders, sizeof(m_szHeaders));
			m_ulRows = 0;
			m_ulCols = 0;

			m_pBuffer = NULL;
			m_ulRowBufferSize = 0;
			m_ulTotalBufferSize = 0;
			m_lOffsets.MemSetAll();;
			}
			
		CHAR*	Get(UINT32 ulRow, UINT32 ulCol)
			{
			if (ulRow >= m_ulRows || ulCol > m_ulCols)
				return NULL;

			ASSERT(ulCol < MAX_COLUMN_SIZE);

			return (CHAR *)m_pBuffer + m_lOffsets[ulCol] + (ulRow * m_ulRowBufferSize);
			}
	};




/************************************************/
/*		The Definition of AgpmAuction class		*/
/************************************************/
//
class AgpmAuction : public ApModule
	{
	private:
		//	Related modules
		AgpmGrid		*m_pAgpmGrid;
		ApmMap			*m_pApmMap;
		ApmEventManager	*m_pApmEventManager;
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmFactors		*m_pAgpmFactors;
		AgpmItem		*m_pAgpmItem;

	public:
		//	Packet
		AuPacket		m_csPacket;
		AuPacket		m_csPacketSales;
		AuPacket		m_csPacketSelect;
		AuPacket		m_csPacketSelect2;
		AuPacket		m_csPacketSell;
		AuPacket		m_csPacketCancel;
		AuPacket		m_csPacketConfirm;
		AuPacket		m_csPacketBuy;
		AuPacket		m_csPacketNotify;

		// ############
		AuPacket		m_csPacketRowset;

		//	Admin
		AgpaAuctionSales	m_csAdmin;
		
		//	AD
		INT16				m_nIndexCharacterAD;

		ApMutualEx			m_Mutex;

	public:
		AgpmAuction();
		~AgpmAuction();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		BOOL	OnInit();
		BOOL	OnDestroy();
		
		//	Admin
		BOOL				SetMaxSales(INT32 lCount);
		AgpdAuctionSales*	CreateSales();
		void				DestroySales(AgpdAuctionSales* pAgpdAuctionSales);
		AgpdAuctionSales*	GetSales(INT32 lID);
		BOOL				AddSales(AgpdAuctionSales *pAgpdAuctionSales);
		BOOL				RemoveSales(AgpdAuctionSales *pAgpdAuctionSales);

		//	AD
		static BOOL	ConAgpdAuctionCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	DesAgpdAuctionCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdAuctionCAD*	GetCAD(ApBase *pApBase);

		//	Sales
		BOOL	UpdateSalesStatus(INT32 lSalesID, INT16 nStatus);
		BOOL	UpdateSalesStatus(AgpdAuctionSales *pAgpdAuctionSales, INT16 nStatus);
		BOOL	AddSalesToCAD(AgpdCharacter *pAgpdCharacter,  AgpdAuctionSales *pAgpdAuctionSales);
		BOOL	RemoveSalesFromCAD(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales);
		BOOL	RemoveSalesFromCAD(AgpdCharacter *pAgpdCharacter, INT32 lSalesID);
		BOOL	RemoveAllSales(AgpdCharacter *pAgpdCharacter, AuGenerateID *pGenerateID = NULL);
		AgpdAuctionSales*	FindSalesFromCAD(AgpdCharacter *pAgpdCharacter, UINT64 ullDocID);

		//	Requirements
		BOOL	CheckTicket(AgpdCharacter *pAgpdCharacter, BOOL bSub = FALSE);
		BOOL	IsAbleToOpenAnywhere(AgpdCharacter *pAgpdCharacter);

		//	Event
		static BOOL	CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		
		PVOID	MakeEventPacket(ApdEvent *pApdEvent);
		

		//	Callback setting
		BOOL SetCallbackAddSales(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackRemoveSales(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackUpdateSales(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		BOOL SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackConfirm(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass );
		BOOL SetCallbackOpenAuction(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackLogin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackSelect2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackRequestAllSales(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackOpenAnywhere(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// ##########
		BOOL ParseRowsetPacket(PVOID pvPacket, stRowset* pstRowset);
	};


#endif
