//
// Class to load images from BMP files
//
// Author: Alex V. Boreskoff
//

#ifndef	__BMP_LOADER__
#define	__BMP_LOADER__

#include	"TypeDefs.h"

class	Data;
class	Texture;

class	BmpLoader
{
public:
	BmpLoader () {}

	Texture * load ( Data * data );

private:
	bool	loadMono  ( Data * data, Texture * tex, dword * buf );
	bool	loadRGB4  ( Data * data, Texture * tex, dword * but, dword palette [] );
	bool	loadRGB8  ( Data * data, Texture * tex, dword * buf, dword palette [] );
	bool	loadRGB24 ( Data * data, Texture * tex, dword * buf );
	bool	loadRGB32 ( Data * data, Texture * tex, dword * buf );
	bool	loadRLE4  ( Data * data, Texture * tex, dword * buf, dword palette [] );
	bool	loadRLE8  ( Data * data, Texture * tex, dword * buf, dword palette [] );
};

#endif
