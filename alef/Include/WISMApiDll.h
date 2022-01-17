// Desc		: WISM API Layer의 Dll을 사용하는 Header.
// Warn		: 1.이것만 include하면 되게 해라.
//			: 1.사용자가 Dice와 WISMApiDll.h을 동시에 사용하는 경우 DiceApiDll.h를 먼저 include시켜야 한다.
// Copyright: Milkji -- All rights reserved.
// Update	: 20100624(milkji)
#pragma once


////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: Dll Export-Import
////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(WISMAPI_EXPORTS)
	#define	WISMAPI_XPORT	__declspec(dllexport)
#else
	#define	WISMAPI_XPORT	__declspec(dllimport)
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: switch&상수 정의
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sal.h>
#include <CodeAnalysis\SourceAnnotations.h>
#ifndef _INC_WINDOWS
#include <windows.h>
#endif /* _INC_WINDOWS */

// VS2008 미만 사용하는 경우 필요 (hjjin)
#if defined(_MSC_VER)
	#if(_MSC_VER < 1500)
		#define	_Check_return_
		#define	_In_
		#define	_In_z_
		#define	_Inout_
		#define	_In_opt_
		#define	_In_opt_z_
		#define	_Out_
		#define	_Out_opt_
		#define	_Printf_format_string_
		#define	_In_count_(x)
		#define	_In_z_count_(x)
		#define	_In_bytecount_(x)
		#define	_In_opt_bytecount_(x)
		#define	_Out_cap_(x)
		#define	_Out_z_cap_(x)
		#define	_Out_bytecap_(x)
		#define	_Inout_bytecount_(x)
	#endif
#endif

namespace WISM
{
	enum
	{
		eNumberQueueSize=512,					/*! 숫자Queue의 크기						*/
		eStringQueueSize=512,					/*! 문자Queue의 크기						*/
		eMaxLengthOfString=512,
		eHeartbeatIdentification=1,				/*! 서비스가 살았음을 의미하는 식별자		*/
		eConcurrentUserCountIdentification=2,	/*! 현재 연결된 사용자 수(동접) 식별자	*/
	};

	enum ENotificationNumber
	{
		eNotificationNumberValue=0,				/*!	현재값								*/
		eNotificationNumberMin,					/*! 최소값								*/
		eNotificationNumberMax,					/*! 최대값								*/
		eNotificationNumberCount				/*! 통보해야할 숫자의 개수				*/
	};

