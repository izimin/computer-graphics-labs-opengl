//
// Class for simple particle system.
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#ifndef	__PARTICLE_SYSTEM__
#define	__PARTICLE_SYSTEM__

#include	<stdlib.h>
#include	"Vector3D.h"
#include	"Vector4D.h"

struct	Particle
{
	Vector3D	pos;
	Vector4D	color;
	Vector3D	velocity;
	float		mass;
	float		time;
	float		lifeTime;
	Vector3D	force;						// accumulate force by effectors here
	bool		alive;
};

class	ParticleEffector;

class	ParticleSystem
{
protected:
	Particle  * particle;
	int		    maxParticles;
	int		    numParticles;
	float	    lastUpdateTime;
	float	    lastCreateTime;
	float		birthRate;
	float	    size;
	Vector3D	pos;
	int			srcBlend, dstBlend;
	unsigned	textureId;

public:
	ParticleSystem ( int theMaxParticles, float theBirthRate, const Vector3D& thePos, unsigned id = 0 );
	virtual ~ParticleSystem ();

	void	setTexture ( unsigned id )
	{
		textureId = id;
	}

	virtual	void	update         ();
	virtual	void	render         ();
	virtual	void	createParticle ( Particle& ) = 0;
};

class	ParticleEffector
{
public:
	ParticleEffector () {}
	virtual ~ParticleEffector () {}

														// should accumulate force in p.force field
	virtual	void	apply ( Particle * particles, int maxParticles, float timeElapsed ) = 0;
};

inline	float	rnd ( float a, float b )
{
	return a + (float) rand () * ((b - a) / RAND_MAX);
}

#endif
