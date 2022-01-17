/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
* 
* 文件名称：bsipmsg.h
* 摘    要：各接口消息体定义（client与GS间信息交互）
* 当前版本：1.4.2
* 作    者：李闻
* 完成日期：2005年9月6日
*
* 修改内容：1.4版本变更该文件
* 修改时间：2005年9月6日
* 
* 修改内容：剥离宏定义
* 修改时间：2005年9月21日
*
* 修改内容：Author接口添加account_id字段
*           Account接口添加account_id、map_id字段
* 修改时间：2005年10月8日
*
* 修改内容：添加Notify消息汇总接口
* 修改时间：2005年10月13日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*				\ConsignDeposit\ConsignUnlock消息中的item_id等字段
* 修改时间：2005年11月25日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*				\ConsignDeposit\ConsignUnlock消息中的item_num、item_price等字段
* 修改时间：2005年11月28日
*/

/*
*bsip msg body ,
*/
#ifndef _BSIP_MSG_H_
#define _BSIP_MSG_H_

#include "bsipmacro.h"

/*------------------------------消息体定义------------------------------*/
/*-------------------------------时长计费-------------------------------*/
/*---------------------------------授权---------------------------------*/
typedef struct GSBsipAuthorReq
{
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                
	char		user_type;						//用户类型   
	char		status;							//用户状态
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）
}GSBsipAuthorReqDef;

typedef struct GSBsipAuthorRes
{
	int			result; 						//返回结果                               
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号                         
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号                       
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                                 
	char  		pay_type;						//付费类型                               
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id                               
	int			balance;						//剩余的点数、秒数或天数（对应pay_type） 
	char  		begin_time[MAX_TIME_LEN+1];		//生效时间（保留字段）                   
	char  		end_time[MAX_TIME_LEN+1];		//失效时间（保留字段）                   
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址 
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）                    
}GSBsipAuthorResDef;

/*---------------------------------扣费---------------------------------*/
typedef struct GSBsipAccountReq
{
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                
	char  		pay_type;						//付费类型              
	char  		app_type;     					//消费类别              
	char 		status;							//用户状态              
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char 		couple_type;					//耦合类型              
	int			amount;							//消费数量              
	int			discount;  						//计费折扣率                                       
	int 		item_id;						//游戏道具编号          
	char  		begin_time[MAX_TIME_LEN+1];		//上线时间              
	char  		end_time[MAX_TIME_LEN+1];		//下线时间     
	char		log_time[MAX_TIME_LEN+1];		//发包时间         
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址    
	char		responsable;					//是否需要响应标志 
	int         map_id;							//地图id            
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）           
}GSBsipAccountReqDef;             
                                                                                
typedef struct GSBsipAccountRes
{
	int			result;							//返回结果                               
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号                         
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号                       
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                                 
	char  		pay_type;						//付费类型                               
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id                               
	int			balance;					    //剩余的点数、秒数或天数（对应pay_type） 
	int			pt_balance;						//剩余的PT币                             
	char  		ip[IP_ADDRESS_LEN+1];		    //游戏客户端的IP地址 
	int         map_id;							//地图id                           
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）                             
}GSBsipAccountResDef;

/*-------------------------------按次计费-------------------------------*/
/*-------------------------------账户认证-------------------------------*/
typedef struct GSBsipAccountAuthenReq
{
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                
	char		user_type;						//用户类型              
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char  		ip[IP_ADDRESS_LEN+1];				//游戏客户端的IP地址
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）
}GSBsipAccountAuthenReqDef;

typedef struct GSBsipAccountAuthenRes
{
	int			result; 						//返回结果                               
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号                         
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号                       
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id                                 
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id                               
	char  		pay_type1;						//付费类型                               	
	int			balance1;						//剩余的点数、秒数或天数（对应pay_type） 	
	char  		pay_type2;						//付费类型                               	
	int			balance2;						//剩余的点数、秒数或天数（对应pay_type） 	
	char  		pay_type3;						//付费类型                               	
	int			balance3;						//剩余的点数、秒数或天数（对应pay_type） 	
	char  		pay_type4;						//付费类型                               	
	int			balance4;						//剩余的点数、秒数或天数（对应pay_type） 	
	char  		pay_type5;						//付费类型                               	
	int			balance5;						//剩余的点数、秒数或天数（对应pay_type） 	
	char  		begin_time[MAX_TIME_LEN+1];		//生效时间（保留字段）                   
	char  		end_time[MAX_TIME_LEN+1];		//失效时间（保留字段）                   
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址 
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）                    
}GSBsipAccountAuthenResDef;



