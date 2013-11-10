#ifndef OGLES_H_INCLUDED
#define OGLES_H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : ogles.h
// Purpose : GLES Include Files
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

// --------------------------------------------------------------------------
// GLES include files
// --------------------------------------------------------------------------

#include "GLES/gl.h"
#include "GLES/egl.h"
#include "GLES/egltypes.h"
#include "GLES/glext.h"
#include "stdafx.h"

// --------------------------------------------------------------------------
// API VERSION DESCRIPTION
// --------------------------------------------------------------------------

#ifndef EGL_RELEASE
#	define EGL_RELEASE				"1.0.0"
#endif

#define EGL_CONFIG_VENDOR			"CASLAB OGLES"

#ifdef COMMON_LITE
#	define EGL_CONFIG_VERSION			"OpenGL ES-CL 1.1"
#else
#	define EGL_CONFIG_VERSION			"OpenGL ES-CM 1.1"
#endif

#define EGL_CONFIG_RENDERER		    "Software"
#define EGL_VERSION_NUMBER          EGL_RELEASE "alpha"

// --------------------------------------------------------------------------
// Configuration Options
// --------------------------------------------------------------------------

#define EGL_NUM_TEXTURE_UNITS		2
#define EGL_MAX_LEVELS              8
#define EGL_NUM_LIGHTING            8
#define EGL_MIPMAP_PER_TEXEL		0

// number of varying variables: RGBA + fog + tu/tv per texture unit
#define EGL_MAX_NUM_VARYING			(4 + 1 + 2 * EGL_NUM_TEXTURE_UNITS)


#define ELM4(row,col) (col*4+row)
#define ELM3(row,col) (col*3+row)
#define PI 3.14159265358979323846

// parameters for GPU simulator
#define WINDOW_DEFAULT_WIDTH       640
#define WINDOW_DEFAULT_HEIGHT      480
#define VPNEAR 	    0.0
#define VPFAR 	    1.0

#define TILEWIDTH   64
#define TILEHEIGHT  32

#define TEX_CACHE_BLOCK_SIZE_ROOT_LOG   2
#define TEX_CACHE_BLOCK_SIZE_ROOT       4
#define TEX_CACHE_BLOCK_SIZE            16
#define TEX_CACHE_ENTRY_X_LOG           2
#define TEX_CACHE_ENTRY_X               4
#define TEX_CACHE_ENTRY_Y_LOG           2
#define TEX_CACHE_ENTRY_Y               4

//#define TEX_CACHE_SIZE 512
//#define TEX_PREFETCH_MODE 1 //READ NEXT FOUR TEXEL IN 0:LINEAR 1:BLOCK

#define TILELINE            0
#define RANDOM_TEST         0
#define RANDOM_TEST_TIME    10
#define USE_PIXELSTAMP      0

// --------------------------------------------------------------------------
// DEBUG & Statistics Report Options
// --------------------------------------------------------------------------
//#define PRINT_TRANSFORMATION
//#define UGLIBDEG
//#define COLORBUF_DEG
#define APIFUNC_DEG
#define GENERATE_STAT_CSV

// RM's debug option
#define TEXDEBUG                0
#define PIXEL_GENERATE_DEBUG    0
#define PIXEL_DATA_DEBUG        0
#define SCAN_ALGO_DEBUG         0
// GM's debug option
#define TRANDEBUG           1
#define MATRIXDEBUG         0
#define LIGHTINGDEBUG 	    0
#define TEXTRANDEBUG  	    0
#define FOGCALBUG	        0
#define PERSPECTIVEBUG	    0
#define VTXINDEXBUG			0
#define PRIMASSBUG			0
#define TriVtxABCBUG		1
#define PRECLIPPINGBUG      1
#define CHECKPRECLIPPING    1
#define VIEWPORTBUG			1
#define VERBUFFDEBUG		1
#define TILEDIVIDERDEG      1
#define CheckorderBUG       0
#define KICKSMALLAREA       1

#define FRAMEDUMP           1

