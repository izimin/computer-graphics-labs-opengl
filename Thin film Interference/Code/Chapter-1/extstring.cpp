//
// Sample to print string with a list of all supported extensions
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#ifdef	_WIN32
	#include	<windows.h>
#endif

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<GL/gl.h>

	#include	"glut.h"
#endif

#include	"../glext.h"
#include	<stdio.h>
#include	<ctype.h>

void	printExtList ( const char * extension )
{
	char	name [1024];					// hope this will be enough
	int	i, j;

	printf ( "Supported extensions:\n" );

	for ( i = 0, j = 0; extension [i] != '\0'; i++ )
		if ( !isspace ( extension [i] ) )
			name [j++] = extension [i];
		else								// end of name
		{
			name [j] = '\0';				// terminate name

			printf ( "\t%s\n", name );

			j = 0;
		}

	if ( j > 0 )
	{
		name [j] = '\0';

		printf ( "\t%s\n", name );
	}
}


int main ( int argc, char * argv [] )
{
							// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );   
	glutInitWindowSize  ( 400, 400 );


							// create window
	glutCreateWindow ( "OpenGL example 1" );

	const char * vendor    = (const char *)glGetString ( GL_VENDOR     );
	const char * renderer  = (const char *)glGetString ( GL_RENDERER   );
	const char * version   = (const char *)glGetString ( GL_VERSION    );
	const char * extension = (const char *)glGetString ( GL_EXTENSIONS );

	printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

	printExtList ( extension );

	return 0;
}
