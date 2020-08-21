//
// Cell class containing several objects inside
//
// Author: Alex V. Boreskoff
//

#include	"libExt.h"
#include	"Cell.h"
#include	"SceneObject.h"
#include	"boxes.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif

Cell :: Cell ( const Vector3D& org, const Vector3D& size, const list <SceneObject *>& srcObjects ) : box ( org, org + size )
{
							// now add any object with not-empty intersection
							// and compute real bounding box
	BoundingBox	b;

	for ( list <SceneObject *> :: const_iterator it = srcObjects.begin (); it != srcObjects.end (); ++it )
	{
		SceneObject * object = *it;

		if ( box.intersects ( object -> getBounds () ) )
		{
			objects.push_front ( object );

			b.merge ( object -> getBounds () );
		}
	}

	box        = b;				// set real bounding box
	visToFrame = -1;
}

void	Cell :: render ( int frameNo ) const
{
	for ( list <SceneObject *> :: const_iterator it = objects.begin (); it != objects.end (); ++it )
		if ( (*it) -> frame != frameNo )					// avoid rerendering of already rendered objects
		{
			(*it) -> draw ();
			(*it) -> frame = frameNo;
		}


	if ( visToFrame < frameNo )								// was invisible
		visToFrame = frameNo + 3;							// assume visible for next 3 frames
}

void	Cell :: postQuery () const
{
	drawBox ( box.getMinPoint (), box.getSize (), 0, false );
}

