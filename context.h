#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : context.h
// Purpose : EGL Rendering Context Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "ogles.h"
#include "matrix_stack.h"
#include "surface.h"
#include "config.h"

using namespace std;

namespace EGL
{

    class Surface;

    //#define EGL_NUMBER_LIGHTS 8

    class Context
    {
        // ----------------------------------------------------------------------
        // Default viewport configuration
        // ----------------------------------------------------------------------

        /*enum
        {
            NUM_CLIP_PLANES = 6,
            MATRIX_PALETTE_SIZE = 9,

            VIEWPORT_NEAR = 0,
            VIEWPORT_FAR = EGL_ONE
        };

        enum CullMode
        {
            CullModeBack,
            CullModeFront,
            CullModeBackAndFront
        };

        enum FogMode
        {
            FogLinear,
            FogModeExp,
            FogModeExp2
        };*/

    public:
        // ----------------------------------------------------------------------
        // Constructor and Destructor
        // ----------------------------------------------------------------------

        //Context();
        Context(const Config& );
        ~Context();

        // ----------------------------------------------------------------------
        // Fixed point API
        // ----------------------------------------------------------------------

        void ActiveTexture(GLenum texture);
        void AlphaFuncx(GLenum func, GLclampx ref);
        void BindBuffer(GLenum target, GLuint buffer);
        void BindTexture(GLenum target, GLuint texture);
        void BlendFunc(GLenum sfactor, GLenum dfactor);
        void BufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
        void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
        void Clear(GLbitfield mask);
        void ClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
        void ClearDepthx(GLclampx depth);
        void ClearStencil(GLint s);
        void ClientActiveTexture(GLenum texture);
        void ClipPlanex(GLenum plane, const GLfixed *equation);
        void Color4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
        void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
        void ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
        void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
        void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
        void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
        void CullFace(GLenum mode);
        void DeleteBuffers(GLsizei n, const GLuint *buffers);
        void DeleteTextures(GLsizei n, const GLuint *textures);
        void DepthFunc(GLenum func);
        void DepthMask(GLboolean flag);
        void DepthRangex(GLclampx zNear, GLclampx zFar);
        void Disable(GLenum cap);
        void DisableClientState(GLenum array);
        void DrawArrays(GLenum mode, GLint first, GLsizei count);
        void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
        void Enable(GLenum cap);
        void EnableClientState(GLenum array);
        void Finish(void);
        void Flush(void);
        void Fogx(GLenum pname, GLfixed param);
        void Fogxv(GLenum pname, const GLfixed *params);
        void FrontFace(GLenum mode);
        void Frustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
        void GetBooleanv(GLenum pname, GLboolean *params);
        void GetBufferParameteriv(GLenum target, GLenum pname, GLint *params);
        void GetClipPlanex(GLenum pname, GLfixed eqn[4]);
        void GenBuffers(GLsizei n, GLuint *buffers);
        void GenTextures(GLsizei n, GLuint *textures);
        GLenum GetError(void);
        bool GetFixedv(GLenum pname, GLfixed *params);
        void GetIntegerv(GLenum pname, GLint *params);
        bool GetLightxv(GLenum light, GLenum pname, GLfixed *params);
        bool GetMaterialxv(GLenum face, GLenum pname, GLfixed *params);
        void GetPointerv(GLenum pname, void **params);
        void GetTexEnviv(GLenum env, GLenum pname, GLint *params);
        bool GetTexEnvxv(GLenum env, GLenum pname, GLfixed *params);
        void GetTexParameteriv(GLenum target, GLenum pname, GLint *params);
        void GetTexParameterxv(GLenum target, GLenum pname, GLfixed *params);
        const GLubyte * GetString(GLenum name);
        void Hint(GLenum target, GLenum mode);
        GLboolean IsBuffer(GLuint buffer);
        GLboolean IsEnabled(GLenum cap);
        GLboolean IsTexture(GLuint texture);
        void LightModelx(GLenum pname, GLfixed param);
        void LightModelxv(GLenum pname, const GLfixed *params);
        void Lightx(GLenum light, GLenum pname, GLfixed param);
        void Lightxv(GLenum light, GLenum pname, const GLfixed *params);
        void LineWidthx(GLfixed width);
        void LoadIdentity(void);
        void LoadMatrixx(const GLfixed *m);
        void LogicOp(GLenum opcode);
        void Materialx(GLenum face, GLenum pname, GLfixed param);
        void Materialxv(GLenum face, GLenum pname, const GLfixed *params);
        void MatrixMode(GLenum mode);
        void MultMatrixx(const GLfixed *m);
        void MultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
        void Normal3x(GLfixed nx, GLfixed ny, GLfixed nz);
        void NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
        void Orthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
        void PixelStorei(GLenum pname, GLint param);
        void PointParameterx(GLenum pname, GLfixed param);
        void PointParameterxv(GLenum pname, const GLfixed *params);
        void PointSizex(GLfixed size);
        void PolygonOffsetx(GLfixed factor, GLfixed units);
        void PopMatrix(void);
        void PushMatrix(void);
        void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
        void Rotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
        void SampleCoveragex(GLclampx value, GLboolean invert);
        void Scalex(GLfixed x, GLfixed y, GLfixed z);
        void Scissor(GLint x, GLint y, GLsizei width, GLsizei height);
        void ShadeModel(GLenum mode);
        void StencilFunc(GLenum func, GLint ref, GLuint mask);
        void StencilMask(GLuint mask);
        void StencilOp(GLenum fail, GLenum zfail, GLenum zpass);
        void TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void TexEnvx(GLenum target, GLenum pname, GLfixed param);
        void TexEnvxv(GLenum target, GLenum pname, const GLfixed *params);
        void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
        void TexParameterx(GLenum target, GLenum pname, GLfixed param);
        void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
        void Translatex(GLfixed x, GLfixed y, GLfixed z);
        void VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void Viewport(GLint x, GLint y, GLsizei width, GLsizei height);

