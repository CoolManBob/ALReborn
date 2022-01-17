/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
*
* 文件名称：bsiperrcode.h
* 摘    要：返回错误码定义
* 当前版本：1.4
* 作    者：李闻
* 完成日期：2005年9月12日
*
* 修改内容：1.4版本新增该文件
* 修改时间：2005年9月12日
*
* 修改内容：1.5版本新增几个错误码：
* 修改时间：2005-12-29 16:01 zlc
*/


/*
 * cbs数据库存储过程错误码范围：100001－109999；
 * lbs数据库存储过程错误码范围：120001－129999；
 * 数据格式非法错误码范围：200001－299999；
 * cbs业务处理错误码范围：300001－399999；
 * lbs业务处理错误码范围：400001－499999；
 * 其他未定义:-1
 */

#ifndef _BSIP_ERRCODE_H_
#define _BSIP_ERRCODE_H_

enum    //数据格式非法错误码
{
    //用户数字帐号
    SDIDLONGER             = -200101,   //输入过长
    SDIDNULL               = -200102,   //为空
    SDIDILLEGAL            = -200103,   //含非法字符
    SDIDOTHER              = -200199,   //其他错误

    //用户PT字符帐号
    PTIDLONGER             = -200201,   //输入过长
    PTIDNULL               = -200202,   //为空
    PTIDILLEGAL            = -200203,   //含非法字符
    PTIDALLNUM             = -200204,   //全为数字或以数字开头
    PTIDOTHER              = -200299,   //其他错误

    //业务类型编号
    SERVIDLONGER           = -200301,   //输入过长
    SERVIDNULL             = -200302,   //为空
    SERVIDILLEGAL          = -200303,   //含非法字符
    SERVIDOTHER            = -200399,   //其他错误

    //游戏区编号
    AREAIDLONGER           = -200401,   //输入过长
    AREAIDNULL             = -200402,   //为空
    AREAIDILLEGAL          = -200403,   //含非法字
    AREAIDOTHER            = -200499,   //其他错误

    //会话id
    SESSIDLONGER           = -200501,   //输入过长
    SESSIDNULL             = -200502,   //为空
    SESSIDILLEGAL          = -200503,   //含非法字
    SESSIDERRFORMAT        = -200504,   //格式错误（组成不合法）
    SESSIDOTHER            = -200599,   //其他错误

    //数据包id
    UNIQUEIDLONGER         = -200601,   //输入过长
    UNIQUEIDNULL           = -200602,   //为空
    UNIQUEIDILLEGAL        = -200603,   //含非法字
    UNIQUEIDERRFORMAT      = -200604,   //格式错误（组成不合法）
    UNIQUEIDREPEAT         = -200605,   //uniqueid重复
    UNIQUEIDOTHER          = -200699,   //其他错误

    //服务供应商编号
    SPIDLONGER             = -200701,   //输入过长
    SPIDNULL               = -200702,   //为空
    SPIDILLEGAL            = -200703,   //含非法字
    SPIDOTHER              = -200799,   //其他错误

    //组编号
    GROUPIDLONGER          = -200801,   //输入过长
    GROUPIDNULL            = -200802,   //为空
    GROUPIDILLEGAL         = -200803,   //含非法字
    GROUPIDOTHER           = -200899,   //其他错误

    //游戏道具编号
    ITEMIDLONGER           = -200901,   //输入过长
    ITEMIDNULL             = -200902,   //为空
    ITEMIDILLEGAL          = -200903,   //含非法字
    ITEMIDERRVALUE         = -200904,   //输入错误（未取特定值）
    ITEMIDOTHER            = -200999,   //其他错误

    //用户类型
    USERTYPELONGER         = -201101,   //输入过长
    USERTYPENULL           = -201102,   //为空
    USERTYPEILLEGAL        = -201103,   //含非法字
    USERTYPEOTHER          = -201199,   //其他错误

    //开始时间
    BEGINTIMELONGER        = -201301,   //输入过长
    BEGINTIMENULL          = -201302,   //为空
    BEGINTIMEILLEGAL       = -201303,   //含非法字
    BEGINTIMEERRLONGFOMAT  = -201304,   //格式错误，必须为YYYY-MM-DD hh:mi:ss
    BEGINTIMEERRSHORTFOMAT = -201305,   //含非法字，必须为YYYY-MM-DD
    BEGINTIMEOTHER         = -201399,   //其他错误

