//
// Wrapping of Noise class into a python module
//
// Author: Alex V. Boreskoff
//

#include	"noise.h"
#include	"python.h"

static	Noise	noiseObject;

static	PyObject * noise ( PyObject * self, PyObject * args )
{
	float	x, y, z;
	
	if ( !PyArg_ParseTuple ( args, "fff", &x, &y, &z ) )
		return NULL;
		
	float	res = noiseObject.noise ( Vector3D ( x, y, z ) );
	
	return Py_BuildValue ( "f", res );
}

static	PyObject * turbulence ( PyObject * self, PyObject * args )
{
	float	x, y, z;
	int		octaves;
	
	if ( !PyArg_ParseTuple ( args, "fffi", &x, &y, &z, &octaves ) )
		return NULL;
		
	float	res = noiseObject.turbulence ( Vector3D ( x, y, z ), octaves );
	
	return Py_BuildValue ( "f", res );
}

static	PyObject * signedTurbulence ( PyObject * self, PyObject * args )
{
	float	x, y, z;
	float	minFreq, maxFreq;
	
	if ( !PyArg_ParseTuple ( args, "fffff", &x, &y, &z, &minFreq, &maxFreq ) )
		return NULL;
		
	float	res = noiseObject.signedTurbulence ( Vector3D ( x, y, z ), minFreq, maxFreq );
	
	return Py_BuildValue ( "f", res );
}

static	PyObject * fBm ( PyObject * self, PyObject * args )
{
	float	x, y, z;
	float 	h, lacunarity, octaves;
	
	if ( !PyArg_ParseTuple ( args, "ffffff", &x, &y, &z, &h, &lacunarity, &octaves ) )
		return NULL;
		
	float	res = noiseObject.fBm( Vector3D ( x, y, z ), h, lacunarity, octaves );
	
	return Py_BuildValue ( "f", res );
}

static	PyMethodDef	noiseMethods [] =
{
	{ "noise",       noise,            METH_VARARGS, "Compute noise for a 3D point" },
	{ "turbulence",  turbulence,       METH_VARARGS, "Compute a turbulense"         },
	{ "sturbulence", signedTurbulence, METH_VARARGS, "Compute a signed turbulense"  },
	{ "fBm",         fBm,              METH_VARARGS, "Compute fBm function"         },
	{ NULL,          NULL,             0,            NULL                           }
};

/*DL_EXPORT(void)*/ void	initnoise ()
{
	(void) Py_InitModule ( "noise", noiseMethods );
}
