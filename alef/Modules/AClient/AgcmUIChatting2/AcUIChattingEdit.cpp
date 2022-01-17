#include	"AcUIChattingEdit.h"
#include	"AgpmChatting.h"

#include	"AuInputComposer.h"
#include	"AuStringConv.h"

extern AgcEngine*	g_pEngine;
extern UINT32	g_aulDefaultChatColor[AGPDCHATTING_TYPE_MAX];

AcUIChattingEdit::AcUIChattingEdit()
{
	SetTextMaxLength(100);

	m_eInputType = CHAT_INPUT_NORMAL;
	m_eInputStatus = CHAT_INPUT_MESSAGE;
	
	m_listChat = NULL;
	m_pCurChat = NULL;
	m_iChatNum = 0;

	m_listWhisperID = NULL;
	m_pCurWhisperID = NULL;
	m_iWhisperIDNum = 0;

	m_pInputInfoTex = NULL;
	m_hKeyboardLayOut = 0;
}

AcUIChattingEdit::~AcUIChattingEdit()
{
	if(m_listChat)
	{
		ChatHistory*	cur_chat = m_listChat;
		ChatHistory*	end_chat = cur_chat;
		ChatHistory*	next_chat;
		do
		{
			next_chat = cur_chat->next;
			delete cur_chat;
			cur_chat = next_chat;
		}while(cur_chat != end_chat);
		
		m_listChat = NULL;
	}

	m_pCurChat = NULL;

	if(m_listWhisperID)
	{
		WhisperIDHistory*	cur_id = m_listWhisperID;
		WhisperIDHistory*	end_id = cur_id;
		WhisperIDHistory*	next_id;
		
		do
		{
			next_id = cur_id;
			delete cur_id;
			cur_id = next_id;
		}while(cur_id != end_id);

		m_listWhisperID = NULL;
	}

	m_pCurWhisperID = NULL;
}


VOID	AcUIChattingEdit::OnClose()
{

}

BOOL	AcUIChattingEdit::OnIdle			(UINT32 ulClockCount)
{
	return AcUIEdit::OnIdle(ulClockCount);
}

