
#include	"libExt.h"
#include	"Torus.h"
#include	"Vector4D.h"
#include	"Matrix3D.h"
											// compute transform matrix for
											// reflective bump mapping
void	Vertex :: buildTransformMatrix ( const Matrix3D& mInvT, Matrix3D& tr ) const
{
	Matrix3D	s ( t, b, n );

	tr = mInvT * s;
}

///////////////////////////////////////////////////////////////////////////////////

Torus :: Torus ( float r1, float r2, int rings, int sides )
{
	float 	ringDelta = 2.0 * M_PI / rings;
	float 	sideDelta = 2.0 * M_PI / sides;
	float	invRings  = 1.0f / (float) rings;
	float	invSides  = 1.0f / (float) sides;
	int 	i, j;

	numRings = rings;
	numSides = sides;
	vertices = new Vertex [(numSides+1)*(numRings+1)];
	faces    = new Face   [numSides*numRings*2];

								// create vertices
	int	index = 0;

	for ( i = 0; i <= rings; i++ )
	{
    	float theta    = i * ringDelta;
	    float cosTheta = cos ( theta );
    	float sinTheta = sin ( theta );

		for ( j = 0; j <= sides; j++ )
		{
			float phi    = j * sideDelta;
			float cosPhi = cos ( phi );
			float sinPhi = sin ( phi );
			float dist = r2 + r1 * cosPhi;

      	// (x,y,z) below is the parametric equation for a torus
        // when theta and phi both vary from 0 to pi.

      				// x = cos(theta) * (R + r * cos(phi))
      				// y = -sin(theta) * (R + r * cos(phi))
      				// z = r * sin(phi)

      		vertices [index].pos.x = cosTheta * dist;
      		vertices [index].pos.y = -sinTheta * dist;
      		vertices [index].pos.z = r1 * sinPhi;

      // now setup texture coordinates

			vertices [index].tex.x = j * invSides;
      		vertices [index].tex.y = i * invRings;

      // normalize the partial derivative of (x,y,z) with respect to theta.
      // T = normalize([dx/dtheta,dy/dtheta,dz/dtheta])

      		float	dxdtheta = -sinTheta * dist;
			float	dydtheta = -cosTheta * dist;
			float	d        = 1 / sqrt ( dxdtheta*dxdtheta + dydtheta*dydtheta );

      		vertices [index].t.x = dxdtheta * d;
			vertices [index].t.y = dydtheta * d;
			vertices [index].t.z = 0;

		// compute unit normal

			vertices [index].n.x =  cosTheta * cosPhi;
			vertices [index].n.y = -sinTheta * cosPhi;
			vertices [index].n.z = sinPhi;

		// B = N cross T

			vertices [index].b = vertices [index].n ^ vertices [index].t;

		// Note:  by the construction above, [T, B, N] form an orthonormal space.

      		index++;
		}
	}

	numVertices = index;

											// Create faces
	index = 0;

	for ( i = 0; i < rings; i++ )
		for ( j = 0; j < sides; j++, index += 2 )
		{
			faces [index].index [0] = i*(sides+1) + j;
			faces [index].index [1] = (i+1)*(sides+1) + j;
			faces [index].index [2] = (i+1)*(sides+1) + j + 1;

			faces [index+1].index [0] = i*(sides+1) + j;
			faces [index+1].index [1] = (i+1)*(sides+1) + j + 1;
			faces [index+1].index [2] = i*(sides+1) + j + 1;
		}

	numFaces = index;
}

void	Torus :: draw ( const Vector3D& eye )
{
											// get modelview matrix from OpenGL
	float	mGl [16];

	glGetFloatv ( GL_MODELVIEW_MATRIX, mGl );

											// now extract 3*3 submatrix from it, remember OpenGL matrices
											// are column-ordered
	Matrix3D	mv;

	mv [0][0] = mGl [0];
	mv [0][1] = mGl [4];
	mv [0][2] = mGl [8];

	mv [1][0] = mGl [1];
	mv [1][1] = mGl [5];
	mv [1][2] = mGl [9];

	mv [2][0] = mGl [2];
	mv [2][1] = mGl [6];
	mv [2][2] = mGl [10];

	mv.invert    ();
	mv.transpose ();

	Matrix3D	t;

	glBegin ( GL_TRIANGLES );

	for ( int i = 0; i < numFaces; i++ )
		for ( int j = 0; j < 3; j++ )
		{
			Vertex&	v = vertices [faces [i].index [j]];

			v.buildTransformMatrix ( mv, t );

			Vector4D	t1 ( t [0][0], t [0][1], t [0][2], eye.x );
			Vector4D	t2 ( t [1][0], t [1][1], t [1][2], eye.y );
			Vector4D	t3 ( t [2][0], t [2][1], t [2][2], eye.z );

			glMultiTexCoord2fv ( GL_TEXTURE0_ARB, v.tex );
			glMultiTexCoord3fv ( GL_TEXTURE1_ARB, t1    );
			glMultiTexCoord2fv ( GL_TEXTURE2_ARB, t2    );
			glMultiTexCoord3fv ( GL_TEXTURE3_ARB, t3    );
		  	glVertex3fv        ( v.pos                  );
		}

	glEnd   ();
}

//////////////////////////// end of torus methods //////////////////////////////////
