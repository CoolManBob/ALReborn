#pragma once

template< typename T >
class SingleTon
{
public:
	SingleTon(void)
	{
		ASSERT( !ms_pSingle );

		ms_pSingle   = static_cast< T* >( this );
	}
	virtual ~SingleTon(void)
	{
		ms_pSingle   = NULL;
	}

	static T& GetSingleTon()    
	{
		ASSERT( ms_pSingle );

		return (*ms_pSingle);
	}
	static T* GetSingleTonPtr() 
	{  
		ASSERT( ms_pSingle );

		return (ms_pSingle);
	}

protected:

	static T*    ms_pSingle;

};

template<typename T> T* SingleTon<T>::ms_pSingle = NULL;