//// tile size
//#define TB32
//
//#ifdef TB32
//#define LOG_RASTER_BLOCK_SIZE   5
//#define RASTER_BLOCK_SIZE       32
//#endif
//#ifdef TB16
//#define LOG_RASTER_BLOCK_SIZE   4
//#define RASTER_BLOCK_SIZE       16
//#endif
//#ifdef TB8
//#define LOG_RASTER_BLOCK_SIZE   3
//#define RASTER_BLOCK_SIZE       8
//#endif

#define TILE_COUNT (const int)(((int)WINDOW_DEFAULT_WIDTH/(int)TILEWIDTH)*((int)WINDOW_DEFAULT_HEIGHT/(int)TILEHEIGHT))

// Floating type simulation or Fixed
// TO DO - use class template instead of burte force
#define __GL_FLOAT
//#define __GL_FIXED

/* single precision fixed point type */

/*typedef int GLfixedOES;*/
/*typedef int GLclampxOES;*/
#define __GL_X_FRAC_BITS	16
#define __GL_X_INT_BITS		16
#define __GL_X_ZERO		0x00000		/* S15.16 0.0 */
#define __GL_X_ONE		0x10000		/* S15.16 1.0 */
#define __GL_X_TWO		0x20000		/* S15.16 2.0 */
#define __GL_X_HALF		0x08000		/* S15.16 0.5 */
#define __GL_X_PINF		0x7fffffff	/* +inf */
#define __GL_X_MINF		0x80000000	/* -inf */

/* arithmetic of floating or fixed point */

#define __GL_F_ZERO		(0.0f)
#define __GL_F_ONE		(1.0f)
#define __GL_F_TWO		(2.0f)
#define __GL_F_HALF		(0.5f)
#define __GL_F_ADD(a,b)		((a)+(b))
#define __GL_F_MUL(a,b)		((a)*(b))
#define __GL_F_DIV(a,b)		((a)/(b))
#define __GL_F_INT(a)		((GLint)(a))
#define __GL_X_2_F(a)		((GLfloat)(a)/(1<<__GL_X_FRAC_BITS))
#define __GL_F_DEG_2_RAD(x)	__GL_F_MUL(x,(PI/180.f))
#define __GL_FLOAT_2_F(a)	(a)
#define __GL_F_2_FLOAT(a)	(a)
#define __GL_INT_2_F(a)		((GLfloat)(a))
#define __GL_Det2X2(a, b, c, d) ((a*d) - (c*b))
//#define __GL_FLOAT_2_X(a)	((GLfixed)((a)*(1<<__GL_X_FRAC_BITS)))
#define __GL_FLOAT_2_X(f)   static_cast<GLfixed>((f) >= 32768.0 ? 0x7ffffff : (f) < -32768.0 ? 0x80000000 : (f)*static_cast<float>(__GL_X_ONE))
#define __GL_F_2_CLAMPF(a)	((a) < __GL_F_ZERO ? __GL_F_ZERO : (a) > __GL_F_ONE ? __GL_F_ONE : (a))

#define __GL_FLOAT_2_S(a)   ((GLushort)((a)*(1<<8)))
#define __GL_S_2_FLOAT(a)   ((GLfloat)(a)/(1<<8))

#define __GL_X_ADD(a,b) 	((a)+(b))
#define __GL_X_SUB(a,b) 	((a)-(b))
#define __GL_X_MUL(a,b)		((GLfixed)((((long long)(a))*(b))>>__GL_X_FRAC_BITS))
#define __GL_X_DIV(a,b) 	((GLfixed)((((long long)(a))<<__GL_X_FRAC_BITS)/(b)))
#define __GL_X_DIVI(a,b) 	((GLfixed)((a)/(b)))
#define __GL_X_INT(a)		((a) >= 0 ? (a)>>__GL_X_FRAC_BITS : -(-(a)>>__GL_X_FRAC_BITS))
#define __GL_X_INTP(a)		((a)>>__GL_X_FRAC_BITS) //  a/= 2^16
#define __GL_X_FLOOR(a)		((a)&~((1<<__GL_X_FRAC_BITS)-1))
#define __GL_X_FRAC(a)		((a)-__GL_X_FLOOR(a))
#define __GL_X_TRUNC(a) 	__GL_INT_2_X(__GL_X_INT(a))
#define __GL_X_CEIL(a) 		__GL_X_FLOOR((a)+((1<<__GL_X_FRAC_BITS)-1))
#define __GL_X_RND(a)		__GL_X_INT((a)+__GL_X_HALF)
#define __GL_X_ICEIL(a)		__GL_X_INT((a)+((1<<__GL_X_FRAC_BITS)-1))
#define __GL_X_2_FLOAT(a)	((GLfloat)(a)/(1<<__GL_X_FRAC_BITS))
#define __GL_X_2_CLAMPX(a)	((a) < __GL_X_ZERO ? __GL_X_ZERO : (a) > __GL_X_ONE ? __GL_X_ONE : (a))
#define __GL_INT_2_X(a)		((GLfixed)((a)<<__GL_X_FRAC_BITS))

