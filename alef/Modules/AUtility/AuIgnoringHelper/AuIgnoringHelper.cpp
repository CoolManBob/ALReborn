#include "AuIgnoringHelper.h"
#include <list>
#include <string>
#include <cstdio>
#include <WinSock2.h>

//-----------------------------------------------------------------------
//

bool AuIgnoringHelper::allow_ = false;

//-----------------------------------------------------------------------
//

bool AuIgnoringHelper::IgnoreToFile( char * file )
{
	Instance();

	bool result = false;

	if( !allow_ )
		result;

	FILE * f = 0;

	fopen_s( &f, file, "rb" );

	if( f )
	{
		fclose(f);

		result = true;
	}

	return result;
}

//-----------------------------------------------------------------------
//

AuIgnoringHelper & AuIgnoringHelper::Instance()
{
	static AuIgnoringHelper inst;

	return inst;
}

//-----------------------------------------------------------------------
//

AuIgnoringHelper::AuIgnoringHelper()
{
	typedef std::list<std::string> IPs;

	IPs auth_ips;
	IPs my_ips;

	auth_ips.push_back( "10.95." );
	auth_ips.push_back( "10.16." );
	auth_ips.push_back( "10.1." );

	char buf[1024] = {0,};
	PHOSTENT hostInfo;

	// 자신의 아이피를 가져옴
	gethostname(buf, sizeof(buf));
	hostInfo = gethostbyname(buf);

	if( hostInfo )
	{
		sockaddr_in addr;

		for(int i=0 ; hostInfo->h_addr_list[i] != NULL ; i++)      
		{
			memcpy(&addr.sin_addr, hostInfo->h_addr_list[i],hostInfo->h_length);
			char* tmp = inet_ntoa(addr.sin_addr);
			my_ips.push_back(tmp ? tmp : "");
		}
	}

	// 자신의 아이피가 허용 아이피인지 확인
	for( IPs::iterator my_ip = my_ips.begin(); my_ip != my_ips.end(); ++my_ip )
	{
		for( IPs::iterator auth_ip = auth_ips.begin(); auth_ip != auth_ips.end(); ++auth_ip )
		{
			if( my_ip->find( *auth_ip ) != std::string::npos )
			{
				allow_ = true;
				break;
			}
		}

		if( allow_ )
			break;
	}
}

//-----------------------------------------------------------------------
//

AuIgnoringHelper::~AuIgnoringHelper()
{}

//-----------------------------------------------------------------------