typedef struct GSBsipAccountLockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id                
	char  			pay_type;						//付费类型              
	char  			app_type;     					//消费类别              
	char			status;							//用户状态              
	char 			couple_type;					//耦合类型
	unsigned short	item_num;						//有效道具种类数              
	int				discount;  						//计费折扣率                     
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id              
	char  			ip[IP_ADDRESS_LEN+1];		    //游戏客户端的IP地址    
	int				lock_period;					//lock相对时长 
	char  			log_time[MAX_TIME_LEN+1];		//数据包发送时间
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价
}GSBsipAccountLockReqDef;     


                                                                                                                                 
typedef struct GSBsipAccountLockRes
{
	int				result;							//返回结果          
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号    
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号  
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id            
	char  			pay_type;						//付费类型          
	int				pt_balance;						//剩余的PT币 
	unsigned short	item_num;						//有效道具种类数
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id          
	char  			ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
	char  		log_time[MAX_TIME_LEN+1];			//数据包发送时间
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价
}GSBsipAccountLockResDef;

/*---------------------------------解冻---------------------------------*/
typedef struct GSBsipAccountUnlockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			confirm;						//确认扣除              
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id                
	char  			pay_type;						//付费类型              
	char  			app_type;     					//消费类别              
	char 			status;							//用户状态              
	char 			couple_type;					//耦合类型              
	unsigned short	item_num;						//有效道具种类数           
	int				discount;  						//计费折扣率            
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id                     
	char  			ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址    
	char			responsable;					//是否需要响应标志 
	char  			log_time[MAX_TIME_LEN+1];		//数据包发送时间
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价     
}GSBsipAccountUnlockReqDef;      
                             
                                                                                                                               
typedef struct GSBsipAccountUnlockRes                             
{                                                                 
	int			result;							//返回结果          
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号    
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号  
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id            
	char  		pay_type;						//付费类型          
	int			pt_balance;						//剩余的PT币        
	char  		order_id[MAX_ORDERID_LEN+1];	//数据包id          
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
}GSBsipAccountUnlockResDef;


/*---------------------------------预冻扩展---------------------------------*/
typedef struct GSBsipAccountLockExReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char pay_type1;                      //付费类型
    char pay_type2;                      //付费类型
    char pay_type3;                      //付费类型
    char pay_type4;                      //付费类型
    char pay_type5;                      //付费类型
    int	 balance1;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance2;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance3;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance4;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance5;						 //剩余的点数、秒数或天数（对应pay_type） 	
    char app_type;                       //消费类别
    char status;                         //用户状态
    char couple_type;                    //耦合类型
    unsigned short item_num;             //有效道具种类数
    int  discount;                       //计费折扣率
    char order_id[MAX_ORDERID_LEN+1];    //数据包id 
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    int  lock_period;                    //lock相对时长
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
    int  item0_id;                       //游戏道具编号
    int  item1_id;                       //游戏道具编号
    int  item2_id;                       //游戏道具编号
    int  item3_id;                       //游戏道具编号
    int  item4_id;                       //游戏道具编号
    unsigned short item0_num;            //游戏道具数量
    unsigned short item1_num;            //游戏道具数量
    unsigned short item2_num;            //游戏道具数量
    unsigned short item3_num;            //游戏道具数量
    unsigned short item4_num;            //游戏道具数量
    int  item0_price;                    //游戏道具单价
    int  item1_price;                    //游戏道具单价
    int  item2_price;                    //游戏道具单价
    int  item3_price;                    //游戏道具单价
    int  item4_price;                    //游戏道具单价
}GSBsipAccountLockExReqDef;
          
