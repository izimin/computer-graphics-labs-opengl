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
											// map vector to tangent (TBN) space
	Vector3D	mapToTangentSpace ( const Vector3D& v ) const
	{
		return Vector3D ( v & t, v & b, v & n );
	}
};

struct	LightVectors
{
	Vector3D	l;							// light vector in the tangent space
	Vector3D	h;
};

struct	Face								// triangular face
{
	int	index [3];							// indices to Vertex array
};

class	Matrix4x4;

class	Torus
{
	int		 		numRings;
	int		 		numSides;
	int		 		numVertices;
	int		 		numFaces;
	Vertex        * vertices;
	LightVectors  * lh;
	Face          * faces;
	GLuint          vertexId;						// id of vertex VBO
	GLuint          indexId;						// id of index VBO
	GLuint          lhId;							// id of l-h VBO

public:
	Torus  ( float r1, float r2, int rings, int sides );
	~Torus ()
	{
		delete [] vertices;
		delete [] faces;

		glDeleteBuffersARB ( 1, &vertexId );
		glDeleteBuffersARB ( 1, &indexId  );
		glDeleteBuffersARB ( 1, &lhId     );
	}

	void	setupBuffers     ();
	void	calcLightVectors ( const Vector3D& light );
	void	calcLightVectors ( const Vector3D& light, const Vector3D& eye, const Matrix4x4& m );
	void	draw             ();
};

#endif
