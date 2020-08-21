//
// Class to load images from jpeg files
//
// Author: Alex V. Boreskoff
//

#ifndef	__JPEG_LOADER__
#define	__JPEG_LOADER__

#include	"TypeDefs.h"

class	Data;
class	Texture;

class	JpegLoader
{
public:
	JpegLoader () {}

	Texture * load ( Data * data );
};

#endif