typedef struct GSBsipAccountLockExRes
{        
    int  result;                         //返回结果
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id    
    char pay_type1;                      //付费类型
    char pay_type2;                      //付费类型
    char pay_type3;                      //付费类型
    char pay_type4;                      //付费类型
    char pay_type5;                      //付费类型
    int	 balance1;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance2;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance3;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance4;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance5;						 //剩余的点数、秒数或天数（对应pay_type） 	
    unsigned short item_num;             //有效道具种类数
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    int  item0_id;                       //游戏道具编号
    int  item1_id;                       //游戏道具编号
    int  item2_id;                       //游戏道具编号
    int  item3_id;                       //游戏道具编号
    int  item4_id;                       //游戏道具编号
    unsigned short item0_num;            //游戏道具数量
    unsigned short item1_num;            //游戏道具数量
    unsigned short item2_num;            //游戏道具数量
    unsigned short item3_num;            //游戏道具数量
    unsigned short item4_num;            //游戏道具数量
    int  item0_price;                    //游戏道具单价
    int  item1_price;                    //游戏道具单价
    int  item2_price;                    //游戏道具单价
    int  item3_price;                    //游戏道具单价
    int  item4_price;                    //游戏道具单价
}GSBsipAccountLockExResDef;

/*---------------------------------解冻扩展---------------------------------*/
typedef struct GSBsipAccountUnlockExReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char confirm;                        //确认扣除
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char pay_type1;                      //付费类型
    char pay_type2;                      //付费类型
    char pay_type3;                      //付费类型
    char pay_type4;                      //付费类型
    char pay_type5;                      //付费类型
    int	 balance1;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance2;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance3;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance4;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance5;						 //剩余的点数、秒数或天数（对应pay_type） 		
    char app_type;                       //消费类别
    char status;                         //用户状态
    char couple_type;                    //耦合类型
    unsigned short item_num;             //有效道具种类数
    int  discount;                       //计费折扣率
    char order_id[MAX_ORDERID_LEN+1];    //数据包id   
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char responsable;                    //是否需要响应标志
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
    int  item0_id;                       //游戏道具编号
    int  item1_id;                       //游戏道具编号
    int  item2_id;                       //游戏道具编号
    int  item3_id;                       //游戏道具编号
    int  item4_id;                       //游戏道具编号
    unsigned short item0_num;            //游戏道具数量
    unsigned short item1_num;            //游戏道具数量
    unsigned short item2_num;            //游戏道具数量
    unsigned short item3_num;            //游戏道具数量
    unsigned short item4_num;            //游戏道具数量
    int  item0_price;                    //游戏道具单价
    int  item1_price;                    //游戏道具单价
    int  item2_price;                    //游戏道具单价
    int  item3_price;                    //游戏道具单价
    int  item4_price;                    //游戏道具单价
}GSBsipAccountUnlockExReqDef;

typedef struct GSBsipAccountUnlockExRes
{
    int  result;                         //返回结果
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id    
    char pay_type1;                      //付费类型
    char pay_type2;                      //付费类型
    char pay_type3;                      //付费类型
    char pay_type4;                      //付费类型
    char pay_type5;                      //付费类型
    int	 balance1;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance2;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance3;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance4;						 //剩余的点数、秒数或天数（对应pay_type） 	
	int	 balance5;						 //剩余的点数、秒数或天数（对应pay_type） 	
    unsigned short item_num;             //有效道具种类数
    char order_id[MAX_ORDERID_LEN+1];    //数据包id 
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}GSBsipAccountUnlockExResDef;


