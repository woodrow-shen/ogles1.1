// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : context.cpp
// Purpose : EGL Rendering Context Class
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "context.h"
//#include "rasterizer.h"
#include "drv_impl.h"

using namespace EGL;

static Context *currentContext = NULL;


// --------------------------------------------------------------------------
// Constructor and Destructor
// --------------------------------------------------------------------------
Context::Context(const Config &config)
        :
        m_modelview_stack(16),
        m_current_stack(&m_modelview_stack),
        m_matrix_mode(GL_MODELVIEW),

        // EGL Object
        m_draw_surface(0),
        m_read_surface(0),
        m_config(config)
{
    m_current = false;
    m_disposed = false;

    active_light = 0;
    active_texture = 0;
    client_active_texture = 0;
    pre_state_counter = 0;
    state_counter = 0;
    current_teximage = NULL;

    // GL initial state - GM
    s_light[0].source_diffuse.red = s_light[0].source_diffuse.green = s_light[0].source_diffuse.blue = s_light[0].source_diffuse.alpha = 1.0f;
    s_light[0].source_specular.red = s_light[0].source_specular.green = s_light[0].source_specular.blue = s_light[0].source_specular.alpha = 1.0f;
    light_model_ambient.red = light_model_ambient.green = light_model_ambient.blue = 0.2f;
    light_model_ambient.alpha = 1.0f;

    light_model_two_side = GL_FALSE;
    shade_model = GL_SMOOTH;
    lihgt_unit_mask = 0;
    clip_plane_mask = 0;
    lighting_enable = GL_FALSE;
    culling_enable = GL_FALSE;
    normalize_enable = GL_FALSE;
    rescale_normal_enable = GL_FALSE;
    color_material_enable = GL_FALSE;

    front_face = GL_CCW;
    cull_face = GL_BACK;

    // GL initial state - RM
    scissor_test.enable = GL_FALSE;
    scissor_test.left = 0;
    scissor_test.bottom = 0;
    scissor_test.width = WINDOW_DEFAULT_WIDTH;
    scissor_test.height = WINDOW_DEFAULT_HEIGHT;

    alpha_test.enable = GL_FALSE;
    alpha_test.func = GL_ALWAYS;
    alpha_test.ref_val = 0;

    stencil_test.enable = GL_FALSE;
    stencil_test.func = GL_ALWAYS;
    stencil_test.ref_val = 0;
    stencil_test.mask = 1;
    stencil_test.sfail = GL_KEEP;
    stencil_test.dfail = GL_KEEP;
    stencil_test.dpass = GL_KEEP;

    depth_test.enable = GL_FALSE;
    depth_test.func = GL_LESS;

    blend_func.enable = GL_FALSE;
    blend_func.src = GL_ONE;
    blend_func.dst = GL_ZERO;

    logic_op.enable = GL_FALSE;
    logic_op.op = GL_COPY;

    fog.enable = GL_FALSE;
    fog.mode = GL_EXP;
    fog.density = 1.0;
    fog.end = 1.0;
    fog.start = 0.0;
    fog.cf.red = 0.f;
    fog.cf.green = 0.f;
    fog.cf.blue = 0.f;
    fog.cf.alpha = 0.f;

    aa_enable = GL_FALSE;

    cout << "texvec: size: " << texvec.size()
         << " capacity: " << texvec.capacity() << endl;

    if (texvec.empty() != true)
    {
        cout << "texvec: size: " << texvec.size()
             << " capacity: " << texvec.capacity() << endl;
    }
    texture_total_seq = 0;
}

Context::~Context()
{
    if (m_draw_surface != 0)
    {
        m_draw_surface->SetCurrentContext(0);
    }

    if (m_read_surface != 0 && m_read_surface != m_draw_surface)
    {
        m_read_surface->SetCurrentContext(0);
    }

    m_draw_surface = m_read_surface = 0;

}

// --------------------------------------------------------------------------
// Context Management
// --------------------------------------------------------------------------
void Context::Dispose()
{
    if (m_current)
    {
        m_disposed = true;
    }
    else
    {
        delete this;
    }

    deallocate();
}

void Context::SetCurrent(bool current)
{
    m_current = current;

    if (!m_current && m_disposed)
    {
        delete this;
    }
}

void Context::SetCurrentContext(Context* context)
{

    Context* oldContext = GetCurrentContext();

    if (oldContext != context)
    {

        if (oldContext != 0)
            oldContext->SetCurrent(false);

        currentContext = context;

        if (context != 0)
            context->SetCurrent(true);
    }
}

Context* Context::GetCurrentContext()
{

    return currentContext;
}