BOOL	AcUIChattingEdit::OnChar( CHAR * pChar		, UINT lParam )
{
	if (m_bReadOnly) return FALSE;
	if (!m_szText)	return FALSE;

	// Multi Line이 아닌데, Enter를 누르면, 
	if (*pChar == VK_RETURN && !m_bMultiLine)
	{
		BOOL	bRet = TRUE;
		if (g_pEngine->GetCharCheckState())
		{
			if (!m_bActiveEdit) return FALSE; // 나에게 보내는 Enter가 아니다 

			char	szMsg[512];
			int		start_pos = 0;
			szMsg[0] = '\0';

			// message만 추출
			if(m_eInputType == CHAT_INPUT_NORMAL)
			{
				strcpy(szMsg,m_szText);
			}
			if(m_eInputType == CHAT_INPUT_WHISPER)
			{
				//귓말 모드였음 ID 를 IDHistory에 추가
				char	szID[32];
				char*	pFindSpace;

				if(m_szText[0] == '/')	start_pos = 3;			// /w
				else	start_pos = 1;							// ^
				
				int		len = 0;
				pFindSpace = strstr(&m_szText[start_pos]," ");
				if(pFindSpace)
				{
					len = pFindSpace - &m_szText[start_pos];
					strncpy(szID,&m_szText[start_pos],len);
					szID[len] = '\0';
					
					if( ( int ) strlen(m_szText) > start_pos + len + 1)
						strcpy(szMsg,&m_szText[start_pos + len + 1]);
					else 
						szMsg[0] = '\0';

					// 기존 IDHistory에 있는지 검사
					BOOL	bFind = FALSE;
					if(m_listWhisperID)
					{
						WhisperIDHistory*	cur_id = m_listWhisperID;
						WhisperIDHistory*	end_id = cur_id;
						
						do
						{
							if(!strcmp(szID,cur_id->szBuffer))
							{
								m_pCurWhisperID = cur_id;
								bFind = TRUE;
								break;
							}
							cur_id = cur_id->next;
						}while(cur_id != end_id);
					}

					if(!bFind)
					{
						WhisperIDHistory*	bef_id;
						if(m_iWhisperIDNum >= NUMBER_OF_ID_HISTORY)
						{
							bef_id = m_listWhisperID->bef;
							strcpy(bef_id->szBuffer,szID);

							m_pCurWhisperID = bef_id;
						}
						else 
						{
							WhisperIDHistory*	nw_id = new WhisperIDHistory;
							strcpy(nw_id->szBuffer,szID);
							m_pCurWhisperID = nw_id;

							if(m_listWhisperID)
							{
								bef_id = m_listWhisperID->bef;

								bef_id->next = nw_id;
								nw_id->bef = bef_id;
								nw_id->next = m_listWhisperID;
								m_listWhisperID->bef = nw_id;
							}
							else
							{
								m_listWhisperID = nw_id;
								nw_id->bef = m_listWhisperID;
								nw_id->next = m_listWhisperID;
							}

							++m_iWhisperIDNum;
						}
					}
				}
			}
			else if(m_eInputType == CHAT_INPUT_GUILD || m_eInputType == CHAT_INPUT_PARTY || m_eInputType == CHAT_INPUT_SHOUT)
			{
				if(m_szText[0] == '/')	start_pos = 3;		 
				else	start_pos = 1;				

				strcpy(szMsg,&m_szText[start_pos]);
			}

			//Message ChattingHistory에 추가
			if(strlen(szMsg))
			{
				// 기존 있는지 검사
				BOOL	bFind = FALSE;
				if(m_listChat)
				{
					ChatHistory*	cur_msg = m_listChat;
					ChatHistory*	end_msg = cur_msg;
					
					do
					{
						if(!strcmp(szMsg,cur_msg->szBuffer))
						{
							m_pCurChat = cur_msg;
							bFind = TRUE;
							break;
						}
						cur_msg = cur_msg->next;
					}while(cur_msg != end_msg);
				}

				if(!bFind)
				{
					ChatHistory*	bef_msg;
					if(m_iChatNum >= NUMBER_OF_CHAT_HISTORY)
					{
						bef_msg = m_listChat->bef;
						strcpy(bef_msg->szBuffer,szMsg);

						m_pCurChat = bef_msg;
					}
					else 
					{
						ChatHistory*	nw_msg = new ChatHistory;
						strcpy(nw_msg->szBuffer,szMsg);
						m_pCurChat = nw_msg;

						if(m_listChat)
						{
							bef_msg = m_listChat->bef;

							bef_msg->next = nw_msg;
							nw_msg->bef = bef_msg;
							nw_msg->next = m_listChat;
							m_listChat->bef = nw_msg;
						}
						else
						{
							m_listChat = nw_msg;
							nw_msg->bef = m_listChat;
							nw_msg->next = m_listChat;
						}
					}
				}
			}

			if (pParent)
				bRet = pParent->SendMessage( MESSAGE_COMMAND, (PVOID)UICM_EDIT_INPUT_END, (PVOID)&m_lControlID );

			ReleaseMeActiveEdit();

			m_eInputType = CHAT_INPUT_NORMAL;
			m_eInputStatus = CHAT_INPUT_MESSAGE;
		}
		else if (!m_bReadOnly)
		{
			SetMeActiveEdit();
		}

		return bRet;
	}

	if ( false == m_bActiveEdit )
		return FALSE;

	if (*pChar == VK_ESCAPE) // ESC 처리~
	{
		BOOL	bRet = TRUE;
		ClearText();
		if ( NULL != pParent )
			bRet = pParent->SendMessage( MESSAGE_COMMAND, (PVOID)UICM_EDIT_INPUT_END, (PVOID)&m_lControlID );

		// 엑티브 없에기.
		ReleaseMeActiveEdit();

		return bRet;
	}
	// Back Space일 경우 바로 앞 글자 지우기
	else if(*pChar == VK_BACK)
	{
		INT32	lCharLength = 0;

		// Start Of Line일 경우
		if (CheckSOL())
		{
			if (m_lCursorRow)
			{
				lCharLength		=	m_lDelimiterLength;
			}
		}
		// 한글일 경우
		else if (m_lCursorPoint > 1 && IsDBC( (unsigned char*)&m_szText[m_lCursorPoint - 1] ) )
		{
			if( IsDoubleByteCode_Kr( m_szText, m_lCursorPoint ) )
			{
				lCharLength	= 2;
			}
			else
			{
				lCharLength	= 1;
			}
		}
		// '<<'일 경우
		else if (m_bEnableTag && m_lCursorPoint > 1 && m_szText[m_lCursorPoint - 1] == '<' && m_szText[m_lCursorPoint - 2] == '<')
		{
			lCharLength = 2;
		}
		// 아닐 경우
		else if(m_lCursorPoint > 0)
		{
			lCharLength = 1;
		}

		// 지울 위치로부터 이후의 문자열을 복사해놓고
		CHAR strNext[ 256 ] = { 0, };
		strcpy_s( strNext, sizeof( CHAR ) * 256, &m_szText[ m_lCursorPoint ] );

		// 지울 위치에 NULL 문자열을 넣어서 끝이라고 표시해주고
		m_szText[ m_lCursorPoint - lCharLength ] = '\0';

		// 처음 위치부터 지울 위치까지 복사해주고
		CHAR strPrev[ 256 ] = { 0, };
		strcpy_s( strPrev, sizeof( CHAR ) * 256, m_szText );

		char tmp[256] = {0,};
		sprintf_s( tmp, sizeof( tmp ), "%s%s", strPrev, strNext );
		
		// 버퍼를 초기화하고 복사해둔 것들을 조합해서 넣어준다.
		memset( m_szText, 0, m_ulTextMaxLength );
		memcpy_s( m_szText, m_ulTextMaxLength, tmp, m_ulTextMaxLength );
		m_szText[m_ulTextMaxLength] = 0;

		m_lCursorPoint -= lCharLength;
	}
	// Tab일 경우 Message 보내기
	else if (*pChar == VK_TAB)
	{
		if ( pParent )
		{
			if (GetKeyState( VK_SHIFT ) < 0 )
			{
				pParent->SendMessage( MESSAGE_COMMAND, (PVOID)UICM_EDIT_INPUT_SHIFT_TAB, &m_lControlID );
			}
			else
			{
				pParent->SendMessage( MESSAGE_COMMAND, (PVOID)UICM_EDIT_INPUT_TAB, &m_lControlID );
			}
		}
	}
	else if ( strlen( pChar ) == 1 && *pChar >= 0 && *pChar < 0x20 && *pChar != VK_RETURN )
	{
		// ASCII 코드셋에서 0x20 이하는 컨트롤 셋이므로..
		// 처리하지 않은것들은 그냥 스킵 시켜 버린다.

		// do nothing..
	}
	// 그외엔 Buffer에 넣고 SetText
	else
	{
		CHAR *	pString		=	pChar;
		INT32	lStringLength;
		BOOL	bNewLine	=	FALSE;

		if (*pChar == VK_RETURN && m_bMultiLine)
		{
			pString			=	&m_szLineDelimiter[0];
			lStringLength	=	m_lDelimiterLength;
			bNewLine		=	TRUE;
		}
		else if (m_bNumberOnly && ( *pChar < 48 || *pChar > 57 ))
		{
			// NumberOnly인데 숫자가 아니다. NOP
		}
		else if( IsDBC((unsigned char*)(pChar+1)) ) // DBC
		{
			lStringLength = strnlen( pChar, m_ulTextMaxLength );
			memset(m_szComposing, 0, sizeof(m_szComposing));
		}
		else
		{
			lStringLength	= strnlen( pChar, m_ulTextMaxLength );
		}

		if(m_lCursorPoint + lStringLength <= ( int ) m_ulTextMaxLength)
		{
			INT32	lIndex;

			for (lIndex = m_lTextLength - 1; lIndex >= m_lCursorPoint; --lIndex)
			{
				m_szText[lIndex + lStringLength] = m_szText[lIndex];
			}

			memcpy( &m_szText[m_lCursorPoint], pString, lStringLength );

			m_szText[m_lTextLength + lStringLength]	=	0;

			m_lCursorPoint	+=	lStringLength;
		}
	}

	SetText();
	UpdateCurrentStatus();

	return TRUE;
}

