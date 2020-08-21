//
// Sample to to check for vertex program support in OpenGL card and driver
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	"libExt.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>

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

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 400, 400 );


								// create window
	glutCreateWindow ( "OpenGL VP info" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
    glutKeyboardFunc ( key   );

	init           ();
	initExtensions ();

	const char * vendor    = (const char *)glGetString ( GL_VENDOR     );
	const char * renderer  = (const char *)glGetString ( GL_RENDERER   );
	const char * version   = (const char *)glGetString ( GL_VERSION    );
	const char * extension = (const char *)glGetString ( GL_EXTENSIONS );

	printf ( "Vendor:   %s\nRenderer: %s\nVersion:  %s\n", vendor, renderer, version );

	if ( !isExtensionSupported ( "GL_ARB_vertex_program" ) )
	{
		printf ( "GL_ARB_vertex_program extension NOT supported.\n" );

		return 1;
	}

	printf ( "ARB_vertex_program extension is supported !\n" );

    GLint	maxVertexAttribs;
    GLint	maxLocalParams;
    GLint	maxEnvParams;
    GLint 	maxMatrices;
    GLint	maxTemporaries;
    GLint	maxParams;
    GLint	maxAddressRegs;

    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_VERTEX_ATTRIBS_ARB,            &maxVertexAttribs );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB,  &maxLocalParams   );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,    &maxEnvParams     );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_MATRICES_ARB,          &maxMatrices      );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB,       &maxTemporaries   );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB,        &maxParams        );
    glGetProgramivARB ( GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB, &maxAddressRegs   );

	printf ( "\nVertex program limits:\n"                      );
	printf ( "\tmax vertex attributes  : %d\n", maxVertexAttribs );
	printf ( "\tmax local parameters   : %d\n", maxLocalParams   );
	printf ( "\tmax env. parameters    : %d\n", maxEnvParams     );
	printf ( "\tmax program matrices   : %d\n", maxMatrices      );
	printf ( "\tmax program temporaries: %d\n", maxTemporaries   );
	printf ( "\tmax parameters         : %d\n", maxParams        );
	printf ( "\tmax address registers  : %d\n", maxAddressRegs   );

	return 0;
}
