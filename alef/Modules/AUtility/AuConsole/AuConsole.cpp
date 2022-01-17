// -----------------------------------------------------------------------------
//                  _____                       _                           
//     /\          / ____|                     | |                          
//    /  \   _   _| |      ___  _ __  ___  ___ | | ___      ___ _ __  _ __  
//   / /\ \ | | | | |     / _ \| '_ \/ __|/ _ \| |/ _ \    / __| '_ \| '_ \ 
//  / ____ \| |_| | |____| (_) | | | \__ \ (_) | |  __/ _ | (__| |_) | |_) |
// /_/    \_\\__,_|\_____|\___/|_| |_|___/\___/|_|\___|(_) \___| .__/| .__/ 
//                                                             | |   | |    
//                                                             |_|   |_|    
//
// Angelscript console system base class
//
// -----------------------------------------------------------------------------
// Originally created on 01/26/2005 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AuConsole.h"
#include <ctype.h>
#include <sstream>

// -----------------------------------------------------------------------------
AuConsole::AuConsole(AuScriptEngine *pScriptEngine, unsigned int maxLines, unsigned int maxHistory)
		: pScriptEngine_(pScriptEngine), caretPos_(0), historyIterator_(0), consecutiveTabs_(0)
{
	maxLines_ = maxLines ? maxLines : 1;
	maxHistory_ = maxHistory;
	textBuffer_.push_back("");
}

// -----------------------------------------------------------------------------
AuConsole::~AuConsole()
{
	commandBuffer_.clear();
	textBuffer_.clear();
}

// -----------------------------------------------------------------------------
void AuConsole::print(const std::string& text)
{
	std::istringstream stream(text);
	std::string buf;
	do
	{
		// For each line,
		getline(stream, buf);
		std::istringstream stream2(buf);
		std::string token;
		std::ostringstream output;
		do
		{
			// Format the line by tabs.
			getline(stream2, token, stream2.widen('\t'));
			size_t length = token.length();
			if(length==0 && stream2.eof())
				break;
			size_t numOfSpaces = 4 - length % 4;
			output << token;
			while(numOfSpaces)
			{
				output << " ";
				--numOfSpaces;
			}
		}
		while(!stream2.eof());
		// Add the tabbed string to the buffer. 
		textBuffer_.push_back(output.str());
		if(textBuffer_.size() > maxLines_)
			textBuffer_.pop_front();
	}
	while(!stream.eof());
}

void AuConsole::passKey(char key)
{
	commandLine_.insert(caretPos_, 1, key);
	++caretPos_;

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passBackspace()
{
	if(caretPos_ > 0)
	{
		--caretPos_;
		commandLine_= commandLine_.erase(caretPos_, 1);
	}

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passDo()
{
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

		// Execute.
		bool result = pScriptEngine_->execute(commandLine_);

		// If the execution was failed,
		if(false == result)
		{
			// Report it to the user properly.
			if(defaultCommand_.empty())
				if(!defaultMessage_.empty())
					print(defaultMessage_);
			else
				pScriptEngine_->execute(defaultCommand_);
		}
		// Return the result, if any.
		//else
		//{
		//}

		//@{ Jaewon 20050216
		// Clear the command line.
		commandLine_.clear();
		caretPos_ = 0;
		//@} Jaewon
	}

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passLeft()
{
	if(caretPos_ > 0)
		--caretPos_;

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passRight()
{
	if(caretPos_ < commandLine_.length())
		++caretPos_;

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passUp()
{
	if(historyIterator_ > 0)
	{
		--historyIterator_;
		commandLine_ = commandBuffer_[historyIterator_];
		
		// Move caret to end.
		caretPos_ = commandLine_.length();
	}

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passDown()
{
	if(historyIterator_ < commandBuffer_.size())
	{
		++historyIterator_;
		if(historyIterator_ == commandBuffer_.size())
			commandLine_ = "";
		else
			commandLine_ = commandBuffer_[historyIterator_];

		// Move caret to end.
		caretPos_ = commandLine_.length();
	}

	// Reset the tab count.
	consecutiveTabs_ = 0;
}
void AuConsole::passTab()
{
	// Get a prefix.
	std::string prefix;
	if(consecutiveTabs_ == 0)
	{
		//@{ Jaewon 20050221
		// Improve auto-completion by caret-based auto-completing.
		if(caretPos_ == 0
			|| isspace(commandLine_[caretPos_-1]))
			return;
		autoCompletionPos_ = caretPos_-1;
		while(autoCompletionPos_>0 && !isspace(commandLine_[autoCompletionPos_-1]))
			--autoCompletionPos_;
		autoCompletionPrefix_ = commandLine_.substr(autoCompletionPos_, caretPos_-autoCompletionPos_);
		//@} Jaewon
	}

	// Find a matching keyword.
	std::vector<std::string> matches;
	for(unsigned int i=0; i<keywords_.size(); ++i)
	{
		if(keywords_[i].compare(0, autoCompletionPrefix_.length(), autoCompletionPrefix_) == 0)
			matches.push_back(keywords_[i]);
	}

	if(matches.size() == 0)
		return;

	//@{ Jaewon 20050221
	// Improve auto-completion by caret-based auto-completing.
	// Complete a word.
	std::string tmp = commandLine_.substr(0, autoCompletionPos_) + matches[consecutiveTabs_ % matches.size()];
	commandLine_ = tmp + commandLine_.substr(caretPos_);
	// Move caret properly.
	caretPos_ = tmp.length();
	//@} Jaewon

	++consecutiveTabs_;
}

// -----------------------------------------------------------------------------
// AuConsole.cpp - End of file
// -----------------------------------------------------------------------------
