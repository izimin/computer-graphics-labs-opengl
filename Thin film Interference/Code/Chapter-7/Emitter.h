//
// Simple emitter of particles
//

#ifndef	__EMITTER__
#define	__EMITTER__

#include	"ParticleSystem.h"

class	Emitter : public ParticleSystem
{
protected:
	Vector4D	color;
	float		minLifeTime;
	float		maxLifeTime;
	float		velCoeff;

public:
	Emitter ( int theMaxParticles, float theBirthRate, const Vector3D& thePos, const Vector4D& theColor,
			 float theMinLife, float theMaxLife ) : ParticleSystem ( theMaxParticles, theBirthRate, thePos )
	{
		color       = theColor;
		minLifeTime = theMinLife;
		maxLifeTime = theMaxLife;
		velCoeff    = 0.2;
		size        = 20;
	}

	virtual	void	update ()
	{
		ParticleSystem :: update ();

		for ( register int i = 0; i < maxParticles; i++ )
			if ( particle [i].alive )
				particle [i].color.w = 1 - particle [i].time / particle [i].lifeTime;
	}

	virtual	void	createParticle ( Particle& p )
	{
		p.pos      = pos;
		p.color    = color;
		p.velocity = Vector3D :: getRandomVector ( velCoeff );
		p.lifeTime = rnd ( minLifeTime, maxLifeTime );
	}
};

#endif