/*---------------------------------领奖---------------------------------*/
/*-------------------------------领奖认证-------------------------------*/
typedef struct GSBsipAwardAuthenReq
{
	char		sd_id[MAX_SDID_LEN+1];					//用户的数字帐号        
	char		pt_id[MAX_PTID_LEN+1];					//用户的PT字符帐号      
	char 		sess_id[MAX_SESSID_LEN+1];				//会话id                
	char		user_type;								//用户类型              
	char 		couple_type;							//耦合类型              
	char		unique_id[MAX_ORDERID_LEN+1];			//数据包id              
	char  		ip[IP_ADDRESS_LEN+1];					//游戏客户端的IP地址    
	int			award_type;								//领奖类型              
	char		award_num[MAX_AWARD_LEN+1];				//领奖号码
	char		award_patchid[MAX_AWARDPATCH_LEN+1]; 	//领奖批次号 
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）            
}GSBsipAwardAuthenReqDef;

typedef struct GSBsipAwardAuthenRes
{
	int  			result;							//返回结果          
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号    
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号  
	char 			sess_id[MAX_SESSID_LEN+1];  	//会话id            
	char 			couple_type;					//耦合类型          
	char			unique_id[MAX_ORDERID_LEN+1];	//数据包id          
	char  			ip[IP_ADDRESS_LEN+1];       	//游戏客户端的IP地址
	char			award_num[MAX_AWARD_LEN+1];		//领奖号码          
	char			award_sn[MAX_AWARDSN_LEN+1];	//领奖序列号        
	char			bind_user;						//邦定用户标记      
	int				award_type1;					//奖品类型          
	unsigned int	award_item_id1;					//奖品编号          
	unsigned int	award_item_count1;				//奖品数量          
	int				award_type2;					//奖品类型          
	unsigned int 	award_item_id2;					//奖品编号          
	unsigned int	award_item_count2;				//奖品数量          
	int				award_type3;					//奖品类型          
	unsigned int 	award_item_id3;					//奖品编号          
	unsigned int	award_item_count3;				//奖品数量          
	int				award_type4;					//奖品类型          
	unsigned int 	award_item_id4;					//奖品编号          
	unsigned int	award_item_count4;				//奖品数量          
	int				award_type5;					//奖品类型          
	unsigned int 	award_item_id5;					//奖品编号          
	unsigned int	award_item_count5;				//奖品数量   
	int				reserve_int;					//保留字段（备用）
	char			reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）       
}GSBsipAwardAuthenResDef;

/*-------------------------------领奖确认-------------------------------*/
typedef struct GSBsipAwardAck
{
	int  			result;							//返回结果              
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			sess_id[MAX_SESSID_LEN+1];  	//会话id                
	char			unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char  			ip[IP_ADDRESS_LEN+1];         	//游戏客户端的IP地址    
	int				award_type;						//领奖类型
	char			award_num[MAX_AWARD_LEN+1];		//领奖号码              
	char			award_sn[MAX_AWARDSN_LEN+1];	//领奖序列号            
	char			award_time[MAX_TIME_LEN+1];		//发奖成功、失败时间 
	int				reserve_int;					//保留字段（备用）
	char			reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）   
}GSBsipAwardAckDef;

typedef struct GSBsipAwardAckRes
{
	int  			result;							//返回结果              
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			sess_id[MAX_SESSID_LEN+1];  	//会话id     
	char			couple_type;					//耦合类型
	char			unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char  			ip[IP_ADDRESS_LEN+1];         	//游戏客户端的IP地址    
	int				award_type;						//奖品类型
	char			pay_type;						//领奖的充值类型
	int				balance;						//充值后用户余额	
}GSBsipAwardAckResDef;
/*-------------------------------寄售交易-------------------------------*/
/*-------------------------------寄售预冻-------------------------------*/


typedef struct GSBsipConsignLockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id                
	char  			pay_type;						//付费类型
	int				group_id;						//组编号              
	char  			app_type;     					//消费类别              
	char			status;							//用户状态              
	char 			couple_type;					//耦合类型              
	unsigned short	item_num;						//有效道具种类数
	int				discount;  						//计费折扣率                 
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id  
	int				area_id;						//区编号            
	char  			ip[IP_ADDRESS_LEN+1];		    //游戏客户端的IP地址    
	int				lock_period;					//lock相对时长 
	char  			log_time[MAX_TIME_LEN+1];		//数据包发送时间
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价    
}GSBsipConsignLockReqDef;     
                                                                                                                                 
