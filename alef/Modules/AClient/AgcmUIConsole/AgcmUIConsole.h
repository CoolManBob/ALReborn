// -----------------------------------------------------------------------------
//                                _    _ _____  _____                       _          _     
//     /\                        | |  | |_   _|/ ____|                     | |        | |    
//    /  \    __ _  ___ _ __ ___ | |  | | | | | |      ___  _ __  ___  ___ | | ___    | |__  
//   / /\ \  / _` |/ __| '_ ` _ \| |  | | | | | |     / _ \| '_ \/ __|/ _ \| |/ _ \   | '_ \ 
//  / ____ \| (_| | (__| | | | | | |__| |_| |_| |____| (_) | | | \__ \ (_) | |  __/ _ | | | |
// /_/    \_\\__, |\___|_| |_| |_|\____/|_____|\_____|\___/|_| |_|___/\___/|_|\___|(_)|_| |_|
//            __/ |                                                                          
//           |___/                                                                           
//
// Angelscript-based script engine
//
// -----------------------------------------------------------------------------
// Originally created on 02/15/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCMUICONSOLE
#define _H_AGCMUICONSOLE

#include "AgcModule.h"
#include "AgcEngine.h"
#include "AgcmUIManager2.h"
#include "AgpmScript.h"
#include "AuConsole.h"
#include "AuAngelScriptEngine.h"

#include <set>

