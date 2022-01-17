/*============================================================

	AgpdMailBox.h
	
============================================================*/


#ifndef _AGPD_MAILBOX_H_
	#define _AGPD_MAILBOX_H_

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include <time.h>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGPMMAILBOX_MAX_MAILS_IN_EFFECT				30
#define AGPMMAILBOX_MAX_SUBJECT_LENGTH				50
#define AGPMMAILBOX_MAX_CONTENT_LENGTH				1000


/************************************************/
/*		The Definition of AgpdMail class		*/
/************************************************/
//
//	==========	Mail element	==========
//
class AgpdMail
	{
	public :
		enum eAGPMMAILBOX_MAIL_FLAG
			{
			AGPMMAILBOX_MAIL_FLAG_NONE			= 0,
			AGPMMAILBOX_MAIL_FLAG_READ			= (1 << 0),
			AGPMMAILBOX_MAIL_FLAG_READ_ATTACH	= (1 << 4),
			AGPMMAILBOX_MAIL_FLAG_DELETE		= (1 << 8),			// server only
			AGPMMAILBOX_MAIL_FLAG_SIZE_OVER		= (1 << 12),		// server only
			};

	public:
		INT32			m_lID;
		UINT64			m_ullDBID;
		TCHAR			m_szFromCharID[AGPDCHARACTER_NAME_LENGTH + 1];
		TCHAR			m_szToCharID[AGPDCHARACTER_NAME_LENGTH + 1];
		TCHAR			m_szSubject[AGPMMAILBOX_MAX_SUBJECT_LENGTH + 1];
		time_t			m_lDate;
		UINT32			m_ulFlag;
		UINT64			m_ullItemSeq;
		INT32			m_lItemTID;
		INT16			m_nItemQty;
		TCHAR			*m_pszContent;

	private:
		INT16			m_nContentSize;

	private:
		void	_SetFlag(eAGPMMAILBOX_MAIL_FLAG eFlag, BOOL bSet)
			{
			if (bSet)
				m_ulFlag |= eFlag;
			else
				m_ulFlag &= ~eFlag;
			}

		BOOL	_GetFlag(eAGPMMAILBOX_MAIL_FLAG eFlag)
			{
			return (m_ulFlag & eFlag);
			}	
	
	public:
		AgpdMail()
			{
			Init();
			}

		~AgpdMail()
			{
			ResetContent();
			}
		
		void	Init()
			{
			m_lID = 0;
			m_ullDBID = 0;
			ZeroMemory(m_szFromCharID, sizeof(m_szFromCharID));
			ZeroMemory(m_szToCharID, sizeof(m_szToCharID));
			ZeroMemory(m_szSubject, sizeof(m_szSubject));
			m_lDate = 0;
			m_ulFlag = AGPMMAILBOX_MAIL_FLAG_NONE;
			m_ullItemSeq = 0;
			m_lItemTID = 0;
			m_nItemQty = 0;
			m_pszContent = NULL;
			m_nContentSize = 0;
			}
				
		void	SetRead(BOOL bSet)			{ _SetFlag(AGPMMAILBOX_MAIL_FLAG_READ, bSet); }
		void	SetReadAttach(BOOL bSet)	{ _SetFlag(AGPMMAILBOX_MAIL_FLAG_READ_ATTACH, bSet); }
		void	SetDelete(BOOL bSet)		{ _SetFlag(AGPMMAILBOX_MAIL_FLAG_DELETE, bSet); }
		void	SetSizeOver(BOOL bSet)		{ _SetFlag(AGPMMAILBOX_MAIL_FLAG_SIZE_OVER, bSet); }
		
		BOOL	GetRead()					{ return _GetFlag(AGPMMAILBOX_MAIL_FLAG_READ); }
		BOOL	GetReadAttach()				{ return _GetFlag(AGPMMAILBOX_MAIL_FLAG_READ_ATTACH); }
		BOOL	GetDelete()					{ return _GetFlag(AGPMMAILBOX_MAIL_FLAG_DELETE); }
		BOOL	GetSizeOver()				{ return _GetFlag(AGPMMAILBOX_MAIL_FLAG_SIZE_OVER); }
		
		TCHAR*	GetContent()
			{
			return m_pszContent;
			}
		INT16	GetContentSize()
			{
			return m_nContentSize;
			}
		BOOL	SetContent(TCHAR *pszContent)
			{
			if (NULL == pszContent)
				return FALSE;
			
			INT32 lLen = (INT32)_tcslen(pszContent) + 1;
			if (0 >= lLen
				|| lLen > AGPMMAILBOX_MAX_CONTENT_LENGTH
				)
				return FALSE;
			
			ResetContent();
			
			m_nContentSize = (INT16) (sizeof(TCHAR) * lLen);
			
			m_pszContent = new TCHAR[lLen];
			if (NULL == m_pszContent)
				return FALSE;
			ZeroMemory(m_pszContent, sizeof(TCHAR) * lLen);
			_tcscpy(m_pszContent, pszContent);
			
			return TRUE;
			}
		BOOL	SetContent(PVOID pvContent, INT16 nContentSize)
			{
			if (NULL == pvContent
				|| 0 >= nContentSize
				|| nContentSize > AGPMMAILBOX_MAX_CONTENT_LENGTH
				)
				return FALSE;
			
			ResetContent();
			
			m_nContentSize = nContentSize;
			m_pszContent = (TCHAR *) new BYTE[nContentSize];
			if (NULL == m_pszContent)
				return FALSE;			
			CopyMemory(m_pszContent, pvContent, nContentSize);
			
			return TRUE;
			}
		void	ResetContent()
			{
			if (m_pszContent)
				{
				delete [] m_pszContent;
				m_pszContent = NULL;
				}

			m_nContentSize = 0;
			}
		
		AgpdMail& operator = (const AgpdMail& cpy)
			{
			m_lID = cpy.m_lID;
			m_ullDBID = cpy.m_ullDBID;
			_tcscpy(m_szFromCharID, cpy.m_szFromCharID);
			_tcscpy(m_szToCharID, cpy.m_szToCharID);
			_tcscpy(m_szSubject, cpy.m_szSubject);
			m_lDate = cpy.m_lDate;
			m_ulFlag = cpy.m_ulFlag;
			m_ullItemSeq = cpy.m_ullItemSeq;
			m_lItemTID = cpy.m_lItemTID;
			m_nItemQty = cpy.m_nItemQty;
			SetContent(cpy.m_pszContent);
			
			return *this;
			}
	};
	
	
