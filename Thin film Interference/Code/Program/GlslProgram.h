//
// Class to encapsulate GLSL program and shader objects and working with them
//
// Author: Alex V. Boreskoff <alexboreskoff@mtu-net.ru>, <steps3d@narod.ru>
//

#ifndef __GLSL_PROGRAM__
#define __GLSL_PROGRAM__

#include    "libExt.h"

#include    <string>

using namespace std;

class   Vector2D;
class   Vector3D;
class   Vector4D;
class   Matrix3D;
class   Matrix4x4;
class   Data;

class   GlslProgram
{
protected:
    GLhandleARB program;                            // program object handle
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;
    bool        ok;                                 // whether program is loaded and ready to be used
    string      glError;
    string      log;

public:
    GlslProgram  ();
    ~GlslProgram ();

                                                    // load shaders
    bool    loadShaders ( const char * vertexFileName, const char * fragmentFileName );
    bool    loadShaders ( Data * vertexShaderData, Data * fragmentShaderData         );

                                                    // remove all shaders and free all objects
    void    clear ();


    string  getLog     () const                         // get current log
    {
        return log;
    }

    bool    isOk () const                               // whether shader is ok
    {
        return ok;
    }

    string  getGlError () const
    {
        return glError;
    }

    void    bind   ();
    void    unbind ();

                                                        // uniform variables handling methods
    bool        setUniformVector  ( const char * name, const Vector4D& value  );
    bool        setUniformVector  ( int loc,            const Vector4D& value  );
    bool        setUniformVector  ( const char * name, const Vector3D& value  );
    bool        setUniformVector  ( int loc,            const Vector3D& value  );
    bool        setUniformVector  ( const char * name, const Vector2D& value  );
    bool        setUniformVector  ( int loc,            const Vector2D& value  );
    bool        setUniformFloat   ( const char * name, float value            );
    bool        setUniformFloat   ( int loc,            float value            );
    bool        setUniformMatrix  ( const char * name, const Matrix4x4& value );
    bool        setUniformMatrix  ( const char * name, const Matrix3D&  value );
    bool        setUniformInt     ( const char * name, int value              );
    bool        setUniformInt     ( int loc,            int value              );
    Vector4D    getUniformVector  ( const char * name );
    Vector4D    getUniformVector  ( int loc            );
    int         locForUniformName ( const char * name );

                                                        // attribute variables handling methods
    bool        setAttribute     ( const char * name, const Vector4D& value );
    bool        setAttribute     ( int index,         const Vector4D& value );
    Vector4D    getAttribute     ( const char * name );
    Vector4D    getAttribute     ( int index );
    int         indexForAttrName ( const char * name );
    bool        bindAttributeTo  ( int no, const char * name );

    bool    setTexture ( const char * name, int texUnit );
    bool    setTexture ( int loc,           int texUnit );

                                                        // check whether there is a support for GLSL
    static  bool    isSupported ();
    static  string  version     ();
                                                        // some limitations on program
    static  int maxVertexUniformComponents   ();
    static  int maxVertexAttribs             ();
    static  int maxFragmentTextureUnits      ();
    static  int maxVertexTextureUnits        ();
    static  int maxCombinedTextureUnits      ();
    static  int maxVaryingFloats             ();
    static  int maxFragmentUniformComponents ();
    static  int maxTextureCoords             ();

protected:
    bool    loadShader   ( GLhandleARB shader, Data * data );
    bool    checkGlError ();
    void    loadLog      ( GLhandleARB object );
};

#endif
