// -----------------------------------------------------------------------------
//                  _____           _       _   ______             _                _     
//     /\          / ____|         (_)     | | |  ____|           (_)              | |    
//    /  \   _   _| (___   ___ _ __ _ _ __ | |_| |__   _ __   __ _ _ _ __   ___    | |__  
//   / /\ \ | | | |\___ \ / __| '__| | '_ \| __|  __| | '_ \ / _` | | '_ \ / _ \   | '_ \ 
//  / ____ \| |_| |____) | (__| |  | | |_) | |_| |____| | | | (_| | | | | |  __/ _ | | | |
// /_/    \_\\__,_|_____/ \___|_|  |_| .__/ \__|______|_| |_|\__, |_|_| |_|\___|(_)|_| |_|
//                                   | |                      __/ |                       
//                                   |_|                     |___/                        
//
// Script engine interface
//
// -----------------------------------------------------------------------------
// Originally created on 01/26/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AUSCRIPTENGINE
#define _H_AUSCRIPTENGINE

// 2006.02.16. steeple
// ignore deprecated warning message in VS2005
#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

#include <string>

// -----------------------------------------------------------------------------

class AuScriptEngine
{
public:
	// Construction/Destruction
	AuScriptEngine();
	virtual	~AuScriptEngine();

	// Operators

	// Accessors

	// Interface methods
	// Add a script file(so, its functions & variables) to the engine.
	virtual bool addScriptFile(const std::string& filename) = 0;
	virtual bool addScriptString(const std::string& source) = 0;
	// Execute a script command.
	virtual bool execute(const std::string& command) = 0;

private:
	// Data members

	// Implementation methods

};

#endif // _H_AUSCRIPTENGINE
// -----------------------------------------------------------------------------
// AuScriptEngine.h - End of file
// -----------------------------------------------------------------------------