BOOL	AcUIChattingEdit::OnKeyDown		( RsKeyStatus *ks	)
{
	if (!m_szText)
		return FALSE;

	if (!m_bActiveEdit)
		return FALSE;

	// 2005.2.17 gemani (Ime편집중엔 OnKeyDown은 2번 오더라.. 한글 편집중엔 무시하자)
	if(strlen(m_szComposing)) 
	{
		SetText();
		UpdateCurrentStatus();
		return TRUE;
	}

	switch (ks->keyCharCode)
	{
	case rsLEFT:
		{
			BOOL bExitLoop = FALSE;

			while (!bExitLoop)
			{
				bExitLoop = TRUE;

				if (CheckSOL())
				{
					m_lCursorPoint	-=	m_lDelimiterLength;
				}
				else if (m_bEnableTag && m_lCursorPoint > 1 && m_szText[m_lCursorPoint - 1] == '>')
				{
					INT32	lIndex;
					INT32	lIndex2;
					INT32	lNum = 0;

					for (lIndex = m_lCursorPoint - 1; lIndex >= 0; --lIndex)
					{
						if (m_szText[lIndex] == '<')
							break;
					}

					for (lIndex2 = lIndex; lIndex2 >= 0; --lIndex2)
					{
						if (m_szText[lIndex2] == '<')
							++lNum;
						else
							break;
					}

					// 만약 '<'이 짝수개면, '>'는 다른 의미를 가지지 않는다.
					if (lNum % 2 == 0)
					{
						--m_lCursorPoint;
					}
					else
					{
						m_lCursorPoint = lIndex;
						bExitLoop = FALSE;
					}
				}
				else if (m_lCursorPoint > 1 && IsDBC((unsigned char*)&m_szText[m_lCursorPoint - 1]) )
				{
					m_lCursorPoint	-=	2;
				}
				else if (m_lCursorPoint > 0)
				{
					--m_lCursorPoint;
				}
			}

			SetText();
			UpdateCurrentStatus();
		}
		break;

	case rsRIGHT:
		{
			BOOL bExitLoop = FALSE;

			while (!bExitLoop)
			{
				bExitLoop = TRUE;

				if (CheckEOL())
				{
					m_lCursorPoint	+=	m_lDelimiterLength;
				}
				else if (m_bEnableTag && m_szText[m_lCursorPoint] == '<')
				{
					INT32	lIndex;

					// '<<' 이면 무시
					if (m_lCursorPoint < m_lTextLength - 1 && m_szText[m_lCursorPoint + 1] == '<')
						m_lCursorPoint += 2;
					else
					{
						for (lIndex = m_lCursorPoint + 1; lIndex < m_lTextLength; ++lIndex)
						{
							if (m_szText[lIndex] == '>')
								break;
						}

						if (lIndex != m_lTextLength)
						{
							m_lCursorPoint = lIndex + 1;
							bExitLoop = FALSE;
						}
					}
				}
				else if (m_lCursorPoint < m_lTextLength - 1 && IsDBC((unsigned char*)&m_szText[m_lCursorPoint+1]) )
				{
					m_lCursorPoint	+=	2;
				}
				else if (m_lCursorPoint < m_lTextLength)
				{
					++m_lCursorPoint;
				}
			}

			SetText();
			UpdateCurrentStatus();
		}
		break;

	case rsUP:
		{
			if(m_eInputStatus == CHAT_INPUT_MESSAGE)
			{
				if(m_pCurChat)
				{
					if(!strcmp(&m_szText[m_iChatStartPos],m_pCurChat->szBuffer) )
					{
						m_pCurChat = m_pCurChat->bef;
					}

					// 남은 길이만큼만 복사한다
					int restSize = m_ulTextMaxLength - m_iChatStartPos;
					memcpy_s(&m_szText[m_iChatStartPos], restSize, m_pCurChat->szBuffer, restSize);
					m_szText[m_ulTextMaxLength] = 0;

					m_lCursorPoint = strlen(m_szText);
					memset(m_szComposing, 0, sizeof(m_szComposing));
					SetText();
					UpdateCurrentStatus();

					m_pCurChat = m_pCurChat->bef;
				}
			}
			else if(m_eInputStatus == CHAT_INPUT_WHISPER_ID)
			{
				if(m_pCurWhisperID)
				{
					if(!strcmp(&m_szText[m_iWhisperIDStartPos],m_pCurWhisperID->szBuffer) )
					{
						m_pCurWhisperID = m_pCurWhisperID->bef;
					}

					if(m_szText[0] == '/')
					{
						m_szText[2] = ' ';
					}
					
					strcpy(&m_szText[m_iWhisperIDStartPos],m_pCurWhisperID->szBuffer);
					m_lCursorPoint = strlen(m_szText);
					memset(m_szComposing, 0, sizeof(m_szComposing));
					SetText();
					UpdateCurrentStatus();

					m_pCurWhisperID = m_pCurWhisperID->bef;
				}
			}
		}
		break;

	case rsDOWN:
		{
			if(m_eInputStatus == CHAT_INPUT_MESSAGE)
			{
				if(m_pCurChat)
				{
					m_pCurChat = m_pCurChat->next;
					
					if(!strcmp(&m_szText[m_iChatStartPos],m_pCurChat->szBuffer) )
					{
						m_pCurChat = m_pCurChat->next;
					}

					strcpy(&m_szText[m_iChatStartPos],m_pCurChat->szBuffer);
					m_lCursorPoint = strlen(m_szText);
					memset(m_szComposing, 0, sizeof(m_szComposing));
					SetText();
					UpdateCurrentStatus();
				}
			}
			else if(m_eInputStatus == CHAT_INPUT_WHISPER_ID)
			{
				if(m_pCurWhisperID)
				{
					m_pCurWhisperID = m_pCurWhisperID->next;
					
					if(!strcmp(&m_szText[m_iWhisperIDStartPos],m_pCurWhisperID->szBuffer) )
					{
						m_pCurWhisperID = m_pCurWhisperID->next;
					}

					if(m_szText[0] == '/')
					{
						m_szText[2] = ' ';
					}

					strcpy(&m_szText[m_iWhisperIDStartPos],m_pCurWhisperID->szBuffer);
					m_lCursorPoint = strlen(m_szText);
					memset(m_szComposing, 0, sizeof(m_szComposing));
					SetText();
					UpdateCurrentStatus();
				}
			}
		}
		break;
	case rsDEL:
		{
			if(strlen(m_szComposing))		// 편집중인 글자 입력 처리
			{
				strncpy(&m_szText[m_lCursorPoint], m_szComposing, 2);
				memset(m_szComposing, 0, sizeof(m_szComposing));
				m_lCursorPoint += 2;
			}

			if(m_szText[m_lCursorPoint] != '\0')
			{
				if( IsDBC( (unsigned char*)&m_szText[m_lCursorPoint+1] ) ) // 한글이라면
				{
					for (int i = m_lCursorPoint; i < m_lTextLength; ++i)
					{
						if(i+2 < ( int ) m_ulTextMaxLength)	m_szText[i] = m_szText[i+2];
					}
				}
				else
				{
					for (int i = m_lCursorPoint; i < m_lTextLength; ++i)
					{
						if(i+1 < ( int ) m_ulTextMaxLength)	m_szText[i] = m_szText[i+1];
					}
				}
			}

			SetText();
			UpdateCurrentStatus();
		}
		break;
	}

	return TRUE;
}