#define __GL_X_2PI		    __GL_FLOAT_2_X(6.28318530717958647692f)
#define __GL_X_R2PI		    __GL_FLOAT_2_X(1.0f/6.28318530717958647692f)
#define __GL_X_DEG_2_RAD(x)	__GL_X_MUL(x,__GL_FLOAT_2_X(3.14159265358979323846f/180.f))
#define __GL_X_ENUM(a)		(a)





typedef signed char 		I8;
typedef unsigned char 		U8;
typedef short 				I16;
typedef unsigned short 		U16;
typedef int 				I32;
typedef unsigned int 		U32;
typedef unsigned long long	U64;
typedef long long			I64;



/*
namespace EGL {
	class Context;
    class Config;

	struct TlsInfo {
        TlsInfo();
        ~TlsInfo();
		Context * m_Context;
        Config* m_AllConfigurations;
		I32 m_LastError;
	};
}
*/

// --------------------------------------------------------------------------
// Memory helper macros
// --------------------------------------------------------------------------

// number of elements of an array
#define elementsof(a) (sizeof(a) / sizeof(*(a)))


// --------------------------------------------------------------------------
// Data structure of GL context
// --------------------------------------------------------------------------
struct Matrix4x4
{
#ifdef __GL_FLOAT
    GLfloat     element[16];

    inline Matrix4x4()
    {
        MakeIdentity();
    }

    inline void MakeIdentity()
    {
        element[ELM4(0, 0)] = element[ELM4(1, 1)] = element[ELM4(2, 2)] = element[ELM4(3, 3)] = __GL_F_ONE;
        element[ELM4(0, 1)] = element[ELM4(0, 2)] = element[ELM4(0, 3)] = 0;
        element[ELM4(1, 0)] = element[ELM4(1, 2)] = element[ELM4(1, 3)] = 0;
        element[ELM4(2, 0)] = element[ELM4(2, 1)] = element[ELM4(2, 3)] = 0;
        element[ELM4(3, 0)] = element[ELM4(3, 1)] = element[ELM4(3, 2)] = 0;
    }
#else
    GLfixed     element[16];

    inline Matrix4x4()
    {
        MakeIdentity();
    }

    inline void MakeIdentity()
    {
        element[ELM4(0, 0)] = element[ELM4(1, 1)] = element[ELM4(2, 2)] = element[ELM4(3, 3)] = __GL_X_ONE;
        element[ELM4(0, 1)] = element[ELM4(0, 2)] = element[ELM4(0, 3)] = __GL_X_ZERO;
        element[ELM4(1, 0)] = element[ELM4(1, 2)] = element[ELM4(1, 3)] = __GL_X_ZERO;
        element[ELM4(2, 0)] = element[ELM4(2, 1)] = element[ELM4(2, 3)] = __GL_X_ZERO;
        element[ELM4(3, 0)] = element[ELM4(3, 1)] = element[ELM4(3, 2)] = __GL_X_ZERO;
    }
#endif
};

struct Matrix3x3
{
#ifdef __GL_FLOAT
    GLfloat     element[9];

    inline Matrix3x3()
    {
        MakeIdentity();
    }

