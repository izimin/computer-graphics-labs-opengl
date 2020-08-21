//
// Simple torus class
//

#include	"Vector3D.h"
#include	"Vector2D.h"

#ifndef	__TORUS__
#define	__TORUS__

struct	Vertex
{
	Vector3D	pos;						// position of vertex
	Vector2D	tex;						// texture coordinates
	Vector3D	n;							// unit normal
	Vector3D	t, b;						// tangent and binormal
	Vector3D	l;							// light vector in the tangent space

											// map vector to tangent (TBN) space
	Vector3D	mapToTangentSpace ( const Vector3D& v ) const
	{
		return Vector3D ( v & t, v & b, v & n );
	}
};

struct	Face								// triangular face
{
	int	index [3];							// indices to Vertex array
};

class	Torus
{
	int		 numRings;
	int		 numSides;
	int		 numVertices;
	int		 numFaces;
	Vertex * vertices;
	Face   * faces;

public:
	Torus  ( float r1, float r2, int rings, int sides );
	~Torus ()
	{
		delete [] vertices;
		delete [] faces;
	}

	void	calcLightVectors ( const Vector3D& light );
	void	draw             ();
};

#endif
