#include "AuRandomNumber.h"

#ifndef _M_X64

TRandomMotRot::TRandomMotRot(unsigned long seed)
{ // constructor
  static int instance = 0;
  instance++;

  XRandomInit(seed);
}             // initialize

MTRand::MTRand()
{
	//m_csRandom.TRandomMotRot((unsigned long)time(NULL));
}

MTRand::~MTRand()
{
}

double MTRand::rand()
{
	return m_csRandom.Random();
}

double MTRand::rand(double max)
{
	return m_csRandom.Random() * max;
}

int MTRand::randInt(int max)
{
	return m_csRandom.IRandom(0, max);
}

int MTRand::randInt()
{
	return m_csRandom.IRandom(0, 2^31-1);
}

#else

#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

TRandomMotRot::TRandomMotRot(unsigned long seed)
{ // constructor
  static int instance = 0;
  instance++;
}             // initialize

MTRand::MTRand()
{
	//m_csRandom.TRandomMotRot((unsigned long)time(NULL));
}

MTRand::~MTRand()
{
}

double MTRand::rand()
{
	unsigned int number = 0;
	errno_t err = rand_s(&number);
	if(err == 0)
		return (double)number;
	else
	{
		::srand((unsigned int)time(NULL));
		return (double)::rand();
	}
}

double MTRand::rand(double max)
{
	unsigned int number = 0;
	errno_t err = rand_s(&number);
	if(err == 0)
	{
		return (double)number / (double)UINT_MAX * max;
	}
	else
	{
		::srand((unsigned int)time(NULL));
		number = (unsigned int)::rand();
		return (double)number / (double)RAND_MAX * max;
	}
}


// return range [0, max)
int MTRand::randInt(int max)
{
	unsigned int number = 0;
	errno_t err = rand_s(&number);
	if(err == 0)
	{
		return (int)((double)number / (double)UINT_MAX * (double)max);
	}
	else
	{
		::srand((unsigned int)time(NULL));
		number = (unsigned int)::rand();
		return (int)((double)number / (double)RAND_MAX * (double)max);
	}
}

int MTRand::randInt()
{
	unsigned int number = 0;
	errno_t err = rand_s(&number);
	if(err == 0)
	{
		return (int)((double)number / (double)UINT_MAX * (double)INT_MAX);
	}
	else
	{
		::srand((unsigned int)time(NULL));
		number = (unsigned int)::rand();
		return (int)((double)number / (double)RAND_MAX * (double)INT_MAX);
	}
}

#endif