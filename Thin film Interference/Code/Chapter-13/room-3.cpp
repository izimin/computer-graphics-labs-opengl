//
// Sample to show occlusion culling via ARB_occlusion_query extension
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
#include	<list>

using namespace std;

#include	"libTexture.h"
#include	"TypeDefs.h"
#include	"Vector3D.h"
#include	"Vector2D.h"
#include	"boxes.h"
#include	"SceneObject.h"
#include	"Frustum.h"
#include	"Cell.h"

#define	N	20							// size of matrix

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
int			elapsedTime  = 0;
float		fps          = 0;

list <SceneObject *>	objects;		// global list of objects
Cell	 * cells [N][N];

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

void	initCells ()
{
	float	step = 2*roomSize / (float) N;

	for ( int i = 0; i < N; i++ )
		for ( int j = 0; j < N; j++ )
		{
			float	x = -roomSize + i*step;
			float	y = -roomSize + j*step;

			cells [i][j] = new Cell ( Vector3D ( x, y, -3 ), Vector3D ( step, step, 5 ), objects );
		}
}

void	renderMajorX ( const Frustum& frustum, int incr )
{
	float	step = 2*roomSize / (float) N;
	int		i0   = (eye.x + roomSize) / step;			// cell with observer
	int		j0   = (eye.y + roomSize) / step;
	int		jMin = j0;
	int		jMax = j0;
	int		i    = i0;
	int		j;
	GLuint	ids [100];
	int		numQueries;

	glGenQueriesARB ( N, ids );

	for ( ; i >= 0 && i < N; i += incr )
	{
								// update jMin, jMax
		for ( j = jMin - 1; j >= 0; j-- )
			if ( !frustum.boxInFrustum ( cells [i][j] -> getBounds () ) )
				break;

		jMin = j + 1;

		for ( j = jMax + 1; j < N; j++ )
			if ( !frustum.boxInFrustum ( cells [i][j] -> getBounds () ) )
				break;

		jMax = j - 1;

		if ( i == i0 /*|| i == i0 + incr*/ )									// for first row just render
		{
			for ( j = jMin; j <= jMax; j++ )
				cells [i][j] -> render ( frames );

			continue;
		}

														// now post queries for (i,jMin)-(i,jMax)
		numQueries = jMax - jMin + 1;
														// disable writing
		glColorMask ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glDepthMask ( GL_FALSE );
		glDisable   ( GL_TEXTURE_2D );


		for ( j = jMin; j <= jMax; j++ )
		{
			glBeginQueryARB ( GL_SAMPLES_PASSED_ARB, ids [j - jMin] );

			cells [i][j] -> postQuery ();

			glEndQueryARB ( GL_SAMPLES_PASSED_ARB );
		}

		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glDepthMask ( GL_TRUE );
		glEnable    ( GL_TEXTURE_2D );

														// now wait for the last query
		GLint	available;
		GLuint	count;
		GLuint	idLast     = ids [numQueries - 1];
		bool	anyVisible = false;

		do
		{
			glGetQueryObjectivARB ( idLast, GL_QUERY_RESULT_AVAILABLE_ARB, &available );
		} while (!available);

														// restore all masks and options
		for ( j = jMin; j <= jMax; j++ )
		{
			glGetQueryObjectuivARB ( ids [j - jMin], GL_QUERY_RESULT_ARB, &count );

			if ( count > 0 || cells [i][j] -> getBounds ().contains ( eye ) )
			{
				cells [i][j] -> render ( frames );

				anyVisible = true;
			}
		}

		if ( !anyVisible )								// if the whole row is invisible no
			break;										// sense going further
	}

	glDeleteQueriesARB ( N, ids );
}

void	renderMajorY ( const Frustum& frustum, int incr )
{
	float	step = 2*roomSize / (float) N;
	int		i0   = (eye.x + roomSize) / step;			// cell with observer
	int		j0   = (eye.y + roomSize) / step;
	int		iMin = i0;
	int		iMax = i0;
	int		j    = j0;
	int		i;

	GLuint	ids [100];
	int		numQueries;

	glGenQueriesARB ( N, ids );

	for ( ; j >= 0 && j < N; j += incr )
	{
								// update iMin, iMax
		for ( i = iMin - 1; i >= 0; i-- )
			if ( !frustum.boxInFrustum ( cells [i][j] -> getBounds () ) )
				break;

		iMin = i + 1;

		for ( i = iMax + 1; i < N; i++ )
			if ( !frustum.boxInFrustum ( cells [i][j] -> getBounds () ) )
				break;

		iMax = i - 1;

		if ( j == j0 /*|| j == j0 + incr*/ )
		{
			for ( i = iMin; i <= iMax; i++ )
				cells [i][j] -> render ( frames );

			continue;
		}
														// now post queries for (iMin,j)-(iMax,j)
		numQueries = iMax - iMin + 1;

														// disable writing
		glColorMask ( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glDepthMask ( GL_FALSE );
		glDisable   ( GL_TEXTURE_2D );

		for ( i = iMin; i <= iMax; i++ )
		{
			glBeginQueryARB ( GL_SAMPLES_PASSED_ARB, ids [i - iMin] );

			cells [i][j] -> postQuery ();

			glEndQueryARB ( GL_SAMPLES_PASSED_ARB );
		}
														// restore all masks and options
		glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glDepthMask ( GL_TRUE );
		glEnable    ( GL_TEXTURE_2D );

														// now wait for the last query
		GLint	available;
		GLuint	count;
		GLuint	idLast     = ids [numQueries - 1];
		bool	anyVisible = false;

		do
		{
			glGetQueryObjectivARB ( idLast, GL_QUERY_RESULT_AVAILABLE_ARB, &available );
		} while (!available);

		for ( i = iMin; i <= iMax; i++ )
		{
			glGetQueryObjectuivARB ( ids [i - iMin], GL_QUERY_RESULT_ARB, &count );

			if ( count > 0 )
			{
				cells [i][j] -> render ( frames );

				anyVisible = true;
			}
		}

		if ( !anyVisible )								// if the whole row is invisible no
			break;										// sense going further
	}

	glDeleteQueriesARB ( N, ids );
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

	Frustum	frustum;
										// draw main hall
	drawBox  ( Vector3D ( -roomSize, -roomSize, -3 ), Vector3D ( 2*roomSize, 2*roomSize, 5 ), stoneMap, false );

										// draw objects
	if ( fabs ( viewDir.x ) >= fabs ( viewDir.y ) )
		renderMajorX ( frustum, viewDir.x > 0 ? 1 : -1 );
	else
		renderMajorY ( frustum, viewDir.y > 0 ? 1 : -1 );

//printf ( "%7f,%7f\n", viewDir.x, viewDir.y );

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

	init ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_ARB_occlusion_query" ) )
	{
		printf ( "ARB_occlusion_query NOT supported" );

		return 2;
	}

	initExtensions ();
	printfInfo     ();

	stoneMap     = createTexture2D ( true, "../Textures/block.bmp" );
	decalMap [0] = createTexture2D ( true, "../Textures/oak.bmp" );
	decalMap [1] = createTexture2D ( true, "../Textures/FieldStone.tga" );
	decalMap [2] = createTexture2D ( true, "../Textures/wood1.bmp" );

	initObjects ();
	initCells   ();

 	glutMainLoop ();

	return 0;
}
