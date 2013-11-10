#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : config.h
// Purpose : EGL Configuration Management Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "ogles.h"


namespace EGL
{

    class Context;

    class Config
    {

    public:
        friend class Context;
        Config (const Config &other, const EGLint * attribList = 0, const EGLint * validAttributes = 0);
        EGLint GetConfigAttrib(EGLint attribute) const;
        void SetConfigAttrib(EGLint attribute, EGLint value);

    private:
        EGLint	m_BufferSize;
        EGLint	m_RedSize;
        EGLint	m_GreenSize;
        EGLint	m_BlueSize;
        EGLint	m_AlphaSize;
        EGLint	m_ConfigCaveat;
        EGLint	m_ConfigID;
        EGLint	m_DepthSize;
        EGLint	m_Level;
        EGLint	m_MaxPBufferWidth;
        EGLint	m_MaxPBufferHeight;
        EGLint	m_MaxPBufferPixels;
        EGLint	m_NativeRenderable;
        EGLint	m_NativeVisualID;
        EGLint	m_NativeVisualType;
        EGLint	m_SampleBuffers;
        EGLint	m_Samples;
        EGLint	m_StencilSize;
        EGLint	m_SurfaceType;
        EGLint	m_TransparentType;
        EGLint	m_TransparentRedValue;
        EGLint	m_TransparentGreenValue;
        EGLint	m_TransparentBlueValue;
        EGLint  m_Width;
        EGLint	m_Height;
    };
}

#endif // CONFIG_H_INCLUDED
