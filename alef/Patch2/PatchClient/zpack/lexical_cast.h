#pragma once

#ifdef USE_ZPACK

#include <cstdlib>
#include <string>
#include <sstream>
#ifndef _DEBUG
#include <boost/lexical_cast.hpp>
#endif

/////////////////////////////////////
// T -> string or wstring

template< typename R >
struct __select_cast_func_ {
	template< typename T >
	inline static R cast( T t ) {
		return (R)t;
	}
};

template<typename R, typename T>
inline R lexical_cast( T t )
{
	return __select_cast_func_< R >::cast( t );
}

template<>
struct __select_cast_func_< std::string > {
	template< typename T >
	inline static std::string cast( T t )  { 
#ifdef _DEBUG
		std::stringstream str;
		str << t;
		return str.str();
#else
		return boost::lexical_cast< std::string >(t);
#endif
	}
};

template<>
struct __select_cast_func_< std::wstring > {
	template< typename T >
	inline static std::wstring cast( T t )  { 
		std::wstringstream str;
		str << t;// << std::ends;
		return str.str();
	}
};

////////////////////////////////////
// char const * -> R

template< typename R >
inline R lexical_cast( char const * t )
{
	return t ? t : R();
};

template<>
inline char* lexical_cast( char const * t )
{
	return t ? const_cast<char*>(t) : 0;
}

template<>
inline int lexical_cast( char const * t )
{
	return t ? atoi(t) : 0;
}

template<>
inline unsigned int lexical_cast( char const * t )
{
	return t ? (unsigned int)atoi(t) : 0;
}

template<>
inline short int lexical_cast( char const * t )
{
	return t ? (short int)atoi(t) : 0;
}

template<>
inline unsigned short int lexical_cast( char const * t )
{
	return t ? (unsigned short int)atoi(t) : 0;
}

template<>
inline long int lexical_cast( char const * t )
{
	return t ? (long int)atoi(t) : 0;
}

template<>
inline unsigned long int lexical_cast( char const * t )
{
	return t ? (unsigned long int)atoi(t) : 0;
}

template<>
inline float lexical_cast( char const * t )
{
	return t ? (float)atof(t) : 0;
}

template<>
inline double lexical_cast( char const * t )
{
	return t ? atof(t) : 0;
}

template<>
inline __int64 lexical_cast( char const * t )
{
	return t ? _atoi64( t ) : 0;
}

template<>
inline unsigned __int64 lexical_cast( char const * t )
{
	return t ? (unsigned __int64)_atoi64( t ) : 0;
}

template<>
inline std::wstring lexical_cast( char const * t )
{
	std::wstring r;

	if( t )
	{
		size_t len = strlen(t) + 1;
		wchar_t * buf = new wchar_t[ len ];
		mbstowcs_s( 0, buf, len, t, len );

		r = buf;
		delete[] buf;
	}

	return r;
}

////////////////////////////////////
// string -> R

template<typename R>
inline R lexical_cast( std::string const & t )
{
	return lexical_cast<R>(t.c_str());
}

template<>
inline std::string lexical_cast( std::string const & t )
{
	return t;
}

////////////////////////////////////
// wchar_t -> R

template< typename R >
inline R lexical_cast( wchar_t const * t )
{
	std::string s = lexical_cast< std::string >(t);
	return lexical_cast< R >( s.c_str() );
}

template<>
inline wchar_t * lexical_cast( wchar_t const  * t )
{
	return t ? const_cast< wchar_t * >(t) : 0;
}

template<>
inline std::string lexical_cast( wchar_t const * t )
{
	std::string r;

	if( t )
	{
		size_t len = wcslen(t) + 1;
		char * buf = new char[ len ];
		wcstombs_s( 0, buf, len, t, len );

		r = buf;
		delete[] buf;
	}

	return r;
}

///////////////////////////////////////
// wstring -> R

template<typename R>
inline R lexical_cast( std::wstring const & t )
{
	return lexical_cast<R>(t.c_str());
}

template<>
inline std::wstring lexical_cast( std::wstring const & t )
{
	return t;
}

///////////////////////////////////////

#endif