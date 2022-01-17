// AgcuEff2ApMemoryLog.cpp: implementation of the AgcuEff2ApMemoryLog class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEff2ApMemoryLog.h"

#include "AgcuEffUtil.h"

#include "ApMemoryTracker.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

#include <algorithm>
#include "AgcmUIConsole.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPCSTR	EFF2_MEMLOGFILE	= "eff2_ApMemoryLog_at_programexit.txt";

StApMemoryLog::StApMemoryLog(const char* szClassName, int tsize)
	: m_szClassName(szClassName)
	, m_nTSize		(tsize)
	, m_ulCnt		(0LU)
	, m_ulMax		(0LU)
	, m_ulTotalNew	(0LU)
	, m_ulTotalDel	(0LU)
{
	AgcuEff2ApMemoryLog::bGetInst().bAdd(this);
}

StApMemoryLog::~StApMemoryLog()
{
	static BOOL b1st = TRUE;
	if( b1st )
	{
		b1st	= FALSE;
		FILE*	fp	= fopen("eff2_ApMemoryLog_at_programexit.txt", "at");
		if( fp )
		{
			Eff2Ut_TimeStampToFile(fp);
			fclose(fp);
		}
	}
	ToFile("eff2_ApMemoryLog_at_programexit.txt");
}

void StApMemoryLog::ToFile(const char* fname)const
{
	AuAutoFile	fp(fname, "at");
	if( fp )
	{
		fprintf(fp, 
			"Cnt : %8d, Max : %8d, new : %8d, del : %8d, <%3d>%s\n"
			, m_ulCnt		
			, m_ulMax		
			, m_ulTotalNew	
			, m_ulTotalDel	
			, m_nTSize
			, m_szClassName);
	}
}
void StApMemoryLog::ToFile(FILE* fp)const
{
	ASSERT(fp);
	fprintf(fp, 
		"Cnt : %8d, Max : %8d, new : %8d, del : %8d, <%3d>%s\n"
		, m_ulCnt		
		, m_ulMax		
		, m_ulTotalNew	
		, m_ulTotalDel	
		, m_nTSize
		, m_szClassName);
}
LPCTSTR	StApMemoryLog::GetString(void)const
{
	static ApString<127> buff;
	buff.Format(_T("Cnt : %8d, Max : %8d, new : %8d, del : %8d, <%3d>%s\n")
		, m_ulCnt		
		, m_ulMax		
		, m_ulTotalNew	
		, m_ulTotalDel	
		, m_nTSize
		, m_szClassName);
	return buff.GetBuffer();
};

void StApMemoryLog::CallMeAtConstructor()
{
	++m_ulCnt;
	++m_ulTotalNew;

	
	unsigned int currMax	= m_ulMax;
	m_ulMax = T_MAX(m_ulMax, m_ulCnt);
}

void StApMemoryLog::CallMeAtDestructor()
{
	--m_ulCnt;
	++m_ulTotalDel;
}
bool StApMemoryLog::operator == (const char* szClassName) const
{
	if( strcmp( m_szClassName, szClassName ) )
		return false;
	return true;
}

AgcuEff2ApMemoryLog& AgcuEff2ApMemoryLog::bGetInst()
{
	static AgcuEff2ApMemoryLog	inst;
	return inst;
};
void AgcuEff2ApMemoryLog::bAdd(LPSTAPMEMORYLOG pStMemLog)
{
	if( pStMemLog )
		m_container.push_back(pStMemLog);
};
void AgcuEff2ApMemoryLog::bToFile()
{
	AuAutoFile	fp(EFF2_MEMLOGFILE, "at");
	static BOOL b1st = TRUE;
	if( b1st )
	{
		b1st	= FALSE;
		if( fp )
		{
			Eff2Ut_TimeStampToFile(fp);
		}
	}

	if(fp)
	{
		std::list< LPSTAPMEMORYLOG >::iterator	it_curr	= m_container.begin();
		for( ; it_curr != m_container.end(); ++it_curr )
		{
			if( *it_curr )
				(*it_curr)->ToFile(fp);
		}
	}
};

void AgcuEff2ApMemoryLog::bToAgcmUIConsole(AgcmUIConsole* pAgcmUIConsole)const
{
	if(pAgcmUIConsole)
	{	
		std::list< LPSTAPMEMORYLOG >::const_iterator	it_curr	= m_container.begin();
		for( ; it_curr != m_container.end(); ++it_curr )
			pAgcmUIConsole->getConsole().print( (*it_curr)->GetString() );
	}
};

LPSTAPMEMORYLOG AgcuEff2ApMemoryLog::bFind( const char* szClassName )
{
	LPSTAPMEMORYLOG	pr	= NULL;

	std::list< LPSTAPMEMORYLOG >::iterator	it_curr	= m_container.begin();
	for( ; it_curr != m_container.end(); ++it_curr )
	{
		LPSTAPMEMORYLOG pStMemLog = *it_curr;
		if( *pStMemLog == szClassName )
			pr	= pStMemLog;
	}

	return (pr);
};



STPARTICLEPROFILE	g_particleProFile;
tagStParticleProfile::tagStParticleProfile()
{
	reset();
}
void tagStParticleProfile::reset()
{
	st.render	= 
	st.update	= 
	st.draw		= 
	st.ptrans	= 0LU;
};

tagStParticleProfile*	tagStProfilerTimer::pProfiler	= &g_particleProFile;
tagStProfilerTimer::tagStProfilerTimer(tagStParticleProfile::type t)
{
	updatetick	= &pProfiler->time[t];
	begintick	= timeGetTime();
};
tagStProfilerTimer::~tagStProfilerTimer()
{
	*updatetick	+= (timeGetTime() - begintick);
};



namespace NS_EFF2PARTICLEPROFILE
{
	std::list<stProfile>	stProfiler::container;
	stProfiler::stProfiler(LPCSTR szName)
		: starttick( timeGetTime() )
		, szName( szName )
	{
	};

	stProfiler::~stProfiler()
	{
		typedef std::list<stProfile>::iterator ITR;
		ITR	it_f	= std::find( container.begin(), container.end(), szName );
		if( it_f == container.end() )
		{
			container.push_back( stProfile( szName, timeGetTime()-starttick ) );
		}
		else
		{
			(*it_f).accum	+= timeGetTime()-starttick;
			++((*it_f).callcnt);
		}
	};

	class fncter
	{
	public:
		void operator	() (stProfile& prof)
		{
			prof.accum = 0;
			prof.callcnt = 0;
		}
	};
	void stProfiler::reset()
	{
		std::for_each( container.begin(), container.end(), fncter() );
	};
	const stProfile* stProfiler::GetInfo(LPCSTR szName)
	{
		typedef std::list<stProfile>::iterator ITR;
		ITR	it_f	= std::find( container.begin(), container.end(), szName );
		if( it_f == container.end() )
			return NULL;
		else
			return &(*it_f);
	};

	void stProfiler::forAll( funcptr ptrf )
	{
		if( ptrf )
			std::for_each( container.begin(), container.end(), ptrf );
	};
};