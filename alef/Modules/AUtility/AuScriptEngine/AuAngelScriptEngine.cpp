// -----------------------------------------------------------------------------
//                                           _  _____           _       _   ______             _                                 
//     /\             /\                    | |/ ____|         (_)     | | |  ____|           (_)                                
//    /  \   _   _   /  \   _ __   __ _  ___| | (___   ___ _ __ _ _ __ | |_| |__   _ __   __ _ _ _ __   ___      ___ _ __  _ __  
//   / /\ \ | | | | / /\ \ | '_ \ / _` |/ _ \ |\___ \ / __| '__| | '_ \| __|  __| | '_ \ / _` | | '_ \ / _ \    / __| '_ \| '_ \ 
//  / ____ \| |_| |/ ____ \| | | | (_| |  __/ |____) | (__| |  | | |_) | |_| |____| | | | (_| | | | | |  __/ _ | (__| |_) | |_) |
// /_/    \_\\__,_/_/    \_\_| |_|\__, |\___|_|_____/ \___|_|  |_| .__/ \__|______|_| |_|\__, |_|_| |_|\___|(_) \___| .__/| .__/ 
//                                 __/ |                         | |                      __/ |                     | |   | |    
//                                |___/                          |_|                     |___/                      |_|   |_|    
//
// Angelscript-based script engine
//
// -----------------------------------------------------------------------------
// Originally created on 01/26/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AuAngelScriptEngine.h"
#include "stdstring.h"
#include <cassert>
#include <stdio.h>

// -----------------------------------------------------------------------------
AuAngelScriptEngine::AuAngelScriptEngine()
				: haveToBuild_(false)
{
	pASEngine_ = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	pASEngine_->CreateContext(&pASContext_);

	// Register stl string.
	RegisterStdString(pASEngine_);
}

// -----------------------------------------------------------------------------
AuAngelScriptEngine::~AuAngelScriptEngine()
{
	pASContext_->Release();
	pASEngine_->Release();

	scripts_.clear();
}

// -----------------------------------------------------------------------------
bool AuAngelScriptEngine::addScriptFile(const std::string& filename)
{
	// Load the script.
	FILE *pFile = fopen(filename.c_str(), "rb");
	if(NULL == pFile)
		return false;

	fseek(pFile, 0, SEEK_END);
	long length = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *pScript = new char[length];

	fread(pScript, length, 1, pFile);

	fclose(pFile);

	// Add the script to the list.
	std::string script = pScript;

	addScriptString(script);

	delete [] pScript;

	return true;
}

bool AuAngelScriptEngine::addScriptString(const std::string& source)
{
	// Add the script to the list.
	scripts_.push_back(source);

	// We have to rebuild scripts before running.
	haveToBuild_ = true;

	return true;
}

