#ifndef __APCRTHEADER_H__
#define __APCRTHEADER_H__


#define nNoMansLandSize 4
const unsigned char _bNoMansLandFill = 0xFD;
#define CRT_GUARD_BYTES 0xFDFDFDFD

struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader * pBlockHeaderNext;
	struct _CrtMemBlockHeader * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
	size_t                      nDataSize;
	int                         nBlockUse;
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	 *  unsigned char           data[nDataSize];
	 *  unsigned char           anotherGap[nNoMansLandSize];
	 */
};

#define pbData(pblock) ((unsigned char *)((_CrtMemBlockHeader *)pblock + 1))
#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)

#endif	// __APCRTHEADER_H__