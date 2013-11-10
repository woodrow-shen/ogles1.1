// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : float_api.h
// Purpose : GL Float API functions
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "context.h"
#include "rasterizer.h"
#include "drv_impl.h"

using namespace EGL;

namespace
{
    float CLAMP(float value, float min, float max)
    {
        if (value < min)
        {
            return min;
        }
        else if (value > max)
        {
            return max;
        }
        else
        {
            return value;
        }
    }
}

void Context::AlphaFunc(GLenum func, GLclampf ref)
{
    alpha_test.func = func;
    alpha_test.ref_val = ref;

    stateLinkList = addState(GL_ALPHA_TEST_FUNC, static_cast<unsigned int>(func), stateLinkList);
    stateLinkList = addState(GL_ALPHA_TEST_REF, static_cast<unsigned int>(__GL_FLOAT_2_S(ref)), stateLinkList);
    state_counter+= 2;
}

void Context::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    clear_color.red   = red;
    clear_color.green = green;
    clear_color.blue  = blue;
    clear_color.alpha = alpha;

    // Set RM's registers
    U32 color = static_cast<U8>(clear_color.red*255)<<24|static_cast<U8>(clear_color.green*255)<<16|static_cast<U8>(clear_color.blue*255)<< 8|static_cast<U8>(clear_color.alpha*255);
    rm.SetStateReg(GL_COLOR_CLEAR_VALUE, color);
}

void Context::ClearDepthf(GLclampf depth)
{
    clear_depth = __GL_F_2_CLAMPF(depth);

    // Set RM's registers
    //rm.SetStateReg(GL_DEPTH_CLEAR_VALUE, );
    rm.DepthRef = clear_depth;
}

void Context::Color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    current_color.red   = red;
    current_color.green = green;
    current_color.blue  = blue;
    current_color.alpha = alpha;
}

void Context::DepthRangef(GLclampf zNear, GLclampf zFar)
{
    vp.dp_range_near = zNear;
    vp.dp_range_far = zFar;
}

