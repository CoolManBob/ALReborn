#pragma once
#include "sdkconfig.h"
#include "sdtalk\SDMessage.h"
#include "PacketProcessor.h"
#include "ThreadMaster.h"
#include "ByteIO.h"
#include "lock\Lock.h"
#include "SDMsgProcessor.h"
#include "QueueWithLock.h"
using namespace CleverLock;

#define SUCCEESS	0

class PTSDK_STUFF CSDPipeHole
{
public:
	CSDPipeHole();
	~CSDPipeHole();
	enum {RecvBufSize =1024};
	struct RecvBuffer
	{
		int		Size;
		int		ValidOffset;	
		unsigned char	Buffer [RecvBufSize];
	}m_RecvBuffer;

	bool IsConnect();

	CByteIO	* m_pByteIO;
	void * m_pParameter;
	
	CThreadMaster m_RecvThread, m_SendThread;
};

class PTSDK_STUFF CSDMsgPipe
{
public:
	CSDMsgPipe(void);
	~CSDMsgPipe(void);

	void AddByteIO( CByteIO * p_byteIO);
	void RemoveByteIO( CByteIO * p_byteIO);

	int	SendSDMessage(CSDMessage &sdmessage);
	int GetBytoIOSize() {return (int)m_Holes.size(); };

	void SetPacketProcessor( CPacketProcessor* p_preSendPro, CPacketProcessor* p_postRcvPro);    	
	void SetMsgProcessor( CSDMsgProcessor * p_processor);
	bool IsConnect();
	void Clear();

	enum {ComeMsg = 0, ReboundMsg};
protected:
	
	vector<CSDPipeHole * > m_Holes;
	CSDMsgProcessor * m_pMsgProcessor;
	static void RecvFunction( void * lpParameter );
	static void SendFunction( void * lpParameter );	

	CQueueWithLock<CSDPacket *> m_SendQueue;

	CPacketProcessor * m_pPreSendPro,
					 * m_pPostRcvPro;	

	
	enum {RecvCycle = 1000, SendCycle=1000};	
};
