#ifndef  __WBANetwork_Stream_H
#define  __WBANetwork_Stream_H


#define  STREAM_DEFAULT_SIZE		256

namespace WBANetwork
{
	class Stream
	{
	public:
				Stream( int size = STREAM_DEFAULT_SIZE );
				Stream( Stream& stream );
				Stream( PBYTE buffer, int len );
				~Stream();


				unsigned long		GetDataSize()		{	return m_dataSize;		}
				unsigned long		GetBufferSize()		{	return m_bufferSize;	}
				LPBYTE				GetBuffer()			{	return m_buffer;		}

				void				Clear( int resize = 0 );

				bool				Write( void* source, int size );
				bool				Read( void* dest, int size );


		template	< class _T >	Stream& operator << ( _T arg )
		{
			Write( &arg, sizeof( _T ) );

			return *this;
		}

		template	< class _T >	Stream& operator >> ( _T& arg )
		{
			Read( &arg, sizeof( _T ) );

			return *this;
		}

				Stream& operator =  ( Stream& stream );
				Stream& operator << ( Stream& stream );

//				Stream&	operator << ( char* arg );
//				Stream&	operator << ( const char* arg );
//				Stream&	operator << ( unsigned char* arg );
//				Stream&	operator << ( const unsigned char* arg );

//				Stream&	operator >> ( char* arg );
//				Stream&	operator >> ( const char* arg );
//				Stream&	operator >> ( unsigned char* arg );
//				Stream&	operator >> ( const unsigned char* arg );

	private:
				BYTE			m_defaultBuffer[STREAM_DEFAULT_SIZE];
				LPBYTE			m_buffer, m_readPos, m_endPos;
				unsigned long	m_dataSize, m_bufferSize;
	};
}

#endif