typedef struct GSBsipConsignLockRes
{
	int				result;							//返回结果          
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号    
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号  
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id            
	char  			pay_type;						//付费类型  
	int				group_id;						//组编号        
	int				pt_balance;						//剩余的PT币        
	unsigned short	item_num;						//有效道具种类数
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id   
	int				area_id;						//区编号       
	char  			ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价    
}GSBsipConsignLockResDef;

/*-------------------------------寄售解冻-------------------------------*/
typedef struct GSBsipConsignUnlockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char			pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 			confirm;						//确认扣除              
	char 			sess_id[MAX_SESSID_LEN+1];		//会话id                
	char  			pay_type;						//付费类型    
	int				group_id;						//组编号          
	char  			app_type;     					//消费类别              
	char 			status;							//用户状态              
	char 			couple_type;					//耦合类型              
	unsigned short	item_num;						//有效道具种类数
	int				discount;  						//计费折扣率            
	char  			order_id[MAX_ORDERID_LEN+1];	//数据包id               
	int				area_id;						//区编号        
	char  			ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址    
	char			responsable;					//是否需要响应标志
	char  			log_time[MAX_TIME_LEN+1];		//数据包发送时间 
	int 			item0_id;						//游戏道具编号 
	unsigned short	item0_num;						//游戏道具数量    
	int				item0_price;					//游戏道具单价
	int 			item1_id;						//游戏道具编号 
	unsigned short	item1_num;						//游戏道具数量  
	int				item1_price;					//游戏道具单价
	int 			item2_id;						//游戏道具编号 
	unsigned short	item2_num;						//游戏道具数量 
	int				item2_price;					//游戏道具单价 
	int 			item3_id;						//游戏道具编号 
	unsigned short	item3_num;						//游戏道具数量
	int				item3_price;					//游戏道具单价  
	int 			item4_id;						//游戏道具编号 
	unsigned short	item4_num;						//游戏道具数量  
	int				item4_price;					//游戏道具单价   
}GSBsipConsignUnlockReqDef;  

                                                                                                                               
typedef struct GSBsipConsignUnlockRes
{
	int			result;							//返回结果          
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号    
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号  
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id            
	char  		pay_type;						//付费类型  
	int			group_id;						//组编号        
	int			pt_balance;						//剩余的PT币        
	char  		order_id[MAX_ORDERID_LEN+1];	//数据包id    
	int			area_id;						//区编号      
	char  		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
}GSBsipConsignUnlockResDef;



/*-------------------------------寄售充值-------------------------------*/
typedef struct GSBsipConsignDepositReq
{
	char    	sd_id[MAX_SDID_LEN+1];          //用户的数字帐号，全为数字。                                                                              
	char    	pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号，不能全为数字                                                                          
	char    	sess_id[MAX_SESSID_LEN+1];      //会话id
	int			amount;							//元宝金额                                                                                                
	char    	pay_type;                       //付费的方式
	char		patch_id[MAX_PATCHID_LEN+1];	//批次号
	char    	order_id[MAX_ORDERID_LEN+1];    //唯一标识
	char    	user_type;                      //用户类型
	int			group_id;						//组编号
	int			area_id;						//区编号
	char    	ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
	char    	buyer_sd_id[MAX_SDID_LEN+1];    //买家的数字帐号，全为数字。                       
	char    	buyer_pt_id[MAX_PTID_LEN+1];    //买家的PT字符帐号，不能全为数字                      
	int			buyer_group_id;					//表示组id 
	int			buyer_area_id;					//游戏区编号
	char  		log_time[MAX_TIME_LEN+1];		//数据包发送时间      
}GSBsipConsignDepositReqDef;

