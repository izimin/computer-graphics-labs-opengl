//
// Sample to view frustum & occlusion culling
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
#include	<list>

using namespace std;

#include	"libTexture.h"
#include	"TypeDefs.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"boxes.h"
#include	"SceneObject.h"

Vector3D	eye   ( 0, 0, -2 );			// camera position
Vector3D	viewDir ( 1, 0, 0 );		// viewer direction

unsigned	decalMap [3];				// decal (diffuse) texture
unsigned	stoneMap;
int			mouseOldX = 0;
int			mouseOldY = 0;
Vector3D	rot ( 0, 0, 0 );
float		step = 0.003;				// movement rate per second
bool		mousePressed = false;
float		roomSize     = 100;			// room half size
int			frames       = 0;
int			elapsedTime         = 0;
float		fps          = 0;

list <SceneObject *>	objects;		// global list of objects

void	setupCamera ();

inline	float	rnd ()
{
	return (float) rand () / (float)RAND_MAX;
}

void	initObjects ()
{
	for ( int i = 1 - roomSize;  i < roomSize - 1; i += 2 )
		for ( int j = 1 - roomSize; j < roomSize - 1; j += 2 )
		{
			float	x    = i + rnd () * 1.2;
			float	y    = j + rnd () * 1.2;
			float	sx   = 0.7 + 0.6 * rnd ();
			float	sy   = 0.7 + 0.5 * rnd ();
			int		type = 1 + rand () % 4;
			int		tex  = rand () % 3;

			if ( (rand () % 15) == 14 )
				objects.push_back ( new TeapotObject ( Vector3D ( x, y, -2 ), Vector3D ( sx*0.5, sy*0.5, (sx + sy) * 0.25 ), decalMap [tex] ) );
			else
				objects.push_front ( new BoxObject ( Vector3D ( x, y, -3 ), Vector3D ( sx, sy, sx + sy ), decalMap [tex], type ) );
		}
}

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glEnable     ( GL_TEXTURE_2D );
	glDepthFunc  ( GL_LEQUAL     );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

										// draw main hall
	drawBox  ( Vector3D ( -roomSize, -roomSize, -3 ), Vector3D ( 2*roomSize, 2*roomSize, 7 ), stoneMap, false );

										// draw objects
	for ( list <SceneObject *> :: iterator it = objects.begin (); it != objects.end (); ++it )
		(*it) -> draw ();

	frames++;

	if ( (frames % 5) == 0 )						// compute every 5 frames
	{
		int		time2 = glutGet ( GLUT_ELAPSED_TIME );
	   	char	str [256];

		fps         = (float)frames / (0.001 * (time2 - elapsedTime));
		elapsedTime = time2;
		frames      = 0;

   		sprintf ( str, "OpenGL occlusion test example, FPS : %5.2f", fps );

   		glutSetWindowTitle ( str );
	}

	glutSwapBuffers ();
}

void reshape ( int w, int h )
{
    glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ();
    gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 0.1, 1000 );
	setupCamera    ();
}

void	setupCamera ()
{
	Vector3D	to = eye + viewDir;

  	glMatrixMode   ( GL_MODELVIEW );
   	glLoadIdentity ();
   	gluLookAt      ( eye.x, eye.y, eye.z,	// eye
					 to.x,  to.y,  to.z,	// center
					 0.0, 0.0, 1.0 );		// up

}

void motion ( int x, int y )
{
	static	bool	mouseInited = false;

	if ( !mouseInited )
	{
		mouseInited = true;
		mouseOldX   = x;
		mouseOldY   = y;

		return;
	}

	float	scale = -M_PI / 100;

	viewDir.x = cos ( scale * x );
	viewDir.y = sin ( scale * x );
	viewDir.z = 0;

	setupCamera ();

	glutPostRedisplay ();
}

void mouse ( int button, int state, int x, int y )
{
	if ( button == GLUT_LEFT_BUTTON )
		mousePressed = (state == GLUT_DOWN);

	motion ( x, y );
}

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    	exit ( 0 );
}

void	animate ()
{
	static	bool	timeInited = false;
	static	int		lastTime;

	if ( !timeInited )
	{
		lastTime    = glutGet ( GLUT_ELAPSED_TIME );
		timeInited = true;
	}

	if ( mousePressed )
	{
		Vector3D	delta = viewDir * (step * (glutGet ( GLUT_ELAPSED_TIME ) - lastTime)*0.001f);
		Vector3D	e     = eye + delta;

		if ( e.x < roomSize && e.x > -roomSize )
			eye.x = e.x;

		if ( e.y < roomSize && e.y > -roomSize )
			eye.y = e.y;

		setupCamera ();
	}

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL occlusion test example" );

								// register handlers
								// register handlers
	glutDisplayFunc       ( display );
	glutReshapeFunc       ( reshape );
	glutKeyboardFunc      ( key     );
	glutMouseFunc         ( mouse   );
	glutMotionFunc        ( motion  );
	glutPassiveMotionFunc ( motion  );
	glutIdleFunc          ( animate );

	init       ();
	printfInfo ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	initExtensions ();

	stoneMap     = createTexture2D ( true, "../Textures/block.bmp" );
	decalMap [0] = createTexture2D ( true, "../Textures/oak.bmp" );
	decalMap [1] = createTexture2D ( true, "../Textures/FieldStone.tga" );
	decalMap [2] = createTexture2D ( true, "../Textures/wood1.bmp" );

	initObjects  ();

 	glutMainLoop ();

	return 0;
}
