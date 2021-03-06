#include "zpack.h"
#include <cwctype>

#ifdef USE_ZPACK

//namespace umtl
//{
	/*
	struct mat_t
	{
		typedef float elem_type;

		elem_type a, b, c, d, end;

		mat_t() : a(0), b(0), c(0), d(0), end(0) {}

		inline mat_t& operator!()	{
			elem_type discriminant = a*d - b*c;

			if( discriminant != 0 )
			{
				elem_type t = a;
				a = d;
				d = t;

				b = -b;
				c = -c;

				*this *= (1/discriminant);
			}

			return *this;
		}

		inline mat_t & operator*=(elem_type v)	{
			a *= v;	b *= v;	c *= v;	d *= v;
			return *this;
		}

		std::string str()
		{
			char s[5];

			s[0] = (char)floor(a+0.5f);
			s[1] = (char)floor(b+0.5f);
			s[2] = (char)floor(c+0.5f);
			s[3] = (char)floor(d+0.5f);
			s[4] = 0;

			return s;
		}
	};

	//-----------------------------------------------------------------------
	// 어셈블리 리버싱으로 dll 이름과 함수이름을 체크하여
	// 호출시점에 패스워드를 후킹하는것을 방지하기 위해
	// dll 이름과 로딩할 함수이름들을 암호화

	inline std::wstring StrEncoding( std::wstring str )
	{
		if( str.empty() )
			return str;

		typedef mat_t::elem_type elem_type;

		size_t bufLen = str.length() * sizeof( elem_type ) + 1;

		std::wstring result( bufLen, 0 );

		wchar_t * buf = (wchar_t*)result.c_str();

		for( size_t i=0; i<(str.length()/4); ++i )
		{
			mat_t m;

			m.a = (elem_type)str[i*4+0];
			m.b = (elem_type)str[i*4+1];
			m.c = (elem_type)str[i*4+2];
			m.d = (elem_type)str[i*4+3];

			m = !m;

			elem_type * fbuf = (elem_type*)buf;

			fbuf[i*4+0] = m.a;
			fbuf[i*4+1] = m.b;
			fbuf[i*4+2] = m.c;
			fbuf[i*4+3] = m.d;
		}

		for( size_t i=0; i<(str.length()%4); ++i )
		{
			elem_type * fbuf = (elem_type*)buf;
			fbuf[ str.length() - 1 - i ] = str[ str.length() - 1 - i ];
		}

		for( size_t i=0; i<result.length(); ++i )
		{
			result[i] ^= (wchar_t)result.length();
			result[i] = ~result[i];
		}

		return result;
	}

	inline std::string StrDecoding( std::wstring str )
	{
		if( str.empty() )
			return "";

		for( size_t i=0; i<str.length(); ++i )
		{
			str[i] ^= (wchar_t)str.length();
			str[i] = ~str[i];
		}

		typedef mat_t::elem_type elem_type;

		size_t d = (str.length()/sizeof(elem_type));
		size_t n = d%4;

		size_t bufLen = d + n;

		std::wstring result( bufLen, 0 );
		wchar_t * buf = (wchar_t*)result.c_str();

		elem_type * s = (elem_type*)str.c_str();

		for( size_t i=0; i<(d/4); ++i )
		{
			mat_t m;

			m.a = s[i*4+0];
			m.b = s[i*4+1];
			m.c = s[i*4+2];
			m.d = s[i*4+3];

			m = !m;

			memcpy_s( buf+i*4, 8, m.str().c_str(), 8 );
		}

		for( size_t i=0; i < n; ++i )
			buf[d-n+i] = (wchar_t)s[d-n+i];

		//return result;

		return lexical_cast< std::string >(result);
	}
	*/

	//--------------------------------------------------
	//

	HINSTANCE			zpack::dll_				= 0;
	ZpackFind			zpack::find_			= 0;
	ZpackAdd			zpack::add_				= 0;
	ZpackDelete			zpack::del_				= 0;
	ZpackExtract		zpack::extract_			= 0;
	ZpackFolderInfo		zpack::folderInfo_		= 0;
	ZpackRelease		zpack::release_			= 0;
	ZpackSetAllocor		zpack::setAllocor_		= 0;	
	ZpackSetTmpAllocor	zpack::setTmpAllocor_	= 0;
	ZpackWorkDir		zpack::workDir_			= 0;
	ZpackClearCache		zpack::clearCache_		= 0;
	CompressLZMA		zpack::compressLZMA_	= 0;
	UnCompressLZMA		zpack::unCompressLZMA_	= 0;

	//--------------------------------------------------
	//

	zpackBuffer::zpackBuffer()
		: pack_name_(0), release_(0), data_(0), size_(0)
	{}

	zpackBuffer::zpackBuffer(wchar_t const * pack_name, ZpackRelease release)
		: pack_name_(pack_name), release_(release), data_(0), size_(0)
	{}

	zpackBuffer::~zpackBuffer() { 
		release();
	}

	zpackBuffer::zpackBuffer( zpackBuffer & other )
		: pack_name_(other.pack_name_)
		, release_(other.release_)
		, data_(other.data_)
		, size_(other.size_)
	{
		other.clear();
	}

	zpackBuffer & zpackBuffer::operator=( zpackBuffer & other )
	{
		release();

		pack_name_ = other.pack_name_;
		release_ = other.release_;
		data_ = other.data_;
		size_ = other.size_;

		other.clear();

		return *this;
	}

	void zpackBuffer::clear()
	{
		data_ = 0;
		size_ = 0;
		pack_name_ = 0;
		release_ = 0;
	}

	void zpackBuffer::release()
	{
		if( data_ )
		{
			if( pack_name_ && release_ )
				(*release_)( pack_name_, (wchar_t const *)data_ );
			else
				delete[] data_;
		}

		clear();
	}

	//--------------------------------------------------
	//

	zpackBuffer zpack::find( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * password ) {
		init();
		zpackBuffer buffer(pack_name, release_);
		if ( dll_ && find_ )
			buffer.data_ = (*find_)( pack_name, filename, buffer.size_, password );
		return buffer;
	}

	bool zpack::del( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * password ) {
		init();
		return ( dll_ && del_ ) ? (*del_)( pack_name, filename, password ) : false;
	}

	bool zpack::add( wchar_t const * pack_name, wchar_t const * filename, int level, wchar_t const * password ) {
		init();
		return ( dll_ && add_ ) ? (*add_)( pack_name, filename, level, password ) : false;
	}

	bool zpack::extract( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * outDir, bool fullPath, wchar_t const * password ) {
		init();
		return ( dll_ && extract_ ) ? (*extract_)( pack_name, filename, outDir, fullPath, password ) : false;
	}

	void zpack::setAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) )
	{
		init();
		if( dll_ && setAllocor_ ) (*setAllocor_)( pack_file, allocFunc, freeFunc );
	}

	void zpack::setTmpAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) )
	{
		init();
		if( dll_ && setTmpAllocor_ ) (*setTmpAllocor)( pack_file, allocFunc, freeFunc );
	}

	wchar_t const * zpack::folderInfo( wchar_t const * pack_file, wchar_t const * path, wchar_t const * password )
	{
		init();
		return (dll_ && folderInfo_) ? (*folderInfo_)( pack_file, path, password ) : 0;
	}

	void zpack::setWorkDir( wchar_t const * pack_file, wchar_t const * workDir )
	{
		init();
		if( dll_ && workDir_ ) (*workDir_)( pack_file, workDir );
	}

	void zpack::clearCache( wchar_t const * pack_file )
	{
		init();
		if( dll_ && clearCache_ ) (*clearCache_)( pack_file );
	}

	zpackBuffer zpack::compress( unsigned char const * src, unsigned int srcLen, int level )
	{
		init();

		zpackBuffer result;

		if( src && srcLen && compressLZMA_ )
		{
			result.size_ = srcLen + 30;

			unsigned char * dest = new unsigned char[ result.size_ ];

			dest[0] = 0;

			(*compressLZMA_)( src, srcLen, dest, result.size_, level );

			result.data_ = dest;
		}

		return result;
	}

	zpackBuffer zpack::uncompress( unsigned char const * src, unsigned int srcLen )
	{
		init();

		zpackBuffer result;

		if( src && 10 < srcLen && unCompressLZMA_ )
		{
			result.size_ = *(unsigned int*)(src+6);

			unsigned char * dest = new unsigned char[result.size_+1];

			dest[ result.size_ ] = 0;

			(*unCompressLZMA_)( src, srcLen, dest, result.size_ );

			result.data_ = dest;
		}

		return result;
	}

	void zpack::init()
	{
		if( dll_ )
			return;

		// DLL 로드
		dll_ = LoadLibraryA( decrypt( L"ﾝﾕﾗﾆﾬﾚﾚﾒ￘" ).c_str() );

		#define getDllProc( name, encName ) (name)GetProcAddress( dll_, decrypt( encName ).c_str() )

		if( dll_ )
		{
			find_			= getDllProc(ZpackFind, L"ﾝﾕﾗﾆﾬﾒﾘﾟﾰ");
			del_			= getDllProc(ZpackDelete, L"ﾰﾟﾗﾕﾄﾮﾑﾀﾑﾘﾑ" );
			add_			= getDllProc(ZpackAdd, L"铿雿蟿귿鏿鏿뛿鳿");
			extract_		= getDllProc(ZpackExtract, L"뛿飿郿鋿菿꧿蟿郿鋿臿蟿诿" );
			release_		= getDllProc(ZpackRelease, L"ꇿ飿郿鋿菿꧿雿胿鋿雿鿿雿" );
			folderInfo_		= getDllProc(ZpackFolderInfo, L"ﾜﾟﾶﾛﾓﾑﾀﾪﾟﾖﾞﾹﾂﾕﾔ" );
			clearCache_		= getDllProc(ZpackClearCache, L"ﾕﾜﾳﾛﾓﾑﾀﾪﾕﾘﾓﾑﾳﾂﾑ" );
			//compressLZMA_	= getDllProc(CompressLZMA);
			//unCompressLZMA_	= getDllProc(UnCompressLZMA);
		}

		#undef getDllProc
	}

	wchar_t const * zpack::getPackName( std::string path )
	{
		size_t idx1 = path.find( "\\" );
		size_t idx2 = path.find( "/" );

		idx1 = idx1 < idx2 ? idx1 : idx2;

		std::string packName = path.substr( 0, idx1 );

		if( packName.empty() || (packName.find( "." ) != std::string::npos) )
			return 0;

		if( packName[0] == 't' || packName[0] == 'T' )
		{
			for( size_t i=0; i<packName.length(); ++i )
				packName[i] = std::tolower( packName[i] );
		}

		if( packName == "texture" )
		{
			idx1 = path.rfind( "\\" );
			idx2 = path.rfind( "/" );
			idx1 = idx1 < idx2 ? idx1 : idx2;

			if( idx1 != std::string::npos && idx1 > packName.length() )
			{
				std::string subPackName = path.substr( packName.length()+1, idx1 - (packName.length()+1) );

				idx1 = subPackName.find( "\\" );
				idx2 = subPackName.find( "/" );
				idx1 = idx1 < idx2 ? idx1 : idx2;

				subPackName = subPackName.substr( 0, idx1 );

				if( subPackName[0] == 'c' || subPackName[0] == 'C' )
				{
					for( size_t i=0; i<subPackName.length(); ++i )
						subPackName[i] = std::tolower( subPackName[i] );

					if( subPackName == "character" )
					{
						if( *path.rbegin() == 'p' || *path.rbegin() == 'P' )
							subPackName += "1";
						else
							subPackName += "2";
					}
				}

				packName = std::string("t") + subPackName;
			}
		}

		static std::wstring wpackName;

		wpackName = lexical_cast< std::wstring >( packName ) + L".zp";

		std::transform( wpackName.begin(), wpackName.end(), wpackName.begin(), std::towlower );

		return wpackName.c_str();
	}

	std::wstring zpack::getPassword()
	{
		std::wstring result;
		#ifdef _TIW
		static std::wstring pw = L"ﾘﾃﾋﾆﾈﾃﾔﾅﾊﾝﾅﾏﾁﾋﾮﾜﾁ";
		result = pw;
		#else
		static std::wstring pw = L"";
		#endif

		decrypt( (wchar_t*)result.c_str(), result.length() );

		return result;
	}

	void zpack::decrypt( wchar_t * src, size_t len )
	{
		if( !src || len < 1 )
			return;

		wchar_t * str = (wchar_t*)src;

		wchar_t l = (wchar_t)len;

		char * tmp = (char*)str;
		std::reverse( tmp, tmp+len );
		std::reverse( tmp+len, tmp+len*2 );

		for( size_t i=0; i<len; ++i )
		{
			str[i] ^= l;
			str[i] = ~str[i];
		}
	}

	std::string zpack::decrypt( wchar_t const * s )
	{
		if( !s ) return "";

		size_t len = wcslen(s);

		wchar_t * src = new wchar_t[len+1];

		src[ len ] = 0;

		memcpy_s( (char*)src, (len)*2, s, len * 2 );

		decrypt(src, len);

		char result[1024] = {0,};

		WideCharToMultiByte( CP_ACP, 0, src, -1, result, sizeof(result), 0, 0 );

		return result;
	}

	//--------------------------------------------------
//}

#endif