    inline void MakeIdentity()
    {
        element[ELM3(0, 0)] = element[ELM3(1, 1)] = element[ELM3(2, 2)] = __GL_F_ONE;
        element[ELM3(0, 1)] = element[ELM3(0, 2)] = 0;
        element[ELM3(1, 0)] = element[ELM3(1, 2)] = 0;
        element[ELM3(2, 0)] = element[ELM3(2, 1)] = 0;
    }
#else
    GLfixed     element[9];

    inline Matrix3x3()
    {
        MakeIdentity();
    }

    inline void MakeIdentity()
    {
        element[ELM3(0, 0)] = element[ELM3(1, 1)] = element[ELM3(2, 2)] = __GL_X_ONE;
        element[ELM3(0, 1)] = element[ELM3(0, 2)] = __GL_X_ZERO;
        element[ELM3(1, 0)] = element[ELM3(1, 2)] = __GL_X_ZERO;
        element[ELM3(2, 0)] = element[ELM3(2, 1)] = __GL_X_ZERO;
    }
#endif
};

struct ArrayPointer
{
    GLboolean   enable;
    GLint       size;
    GLenum      type;
    GLsizei     stride;
    const void  * ptr;
};

struct Vector3
{
#ifdef __GL_FLOAT
    GLfloat     v_x;
    GLfloat     v_y;
    GLfloat     v_z;

    inline Vector3()
    {
        v_x = v_y = v_z = 0;
    }

    inline void Normalize()
    {
        float temp = v_x * v_x + v_y * v_y + v_z * v_z;
        float invLength = 1/sqrt(temp);
        (*this).v_x = v_x * invLength;
        (*this).v_y = v_y * invLength;
        (*this).v_z = v_z * invLength;

    }
#else
    GLfixed     v_x;
    GLfixed     v_y;
    GLfixed     v_z;

    inline Vector3()
    {
        v_x = v_y = v_z = __GL_X_ZERO;
    }

#endif

};

struct Vertex3
{
#ifdef __GL_FLOAT
    GLfloat     x;
    GLfloat     y;
    GLfloat     z;

    inline Vertex3()
    {
        x = y = z = 0;
    }
#else
    GLfixed     x;
    GLfixed     y;
    GLfixed     z;

    inline Vertex3()
    {
        x = y = z = __GL_X_ZERO;
    }
#endif
};

struct Vertex4
{
#ifdef __GL_FLOAT
    GLfloat     x;
    GLfloat     y;
    GLfloat     z;
    GLfloat     w;

    inline Vertex4()
    {
        x = y = z = 0;
        w = 1.0f;
    }
#else
    GLfixed     x;
    GLfixed     y;
    GLfixed     z;
    GLfixed     w;

    inline Vertex4()
    {
        x = y = z = __GL_X_ZERO;
        w = __GL_X_ONE;
    }
#endif
};

struct DrawArrayCmd
{
    GLint       first;
    GLsizei     count;

};

struct DrawElemCmd
{
    GLsizei     count;
    GLenum      type;
    const void  * indices;
};

struct Normal
{
#ifdef __GL_FLOAT
    GLfloat     nx;
    GLfloat     ny;
    GLfloat     nz;

    inline Normal()
    {
        nx = ny = 0;
        nz = 1.0f;
    }
#else
    GLfixed     nx;
    GLfixed     ny;
    GLfixed     nz;

    inline Normal()
    {
        nx = ny = __GL_X_ZERO;
        nz = __GL_X_ONE;
    }
#endif
};

struct TextureCoord
{
#ifdef __GL_FLOAT
    GLfloat     tu;
    GLfloat     tv;

    inline TextureCoord()
    {
        tu = tv = 0;
    }
#else
    GLfixed     tu;
    GLfixed     tv;

    inline TextureCoord()
    {
        tu = tv = __GL_X_ZERO;
    }
#endif
};

struct Color
{
#ifdef __GL_FLOAT
    GLfloat     red;
    GLfloat     green;
    GLfloat     blue;
    GLfloat     alpha;

    inline Color()
    {
        red = green = blue = alpha = 1.f;
    }
#else
    GLubyte     red;
    GLubyte     green;
    GLubyte     blue;
    GLubyte     alpha;

