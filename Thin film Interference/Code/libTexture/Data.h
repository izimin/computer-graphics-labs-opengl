//
// Basic class to handle reading of static data
//
// Author: Alex V. Boreskoff
//

#ifndef	__DATA__
#define	__DATA__

#include	"TypeDefs.h"

class	Data
{
	byte  * bits;
	int		length;
	int		pos;

public:
	Data ( const char * fileName );
	Data ( void * ptr, int len )
	{
		bits      = (byte *) ptr;
		length    = len;
		pos       = 0;
	}

	bool	isOk () const;

	bool	isEmpty () const
	{
		return pos >= length;
	}

	int	getLength () const
	{
		return length;
	}

	int	getByte ()
	{
		if ( pos < length )
			return bits [pos++];
		else
			return -1;
	}

	int16	getShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		int16 	v = *(int16 *) (bits + pos);

		pos += 2;

		return v;
	}

	uint16	getUnsignedShort ()
	{
		if ( pos + 1 >= length )
			return -1;

		uint16 v = *(uint16 *) (bits + pos);

		pos += 2;

		return v;
	}

	long getLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		long 	v = *(long *) (bits + pos);

		pos += 4;

		return v;
	}

	dword getUnsignedLong ()
	{
		if ( pos + 3 >= length )
			return -1;

		dword v = *(dword *) (bits + pos);

		pos += 4;

		return v;
	}

	void * getPtr () const
	{
		return bits + pos;
	}

	void * getPtr ( int offs ) const;

	int	seekCur ( int delta )
	{
		pos += delta;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}

	int	seekAbs ( int offs )
	{
		pos = offs;

		if ( pos > length )
			pos = length;

		if ( pos < 0 )
			pos = 0;

		return pos;
	}
 
	int		getBytes  ( void * ptr, int len );
};

#endif