        // ----------------------------------------------------------------------
        // Floating Point API
        // ----------------------------------------------------------------------

        void AlphaFunc(GLenum func, GLclampf ref);
        void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        void ClearDepthf(GLclampf depth);
        void ClipPlanef(GLenum plane, const GLfloat *equation);
        void Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        void DepthRangef(GLclampf zNear, GLclampf zFar);
        //void DrawTexf(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
        //void DrawTexfv(GLfloat *coords);
        void Fogf(GLenum pname, GLfloat param);
        void Fogfv(GLenum pname, const GLfloat *params);
        void Frustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
        void GetClipPlanef(GLenum pname, GLfloat eqn[4]);
        void GetFloatv(GLenum pname, GLfloat *params);
        void GetLightfv(GLenum light, GLenum pname, GLfloat *params);
        void GetMaterialfv(GLenum face, GLenum pname, GLfloat *params);
        void GetTexEnvfv(GLenum env, GLenum pname, GLfloat *params);
        void GetTexParameterfv(GLenum target, GLenum pname, GLfloat *params);
        void LightModelf(GLenum pname, GLfloat param);
        void LightModelfv(GLenum pname, const GLfloat *params);
        void Lightf(GLenum light, GLenum pname, GLfloat param);
        void Lightfv(GLenum light, GLenum pname, const GLfloat *params);
        void LineWidth(GLfloat width);
        void LoadMatrixf(const GLfloat *m);
        void Materialf(GLenum face, GLenum pname, GLfloat param);
        void Materialfv(GLenum face, GLenum pname, const GLfloat *params);
        void MultMatrixf(const GLfloat *m);
        //GLbitfield QueryMatrixx(GLfixed mantissa[16], GLint exponent[16]);
        void MultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
        void Normal3f(GLfloat nx, GLfloat ny, GLfloat nz);
        void Orthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
        void PointParameterf(GLenum pname, GLfloat param);
        void PointParameterfv(GLenum pname, const GLfloat *params);
        void PointSize(GLfloat size);
        void PolygonOffset(GLfloat factor, GLfloat units);
        void Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
        void SampleCoverage(GLclampf value, GLboolean invert);
        void Scalef(GLfloat x, GLfloat y, GLfloat z);
        void TexEnvf(GLenum target, GLenum pname, GLfloat param);
        void TexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
        void TexParameteri(GLenum target, GLenum pname, GLint param);
        void TexParameterf(GLenum target, GLenum pname, GLfloat param);
        void Translatef(GLfloat x, GLfloat y, GLfloat z);

