#pragma once

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuMyFirewallD" )
#else
#pragma comment ( lib , "AcuMyFirewall" )
#endif
#endif

namespace myfirewall
{
	void On();
	void Off();
};