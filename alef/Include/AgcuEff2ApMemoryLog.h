// AgcuEff2ApMemoryLog.h: interface for the AgcuEff2ApMemoryLog class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFF2APMEMORYLOG_H__EAD4E1DB_9329_4E50_B348_FE3C51878376__INCLUDED_)
#define AFX_AGCUEFF2APMEMORYLOG_H__EAD4E1DB_9329_4E50_B348_FE3C51878376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#if !defined(EFF2_APMEMORY_LOG)
#define	EFF2_APMEMORY_LOG
#endif // EFF2_APMEMORY_LOG
#endif // _DEBUG

#ifdef EFF2_APMEMORY_LOG
#define EFFMEMORYLOG_SMV		static STAPMEMORYLOG	stApMemoryLog
#define EFFMEMORYLOG(type)		STAPMEMORYLOG type::stApMemoryLog(#type, sizeof(type))
#define EFFMEMORYLOG_CON		stApMemoryLog.CallMeAtConstructor()
#define EFFMEMORYLOG_DES		stApMemoryLog.CallMeAtDestructor()
#else
#define EFFMEMORYLOG_SMV		
#define EFFMEMORYLOG(type)		
#define EFFMEMORYLOG_CON		
#define EFFMEMORYLOG_DES		
#endif //EFF2_APMEMORY_LOG

#include "ApDefine.h"
#include <list>

class AgcmUIConsole;
struct	StApMemoryLog
{	
	explicit StApMemoryLog(const char* szClassName, int tsize=0 );
	~StApMemoryLog();

	bool	operator == (const char* szClassName)const;

	void	ToFile(const char* fname)const;
	void	ToFile(FILE* fp)const;
	LPCTSTR	GetString(void)const;

	void	CallMeAtConstructor();
	void	CallMeAtDestructor();
	
	const char*		m_szClassName;
	const int		m_nTSize;

	unsigned int	m_ulCnt;
	unsigned int	m_ulMax;
	unsigned int	m_ulTotalNew;
	unsigned int	m_ulTotalDel;
};

typedef struct StApMemoryLog	STAPMEMORYLOG, *PSTAPMEMORYLOG, *LPSTAPMEMORYLOG;

class AgcuEff2ApMemoryLog
{
	std::list< LPSTAPMEMORYLOG >	m_container;

private:
	AgcuEff2ApMemoryLog(){};
	~AgcuEff2ApMemoryLog(){};
public:
	static AgcuEff2ApMemoryLog& bGetInst();
	void	bAdd(LPSTAPMEMORYLOG pStMemLog);
	void	bToFile();

	void	bToAgcmUIConsole(AgcmUIConsole* pAgcmUIConsole)const;
	LPSTAPMEMORYLOG
			bFind( const char* szClassName );
};


#include <string>
namespace NS_EFF2PARTICLEPROFILE
{
	typedef unsigned int	UINT;
	typedef int				INT;
	typedef const char*		LPCSTR;
	struct stProfile
	{
		std::string		strName;
		INT				callcnt;
		UINT			accum;

		stProfile( LPCSTR szName, UINT accum )
			: strName(szName)
			, callcnt(1)
			, accum(accum)
		{
		};
		stProfile( const stProfile& cpy )
			: strName(cpy.strName)
			, callcnt(cpy.callcnt)
			, accum(cpy.accum)
		{
		};

		stProfile& operator = (const stProfile& cpy)
		{
			strName = cpy.strName;
			callcnt = cpy.callcnt;
			accum = cpy.accum;
		};

		bool operator == (const stProfile& cmp)const
		{
			return strName == cmp.strName;
		}

		bool operator == (LPCSTR szName)
		{
			return strName == szName;
		}
	};

	typedef void	(*funcptr)(const stProfile& prof);
	struct stProfiler
	{
		static std::list<stProfile>	container;

		UINT	starttick;
		LPCSTR	szName;

		stProfiler( LPCSTR	szName );
		~stProfiler();

		static void reset();
		static const stProfile* GetInfo(LPCSTR szName);
		static void forAll( funcptr ptrf );
	};
};


typedef struct tagStParticleProfile
{
	enum type{e_render, e_update, e_draw, e_ptrans, e_num};
	typedef unsigned int UL;

	union{
		UL	time[e_num];
		struct{
			UL	render;
			UL	update;
			UL	draw;
			UL	ptrans;
		}st;
	};

	tagStParticleProfile();
	void reset();
}STPARTICLEPROFILE, *PSTPARTICLEPROFILE, *LPSTPARTICLEPROFILE;

typedef struct tagStProfilerTimer
{
	typedef unsigned int UL;
	static tagStParticleProfile* pProfiler;
	UL*	updatetick;
	UL	begintick;

	tagStProfilerTimer(tagStParticleProfile::type t);
	~tagStProfilerTimer();
}PARTICLEPROFILER;

#endif