    inline Color()
    {
        red = green = blue = alpha = __GL_X_ONE;
    }
#endif
};

struct Material
{
    Color       material_ambient;
    Color       material_diffuse;
    Color       material_specular;
    Color       material_emission;
#ifdef __GL_FLOAT
    GLfloat     material_shininess;

    inline Material()
    {
        material_ambient.red = material_ambient.green = material_ambient.blue = 0.2f;
        material_ambient.alpha = 1.0f;

        material_diffuse.red = material_diffuse.green = material_diffuse.blue = 0.8f;
        material_diffuse.alpha = 1.0f;

        material_specular.red = material_specular.green = material_specular.blue = 0.f;
        material_specular.alpha = 1.0f;

        material_emission.red = material_emission.green = material_emission.blue = 0.f;
        material_emission.alpha = 1.0f;

        material_shininess = 0.f;
    }
#else
    GLfixed     material_shininess;

    inline Material()
    {
        material_ambient.red = material_ambient.green = material_ambient.blue = __GL_FLOAT_2_X(0.2f);
        material_ambient.alpha = __GL_X_ONE;

        material_diffuse.red = material_diffuse.green = material_diffuse.blue = __GL_FLOAT_2_X(0.8f);
        material_diffuse.alpha = __GL_X_ONE;

        material_specular.red = material_specular.green = material_specular.blue = __GL_X_ZERO;
        material_specular.alpha = __GL_X_ONE;

        material_emission.red = material_emission.green = material_emission.blue = __GL_X_ZERO;
        material_emission.alpha = __GL_X_ONE;

        material_shininess = __GL_X_ZERO;
    }
#endif
};

struct Source
{
    Color       source_ambient;
    Color       source_diffuse;
    Color       source_specular;
    Vertex4     source_position;
    Vector3     source_spot_direction;
#ifdef __GL_FLOAT
    GLfloat     source_spot_exponent;
    GLfloat     source_spot_curoff;
    GLfloat     source_constant_attenuation;
    GLfloat     source_linear_attenuation;
    GLfloat     source_quadratic_attenuation;

    inline Source()
    {
        source_ambient.red = source_ambient.green = source_ambient.blue = 0.f;
        source_ambient.alpha = 1.0f;

        source_diffuse.red = source_diffuse.green = source_diffuse.blue = 0.f;
        source_diffuse.alpha = 1.0f;

        source_specular.red = source_specular.green = source_specular.blue = 0.f;
        source_specular.alpha = 1.0f;

        source_position.x = source_position.y = source_position.w = 0.f;
        source_position.z = 1.0f;

        source_spot_direction.v_x = source_spot_direction.v_y = 0.f;
        source_spot_direction.v_z = -1.0f;

        source_spot_exponent = 0.f;
        source_spot_curoff = 180.f;
        source_constant_attenuation = 1.0f;
        source_linear_attenuation = 0.f;
        source_quadratic_attenuation = 0.f;

    }
#else
    GLfixed     source_spot_exponent;
    GLfixed     source_spot_curoff;
    GLfixed     source_constant_attenuation;
    GLfixed     source_linear_attenuation;
    GLfixed     source_quadratic_attenuation;

    inline Source()
    {
        source_ambient.red = source_ambient.green = source_ambient.blue = __GL_X_ZERO;
        source_ambient.alpha = __GL_X_ONE;

        source_position.x = source_position.y = source_position.w = __GL_X_ZERO;
        source_position.z = __GL_X_ONE;

        source_spot_direction.v_x = source_spot_direction.v_y = __GL_X_ZERO;
        source_spot_direction.v_z = -__GL_X_ONE;

        source_spot_exponent = __GL_X_ZERO;
        source_spot_curoff = __GL_FLOAT_2_X(180.f);
        source_constant_attenuation = __GL_X_ONE;
        source_linear_attenuation = __GL_X_ZERO;
        source_quadratic_attenuation = __GL_X_ZERO;

    }
#endif
};

