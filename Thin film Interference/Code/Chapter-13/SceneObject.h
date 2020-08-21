//
// Simple object for occlusion testing
//

#ifndef	__SCENE_OBJECT__
#define	__SCENE_OBJECT__

#include	"Vector3D.h"
#include	"BoundingBox.h"

class	SceneObject
{
protected:
	Vector3D	pos;
	Vector3D	size;
	unsigned	texture;
	BoundingBox	box;
	
public:
	int			frame;								// frame where object has been rendered

	SceneObject ( const Vector3D& thePos, const Vector3D& theSize, unsigned theTexture ) : box ( thePos, thePos + theSize )
	{
		pos     = thePos;
		size    = theSize;
		texture = theTexture;
		frame   = -1;
	}

	const BoundingBox&	getBounds () const
	{
		return box;
	}

	virtual ~SceneObject () {}

	virtual	void	draw () const = 0;
};

class	BoxObject : public SceneObject
{
protected:
	int	type;

public:
	BoxObject ( const Vector3D& thePos, const Vector3D& theSize, unsigned theTexture, int theType ) :
		SceneObject ( thePos, theSize, theTexture )
	{
		type = theType;
	}

	virtual	void	draw () const;
};

class	TeapotObject : public SceneObject
{
public:
	TeapotObject ( const Vector3D& thePos, const Vector3D& theSize, unsigned theTexture ) :
		SceneObject ( thePos, theSize, theTexture )
	{

	}

	virtual	void	draw () const;
};

#endif
