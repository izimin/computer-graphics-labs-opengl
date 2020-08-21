//
// Class to load images from DDS files
//
// Author: Alex V. Boreskoff
//

#ifndef	__DDS_LOADER__
#define	__DDS_LOADER__

#include	"TypeDefs.h"

class	Data;
class	Texture;

class	DdsLoader
{
public:
	DdsLoader () {}

	Texture * load ( Data * data );
};

#endif
