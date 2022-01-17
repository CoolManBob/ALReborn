#pragma once
#include "sdkconfig.h"
#include "Lock\Lock.h"
using namespace CleverLock;

class PTSDK_STUFF RefCount
{
private:
	int crefs;
	CWithLock Lock;
public:
	RefCount(void)
	{ 
		crefs = 0;
	}
	virtual ~RefCount() {}
	void upcount(void) 
	{ 
		LOCK(Lock);
		++crefs;
	}
	void downcount(void)
	{
		{		
			LOCK(Lock);
			if (--crefs != 0)
				return;
		}
		delete this;
	}

	void myFree(void)
	{
		delete this;
	}
};

template <class T> class SmartPtr 
{
private:
	T* p;
public:
	SmartPtr(T* p_) 
	{
		p=p_;
		if(p!=NULL)
			p->upcount(); 
	}

	SmartPtr(SmartPtr<T> & p_):p(NULL)
	{
		operator=((T *) p_); 
	}

	SmartPtr(): p(NULL){}

	~SmartPtr(void) 
	{ 
		if (p) p->downcount(); 
	}

	operator T*(void)	{ return p; }
	T& operator*(void)	{ return *p;}
	T* operator->(void) { return p; }

	SmartPtr& operator=(SmartPtr<T> &p_) 
	{
		return operator=((T *) p_);
	}

	SmartPtr& operator=(T* p_) 
	{
		if (p_)
			p_->upcount(); 

		if (p)
			p->downcount();

		p = p_; 		

		return *this;
	}

	void myFree()
	{
		//p->myFree();
	}
};
