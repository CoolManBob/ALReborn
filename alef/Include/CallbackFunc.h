#pragma once

#include "bsipfun.h"
#include "bsipmsg.h"

class CCallBackFunc  
{
public:
	CCallBackFunc();
	virtual ~CCallBackFunc();

public:
	static void PCallbackAuthorRes(GSBsipAuthorResDef* );  
	static void PCallbackAccountRes(GSBsipAccountResDef* );
	static void PCallbackAccountAuthenRes(GSBsipAccountAuthenResDef* );		
	static void PCallbackAccountLockRes(GSBsipAccountLockResDef* );  
	static void PCallbackAccountUnlockRes(GSBsipAccountUnlockResDef* ); 
	static void PCallbackAccountLockExRes(GSBsipAccountLockExResDef* );  
	static void PCallbackAccountUnlockExRes(GSBsipAccountUnlockExResDef* );	
	static void PCallbackAwardAuthenRes(GSBsipAwardAuthenResDef* );
	static void PCallbackAwardAckRes(GSBsipAwardAckResDef* );
	static void PCallbackConsignLockRes(GSBsipConsignLockResDef* );  
	static void PCallbackConsignUnlockRes(GSBsipConsignUnlockResDef* );
	static void PCallbackConsignDepositRes(GSBsipConsignDepositResDef* );
	static void PCallbackGoldDepositReq(GSBsipGoldDepositReqDef* );
	static void PCallbackGoldConsumeLockRes(GSBsipGoldConsumeLockResDef* );
	static void PCallbackGoldConsumeUnlockRes(GSBsipGoldConsumeUnlockResDef* );
	static void PCallbackDepositRes(GSBsipDepositResDef* ); 
	static void PCallbackDepositAckRes(GSBsipDepositAckResDef* );
	static void PCallbackNotifyRes(GSBsipNotifyResDef* ); 
	static void PCallbackREAlert(GSBsipAlertMsg* );
};
