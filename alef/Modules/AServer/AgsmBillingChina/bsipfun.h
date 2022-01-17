/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
* 
* 文件名称：bsipfun.h
* 摘    要：各接口函数定义
* 当前版本：1.4
* 作    者：李闻
* 完成日期：2005年9月6日
* 修改内容：在1.3的版本上进行修改，删除弃用的函数
*           添加寄售交易使用的函数
* 修改时间：2005年9月6日
*
* 修改内容：添加Notify消息汇总接口
* 修改时间：2005年10月13日
*/

#ifndef _BSIP_FUN_H_
#define _BSIP_FUN_H_ 

#if defined(WIN32) || defined(WIN64)
	#define BSIP_API_TYPE _declspec(dllexport)
#elif LINUX
	#define BSIP_API_TYPE
#endif

#include "bsipmsg.h"


typedef struct
{
	void (*CallbackAuthorRes)(GSBsipAuthorResDef* );  
	void (*CallbackAccountRes)(GSBsipAccountResDef* );
	void (*CallbackAccountAuthenRes)(GSBsipAccountAuthenResDef* );		
	void (*CallbackAccountLockRes)(GSBsipAccountLockResDef* );  
	void (*CallbackAccountUnlockRes)(GSBsipAccountUnlockResDef* ); 	
	void (*CallbackAccountLockExRes)(GSBsipAccountLockExResDef* );  
	void (*CallbackAccountUnlockExRes)(GSBsipAccountUnlockExResDef* ); 	
	void (*CallbackAwardAuthenRes)(GSBsipAwardAuthenResDef* );
	void (*CallbackAwardAckRes)(GSBsipAwardAckResDef* );
	void (*CallbackConsignLockRes)(GSBsipConsignLockResDef* );  
	void (*CallbackConsignUnlockRes)(GSBsipConsignUnlockResDef* );
	void (*CallbackConsignDepositRes)(GSBsipConsignDepositResDef* );
	void (*CallbackGoldDepositReq)(GSBsipGoldDepositReqDef* );
	void (*CallbackGoldConsumeLockRes)(GSBsipGoldConsumeLockResDef* );
	void (*CallbackGoldConsumeUnlockRes)(GSBsipGoldConsumeUnlockResDef* );
	void (*CallbackDepositRes)(GSBsipDepositResDef* ); 
	void (*CallbackDepositAckRes)(GSBsipDepositAckResDef* ); 
	void (*CallbackNotifyRes)(GSBsipNotifyResDef* ); 
	
}GSCallbackFuncDef;


typedef struct 
{
	void (*CallbackAlertRes)(GSBsipAlertMsg* );
}GSBECallbackFuncDef;



#ifdef __cplusplus
extern "C" 
{
#endif
	BSIP_API_TYPE int GSInitializeEx(GSCallbackFuncDef *gscallbackFun,GSBECallbackFuncDef* recallbackFun, const char *configfilename);
	BSIP_API_TYPE int GSInitialize(GSCallbackFuncDef *callbackFun, const char *configfilename,int client_type);
	BSIP_API_TYPE int Uninitialize(int client_type);	
	BSIP_API_TYPE int SendAuthorRequest(				const GSBsipAuthorReqDef *req			);
	BSIP_API_TYPE int SendAccountRequest(				const GSBsipAccountReqDef *req			);
	BSIP_API_TYPE int SendAccountAuthenRequest(			const GSBsipAccountAuthenReqDef *req	);
	BSIP_API_TYPE int SendAccountLockRequest(			const GSBsipAccountLockReqDef *req		);
	BSIP_API_TYPE int SendAccountUnlockRequest(			const GSBsipAccountUnlockReqDef *req	);	
	BSIP_API_TYPE int SendAccountLockExRequest(			const GSBsipAccountLockExReqDef *req	);
	BSIP_API_TYPE int SendAccountUnlockExRequest(		const GSBsipAccountUnlockExReqDef *req	);
	BSIP_API_TYPE int SendAwardAuthenRequest(			const GSBsipAwardAuthenReqDef *req		);
	BSIP_API_TYPE int SendAwardAck(						const GSBsipAwardAckDef *req			);
	BSIP_API_TYPE int SendConsignLockRequest(			const GSBsipConsignLockReqDef *req		);
	BSIP_API_TYPE int SendConsignUnlockRequest(			const GSBsipConsignUnlockReqDef *req	);
	BSIP_API_TYPE int SendConsignDepositRequest(		const GSBsipConsignDepositReqDef *req	);
	BSIP_API_TYPE int SendGoldDepositResponse(			const GSBsipGoldDepositResDef *res		);
	BSIP_API_TYPE int SendGoldConsumeLockRequest(		const GSBsipGoldConsumeLockReqDef *req	);	
	BSIP_API_TYPE int SendGoldConsumeUnlockRequest(		const GSBsipGoldConsumeUnlockReqDef *req);	
	BSIP_API_TYPE int SendDepositRequest(				const GSBsipDepositReqDef *req			);
	BSIP_API_TYPE int SendDepositAck(					const GSBsipDepositAckReqDef *req		);
	BSIP_API_TYPE int SendNotifyRequest(				const GSBsipNotifyReqDef *req			);
	BSIP_API_TYPE int Login(							const GSBsipLoginMsg*					);
	BSIP_API_TYPE int Logout(const GSBsipUser *pUser,GSBsipSessionMsg *pSession,const int nStatus);
	BSIP_API_TYPE int ReLogin(const GSBsipUser *pUser,const GSBsipSessionMsg *pSession, const int nMapId );	
	
	BSIP_API_TYPE int GetNewId(char *id);
	BSIP_API_TYPE int GetNewIdByParam(int intareaId,int intGroupId, char *id);	

#ifdef __cplusplus
}
#endif

#endif
