#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : surface.h
// Purpose : EGL Drawing Surface Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "ogles.h"
#include "config.h"

namespace EGL
{

    template<class T> T Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<class T> T Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    struct Rect
    {

        Rect(GLint _x = 0, GLint _y = 0, GLsizei _width = 0, GLsizei _height = 0):
                x(_x), y(_y), width(_width), height(_height)
        {}

        static Rect Intersect(const Rect& first, const Rect& second)
        {

            GLint x = Max(first.x, second.x);
            GLint y = Max(first.y, second.y);

            GLint xtop = Min(first.x + first.width, second.x + second.width);
            GLint ytop = Min(first.y + first.height, second.y + second.height);

            return Rect(x, y, xtop - x, ytop - y);
        }

        bool Contains(const Rect& other) const
        {
            return
                x <= other.x &&
                y <= other.y &&
                x + width >= other.x + other.width &&
                y + height >= other.y + other.height;
        }

        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
    };

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
    class Surface
    {

    public:
        friend class Context;

        // Create a PBuffer surface
        Surface(const Config & config, HDC hdc = 0);
        ~Surface();

        U16 GetWidth();
        U16 GetHeight();
        U32 GetPixels();
        const Rect& GetRect() const;

        Config * GetConfig();

        void SetCurrentContext(Context * context);
        Context * GetCurrentContext();

        U32 * GetColorBuffer();
//        U8 *  GetAlphaBuffer();
//        U16 * GetDepthBuffer();
//        U32 * GetStencilBuffer();

        void updateFrameBuffer();
        bool Save(const TCHAR * filename);
        void Dispose();

        // Windows integration
        HDC GetMemoryDC();
        HBITMAP GetBitmap();

    private:
        HDC		    m_HDC;				// windows device context handle
        HBITMAP	  m_Bitmap;			// windows bitmap handle
        Config	  m_Config;			// configuration arguments
        U16 *	    m_FrameBuffer;		// pointer to frame buffer base address 5-6-5
        U32 *     m_ColorBuffer;    // pointer to color buffer RGB 24bit
        U8 *	    m_AlphaBuffer;		// pointer to alpha buffer
        U16 *	    m_DepthBuffer;		// pointer to Z-buffer base address
        U32 *	    m_StencilBuffer;	// stencil buffer

        Rect	    m_Rect;

        Context *	m_CurrentContext;
        bool	    m_Disposed;			// the surface
    };
#elif defined(EGL_ON_LINUX)
    class Surface
    {

    public:
        friend class Context;

        // Create a PBuffer surface
        Surface(const Config & config, void * hdc = 0);
        ~Surface();

        U16 GetWidth();
        U16 GetHeight();
        U32 GetPixels();
        const Rect& GetRect() const;

        Config * GetConfig();

        void SetCurrentContext(Context * context);
        Context * GetCurrentContext();

        U32 * GetColorBuffer();
//        U8 *  GetAlphaBuffer();
//        U16 * GetDepthBuffer();
//        U32 * GetStencilBuffer();

        void updateFrameBuffer();
        bool Save(const char * filename);
        void Dispose();

        // Windows integration
        void * GetMemoryDC();
        void * GetBitmap();

    private:
        void *    m_HDC;				// windows device context handle
        void *	  m_Bitmap;			// windows bitmap handle
        Config	  m_Config;			// configuration arguments
        U16 *	    m_FrameBuffer;		// pointer to frame buffer base address 5-6-5
        U32 *     m_ColorBuffer;    // pointer to color buffer RGB 24bit
        U8 *	    m_AlphaBuffer;		// pointer to alpha buffer
        U16 *	    m_DepthBuffer;		// pointer to Z-buffer base address
        U32 *	    m_StencilBuffer;	// stencil buffer

        Rect	    m_Rect;

        Context *	m_CurrentContext;
        bool	    m_Disposed;			// the surface
    };
#else
#error "Unsupported Operating System"
#endif

    // --------------------------------------------------------------------------
    // Inline accessors
    // --------------------------------------------------------------------------


    inline Config * Surface :: GetConfig()
    {
        return &m_Config;
    }

    inline U32 Surface :: GetPixels()
    {
        return GetWidth() * GetHeight();
    }

    inline Context * Surface :: GetCurrentContext()
    {
        return m_CurrentContext;
    }

    inline void * Surface :: GetMemoryDC()
    {
        return m_HDC;
    }

    inline void * Surface :: GetBitmap()
    {
        return m_Bitmap;
    }

    inline U32 * Surface :: GetColorBuffer()
    {
        return m_ColorBuffer;
    }
//
//    inline U8 * Surface :: GetAlphaBuffer()
//    {
//        return m_AlphaBuffer;
//    }
//
//    inline U16 * Surface :: GetDepthBuffer()
//    {
//        return m_DepthBuffer;
//    }
//
//    inline U32 * Surface :: GetStencilBuffer()
//    {
//        return m_StencilBuffer;
//    }

    inline U16 Surface :: GetWidth()
    {
        return m_Rect.width;
    }

    inline U16 Surface :: GetHeight()
    {
        return m_Rect.height;
    }

    inline const Rect& Surface :: GetRect() const
    {
        return m_Rect;
    }

//    inline void Surface :: ClearDepthBuffer(U16 depth, bool mask)
//    {
//        ClearDepthBuffer(depth, mask, GetRect());
//    }
//
//    inline void Surface :: ClearColorBuffer(const Color & rgba, const Color & mask)
//    {
//        ClearColorBuffer(rgba, mask, GetRect());
//    }
//
//    inline void Surface :: ClearStencilBuffer(U32 value, U32 mask)
//    {
//        ClearStencilBuffer(value, mask, GetRect());
//    }
}

#endif // SURFACE_H_INCLUDED
