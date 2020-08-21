//
// Interface of a class to access data (virtual file system)
//

#ifndef	__FILE_SYSTEM__
#define	__FILE_SYSTEM__

#include	<string>

using namespace std;

class	Data;

class	FileSystem
{
public:
	FileSystem () {}
	virtual ~FileSystem () {}

	virtual	bool	isOk () const
	{
		return false;
	}

	virtual	Data  * getFile  ( const string& name ) = 0;
};

#endif
