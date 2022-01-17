// -----------------------------------------------------------------------------
//                                _    _ _____  _____                       _                           
//     /\                        | |  | |_   _|/ ____|                     | |                          
//    /  \    __ _  ___ _ __ ___ | |  | | | | | |      ___  _ __  ___  ___ | | ___      ___ _ __  _ __  
//   / /\ \  / _` |/ __| '_ ` _ \| |  | | | | | |     / _ \| '_ \/ __|/ _ \| |/ _ \    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| | | | | | |__| |_| |_| |____| (_) | | | \__ \ (_) | |  __/ _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|_| |_| |_|\____/|_____|\_____|\___/|_| |_|___/\___/|_|\___|(_) \___| .__/| .__/ 
//            __/ |                                                                        | |   | |    
//           |___/                                                                         |_|   |_|    
//
// Angelscript-based script engine
//
// -----------------------------------------------------------------------------
// Originally created on 02/15/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcmUIConsole.h"
#include <ctype.h>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <ctime>

#define LINE_STACKED(lines, baseLine, lineCount) ((lineCount) - (lines) + 2*(baseLine) + 1)

static const unsigned int cConsoleWidth = 860;
static const unsigned int cConsoleHeight = 510;

// -----------------------------------------------------------------------------
// Print a text to console.
static void print(const std::string& text)
{
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print(text);
}
static void printInt(int i)
{
	ostringstream stream;
	stream << i;
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print(stream.str());
}
static void printUInt(UINT i)
{
	ostringstream stream;
	stream << i;
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print(stream.str());
}
static void printFloat(float f)
{
	ostringstream stream;
	stream << f;
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print(stream.str());
}
static void exitConsole()
{
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->Off();
}
static void testFormatString()
{
	std::ostringstream output;
	output << "name" << "\t" << "e-mail" << "\t\t\t\t" << "homepage" << std::endl;
	output << "Jaewon" << "\t" << "manee@mac.com" << "\t" << "3dengine.org" << std::endl;
	print(output.str());
}
//@{ Jaewon 20050222
// RenderWare render states for console registering
extern "C"
{
extern RwUInt32 RenderStateD3D[210];
extern RwUInt32 TextureStageStateD3D[RWD3D9_MAX_TEXTURE_STAGES][33];
}
static UINT getRenderStateD3D(UINT index)
{
	if(index >= 210)
	{
		((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print("The index must be within [0, 210).");
		return 0;
	}

	return RenderStateD3D[index];
}
static UINT getTextureStageStateD3D(UINT stage, UINT index)
{
	if(stage >= 8)
	{
		((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print("The stage must be within [0, 8).");
		return 0;
	}
	if(index >= 33)
	{
		((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print("The index must be within [0, 33).");
		return 0;
	}

	return TextureStageStateD3D[stage][index];
}
//@} Jaewon
// Show the value of a script-registered variable.
//static void watch(const std::string& varName)
//{
//	std::string command = "string tmp = " + varName + "; print(tmp);";
//	if(false == ((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getScriptEngine().execute(command))
//	{
//		std::string tmp;
//		if(((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getScriptEngine().getVariableDeclarationByName(tmp, varName))
//		// It's a not-watchable variable.
//		{
//			tmp = "Cannot watch " + varName + ".";
//			print(tmp);
//		}
//		else
//		// It's a unregistered variable.
//		{
//			tmp = varName + " unregistered.";
//			print(tmp);
//		}
//	}
//}

// -----------------------------------------------------------------------------
AgcConsole::AgcConsole(AuScriptEngine *pScriptEngine, unsigned int maxLines, unsigned int maxHistory)
		: AuConsole(pScriptEngine, maxLines, maxHistory), pAgcmFont_(NULL), 
		scrollBar_(0), visibleLines_(30), visibleExtent_(800),
		watchExtent_(200), showWatch_(true)
{
	setDefaultMessage("-- a what?");
	setCommandLineIndicator(":) ");
	fptrPassServerDo_ = NULL;
	pUIconsole_ = NULL;
}
AgcConsole::~AgcConsole()
{
}

// Register methods & variables to the script engine.
void AgcConsole::init()
{
	pAgcmFont_ = (AgcmFont*) g_pEngine->GetModule("AgcmFont");

	AS_REGISTER_TYPE_BEGIN(AgcConsole, console);
		AS_REGISTER_METHOD0(void, moveTL);
		AS_REGISTER_METHOD0(void, moveTR);
		AS_REGISTER_METHOD0(void, moveBL);
		AS_REGISTER_METHOD0(void, moveBR);
		AS_REGISTER_METHOD0(void, moveCenter);
		AS_REGISTER_METHOD3(bool, registerWatch, uint, string &in, bits);
		AS_REGISTER_METHOD0(void, showWatch);
		AS_REGISTER_METHOD0(void, hideWatch);
		AS_REGISTER_VARIABLE(string, bufferForWatching_);
	AS_REGISTER_TYPE_END;
}

BOOL AgcConsole::OnInit()
{
	return TRUE;
}
VOID AgcConsole::OnWindowRender()
{
	render();
}
BOOL AgcConsole::OnKeyDown(RsKeyStatus *ks)
{
	switch(ks->keyCharCode)
	{
	case rsLEFT:
		passLeft();
		scrollBar_ = 0;
		break;
	case rsRIGHT:
		passRight();
		scrollBar_ = 0;
		break;
	case rsUP:
		passUp();
		scrollBar_ = 0;
		break;
	case rsDOWN:
		passDown();
		scrollBar_ = 0;
		break;
	case rsPGUP:
		{
		std::string commandLine = commandLineIndicator_ + commandLine_;
		size_t totalLines = getNumberOfLines(commandLine.c_str(), getTextExtent());
		for(size_t i=0; i<textBuffer_.size(); ++i)
			totalLines += getNumberOfLines(textBuffer_[i].c_str(), getTextExtent());
		if(totalLines > visibleLines_ && scrollBar_ < totalLines-visibleLines_)
			++scrollBar_;
		}
		break;
	case rsPGDN:
		if(scrollBar_ > 0)
			--scrollBar_;
		break;
	}

	return TRUE;
}
BOOL AgcConsole::OnChar(char *keyCode, UINT lParam)
{
	switch(*keyCode)
	{
	case VK_ESCAPE:
		((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->Off();
		break;
	case VK_BACK:
		passBackspace();
		scrollBar_ = 0;
		break;
	case VK_RETURN:
		{
			// 스크립트 함수가 파라메터를 가지는 경우 단순히 set::find만으로는 서버키워드를 찾기에 문제가 많아서
			// 다음과 같이 바꿈
			CHAR* pKeyword;
			BOOL bFind = FALSE;

			for (std::set<string>::iterator iter = serverKeyword_.begin(); iter != serverKeyword_.end(); ++iter)
			{
				pKeyword = strtok(const_cast<CHAR*>(iter->c_str()), "(");
				if (NULL ==pKeyword) 
					continue;

				if (pKeyword[strlen(pKeyword) - 1] == '.')
					continue;

				if (strstr(commandLine_.c_str(), pKeyword))
				{
					bFind = TRUE;
					break;
				}
			}

			if (bFind)
				passServerDo();
			else
				passDo();
			scrollBar_ = 0;
		}
		break;
	case VK_TAB:
		passTab();
		scrollBar_ = 0;
		break;
	default:
		if(__isascii(*keyCode))
		{
			passKey(*keyCode);
			scrollBar_ = 0;
		}
		break;
	}

	return TRUE;
}
BOOL AgcConsole::OnKeyUp(RsKeyStatus *ks)
{
	return TRUE;
}

BOOL AgcConsole::OnLButtonDown(RsMouseStatus *ms)
{
	g_pEngine->SetMeActiveEdit(this);

	return TRUE;
}

void AgcConsole::render()
{
	if(NULL == pAgcmFont_)
		return;

	pAgcmFont_->FontDrawStart(0);

	size_t lines = 0;
	size_t length = 0;
	// Line count of the current buffer
	size_t lineCount;
	// Base line of the current buffer
	size_t baseLine;
	const char *text;
	std::string commandLine = commandLineIndicator_ + commandLine_;
	const char *caret = commandLine.c_str() + commandLineIndicator_.length() + caretPos_;
	
	// Draw the command line.
	text = commandLine.c_str();
	lineCount = getNumberOfLines(text, getTextExtent());
	baseLine = lines;
	do
	{
		if(length>0 && isVisibleLine(LINE_STACKED(lines, baseLine, lineCount)))
		{
			DrawOneLine(LINE_STACKED(lines, baseLine, lineCount), text, length);

			if(text <= caret && caret < text+length)
			// Draw the caret.
			{
				// Compute the pixel offset of the caret.
				int offset = pAgcmFont_->GetTextExtent(0, const_cast<char*>(text), caret-text);
				// Draw.
				// Blink.
				if(unsigned int(2 * clock() / CLOCKS_PER_SEC) % 2)
					pAgcmFont_->DrawTextIM2D(this->x + 30.0f+float(offset), this->y + 480.0f - 15.0f*(LINE_STACKED(lines, baseLine, lineCount)-scrollBar_), "_", 0, 255, 0xff00ff00, false, true);
			}
		}

		++lines;
		text += length;
	} 
	while(length = searchTextLengthOfGivenExtent(text, getTextExtent()));

	if(isVisibleLine(LINE_STACKED(lines, baseLine, lineCount)))
	{
		DrawOneLine(LINE_STACKED(lines, baseLine, lineCount), text, strlen(text));

		if(text <= caret)
		// Draw the caret.
		{
			// Compute the pixel offset of the caret.
			int offset = pAgcmFont_->GetTextExtent(0, const_cast<char*>(text), caret-text);
			// Draw.
			// Blink.
			if(unsigned int(2 * clock() / CLOCKS_PER_SEC) % 2)
				pAgcmFont_->DrawTextIM2D(this->x + 30.0f+float(offset), this->y + 480.0f - 15.0f*(LINE_STACKED(lines, baseLine, lineCount)-scrollBar_), "_", 0, 255, 0xff00ff00, false, true);
		}
	}

	// Draw text buffers.
	for(size_t i=0; i<textBuffer_.size(); ++i)
	{
		//if(lines+1 > scrollBar_+visibleLines_)
		// All visible lines have been printed.
		//	break;

		text = textBuffer_[textBuffer_.size()-i-1].c_str();
		lineCount = getNumberOfLines(text, getTextExtent());
		baseLine = lines;
		do
		{
			if(length>0 && isVisibleLine(LINE_STACKED(lines, baseLine, lineCount)))
				DrawOneLine(LINE_STACKED(lines, baseLine, lineCount), text, length);

			++lines;
			text += length;
		} 
		while(length = searchTextLengthOfGivenExtent(text, getTextExtent()));

		if(isVisibleLine(LINE_STACKED(lines, baseLine, lineCount)))
			DrawOneLine(LINE_STACKED(lines, baseLine, lineCount), text, strlen(text));
	}

	// Draw scroll indicators.
	if(scrollBar_ > 0)
		pAgcmFont_->DrawTextIM2D(this->x + 10.0f, this->y + 315.0f, "↓", 0, 255, 0xff00ff00, false, true);
	if(lines > visibleLines_ && scrollBar_ < lines-visibleLines_)
		pAgcmFont_->DrawTextIM2D(this->x + 10.0f, this->y + 30.0f, "↑", 0, 255, 0xff00ff00, false, true);

	pAgcmFont_->FontDrawEnd();

	// Draw the window frame.
	DrawFrame(this->x+5.0f, this->y+5.0f, this->w-10.0f, this->h-10.0f);

	if(showWatch_)
		DrawWatchList();
}

size_t AgcConsole::searchTextLengthOfGivenExtent(const char *text, size_t extent) const
{
	if(NULL == pAgcmFont_)
		return 0;

	if(pAgcmFont_->GetTextExtent(0, const_cast<char*>(text), strlen(text)) <= (INT32)extent)
		return 0;

	for(size_t i=1; i<strlen(text); ++i)
	{
		if(pAgcmFont_->GetTextExtent(0, const_cast<char*>(text), i) > (INT32)extent)
			return i-1;
	}

	return 0;
}

size_t AgcConsole::getNumberOfLines(const char *text, size_t extent) const
{
	size_t lines = 0;
	size_t length = 0;

	do
	{
		++lines;
		text += length;
	} 
	while(length = searchTextLengthOfGivenExtent(text, extent));

	return lines;
}

bool AgcConsole::isVisibleLine(size_t line) const
{
	return line > scrollBar_ && line <= scrollBar_+visibleLines_;
}

void AgcConsole::DrawOneLine(size_t line, const char* text, size_t length)
{
	char buf[1024];
	length = min(length, 1024-1);
	strncpy(buf, text, length);
	buf[length] = '\0';

	pAgcmFont_->DrawTextIM2D(this->x + 30.0f, this->y + 480.0f - 15.0f*(line-scrollBar_), buf, 0, 255, 0xffffffff, true);
}

void AgcConsole::DrawFrame(float x, float y, float w, float h) const
{
	FLOAT recipCamZ;

	recipCamZ = 1.0f / RwCameraGetNearClipPlane(((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->GetCamera());

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (PVOID)NULL);

	RwIm2DVertex polyLine[5];

	ZeroMemory(polyLine, sizeof(polyLine));

	for(int i=0; i<5; ++i)
	{
		RwIm2DVertexSetIntRGBA(&polyLine[i], 255, 255, 255, 255);
		RwIm2DVertexSetScreenZ(&polyLine[i], RwIm2DGetNearScreenZ());
		RwIm2DVertexSetRecipCameraZ(&polyLine[i], recipCamZ);
	}

	RwIm2DVertexSetScreenX(&polyLine[0], x);
	RwIm2DVertexSetScreenY(&polyLine[0], y);

	RwIm2DVertexSetScreenX(&polyLine[1], x);
	RwIm2DVertexSetScreenY(&polyLine[1], y+h);

	RwIm2DVertexSetScreenX(&polyLine[2], x+w);
	RwIm2DVertexSetScreenY(&polyLine[2], y+h);

	RwIm2DVertexSetScreenX(&polyLine[3], x+w);
	RwIm2DVertexSetScreenY(&polyLine[3], y);

	RwIm2DVertexSetScreenX(&polyLine[4], x);
	RwIm2DVertexSetScreenY(&polyLine[4], y);

	RwIm2DRenderPrimitive(rwPRIMTYPEPOLYLINE, polyLine, 5);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

// Move console to the top-left corner of the parent.
void AgcConsole::moveTL()
{
	MoveWindow(0, 0,
			cConsoleWidth,
			cConsoleHeight);
}

// Move console to the top-right corner of the parent.
void AgcConsole::moveTR()
{
	MoveWindow(pParent->w - cConsoleWidth, 0,
			cConsoleWidth,
			cConsoleHeight);
}

// Move console to the bottom-left corner of the parent.
void AgcConsole::moveBL()
{
	MoveWindow(0, pParent->h - cConsoleHeight,
			cConsoleWidth,
			cConsoleHeight);
}

// Move console to the bottom-right corner of the parent.
void AgcConsole::moveBR()
{
	MoveWindow(pParent->w - cConsoleWidth, pParent->h - cConsoleHeight,
			cConsoleWidth,
			cConsoleHeight);
}

// Move console to the center of the parent.
void AgcConsole::moveCenter()
{
	MoveWindow((pParent->w - cConsoleWidth)/2,
			(pParent->h - cConsoleHeight)/2,
			cConsoleWidth,
			cConsoleHeight);
}

// Register a variable to the watch list.
bool AgcConsole::registerWatch(size_t index, const std::string& varName, DWORD color)
{
	std::string tmp;

	if(index >= visibleLines_)
	{
		print("The index must be within [0, 30).");
		return false;
	}

	if(((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getScriptEngine().getVariableDeclarationByName(tmp, varName))
	// If it is not a valid variable name,
	{
		tmp = varName + " not found.";
		print(tmp);
		return false;
	}

	watchTable_[index].name_ = varName;
	watchTable_[index].color_ = color;

	return true;
}

void AgcConsole::registerServerKeyword(std::string keyword)
{
	serverKeyword_.insert(keyword);
}

void AgcConsole::passServerDo()
{
	assert(fptrPassServerDo_);
	assert(pUIconsole_);

	if(commandLine_.length() > 0)
	{
		// Echo the command.	
		print(commandLineIndicator_ + commandLine_);

		// Add to the command history.
		commandBuffer_.push_back(commandLine_);
		if(commandBuffer_.size() > maxHistory_)
			commandBuffer_.pop_front();

		// Reset the history iterator.
		historyIterator_ = commandBuffer_.size();

		if (fptrPassServerDo_)
		{
			fptrPassServerDo_(pUIconsole_, commandLine_);
			print("-- send command to server");
		}
		else
		{
			print(defaultMessage_);
		}

		//@{ Jaewon 20050216
		// Clear the command line.
		commandLine_.clear();
		caretPos_ = 0;
		//@} Jaewon
	}

	// Reset the tab count.
	consecutiveTabs_ = 0;
}

void AgcConsole::setCallbackPtr(AgcmUIConsole* pUIconsole, fPtrCallback fPtr)
{
	fptrPassServerDo_ = fPtr;
	pUIconsole_ = pUIconsole;
}

// Draw the watch list.
void AgcConsole::DrawWatchList() const
{
	// Draw the divider.
	DrawFrame(this->x+30.0f+(visibleExtent_-watchExtent_), this->y+5.0f, 1.0f, this->h-10.0f);

	pAgcmFont_->FontDrawStart(0);

	// Draw items.
	std::string text;
	DWORD color;
	for(size_t i=0; i<visibleLines_; ++i)
	{
		if(watchTable_[i].name_.empty())
		{
			text = "empty";
			color = 0x50ffffff;
		}
		else
		{
			std::string command = "console.bufferForWatching_ = " + watchTable_[i].name_ + ";";
			if(false == ((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getScriptEngine().execute(command))
				text = watchTable_[i].name_ + " = ??";
			else
				text = watchTable_[i].name_ + " = " + bufferForWatching_;
			color = watchTable_[i].color_;
		}
		
		UINT8 alpha = UINT8((color >> 24) & 0xff);
		// Limit the string to the extent.
		size_t length;
		if(length = searchTextLengthOfGivenExtent(text.c_str(), watchExtent_))
			text.at(length) = '\0';
		pAgcmFont_->DrawTextIM2D(this->x+30.0f+(visibleExtent_-watchExtent_)+5.0f, this->y+30.0f+15.0f*i, const_cast<char*>(text.c_str()), 0, alpha, color, true);
	}

	pAgcmFont_->FontDrawEnd();
}

// -----------------------------------------------------------------------------
AgcmUIConsole::AgcmUIConsole()
			: console_(&scriptEngine_, 100, 10), pAgcmUIManager2_(NULL)
{
	SetModuleName("AgcmUIConsole");

	console_.setCallbackPtr(this, sendToServerCommand);
}

// -----------------------------------------------------------------------------
AgcmUIConsole::~AgcmUIConsole()
{
}

// -----------------------------------------------------------------------------
BOOL AgcmUIConsole::OnAddModule()
{
	pAgcmUIManager2_ = (AgcmUIManager2*)GetModule("AgcmUIManager2");
	pAgpmScript_ = (AgpmScript*)GetModule("AgpmScript");
	pAgcmCharacter_ = (AgcmCharacter*)GetModule("AgcmCharacter");

	if (!pAgcmUIManager2_ || !pAgpmScript_ || !pAgcmCharacter_)
		return FALSE;

	if (!pAgpmScript_->SetCallbackKeywords(CBRegisterServerKeyword, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIConsole::OnInit()
{
	bool result;
	// Register the 'testFormatString' function to the script engine.
	result = getScriptEngine().registerGlobalFunctionCdecl("void testFormatString()", testFormatString);
	assert(result);
	// Register the 'testFormatString' keyword to the console.
	getConsole().registerKeyword("testFormatString()");

	// Register the 'exit' function to the script engine.
	result = getScriptEngine().registerGlobalFunctionCdecl("void exit()", exitConsole);
	assert(result);
	// Register the 'exit' keyword to the console.
	getConsole().registerKeyword("exit()");

	// Register the 'print' function to the script engine.
	result = getScriptEngine().registerGlobalFunctionCdecl("void print(string &in)", print);
	assert(result);
	// Register the 'print' keyword to the console.
	getConsole().registerKeyword("print( )");

	result = getScriptEngine().registerGlobalFunctionCdecl("void print(int)", printInt);
	assert(result);	
	result = getScriptEngine().registerGlobalFunctionCdecl("void print(uint)", printUInt);
	assert(result);
	result = getScriptEngine().registerGlobalFunctionCdecl("void print(float)", printFloat);
	assert(result);

	//@{ Jaewon 20050222
	// Register RenderWare render states to the console.
	result = getScriptEngine().registerGlobalFunctionCdecl("uint getRenderStateD3D(uint)", getRenderStateD3D);
	assert(result);
	getConsole().registerKeyword("getRenderStateD3D( )");
	result = getScriptEngine().registerGlobalFunctionCdecl("uint getTextureStageStateD3D(uint, uint)", getTextureStageStateD3D);
	assert(result);
	getConsole().registerKeyword("getTextureStageStateD3D( , )");
	//@} Jaewon

	// Initialize the console.
	console_.init();

	// Initialize the position of the console.
	On();
	console_.moveCenter();
	Off();

	return TRUE;
}

BOOL AgcmUIConsole::OnDestroy()
{
	return TRUE;
}

// -----------------------------------------------------------------------------
void AgcmUIConsole::On()
{
	if(pAgcmUIManager2_)
	{
		pAgcmUIManager2_->AddWindow(&console_);

		g_pEngine->SetMeActiveEdit(&console_);
	}
}

void AgcmUIConsole::Off()
{
	if(pAgcmUIManager2_)
	{
		g_pEngine->ReleaseMeActiveEdit(&console_);

		pAgcmUIManager2_->RemoveWindow(&console_);
	}
}

void AgcmUIConsole::sendToServerCommand(AgcmUIConsole *pUIconsole, std::string command)
{
	ASSERT(pUIconsole);
	INT16 nPacketLength = 0;
	PVOID pvPacket = pUIconsole->pAgpmScript_->MakeScriptPacket(&nPacketLength, 
							AGPMSCRIPT_OPERATION_COMMAND, const_cast<CHAR*>(command.c_str()), pUIconsole->pAgcmCharacter_->GetSelfCharacter()->m_lID);

	if (!pvPacket || nPacketLength < 1)
		return;

	BOOL bResult =  pUIconsole->SendPacket(pvPacket, nPacketLength);
	pUIconsole->pAgpmScript_->m_csPacket.FreePacket(pvPacket);
}

BOOL AgcmUIConsole::CBRegisterServerKeyword(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIConsole* pThis = (AgcmUIConsole*)pClass;
	CHAR* keywords = *(CHAR**)pData;
	UINT16 messageLength = *(UINT16*)pCustData;
	keywords[messageLength] = NULL;
	CHAR seps[]   = ";";
	CHAR *token;

	token = strtok( keywords, seps );
	while( token != NULL )
	{
		pThis->getConsole().registerKeyword(token);
		pThis->getConsole().registerServerKeyword(token);

		token = strtok( NULL, seps );
	}

	return TRUE;
}

void print_ui_console(const std::string& text)
{
	((AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole"))->getConsole().print(text);
}

// -----------------------------------------------------------------------------
// AgcmUIConsole.cpp - End of file
// -----------------------------------------------------------------------------
