//
// Class to load images from PNG files
//
// Author: Alex V. Boreskoff
//

#ifndef	__PNG_LOADER__
#define	__PNG_LOADER__

#include	"TypeDefs.h"

class	Data;
class	Texture;

class	PngLoader
{
public:
	PngLoader () {}

	Texture * load ( Data * data );
};

#endif