//
//	==========	Character attached data	==========
//	
class AgpdMailCAD : public ApBase
	{
	private:
		INT16		m_nCount;
		ApSafeArray<INT32, AGPMMAILBOX_MAX_MAILS_IN_EFFECT>	m_alMail;
	
	public:
		BOOL		m_bCheckResult;
	
	public:
		AgpdMailCAD()	{}
		~AgpdMailCAD()	{}
		
		void	Init()
			{
			m_nCount = 0;
			m_alMail.MemSetAll();
			m_bCheckResult = FALSE;
			}
					
		INT16	GetCount()
			{
			return m_nCount;
			}
		
		BOOL	AddHead(INT32 lID)
			{
			BOOL bResult = FALSE;
			if (m_nCount >= AGPMMAILBOX_MAX_MAILS_IN_EFFECT)
				return bResult;
			
			INT32 l = 0;
			for (l = 0; l < AGPMMAILBOX_MAX_MAILS_IN_EFFECT - 1; l++)
				{
				if (0 == m_alMail[l])
					break;
				}
			
			for (INT32 k = l; k > 0; k--)
				{
				m_alMail[k] = m_alMail[k-1];
				}
			
			m_alMail[0] = lID;
			m_nCount++;
			bResult = TRUE;

			return bResult;
			}
			
		BOOL	AddTail(INT32 lID)
			{
			BOOL bResult = FALSE;
			if (m_nCount >= AGPMMAILBOX_MAX_MAILS_IN_EFFECT)
				return bResult;
			
			for (INT32 l = 0; l < AGPMMAILBOX_MAX_MAILS_IN_EFFECT; l++)
				{
				if (0 == m_alMail[l])
					{
					m_alMail[l] = lID;
					m_nCount++;
					bResult = TRUE;
					break;
					}
				}

			return bResult;
			}
			
		BOOL	Remove(INT32 lID)
			{
			BOOL bResult = FALSE;
			for (INT32 l=0; l < AGPMMAILBOX_MAX_MAILS_IN_EFFECT; l++)
				{
				if (lID == m_alMail[l])
					{
					for (INT32 m = l; m < AGPMMAILBOX_MAX_MAILS_IN_EFFECT - 1; m++)
						{
						m_alMail[m] = m_alMail[m + 1];
						}

					m_alMail[AGPMMAILBOX_MAX_MAILS_IN_EFFECT - 1] = 0;

					m_nCount--;
					bResult = TRUE;
					break;
					}
				}

			return bResult;
			}
		
		INT32	RemoveTail()
			{
			INT32 lID = 0;
			INT32 lIndex = AGPMMAILBOX_MAX_MAILS_IN_EFFECT - 1;
			// full 일때만 쓰는 함수니 루프 빼자.
			//for (INT32 l=0; l < AGPMMAILBOX_MAX_MAILS_IN_EFFECT - 1; l++)
			//	{
			//	if (0 == m_alMail[l])
			//		{
			//		lIndex = l;
			//		break;
			//		}
			//	}
			
			lID = m_alMail[lIndex];
			m_alMail[lIndex] = 0;
			m_nCount--;
			return lID;
			}

		INT32	Get(INT32 lIndex)
			{
			if (0 > lIndex || lIndex > m_nCount)
				return (-1);
			
			return m_alMail[lIndex];
			}
		
		INT32	GetIndex(INT32 lID)
			{
			INT32 lIndex = -1;
			for (INT32 l=0; l < AGPMMAILBOX_MAX_MAILS_IN_EFFECT; l++)
				{
				if (0 != m_alMail[l] && lID == m_alMail[l])
					{
					lIndex = l;
					break;
					}
				}
			return lIndex;
			}
	};


//
//	==========	Callback Parameter	==========
//
struct AgpdMailArg
	{
	AgpdMail	*m_pAgpdMail;
	INT32		m_lItemID;
	INT32		m_lResult;
	
	AgpdMailArg()
		{
		m_pAgpdMail = NULL;
		m_lItemID = 0;
		}
	};


#endif