struct ViewPort
{
    GLint       Ox,Oy;
    GLint       width,height;

#ifdef __GL_FLOAT
    GLfloat     dp_range_near;
    GLfloat     dp_range_far;

    inline ViewPort()
    {
        width = (GLint)WINDOW_DEFAULT_WIDTH;
        height = (GLint)WINDOW_DEFAULT_HEIGHT;
        Ox = width/2;
        Oy = height/2;
        dp_range_near = 0.f;
        dp_range_far = 1.0f;
    }
#else
    GLfixed     dp_range_near;
    GLfixed     dp_range_far;

    inline ViewPort()
    {
        width = (GLint)WINDOW_DEFAULT_WIDTH;
        height = (GLint)WINDOW_DEFAULT_HEIGHT;
        Ox = width/2;
        Oy = height/2;
        dp_range_near = __GL_X_ZERO;
        dp_range_far = __GL_X_ONE;
    }
#endif
};

struct TextureImage
{
    GLuint      width;
    GLuint      height;
    GLint       internal_format;
    GLuint      level;
    GLuint      border;
    GLenum      format;
    GLenum      type;

    GLvoid      * data;
};

typedef TextureImage * TexImgPtr;

struct MipmapArray
{
    GLuint      width;
    GLuint      height;
    GLuint      internal_format;
    GLuint      red_size;
    GLuint      green_size;
    GLuint      blue_size;
    GLuint      alpha_size;
    GLuint      luminance_size;

    GLboolean   compressed;
    GLuint      image_size;

    GLvoid      * data;
};

//Maybe need to modify the structure
//texture unit setting or texture states different??
struct TextureState
{
    inline TextureState()
    {
        enable = GL_FALSE;
        Min_filter_mode = GL_NEAREST_MIPMAP_LINEAR;
        Mag_filter_mode = GL_LINEAR;
        wrap_s = GL_REPEAT;
        wrap_t = GL_REPEAT;
        env_mode = GL_MODULATE;
        env_color = 0;

        combine_funcRGB = GL_MODULATE;
        combine_funcALPHA = GL_MODULATE;
        combine_src0RGB = GL_TEXTURE;
        combine_src1RGB = GL_PREVIOUS;
        combine_src2RGB = GL_CONSTANT;
        combine_src0ALPHA = GL_TEXTURE;
        combine_src1ALPHA = GL_PREVIOUS;
        combine_src2ALPHA = GL_CONSTANT;
        combine_op0RGB = GL_SRC_COLOR;
        combine_op1RGB = GL_SRC_COLOR;
        combine_op2RGB = GL_SRC_ALPHA;
        combine_op0ALPHA = GL_SRC_ALPHA;
        combine_op1ALPHA = GL_SRC_ALPHA;
        combine_op2ALPHA = GL_SRC_ALPHA;

        auto_genMipmap = GL_FALSE;
        texture_bindID = 0;

#ifdef __GL_FLOAT
        rgb_scale = 1.0;
        alpha_scale = 1.0;
#else
        rgb_scale = __GL_X_ONE;
        alpha_scale = __GL_X_ONE;
#endif
    }

    ~TextureState()
    {
        //this->teximage->
    }

    GLboolean   enable;
    GLenum      Min_filter_mode;
    GLenum      Mag_filter_mode;
    GLenum      wrap_s;
    GLenum      wrap_t;

    GLenum      env_mode;
    GLuint      env_color;

    GLenum      combine_funcRGB;
    GLenum      combine_funcALPHA;
    GLenum      combine_src0RGB;
    GLenum      combine_src1RGB;
    GLenum      combine_src2RGB;
    GLenum      combine_src0ALPHA;
    GLenum      combine_src1ALPHA;
    GLenum      combine_src2ALPHA;
    GLenum      combine_op0RGB;
    GLenum      combine_op1RGB;
    GLenum      combine_op2RGB;
    GLenum      combine_op0ALPHA;
    GLenum      combine_op1ALPHA;
    GLenum      combine_op2ALPHA;

    GLboolean   auto_genMipmap;
    //MipmapArray * tex_array;
    TexImgPtr   teximage;

