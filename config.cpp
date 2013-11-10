// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : config.cpp
// Purpose : EGL Configuration Management Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================


#include "config.h"

using namespace EGL;

Config::Config (const Config &other, const EGLint * attribList, const EGLint * validAttributes)
{

}

EGLint Config::GetConfigAttrib(EGLint attribute) const
{
    switch (attribute)
    {
    case EGL_BUFFER_SIZE:
        return m_BufferSize;

    case EGL_ALPHA_SIZE:
        return m_AlphaSize;

    case EGL_BLUE_SIZE:
        return m_BlueSize;

    case EGL_GREEN_SIZE:
        return m_GreenSize;

    case EGL_RED_SIZE:
        return m_RedSize;

    case EGL_DEPTH_SIZE:
        return m_DepthSize;

    case EGL_STENCIL_SIZE:
        return m_StencilSize;

    case EGL_CONFIG_CAVEAT:
        return m_ConfigCaveat;

    case EGL_CONFIG_ID:
        return m_ConfigID;

    case EGL_LEVEL:
        return m_Level;

    case EGL_MAX_PBUFFER_HEIGHT:
        return m_MaxPBufferHeight;

    case EGL_MAX_PBUFFER_PIXELS:
        return m_MaxPBufferPixels;

    case EGL_MAX_PBUFFER_WIDTH:
        return m_MaxPBufferWidth;

    case EGL_NATIVE_RENDERABLE:
        return m_NativeRenderable;

    case EGL_NATIVE_VISUAL_ID:
        return m_NativeVisualID;

    case EGL_NATIVE_VISUAL_TYPE:
        return m_NativeVisualType;

    case EGL_SAMPLES:
        return m_Samples;

    case EGL_SAMPLE_BUFFERS:
        return m_SampleBuffers;

    case EGL_SURFACE_TYPE:
        return m_SurfaceType;

    case EGL_TRANSPARENT_TYPE:
        return m_TransparentType;

    case EGL_TRANSPARENT_BLUE_VALUE:
        return m_TransparentBlueValue;

    case EGL_TRANSPARENT_GREEN_VALUE:
        return m_TransparentGreenValue;

    case EGL_TRANSPARENT_RED_VALUE:
        return m_TransparentRedValue;

    case EGL_WIDTH:
        return m_Width;

    case EGL_HEIGHT:
        return m_Height;

    default:
        return EGL_NONE;
    }
}

void Config::SetConfigAttrib(EGLint attribute, EGLint value)
{
    switch (attribute)
    {
    case EGL_BUFFER_SIZE:
        m_BufferSize = value;
        break;

    case EGL_ALPHA_SIZE:
        m_AlphaSize = value;
        break;

    case EGL_BLUE_SIZE:
        m_BlueSize = value;
        break;

    case EGL_GREEN_SIZE:
        m_GreenSize = value;
        break;

    case EGL_RED_SIZE:
        m_RedSize = value;
        break;

    case EGL_DEPTH_SIZE:
        m_DepthSize = value;
        break;

    case EGL_STENCIL_SIZE:
        m_StencilSize = value;
        break;

    case EGL_CONFIG_CAVEAT:
        m_ConfigCaveat = value;
        break;

    case EGL_CONFIG_ID:
        m_ConfigID = value;
        break;

    case EGL_LEVEL:
        m_Level = value;
        break;

    case EGL_MAX_PBUFFER_HEIGHT:
        m_MaxPBufferHeight = value;
        break;

    case EGL_MAX_PBUFFER_PIXELS:
        m_MaxPBufferPixels = value;
        break;

    case EGL_MAX_PBUFFER_WIDTH:
        m_MaxPBufferWidth = value;
        break;

    case EGL_NATIVE_RENDERABLE:
        m_NativeRenderable = value;
        break;

    case EGL_NATIVE_VISUAL_ID:
        m_NativeVisualID = value;
        break;

    case EGL_NATIVE_VISUAL_TYPE:
        m_NativeVisualType = value;
        break;

    case EGL_SAMPLES:
        m_Samples = value;
        break;

    case EGL_SAMPLE_BUFFERS:
        m_SampleBuffers = value;
        break;

    case EGL_SURFACE_TYPE:
        m_SurfaceType = value;
        break;

    case EGL_TRANSPARENT_TYPE:
        m_TransparentType = value;
        break;

    case EGL_TRANSPARENT_BLUE_VALUE:
        m_TransparentBlueValue = value;
        break;

    case EGL_TRANSPARENT_GREEN_VALUE:
        m_TransparentGreenValue = value;
        break;

    case EGL_TRANSPARENT_RED_VALUE:
        m_TransparentRedValue = value;
        break;

    case EGL_WIDTH:
        m_Width = value;
        break;

    case EGL_HEIGHT:
        m_Height = value;
        break;

    }
}