void Context::Fogf(GLenum pname, const GLfloat param)
{
    switch (pname)
    {
    case GL_FOG_MODE:
        fog.mode = static_cast<GLenum>(param);
        stateLinkList = addState(GL_FOG_MODE, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;

    case GL_FOG_DENSITY:
        if (param < 0)
            RecordError(GL_INVALID_VALUE);
        else
        {
            fog.density = param;
            stateLinkList = addState(GL_FOG_DENSITY, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
            state_counter++;
        }
        break;

    case GL_FOG_START:
        fog.start = param;
        stateLinkList = addState(GL_FOG_START, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
        state_counter++;
        break;

    case GL_FOG_END:
        fog.end = param;
        stateLinkList = addState(GL_FOG_END, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
        state_counter++;
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

void Context::Fogfv(GLenum pname, const GLfloat *params)
{

    switch (pname)
    {
    case GL_FOG_MODE:
    {
        GLenum param;
        param = static_cast<GLenum>(*params);
        fog.mode = param;
        stateLinkList = addState(GL_FOG_MODE, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_DENSITY:
    {
        GLfloat param;
        param = static_cast<GLfloat>(*params);
        if (param < 0)
            RecordError(GL_INVALID_VALUE);
        else
        {
            fog.density = param;
            stateLinkList = addState(GL_FOG_DENSITY, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
            state_counter++;
        }
        break;
    }
    case GL_FOG_START:
    {
        GLfloat param;
        param = static_cast<GLfloat>(*params);
        fog.start = param;
        stateLinkList = addState(GL_FOG_START, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_END:
    {
        GLfloat param;
        param = static_cast<GLfloat>(*params);
        fog.end = param;
        stateLinkList = addState(GL_FOG_END, static_cast<unsigned int>(__GL_FLOAT_2_X(param)), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_COLOR:
    {
        fog.cf.red   = __GL_F_2_CLAMPF(params[0]);
        fog.cf.green = __GL_F_2_CLAMPF(params[1]);
        fog.cf.blue  = __GL_F_2_CLAMPF(params[2]);
        fog.cf.alpha = __GL_F_2_CLAMPF(params[3]);

        unsigned int cf = __GL_FLOAT_2_S(fog.cf.red)<<24|__GL_FLOAT_2_S(fog.cf.green)<<16|__GL_FLOAT_2_S(fog.cf.blue)<<8|__GL_FLOAT_2_S(fog.cf.alpha);
        stateLinkList = addState(GL_FOG_COLOR, cf, stateLinkList);
        state_counter++;
        break;
    }

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

void Context::Frustumf(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    if (l == r || t == b || n <= 0 || f <= 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    Matrix4x4 matrix;

    GLfloat inv_width = (r - l) ? 1/(r - l) : 0;
    GLfloat inv_height = (t - b) ? 1/(t - b) : 0;
    GLfloat inv_depth = (f - n) ? 1/(f - n) : 0;

    GLfloat two_n = n * 2;

    matrix.element[ELM4(0, 0)] = two_n * inv_width;
    matrix.element[ELM4(0, 2)] = (r+l) * inv_width;

    matrix.element[ELM4(1, 1)] = two_n * inv_height;
    matrix.element[ELM4(1, 2)] = (t+b) * inv_height;

    matrix.element[ELM4(2, 2)] = - ((f+n) * inv_depth);
    matrix.element[ELM4(2, 3)] = - (two_n*f * inv_depth);

    matrix.element[ELM4(3, 2)] = -1;
    matrix.element[ELM4(3, 3)] = 0;

    CurrentMatrixStack()->MultMatrix(matrix);
#ifdef PRINT_TRANSFORMATION
    Matrix4x4& mtx = CurrentMatrixStack()->CurrentMatrix();
    fprintf(Ctxdumper,"---------------------------- Projection after Frustum --------------------------------------------\n");
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(0, 0)], mtx.element[ELM4(0, 1)], mtx.element[ELM4(0, 2)], mtx.element[ELM4(0, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(1, 0)], mtx.element[ELM4(1, 1)], mtx.element[ELM4(1, 2)], mtx.element[ELM4(1, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(2, 0)], mtx.element[ELM4(2, 1)], mtx.element[ELM4(2, 2)], mtx.element[ELM4(2, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(3, 0)], mtx.element[ELM4(3, 1)], mtx.element[ELM4(3, 2)], mtx.element[ELM4(3, 3)]);
    fprintf(Ctxdumper,"--------------------------------------------------------------------------------------------------\n");
#endif
}

void Context::LightModelf(GLenum pname, GLfloat param)
{
    switch (pname)
    {
    case GL_LIGHT_MODEL_TWO_SIDE:
        light_model_two_side = (param != 0);
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

void Context::LightModelfv(GLenum pname, const GLfloat *params)
{
    switch (pname)
    {
    case GL_LIGHT_MODEL_AMBIENT:
        light_model_ambient.red   = params[0];
        light_model_ambient.green = params[1];
        light_model_ambient.blue  = params[2];
        light_model_ambient.alpha = params[3];
        break;

    default:
        LightModelf(pname, *params);
        break;

    }
}

void Context::Lightf(GLenum light, GLenum pname, GLfloat param)
{
    if (light < GL_LIGHT0 || light > GL_LIGHT7)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    short lightNO = light - GL_LIGHT0;

    switch (pname)
    {
    case GL_SPOT_EXPONENT:
        if (param < 0 || param > 128.0)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            s_light[lightNO].source_spot_exponent = param;
        }
        break;

    case GL_SPOT_CUTOFF:
        if (param < 0 || param > 90.0 && param != 180.0)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            s_light[lightNO].source_spot_curoff = param;
        }
        break;

    case GL_CONSTANT_ATTENUATION:
        s_light[lightNO].source_constant_attenuation = param;
        break;

    case GL_LINEAR_ATTENUATION:
        s_light[lightNO].source_linear_attenuation = param;
        break;

    case GL_QUADRATIC_ATTENUATION:
        s_light[lightNO].source_quadratic_attenuation = param;
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;

    }
}

void Context::Lightfv(GLenum light, GLenum pname, const GLfloat *params)
{
    if (light < GL_LIGHT0 || light > GL_LIGHT7)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    short lightNO = light - GL_LIGHT0;

    switch (pname)
    {
    case GL_AMBIENT:
        s_light[lightNO].source_ambient.red   = params[0];
        s_light[lightNO].source_ambient.green = params[1];
        s_light[lightNO].source_ambient.blue  = params[2];
        s_light[lightNO].source_ambient.alpha = params[3];
        break;

    case GL_DIFFUSE:
        s_light[lightNO].source_diffuse.red   = params[0];
        s_light[lightNO].source_diffuse.green = params[1];
        s_light[lightNO].source_diffuse.blue  = params[2];
        s_light[lightNO].source_diffuse.alpha = params[3];
        break;

    case GL_SPECULAR:
        s_light[lightNO].source_specular.red   = params[0];
        s_light[lightNO].source_specular.green = params[1];
        s_light[lightNO].source_specular.blue  = params[2];
        s_light[lightNO].source_specular.alpha = params[3];
        break;

    case GL_POSITION:
    {
        Vertex4 pos;
        pos.x = params[0];
        pos.y = params[1];
        pos.z = params[2];
        pos.w = params[3];

        MatrixTransformVec4(m_modelview_stack.CurrentMatrix(), pos);

        s_light[lightNO].source_position.x = pos.x;
        s_light[lightNO].source_position.y = pos.y;
        s_light[lightNO].source_position.z = pos.z;
        s_light[lightNO].source_position.w = pos.w;
        break;
    }
    case GL_SPOT_DIRECTION:
    {
        Vector3 vec3;

        vec3.v_x = params[0];
        vec3.v_y = params[1];
        vec3.v_z = params[2];

        MatrixTransformVec3(m_modelview_stack.CurrentMatrix(), vec3);

        s_light[lightNO].source_spot_direction.v_x = vec3.v_x;
        s_light[lightNO].source_spot_direction.v_y = vec3.v_y;
        s_light[lightNO].source_spot_direction.v_z = vec3.v_z;

        break;
    }
    default:
        Lightf(light, pname, *params);
        break;
    }
}

void Context::LoadMatrixf(const GLfloat *m)
{
    CurrentMatrixStack()->LoadMatrix(m);
    RebuildMatrices();
}

void Context::Materialf(GLenum face, GLenum pname, GLfloat param)
{
    if (face != GL_FRONT_AND_BACK)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    switch (pname)
    {
    case GL_SHININESS:
        if (param < 0 || param > 128.0)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            m_light.material_shininess = param;
        }

        break;

    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }
}

void Context::Materialfv(GLenum face, GLenum pname, const GLfloat *params)
{
    if (face != GL_FRONT_AND_BACK)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    switch (pname)
    {
    case GL_AMBIENT:
        m_light.material_ambient.red   = params[0];
        m_light.material_ambient.green = params[1];
        m_light.material_ambient.blue  = params[2];
        m_light.material_ambient.alpha = params[3];
        break;

    case GL_DIFFUSE:
        m_light.material_diffuse.red   = params[0];
        m_light.material_diffuse.green = params[1];
        m_light.material_diffuse.blue  = params[2];
        m_light.material_diffuse.alpha = params[3];
        break;

    case GL_AMBIENT_AND_DIFFUSE:
        m_light.material_ambient.red   = params[0];
        m_light.material_ambient.green = params[1];
        m_light.material_ambient.blue  = params[2];
        m_light.material_ambient.alpha = params[3];

        m_light.material_diffuse.red   = params[0];
        m_light.material_diffuse.green = params[1];
        m_light.material_diffuse.blue  = params[2];
        m_light.material_diffuse.alpha = params[3];
        break;

    case GL_SPECULAR:
        m_light.material_specular.red   = params[0];
        m_light.material_specular.green = params[1];
        m_light.material_specular.blue  = params[2];
        m_light.material_specular.alpha = params[3];
        break;

    case GL_EMISSION:
        m_light.material_emission.red   = params[0];
        m_light.material_emission.green = params[1];
        m_light.material_emission.blue  = params[2];
        m_light.material_emission.alpha = params[3];
        break;

    default:
        Materialf(face, pname, *params);
        break;
    }
}

void Context::MultMatrixf(const GLfloat *m)
{
    CurrentMatrixStack()->MultMatrix(m);
    RebuildMatrices();
}

void Context::Normal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    current_normal.nx = nx;
    current_normal.ny = ny;
    current_normal.nz = nz;
}

void Context::Orthof(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat n, GLfloat f)
{
    Matrix4x4 matrix;

    GLfloat inv_width = (r - l) ? 1/(r - l) : 0;
    GLfloat inv_height = (t - b) ? 1/(t - b) : 0;
    GLfloat inv_depth = (f - n) ? 1/(f - n) : 0;

    matrix.element[ELM4(0, 0)] = 2 * inv_width;
    matrix.element[ELM4(0, 3)] = - ((r+l) * inv_width);

    matrix.element[ELM4(1, 1)] = 2 * inv_height;
    matrix.element[ELM4(1, 3)] = - ((t+b) * inv_height);

    matrix.element[ELM4(2, 2)] = - 2 * inv_depth;
    matrix.element[ELM4(2, 3)] = - ((f+n) * inv_depth);

    CurrentMatrixStack()->MultMatrix(matrix);
#ifdef PRINT_TRANSFORMATION
    Matrix4x4& mtx = CurrentMatrixStack()->CurrentMatrix();
    fprintf(Ctxdumper,"---------------------------- Projection after Ortho ----------------------------------------------\n");
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(0, 0)], mtx.element[ELM4(0, 1)], mtx.element[ELM4(0, 2)], mtx.element[ELM4(0, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(1, 0)], mtx.element[ELM4(1, 1)], mtx.element[ELM4(1, 2)], mtx.element[ELM4(1, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(2, 0)], mtx.element[ELM4(2, 1)], mtx.element[ELM4(2, 2)], mtx.element[ELM4(2, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(3, 0)], mtx.element[ELM4(3, 1)], mtx.element[ELM4(3, 2)], mtx.element[ELM4(3, 3)]);
    fprintf(Ctxdumper,"--------------------------------------------------------------------------------------------------\n");
#endif
}

void Context::PolygonOffset(GLfloat factor, GLfloat units)
{
    depth_offset.factor = factor;
    depth_offset.units = units;
}

void Context::Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    Matrix4x4 matrix;

    Vector3 axis;
    axis.v_x = x;
    axis.v_y = y;
    axis.v_z = z;

    axis.Normalize();

    angle = __GL_F_DEG_2_RAD(angle);

    GLfloat c = cos(angle);
    GLfloat s = sin(angle);
    GLfloat one_minus_cosine = 1.0-c;

    matrix.element[ELM4(0, 0)] = c + one_minus_cosine * axis.v_x * axis.v_x;
    matrix.element[ELM4(0, 1)] = one_minus_cosine * axis.v_x * axis.v_y - axis.v_z * s;
    matrix.element[ELM4(0, 2)] = one_minus_cosine * axis.v_x * axis.v_z + axis.v_y * s;

    matrix.element[ELM4(1, 0)] = one_minus_cosine * axis.v_x * axis.v_y + axis.v_z * s;
    matrix.element[ELM4(1, 1)] = c + one_minus_cosine * axis.v_y * axis.v_y;
    matrix.element[ELM4(1, 2)] = one_minus_cosine * axis.v_y * axis.v_z - axis.v_x * s;

    matrix.element[ELM4(2, 0)] = one_minus_cosine * axis.v_x * axis.v_z - axis.v_y * s;
    matrix.element[ELM4(2, 1)] = one_minus_cosine * axis.v_y * axis.v_z + axis.v_x * s;
    matrix.element[ELM4(2, 2)] = c + one_minus_cosine * axis.v_z * axis.v_z;

    CurrentMatrixStack()->MultMatrix(matrix);
    RebuildMatrices();
#ifdef PRINT_TRANSFORMATION
    Matrix4x4& mtx = CurrentMatrixStack()->CurrentMatrix();
    fprintf(Ctxdumper,"---------------------------- ModelView after Rotate -------------------------------\n");
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(0, 0)], mtx.element[ELM4(0, 1)], mtx.element[ELM4(0, 2)], mtx.element[ELM4(0, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(1, 0)], mtx.element[ELM4(1, 1)], mtx.element[ELM4(1, 2)], mtx.element[ELM4(1, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(2, 0)], mtx.element[ELM4(2, 1)], mtx.element[ELM4(2, 2)], mtx.element[ELM4(2, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(3, 0)], mtx.element[ELM4(3, 1)], mtx.element[ELM4(3, 2)], mtx.element[ELM4(3, 3)]);
    fprintf(Ctxdumper,"-----------------------------------------------------------------------------------\n");
#endif
}

void Context::Scalef(GLfloat x, GLfloat y, GLfloat z)
{
    Matrix4x4 matrix;

    matrix.element[ELM4(0, 0)] = x;
    matrix.element[ELM4(1, 1)] = y;
    matrix.element[ELM4(2, 2)] = z;

    CurrentMatrixStack()->MultMatrix(matrix);
    RebuildMatrices();
#ifdef PRINT_TRANSFORMATION
    Matrix4x4& mtx = CurrentMatrixStack()->CurrentMatrix();
    fprintf(Ctxdumper,"---------------------------- ModelView after Scale --------------------------------\n");
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(0, 0)], mtx.element[ELM4(0, 1)], mtx.element[ELM4(0, 2)], mtx.element[ELM4(0, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(1, 0)], mtx.element[ELM4(1, 1)], mtx.element[ELM4(1, 2)], mtx.element[ELM4(1, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(2, 0)], mtx.element[ELM4(2, 1)], mtx.element[ELM4(2, 2)], mtx.element[ELM4(2, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(3, 0)], mtx.element[ELM4(3, 1)], mtx.element[ELM4(3, 2)], mtx.element[ELM4(3, 3)]);
    fprintf(Ctxdumper,"-----------------------------------------------------------------------------------\n");
#endif
}

void Context::TexEnvf(GLenum target, GLenum pname, GLfloat param)
{
    switch (target)
    {
    case GL_TEXTURE_ENV:

        switch (pname)
        {
        case GL_TEXTURE_ENV_MODE:
            /* set texture functions you want*/
            switch ((GLenum)param)
            {
            case GL_MODULATE:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            case GL_REPLACE:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            case GL_DECAL:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            case GL_BLEND:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            case GL_ADD:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            case GL_COMBINE:
                texture_state[0].env_mode = (GLenum)param;
                texvec[texture_state[0].texture_bindID].env_mode = (GLenum)param;
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;
            }
            break;

        case GL_COMBINE_RGB:
            /* use COMBINE function, and set mode that RGB computed */
            switch ((GLenum)param)
            {
            case GL_REPLACE:
                break;

            case GL_MODULATE:
                break;

            case GL_ADD:
                break;

            case GL_ADD_SIGNED:
                break;

            case GL_INTERPOLATE:
                break;

            case GL_SUBTRACT:
                break;

            case GL_DOT3_RGB:
                break;

            case GL_DOT3_RGBA:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;
            }
            break;

        case GL_COMBINE_ALPHA:
            /* use COMBINE function, and set mode that ALPHA computed */
            switch ((GLenum)param)
            {
            case GL_REPLACE:
                break;

            case GL_MODULATE:
                break;

            case GL_ADD:
                break;

            case GL_ADD_SIGNED:
                break;

            case GL_INTERPOLATE:
                break;

            case GL_SUBTRACT:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;
            }
            break;

        case GL_SRC0_RGB:
        case GL_SRC1_RGB:
        case GL_SRC2_RGB:
            /* set arguments of COMBINE_RGB functions */
        {
            size_t index = pname - GL_SRC0_RGB;

            switch ((GLenum)param)
            {
            case GL_TEXTURE:
                break;

            case GL_CONSTANT:
                break;

            case GL_PRIMARY_COLOR:
                break;

            case GL_PREVIOUS:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;

            }
        }
        break;

        case GL_SRC0_ALPHA:
        case GL_SRC1_ALPHA:
        case GL_SRC2_ALPHA:
            /* set arguments of COMBINE_ALPHA functions */
        {
            size_t index = pname - GL_SRC0_ALPHA;

            switch ((GLenum)param)
            {
            case GL_TEXTURE:
                break;

            case GL_CONSTANT:
                break;

            case GL_PRIMARY_COLOR:
                break;

            case GL_PREVIOUS:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;

            }
        }
        break;

        case GL_OPERAND0_RGB:
        case GL_OPERAND1_RGB:
        case GL_OPERAND2_RGB:
            /* set arguments of COMBINE_RGB functions */
        {
            size_t index = pname - GL_OPERAND0_RGB;

            switch ((GLenum)param)
            {
            case GL_SRC_COLOR:
                break;

            case GL_ONE_MINUS_SRC_COLOR:
                break;

            case GL_SRC_ALPHA:
                break;

            case GL_ONE_MINUS_SRC_ALPHA:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;

            }
        }

        case GL_OPERAND0_ALPHA:
        case GL_OPERAND1_ALPHA:
        case GL_OPERAND2_ALPHA:
            /* set arguments of COMBINE_ALPHA functions */
        {
            size_t index = pname - GL_OPERAND0_ALPHA;

            switch ((GLenum)param)
            {
            case GL_SRC_ALPHA:
                break;

            case GL_ONE_MINUS_SRC_ALPHA:
                break;

            default:
                RecordError(GL_INVALID_ENUM);
                break;
            }
        }
        break;

        case GL_RGB_SCALE:
            break;

        case GL_ALPHA_SCALE:
            break;

        default:
            RecordError(GL_INVALID_ENUM);
            break;
        }// pname
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }// target

//    state_counter++;
}

void Context::TexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
    switch (target)
    {
    case GL_TEXTURE_ENV:

        switch (pname)
        {
        case GL_TEXTURE_ENV_COLOR:
        {
            float red   = __GL_F_2_CLAMPF(params[0]);
            float green = __GL_F_2_CLAMPF(params[1]);
            float blue  = __GL_F_2_CLAMPF(params[2]);
            float alpha = __GL_F_2_CLAMPF(params[3]);

            unsigned int cf = __GL_FLOAT_2_S(red)<<24|__GL_FLOAT_2_S(green)<<16|__GL_FLOAT_2_S(blue)<<8|__GL_FLOAT_2_S(alpha);

            texture_state[0].env_color = cf;
            texvec[texture_state[0].texture_bindID].env_color = cf;
            break;
        }

        default:
            TexEnvf(target, pname, *params);
            break;

        }
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;

    }
}

void Context::TexParameteri(GLenum target, GLenum pname, GLint param)
{
    if (target != GL_TEXTURE_2D)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    switch (pname)
    {
    case GL_TEXTURE_MIN_FILTER:
        /*****************************************************
            GL_NEAREST or GL_LINEAR mode : Not apply mipmap

            GL_NEAREST_MIPMAP_NEAREST
            GL_NEAREST_MIPMAP_LINEAR     apply mipmap
            GL_LINEAR_MIPMAP_NEAREST
            GL_LINEAR_MIPMAP_LINEAR
        /****************************************************/
        texture_state[active_texture].Min_filter_mode = param;
        texvec[texture_state[0].texture_bindID].Min_filter_mode = param;
        break;

    case GL_TEXTURE_MAG_FILTER:
        /*****************************************************
            support GL_NEAREST or GL_LINEAR mode
        /****************************************************/
        texture_state[active_texture].Mag_filter_mode = param;
        texvec[texture_state[0].texture_bindID].Mag_filter_mode = param;
        break;

    case GL_TEXTURE_WRAP_S:
        texture_state[active_texture].wrap_s = param;
        texvec[texture_state[0].texture_bindID].wrap_s = param;
        break;

    case GL_TEXTURE_WRAP_T:
        texture_state[active_texture].wrap_t = param;
        texvec[texture_state[0].texture_bindID].wrap_t = param;
        break;

    case GL_GENERATE_MIPMAP:
        texture_state[active_texture].auto_genMipmap = (param != 0);
        texvec[texture_state[0].texture_bindID].auto_genMipmap = (param != 0);
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

void Context::TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    if (target != GL_TEXTURE_2D)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    TexParameteri(target, pname, (GLint)param);
}

void Context::Translatef(GLfloat x, GLfloat y, GLfloat z)
{
    Matrix4x4 matrix;

    matrix.element[ELM4(0, 3)] = x;
    matrix.element[ELM4(1, 3)] = y;
    matrix.element[ELM4(2, 3)] = z;

    CurrentMatrixStack()->MultMatrix(matrix);
    RebuildMatrices();
#ifdef PRINT_TRANSFORMATION
    Matrix4x4& mtx = CurrentMatrixStack()->CurrentMatrix();
    fprintf(Ctxdumper,"---------------------------- ModelView after Translate ----------------------------\n");
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(0, 0)], mtx.element[ELM4(0, 1)], mtx.element[ELM4(0, 2)], mtx.element[ELM4(0, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(1, 0)], mtx.element[ELM4(1, 1)], mtx.element[ELM4(1, 2)], mtx.element[ELM4(1, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(2, 0)], mtx.element[ELM4(2, 1)], mtx.element[ELM4(2, 2)], mtx.element[ELM4(2, 3)]);
    fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mtx.element[ELM4(3, 0)], mtx.element[ELM4(3, 1)], mtx.element[ELM4(3, 2)], mtx.element[ELM4(3, 3)]);
    fprintf(Ctxdumper,"-----------------------------------------------------------------------------------\n");
#endif
}

/*****************************************************************************************/
/*                                 non-implemented functions                             */
/*****************************************************************************************/

void Context::ClipPlanef(GLenum plane, const GLfloat *equation)
{

}

void Context::GetClipPlanef(GLenum pname, GLfloat eqn[4])
{

}

void Context::GetFloatv(GLenum pname, GLfloat *params)
{

}

void Context::GetLightfv(GLenum light, GLenum pname, GLfloat *params)
{

}

void Context::GetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{

}

void Context::GetTexEnvfv(GLenum env, GLenum pname, GLfloat *params)
{

}

void Context::GetTexParameterfv(GLenum target, GLenum pname, GLfloat *params)
{

}

void Context::LineWidth(GLfloat width)
{

}

void Context::MultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{

}

void Context::PointParameterf(GLenum pname, GLfloat param)
{

}

void Context::PointParameterfv(GLenum pname, const GLfloat *params)
{

}

void Context::PointSize(GLfloat size)
{

}

void Context::SampleCoverage(GLclampf value, GLboolean invert)
{

}

