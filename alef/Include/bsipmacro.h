/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
*
* 文件名称：bsipmacro.h
* 摘    要：宏定义（各接口消息体定义中使用）
* 当前版本：1.4
* 作    者：李闻
* 完成日期：2005年9月21日
*
* 修改内容：1.4版本新增该文件
* 修改时间：2005年9月21日
*
* 修改内容：添加MAX_NOTIFY_LEN宏
* 修改时间：2005年10月13日
*/

/*
*bsip msg body ,
*/
#ifndef _BSIP_MACRO_H_
#define _BSIP_MACRO_H_

/*-------------------------消息体字段长度宏定义-------------------------*/
#define MAX_TIME_LEN                19  //时间长， 格式为：YYYY-MM-DD HH24:MI:SS
#define MAX_USERID_LEN              32  //用户姓名长，如：张山，非unicode
#define MAX_SDID_LEN                16  //用户数字帐号长度
#define MAX_PTID_LEN                32  //poptang id长度
#define MAX_USERPWD_LEN             16  //用户密码长
#define MAX_MD5PWD_LEN              16  //用户密码长
#define IP_ADDRESS_LEN              15  //ip地址长， 如：61.172.252.37
#define MAX_DIGEST_LEN              16  //加密结果字段长
#define MAX_HOSTNAME_LEN            64  //机器名长
#define MAX_ORDERID_LEN             32  //订单号长 目前用于unique_id
#define MAX_AWARD_LEN               30  //领奖号长
#define MAX_ENC_PWD_LEN             16  //游戏服务器间加密密码长
#define MAX_ENC_BUFFER_LEN          1024//加密后数据包长度
#define MAX_TELE_LEN                15  //主叫号码
#define MAX_USERNAME_LEN            32  //用户名称长度,只支持16位长汉字
#define MAX_CARDNUM_LEN             20  //充值卡号长度
#define MAX_SESSID_LEN              32  //会话ID长度
#define MAX_AWARDSN_LEN             20  //发送道具序列号
#define MAX_AWARDPATCH_LEN          10  //领奖批次号
#define MAX_RESERVE_LEN             16  //保留备用字符串长度
#define MAX_PATCHID_LEN             10  //充值对应活动批次号
#define MAX_CONTENT_LEN             30  //消息汇总的字符串长度

/*-------------------------消息体字段取值宏定义-------------------------*/
#define SD_ID_INVALIDATION          '0' //数字帐号无效
#define PT_ID_INVALIDATION          '0' //数字帐号无效

#define USER_TYPE_REG               '1' // '1'-游戏注册用户
#define USER_TYPE_OTHER_REG         '2' // '2'-其它游戏平台用户,其它类型待定

#define PAY_TYPE_DAY                '1' // 按天
#define PAY_TYPE_SEC                '2' // 按秒
#define PAY_TYPE_POINT              '3' // 按点（PT币）
#define PAY_TYPE_PROP               '4' // 流通道具（元宝）
#define PAY_TYPE_MONEY              '5' // 按钱
#define PAY_TYPE_SCORE              '6' // 按积分

#define APP_TYPE_GAME               '1' //游戏计费
#define APP_TYPE_PROP               '2' //买道具
#define APP_TYPE_VOD                '3' //VOD点播
#define APP_TYPE_VOIP               '4' //VOIP时长计费
#define APP_TYPE_IVR                '5' //IVR时长计费
#define APP_TYPE_SONG               '6' //歌曲点播
#define APP_TYPE_TIME               '7' //买时间
#define APP_TYPE_CONSIGN            '8' //寄售交易

#define USER_STATUS_NONE            '0' //无状态
#define USER_STATUS_LOGON           '1' //初次上线
#define USER_STATUS_ONLINE          '2' //在线
#define USER_STATUS_LOGOFF          '3' //下线
#define USER_STATUS_JUMP            '4' //跳地图（服务器）
#define USER_STATUS_ENDPOINT        '5' //余额临界点

#define UNLOCK_CONFIRM_NO           '0' //取消预冻
#define UNLOCK_CONFIRM_YES          '1' //确认预冻

#define COUPLE_TYPE_STRICT          '1' //紧耦合
#define COUPLE_TYPE_FREE            '2' //松耦合

#define TIME_TYPE_DAY               '1' //元宝购买时长单位为天
#define TIME_TYPE_SEC               '2' //元宝购买时长单位为秒

#define PATCH_ID_ZERO               '0' //活动批次号：玩家交易充值

#define SYNC_FLAG_FIRST             '0' //首次发送充值消息
#define SYNC_FLAG_MORE              '1' //多次发送充值消息

#define AWARD_TYPE_BOUNDUSER        1   //与帐号邦定的领奖
#define AWARD_TYPE_BOUNDNUM         2   //与领奖号绑定的领奖
#define AWARD_TYPE_NOBOUND          3   //不与帐号、领奖号绑定的领奖
#define AWARD_TYPE_DEPOSIT          4   //道具换元宝

#define RESPONSE_YES                '1' //需要
#define RESPONSE_NO                 '2' //不需要

/*-------------------------接口函数参数宏定义-------------------------*/
#define UNION_CLIENT        0x0         //处理计费系统包含的所有接口的客户端
#define NORMAL_CLIENT       0x1         //处理时长授权、扣费消息的客户端
#define GOLD_CONSUME_CLIENT 0x2         //处理除时长授权、扣费以外消息的客户端

#endif
