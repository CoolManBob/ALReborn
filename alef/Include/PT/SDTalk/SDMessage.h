#pragma once
#include <queue>
using namespace std;

class CSDMessage
{
public:
	CSDMessage();
	~CSDMessage();


#pragma	pack(2)
	struct MsgHead
	{
		unsigned short msg_cmd;
		unsigned long  trans_id;
		unsigned short game_type;
		unsigned short major_v;
		unsigned short minor_v;
		unsigned short area_code;
		unsigned short mark_code;
	};
#pragma pack()

public:
	MsgHead	m_Head;

	inline int	GetContentCount();
	const char*	GetContent(int index);
	void SetContent(int index, const char * cs_content);
	void AddContent(const char * cs_content);
	void AddContent(int i_content);
	void RemoveContent(int index, int count = 1);
	void ClearContent();

	int	 GetSize();
	void ToBuf(unsigned char * buf);
	void FromBuf(const char * buf, int length);

	void CopyHead(CSDMessage & a);
	void CopyHead(MsgHead & head);
	void CopyContent(CSDMessage & a);
	CSDMessage & operator = (CSDMessage & a);

	const char * operator[](int index){return GetContent(index);}

protected:
	vector<const char *> m_aContent;

	long ntohl(long a);
	short ntohs (short a);

private:
	void CutTail();
};

#define SDHeadSize sizeof(CSDMessage::MsgHead)