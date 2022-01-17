#ifndef _ERRCODE_H_
#define _ERRCODE_H_

enum
{
	BSIP_OK			  = 0,
    SOCKDISCONNECT    = -1001,  //socket连接断开
    SOCKIP_OR_PORTERR = -1002,  //服务器的ip port配置错误
    SOCKCONNECTFAIL   = -1003,  //sock连接失败
    NOINITCLIENT      = -1011,  //没有初始化的客户端

    QUEUECACHEFULL    = -2001,  //队列缓存数满
    QUEUELOCKERROR    = -2002,  //队列锁定错误
    QUEUEOPERATEERROR = -2003,  //队列Push Pop传入参数错(空指针)
    QUEUEEMPTY        = -2004,  //队列空，Pop无数据
    QUEUEDATAERR      = -2005,  //缓存数据的内容错，如长度大于小于可缓存的范围

    USERDATALENERROR  = -3001,  //用户数据Push长度和业务结构的长度不一直
    COMMUDATAERROR    = -3002,  //接收的通讯数据包加密错

    POINTERNULL       = -4001,  //传入了空指针(需要指针的地方是不能接收空指针的)

    ENCRYPTERROR      = -5001,  //加密错
    DECRYPTERROR      = -5002,  //解密错

    PARAMPATHERROR    = -6001,  //缓存或者环形文件路径错误
	CONFLOADFAILED	  = -6002,  //配置文件加载失败
	CONFGETFAILED	  = -6003,	//从配置服务器获取配置失败
	RELOADFAILED	  = -6004,  //加载Re失败
	
	BSIP_BE_START_FAILD			=-7001,//引擎启动失败
	BSIP_BE_LOGIN_FAILD			=-7002,//用户登入失败
	BSIP_BE_LOGOUT_FAILD		=-7003,//用户注销失败
	BSIP_BE_CHANGEMAP_FAILD		=-7004,//用户跳地图失败
	BSIP_BE_USER_ALREADY_EXIST	=-7005,//用户已经存在引擎中
	BSIP_BE_USER_NOT_EXIST		=-7006,//用户不存在
	BSIP_BE_MEMORY_ERROR		=-7007,//引擎分配内存错误

    PUSHBUFF          = 9001   //放入缓存正确，此返回值表示发送成功，非错误代码
};

#define BSIP_BE_OK		0

#endif
