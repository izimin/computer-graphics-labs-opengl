//
// Sample to show distance attenuation in OpenGL using PASS_THROUGH_NV shader
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
#include	"TypeDefs.h"
#include	"Vector3D.h"
#include	"Vector2D.h"

Vector3D	eye   ( 0, 0.3, -3.5 );		// camera position
Vector3D	light ( 0.3, 0.7, 0.9f );	// light position
Vector3D	vl    ( 0.4, 0.4, -0.4 );	// light velocity
float		lightRadius = 1;
unsigned	decalMap;					// decal (diffuse) texture

struct	Vertex
{
	Vector2D	tex;
	Vector3D	pos;
};

Vertex	front [4];
Vertex	lower [4];
Vertex	upper [4];
Vertex	left  [4];
Vertex	right [4];

void init ()
{
	glClearColor ( 0.0, 0.0, 0.0, 1.0 );
	glEnable     ( GL_DEPTH_TEST );
	glEnable     ( GL_TEXTURE_2D );
	glDepthFunc  ( GL_LEQUAL     );

	glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
	glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void	drawFace ( Vertex v [], int count )
{
	glBegin ( GL_QUADS );

	for ( int i = 0; i < count; i++ )
	{
		Vector3D	d = v [i].pos - light;

		d = Vector3D ( 0.5, 0.5, 0.5f ) + d * (0.5 / lightRadius );

		glMultiTexCoord2fv ( GL_TEXTURE0_ARB, v [i].tex );
		glMultiTexCoord3fv ( GL_TEXTURE1_ARB, d );
		glVertex3fv        ( v [i].pos );
	}

	glEnd   ();
}

void display ()
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

										// bind decal map to texture unit 0

										// setup texture shader
	glEnable  ( GL_TEXTURE_SHADER_NV );

										// unit 0 -> decal
	glActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnable           ( GL_TEXTURE_2D   );
	glBindTexture      ( GL_TEXTURE_2D, decalMap );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_TEXTURE_2D );

										// unit 1 -> pass through
	glActiveTextureARB ( GL_TEXTURE1_ARB );
	glTexEnvi          ( GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_PASS_THROUGH_NV );
	glActiveTextureARB ( GL_TEXTURE0_ARB );


										// setup register combiners
	glEnable               ( GL_REGISTER_COMBINERS_NV );
	glCombinerParameteriNV ( GL_NUM_GENERAL_COMBINERS_NV, 1 );

										// combiner0 computes (tex1,tex1) -> spare0
										// A = texture1
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV, GL_TEXTURE1_ARB,
					    GL_EXPAND_NORMAL_NV, GL_RGB );

										// B = texture1
	glCombinerInputNV ( GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV, GL_TEXTURE1_ARB,
						GL_EXPAND_NORMAL_NV, GL_RGB );

										// spare0 = tex1 + tex2
	glCombinerOutputNV ( GL_COMBINER0_NV, GL_RGB,
						 GL_SPARE0_NV,		// put (A,B) into spare0
						 GL_DISCARD_NV, 	// discard CD
                         GL_DISCARD_NV, 	// discard AB+CD
	                     GL_NONE, GL_NONE, 	// no bias or scale
						 GL_TRUE, 			// use dot porduct for AB
						 GL_FALSE,
						 GL_FALSE );

	                      				// configure final combiner to output clamp (1 - spare0)
	                      				// A = 1 - spare0
	glFinalCombinerInputNV ( GL_VARIABLE_A_NV, GL_SPARE0_NV, GL_UNSIGNED_INVERT_NV, GL_RGB );


										// B = tex0 (decal)
	glFinalCombinerInputNV ( GL_VARIABLE_B_NV, GL_TEXTURE0_ARB, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

										// C = 0
	glFinalCombinerInputNV ( GL_VARIABLE_C_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

										// D = 0
	glFinalCombinerInputNV ( GL_VARIABLE_D_NV, GL_ZERO, GL_UNSIGNED_IDENTITY_NV, GL_RGB );

	drawFace ( front, 4 );
	drawFace ( lower, 4 );
	drawFace ( upper, 4 );
	drawFace ( left,  4 );
	drawFace ( right, 4 );

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

void key ( unsigned char key, int x, int y )
{
	if ( key == 27 || key == 'q' || key == 'Q' )		// quit requested
    		exit ( 0 );
}

void	animate ()
{
	Vector3D	delta = vl * 0.01f;

	light += delta;

	if ( light.x < 0.2f || light.x > 0.9f )
		vl.x *= -1;

	if ( light.y < -0.9f || light.y > 0.9f )
		vl.y *= -1;

	if ( light.z < -0.9f || light.z > 0.9f )
		vl.z *= -1;

	glutPostRedisplay ();
}

int main ( int argc, char * argv [] )
{
								// initialize glut
	glutInit            ( &argc, argv );
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize  ( 500, 500 );


								// create window
	glutCreateWindow ( "OpenGL per-pixel distance attenuation using PASS_THROUGH shader" );

								// register handlers
	glutDisplayFunc  ( display );
	glutReshapeFunc  ( reshape );
	glutKeyboardFunc ( key     );
	glutIdleFunc     ( animate );

	init       ();
	printfInfo ();

	if ( !isExtensionSupported ( "GL_ARB_multitexture" ) )
	{
		printf ( "ARB_multitexture NOT supported.\n" );

		return 1;
	}

	if ( !isExtensionSupported ( "GL_NV_register_combiners" ) )
	{
		printf ( "NV_register_combiners NOT supported" );

		return 2;
	}

	if ( !isExtensionSupported ( "GL_NV_texture_shader" ) )
	{
		printf ( "NV_register_combiners NOT supported" );

		return 2;
	}

	initExtensions ();

	decalMap   = createTexture2D      ( true, "../Textures/oak.bmp" );


	front [0].tex = Vector2D ( 0, 0 );
	front [0].pos = Vector3D ( -1, -1, 1 );
	front [1].tex = Vector2D ( 0, 1 );
	front [1].pos = Vector3D ( -1,  1, 1 );
	front [2].tex = Vector2D ( 1, 1 );
	front [2].pos = Vector3D (  1,  1, 1 );
	front [3].tex = Vector2D ( 1, 0 );
	front [3].pos = Vector3D (  1, -1, 1 );

	lower [0].tex = Vector2D (  0, 0 );
	lower [0].pos = Vector3D ( -1, -1, -1 );
	lower [1].tex = Vector2D (  0, 1 );
	lower [1].pos = Vector3D ( -1, -1,  1 );
	lower [2].tex = Vector2D (  1, 1 );
	lower [2].pos = Vector3D (  1, -1,  1 );
	lower [3].tex = Vector2D (  1, 0 );
	lower [3].pos = Vector3D (  1, -1, -1 );

	upper [0].tex = Vector2D (  0, 0 );
	upper [0].pos = Vector3D ( -1, 1, -1 );
	upper [1].tex = Vector2D (  0, 1 );
	upper [1].pos = Vector3D ( -1, 1,  1 );
	upper [2].tex = Vector2D (  1, 1 );
	upper [2].pos = Vector3D (  1, 1,  1 );
	upper [3].tex = Vector2D (  1, 0 );
	upper [3].pos = Vector3D (  1, 1, -1 );

	left [0].tex = Vector2D ( 0, 0 );
	left [0].pos = Vector3D ( -1, -1, -1 );
	left [1].tex = Vector2D ( 0, 1 );
	left [1].pos = Vector3D ( -1, -1, 1 );
	left [2].tex = Vector2D ( 1, 1 );
	left [2].pos = Vector3D ( -1, 1, 1 );
	left [3].tex = Vector2D ( 1, 0 );
	left [3].pos = Vector3D ( -1, 1, -1 );

	right [0].tex = Vector2D ( 0, 0 );
	right [0].pos = Vector3D ( 1, -1, -1 );
	right [1].tex = Vector2D ( 0, 1 );
	right [1].pos = Vector3D ( 1, -1, 1 );
	right [2].tex = Vector2D ( 1, 1 );
	right [2].pos = Vector3D ( 1, 1, 1 );
	right [3].tex = Vector2D ( 1, 0 );
	right [3].pos = Vector3D ( 1, 1, -1 );

	glutMainLoop ();

	return 0;
}
