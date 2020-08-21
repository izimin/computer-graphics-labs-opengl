//
// Sample to to specular per-pixel lighting with Register Combiners in OpenGL
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

#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"Torus.h"
#include	"VertexProgram.h"
#include	"FragmentProgram.h"

Vector3D	eye   ( 2, 2, 2 );			// camera position
Vector3D	light ( 0.5, 0, 1 );		// light position
unsigned	normCubeMap;				// normalization cubemap id
unsigned	bumpMap;					// normal map
unsigned	ambMap;						// ambient texture
unsigned	diffuseMap;					// diffuse texture
unsigned	glossMap;					// gloss (specular) map
float	 	angle = 0;

Vector3D	v  [4];
Vector2D	t0 [4];
Vector3D	t1 [4];

VertexProgram	vertexProgram;
FragmentProgram	fragmentProgram;

/////////////////////////////////////////////////////////////////////////////////

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glDepthFunc  ( GL_LEQUAL );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

												// draw the light
	glMatrixMode ( GL_MODELVIEW );
	glPushMatrix ();

	glTranslatef       ( light.x, light.y, light.z );
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glActiveTextureARB ( GL_TEXTURE3_ARB );
	glDisable          ( GL_TEXTURE_2D );
	glColor4f          ( 1, 1, 1, 1 );
	glutSolidSphere    ( 0.05, 15, 15 );
	glPopMatrix        ();

												// setup texture units
												// bind bump (normal) map to texture unit 0
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, bumpMap );

	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, diffuseMap );

	glActiveTextureARB ( GL_TEXTURE2_ARB );
	glEnable           ( GL_TEXTURE_2D );
	glBindTexture      ( GL_TEXTURE_2D, glossMap );

	glMatrixMode   ( GL_MATRIX0_ARB );
	glLoadIdentity ();

	glMatrixMode   ( GL_MODELVIEW );
	glPushMatrix   ();

	vertexProgram.enable   ();
	vertexProgram.bind     ();
	fragmentProgram.enable ();
	fragmentProgram.bind   ();

	int	i;
												// now draw quad
	glBegin ( GL_QUADS );

	for ( i = 0; i < 4; i++ )
	{
		glMultiTexCoord2fv ( GL_TEXTURE0_ARB, t0 [i] );
		glMultiTexCoord3fv ( GL_TEXTURE1_ARB, Vector3D ( 1, 0, 0 ) );
		glMultiTexCoord3fv ( GL_TEXTURE2_ARB, Vector3D ( 0, 1, 0 ) );
		glNormal3fv        ( Vector3D ( 0, 0, 1 )   );
	  	glVertex3fv        ( v [i]                   );
	}

	glEnd   ();

	vertexProgram.disable   ();
	fragmentProgram.disable ();

	glPopMatrix     ();
	glutSwapBuffers ();
}

void reshape ( int w, int h )
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
   glLoadIdentity ();
   gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 1.0, 60.0 );
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ();
   gluLookAt      ( eye.x, eye.y, eye.z,	// eye
					0, 0, 0,				// center
					0, 0, 1 );				// up
}

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )		//quit requested
    	exit ( 0 );
}

void	animate ()
{
	angle  = 0.004f * glutGet ( GLUT_ELAPSED_TIME );

	light.x = 1.75*cos ( angle );
	light.y = 1.75*sin ( angle );
	light.z = 0.75 + 0.1 * sin ( angle / 3 );

										// setup data
	vertexProgram.local [0] = eye;
	vertexProgram.local [1] = light;

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL per-pixel anisotropic lit quad" );

								// register handlers
								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

	init ();

	printfInfo ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_ARB_vertex_program" ) )
	{
		printf ( "GL_ARB_vertex_program NOT supported" );

		return 3;
	}

	if ( !isExtensionSupported ( "GL_ARB_fragment_program" ) )
	{
		printf ( "GL_ARB_fragment_program NOT supported" );

		return 3;
	}

	initExtensions ();

	v [0] = Vector3D ( -1.3, -1.3, 0 );
	v [1] = Vector3D (  1.3, -1.3, 0 );
	v [2] = Vector3D (  1.3,  1.3, 0 );
	v [3] = Vector3D ( -1.3,  1.3, 0 );

												// setup texture coordinates for bump map
	t0 [0] = Vector2D ( 0, 0 );
	t0 [1] = Vector2D ( 1, 0 );
	t0 [2] = Vector2D ( 1, 1 );
	t0 [3] = Vector2D ( 0, 1 );

	bumpMap     = createTexture2D ( false, "../Textures/TangentMaps/tangent-map-radial.bmp" );
	diffuseMap  = createTexture2D ( true,  "../Textures/16tile01.jpg" );
	glossMap    = createTexture2D ( true,  "../Textures/oxidatedGloss.tga" );

	if ( !vertexProgram.load ( "aniso.vp" ) )
	{
		printf ( "Error loading vertex program:\n %s\n", vertexProgram.getErrorString ().c_str () );

		return 1;
	}

	if ( !fragmentProgram.load ( "aniso.fp" ) )
	{
		printf ( "Error loading fragment program:\n %s\n", fragmentProgram.getErrorString ().c_str () );

		return 2;
	}

	glutMainLoop ();

	return 0;
}
