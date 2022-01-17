// -----------------------------------------------------------------------------
//                                           _  _____           _       _   ______             _                _     
//     /\             /\                    | |/ ____|         (_)     | | |  ____|           (_)              | |    
//    /  \   _   _   /  \   _ __   __ _  ___| | (___   ___ _ __ _ _ __ | |_| |__   _ __   __ _ _ _ __   ___    | |__  
//   / /\ \ | | | | / /\ \ | '_ \ / _` |/ _ \ |\___ \ / __| '__| | '_ \| __|  __| | '_ \ / _` | | '_ \ / _ \   | '_ \ 
//  / ____ \| |_| |/ ____ \| | | | (_| |  __/ |____) | (__| |  | | |_) | |_| |____| | | | (_| | | | | |  __/ _ | | | |
// /_/    \_\\__,_/_/    \_\_| |_|\__, |\___|_|_____/ \___|_|  |_| .__/ \__|______|_| |_|\__, |_|_| |_|\___|(_)|_| |_|
//                                 __/ |                         | |                      __/ |                       
//                                |___/                          |_|                     |___/                        
//
// Angelscript-based script engine
//
// -----------------------------------------------------------------------------
// Originally created on 01/26/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AUANGELSCRIPTENGINE
#define _H_AUANGELSCRIPTENGINE

#include "AuScriptEngine.h"
#include "angelscript.h"
#include <vector>

// -----------------------------------------------------------------------------

class AuAngelScriptEngine : public AuScriptEngine
{
public:
	// Construction/Destruction
	AuAngelScriptEngine();
	virtual	~AuAngelScriptEngine();

	// Operators

	// Accessors

	// Interface methods
	// Add a script file(so, its functions & variables) to the engine.
	virtual bool addScriptFile(const std::string& filename);
	virtual bool addScriptString(const std::string& source);
	// Execute a script command.
	virtual bool execute(const std::string& command);

	// Get a return value.
	asDWORD getReturnDWord();
	asQWORD getReturnQWord();
	float getReturnFloat();
	double getReturnDouble();
	void *getReturnObject();

	// Register globals of the host application.
	bool registerGlobalVariable(const std::string& declaration, void *pData);
	bool registerGlobalFunctionCdecl(const std::string& declaration, void *pFunction);
	bool registerGlobalFunctionStdcall(const std::string& declaration, void *pFunction);

	// Register objects of the host application & its members.
	bool registerObjectType(const std::string& name, int byteSize);
	bool registerObjectVariable(const std::string& obj, const std::string& declaration, int byteOffset);
	bool registerObjectMethod(const std::string& obj, const std::string& declaration, asUPtr pFunction);

	//@{ Jaewon 20050218
	// Query a variable declaration from its name.
	bool getVariableDeclarationByName(std::string& declaration, const std::string& name);
	// Query a function declaration from its name.
	bool getFunctionDeclarationByName(std::string& declaration, const std::string& name);
	//@} Jaewon

	// Get the last error.
	void getLastError(std::string& errorLog) const
	{ errorLog = errorLog_; }
private:
	// Data members
	// Angelscript engine
	asIScriptEngine *pASEngine_;
	// Angelscript context - common running environment
	asIScriptContext *pASContext_;
	// Added scripts for building
	std::vector<std::string> scripts_;
	// Do we have to rebuild scripts?
	bool haveToBuild_;
	// Last error log
	std::string errorLog_;

	// Implementation methods

};

#endif // _H_AUANGELSCRIPTENGINE
// -----------------------------------------------------------------------------
// AuAngelScriptEngine.h - End of file
// -----------------------------------------------------------------------------

