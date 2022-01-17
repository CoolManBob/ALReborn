
/**************************************************************************************************

카테고리 객체

하나의 카테고리 정보를 가지고 있다.
카테고리 하위의 "카테고리 번호"를 목록으로 가지고 있다.
현재 카테고리가 최 하위 카테고리라면 "패키지 번호" 목록을 가지고 있다.

**************************************************************************************************/

#pragma once

#include "include.h"

class CShopCategory
{
public:
	CShopCategory();
	virtual ~CShopCategory();
	
	bool SetCategory(tstring strData);

	void ClearChildCategorySeqList();
	void ClearChildPackageSeqList();

	void AddChildCategorySeqList(int CategorySeq);
	void AddChildPackageSeqList(int PackageSeq);

	// Interface --------------------------------------------------
	void SetChildCategorySeqFirst();					// 하위 카테고리 목록의 첫 번째 항록을 가리키도록 설정한다.
	bool GetChildCategorySeqNext(int& CategorySeq);		// 하위 카테고리 번호를 리턴하고 다음 하위 카테고리 번호를 가리킨다.

	void SetChildPackagSeqFirst();						// 카테고리에 등록되어 있는 패키지 목록의 첫 번째 항목을 가리키도록 설정한다.
	bool GetChildPackagSeqNext(int& PackagSeq);			// 패키지 번호를 리턴하고 다음 패키지 번호를 가리킨다.
	// ------------------------------------------------------------

public:
	int ProductDisplaySeq;								// 1. 카테고리 번호
	TCHAR CategroyName[SHOPLIST_LENGTH_CATEGORYNAME];	// 2. 카테고리 이름
	int EventFlag;										// 3. 이벤트 카테고리 여부 (199:이벤트, 200:일반)	
	int OpenFlag;										// 4. 공개 여부 (201:공개, 202: 비공개)
	int ParentProductDisplaySeq;						// 5. 부모 카테고리 번호
	int DisplayOrder;									// 6. 노출 순서
	int Root;											// 7. 최상위 카테고리 구분 (1: 최상위, 0: 서브)

private:
	std::vector<int> ChildCategorySeqList;				// 현재 카테고리의 하위 "카테고리 번호" 목록
	std::vector<int>::iterator ChildCategorySeqIter;

	std::vector<int> ChildPackageSeqList;				// 현재 카테고리에 포함된 "패키지 번호" 목록
	std::vector<int>::iterator ChildPackageSeqIter;
};