bool AuAngelScriptEngine::execute(const std::string& command)
{
#pragma warning(disable : 4512)
	class _logStream : public asIOutputStream
	{
	public:
		std::string& log_;
		_logStream(std::string& log) : log_(log) {}

		virtual void Write(const char* text)
		{
			log_ = text;
		}
	} logStream(errorLog_);
#pragma warning(default : 4512)

	if(haveToBuild_)
	{
		// Compile the script.
		int result;
		for(unsigned int i=0; i<scripts_.size(); ++i)
		{
			char buf[32];
			sprintf(buf, "Section%d", i);
			result = pASEngine_->AddScriptSection(0, buf, scripts_[i].c_str(), static_cast<int>(scripts_[i].size()));
			if(result < 0)
				return false;
		}
		result = pASEngine_->Build(0, &logStream);
		if(result < 0)
			return false;

		haveToBuild_ = false;
	}

	int result = pASEngine_->ExecuteString(0, command.c_str(), &logStream, &pASContext_, asEXECSTRING_USE_MY_CONTEXT);
	if(result < 0)
		return false;
	if(pASContext_->GetState() == asEXECUTION_EXCEPTION)
	{
		char line[32];
		sprintf(line, "%d", pASContext_->GetExceptionLineNumber());
		errorLog_ += pASContext_->GetExceptionString();
		errorLog_ += " : "; 
		errorLog_ += pASEngine_->GetFunctionDeclaration(pASContext_->GetExceptionFunction());
		errorLog_ += ", ";
		errorLog_ += line;
		return false;
	}

	return true;
}
// -----------------------------------------------------------------------------
asDWORD AuAngelScriptEngine::getReturnDWord()
{
	assert(pASContext_->GetState() == asEXECUTION_FINISHED);

	return pASContext_->GetReturnDWord();
}
asQWORD AuAngelScriptEngine::getReturnQWord()
{
	assert(pASContext_->GetState() == asEXECUTION_FINISHED);

	return pASContext_->GetReturnQWord();
}
float AuAngelScriptEngine::getReturnFloat()
{
	assert(pASContext_->GetState() == asEXECUTION_FINISHED);

	return pASContext_->GetReturnFloat();
}
double AuAngelScriptEngine::getReturnDouble()
{
	assert(pASContext_->GetState() == asEXECUTION_FINISHED);

	return pASContext_->GetReturnDouble();
}
void *AuAngelScriptEngine::getReturnObject()
{
	assert(pASContext_->GetState() == asEXECUTION_FINISHED);

	return pASContext_->GetReturnObject();
}

// -----------------------------------------------------------------------------
bool AuAngelScriptEngine::registerGlobalVariable(const std::string& declaration, void *pData)
{
	int result = pASEngine_->RegisterGlobalProperty(declaration.c_str(), pData);
	if(result < 0)
		return false;

	return true;
}
bool AuAngelScriptEngine::registerGlobalFunctionCdecl(const std::string& declaration, void *pFunction)
{
	int result = pASEngine_->RegisterGlobalFunction(declaration.c_str(), asFUNCTION(pFunction), asCALL_CDECL);
	if(result < 0)
		return false;

	return true;
}
bool AuAngelScriptEngine::registerGlobalFunctionStdcall(const std::string& declaration, void *pFunction)
{
	int result = pASEngine_->RegisterGlobalFunction(declaration.c_str(), asFUNCTION(pFunction), asCALL_STDCALL);
	if(result < 0)
		return false;

	return true;
}

bool AuAngelScriptEngine::registerObjectType(const std::string& name, int byteSize)
{
	int result = pASEngine_->RegisterObjectType(name.c_str(), byteSize, asOBJ_CLASS);
	if(result < 0)
		return false;

	return true;
}
bool AuAngelScriptEngine::registerObjectVariable(const std::string& obj, const std::string& declaration, int byteOffset)
{
	int result = pASEngine_->RegisterObjectProperty(obj.c_str(), declaration.c_str(), byteOffset);
	if(result < 0)
		return false;

	return true;
}
bool AuAngelScriptEngine::registerObjectMethod(const std::string& obj, const std::string& declaration, asUPtr pFunction)
{
	int result = pASEngine_->RegisterObjectMethod(obj.c_str(), declaration.c_str(), pFunction, asCALL_THISCALL);
	if(result < 0)
		return false;

	return true;
}

//@{ Jaewon 20050218
// Query a variable declaration from its name.
bool AuAngelScriptEngine::getVariableDeclarationByName(std::string& declaration, const std::string& name)
{
	int id = pASEngine_->GetGlobalVarIDByName(0, name.c_str());
	if(id < 0)
		return false;

	declaration = pASEngine_->GetGlobalVarDeclaration(id);
	return true;
}
// Query a function declaration from its name.
bool AuAngelScriptEngine::getFunctionDeclarationByName(std::string& declaration, const std::string& name)
{
	int id = pASEngine_->GetFunctionIDByName(0, name.c_str());
	if(id < 0)
		return false;

	declaration = pASEngine_->GetFunctionDeclaration(id);
	return true;
}
//@} Jaewon

// -----------------------------------------------------------------------------
// AuAngelScriptEngine.cpp - End of file
// -----------------------------------------------------------------------------
