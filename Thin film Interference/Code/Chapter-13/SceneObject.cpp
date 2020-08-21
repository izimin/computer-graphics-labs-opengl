//
// Simple object for occlusion testing
//

#ifdef	_WIN32
	#include	<windows.h>
#endif

#include	"libExt.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

#include	"SceneObject.h"
#include	"boxes.h"

void	BoxObject :: draw () const
{
	switch ( type )
	{
		case 0:
		case 1:
			drawBox ( pos, size, texture );
			return;

		case 2:
			drawBox2 ( pos, size, texture );
			return;

		case 3:
			drawBox3 ( pos, size, texture );
			return;

		case 4:
			drawBox4 ( pos, size, texture );
			return;
	}

	return;
}

void	TeapotObject :: draw () const
{
	glMatrixMode    ( GL_MODELVIEW );
	glPushMatrix    ();
	glTranslatef    ( pos.x, pos.y, pos.z );
	glScalef        ( size.x, size.y, size.z );
	glBindTexture   ( GL_TEXTURE_2D, texture );
	glutSolidTeapot ( size.x );
	glPopMatrix     ();
}
