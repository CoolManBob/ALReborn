// D3Wrapper.h: interface for the CD3Wrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_D3WRAPPER_H_)
#define _D3WRAPPER_H_

#pragma warning(disable:4244)
#include "sdkconfig.h"

#define D2_DES		/* include double-length support */
#define D3_DES		/* include triple-length support */
#define EN0	0		/* MODE == encrypt */
#define DE1	1		/* MODE == decrypt */

#define KEY_SIZE 24

#ifdef D3_DES

#ifndef D2_DES
#define D2_DES		/* D2_DES is needed for D3_DES */
#endif

#endif

static unsigned char Df_Key[24] = {
	0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
	0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
	0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67 };
	
static unsigned short bytebit[8] = {	
	0200, 0100, 040, 020, 010, 04, 02, 01 };
		
static unsigned long bigbyte[24] = {
	0x800000L,	0x400000L,	0x200000L,	0x100000L,	
	0x80000L,	0x40000L,	0x20000L,	0x10000L,				
	0x8000L,	0x4000L,	0x2000L,	0x1000L,				
	0x800L, 	0x400L, 	0x200L, 	0x100L,				
	0x80L,		0x40L,		0x20L,		0x10L,				
	0x8L,		0x4L,		0x2L,		0x1L	};			
			
/* Use the key schedule specified in the Standard (ANSI X3.92-1981). */	
static unsigned char pc1[56] = {				
	56, 48, 40, 32, 24, 16,  8,	 0, 57, 49, 41, 33, 25, 17,					
	9,  1, 58, 50, 42, 34, 26,	18, 10,  2, 59, 51, 43, 35,					
	62, 54, 46, 38, 30, 22, 14,	 6, 61, 53, 45, 37, 29, 21,					
	13,  5, 60, 52, 44, 36, 28,	20, 12,  4, 27, 19, 11,  3 };				
								
static unsigned char totrot[16] = {					
	1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28 };										
					
static unsigned char pc2[48] = {						
	13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,							
	22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1,							
	40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,							
	43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31 };

static unsigned long SP1[64] = {
	0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
	0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
	0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
	0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
	0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
	0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
	0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
	0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
	0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
	0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
	0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
	0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
	0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
	0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L };

static unsigned long SP2[64] = {
	0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
	0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
	0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
	0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
	0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
	0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
	0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
	0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
	0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
	0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
	0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
	0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
	0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
	0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
	0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
	0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L };

static unsigned long SP3[64] = {
	0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
	0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
	0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
	0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
	0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
	0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
	0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
	0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
	0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
	0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
	0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
	0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
	0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
	0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
	0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
	0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L };

static unsigned long SP4[64] = {
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
	0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
	0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
	0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
	0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
	0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
	0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
	0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
	0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L };

static unsigned long SP5[64] = {
	0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
	0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
	0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
	0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
	0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
	0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
	0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
	0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
	0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
	0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
	0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
	0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
	0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
	0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
	0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
	0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L };

static unsigned long SP6[64] = {
	0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
	0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
	0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
	0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
	0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
	0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
	0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
	0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
	0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
	0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
	0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
	0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
	0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
	0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L };

static unsigned long SP7[64] = {
	0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
	0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
	0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
	0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
	0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
	0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
	0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
	0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
	0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
	0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
	0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
	0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
	0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
	0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
	0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
	0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L };

static unsigned long SP8[64] = {
	0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
	0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
	0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
	0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
	0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
	0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
	0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
	0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
	0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
	0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
	0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
	0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
	0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
	0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
	0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
	0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L };
	
class PTSDK_STUFF CD3Wrapper  
{
public:
	// 根据一串字符产生Key
	void Make3Key(char * szDigest, unsigned char szKey[KEY_SIZE]);
	// 根据相应模式进行加密或解密
	int DoD3Des(unsigned char *pSrc, int nLenSrc, unsigned char * &pDst);
	// 设置密钥和加密解密模式, 0:加密,1:解密
	int SetDes3Key(unsigned char szKey[KEY_SIZE], short nMode);
	CD3Wrapper();
	virtual ~CD3Wrapper();
private:
	bool m_bKeySetted;
	int  m_nMode;
	void scrunch(unsigned char *, unsigned long *);
	void unscrun(unsigned long *, unsigned char *);
	void desfunc(unsigned long *, unsigned long *);
	void cookey(unsigned long *);
	