typedef struct GSBsipConsignDepositRes
{
	int			result;							//返回结果
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号，全为数字。
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号，不能全为数字
	char    	sess_id[MAX_SESSID_LEN+1];      //会话id
	char		pay_type;						//付费的方式
	char		patch_id[MAX_PATCHID_LEN+1];	//批次号
	int			group_id;						//组编号
	char		order_id[MAX_ORDERID_LEN+1];	//唯一标识
	int			area_id;						//区编号
	char		ip[IP_ADDRESS_LEN+1];			//ip游戏客户端地址
	char    	buyer_sd_id[MAX_SDID_LEN+1];    //买家的数字帐号，全为数字。                       
	char    	buyer_pt_id[MAX_PTID_LEN+1];    //买家的PT字符帐号，不能全为数字 
	int			buyer_group_id;					//表示组id 
	int			buyer_area_id;					//游戏区编号          
	int			pt_balance;						//剩余PT币   

}GSBsipConsignDepositResDef;

/*-------------------------------元宝交易-------------------------------*/
/*-------------------------------元宝充值-------------------------------*/
typedef struct GSBsipGoldDepositReq
{
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	int			amount;							//充值金额              
	char		pay_type;						//付费类型              
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char		user_type;						//用户类型              
	char		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址
	char		sync_flag;						//充值消息标志位   
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用） 
}GSBsipGoldDepositReqDef;

typedef struct GSBsipGoldDepositRes
{
	int			result;							//返回结果              
	char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号        
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char		pay_type;						//付费类型              
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id              
	char		ip[IP_ADDRESS_LEN+1];			//游戏客户端的IP地址  
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）  
}GSBsipGoldDepositResDef;

/*-------------------------------元宝换时间-------------------------------*/
typedef struct GSBsipGoldConsumeLockReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char pay_type;                       //付费类型
    char couple_type;                    //耦合类型
    int  amount;                         //消费数量
    int  discount;                       //计费折扣率
    char time_type;                      //充入的时长类型
    int  time_amount;                    //兑换的时长数量
    char unique_id[MAX_ORDERID_LEN+1];   //数据包id 
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipGoldConsumeLockReqDef;

typedef struct GSBsipGoldConsumeLockRes
{
    int  result;                         //返回结果
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char pay_type;                       //付费类型
    char time_type;                      //充入的时长类型
    int  time_amount;                    //兑换的时长数量
    int  balance;                        //兑换后总的时长数量
    int  pt_balance;                     //兑换后总的点券数量
    char unique_id[MAX_ORDERID_LEN+1];   //数据包id    
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
}GSBsipGoldConsumeLockResDef;

typedef struct GSBsipGoldConsumeUnlockReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char confirm;                        //确认扣除
    char pay_type;                       //付费类型
    char couple_type;                    //耦合类型
    int  amount;                         //消费数量
    int  discount;                       //计费折扣率
    char time_type;                      //充入的时长类型
    int  time_amount;                    //兑换的时长数量
    char unique_id[MAX_ORDERID_LEN+1];   //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipGoldConsumeUnlockReqDef;

typedef struct GSBsipGoldConsumeUnlockRes
{
    int  result;                         //返回结果
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char pay_type;                       //付费类型
    char time_type;                      //充入的时长类型
    int  time_amount;                    //兑换的时长数量
    int  balance;                        //兑换后总的时长数量
    int  pt_balance;                     //兑换后总的点券数量
    char unique_id[MAX_ORDERID_LEN+1];   //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
}GSBsipGoldConsumeUnlockResDef;

/*-------------------------------活动充值-------------------------------*/
/*-------------------------------PT币充值-------------------------------*/
typedef struct GSBsipDepositReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号，全为数字。
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号，不能全为数字
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    int  item_id;                        //游戏道具编号
    unsigned short item_num;             //游戏道具数量
    char pay_type;                       //付费的方式
    int  amount;                         //元宝金额
    char unique_id[MAX_ORDERID_LEN+1];    //唯一标识
    char user_type;                      //用户类型
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipDepositReqDef;

typedef struct GSBsipDepositRes
{
    int  result;                         //返回结果 
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号，全为数字。
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号，不能全为数字
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    int  item_id;                        //游戏道具编号
    unsigned short item_num;             //游戏道具数量
    char pay_type;                       //付费的方式
    int  balance;                        //元宝金额
    char unique_id[MAX_ORDERID_LEN+1];   //唯一标识    
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
}GSBsipDepositResDef;

