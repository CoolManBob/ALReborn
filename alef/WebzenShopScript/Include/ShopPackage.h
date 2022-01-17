
/**************************************************************************************************

패키지 객체

하나의 패키지 정보를 가지고 있다.
현재 패키지에 등록 되어있는 "상품 번호" 목록과 "가격 번호" 목록을 가지고 있다.

패키지에 여러 상품이 들어있는 경우에는 가격 번호가 한 개 이다.
패키지에 여러 가격이 설정되어 있다면 상품은 한 개 이다.

(exe 1)
A 패키지에 a와 a' 라는 상품이 두 개 있다면 여러 가격이 설정 될 수 없다. 
-> A상품 a, a' 5000원

(exe 2)
B 패키지에 b 라는 상품이 하나만 있다면 1000원, 2000원, 3000원 으로 여러 가격이 설정 될 수 있다. 
-> B상품 b 1주일 1000원
-> B상품 b 2주일 2000원
-> B상품 b 4주일 3000원
세 가지로 판매가 가능하도록 보여주어야 한다.

**************************************************************************************************/

#pragma once

#include "include.h"
#include <time.h>

class CShopPackage
{
public:
	CShopPackage();
	virtual ~CShopPackage();

	bool	SetPackage(tstring strdata);

	// Interface --------------------------------------------------
	int		GetProductCount();									// 패키지 내부의 상품 수 가져오기
	void	SetProductSeqFirst();								// 패키지 내부의 첫번째 상품 번호에 위치
	bool	GetProductSeqFirst(int& ProductSeq);				// 패키지 내부의 첫번째 상품 번호를 가져오고 다음 상품 번호로 이동
	bool	GetProductSeqNext(int& ProductSeq);					// 상품 번호 가져오고 다음 위치로 이동

	int		GetPriceCount();									// 패키지 내부의 가격 수 가져오기
	void	SetPriceSeqFirst();									// 패키지 내부의 첫번째 가격 번호에 위치
	bool	GetPriceSeqFirst(int& PriceSeq);					// 패키지 내부의 첫번째 가격 번호를 가져오고 다음 가격 번호로 이동
	bool	GetPriceSeqNext(int& PriceSeq);						// 가격 번호 가져오고 다음 위치로 이동									

	void	SetLeftCount(int nCount);
	// ------------------------------------------------------------

public:
	int		PackageProductSeq;									//  1. 패키지 번호
	TCHAR	PackageProductName[SHOPLIST_LENGTH_PACKAGENAME];	//  2. 패키지 명
	int		PackageProductType;									//  3. 패키지 유형 (170:일반 상품, 171:이벤트 상품)
	int		Price;												//  4. 가격
	TCHAR	Description[SHOPLIST_LENGTH_PACKAGEDESC];			//  5. 상세 설명
	TCHAR	Caution[SHOPLIST_LENGTH_PACKAGECAUTION];			//  6. 주의 사항
	int		SalesFlag;											//  7. 구매 가능 여부(구매버튼 노출여부) (182:가능, 183:불가)
	int		GiftFlag;											//  8. 선물 가능 여부(선물버튼 노출여부) (184:가능, 185:불가)
	tm		StartDate;											//  9. 판매 시작일
	tm		EndDate;											// 10. 판매 종료일
	int		CapsuleFlag;										// 11. 캡슐 상품 구분 (176:캡슐, 177:일반)
	int		CapsuleCount;										// 12. 패키지에 포함된 상품 개수
	TCHAR	ProductCashName[SHOPLIST_LENGTH_PACKAGECASHNAME];	// 13. 소진 캐시 명
	TCHAR	PricUnitName[SHOPLIST_LENGTH_PACKAGEPRICEUNIT];		// 14. 가격 단위 표시 명
	int		DeleteFlag;											// 15. 삭제 여부 (180:삭제, 181:활성)
	int		ProductAmount;										// 16. 한정 상품 여부	
	TCHAR	InGamePackageID[SHOPLIST_LENGTH_INGAMEPACKAGEID];	// 18. 패키지 아이템 코드
	int		ProductCashSeq;										// 19. 소진 캐시 유형 코드
	int		PriceCount;											// 20. 단위 상품 가격 정책 보유 개수 (단위 상품이 1개인 경우에만 PriceSeq가 여러 개일 수 있다.)
	bool	DeductMileageFlag;									// 22. 마일리지로 차감 상품 여부 (false : 일반, true : 마일리지 차감 상품)
	int		CashType;											// 23. 글로벌 전용 : Wcoin(C), WCoin(P) 구분
	int		CashTypeFlag;										// 24. 글로벌 전용 : Wcoin(C), WCoin(P) 선택 or 자동 여부(668: 개인선택, 669: 자동차감)
	int		ImageTagType;										// 25. 이미지태그 코드 값 (문서 공통 코드 정의 참고)

	int		LeftCount;											// 잔여 개수

private:
	void	SetProductSeqList(tstring strdata);
	void	SetPriceSeqList(tstring strdata);

	std::vector<int> ProductSeqList;							// 17. 패키지에 포함된 상품 번호 목록
	std::vector<int>::iterator ProductSeqIter;

	std::vector<int> PriceSeqList;								// 21. 패키지에 포함되는 가격 번호 목록 (상품이 1개인 경우에만 값을 제공.)
	std::vector<int>::iterator PriceSeqIter;
};
