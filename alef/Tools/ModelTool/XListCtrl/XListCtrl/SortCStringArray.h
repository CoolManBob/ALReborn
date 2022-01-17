// SortCStringArray.h
//
// Based on the article "QSort for CStringArray" by Chris Losinger -
//    http://www.codeguru.com/cpp/cpp/cpp_mfc/collections/article.php/c809/
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SORTCSTRINGARRAY_H
#define SORTCSTRINGARRAY_H


class CSortCStringArray
{
private:
	CSortCStringArray() {};
	~CSortCStringArray() {};

public:
	static void SortCStringArray(CStringArray * psa, 
								 BOOL bCaseSensitive = FALSE, 
								 BOOL bAscending = TRUE)
	{
		int nArraySize = psa->GetSize();
		if (nArraySize <= 0)
			return;

		int nCStringSize = sizeof (CString*);
		void* pArrayStart = (void *)&(*psa)[0];

		if (bCaseSensitive)
		{
			if (bAscending)
				qsort(pArrayStart, nArraySize, nCStringSize, CompareAscending);
			else
				qsort(pArrayStart, nArraySize, nCStringSize, CompareDescending);
		}
		else
		{
			if (bAscending)
				qsort(pArrayStart, nArraySize, nCStringSize, CompareNCAscending);
			else
				qsort(pArrayStart, nArraySize, nCStringSize, CompareNCDescending);
		}
	}

private:
	static int CompareAscending(const void *a, const void *b)
	{
		CString *pA = (CString*)a;
		CString *pB = (CString*)b;
		return (pA->Compare(*pB));
	}

	static int CompareDescending(const void *a, const void *b)
	{
		CString *pA = (CString*)a;
		CString *pB = (CString*)b;
		return (-1 * (pA->Compare(*pB)));
	}

	static int CompareNCAscending(const void *a, const void *b)
	{
		CString *pA = (CString*)a;
		CString *pB = (CString*)b;
		return (pA->CompareNoCase(*pB));
	}

	static int CompareNCDescending(const void *a, const void *b)
	{
		CString *pA = (CString*)a;
		CString *pB = (CString*)b;
		return (-1 * (pA->CompareNoCase(*pB)));
	}
};

#endif //SORTCSTRINGARRAY_H