void Context::SetReadSurface(EGL::Surface * surface)
{
    if (m_read_surface != 0 && m_read_surface != m_draw_surface && m_read_surface != surface)
    {
        m_read_surface->SetCurrentContext(0);
    }

    m_read_surface = surface;
    m_read_surface->SetCurrentContext(this);
}


void Context::SetDrawSurface(EGL::Surface * surface)
{
    if (m_draw_surface != 0 && m_read_surface != m_draw_surface && m_draw_surface != surface)
    {
        m_draw_surface->SetCurrentContext(0);
    }

//    if (surface != 0 && !m_ViewportInitialized)
//    {
//        U16 width = surface->GetWidth();
//        U16 height = surface->GetHeight();
//
//        Viewport(0, 0, width, height);
//        Scissor(0, 0, width, height);
//        m_ViewportInitialized = true;
//    }

    m_draw_surface = surface;
    m_draw_surface->SetCurrentContext(this);

    //UpdateScissorTest();
}

// --------------------------------------------------------------------------
// Error Handling
// --------------------------------------------------------------------------
void Context::RecordError(GLenum error)
{
    switch(error)
    {
        case GL_NO_ERROR:
            break;

        case GL_INVALID_ENUM:
            printf("Error message: GL_INVALID_ENUM!\n");
            break;

        case GL_INVALID_OPERATION:
            printf("Error message: GL_INVALID_OPERATION!\n");
            break;

        case GL_STACK_OVERFLOW:
            printf("Error message: GL_STACK_OVERFLOW!\n");
            break;

        case GL_STACK_UNDERFLOW:
            printf("Error message: GL_STACK_UNDERFLOW!\n");
            break;

        case GL_OUT_OF_MEMORY:
            printf("Error message: GL_OUT_OF_MEMORY!\n");
            break;

        default:
            printf("Error message: GL_UNKNOWN!\n");
            break;

    }
}

// --------------------------------------------------------------------------
// Private Functions
// --------------------------------------------------------------------------
void Context::RebuildMatrices(void)
{
    if (m_current_stack == &m_modelview_stack)
    {
        Matrix3x3 invM;
        invM.element[ELM3(0,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,0)];
        invM.element[ELM3(0,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,1)];
        invM.element[ELM3(0,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,2)];
        invM.element[ELM3(1,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,0)];
        invM.element[ELM3(1,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,1)];
        invM.element[ELM3(1,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,2)];
        invM.element[ELM3(2,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,0)];
        invM.element[ELM3(2,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,1)];
        invM.element[ELM3(2,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,2)];

        InverseUpper3x3(invM, m_inversed_modelview, rescale_normal_enable);

    }

//    if (frame_count==199)
//    {
//        fprintf(Ctxdumper,"---------------------------- Inversed M --------------------------------\n");
//        fprintf(Ctxdumper,"(%f , %f , %f)\n", m_inversed_modelview.element[ELM3(0, 0)], m_inversed_modelview.element[ELM3(0, 1)], m_inversed_modelview.element[ELM3(0, 2)]);
//        fprintf(Ctxdumper,"(%f , %f , %f)\n", m_inversed_modelview.element[ELM3(1, 0)], m_inversed_modelview.element[ELM3(1, 1)], m_inversed_modelview.element[ELM3(1, 2)]);
//        fprintf(Ctxdumper,"(%f , %f , %f)\n", m_inversed_modelview.element[ELM3(2, 0)], m_inversed_modelview.element[ELM3(2, 1)], m_inversed_modelview.element[ELM3(2, 2)]);
//        fprintf(Ctxdumper,"------------------------------------------------------------------------\n");
//    }

}

void Context::UpdateInverseModelViewMatrix(void)
{
    Matrix3x3 invM;
    invM.element[ELM3(0,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,0)];
    invM.element[ELM3(0,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,1)];
    invM.element[ELM3(0,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(0,2)];
    invM.element[ELM3(1,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,0)];
    invM.element[ELM3(1,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,1)];
    invM.element[ELM3(1,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(1,2)];
    invM.element[ELM3(2,0)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,0)];
    invM.element[ELM3(2,1)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,1)];
    invM.element[ELM3(2,2)] = m_modelview_stack.CurrentMatrix().element[ELM4(2,2)];


    InverseUpper3x3(invM, m_inversed_modelview, rescale_normal_enable);
}

