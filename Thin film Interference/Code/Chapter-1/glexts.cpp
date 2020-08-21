//
// Sample to print info about OpenGL card and driver
// Lists all supported extensions
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
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>


void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glutSwapBuffers ();
}

void reshape ( int w, int h ) 
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
   glLoadIdentity ();
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ();
}

void key ( unsigned char key, int x, int y )
{
  if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    exit ( 0 );
}

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


bool	isExtensionSupported ( const char * ext )
{
	const char * extensions = (const char *)glGetString ( GL_EXTENSIONS );
	const char * start      = extensions;
	const char * ptr;

	while ( ( ptr = strstr ( start, ext ) ) != NULL )
	{
											// we've found, ensure name is exactly ext
		const char * end = ptr + strlen ( ext );

		if ( isspace ( *end ) || *end == '\0' )
			return true;

		start = end;
	}

	return false;
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );   
	glutInitWindowSize  ( 400, 400 );


								// create window
	glutCreateWindow ( "OpenGL example 1" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key );

	init ();

	const char * vendor    = (const char *)glGetString ( GL_VENDOR     );
	const char * renderer  = (const char *)glGetString ( GL_RENDERER   );
	const char * version   = (const char *)glGetString ( GL_VERSION    );
	const char * extension = (const char *)glGetString ( GL_EXTENSIONS );

	printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

	if ( argc < 2 )				// print a list of extensions
		printExtList ( extension );
	else						// check extensions
		for ( int i = 1; i < argc; i++ )
			if ( isExtensionSupported ( argv [i] ) )
				printf ( "%-40s - supported\n", argv [i] );
			else
				printf ( "%-40s - NOT supported\n", argv [i] );

	return 0;
}
