#include	"libExt.h"
#include	"Torus-1.h"
#include	"Matrix4x4.h"

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
			faces [index].index [2] = i*(sides+1) + j;
			faces [index].index [1] = (i+1)*(sides+1) + j;
			faces [index].index [0] = (i+1)*(sides+1) + j + 1;

			faces [index+1].index [2] = i*(sides+1) + j;
			faces [index+1].index [1] = (i+1)*(sides+1) + j + 1;
			faces [index+1].index [0] = i*(sides+1) + j + 1;
		}

	numFaces = index;
}

void	Torus :: calcLightVectors ( const Vector3D& light )
{
									// compute texture coordinates for normalization cibe map
	for ( int i = 0; i < numVertices; i++ )
		vertices [i].l = vertices [i].mapToTangentSpace ( light - vertices [i].pos );
}

void	Torus :: calcLightVectors ( const Vector3D& light, const Vector3D& eye, const Matrix4x4& m )
{
	Vector3D	lt = m.transformPoint ( light );
	Vector3D	et = m.transformPoint ( eye   );

									// compute texture coordinates for
									// normalization cube map
	for ( int i = 0; i < numVertices; i++ )
	{
		Vector3D	l = (lt - vertices [i].pos).normalize ();
		Vector3D	v = (et - vertices [i].pos).normalize ();


		vertices [i].l = vertices [i].mapToTangentSpace ( l );
		vertices [i].h = vertices [i].mapToTangentSpace ( l + v );
	}
}

void	Torus :: draw ()
{
	glPushClientAttrib  ( GL_CLIENT_VERTEX_ARRAY_BIT );

												// setup vertices array
	glEnableClientState ( GL_VERTEX_ARRAY );

	glVertexPointer ( 3, GL_FLOAT, sizeof ( Vertex ), &vertices [0].pos );

												// setup texcoord0 array
	glClientActiveTextureARB ( GL_TEXTURE0_ARB );
	glEnableClientState      ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer	     ( 2, GL_FLOAT, sizeof ( Vertex ), &vertices [0].tex );

												// setup l array
	glClientActiveTextureARB ( GL_TEXTURE1_ARB );
	glEnableClientState      ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer	     ( 3, GL_FLOAT, sizeof ( Vertex ), &vertices [0].h );

												// setup tex0 array
	glClientActiveTextureARB ( GL_TEXTURE2_ARB );
	glEnableClientState      ( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer	     ( 2, GL_FLOAT, sizeof ( Vertex ), &vertices [0].tex );

	glDrawElements ( GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, &faces [0] );

	glPopClientAttrib ();
}

//////////////////////////// end of torus methods //////////////////////////////////
