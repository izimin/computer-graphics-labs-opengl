//
// Basic class to handle reading of static data
//
// Author: Alex V. Boreskoff
//

#include	<stdio.h>
#ifdef	_WIN32
    
	#include	<fcntl.h>
	#include	<io.h>
#else
    
	#include	<unistd.h>
    
	#include	<sys/types.h>
    
	#include	<sys/stat.h>
    
	#include	<fcntl.h>
    
	#define	O_BINARY	0

#endif

#ifdef	MACOSX
	#include	<stdlib.h>
#else
	#include	<malloc.h>
#endif

#include	<memory.h>
#include	<string.h>
#include	"Data.h"

Data :: Data ( const char * fileName )
{
						// make a fix for windows to replace '/' in file path to 
						// Windoze style '\\' if under windoze
	char * name = strdup ( fileName );

#ifdef	_WIN32
	char * ptr;

	while ( ( ptr = strchr ( name, '/' ) ) != NULL )
		*ptr = '\\';
#endif

	bits      = NULL;
	length    = 0;
	pos       = 0;

	int	file = open ( name, O_RDONLY | O_BINARY );

	free ( name );

	if ( file == -1 )
		return;

#ifndef _WIN32
	struct	stat statBuf;
	
	fstat ( file, &statBuf );
	
	long	len = statBuf.st_size; 
#else	
	long	len = filelength ( file );
#endif

	if ( len == -1 )
	{
		close ( file );

		return;
	}

	bits = (byte *) malloc ( len );

	if ( bits == NULL )
	{
		close ( file );

		return;
	}

	length = read ( file, bits, len );

	close ( file );
}

bool	Data :: isOk () const
{
	return bits != NULL;
}

void * Data :: getPtr ( int offs ) const
{
	if ( offs < 0 || offs >= length )
		return NULL;

	return bits + offs;
}

int	Data :: getBytes ( void * ptr, int len )
{
	if ( pos >= length )
		return -1;

	if ( pos + len > length )
		len = length - pos;

	memcpy ( ptr, bits + pos, len );

	pos += len;

	return len;
}

