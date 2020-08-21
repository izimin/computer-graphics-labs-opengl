//
// Simple library to load basic textures format
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#ifndef	__LIB_TEXTURE__
#define	__LIB_TEXTURE__

#include	"TypeDefs.h"

class	FileSystem;
class	Data;
							// create corresponding textures from file (or build them)
GLuint	createNormalizationCubemap ( int cubeSize, bool mipmap = false );
GLuint	createCubeMap   ( bool mipmap, const char * maps [] );
GLuint	createTexture2D ( bool mipmap, const char * fileName );
GLuint	createTexture1D ( bool mipmap, const char * fileName );

							// create normalmaps from heightmap and from
							// from normal map textures
GLuint	createNormalMapFromHeightMap ( bool mipmap, const char * fileName, float scale );
GLuint	createNormalMap              ( bool mipmap, const char * fileName );

							// load texture from file into given texture target
bool		loadTexture ( int target, bool mipmap, const char * fileName );

							// simple check whether the file exists
bool		fileExist ( const char * fileName );

							// save contents of the window in a TGA file
bool		saveScreenShot ( const char * fileName );

							// add file system
bool		addFileSystem ( FileSystem * fileSystem );

							// add zip file source
bool		addZipFileSystem ( const char * fileName );

							// get file contents (possibly scanning several sources)
Data * 		getFile ( const char * fileName );

#endif
