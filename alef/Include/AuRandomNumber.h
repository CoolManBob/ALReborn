/***************************** RANDOMA.H ********************** 2004-03-31 AF *
*
* This file contains function prototypes for several random number generators.
*
* These functions are coded in assembly language for improved speed and 
* contained in the libraries RANDOMAO.LIB, RANDOMAC.LIB, RANDOMAE.A.
*
* The functions are compatible with C++ and many other programming languages
* under Windows, Linux, BSD, UNIX and other 32-bit operating systems running
* on Intel 80x86 and compatible microprocessors, such as the Pentium family
* of microprocessors and corresponding microprocessors from AMD. 
* For computers that are not compatible with these, you have to use the
* C++ version of the functions.
*
* The theory of these random number generators is explained at
* www.agner.org/random
*
*
* Overview of random number generators in this library:
* =====================================================
*
* TRandom uses the Mersenne Twister type MT11213A.
* The resolution is 32 bits.
*
* MRandom uses the Mother-of-All algorithm.
* The resolution is 32 bits.
*
* WRandom uses the RANROT type W algorithm with self-test.
* The resolution is 63 bits for WRandom and 64 bits for WIRandom.
*
* XRandom combines the RANROT type W and the Mother-of-All generators.
* The resolution is 63 bits for WRandom and 64 bits for WIRandom.
*
*
* Function descriptions:
* ======================
*
* All these generators are used in the same way: They must be initialized
* with a random seed before the first call to any of the random functions.
* You may use the time in seconds or milliseconds as seed.
*
* Call the appropriate version of _RandomInit with your seed before gene-
* rating any random number, where _ is the corresponding prefix letter.
* The Mersenne Twister has an additional alternative initialization function
* TRandomInitByArray which you can use for longer seeds.
*
* All the generators can generate three different kinds of random numbers:
*
* _Random gives a uniformly distributed floating point number in the 
* interval 0 <= x < 1.
*
* _IRandom gives a uniformly distributed integer in the closed interval
* defined by min and max:  min <= x <= max.
*
* _BRandom gives 32 random bits.
*
*
* Further documentation:
* ======================
* See the file randoma.htm for further documentation.
*
* ?2001, 2004 Agner Fog. All code in this library is published under the
* GNU General Public License: www.gnu.org/copyleft/gpl.html
*******************************************************************************/

#ifndef RANDOMA_H
#define RANDOMA_H

#include <time.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment (lib, "AuRandomNumberGeneratorD")
#else
#pragma comment (lib, "AuRandomNumberGenerator")
#endif
#endif

#ifndef _M_X64
	#pragma comment (lib, "randomam")
#endif

// Mersenne Twister
extern "C" void     TRandomInit (int seed);
extern "C" void     TRandomInitByArray (unsigned long int seeds[], int length);
extern "C" double   TRandom (void);
extern "C" double   TRandom2 (void);
extern "C" long double TRandom3 (void);
extern "C" int      TIRandom (int min, int max);
extern "C" unsigned TBRandom ();

// Mother-of-all generator
extern "C" void     MRandomInit (int seed);
extern "C" double   MRandom (void);
extern "C" int      MIRandom (int min, int max);
extern "C" unsigned MBRandom (void);

// RANROT type W generator                                                                              
extern "C" void     WRandomInit (int seed);
extern "C" double   WRandom  (void);
extern "C" int      WIRandom (int min, int max);
extern "C" unsigned WBRandom (void);

// Combined generator
extern "C" void     XRandomInit (int seed);
extern "C" double   XRandom (void);
extern "C" int      XIRandom (int min, int max);
extern "C" unsigned XBRandom ();

class TRandomMotRot {                  // encapsulate random number generator from assembly library
  public:
  TRandomMotRot() { TRandomMotRot((unsigned long)time(0)); }
  TRandomMotRot(unsigned long seed);   // constructor
  int IRandom(int min, int max) {      // get integer random number in desired interval
    return XIRandom(min, max);}        // inline call to assembly function  
  double Random() {                    // get floating point random number
    return XRandom();}                 // inline call to assembly function  
  };

class MTRand {
private:
	TRandomMotRot	m_csRandom;

public:
	MTRand();
	~MTRand();

	double	rand();
	double	rand(double max);
	int		randInt(int max);
	int		randInt();
};

#endif

