#pragma once

#include "bsipmsg.h"
#include "bsipfun.h"

typedef int (*FunGSInitializeEx)(GSCallbackFuncDef *gscallbackFun,GSBECallbackFuncDef* becallbackFun, const char *configfilename);
typedef int (*FunGSInitialize)(GSCallbackFuncDef *callbackFun, const char *configfilename,int type);
typedef int (*FunUninitialize)( int type );	
typedef int (*FunSendAuthorRequest)(				const GSBsipAuthorReqDef *req			);
typedef int (*FunSendAccountRequest)(				const GSBsipAccountReqDef *req			);
typedef int (*FunSendAccountAuthenRequest)(			const GSBsipAccountAuthenReqDef *req	);
typedef int (*FunSendAccountLockRequest)(			const GSBsipAccountLockReqDef *req		);
typedef int (*FunSendAccountUnlockRequest)(			const GSBsipAccountUnlockReqDef *req	);	
typedef int (*FunSendAccountLockExRequest)(			const GSBsipAccountLockExReqDef *req	);
typedef int (*FunSendAccountUnlockExRequest)(		const GSBsipAccountUnlockExReqDef *req	);
typedef int (*FunSendAwardAuthenRequest)(			const GSBsipAwardAuthenReqDef *req		);
typedef int (*FunSendAwardAck)(						const GSBsipAwardAckDef *req			);
typedef int (*FunSendConsignLockRequest)(			const GSBsipConsignLockReqDef *req		);
typedef int (*FunSendConsignUnlockRequest)(			const GSBsipConsignUnlockReqDef *req	);
typedef int (*FunSendConsignDepositRequest)(		const GSBsipConsignDepositReqDef *req	);
typedef int (*FunSendGoldDepositResponse)(			const GSBsipGoldDepositResDef *res		);
typedef int (*FunSendGoldConsumeLockRequest)(		const GSBsipGoldConsumeLockReqDef *req	);	
typedef int (*FunSendGoldConsumeUnlockRequest)(		const GSBsipGoldConsumeUnlockReqDef *req);	
typedef int (*FunSendDepositRequest)(				const GSBsipDepositReqDef *req			);
typedef int (*FunSendDepositAck)(					const GSBsipDepositAckReqDef *req		);
typedef int (*FunSendNotifyRequest)(				const GSBsipNotifyReqDef *req		);

typedef int (*FunPlayerLogin)(						const GSBsipLoginMsg*					);
typedef int (*FunPlayerLogout)(  const GSBsipUser *pUSER,GSBsipSessionMsg *pSession,const int nStatus );
typedef int (*FunPlayerReLogin)( const GSBsipUser *pUser,const GSBsipSessionMsg *pSession, const int nMapId );	

typedef int (*FunGetNewId)(char *id);
typedef int (*FunGetNewIdByParam)(int intareaId,int intGroupId, char *id);
