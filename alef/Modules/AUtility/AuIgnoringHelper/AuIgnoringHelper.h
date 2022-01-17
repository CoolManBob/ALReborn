#pragma once

class AuIgnoringHelper
{
public:
	~AuIgnoringHelper();

	static bool IgnoreToFile( char * file );
	static AuIgnoringHelper & Instance();

private:
	AuIgnoringHelper();

	static bool allow_;
};