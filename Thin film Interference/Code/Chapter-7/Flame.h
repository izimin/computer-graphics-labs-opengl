//
// Simple flame particle system
//

#ifndef	__FLAME_SYSTEM__
#define	__FLAME_SYSTEM__

#include	"ParticleSystem.h"
#include	"ColorSpline.h"

class	Flame : public ParticleSystem
{
protected:
	float		radius;
	float		randCoeff;
	float		centerCoeff;
	ColorSpline	colors;

public:
	Flame ( int theMaxParticles, float theBirthRate, const Vector3D& thePos, float r, float r1, float r2 ) :
	    ParticleSystem ( theMaxParticles, theBirthRate, thePos )
	{
		radius      = r;
		randCoeff   = r1;
		centerCoeff = r2;
		size        = 15;

		colors.addColorAt ( Vector4D ( 0,   0.3, 1, 0.5 ), 0    );
		colors.addColorAt ( Vector4D ( 0,   0.7, 1, 1   ), 0.15 );
		colors.addColorAt ( Vector4D ( 1,   1,   0, 1   ), 0.2  );
		colors.addColorAt ( Vector4D ( 1,   0,   0, 1   ), 0.8  );
		colors.addColorAt ( Vector4D ( 1,   0,   0, 0   ), 1    );
	}


	virtual	void	createParticle ( Particle& p )
	{
		p.pos      = pos + Vector3D :: getRandomVector ( radius );
		p.lifeTime = 1.5;
		p.velocity = Vector3D ( 0, 0, 0 );
	}

	virtual	void	update ()
	{
		float	saveTime = lastUpdateTime;				// will be changed in inherited method

		ParticleSystem :: update ();

		float	delta = lastUpdateTime - saveTime;		// interval in milliseconds since last update

		for ( register int i = 0; i < maxParticles; i++ )
		{
			if ( !particle [i].alive )
				continue;
													// create random variation
			Vector3D	v = Vector3D :: getRandomVector ( randCoeff );

			v.z = 1;								// particle allways goes up


			particle [i].pos += delta * v;

													// now move projection of particle to
													// center (in Oxy plane)
			float d  = centerCoeff * delta * (particle [i].pos.z - pos.z);
			float dx = (particle [i].pos.x - pos.x) * d;
			float dy = (particle [i].pos.y - pos.y) * d;

			particle [i].pos.x -= dx;
			particle [i].pos.y -= dy;

													// compute color
			particle [i].color = colors.valueAt ( particle [i].time / particle [i].lifeTime );
		}
	}
};

#endif
