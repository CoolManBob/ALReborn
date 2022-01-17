#ifndef _CSINGLETON_H_
#define _CSINGLETON_H_

// Method1: template class
#include <cassert>

template<typename Derived> class CSingleton
{
private:
	static Derived* ms_pSingleton;

protected:
	// constructor and destructor
	CSingleton(void);
	virtual ~CSingleton(void);

public:

	inline static Derived& GetInst(void);
	inline static Derived* GetInstPtr(void);
};

template<typename Derived> CSingleton<Derived>::CSingleton( void )
{
	assert(!ms_pSingleton && "Singleton 클래스가 이미 생성되어 있습니다.");
	size_t nOffset = (size_t)(Derived*) 1 - (size_t)(CSingleton<Derived>*)(Derived*) 1;
	ms_pSingleton = (Derived*)((size_t)this + nOffset);
}

template<typename Derived> CSingleton<Derived>::~CSingleton( void )
{
	assert(ms_pSingleton && "Singleton 클래스가 생성되지 않았습니다");
    ms_pSingleton = 0;
}

template<typename Derived> inline Derived& CSingleton<Derived>::GetInst( void )
{
	assert( ms_pSingleton && "Singleton 클래스가 생성되지 않았습니다" );
    return ( *ms_pSingleton );
}

template<typename Derived> inline Derived* CSingleton<Derived>::GetInstPtr( void )
{
	assert( ms_pSingleton && "Singleton 클래스가 생성되지 않았습니다" );
    return ( ms_pSingleton );
}

template<typename Derived> Derived* CSingleton<Derived>::ms_pSingleton = 0;

// Method2: MACRO
#define DECLARE_SINGLETON_CLASS( cmName )	\
private: \
  static cmName	s_Instance;	\
public: \
	static cmName*	GetInst(void)		{ return &s_Instance; }	\
	static cmName*	GetInstPtr(void)	{ return &s_Instance; }

#define DEFINE_SINGLETON_CLASS( cmName )	cmName cmName::s_Instance;

#endif