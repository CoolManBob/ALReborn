#include "StdAfx.h"
#include ".\bindstatuscallback.h"
#include <shlwapi.h>
#include <mmsystem.h>
#include <iostream>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////
//. \brief 생성자
//. \param url 다운로드받을 파일의 URL
//. \param timeout 타임아웃
CBindStatusCallback::CBindStatusCallback(const std::string& url, DWORD timeout)
: m_URL(url), m_Timeout(timeout)
{
    assert(!m_URL.empty());
}

CBindStatusCallback::~CBindStatusCallback()
{	
}

//////////////////////////////////////////////////////////////////////////////
//. \brief 진행 상황을 알려주기 위해 인터넷 익스플로러가 호출하게 되는
//. 콜백 함수
//.
//. \param ulProgress 다운로드받은 바이트 수
//. \param ulProgressMax 다운로드받아야할 전체 바이트 수
//. \param ulStatusCode 상태 코드
//. \param wszStatusText 상태 문자열
//. \return HRESULT 인터넷 익스플로러에게 알려줘야하는 결과값. 다운로드를
//. 계속해야하는 경우에는 S_OK를 반환해야하고, 중단하려면 E_ABORT를 반환해야 
//. 한다.
HRESULT STDMETHODCALLTYPE CBindStatusCallback::OnProgress(ULONG ulProgress, 
                                                ULONG ulProgressMax,
                                                ULONG ulStatusCode, 
                                                LPCWSTR wszStatusText)
{
    //. 이 함수 내부에서 혹시 사용자가 중지 버튼을 눌렀다던가, 
    //. 타임아웃이 되었다던가, ulStatusCode가 이상하던가 하면,
    //. E_ABORT를 반환해서, 다운로드를 중단해야 한다.
    if (m_Timeout < timeGetTime())
    {
		std::cout << STATUS_MSG_WAITOVER << std::endl;
        return E_ABORT;
    }
    
    //. 다운로드를 중지해야하는 상황이 아니라면, 
    //. ulProgress 값과, ulProgressMax 값을 이용해 
    //. 프로그레스바 등을 적당히 업데이트하면 된다.
    if (0 != ulProgressMax)
    {
        std::cout << STATUS_MSG_DOWNLOADING << m_URL 
            << "(" << int( 100.0 * ulProgress / ulProgressMax ) << "%)" << std::endl;
    }
    else
    {
        std::cout << STATUS_MSG_DOWNLOADING << m_URL << std::endl;
    }

    return S_OK;
}