    GLuint      tex_array_num;
    GLuint      texture_bindID;

#ifdef __GL_FLOAT
    GLfloat     rgb_scale;
    GLfloat     alpha_scale;
#else
    GLfixed     rgb_scale;
    GLfixed     alpha_scale;
#endif

};

typedef TextureState * TexSptr;

struct ScissorTest
{
    GLboolean   enable;
    GLint       left;
    GLint       bottom;
    GLsizei     width;
    GLsizei     height;
};

struct AlphaTest
{
    GLboolean   enable;
    GLenum      func;
#ifdef __GL_FLOAT
    GLclampf    ref_val;
#else
    GLclampx    ref_val;
#endif
};

struct StencilTest
{
    GLboolean   enable;
    GLenum      func;
    GLint       ref_val;
    GLuint      mask;

    GLenum      sfail;
    GLenum      dfail;
    GLenum      dpass;
};

struct DepthTest
{
    GLboolean   enable;
    GLenum      func;
};

struct Blending
{
    GLboolean   enable;
    GLenum      src;
    GLenum      dst;
};

struct LogicOpTest
{
    GLboolean   enable;
    GLenum      op;
};

struct Fog
{
    GLboolean   enable;
    GLenum      mode;
#ifdef __GL_FLOAT
    GLfloat     start;
    GLfloat     end;
    GLfloat     density;
#else
    GLfixed     start;
    GLfixed     end;
    GLfixed     density;
#endif
    Color       cf;
};

struct DepthOffSet
{
    GLboolean   enable;
#ifdef __GL_FLOAT
    GLfloat     factor;
    GLfloat     units;
#else
    GLfixed     factor;
    GLfixed     units;
#endif
};

struct Statistics
{
    inline Statistics()
    {
        total_states = 0;
        num_of_tiles = 0;
        num_of_vertex = 0;
        num_of_triangle = 0;
        culled_triangles = 0;
        covered_tiles = 0;
        removed_kick_triangles = 0;
        triangle_size = 0;
        triangle_list_size = 0;
        num_of_tile_node = 0;
        tiled_list_size = 0;
        num_of_state_node = 0;
        state_list_size = 0;

        tex_cache_hit = 0;
        tex_cache_miss = 0;
        tex_cache_hitrate = 0.0;

        visited_pixels = 0;
        processed_pixels = 0;
        shaded_pixels = 0;

        average_scan_rate = 0;

        state_raster_bw = 0;
        triangle_raster_bw = 0;
        depth_buffer_bw[0] = depth_buffer_bw[1] = 0;
        color_buffer_bw[0] = color_buffer_bw[1] = 0;
        texture_mem_read = 0;
        total_bandwidth = 0;

        fps = 0.0;
    }
    int         total_states;

    int         num_of_tiles;
    int         num_of_vertex;
    int         num_of_triangle;
    int         culled_triangles;
    int         covered_tiles;
    int         removed_kick_triangles;
    int         triangle_size;
    int         triangle_list_size;
    int         num_of_tile_node;
    int         tiled_list_size;
    int         num_of_state_node;
    int         state_list_size;

    int         tex_cache_hit;
    int         tex_cache_miss;
    float       tex_cache_hitrate;
    int         visited_pixels;
    int         processed_pixels;
    int         shaded_pixels;

    float       average_scan_rate;

    int         state_raster_bw;
    int         triangle_raster_bw;
    int         depth_buffer_bw[2];
    int         color_buffer_bw[2];
    int         texture_mem_read;
    int         total_bandwidth;

    float       fps;
};