void Context::InverseUpper3x3(Matrix3x3& invM, Matrix3x3& result, bool rescale)
{
    int i;
    float d,r;
    float temp;

//        if (frame_count==199)
//        {
//            fprintf(Ctxdumper,"------------------ Upper Leftmost 3x3 from ModelView -------------------\n");
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", invM.element[ELM3(0, 0)], invM.element[ELM3(0, 1)], invM.element[ELM3(0, 2)]);
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", invM.element[ELM3(1, 0)], invM.element[ELM3(1, 1)], invM.element[ELM3(1, 2)]);
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", invM.element[ELM3(2, 0)], invM.element[ELM3(2, 1)], invM.element[ELM3(2, 2)]);
//            fprintf(Ctxdumper,"------------------------------------------------------------------------\n");
//        }

    /* compute 3x3 inverse using Cramer's rule: A^-1 = adj(A)/det(A) */
    /* cofactors */
    result.element[ELM3(0,0)] =  __GL_Det2X2(invM.element[ELM3(1,1)], invM.element[ELM3(1,2)], invM.element[ELM3(2,1)], invM.element[ELM3(2,2)]);
    result.element[ELM3(0,1)] = -__GL_Det2X2(invM.element[ELM3(1,0)], invM.element[ELM3(1,2)], invM.element[ELM3(2,0)], invM.element[ELM3(2,2)]);
    result.element[ELM3(0,2)] =  __GL_Det2X2(invM.element[ELM3(1,0)], invM.element[ELM3(1,1)], invM.element[ELM3(2,0)], invM.element[ELM3(2,1)]);

    result.element[ELM3(1,0)] = -__GL_Det2X2(invM.element[ELM3(0,1)], invM.element[ELM3(0,2)], invM.element[ELM3(2,1)], invM.element[ELM3(2,2)]);
    result.element[ELM3(1,1)] =  __GL_Det2X2(invM.element[ELM3(0,0)], invM.element[ELM3(0,2)], invM.element[ELM3(2,0)], invM.element[ELM3(2,2)]);
    result.element[ELM3(1,2)] = -__GL_Det2X2(invM.element[ELM3(0,0)], invM.element[ELM3(0,1)], invM.element[ELM3(2,0)], invM.element[ELM3(2,1)]);

    result.element[ELM3(2,0)] =  __GL_Det2X2(invM.element[ELM3(0,1)], invM.element[ELM3(0,2)], invM.element[ELM3(1,1)], invM.element[ELM3(1,2)]);
    result.element[ELM3(2,1)] = -__GL_Det2X2(invM.element[ELM3(0,0)], invM.element[ELM3(0,2)], invM.element[ELM3(1,0)], invM.element[ELM3(1,2)]);
    result.element[ELM3(2,2)] =  __GL_Det2X2(invM.element[ELM3(0,0)], invM.element[ELM3(0,1)], invM.element[ELM3(1,0)], invM.element[ELM3(1,1)]);

    /* determinant */
    d=0;

    for (i = 0; i < 3; i++)
        d += invM.element[ELM3(0,i)] * result.element[ELM3(0,i)];

    if (d == 0)
    {
        // singluar matrix
//        if (frame_count==199)
//            fprintf(Ctxdumper,"singluar matrix!\n");
        return;
    }
    else
    {
        r = 1/d;

        for (i=0; i<9; i++)
        {
            temp = result.element[i] * r;
            //printf ("%6.3f=%6.3f/%6.3f ",temp, (float)result.element[i], (float)d);
            result.element[i] = temp; //result.element[i] = GLmulF(result.element[i], r);
        }
    }
//        if (frame_count==199)
//        {
//            fprintf(Ctxdumper,"------------------ Final Inversed Matrix -------------------\n");
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", result.element[ELM3(0, 0)], result.element[ELM3(0, 1)], result.element[ELM3(0, 2)]);
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", result.element[ELM3(1, 0)], result.element[ELM3(1, 1)], result.element[ELM3(1, 2)]);
//            fprintf(Ctxdumper,"(%f , %f , %f)\n", result.element[ELM3(2, 0)], result.element[ELM3(2, 1)], result.element[ELM3(2, 2)]);
//            fprintf(Ctxdumper,"------------------------------------------------------------\n");
//        }

    if (rescale)
    {
        float sumOfSquares =
            result.element[ELM3(2, 0)] * result.element[ELM3(2, 0)] +
            result.element[ELM3(2, 1)] * result.element[ELM3(2, 1)] +
            result.element[ELM3(2, 2)] * result.element[ELM3(2, 2)];

        if (sumOfSquares != __GL_F_ONE)
        {
            float factor = 1/sqrt(sumOfSquares);

            for (size_t index = 0; index < 9; ++index)
            {
                result.element[index] = result.element[index] * factor;
            }
        }
    }
}

