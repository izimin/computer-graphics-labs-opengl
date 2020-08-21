//
// Sample to rendering to texture - deformed reflection
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include    "libExt.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include    <stdio.h>
#include    <stdlib.h>

#include    "libTexture.h"
#include    "TypeDefs.h"
#include    "Vector3D.h"
#include    "Vector2D.h"
#include    "Vector4D.h"
#include    "boxes.h"
#include    "PBuffer.h"

#define GRID_SIZE   30                  // grid subdivision

Vector3D    eye   ( -0.5, -0.5, 1.5 );  // camera position
unsigned    decalMap;                   // decal (diffuse) texture
unsigned    stoneMap;
unsigned    teapotMap;
float       angle     = 0;
float       rot       = 0;

float   mv [16];                        // to hold modelview matrix used for building reflection texture
float   pr [16];                        // to hold projection matrix used for building reflection texture

Vector3D    v [GRID_SIZE][GRID_SIZE];   // grid vertices for floor
Vector3D    t [GRID_SIZE][GRID_SIZE];   // texture coordinates for floor vertices

Vector3D    floorPos  ( -5, -5, 0 );
Vector3D    floorSize ( 10, 10, 3 );

PBuffer     pbuffer ( 512, 512, PBuffer :: modeTextureMipmap | PBuffer :: modeAlpha |
                      PBuffer :: modeDepth | PBuffer :: modeTexture2D, true );

void    renderToBuffer  ();
void    reshapeMirrored ( int w, int h );
void    reshape         ( int w, int h );