    //结束时间
    ENDTIMELONGER          = -201401,   //输入过长
    ENDTIMENULL            = -201402,   //为空
    ENDTIMEILLEGAL         = -201403,   //含非法字
    ENDTIMEERRLONGFOMAT    = -201404,   //格式错误，必须为YYYY-MM-DD hh:mi:ss
    ENDTIMEERRSHORTFOMAT   = -201405,   //含非法字，必须为YYYY-MM-DD
    ENDTIMEERRVALUE        = -201406,   //结束时间大于开始时间
    ENDTIMEOTHER           = -201499,   //其他错误

    //数据包id
    ORDERIDLONGER          = -201501,   //输入过长
    ORDERIDNULL            = -201502,   //为空
    ORDERIDILLEGAL         = -201503,   //含非法字
    ORDERIDERRFORMAT       = -201504,   //格式错误（组成不合法）
    ORDERIDREPEAT          = -201505,   //orderid重复
    ORDERIDOTHER           = -201599,   //其他错误

    //服务器编号
    HOSTIDLONGER           = -201701,   //输入过长
    HOSTIDNULL             = -201702,   //为空
    HOSTIDILLEGAL          = -201703,   //含非法字
    HOSTIDOTHER            = -201799,   //其他错误

    //付费类型
    PAYTYPELONGER          = -201901,   //输入过长
    PAYTYPENULL            = -201902,   //为空
    PAYTYPEILLEGAL         = -201903,   //含非法字
    PAYTYPEERRVALUE        = -201904,   //输入错误（未取特定值）
    PAYTYPEOTHER           = -201999,   //其他错误

    //消费类型
    APPTYPELONGER          = -202001,   //输入过长
    APPTYPENULL            = -202002,   //为空
    APPTYPEILLEGAL         = -202003,   //含非法字
    APPTYPEERRVALUE        = -202004,   //输入错误（未取特定值）
    APPTYPEOTHER           = -202099,   //其他错误

    //耦合类型
    COUPLETYPELONGER       = -202101,   //输入过长
    COUPLETYPENULL         = -202102,   //为空
    COUPLETYPEILLEGAL      = -202103,   //含非法字
    COUPLETYPEERRVALUE     = -202104,   //输入错误（未取特定值）
    COUPLETYPEOTHER        = -202199,   //其他错误

    //金额
    AMOUNTLONGER           = -202201,   //输入过长
    AMOUNTNULL             = -202202,   //为空
    AMOUNTILLEGAL          = -202203,   //含非法字
    AMOUNTNEGATIVE         = -202204,   //输入为负数
    AMOUNTZERO             = -202205,   //输入为零
    AMOUNTERRVALUE         = -202206,   //输入过大
    AMOUNTOTHER            = -202299,   //其他错误

    //计费折扣率
    DISCOUNTLONGER         = -202301,   //输入过长
    DISCOUNTNULL           = -202302,   //为空
    DISCOUNTILLEGAL        = -202303,   //含非法字
    DISCOUNTOTHER          = -202399,   //其他错误

    //客户端IP地址
    IPLONGER               = -202501,   //输入过长
    IPNULL                 = -202502,   //为空
    IPILLEGAL              = -202503,   //含非法字
    IPERRVALUE             = -202504,   //输入错误（不在指定范围之内）
    IPOTHER                = -202599,   //其他错误

    //预冻锁定锁定时间
    LOCKPERIODLONGER       = -202801,   //输入过长
    LOCKPERIODNULL         = -202802,   //为空
    LOCKPERIODILLEGAL      = -202803,   //含非法字
    LOCKPERIODERRVALUE     = -202804,   //输入错误（不在指定范围之内）
    LOCKPERIODOTHER        = -202899,   //其他错误

    //返回结果
    RESULTLONGER           = -204101,   //输入过长
    RESULTNULL             = -204102,   //为空
    RESULTILLEGAL          = -204103,   //含非法字
    RESULTERRVALUE         = -204104,   //输入错误（与指定值不一致）
    RESULTOTHER            = -204199,   //其他错误

    //需要响应标志
    RESPONSABLELONGER      = -204201,   //输入过长
    RESPONSABLENULL        = -204202,   //为空
    RESPONSABLEILLEGAL     = -204203,   //含非法字
    RESPONSABLEERRVALUE    = -204204,   //输入错误（与指定值不一致）
    RESPONSABLEOTHER       = -204299,   //其他错误