struct GLfuncstat
{
    inline GLfuncstat()
    {
        ActiveTexture = 0;
        AlphaFunc  = 0;
        BindTexture = 0;
        BlendFunc = 0;
        Clear = 0;
        ClearColor = 0;
        ClearDepth = 0;
        ClearStencil = 0;
        Color4 = 0;
        ColorPointer = 0;
        CullFace = 0;
        DeleteTextures = 0;
        DepthFunc = 0;
        DepthRange = 0;
        Disable = 0;
        DisableClientState = 0;
        DrawArrays = 0;
        DrawElements = 0;
        Enable = 0;
        EnableClientState = 0;
        Finish = 0;
        Flush = 0;
        Fog = 0;
        Fogv = 0;
        FrontFace = 0;
        Frustum = 0;
        GenTextures = 0;
        LightModel = 0;
        LightModelv = 0;
        Light = 0;
        Lightv = 0;
        LoadIdentity = 0;
        LoadMatrix = 0;
        LogicOp = 0;
        Material = 0;
        Materialv = 0;
        MatrixMode = 0;
        MultMatrix = 0;
        Normal3 = 0;
        NormalPointer = 0;
        Ortho = 0;
        PolygonOffset = 0;
        PopMatrix = 0;
        PushMatrix = 0;
        Rotate = 0;
        Scale = 0;
        Scissor = 0;
        ShadeModel = 0;
        StencilFunc = 0;
        StencilOp = 0;
        TexCoordPointer = 0;
        TexEnv = 0;
        TexEnvv = 0;
        TexImage2D = 0;
        TexParameter = 0;
        Translate = 0;
        VertexPointer = 0;
        Viewport = 0;
        Total  = 0;

    }
    int         ActiveTexture;
    int         AlphaFunc;
    int         BindTexture;
    int         BlendFunc;
    int         Clear;
    int         ClearColor;
    int         ClearDepth;
    int         ClearStencil;
    int         Color4;
    int         ColorPointer;
    int         CullFace;
    int         DeleteTextures;
    int         DepthFunc;
    int         DepthRange;
    int         Disable;
    int         DisableClientState;
    int         DrawArrays;
    int         DrawElements;
    int         Enable;
    int         EnableClientState;
    int         Finish;
    int         Flush;
    int         Fog;
    int         Fogv;
    int         FrontFace;
    int         Frustum;
    int         GenTextures;
    int         LightModel;
    int         LightModelv;
    int         Light;
    int         Lightv;
    int         LoadIdentity;
    int         LoadMatrix;
    int         LogicOp;
    int         Material;
    int         Materialv;
    int         MatrixMode;
    int         MultMatrix;
    int         Normal3;
    int         NormalPointer;
    int         Ortho;
    int         PolygonOffset;
    int         PopMatrix;
    int         PushMatrix;
    int         Rotate;
    int         Scale;
    int         Scissor;
    int         ShadeModel;
    int         StencilFunc;
    int         StencilOp;
    int         TexCoordPointer;
    int         TexEnv;
    int         TexEnvv;
    int         TexImage2D;
    int         TexParameter;
    int         Translate;
    int         VertexPointer;
    int         Viewport;
    int         Total;

};

// --------------------------------------------------------------------------
// Data structure of Tile-based State Management
// --------------------------------------------------------------------------
struct Node
{
    Node(void) : kind(0), addr(0), next(0) {};
    U8          kind;
    U32         addr;
    struct Node * next;
};

struct RenderState
{
    RenderState(void) : ID(0), Value(0), next(0) {};
    U16         ID;
    U32         Value;
    struct RenderState * next;
};

/* The following definitions are for ID of RenderState structure */
#define GL_TEXTURE_HEIGHT   0X9000
#define GL_TEXTURE_WIDTH    0X9001
#define GL_TEXTURE_FILTER   0x9002
#define GL_TEXTURE_BASE     0x9003
#define GL_TEXTURE_FORMAT   0X9004

extern RenderState * stateLinkList;
extern Node * displayList[TILE_COUNT];
extern bool dirtyTileMap[TILE_COUNT];

// --------------------------------------------------------------------------
// File Handling for Debugging Info
// --------------------------------------------------------------------------
extern FILE * APIdumper;
extern FILE * Ctxdumper;
extern FILE * Drvdumper;
extern FILE * GMdumper;
extern FILE * RMdumper;
extern FILE * UGdumper;
extern FILE * Cbufdumper;
extern FILE * CSVhanlder;
extern FILE * CSVglfunc;

extern int frame_count;
extern GLfuncstat glfunc;

extern void CreateDebugFile();
extern void CloseFile();

#endif // OGLES_H_INCLUDED