	unsigned long KnL[32];
	unsigned long KnR[32];
	unsigned long Kn3[32];
	
	/* A useful alias on 68000-ish machines, but NOT USED HERE. */
	typedef union {
		
		unsigned long blok[2];
		
		unsigned short word[4];
		
		unsigned char byte[8];
		
	} M68K;

	// cookedkey[32]
	// Copies the contents of the internal key register into the storage
	// located at &cookedkey[0].
	void cpkey(unsigned long *);

	// from[8]	      to[8]
	// Encrypts/Decrypts (according to the key currently loaded in the
	// internal key register) one block of eight bytes at address 'from'
	// into the block at address 'to'.  They can be the same.
	void des(unsigned char *, unsigned char *);

	// hexkey[8]     MODE
	// Sets the internal key register according to the hexadecimal
	// key contained in the 8 bytes of hexkey, according to the DES,
	// for encryption or decryption according to MODE.
    void deskey(unsigned char *, short);

	// cookedkey[32]
	// Loads the internal key register with the data in cookedkey.	
    void usekey(unsigned long *);

#ifdef D2_DES

#define desDkey(a,b)	des2key((a),(b))

	// hexkey[16]     MODE
	// Sets the internal key registerS according to the hexadecimal
	// keyS contained in the 16 bytes of hexkey, according to the DES,
	// for DOUBLE encryption or decryption according to MODE.
	// NOTE: this clobbers all three key registers!
	void des2key(unsigned char *, short);

	// from[8]	      to[8]
	// Encrypts/Decrypts (according to the keyS currently loaded in the
	// internal key registerS) one block of eight bytes at address 'from'
	// into the block at address 'to'.  They can be the same.
	void Ddes(unsigned char *, unsigned char *);

	// from[16]	      to[16]
	// Encrypts/Decrypts (according to the keyS currently loaded in the
	// internal key registerS) one block of SIXTEEN bytes at address 'from'
	// into the block at address 'to'.  They can be the same.
	void D2des(unsigned char *, unsigned char *);

	// password,	single-length key[8]
	// With a double-length default key, this routine hashes a NULL-terminated
	// string into an eight-byte random-looking key, suitable for use with the
	// deskey() routine.
	void makekey(char *, unsigned char *);

#define makeDkey(a,b)	make2key((a),(b))

	// password,	double-length key[16]
	// With a double-length default key, this routine hashes a NULL-terminated
	// string into a sixteen-byte random-looking key, suitable for use with the
	// des2key() routine.
	void make2key(char *, unsigned char *);

#ifndef D3_DES	/* D2_DES only */

#define useDkey(a)	use2key((a))
#define cpDkey(a)	cp2key((a))

	// cookedkey[64]
	// Loads the internal key registerS with the data in cookedkey.
	// NOTE: this clobbers all three key registers!
	void use2key(unsigned long *);

	// cookedkey[64]
	// Copies the contents of the internal key registerS into the storage
	// located at &cookedkey[0].
	void cp2key(unsigned long *);

#else	/* D3_DES too */

#define useDkey(a)	use3key((a))
#define cpDkey(a)	cp3key((a))

	// cookedkey[96]
	// Loads the 3 internal key registerS with the data in cookedkey.
	void use3key(unsigned long *);

	// cookedkey[96]
	// Copies the contents of the 3 internal key registerS into the storage
	// located at &cookedkey[0].
	void cp3key(unsigned long *);
	
	// hexkey[24]     MODE
	// Sets the internal key registerS according to the hexadecimal
	// keyS contained in the 24 bytes of hexkey, according to the DES,
	// for DOUBLE encryption or decryption according to MODE.
    void des3key(unsigned char *, short);
	
	// password,	triple-length key[24]
	// With a triple-length default key, this routine hashes a NULL-terminated
	// string into a twenty-four-byte random-looking key, suitable for use with
	// the des3key() routine.
	void make3key(char *, unsigned char *);

    void D3des(unsigned char *, unsigned char *);
	
#endif	/* D3_DES */

#endif	/* D2_DES */

};

#endif // !defined(_D3WRAPPER_H_)
