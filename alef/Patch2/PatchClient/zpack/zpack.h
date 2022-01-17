#pragma once

#ifdef USE_ZPACK

#include <winsock2.h>
#include <string>
#include <algorithm>
#include "lexical_cast.h"

//namespace umtl
//{
	//--------------------------------------------------
	//

	typedef unsigned char *	(*ZpackFind)			( wchar_t const * pack_file, wchar_t const * file_name, unsigned int & outSizeProcessed, wchar_t const * password );
	typedef bool			(*ZpackDelete)			( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * password );
	typedef bool			(*ZpackAdd)				( wchar_t const * pack_file, wchar_t const * file_name, int level, wchar_t const * password );
	typedef bool			(*ZpackExtract)			( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * outDir, bool fullPath, wchar_t const * password );
	typedef void			(*ZpackRelease)			( wchar_t const * pack_file, wchar_t const * buffer );
	typedef void			(*ZpackSetAllocor)		( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );
	typedef void			(*ZpackSetTmpAllocor)	( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );
	typedef wchar_t const *	(*ZpackFolderInfo)		( wchar_t const * pack_file, wchar_t const * path, wchar_t const * password );
	typedef void			(*ZpackWorkDir)			( wchar_t const * pack_file, wchar_t const * workDir );
	typedef void			(*ZpackClearCache)		( wchar_t const * pack_file );

	typedef bool			(*CompressLZMA)			( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen, int level );
	typedef bool			(*UnCompressLZMA)		( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen );

	//--------------------------------------------------
	//

	class zpackBuffer
	{
	public:
		unsigned char const *	data_;
		unsigned int			size_;

		zpackBuffer();
		zpackBuffer( zpackBuffer & other );
		zpackBuffer(wchar_t const * pack_name, ZpackRelease release);
		~zpackBuffer();
		zpackBuffer & operator=( zpackBuffer & other );

	private:
		wchar_t const * pack_name_;
		ZpackRelease	release_;

		void clear();
		void release();
	};

	//--------------------------------------------------
	//

	class zpack
	{
	public:	

		static zpackBuffer find( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * password );

		static bool del( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * password );

		static bool add( wchar_t const * pack_name, wchar_t const * filename, int level, wchar_t const * password );

		static bool extract( wchar_t const * pack_name, wchar_t const * filename, wchar_t const * outDir, bool fullPath, wchar_t const * password );

		static void setAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );

		static void setTmpAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );

		static wchar_t const * folderInfo( wchar_t const * pack_file, wchar_t const * path, wchar_t const * password );

		static void setWorkDir( wchar_t const * pack_file, wchar_t const * workDir );

		static void clearCache( wchar_t const * pack_file );

		static zpackBuffer compress( unsigned char const * src, unsigned int srcLen, int level = 5 );

		static zpackBuffer uncompress( unsigned char const * src, unsigned int srcLen );

		static wchar_t const * getPackName( std::string path );

		static std::wstring getPassword();

	private:
		static HINSTANCE			dll_;
		static ZpackFind			find_;
		static ZpackDelete			del_;
		static ZpackAdd				add_;
		static ZpackExtract			extract_;
		static ZpackRelease			release_;
		static ZpackSetAllocor		setAllocor_;
		static ZpackSetTmpAllocor	setTmpAllocor_;
		static ZpackFolderInfo		folderInfo_;
		static ZpackWorkDir			workDir_;
		static ZpackClearCache		clearCache_;
		static CompressLZMA			compressLZMA_;
		static UnCompressLZMA		unCompressLZMA_;

		static void init();

		static inline void encrypt( std::wstring & src )
		{
			if( src.empty() )
				return;

			wchar_t * str = (wchar_t*)src.c_str();

			wchar_t len = (wchar_t)src.length();

			for( size_t i=0; i<src.length(); ++i )
			{
				str[i] = ~str[i];
				str[i] ^= len;
			}

			char * tmp = (char*)str;
			std::reverse( tmp, tmp+src.length() );
			std::reverse( tmp+src.length(), tmp+src.length()*2 );
		}

		static void decrypt( wchar_t * src, size_t len );

		static std::string decrypt( wchar_t const * s );
	};

	//--------------------------------------------------
//}

#endif