        // ----------------------------------------------------------------------
        // Extensions
        // ----------------------------------------------------------------------

        /* OES_point_size_array */
        //void PointSizePointer(GLenum type, GLsizei stride, const GLvoid *pointer);

        // ----------------------------------------------------------------------
        // Context Management Functions
        // ----------------------------------------------------------------------

        void Dispose();
        void SetCurrent(bool current);

        static void SetCurrentContext(Context * context);
        static Context * GetCurrentContext();

        void SetReadSurface(Surface * surface);
        void SetDrawSurface(Surface * surface);

//        Surface * GetReadSurface() const;
//        Surface * GetDrawSurface() const;

        // ----------------------------------------------------------------------
        // Error Handling
        // ----------------------------------------------------------------------

        void RecordError(GLenum error);

        // ----------------------------------------------------------------------
        // Private Functions
        // ----------------------------------------------------------------------

        inline MatrixStack * CurrentMatrixStack()
        {
            return m_current_stack;
        }

        void RebuildMatrices(void);
        void UpdateInverseModelViewMatrix(void);
        void InverseUpper3x3(Matrix3x3&, Matrix3x3&, bool);

        void MatrixTransformVec4(Matrix4x4&, Vertex4&);
        void MatrixTransformVec3(Matrix4x4&, Vector3&);

        void Toggle(GLenum, bool);

        inline size_t getActiveTexture()
        {
            return active_texture;
        }
        inline size_t getActiveLight()
        {
            return active_light;
        }

        // ----------------------------------------------------------------------
        // Members of Context - Register, Constant, Variable
        // ----------------------------------------------------------------------
        MatrixStack     m_modelview_stack;
        MatrixStack     m_projection_stack;
        MatrixStack     m_texture_stack[EGL_NUM_TEXTURE_UNITS];
        MatrixStack     * m_current_stack;
        Matrix3x3       m_inversed_modelview;
        GLenum	        m_matrix_mode;

        Material        m_light;

        Source          s_light[EGL_NUM_LIGHTING];

        Color           light_model_ambient;
        GLboolean       light_model_two_side;

        ViewPort        vp;

        Color           current_color;
        Normal          current_normal;
        TextureCoord    current_texture_coords;

        GLenum          shade_model;
        GLubyte         lihgt_unit_mask;
        GLubyte         clip_plane_mask;
        GLboolean       lighting_enable;
        GLboolean       culling_enable;
        GLboolean       normalize_enable;
        GLboolean       rescale_normal_enable;
        GLboolean       color_material_enable;

        GLenum          front_face;
        GLenum          cull_face;

        ArrayPointer    vertex_array;
        ArrayPointer    color_array;
        ArrayPointer    normal_array;
        ArrayPointer    texture_coord_array;

        DrawArrayCmd    draw_array;
        DrawElemCmd     draw_element;
        GLenum          draw_mode;

        Color           default_color;
        Color           clear_color;
        GLfloat         clear_depth;
        GLint           clear_stencil;

        GLbitfield      clear_buffer_mask;

        Fog             fog;

        DepthOffSet     depth_offset;

        TextureState    texture_state[EGL_NUM_TEXTURE_UNITS];
        GLuint          texture_total_seq;
        TextureImage    * current_teximage;

        AlphaTest       alpha_test;
        Blending        blend_func;
        DepthTest       depth_test;
        StencilTest     stencil_test;
        ScissorTest     scissor_test;
        LogicOpTest     logic_op;
        GLboolean       aa_enable;

        Statistics      measure_stat;

        vector<TextureState> texvec;

        size_t  pre_state_counter;
        size_t  state_counter;

        // ----------------------------------------------------------------------
        // Members of EGL Display, Surface, Config
        // ----------------------------------------------------------------------
        Surface         * m_draw_surface;		// current surface for drawing
        Surface 		* m_read_surface;		// current surface for reading

        Config			m_config;			// copy of configuration args

    private:
        size_t	active_texture;
        size_t  active_light;
        size_t	client_active_texture;

        bool    m_current;
        bool    m_disposed;

    };

//    inline Surface * Context::GetDrawSurface() const
//    {
//        return m_draw_surface;
//    }
//
//
//    inline Surface * Context::GetReadSurface() const
//    {
//        return m_read_surface;
//    }

}


#endif // CONTEXT_H_INCLUDED