VOID	AcUIChattingEdit::OnWindowRender	(					)
{
	PROFILE("AcUIChattingEdit::OnWindowRender");

	INT32				abs_x = 0	;
	INT32				abs_y = 0	;
	AgcdFontClippingBox	stClippingBox;
	INT32				lStartX		;
	INT32				lStartY		;
	//INT32				lOffsetX	;
	INT32				lScrollMargin = 0;
	INT32				lFontType	;
	AgcdUIEditStringInfo *	pstStringInfo;

	ClientToScreen( &abs_x, &abs_y );

	AcUIBase::OnWindowRender();

	if (!m_pAgcmFont)	return;

	++m_lCursorCount;

	DWORD				dwFontColor;
	if(m_eInputType == CHAT_INPUT_NORMAL)	dwFontColor = g_aulDefaultChatColor[AGPDCHATTING_TYPE_NORMAL];
	else if(m_eInputType == CHAT_INPUT_WHISPER) dwFontColor = g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER];
	else if(m_eInputType == CHAT_INPUT_PARTY) dwFontColor = g_aulDefaultChatColor[AGPDCHATTING_TYPE_PARTY];
	else if(m_eInputType == CHAT_INPUT_GUILD) dwFontColor = g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD];
	else if(m_eInputType == CHAT_INPUT_SHOUT) dwFontColor = g_aulDefaultChatColor[AGPDCHATTING_TYPE_WORD_BALLOON];
	
	stClippingBox.x	=	( float ) abs_x;
	stClippingBox.y	=	( float ) abs_y;
	stClippingBox.w	=	( float ) m_lBoxWidth;
	stClippingBox.h	=	( float ) h;
	if (m_bVScrollLeft && m_pcsVScroll)
	{
		lScrollMargin = m_pcsVScroll->w;
		stClippingBox.x += lScrollMargin;
	}

	lFontType = m_lFontType;

	m_pAgcmFont->FontDrawStart(lFontType);
	m_pAgcmFont->SetClippingArea(&stClippingBox);

	pstStringInfo = m_pstStringInfoHead;
	while (pstStringInfo)
	{
		lStartX = abs_x + ACUI_EDIT_LEFT_MARGIN + m_lStartX;

		if (pstStringInfo->lFontType != -1 && lFontType != pstStringInfo->lFontType)
		{
			m_pAgcmFont->FontDrawEnd();
			m_pAgcmFont->FontDrawStart(pstStringInfo->lFontType);

			lFontType = pstStringInfo->lFontType;
		}

		lStartY = abs_y + m_lStartY + pstStringInfo->lOffsetY;

		if (lStartY + m_alStringHeight[pstStringInfo->lLineIndex] >= stClippingBox.y &&
			lStartY <= stClippingBox.y + stClippingBox.h)
		{
			// 맨 마지막 문자열이고 편집중인 문자가 있으면 편집화면 전까지 문자 찍자..
			if(pstStringInfo == m_pstStringInfoTail && strlen(m_szComposing))
			{
				char	szBufferTemp[2048];
				strncpy(szBufferTemp,pstStringInfo->szString,m_lCursorPoint);
				szBufferTemp[m_lCursorPoint] = '\0';

				m_pAgcmFont->DrawTextIM2D(
									( float ) ( lStartX + pstStringInfo->lOffsetX + lScrollMargin ),
									( float ) lStartY
									,szBufferTemp,
									m_lFontType, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ), dwFontColor);//pstStringInfo->dwColor);

				if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
				{
					// caret 출력
					if(m_lCursorCount > 500 && m_bActiveEdit)
					{
						if (g_pEngine->GetIMEComposingState() )
						{
							m_pAgcmFont->DrawCaret( (FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY, m_lFontType, 0xFFFFFFFF, m_pAgcmFont->GetTextExtent(m_lFontType, m_szComposing, strlen(m_szComposing)) );
						}
						else
							m_pAgcmFont->DrawCaretE((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY, m_lFontType);

						// 편집중 문자 출력
						m_pAgcmFont->DrawTextIM2D((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY,
										m_szComposing, m_lFontType, 255, 0xff000000);
					}
					else
					{
						// 편집중 문자 출력
						m_pAgcmFont->DrawTextIM2D((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY,
										m_szComposing, m_lFontType, 255, dwFontColor);// pstStringInfo->dwColor);
					}
				}
				else
				{
					if(m_lCursorCount > 500 && m_bActiveEdit)
					{
						m_pAgcmFont->DrawCaretE((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY, m_lFontType);
					}

					m_pAgcmFont->DrawTextIM2D((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY,
									m_szComposing, m_lFontType, 255, 0xFFE1DC16);// pstStringInfo->dwColor);
				}

				int str_offset_temp = m_pAgcmFont->GetTextExtent(pstStringInfo->lFontType,szBufferTemp,strlen(szBufferTemp))
									  + m_pAgcmFont->GetTextExtent(pstStringInfo->lFontType, m_szComposing, strlen(m_szComposing) );

				// 나머지 문자 출력
				if(pstStringInfo->szString[m_lCursorPoint])
				{
					strcpy(szBufferTemp,&pstStringInfo->szString[m_lCursorPoint]);
					m_pAgcmFont->DrawTextIM2D(
										( float ) ( lStartX + pstStringInfo->lOffsetX + lScrollMargin + str_offset_temp ),
										( float ) lStartY,
										szBufferTemp,
										m_lFontType, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ), dwFontColor );//pstStringInfo->dwColor);
				}
			}
			else
			{
				m_pAgcmFont->DrawTextIM2D(
									( float ) ( lStartX + pstStringInfo->lOffsetX + lScrollMargin ),
									( float ) lStartY,
									pstStringInfo->szString,
									m_lFontType, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ),dwFontColor);// pstStringInfo->dwColor);

				// caret 출력
				if(m_lCursorCount > 500 && m_bActiveEdit)
				{
					if (g_pEngine->GetIMEComposingState() )
					{
						m_pAgcmFont->DrawCaret((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin),
											   (FLOAT)abs_y + m_lCursorPosY,
											   m_lFontType,
											   0xFFFFFFFF,
											   m_pAgcmFont->GetTextExtent(m_lFontType, m_szComposing, strlen(m_szComposing)));
					}
					else
						m_pAgcmFont->DrawCaretE((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)abs_y + m_lCursorPosY, m_lFontType);
				}
			}
		}

		pstStringInfo = pstStringInfo->pstNext;
	}

	if(m_lCursorCount > 1000)
		m_lCursorCount = 0;

    m_pAgcmFont->SetClippingArea(NULL);

	if ( g_eServiceArea == AP_SERVICE_AREA_JAPAN )
	{
		// clipping되는 영역 바깥쪽에 그린다.
		// reading window 그리기
		DrawReadingWindow((FLOAT)(abs_x + m_lCursorPosX + lScrollMargin), (FLOAT)(abs_y + m_lCursorPosY));

		// candidate window 그리기
		DrawCandidateWindow((FLOAT)(abs_x + lScrollMargin + ACUI_EDIT_LEFT_MARGIN ), (FLOAT)(abs_y + m_lCursorPosY));
	}

	// 한/영 모드 출력
	if(m_pInputInfoTex)
	{
		g_pEngine->DrawIm2D(m_pInputInfoTex, ( float ) abs_x - 41.0f, ( float ) abs_y,16.0f,16.0f);

		if ( AuInputComposer::GetInstance().GetImeState() )
			m_pAgcmFont->DrawTextIM2D( ( float ) abs_x - 38.0f, ( float ) abs_y + 1.0f, AuInputComposer::GetInstance().GetIndicator(), m_lFontType, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ),0xffffffff);
		else
			m_pAgcmFont->DrawTextIM2D( ( float ) abs_x - 38.0f, ( float ) abs_y + 1.0f, "A", m_lFontType, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ), 0xffffffff);
	}

	m_pAgcmFont->FontDrawEnd();
}

