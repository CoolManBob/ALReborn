#pragma once

#include <string.h>

#ifndef SHA1_DIGESTSIZE
#define SHA1_DIGESTSIZE  20
#endif

#ifndef SHA1_BLOCKSIZE
#define SHA1_BLOCKSIZE   64
#endif

typedef struct {
    unsigned long state[5];
    unsigned long count[2];	/* stores the number of bits */
    unsigned char buffer[SHA1_BLOCKSIZE];
} SHA1_CTX; 

#define LITTLE_ENDIAN /* should be defined if so */

/* Rotation of "value" by "bits" to the left */
#define rotLeft(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* Basic SHA1 functions */
#define f(u,v,w) (((u) & (v)) | ((~u) & (w)))
#define g(u,v,w) (((u) & (v)) | ((u) & (w)) | ((v) & (w)))
#define h(u,v,w) ((u) ^ (v) ^ (w))

/* These are the 16 4-byte words of the 64-byte block */
#ifdef LITTLE_ENDIAN
/* Reverse the order of the bytes in the i-th 4-byte word */
#define x(i) (block->l[i] = (rotLeft(block->l[i], 24)& 0xff00ff00) \
| (rotLeft(block->l[i], 8) & 0x00ff00ff))
#else
#define x(i) (block->l[i])
#endif

/* Used in expanding from a 16 word block into an 80 word block  */
#define X(i) (block->l[(i)%16] = rotLeft (block->l[((i)-3)%16] ^ block->l[((i)-8)%16] \
^ block->l[((i)-14)%16] ^ block->l[((i)-16)%16],1))

/* (R0+R1), R2, R3, R4 are the different round operations used in SHA1 */
#define R0(a, b, c, d, e, i) { \
    (e) += f((b), (c), (d)) + (x(i)) + 0x5A827999 + rotLeft((a),5); \
    (b) = rotLeft((b), 30); \
}
#define R1(a, b, c, d, e, i)  { \
    (e) += f((b), (c), (d)) + (X(i)) + 0x5A827999 + rotLeft((a),5); \
    (b) = rotLeft((b), 30); \
}
#define R2(a, b, c, d, e, i) { \
    (e) += h((b), (c), (d)) + (X(i)) + 0x6ED9EBA1 + rotLeft((a),5); \
    (b) = rotLeft((b), 30); \
}
#define R3(a, b, c, d, e, i)  { \
    (e) += g((b), (c), (d)) + (X(i)) + 0x8F1BBCDC + rotLeft((a),5); \
    (b) = rotLeft((b), 30); \
}
#define R4(a, b, c, d, e, i)  { \
    (e) += h((b), (c), (d)) + (X(i)) + 0xCA62C1D6 + rotLeft((a),5); \
    (b) = rotLeft((b), 30); \
}

class CHashEncrypt
{
public:
	CHashEncrypt(){}
	~CHashEncrypt(){}

	BOOL HashEncrypt(char buffer[SHA1_DIGESTSIZE]);

private:
	BOOL SHA1Transform(unsigned long state[5], const unsigned char buffer[SHA1_BLOCKSIZE]);
	BOOL SHA1Init(SHA1_CTX *context);
	BOOL SHA1Update(SHA1_CTX *context, const unsigned char *data, unsigned long len);
	BOOL SHA1Final(unsigned char digest[SHA1_DIGESTSIZE], SHA1_CTX *context);
};