    //确认标志
    CONFIRMLONGER          = -204301,   //输入过长
    CONFIRMNULL            = -204302,   //为空
    CONFIRMILLEGAL         = -204303,   //含非法字
    CONFIRMERRVALUE        = -204304,   //输入错误（与指定值不一致）
    CONFIRMOTHER           = -204399,   //其他错误

    //状态
    STATUSLONGER           = -204401,   //输入过长
    STATUSNULL             = -204402,   //为空
    STATUSILLEGAL          = -204403,   //含非法字
    STATUSERRVALUE         = -204404,   //输入错误（与指定值不一致）
    STATUSOTHER            = -204499,   //其他错误

    //领奖类型
    AWARDTYPELONGER        = -204601,   //输入过长
    AWARDTYPENULL          = -204602,   //为空
    AWARDTYPEILLEGAL       = -204603,   //含非法字
    AWARDTYPEERRVALUE      = -204604,   //输入错误（与指定值不一致）
    AWARDTYPEOTHER         = -204699,   //其他错误

    //领奖号码
    AWARDNUMLONGER         = -205001,   //输入过长
    AWARDNUMNULL           = -205002,   //为空
    AWARDNUMILLEGAL        = -205003,   //含非法字
    AWARDNUMOTHER          = -205099,   //其他错误

    //领奖序列号
    AWARDSNLONGER          = -205101,   //输入过长
    AWARDSNNULL            = -205102,   //为空
    AWARDSNILLEGAL         = -205103,   //含非法字
    AWARDSNOTHER           = -205199,   //其他错误

    //发奖时间
    AWARDTIMELONGER        = -205201,   //输入过长
    AWARDTIMENULL          = -205202,   //为空
    AWARDTIMEILLEGAL       = -205203,   //含非法字
    AWARDTIMEERRLONGFOMAT  = -205204,   //格式错误，必须为YYYY-MM-DD hh:mi:ss
    AWARDTIMEERRSHORTFOMAT = -205205,   //含非法字，必须为YYYY-MM-DD
    AWARDTIMEOTHER         = -205299,   //其他错误

    //充入时长数量
    TIMEAMOUNTLONGER       = -205501,   //输入过长
    TIMEAMOUNTNULL         = -205502,   //为空
    TIMEAMOUNTILLEGAL      = -205503,   //含非法字
    TIMEAMOUNTNEGATIVE     = -205504,   //输入为负数
    TIMEAMOUNTZERO         = -205505,   //输入为零
    TIMEAMOUNTOTHER        = -205599,   //其他错误

    //充入时长类型
    TIMETYPELONGER         = -205601,   //输入过长
    TIMETYPENULL           = -205602,   //为空
    TIMETYPEILLEGAL        = -205603,   //含非法字
    TIMETYPEERRVALUE       = -205604,   //输入错误（未取特定值）
    TIMETYPEOTHER          = -205699,   //其他错误

    //充值批次号
    PATCHIDLONGER          = -206201,   //输入过长
    PATCHIDNULL            = -206202,   //为空
    PATCHIDILLEGAL         = -206203,   //含非法字
    PATCHIDERRVALUE        = -206204,   //输入错误（批次号不存在）
    PATCHIDOTHER           = -206299,   //其他错误

    //领奖批次号
    AWARDPATCHIDLONGER     = -206301,   //输入过长
    AWARDPATCHIDNULL       = -206302,   //为空
    AWARDPATCHIDILLEGAL    = -206303,   //含非法字
    AWARDPATCHIDERRVALUE   = -206304,   //输入错误（批次号不存在）
    AWARDPATCHIDOTHER      = -206399,   //其他错误

    //买家数字帐号
    BUYERSDIDLONGER        = -206401,   //输入过长
    BUYERSDIDNULL          = -206402,   //为空
    BUYERSDIDILLEGAL       = -206403,   //含非法字符
    BUYERSDIDOTHER         = -206499,   //其他错误

    //买家PT字符帐号
    BUYERPTIDLONGER        = -206501,   //输入过长
    BUYERPTIDNULL          = -206502,   //为空
    BUYERPTIDILLEGAL       = -206503,   //含非法字符
    BUYERPTIDALLNUM        = -206504,   //全为数字或以数字开头
    BUYERPTIDOTHER         = -206599,   //其他错误

