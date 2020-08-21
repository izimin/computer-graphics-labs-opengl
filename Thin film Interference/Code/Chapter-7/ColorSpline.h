//
// Simple color spline class to interpolate colors for parameter in [0,1]
//
// Author: Alex V. Boreskoff <steps3d@narod.ru>, <alexboreskoff@mtu-net.ru>
//

#ifndef	__COLOR_SPLINE__
#define	__COLOR_SPLINE__

#include	"Vector4D.h"

#define	MAX_COLOR_SPLINE_ENTRIES	20

class	ColorSpline
{
	struct Entry
	{
		float	 t;					// parameter value
		Vector4D color;				// corresponding color value
		Vector4D delta;				// delta value used in interpolation
	};

	Entry	entries [MAX_COLOR_SPLINE_ENTRIES];
	int		numEntries;

public:
	ColorSpline ()
	{
		numEntries = 0;
	}

	void	addColorAt ( const Vector4D& color, float t )
	{
		if ( numEntries >= MAX_COLOR_SPLINE_ENTRIES )
			return;

									// store the entry
		entries [numEntries].t     = t;
		entries [numEntries].color = color;

									// compute delta for prev. entry
		if ( numEntries > 0 )
			entries [numEntries-1].delta = (entries [numEntries].color - entries [numEntries-1].color) /
									       (entries [numEntries].t - entries [numEntries-1].t);

		numEntries++;
	}

	Vector4D	valueAt ( float t ) const
	{
		if ( t < entries [0].t )
			return entries [0].color;

		for ( register int i = 1; i < numEntries; i++ )
			if ( t <= entries [i].t )
				return entries [i-1].color + (t - entries [i-1].t) * entries [i-1].delta;

		return entries [numEntries - 1].color;
	}
};

#endif

