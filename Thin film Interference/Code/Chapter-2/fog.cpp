//
// Simple demo for GL_EXT_fog_coord OpenGL extension
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru
//

#include	"libExt.h"
#include	"libTexture.h"
#include	"Vector3D.h"
#include	"Vector2D.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>

#define	N1	90
#define	N2	20

struct	Facet
{
	int	 index [3];					// indices into vertex, normal and tex arrays
};

Vector3D	vertex [N1 * N2];
Vector3D	normal [N1 * N2];
Vector2D	tex    [N1 * N2];
Facet		facet  [N1 * N2 * 2];

float	 angle  = 0;
float	 angle2 = 0;
unsigned texture;					// main texture

Vector3D	eye      ( 0, 0, 0  );
Vector3D	lookAt   ( 0, -5, 5 );
Vector3D	up       ( 0, 1, 0  );
Vector3D	fogColor ( 0, 0.7f, 0.8f );

inline	Vector3D	knot1D ( float t )
{
	float	r   = 1.8 + 0.8 * cos ( 3*t );
	float	phi = 0.2 * M_PI * sin ( 3*t );

	return r * Vector3D ( cos ( phi ) * sin ( 2*t ), cos ( phi ) * cos ( 2*t ), sin ( phi ) );
}

inline	Vector3D	knot ( float u, float v, Vector3D& n )
{
	Vector3D	t, b;

	t = (knot1D (u + 0.01) - knot1D (u - 0.01)).normalize ();
	b = (t ^ Vector3D ( 0, 0, 1 )).normalize ();
	n =  t ^ b;

	return knot1D ( u ) + 0.55 * ( sin (v) * b + cos (v) * n );
}

void	initKnot ()
{
    int	i, j, k;
					// 1. Create vertices
	for ( i = 0, k = 0; i < N1; i++ )
	{
		float	phi = i * 2 * M_PI / N1;

		for ( j = 0; j < N2; j++, k++ )
		{
			float	psi = j * 2 * M_PI / N2;

											// compute vertex coordinates
			vertex [k] = knot ( phi, psi, normal [k] );

											// compute texture coordinates
			tex [k].x = (float) i / (float) N1;
			tex [k].y = (float) j / (float) N2;
		}
	}

					// 2. Create facets
	for ( i = k = 0; i < N1; i++ )
		for ( j = 0; j < N2; j++, k += 2 )
		{
			facet [k].index [0] = i*N2 + j;
			facet [k].index [1] = ((i+1) % N1)*N2 + j;
			facet [k].index [2] = ((i+1) % N1)*N2 + (j + 1) % N2;

			facet [k+1].index [0] = i*N2 + j;
			facet [k+1].index [1] = ((i+1) % N1)*N2 + (j + 1) % N2;
			facet [k+1].index [2] = i*N2 + (j + 1) % N2;
		}
}

void	draw ()
{
	float	mvMatrix [16];

						// get modelview matrix to transform back
	glGetFloatv ( GL_MODELVIEW_MATRIX, mvMatrix );


	glBegin ( GL_TRIANGLES );

	for ( int i = 0; i < N1 * N2 * 2; i++ )
		for ( int j = 0; j < 3; j++ )
		{
			int 	k    = facet [i].index [j];
			Vector3D v   = vertex [k];
			float    fog = -v.x * mvMatrix [1] - v.y * mvMatrix [5] - v.z * mvMatrix [9] - mvMatrix [13];

			glFogCoordf   ( fog        );
			glNormal3fv   ( normal [k] );
			glTexCoord2fv ( tex    [k] );
			glVertex3fv   ( vertex [k] );
		}

	glEnd ();
}

void init ()
{
	glClearColor  ( 0, 0, 0, 1 );
	glEnable      ( GL_DEPTH_TEST );
	glDepthFunc   ( GL_LEQUAL );
	glEnable      ( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, texture );

											// setup fog
	glEnable ( GL_FOG                 );
	glFogi   ( GL_FOG_MODE, GL_LINEAR );
	glFogfv  ( GL_FOG_COLOR, fogColor );
	glFogf   ( GL_FOG_START, -4       );
	glFogf   ( GL_FOG_END,   3        );
	glFogi   ( GL_FOG_HINT, GL_NICEST );
	glFogi   ( GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT );
}

void display ()
{
	glClear         ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glPushMatrix    ();
	glTranslatef    ( 0, -7, 7 );
	glRotatef       ( angle,  1, 1, 0 );
	glRotatef       ( angle2, 0, 1, 1 );

	draw ();

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
   gluLookAt      ( eye.x, eye.y, eye.z,			// eye
					lookAt.x, lookAt.y, lookAt.z,	// center
					up.x, up.y, up.z );
}

void key ( unsigned char key, int x, int y )
{
    if ( key == 27 || key == 'q' || key == 'Q' )	// quit requested
        exit ( 0 );

	if ( key == 'p' || key == 'P' )					// saving screenshots
		saveScreenShot ( "fog.tga" );
}

void	animate ()
{
	angle  = 0.04f * glutGet ( GLUT_ELAPSED_TIME );
	angle2 = 0.02f * glutGet ( GLUT_ELAPSED_TIME );

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	int	win = glutCreateWindow ( "OpenGL fog_coord example" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key );
	glutIdleFunc     ( animate );

	initExtensions ();
	printfInfo     ();
	
	if ( !isExtensionSupported ( "GL_EXT_fog_coord" ) )
	{
		printf ( "GL_EXT_fog coord NOT supported.\n" );

		return 1;
	}


	texture = createTexture2D ( true, "../Textures/concgrey.bmp" );

	init     ();
	initKnot ();

	glutMainLoop ();

	return 0;
}