void Context::MatrixTransformVec4(Matrix4x4& matrix, Vertex4& vec4)
{
    float temp[4];
    temp[0] = vec4.x * matrix.element[ELM4(0,0)] + vec4.y * matrix.element[ELM4(0,1)] + vec4.z * matrix.element[ELM4(0,2)] + vec4.w * matrix.element[ELM4(0,3)];
    temp[1] = vec4.x * matrix.element[ELM4(1,0)] + vec4.y * matrix.element[ELM4(1,1)] + vec4.z * matrix.element[ELM4(1,2)] + vec4.w * matrix.element[ELM4(1,3)];
    temp[2] = vec4.x * matrix.element[ELM4(2,0)] + vec4.y * matrix.element[ELM4(2,1)] + vec4.z * matrix.element[ELM4(2,2)] + vec4.w * matrix.element[ELM4(2,3)];
    temp[3] = vec4.x * matrix.element[ELM4(3,0)] + vec4.y * matrix.element[ELM4(3,1)] + vec4.z * matrix.element[ELM4(3,2)] + vec4.w * matrix.element[ELM4(3,3)];

    vec4.x = temp[0];
    vec4.y = temp[1];
    vec4.z = temp[2];
    vec4.w = temp[3];
    //printf("Position: %f, %f, %f, %f\n", vec4.x, vec4.y, vec4.z, vec4.w);
}

void Context::MatrixTransformVec3(Matrix4x4& matrix, Vector3& vec3)
{
    float temp[3];
    temp[0] = vec3.v_x * matrix.element[ELM4(0,0)] + vec3.v_y * matrix.element[ELM4(0,1)] + vec3.v_z * matrix.element[ELM4(0,2)];
    temp[1] = vec3.v_x * matrix.element[ELM4(1,0)] + vec3.v_y * matrix.element[ELM4(1,1)] + vec3.v_z * matrix.element[ELM4(1,2)];
    temp[2] = vec3.v_x * matrix.element[ELM4(2,0)] + vec3.v_y * matrix.element[ELM4(2,1)] + vec3.v_z * matrix.element[ELM4(2,2)];

    vec3.v_x = temp[0];
    vec3.v_y = temp[1];
    vec3.v_z = temp[2];
}

void Context::Toggle(GLenum cap, bool value)
{
    //
    switch (cap)
    {
    case GL_LIGHTING:
        lighting_enable = value;
        break;

    case GL_FOG:
        fog.enable = value;
        break;

    case GL_TEXTURE_2D:
        texture_state[0].enable = value;
        break;

    case GL_CULL_FACE:
        culling_enable = value;
        break;

    case GL_ALPHA_TEST:
        alpha_test.enable = value;
        break;

    case GL_BLEND:
        blend_func.enable = value;
        break;

    case GL_COLOR_LOGIC_OP:
        logic_op.enable = value;
        break;

    case GL_DITHER:
        break;

    case GL_STENCIL_TEST:
        stencil_test.enable = value;
        break;

    case GL_DEPTH_TEST:
        depth_test.enable = value;
        break;

    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
    case GL_LIGHT4:
    case GL_LIGHT5:
    case GL_LIGHT6:
    case GL_LIGHT7:
    {
        int mask = 1 << (cap - GL_LIGHT0);

        if (value)
        {
            lihgt_unit_mask |= mask;
        }
        else
        {
            lihgt_unit_mask &= ~mask;
        }
    }
    break;

    case GL_POINT_SMOOTH:
        break;

    case GL_POINT_SPRITE_OES:
        break;

    case GL_LINE_SMOOTH:
        break;

    case GL_SCISSOR_TEST:
        scissor_test.enable = value;

        //UpdateScissorTest();

        break;

    case GL_COLOR_MATERIAL:
        color_material_enable = value;
        break;

    case GL_NORMALIZE:
        normalize_enable = value;
        break;

    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
        /*{
            size_t plane = cap - GL_CLIP_PLANE0;
            U32 mask = ~(1u << plane);
            U32 bit = value ? (1u << plane) : 0;

            clip_plane_mask = (clip_plane_mask & mask) | bit;
        }*/

        break;

    case GL_RESCALE_NORMAL:
        rescale_normal_enable = value;
        UpdateInverseModelViewMatrix();
        break;

    case GL_POLYGON_OFFSET_FILL:
        depth_offset.enable = value;
        break;

    case GL_MULTISAMPLE:
        aa_enable = value;
        break;

    case GL_SAMPLE_ALPHA_TO_COVERAGE:
        break;

    case GL_SAMPLE_ALPHA_TO_ONE:
        break;

    case GL_SAMPLE_COVERAGE:
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }
}

// --------------------------------------------------------------------------
// UpdateScissorTest() by Vincent
// Update the stencil test settings based on
//	- the current viewport size
//	- the current drawing surface dimensions
//	- the actual scissor test settings
// we do not need any scissoring within the rasterizer as long as the viewport
// rectangle is contained inside the surface area and (if the scissor test
// is enabled) the scissor rectangle. Otherwise, determine the size
// of the surface area (& optionally intersected with the scissor rectangle)
// and enable scissoring in the rasterizer accordingly.
// --------------------------------------------------------------------------
