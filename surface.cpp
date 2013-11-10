// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : surface.cpp
// Purpose : EGL Drawing Surface Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================


#include "surface.h"

using namespace EGL;


namespace
{
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
    struct InfoHeader
    {
        BITMAPINFOHEADER bmiHeader;
        DWORD            bmiColors[3];

        InfoHeader(U32 width, U32 height)
        {
            bmiHeader.biSize = sizeof(bmiHeader);
            bmiHeader.biWidth = width;
            bmiHeader.biHeight = height;
            bmiHeader.biPlanes = 1;
            bmiHeader.biBitCount = 16;
            bmiHeader.biCompression = BI_BITFIELDS;
            bmiHeader.biSizeImage = width * height * sizeof(U16);
            bmiHeader.biXPelsPerMeter = 72 * 25;
            bmiHeader.biYPelsPerMeter = 72 * 25;
            bmiHeader.biClrUsed = 0;
            bmiHeader.biClrImportant = 0;

            bmiColors[0] = 0xF800;
            bmiColors[1] = 0x07E0;
            bmiColors[2] = 0x001F;
        }
    };
#elif defined(EGL_ON_LINUX)

#else
#error "Unsupported Operating System"
#endif	

}

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
Surface::Surface(const Config & config, HDC hdc)
        :	m_Config(config),
        m_Rect (0, 0, 640, 480), //config.GetConfigAttrib(EGL_WIDTH), config.GetConfigAttrib(EGL_HEIGHT)),
        m_Bitmap(reinterpret_cast<HBITMAP>(INVALID_HANDLE_VALUE)),
        m_HDC(reinterpret_cast<HDC>(INVALID_HANDLE_VALUE))
{
    //m_FrameBuffer = new U16[m_Width * m_Height];
    U32 width = GetWidth();
    U32 height = GetHeight();
    U8 CSIZE = 3;

//    m_AlphaBuffer = new U8[width * height];
//    m_DepthBuffer = new U16[width * height];
//    m_StencilBuffer = new U32[width * height];
    m_ColorBuffer = new U32[width*height];

    if (hdc != INVALID_HANDLE_VALUE)
    {
        m_HDC = CreateCompatibleDC(hdc);
    }

    InfoHeader info(width, height);

    m_Bitmap = CreateDIBSection(m_HDC, reinterpret_cast<BITMAPINFO *>(&info), DIB_RGB_COLORS,
                                reinterpret_cast<void **>(&m_FrameBuffer), NULL, 0);

}
#elif defined(EGL_ON_LINUX)
Surface::Surface(const Config & config, void * hdc)
				:m_Config(config)
{
	
}
#else
#error "Unsupported Operating System"
#endif	


Surface::~Surface()
{
/*
    if (m_Bitmap != INVALID_HANDLE_VALUE)
    {
        DeleteObject(m_Bitmap);
        m_Bitmap = reinterpret_cast<HBITMAP>(INVALID_HANDLE_VALUE);
    }

    if (m_HDC != INVALID_HANDLE_VALUE)
    {
        DeleteDC(m_HDC);
        m_HDC = reinterpret_cast<HDC>(INVALID_HANDLE_VALUE);
    }
*/
    /*if (m_FrameBuffer != 0) {
    	delete [] m_FrameBuffer;
    	m_FrameBuffer = 0;
    }*/

    if (m_ColorBuffer != 0)
    {
        delete [] m_ColorBuffer;
        m_ColorBuffer = 0;
    }

    if (m_AlphaBuffer != 0)
    {
        delete [] m_AlphaBuffer;
        m_AlphaBuffer = 0;
    }

    if (m_DepthBuffer != 0)
    {
        delete[] m_DepthBuffer;
        m_DepthBuffer = 0;
    }

    if (m_StencilBuffer != 0)
    {
        delete[] m_StencilBuffer;
        m_StencilBuffer = 0;
    }
}

void Surface::Dispose()
{
    if (GetCurrentContext() != 0)
    {
        m_Disposed = true;
    }
    else
    {
        delete this;
    }
}


void Surface::SetCurrentContext(Context * context)
{
    m_CurrentContext = context;

    if (context == 0 && m_Disposed)
    {
        delete this;
    }
}

namespace
{
    template <class T> U16 ConvertTo565(T color)
    {
        U8 r,g,b;
        r = (color & 0xFF000000) >> 24;
        g = (color & 0x00FF0000) >> 16;
        b = (color & 0x0000FF00) >>  8;
        return (b & 0xF8) >> 3 | (g & 0xFC) << 3 | (r & 0xF8) << 8;
    }

    template <class T> void FillRect(T * base, const Rect & bufferRect, const Rect & fillRect,
                                     const T& value, const T& mask)
    {
        Rect rect = Rect::Intersect(fillRect, bufferRect);

        base += fillRect.x + fillRect.y * bufferRect.width;
        size_t gap = bufferRect.width - fillRect.width;

        size_t rows = fillRect.height;
        T inverseMask = ~mask;
        T maskedValue = value & mask;

        while (rows--)
        {
            for (size_t columns = fillRect.width; columns > 0; columns--)
            {
                *base = (*base & inverseMask) | maskedValue;
                ++base;
            }

            base += gap;
        }
    }

    template <class T> void FillRect(T * base, const Rect & bufferRect, const Rect & fillRect,
                                     const T& value)
    {
        Rect rect = Rect::Intersect(fillRect, bufferRect);

        base += fillRect.x + fillRect.y * bufferRect.width;
        size_t gap = bufferRect.width - fillRect.width;

        size_t rows = fillRect.height;

        while (rows--)
        {
            for (size_t columns = fillRect.width; columns > 0; columns--)
            {
                *base = value;
                ++base;
            }

            base += gap;
        }
    }
}

void Surface::updateFrameBuffer()
{
    U16 * base = m_FrameBuffer;
    U32 * cbuf = m_ColorBuffer;
    size_t rows = 480;

    while (rows--)
    {
        for (size_t columns = 640; columns > 0; columns--)
        {
            *base = ConvertTo565(*cbuf);
            ++base;
            ++cbuf;
        }

        //base += gap;
    }
}

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
bool Surface::Save(const TCHAR * filename)
{

    InfoHeader info(GetWidth(), GetHeight());

    BITMAPFILEHEADER header;
    header.bfType      = 0x4d42;
    header.bfSize      = sizeof(BITMAPFILEHEADER) + sizeof(info) + info.bmiHeader.biSizeImage;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(info);

    // Create the file
    HANDLE hFile = ::CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD temp;

    // Write the header + bitmap info
    ::WriteFile(hFile, &header, sizeof(header), &temp, 0);
    ::WriteFile(hFile, &info, sizeof(info), &temp, 0);

    // Write the image
    U16 * pixels = m_FrameBuffer;

    for (int h = GetHeight(); h; --h)
    {
        ::WriteFile(hFile, pixels, GetWidth() * sizeof(U16), &temp, 0 );
        pixels += GetWidth();
    }

    ::CloseHandle(hFile);

    return true;
}
#elif defined(EGL_ON_LINUX)
bool Surface::Save(const char * filename)
{
		
}
#else
#error "Unsupported Operating System"
#endif	