void	AcUIChattingEdit::UpdateCurrentStatus()
{
	if(!m_szText) return;

	m_eInputType = CHAT_INPUT_NORMAL;
	m_eInputStatus = CHAT_INPUT_MESSAGE;
	m_iChatStartPos = 0;

	if(m_szText[0] == '^')
	{
		m_eInputType = CHAT_INPUT_WHISPER;

		char*	pFindSpace = strstr(m_szText," ");
		if(pFindSpace) 
		{
			m_eInputStatus = CHAT_INPUT_MESSAGE;
			m_iChatStartPos = pFindSpace - m_szText + 1;
		}
		else
		{
			m_eInputStatus = CHAT_INPUT_WHISPER_ID;
			m_iWhisperIDStartPos = 1;
		}
	}
	else if(m_szText[0] == '$')
	{
		m_eInputType = CHAT_INPUT_PARTY;
		m_iChatStartPos = 1;
	}
	else if(m_szText[0] == '@')
	{
		m_eInputType = CHAT_INPUT_GUILD;
		m_iChatStartPos = 1;
	}
	else if(m_szText[0] == '!')
	{
		m_eInputType = CHAT_INPUT_SHOUT;
		m_iChatStartPos = 1;
	}
	else if(m_szText[0] == '/')
	{
		if(m_szText[1] == 'w')
		{
			m_eInputType = CHAT_INPUT_WHISPER;
			if(strlen(m_szText) > 2)
			{
				char*	pFindSpace = strstr(m_szText," ");
				if(pFindSpace)
				{
					pFindSpace = strstr(pFindSpace+1," ");
					if(pFindSpace)
					{
						m_eInputStatus = CHAT_INPUT_MESSAGE;
						m_iChatStartPos = pFindSpace - m_szText + 1;
					}
					else
					{
						m_eInputStatus = CHAT_INPUT_WHISPER_ID;
						m_iWhisperIDStartPos = 3;
					}
				}
			}
			else
			{
				m_eInputStatus = CHAT_INPUT_WHISPER_ID;
				m_iWhisperIDStartPos = 3;
			}
		}
		else
		{
			if(m_szText[2] == ' ')
			{
				if(m_szText[1] == 'p')
				{
					m_eInputType = CHAT_INPUT_PARTY;
					m_iChatStartPos = 3;
				}
				else if(m_szText[1] == 'g')
				{
					m_eInputType = CHAT_INPUT_GUILD;
					m_iChatStartPos = 3;
				}
				else if(m_szText[1] == 's')
				{
					m_eInputType = CHAT_INPUT_SHOUT;
					m_iChatStartPos = 3;
				}
			}
		}
	}
}

