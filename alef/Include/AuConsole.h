// -----------------------------------------------------------------------------
//                  _____                       _          _     
//     /\          / ____|                     | |        | |    
//    /  \   _   _| |      ___  _ __  ___  ___ | | ___    | |__  
//   / /\ \ | | | | |     / _ \| '_ \/ __|/ _ \| |/ _ \   | '_ \ 
//  / ____ \| |_| | |____| (_) | | | \__ \ (_) | |  __/ _ | | | |
// /_/    \_\\__,_|\_____|\___/|_| |_|___/\___/|_|\___|(_)|_| |_|
//                                                               
//                                                               
//
// Console system base class
//
// -----------------------------------------------------------------------------
// Originally created on 01/26/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AUCONSOLE
#define _H_AUCONSOLE

#include "AuScriptEngine.h"
#include <string>
#include <deque>
#include <vector>

// -----------------------------------------------------------------------------

class AuConsole
{
public:
	// Construction/Destruction
	AuConsole(AuScriptEngine *pScriptEngine, unsigned int maxLines, unsigned int maxHistory);
	virtual	~AuConsole();

	// Operators

	// Accessors

	// Interface methods
	void setDefaultCommand(const std::string& command)
	{ defaultCommand_ = command; }
	void setDefaultMessage(const std::string& message)
	{ defaultMessage_ = message; }
	void setCommandLineIndicator(const std::string& indicator)
	{ commandLineIndicator_= indicator; }

	// Print a text to console.
	void print(const std::string& text);

	// Insert a character.
	void passKey(char key);
	// Delete a character.
	void passBackspace();
	// Enter a command. 
	void passDo();
	// Move caret to left.
	void passLeft();
	// Move caret to right.
	void passRight();
	// Traverse command history to older.
	void passUp();
	// Traverse command history to newer.
	void passDown();
	// Auo-complete a keyword.
	void passTab();

	void registerKeyword(const std::string& keyword)
	{ keywords_.push_back(keyword); }

	virtual void render() = 0;

protected:
	// Data members
	// Script engine for the command execution
	AuScriptEngine *pScriptEngine_;
	// Command history
	std::deque<std::string> commandBuffer_;
	// A command that will be executed on error
	std::string defaultCommand_;
	// A message that will be displayed on error and if no default command has been specified
	std::string defaultMessage_;
	// Maximum length of the command history
	unsigned int maxHistory_;
	// Command history iterator
	size_t historyIterator_;
	// Registered keywords for auto-completion
	std::vector<std::string> keywords_;
	// Consecutive tabs count
	// When there are many possible matching keywords, 
	// you can use consecutive tabs to traverse them.
	unsigned int consecutiveTabs_;
	// Saved auto-completion prefix for consecutive tabs
	std::string autoCompletionPrefix_;
	// Saved auto-completion prefix position for consecutive tabs
	size_t autoCompletionPos_;

	// Implementation methods

protected:
	// Maximum line count of the text buffer
	unsigned int maxLines_;
	std::deque<std::string> textBuffer_;
	// A buffer for the current command line
	std::string commandLine_;
	// The current position of carret
	size_t caretPos_;
	// Command line indicator
	std::string commandLineIndicator_;
};

#endif // _H_AUCONSOLE
// -----------------------------------------------------------------------------
// AuConsole.h - End of file
// -----------------------------------------------------------------------------

