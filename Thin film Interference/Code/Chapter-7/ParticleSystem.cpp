//
// Class for simple particle system.
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#include	<string.h>

#include	"libExt.h"
#include	"ParticleSystem.h"

#ifdef	MACOSX
	#include	<GLUT/glut.h>
#else
	#include	<glut.h>
#endif


ParticleSystem :: ParticleSystem ( int theMaxParticles, float theBirthRate, const Vector3D& thePos, unsigned id )
{
	maxParticles   = theMaxParticles;
	particle       = new Particle [maxParticles];
	numParticles   = 0;
	birthRate      = theBirthRate;
	pos            = thePos;
	textureId      = id;
	lastUpdateTime = 0.001f * glutGet ( GLUT_ELAPSED_TIME );
	lastCreateTime = lastUpdateTime;
	srcBlend       = GL_SRC_ALPHA;
	dstBlend       = GL_ONE;

	memset ( particle, '\0', maxParticles * sizeof ( Particle ) );
}

ParticleSystem :: ~ParticleSystem ()
{
	delete particle;
}

void	ParticleSystem :: update ()
{
	float	curTime = 0.001f * glutGet ( GLUT_ELAPSED_TIME );
	float	delta   = curTime - lastUpdateTime;

	register int	i, j;

	for ( i = 0; i < maxParticles; i++ )
		if ( particle [i].alive )
		{
			particle [i].time += delta;

			if ( particle [i].time >= particle [i].lifeTime )
			{
				particle [i].alive = false;

				continue;
			}

			particle [i].pos += delta * particle [i].velocity;
		}

	lastUpdateTime = curTime;
											// now add enough particles
	int	newParticles = (int)( (curTime - lastCreateTime) * birthRate + 0.5f );

	for ( i = j = 0; i < newParticles; i++ )
	{
											// find free slot
		while ( j < maxParticles && particle [j].alive )
			j++;

		if ( j >= maxParticles )
			return;

											// setup some default values
		particle [j].alive = true;
		particle [j].time  = 0;
		particle [j].mass  = 1;

		createParticle ( particle [j] );
	}

	if ( newParticles > 0 )
		lastCreateTime = curTime;
}

void	ParticleSystem :: render ()
{
	glPushAttrib  ( GL_ENABLE_BIT | GL_POINT_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable      ( GL_TEXTURE_2D );
	glDepthMask   ( GL_FALSE );
	glBindTexture ( GL_TEXTURE_2D, textureId );
	glEnable      ( GL_BLEND );
	glBlendFunc   ( srcBlend, dstBlend );

    float quadratic [] =  { 1.0f, 0.0f, 0.01f };

    glEnable              ( GL_POINT_SPRITE_ARB );
    glPointParameterfvARB ( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
    glPointParameterfARB  ( GL_POINT_FADE_THRESHOLD_SIZE_ARB,  20.0f );

    glTexEnvf   ( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    glPointSize ( size );

	glBegin( GL_POINTS );

    for( int i = 0; i < maxParticles; i++ )
    	if ( particle [i].alive )
    	{
    		glColor4fv  ( particle [i].color );
	        glVertex3fv ( particle [i].pos   );
        }

	glEnd();

	glPopAttrib ();
}