    //买家组编号
    BUYERGROUPIDLONGER     = -206601,   //输入过长
    BUYERGROUPIDNULL       = -206602,   //为空
    BUYERGROUPIDILLEGAL    = -206603,   //含非法字
    BUYERGROUPIDOTHER      = -206699,   //其他错误

    //买家业务类型编号
    BUYERSERVIDLONGER      = -206701,   //输入过长
    BUYERSERVIDNULL        = -206702,   //为空
    BUYERSERVIDILLEGAL     = -206703,   //含非法字符
    BUYERSERVIDOTHER       = -206799,   //其他错误

    //买家游戏区编号
    BUYERAREAIDLONGER      = -206801,   //输入过长
    BUYERAREAIDNULL        = -206802,   //为空
    BUYERAREAIDILLEGAL     = -206803,   //含非法字
    BUYERAREAIDOTHER       = -206899,   //其他错误

    //保留备用字段(字符串)
    RESERVECHAROTHER       = -206999,   //其他错误

    //保留备用字段（整型）
    RESERVEINTOTHER        = -207099,   //其他错误

	//消息汇总——消息类型
	NOTIFYTYPELONGER       = -209301,   //输入过长
	NOTIFYTYPEUNLL         = -209302,   //为空
	NOTIFYTYPEILLEGAL      = -209303,   //含非法字
	NOTIFYTYPEOTHER        = -209399,   //其他错误
	
	//消息汇总——消息数据--整型
	NOTIFYDATALONGER       = -209401,   //输入过长
	NOTIFYDATAUNLL         = -209402,   //为空
	NOTIFYDATAILLEGAL      = -209403,   //含非法字 
	NOTIFYDATAERRVALUE     = -209404,   //输入错误（未取特定值）
	NOTIFYDATAOTHER        = -209499,   //其他错误

	//消息汇总——消息数据--字符串
	NOTIFYCONTENTLONGER    = -209501,   //输入过长
	NOTIFYCONTENTUNLL      = -209502,   //为空
	NOTIFYCONTENTILLEGAL   = -209503,   //含非法字
	NOTIFYCONTENTERRVALUE  = -209504,   //输入错误（未取特定值）
	NOTIFYCONTENTOTHER     = -209599,   //其他错误

    //有效道具种类数
    ITEMNUMZERO            = -209601,   //道具种类数为0
    ITEMNUMERRVALUE        = -209602,   //输入错误（未取特定值）
    ITEMNUMOTHER           = -209699,   //其他错误

    //游戏道具数量
    ITEMCOUNTZERO          = -209701,  //道具种类数为0
    ITEMCOUNTILLEGAL       = -209702,  //含非法字
    ITEMCOUNTOTHER         = -209799,  //其他错误

    //游戏道具单价
    ITEMPRICEZERO          = -209801,  //道具种类数为0
    ITEMPRICEILLEGAL       = -209802,  //含非法字
    ITEMPRICEOTHER         = -209899,  //其他错误
};

enum  //cbs业务处理错误码
{
    CBSDB_IS_DISCONNECT    = -300001,  //cbs数据库连接异常
    CBSDB_PROC_ERROR       = -300002,  //cbs sql执行错误
    UNAUTHOR_OPERATION     = -300003,  //未授权的请求包类型

    AWARD_TYPE_OUTLAW      = -300101,  //领奖类型错误（award_type取值范围1-3）
    AWARD_NUM_ERR          = -300102,  //领奖号码不存在
    AWARD_PITCH_ERR        = -300103,  //领奖批次号不存在
    AWARD_NO_AWARD         = -300104,  //玩家无奖品
    AWARD_HAS_TAKE         = -300105,  //玩家奖品已领取
    AWARD_LOCKED           = -300106,  //玩家奖品已锁定
    AWARD_SERV_ERR         = -300107,  //无此service
    AMOUNT_NOT_MATCHING    = -300201,  //金额不一致
};

enum   //lbs业务处理错误
{
    LBSDB_IS_DISCONNECT    = -400001,  //lbs数据库连接异常
    LBSDB_PROC_ERROR       = -400002,  //lbs sql执行错误
};

enum   //其他
{
    RES_TIMEOUT            = -390001,  //响应超时,占用一个3错误段的,要注意
    UNDECLARE_ERROR        = -1        //未定义或未知的错误
};

#endif