typedef struct GSBsipDepositAckReq
{    
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号，全为数字。
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号，不能全为数字
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char confirm;                        //确认充值
    int  item_id;                        //游戏道具编号
    unsigned short item_num;             //游戏道具数量
    char pay_type;                       //付费的方式
    int  amount;                         //元宝金额
    char unique_id[MAX_ORDERID_LEN+1];    //唯一标识
    char user_type;                      //用户类型    
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipDepositAckReqDef;

typedef struct GSBsipDepositAckRes
{
    int  result;                         //返回结果
    char sd_id[MAX_SDID_LEN+1];          //用户的数字帐号，全为数字。
    char pt_id[MAX_PTID_LEN+1];          //用户的PT字符帐号，不能全为数字
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    int  item_id;                        //游戏道具编号
    unsigned short item_num;             //游戏道具数量
    char pay_type;                       //付费的方式
    int  balance;                        //元宝金额
    char unique_id[MAX_ORDERID_LEN+1];   //唯一标识    
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
}GSBsipDepositAckResDef;
/*-------------------------------消息汇总-------------------------------*/
typedef struct GSBsipNotifyReq
{
	int			area_id;						//游戏区编号
	int			group_id;						//表示组id
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id
	int			notify_type;					//消息类型
	int			info_data;						//消息数据--整型
	char		info_content[MAX_CONTENT_LEN+1];//消息数据--字符串
	char		ip[IP_ADDRESS_LEN+1];			//Login Server的IP地址
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）             
}GSBsipNotifyReqDef;

typedef struct GSBsipNotifyRes
{
	int			result;							//返回结果
	int			area_id;						//游戏区编号
	int			group_id;						//表示组id
	char		unique_id[MAX_ORDERID_LEN+1];	//数据包id
	int			notify_type;					//消息类型
	int			info_data;						//消息数据--整型
	char		info_content[MAX_CONTENT_LEN+1];//消息数据--字符串
	char		ip[IP_ADDRESS_LEN+1];			//Login Server的IP地址
	int			reserve_int;					//保留字段（备用）
	char		reserve_char[MAX_RESERVE_LEN+1];//保留字段（备用）             
}GSBsipNotifyResDef;


/*计费引擎所需结构*/

typedef struct{
	char		sd_id[MAX_SDID_LEN+1];	//用户的数字帐号
	char		pt_id[MAX_PTID_LEN+1];	//用户的PT字符帐号
}GSBsipUser;

typedef struct{
    char		sd_id[MAX_SDID_LEN+1];			//用户的数字帐号
	char		pt_id[MAX_PTID_LEN+1];			//用户的PT字符帐号      
	char 		sess_id[MAX_SESSID_LEN+1];		//会话id 
	int 		map_id;							//游戏的map_id
} GSBsipLoginMsg;

typedef struct{
		char	sess_id[MAX_SESSID_LEN+1];		//返回会话id 
		int		balance;      					//返回用户当前时常余额
		char	pay_type;						//返回用户当前付费类型
}GSBsipSessionMsg;

/*计费引擎通知消息类型*/
#define ALERT_BALANCE_NOTIFY	0x00400000 //BE返回,正常余额通知
#define ALERT_NOT_ENOUGH_BALANCE	0x00400001 //BE返回,余额不足,通知下线
#define ALERT_AUTHOR_FAILURE	0x00400002 //BE返回,AUTHOR失败,错误提示在msg中
#define ALERT_ACCOUNT_FAILURE	0x00400003 //BE返回,ACCOUNT失败,错误提示在msg中
#define ALERT_LOGIN_TIMEOUT	0x00400004 //BE返回,用户登入超时

typedef struct{
	char		sd_id[MAX_SDID_LEN+1];	//用户的数字帐号
	char		pt_id[MAX_PTID_LEN+1];	//用户的PT字符帐号
	unsigned int	msg_type;	/*通知消息类型*/
	char	msg[256];	/*消息内容*/
}GSBsipAlertMsg;



#endif