void init ()
{
    glClearColor ( 0.0, 0.0, 0.0, 1.0 );
    glEnable     ( GL_DEPTH_TEST );
    glEnable     ( GL_TEXTURE_2D );
    glDepthFunc  ( GL_LEQUAL     );

    glHint ( GL_POLYGON_SMOOTH_HINT,         GL_NICEST );
    glHint ( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void displayBoxes ()
{
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ();

    drawBoxNoBottom ( Vector3D ( -5, -5, 0 ), Vector3D ( 10, 10, 3 ), stoneMap, false );
    drawBox         ( Vector3D ( 3, 2, 0.5 ), Vector3D ( 1,  2,  2 ), decalMap, false );

    glBindTexture   ( GL_TEXTURE_2D, teapotMap );
    glTranslatef    ( 2, 2, 1 );
    glRotatef       ( angle * 45.3, 1, 0, 0 );
    glRotatef       ( angle * 57.2, 0, 1, 0 );
    glutSolidTeapot ( 0.5 );

    glPopMatrix ();                                 // restore modelview matrix
}

//
// distort texture coordinates for internal vertices by moving them sligtly with the time
//
void    distortVertices ( float time )
{
    float   hx = floorSize.x / (float) (GRID_SIZE - 1);
    float   hy = floorSize.y / (float) (GRID_SIZE - 1);

    for ( int i = 1; i < GRID_SIZE - 1; i++ )
        for ( int j = 1; j < GRID_SIZE - 1; j++ )
        {
            Vector3D    p ( v [i][j] );
            float   sine   = 0.03 * sin (  11*p.y + 16*p.x + 2*time );
            float   cosine = 0.03 * cos ( -15*p.y - 14*p.x + 2*time );

            t [i][j].x = v [i][j].x + sine;
            t [i][j].y = v [i][j].y + cosine;
            t [i][j].z = v [i][j].z;
        }
}

void display ()
{
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    renderToBuffer ();                              // render reflected image into pbuffer

    reshape ( 512, 512 );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ();                                // save modelview matrix

    glRotatef    ( rot, 0, 0, 1 );

    displayBoxes ();

    if ( !pbuffer.bindToTexture () )
        pbuffer.printLastError ();

    glMatrixMode  ( GL_TEXTURE );
    glPushMatrix  ();

    glLoadIdentity ();
    glTranslatef   ( 0.5, 0.5, 0 );     // remap from [-1,1]^2 to [0,1]^2
    glScalef       ( 0.5, 0.5, 1 );
    glMultMatrixf  ( pr );
    glMultMatrixf  ( mv );


    float       x2 = floorPos.x + floorSize.x;
    float       y2 = floorPos.y + floorSize.y;

    glBindTexture ( GL_TEXTURE_2D, pbuffer.getTexture () );

                                        // draw floor
    glColor3f ( 0.8, 0.8, 0.8 );        // make it a bit darker

/*
    glBegin ( GL_QUADS );
        glNormal3f ( 0, 0, -1 );

        glTexCoord3f ( x2, floorPos.y, floorPos.z );
        glVertex3f   ( x2, floorPos.y, floorPos.z );

        glTexCoord3f ( floorPos.x, floorPos.y, floorPos.z );
        glVertex3f   ( floorPos.x, floorPos.y, floorPos.z );

        glTexCoord3f ( floorPos.x, y2, floorPos.z );
        glVertex3f   ( floorPos.x, y2, floorPos.z );

        glTexCoord3f ( x2, y2, floorPos.z );
        glVertex3f   ( x2, y2, floorPos.z );
    glEnd ();
*/
                                        // draw the distorted mesh
    glBegin ( GL_TRIANGLES );

    glNormal3f ( 0, 0, -1 );

    for ( int i = 0; i < GRID_SIZE - 1; i++ )
        for ( int j = 0; j < GRID_SIZE - 1; j++ )
        {
            glTexCoord3fv ( t [i][j] );
            glVertex3fv   ( v [i][j] );

            glTexCoord3fv ( t [i][j+1] );
            glVertex3fv   ( v [i][j+1] );

            glTexCoord3fv ( t [i+1][j+1] );
            glVertex3fv   ( v [i+1][j+1] );

            glTexCoord3fv ( t [i][j] );
            glVertex3fv   ( v [i][j] );

            glTexCoord3fv ( t [i+1][j] );
            glVertex3fv   ( v [i+1][j] );

            glTexCoord3fv ( t [i+1][j+1] );
            glVertex3fv   ( v [i+1][j+1] );
        }

    glEnd ();

    glColor3f ( 1, 1, 1 );

    if ( !pbuffer.unbindFromTexture () )
        pbuffer.printLastError ();

    glPopMatrix ();

    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix  ();

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
   gluLookAt      ( eye.x, eye.y, eye.z,    // eye
                    3, 3, 1,                // center
                    0, 0, 1 );              // up
}

void reshapeMirrored ( int w, int h )
{
   glViewport     ( 0, 0, (GLsizei)w, (GLsizei)h );
   glMatrixMode   ( GL_PROJECTION );
   glLoadIdentity ();
   gluPerspective ( 60.0, (GLfloat)w/(GLfloat)h, 1.0, 60.0 );
   glMatrixMode   ( GL_MODELVIEW );
   glLoadIdentity ();
   glScalef       ( 1, -1, 1 );             // change handedness of the coordinate system
   gluLookAt      ( eye.x, eye.y, -eye.z,   // eye
                    3, 3, -1,               // center
                    0, 0, -1 );             // up
}

void key ( unsigned char key, int x, int y )
{
    if ( key == 27 || key == 'q' || key == 'Q' )        // quit requested
        exit ( 0 );
}

void    specialKey ( int key, int x, int y )
{
    if ( key == GLUT_KEY_RIGHT )
        rot += 5;
    else
    if ( key == GLUT_KEY_LEFT )
        rot -= 5;

    glutPostRedisplay ();
}

void    animate ()
{
    angle  = 0.001f * glutGet ( GLUT_ELAPSED_TIME );

    distortVertices ( angle );

    glutPostRedisplay ();
}

void    initPBuffer ()
{
    if ( !pbuffer.create () )
    {
        printf ( "create error\n" );

        pbuffer.printLastError ();
    }

    pbuffer.makeCurrent ();

    init ();

    reshapeMirrored ( pbuffer.getWidth (), pbuffer.getHeight () );

    pbuffer.restoreCurrent ();
}

void    renderToBuffer ()
{
    if ( pbuffer.isLost () )
        pbuffer.create ();

    if ( !pbuffer.makeCurrent () )
        printf ( "makeCurrent failed\n" );

    glClearColor ( 0, 0, 1, 1 );
    glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    reshapeMirrored ( pbuffer.getWidth (), pbuffer.getHeight () );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ();

    glRotatef    ( rot, 0, 0, 1 );

    displayBoxes ();

                                        // get modelview and projections matrices
    glGetFloatv ( GL_MODELVIEW_MATRIX,  mv );
    glGetFloatv ( GL_PROJECTION_MATRIX, pr );

    glPopMatrix ();

    pbuffer.restoreCurrent ();
}

int main ( int argc, char * argv [] )
{
                                // initialize glut
    glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize  ( 512, 512 );


                                // create window
    int win = glutCreateWindow ( "OpenGL deformed reflection" );

                                // register handlers
    glutDisplayFunc  ( display    );
    glutReshapeFunc  ( reshape    );
    glutKeyboardFunc ( key        );
    glutSpecialFunc  ( specialKey );
    glutIdleFunc     ( animate    );

    init           ();
    initExtensions ();


    if ( !pbuffer.checkExtensions () )
    {
        printf ( "Pbuffer extensions not found\n" );

        return 1;
    }

    decalMap  = createTexture2D ( true, "../Textures/oak.bmp" );
    stoneMap  = createTexture2D ( true, "../Textures/block.bmp" );
    teapotMap = createTexture2D ( true, "../Textures/Oxidated.jpg" );

                                // init vertices
    float   hx = floorSize.x / (float) (GRID_SIZE - 1);
    float   hy = floorSize.y / (float) (GRID_SIZE - 1);

    for ( int i = 0; i < GRID_SIZE; i++ )
        for ( int j = 0; j < GRID_SIZE; j++ )
        {
            float   x = floorPos.x + i * hx;
            float   y = floorPos.y + j * hy;

            v [i][j] = Vector3D ( x, y, floorPos.z );
            t [i][j] = Vector3D ( x, y, floorPos.z );
        }

    initPBuffer  ();
    glutMainLoop ();

    return 0;
}