VOID	AcUIChattingEdit::OnEditActive	(					)
{
	//if (pParent)
	//	pParent->SendMessage(MESSAGE_COMMAND, (PVOID) UICM_EDIT_ACTIVE, (PVOID)&m_lControlID);

	UpdateCurrentStatus();
}

BOOL AcUIChattingEdit::IsDoubleByteCode_Kr( char* pString, int nIndex )
{
	int nLength = strlen( pString );
	if( !pString || nLength <= 0 ) return FALSE;
	if( nIndex > nLength ) return FALSE;

	BOOL bIsNullCode = FALSE;
	char* pCode = &pString[ nIndex ];
	if( !pCode ) return FALSE;
	if( *pCode == '\0' )
	{
		// 널문자의 경우에는 한바이트 위를 검사한다.
		pCode = &pString[ nIndex - 1 ];
		bIsNullCode = TRUE;

		if( *pCode == '\\' || *pCode == '@' || *pCode == '^' || *pCode == '~' || *pCode == '`' || *pCode == '|' || *pCode == '_'
			|| *pCode == '[' || *pCode == ']' || *pCode == '{' || *pCode == '}' )
		{
			return FALSE;
		}
	}
	else if( *pCode == '\\' || *pCode == '@' || *pCode == '^' || *pCode == '~' || *pCode == '`' || *pCode == '|' || *pCode == '_'
		|| *pCode == '[' || *pCode == ']' || *pCode == '{' || *pCode == '}' )
	{
		return FALSE;
	}

	if( !bIsNullCode )
	{
		// 2바이트 문자의 상위바이트인가를 검사하기 위해서 앞의 2바이트가 한개의 문자인지 검사
		if( IsDBC( ( unsigned char* )pCode - 1 ) )
		{
			// 현 위치로부터 2바이트가 2바이트문자코드인가
			return IsDBC( ( unsigned char* )pCode + 1 ) ? TRUE : FALSE;
		}
		// 2바이트 문자의 하위바이트인가
		else if( IsDBC( ( unsigned char* )pCode ) )
		{
			// 2바이트 문자의 하위바이트이다.
			return TRUE;
		}
	}
	else
	{
		// 2바이트 문자의 상위바이트인가를 검사하기 위해서 앞의 2바이트가 한개의 문자인지 검사
		if( IsDBC( ( unsigned char* )pCode ) )
		{
			// 2바이트 문자의 하위바이트이다.
			return TRUE;
		}
	}

	// 이도 저도 아닌 경우에는 2바이트 문자가 아닌 것이다.
	return FALSE;
}
