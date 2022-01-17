#ifndef MCI_EVENTS_H
#define MCI_EVENTS_H

#include <string>

struct MethodInfo;

struct IMciEvents
{
	virtual void OnEnter(const std::string & filename, int line, const MethodInfo & mi) {}
	virtual void OnLeave(const std::string & filename, int line, const MethodInfo & mi) {}
};

#endif
