//
// Sample to particle system simulation via vertex programs in OpenGL
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
#include	"VertexProgram.h"

#define	NUM_PARTICLES	700

struct	ParticleData
{
	Vector3D	pos;
	Vector3D	r;
	float		freq;
};

Vector3D	eye   ( 7, 5, 7 );			// camera position
unsigned	decalMap;					// decal (diffuse) texture
float	 	angle = 0;
Vector3D	rot ( 0, 0, 0 );
int			mouseOldX = 0;
int			mouseOldY = 0;

ParticleData	particles [NUM_PARTICLES];
VertexProgram	program;

/////////////////////////////////////////////////////////////////////////////////

void	initParticles ( const Vector3D& center )
{
	Vector3D	e1 ( 1,  0, 0 );
	Vector3D	e2 ( 0, -1, 0 );

	for ( int i = 0; i < NUM_PARTICLES; i++ )
	{
		Vector3D	r = Vector3D:: getRandomVector ( 2 );

		particles [i].pos  = center;
		particles [i].r    = r;
		particles [i].freq = 1 + 8 * pow ( 0.5 + 0.5 * fabs ( (e1 & r) ), 4 );
	}
}

void init ()
{
	initParticles ( Vector3D ( 0, 0, 0 ) );

	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glDepthFunc  ( GL_LEQUAL );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glPushAttrib  ( GL_ENABLE_BIT | GL_POINT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable      ( GL_TEXTURE_2D );
	glDepthMask   ( GL_FALSE );
	glBindTexture ( GL_TEXTURE_2D, decalMap );
	glEnable      ( GL_BLEND );
	glBlendFunc   ( GL_SRC_ALPHA, GL_ONE );

    float quadratic [] =  { 1.0f, 0.0f, 0.01f };

    glEnable              ( GL_POINT_SPRITE_ARB );
    glPointParameterfvARB ( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
    glPointParameterfARB  ( GL_POINT_FADE_THRESHOLD_SIZE_ARB,  20.0f );

    glTexEnvf   ( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    glPointSize ( 8 );

	program.enable ();
	program.bind ();

	glPushMatrix   ();

    glRotatef    ( rot.x, 1, 0, 0 );
    glRotatef    ( rot.y, 0, 1, 0 );
    glRotatef    ( rot.z, 0, 0, 1 );

	glBegin ( GL_POINTS );

	for ( int i = 0; i < NUM_PARTICLES; i++ )
	{
		glMultiTexCoord3fv ( GL_TEXTURE0_ARB, particles [i].r    );
		glMultiTexCoord1f  ( GL_TEXTURE1_ARB, particles [i].freq );
	  	glVertex3fv        ( particles [i].pos                   );

	}

	glEnd ();

	program.disable ();

	glPopMatrix ();
	glPopAttrib ();

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
					0.0, 0.0, 1.0 );		// up
}

void motion ( int x, int y )
{
    rot.y -= ((mouseOldY - y) * 180.0f) / 200.0f;
    rot.z -= ((mouseOldX - x) * 180.0f) / 200.0f;
    rot.x  = 0;

    if ( rot.z > 360 )
		rot.z -= 360;

	if ( rot.z < -360 )
		rot.z += 360;

    if ( rot.y > 360 )
		rot.y -= 360;

	if ( rot.y < -360 )
        rot.y += 360;

    mouseOldX = x;
    mouseOldY = y;

	glutPostRedisplay ();
}

void mouse ( int button, int state, int x, int y )
{
    if ( state == GLUT_DOWN )
    {
        mouseOldX = x;
        mouseOldY = y;
	}
}

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )	// quit requested
    	exit ( 0 );
}

void	animate ()
{
	float	t  = 0.003f * glutGet ( GLUT_ELAPSED_TIME );
	float	st = 0.5f * ( 1 + sin ( t ) );

	program.local [0] = Vector4D ( st, st, st, st );

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL particle systems animation via vertex program" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutMouseFunc    ( mouse   );
	glutMotionFunc   ( motion  );
	glutKeyboardFunc ( key );
	glutIdleFunc     ( animate );

	init           ();
	initExtensions ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_ARB_point_parameters" ) )
	{
		printf ( "ARB_point_parameters NOT supported.\n" );

		return 2;
	}

	if ( !isExtensionSupported ( "GL_ARB_point_sprite" ) )
	{
		printf ( "ARB_point_sprite NOT supported" );

		return 3;
	}

	if ( !isExtensionSupported ( "GL_ARB_vertex_program" ) )
	{
		printf ( "GL_ARB_vertex_program NOT supported" );

		return 4;
	}

	decalMap = createTexture2D ( true,  "../Textures/particle1.bmp" );

	if ( !program.load ( "particles.vp" ) )
	{
		printf ( "Error loading program: %s\n", program.getErrorString ().c_str () );

		return 1;
	}

	glutMainLoop ();

	return 0;
}
