
#include	"libExt.h"
#include	"Torus.h"

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

		// compute unit normal

			vertices [index].n.x =  cosTheta * cosPhi;
			vertices [index].n.y = -sinTheta * cosPhi;
			vertices [index].n.z = sinPhi;

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

void	Torus :: draw ()
{
	glBegin ( GL_TRIANGLES );

	for ( int i = 0; i < numFaces; i++ )
		for ( int j = 0; j < 3; j++ )
		{
			int	k = faces [i].index [j];

			glNormal3fv        ( vertices [k].n   );
			glMultiTexCoord2fv ( GL_TEXTURE0_ARB, vertices [k].tex );
			glMultiTexCoord2fv ( GL_TEXTURE1_ARB, vertices [k].tex );
			glMultiTexCoord2fv ( GL_TEXTURE2_ARB, vertices [k].tex );
			glVertex3fv        ( vertices [k].pos );
		}

	glEnd   ();
}

//////////////////////////// end of torus methods //////////////////////////////////
