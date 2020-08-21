//
// Cell class containing several objects inside
//
// Author: Alex V. Boreskoff
//

#ifndef	__CELL__
#define	__CELL__

#include	"BoundingBox.h"
#include	<list>

using namespace std;

class	SceneObject;

class	Cell
{
	list <SceneObject *> 	objects;
	BoundingBox				box;
	mutable int				visToFrame;					// to which frame no we assume it's visible

public:
	Cell ( const Vector3D& org, const Vector3D& size, const list <SceneObject *>& srcObjects );

	const BoundingBox&	getBounds () const
	{
		return box;
	}

	int	getVisToFrame () const
	{
		return visToFrame;
	}

	void	render    ( int frameNo ) const;
	void	postQuery () const;
};

#endif