// -----------------------------------------------------------------------------
#define AS_REGISTER_TYPE_BEGIN(type, name)																\
	do																									\
	{																									\
		AgcmUIConsole *pAgcmUIConsole = (AgcmUIConsole*) g_pEngine->GetModule("AgcmUIConsole");			\
		if(pAgcmUIConsole)																				\
		{																								\
			typedef type myType;																		\
			std::string typeString = #type;																\
			std::string nameString = #name;																\
			std::string tmpString;																		\
			bool result;																				\
			result = pAgcmUIConsole->getScriptEngine().registerObjectType(typeString, sizeof(myType));	\
			ASSERT(result);																				\
			result = pAgcmUIConsole->getScriptEngine().registerGlobalVariable(#type " " #name, this);	\
			ASSERT(result);																				\
			pAgcmUIConsole->getConsole().registerKeyword(#name ".")

#define AS_REGISTER_VARIABLE(type, name)																							\
			result = pAgcmUIConsole->getScriptEngine().registerObjectVariable(typeString, #type " " #name, offsetof(myType,name));	\
			ASSERT(result);																											\
			tmpString = nameString + "." + #name;																					\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD0(type, name)																									\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "()", asMETHOD(myType,name));	\
			ASSERT(result);																												\
			tmpString = nameString + "." + #name + "()";																				\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD1(type, name, param0)																										\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 ")", asMETHOD(myType,name));	\
			ASSERT(result);																															\
			tmpString = nameString + "." + #name + "( )";																							\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD2(type, name, param0, param1)																											\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 ")", asMETHOD(myType,name));	\
			ASSERT(result);																																		\
			tmpString = nameString + "." + #name + "( , )";																										\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD3(type, name, param0, param1, param2)																												\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 ")", asMETHOD(myType,name));	\
			ASSERT(result);																																					\
			tmpString = nameString + "." + #name + "( , , )";																												\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)


#define AS_REGISTER_METHOD4(type, name, param0, param1, param2, param3)																													\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 "," #param3 ")", asMETHOD(myType,name));	\
			ASSERT(result);																																								\
			tmpString = nameString + "." + #name + "( , , , )";																															\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD5(type, name, param0, param1, param2, param3, param4)																														\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 "," #param3 "," #param4 ")", asMETHOD(myType,name));	\
			ASSERT(result);																																											\
			tmpString = nameString + "." + #name + "( , , , , )";																																	\
			pAgcmUIConsole->getConsole().registerKeyword(tmpString)

#define AS_REGISTER_METHOD_HIDE(type, name)																														\
			result = pAgcmUIConsole->getScriptEngine().registerObjectMethod(typeString, #type " " #name "()", asMETHOD(myType,name));	\
			ASSERT(result);		

#define AS_REGISTER_TYPE_END	\
		}						\
	} while(0)

// -----------------------------------------------------------------------------
class AgcmUIConsole;
class AgcConsole : public AgcWindow, public AuConsole
{
public:
	typedef void (*fPtrCallback)(AgcmUIConsole*, std::string);
	AgcConsole(AuScriptEngine *pScriptEngine, unsigned int maxLines, unsigned int maxHistory);
	virtual ~AgcConsole();

	BOOL OnInit();
	VOID OnWindowRender();
	BOOL OnKeyDown(RsKeyStatus *ks);
	BOOL OnKeyUp(RsKeyStatus *ks);
	BOOL OnChar(char *keyCode, UINT lParam);
	BOOL OnLButtonDown(RsMouseStatus *ms);

	// Register methods & variables to the script engine.
	void init();

	// Move console to the top-left corner of the parent.
	void moveTL();
	// Move console to the top-right corner of the parent.
	void moveTR();
	// Move console to the bottom-left corner of the parent.
	void moveBL();
	// Move console to the bottom-right corner of the parent.
	void moveBR();
	// Move console to the center of the parent.
	void moveCenter();

	// Register a variable to the watch list.
	bool registerWatch(size_t index, const std::string& varName, DWORD color);

	// Register a keyword
	void registerServerKeyword(std::string keyword);
	// enter a server-command
	void passServerDo();

	void setCallbackPtr(AgcmUIConsole* pUIconsole, fPtrCallback fPtr);

	// Show watch table.
	void showWatch()
	{ showWatch_ = true; }
	// Hide watch table.
	void hideWatch()
	{ showWatch_ = false; }
	// Change the Watch Width
	void setWatchWidth( size_t uWidth )
	{ watchExtent_ = uWidth; }
private:
	// Font for drawing
	AgcmFont* pAgcmFont_;
	// Count of visible lines
	size_t visibleLines_;
	// Pixel width of visible extent
	size_t visibleExtent_;
	// Scroll bar
	size_t scrollBar_;

	// Pixel width of watch extent
	size_t watchExtent_;
	// A flag telling whether currently watches should be displayed or not. 
	bool showWatch_;
	// A list of registered watch variables 
	struct Watch
	{
		std::string name_;
		DWORD color_;
	};
	Watch watchTable_[30];
	// This string buffer is registered to the console,
	// and used to get the value of a watched variable.
	std::string bufferForWatching_;

	// Server keyword
	std::set<std::string> serverKeyword_;
	AgcmUIConsole *pUIconsole_;
	fPtrCallback fptrPassServerDo_;

	virtual void render();

	// If the whole text is within the extent, this returns 0.
	// Otherwise it returns a proper string length.
	size_t searchTextLengthOfGivenExtent(const char *text, size_t extent) const;
	size_t getNumberOfLines(const char *text, size_t extent) const;
	// Parameter 'line' is 1-based.
	bool isVisibleLine(size_t line) const;
	void DrawOneLine(size_t line, const char* text, size_t length);
	// Draw window frame.
	void DrawFrame(float x, float y, float w, float h) const;

	// Get the current pixel width of the text buffer.
	size_t getTextExtent() const
	{ return showWatch_?(visibleExtent_-watchExtent_-5):visibleExtent_; }

	// Draw the watch list.
	void DrawWatchList() const;
};

// -----------------------------------------------------------------------------

class AgcmUIConsole : public AgcModule
{
public:
	// Construction/Destruction
	AgcmUIConsole();
	virtual	~AgcmUIConsole();

	// Operators

	// Accessors

	// Interface methods
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();

	void On();
	void Off();

	AgcConsole& getConsole()
	{ return console_; }

	AuAngelScriptEngine& getScriptEngine()
	{ return scriptEngine_; }

	static void sendToServerCommand(AgcmUIConsole *pUIconsole, std::string command);

	static BOOL CBRegisterServerKeyword(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	// Data members
	AuAngelScriptEngine scriptEngine_;
	AgcConsole console_;
	AgcmUIManager2 *pAgcmUIManager2_;
	AgpmScript *pAgpmScript_;
	AgcmCharacter *pAgcmCharacter_;
	// Implementation methods

};
// 마고자 (2005-10-26 오후 4:59:16) : 여러군데에서 쓸려고 펑션 추가.
void print_ui_console(const std::string& text);

#endif // _H_AGCMUICONSOLE
// -----------------------------------------------------------------------------
// AgcmUIConsole.h - End of file
// -----------------------------------------------------------------------------