	const	float	gMeaninglessValue=-99999.0f;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Desc	: class정의
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace WISM
{
	/*
	 *	@brief	WISMApi사용자와 WISM Agent간의 숫자Data 전달 형식
	 */
	class WISMAPI_XPORT	CNotificationNumber
	{
	public:	explicit CNotificationNumber(_In_ const int pSystemId=0, _In_ const int pActivityId=0, _In_ const float pValue=gMeaninglessValue, _In_ const float pMin=gMeaninglessValue, _In_ const float pMax=gMeaninglessValue) throw();

	public:	SYSTEMTIME	mRegisteredTime;					// 등록시간(local time임)
	public:	int			mSystemId;
	public:	int			mActivityId;
	public:	float		mNumber[eNotificationNumberCount];	// gMeaninglessValue면 의미없는 값임.
	};

	/*
	 *	@brief	WISMApi사용자와 WISM Agent간의 문자Data 전달 형식
	 */
	class WISMAPI_XPORT	CNotificationString
	{
	public:	explicit CNotificationString(_In_ const int pSystemId=0, _In_ const int pActivityId=0, _In_z_ const wchar_t* pString=L"") throw();
				
	public:	SYSTEMTIME	mRegisteredTime;					// 등록시간(local time임)
	public:	int			mSystemId;
	public:	int			mActivityId;
	public:	wchar_t		mString[eMaxLengthOfString+1];
	};


	/*!
	 *	@brief	퍼블리셔에게 통보하는 abstract base class
	 *	@usage	호출순서는 Open()->Notify()의 반복->Close()다.
	 */
	class WISMAPI_XPORT CNotification
	{
	public:	CNotification() throw();
	public:	virtual ~CNotification() throw()=0;

	public:	_Check_return_	virtual unsigned int	Open(void) throw()=0;
	public:					virtual void			Close(void) throw()=0;
	public:	_Check_return_			bool			IsOpened(void) const throw();

	/*!
	 *	@brief	값(pValue) 또는 문자열(pString)를 전달함.
	 *	@return	성공하면 0, 이외는 오류코드다.
	 */
	public:	_Check_return_	virtual unsigned int	NotifyNumber(_In_ const CNotificationNumber& pNumber) throw()=0;
	public:	_Check_return_	virtual unsigned int	NotifyStringA(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const char* pString) throw()=0;
	public:	_Check_return_	virtual unsigned int	NotifyStringW(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const wchar_t* pString) throw()=0;
	public:	_Check_return_			unsigned int	NotifyHeartbeat(_In_ const int pSystemId) throw();
	public:	_Check_return_			unsigned int	NotifyConcurrentUserCount(_In_ const int pSystemId,  _In_ const float pValue) throw();

	/*!
	 *	@brief	값을 가져온다.
	 *	@param	pString	문자열과 NULL을 저장할 만큼 충분한 공간을 caller가 확보해서 전달해야 한다.
	 *	@remark	overriding할때 base를 호출하지 말아라.
	 */
	public:	_Check_return_	virtual unsigned int	GetNumber(_Out_ CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	GetStringW(_Out_ CNotificationString& pString) throw();

	protected:bool			mIsOpened;
	};


	/*!
	 *	@brief	웹젠의 WISM에게 통보함.
	 */
	class WISMAPI_XPORT CWISMNotification : public CNotification
	{
	private:typedef	CNotification	TSuper;
	public:					CWISMNotification() throw();
	public:	virtual			~CWISMNotification() throw();
	private:				CWISMNotification(_In_ const CWISMNotification&) throw()				{}
	private:_Check_return_	CWISMNotification&	operator=(_In_ const CWISMNotification&) throw()	{return(*this);}
	private:void			Ctor(void) throw();
	private:void			Dtor(void) throw();

	public:	_Check_return_	virtual unsigned int	Open(void) throw();
	public:					virtual void			Close(void) throw();
	

	/*!
	 *	@remark	queue가 가득찬 경우에는 오래된 것을 삭제하고 넣는다. 단, 오류를 반환한다.
	 *			-> Close()할 필요는 없다.
	 *			호출순서 및 인수의 유효성은 성능을 위해서 검사하지 않는다.(일반적으로 호출자가 확인함으로)
	 *			MBCS보다는 UNICODE가 속도가 좋다. 가능하면 UNICODE를 사용해라.
	 */
	public:	_Check_return_	virtual unsigned int	NotifyNumber(_In_ const CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	NotifyStringA(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const char* pString) throw();
	public:	_Check_return_	virtual unsigned int	NotifyStringW(_In_ const int pSystemId, _In_ const int pActivityId,_In_z_ const wchar_t* pString) throw();

	/*!
	 *	@remark	오류가 발생하면 Close()->Open()을 해야 한다.
	 *	@return	data가 있으면 0, 없으면 ERROR_EMPTY, 이외는 오류코드.
	 */
	public:	_Check_return_	virtual unsigned int	GetNumber(_Out_ CNotificationNumber& pNumber) throw();
	public:	_Check_return_	virtual unsigned int	GetStringW(_Out_ CNotificationString& pString) throw();
	};
};



namespace WISM
{
	inline	bool	CNotification::IsOpened(void) const
	{
		return(mIsOpened);
	}

	inline	unsigned int	CNotification::NotifyHeartbeat(const int pSystemId)
	{
		return(NotifyNumber(CNotificationNumber(pSystemId, eHeartbeatIdentification, 0)));
	}

	inline	unsigned int	CNotification::NotifyConcurrentUserCount(const int pSystemId, const float pValue)
	{
		return(NotifyNumber(CNotificationNumber(pSystemId, eConcurrentUserCountIdentification, pValue)));
	}
};