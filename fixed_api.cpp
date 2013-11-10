// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : fixed_api.cpp
// Purpose : GL Fixed API functions
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "context.h"
#include "drv_impl.h"
#include "fixed.h"

using namespace EGL;

//stateLinkList = addState(GL_FOG, value, stateLinkList);
namespace
{
    bool IsPowerOf2(GLsizei value)
    {
        if (!value)
            return true;

        while (!(value & 1))
            value >>= 1;

        value >>= 1;

        return value == 0;
    }
}


void Context::ActiveTexture(GLenum texture)
{
    if (texture < GL_TEXTURE0 || texture >= GL_TEXTURE0 + EGL_NUM_TEXTURE_UNITS)
    {
        RecordError(GL_INVALID_ENUM);
    }

    active_texture = texture - GL_TEXTURE0;

    if (m_matrix_mode == GL_TEXTURE)
    {
        m_current_stack = &m_texture_stack[active_texture];
    }

    //ateLinkList = addState(GL_ACTIVE_TEXTURE, static_cast<unsigned int>(active_texture), stateLinkList);
    //state_counter++;
}

/******************* fixed point ***************************/
void Context::AlphaFuncx(GLenum func, GLclampx ref)
{
    Fixed refer(ref);

    alpha_test.func = func;
    alpha_test.ref_val = refer;

    stateLinkList = addState(GL_ALPHA_TEST_FUNC, static_cast<unsigned int>(func), stateLinkList);
    stateLinkList = addState(GL_ALPHA_TEST_REF, static_cast<unsigned int>(__GL_FLOAT_2_S(alpha_test.ref_val)), stateLinkList);
    state_counter+= 2;
}

void Context::BindTexture(GLenum target, GLuint texture)
{
    if (target != GL_TEXTURE_2D)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    texture_state[0].texture_bindID = texture;

    stateLinkList = addState(GL_TEXTURE_BINDING_2D, texture, stateLinkList);
    state_counter++;
}

void Context::BlendFunc(GLenum sfactor, GLenum dfactor)
{
    blend_func.src = sfactor;
    blend_func.dst = dfactor;

    stateLinkList = addState(GL_BLEND_SRC, sfactor, stateLinkList);
    stateLinkList = addState(GL_BLEND_DST, dfactor, stateLinkList);

    state_counter+= 2;
}

void Context::Clear(GLbitfield mask)
{
    if ((mask & (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != mask)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    clear_buffer_mask = mask;
}

/******************* fixed point ***************************/
void Context::ClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    // Set RM's registers
    Fixed r(__GL_X_2_CLAMPX(red));
    Fixed g(__GL_X_2_CLAMPX(green));
    Fixed b(__GL_X_2_CLAMPX(blue));
    Fixed a(__GL_X_2_CLAMPX(alpha));

    clear_color.red   = r;
    clear_color.green = g;
    clear_color.blue  = b;
    clear_color.alpha = a;

    U32 color = static_cast<U8>(clear_color.red*255)<<24|static_cast<U8>(clear_color.green*255)<<16|static_cast<U8>(clear_color.blue*255)<< 8|static_cast<U8>(clear_color.alpha*255);
    rm.SetStateReg(GL_COLOR_CLEAR_VALUE, color);
}

/******************* fixed point ***************************/
void Context::ClearDepthx(GLclampx depth)
{
    Fixed c_depth(__GL_X_2_CLAMPX(depth));

    clear_depth = c_depth;
    // Set RM's registers
    //rm.SetStateReg(GL_DEPTH_CLEAR_VALUE, );
    rm.DepthRef = clear_depth;
}

void Context::ClearStencil(GLint s)
{
    clear_stencil = s;
}

void Context::ClientActiveTexture(GLenum texture)
{
    if (texture < GL_TEXTURE0 || texture >= GL_TEXTURE0 + EGL_NUM_TEXTURE_UNITS)
    {
        RecordError(GL_INVALID_ENUM);
    }

    client_active_texture = texture - GL_TEXTURE0;

    //state_counter++;
}

/******************* fixed point ***************************/
void Context::Color4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    Fixed r(red);
    Fixed g(green);
    Fixed b(blue);
    Fixed a(alpha);

    current_color.red   = r;
    current_color.green = g;
    current_color.blue  = b;
    current_color.alpha = a;
}

void Context::ColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if (size != 4)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    switch(type)
    {
        case GL_FIXED:
        case GL_UNSIGNED_BYTE:
        case GL_FLOAT:
            break;

        default:
            RecordError(GL_INVALID_ENUM);
            return;
    }

    if (stride < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    color_array.size = size;
    color_array.type = type;
    color_array.stride = stride;
    color_array.ptr = (pointer);
}

void Context::CullFace(GLenum mode)
{
    cull_face = mode;
}

void Context::DeleteTextures(GLsizei n, const GLuint *textures)
{
    // TODO : build a complete data structure of texture object
    //        -> garbage collection mechanism
    if (n < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    printf("DeleteTextures-> base:[%d] (VEC size:[%d])\n", *textures, texvec.size());

    vector<TextureState>::iterator startIterator;

    for (int i=0;i<n;++i)
    {
        startIterator = texvec.begin()+ textures[i];
        texvec.erase(startIterator);
    }
    printf("DeleteTextures-> VEC size:[%d], capacity:[%d]\n", texvec.size(), texvec.capacity());
//    delete[] texobj->teximage->data;
//    delete   texobj->teximage;
//    delete[] texobj;
}

void Context::DepthFunc(GLenum func)
{
    depth_test.func = func;

    stateLinkList = addState(GL_DEPTH_FUNC, func, stateLinkList);
    state_counter++;
}

/******************* fixed point ***************************/
void Context::DepthRangex(GLclampx zNear, GLclampx zFar)
{
    Fixed zN(__GL_X_2_CLAMPX(zNear));
    Fixed zF(__GL_X_2_CLAMPX(zFar));

    vp.dp_range_near = zN;
    vp.dp_range_far = zF;
}

void Context::Disable(GLenum cap)
{
    Toggle(cap, false);

    switch (cap)
    {
    case GL_FOG:
    case GL_TEXTURE_2D:
    case GL_ALPHA_TEST:
    case GL_BLEND:
    case GL_COLOR_LOGIC_OP:
    case GL_DITHER:
    case GL_STENCIL_TEST:
    case GL_DEPTH_TEST:
    case GL_SCISSOR_TEST:
    case GL_MULTISAMPLE:
        stateLinkList = addState(cap, static_cast<unsigned int>(false), stateLinkList);
        state_counter++;
        break;
    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }
}

void Context::DisableClientState(GLenum array)
{
    switch (array)
    {
    case GL_VERTEX_ARRAY:
        vertex_array.enable = GL_FALSE;
        break;

    case GL_NORMAL_ARRAY:
        normal_array.enable = GL_FALSE;
        break;

    case GL_COLOR_ARRAY:
        color_array.enable = GL_FALSE;
        break;

    case GL_TEXTURE_COORD_ARRAY:
        texture_coord_array.enable = GL_FALSE;
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

/****************************************************************
            Main action : execute a DrawArrays command
 ---------------------------------------------------------------
     activate back-end GPU simulator to run in pipeline

****************************************************************/
void Context::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (first < 0)
    {
        RecordError(GL_INVALID_VALUE);
    }
    else if (count < 0)
    {
        RecordError(GL_INVALID_VALUE);
    }
    else
    {
        draw_mode = mode;
        draw_array.first = first;
        draw_array.count = count;

        if (draw_mode == GL_POINTS)
        {
            printf("GL_POINTS unsupported!\n");
            return;
        }
        else if (draw_mode == GL_LINES)
        {
            printf("GL_LINES unsupported!\n");
            return;
        }
        else if (draw_mode == GL_LINE_LOOP)
        {
            printf("GL_LINE_LOOP unsupported!\n");
            return;
        }
        else if (draw_mode == GL_LINE_STRIP)
        {
            printf("GL_LINE_STRIP unsupported!\n");
            return;
        }
    }

    /*  Init GL state table  */
    draw_init_table();

    /*  Perform DrawArrays command  */
    drawarrays_fixed_pipeline();

    /*  Update the Tiled linked-list  */
    refresh_display_list();
}

void Context::Enable(GLenum cap)
{
    Toggle(cap, true);

    switch (cap)
    {
    case GL_FOG:
    case GL_TEXTURE_2D:
    case GL_ALPHA_TEST:
    case GL_BLEND:
    case GL_COLOR_LOGIC_OP:
    case GL_DITHER:
    case GL_STENCIL_TEST:
    case GL_DEPTH_TEST:
    case GL_SCISSOR_TEST:
    case GL_MULTISAMPLE:
        stateLinkList = addState(cap, static_cast<unsigned int>(true), stateLinkList);
        state_counter++;
        break;
    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }
}

void Context::EnableClientState(GLenum array)
{
    switch (array)
    {
    case GL_VERTEX_ARRAY:
        vertex_array.enable = GL_TRUE;
        break;

    case GL_NORMAL_ARRAY:
        normal_array.enable = GL_TRUE;
        break;

    case GL_COLOR_ARRAY:
        color_array.enable = GL_TRUE;
        break;

    case GL_TEXTURE_COORD_ARRAY:
        texture_coord_array.enable = GL_TRUE;
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

void Context::Finish(void)
{

}

void Context::Flush(void)
{
    /*  Maintain State Changes  */
    refresh_display_list(true);
    /*  Render All Tiles  */
    tile_render();
}

/******************* fixed point ***************************/
void Context::Fogx(GLenum pname, GLfixed param)
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
            Fixed d(param);
            fog.density = d;
            stateLinkList = addState(GL_FOG_DENSITY, static_cast<unsigned int>(param), stateLinkList);
            state_counter++;
        }
        break;

    case GL_FOG_START:
    {
        Fixed s(param);
        fog.start = s;
        stateLinkList = addState(GL_FOG_START, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_END:
    {
        Fixed e(param);
        fog.end = e;
        stateLinkList = addState(GL_FOG_END, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;
    }
    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

/******************* fixed point ***************************/
void Context::Fogxv(GLenum pname, const GLfixed *params)
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
        GLfixed param;
        param = static_cast<GLfixed>(*params);
        if (param < 0x0000)
            RecordError(GL_INVALID_VALUE);
        else
        {
            Fixed d(param);
            fog.density = d;
            stateLinkList = addState(GL_FOG_DENSITY, static_cast<unsigned int>(param), stateLinkList);
            state_counter++;
        }
        break;
    }
    case GL_FOG_START:
    {
        GLfixed param;
        param = static_cast<GLfixed>(*params);
        Fixed s(param);
        fog.start = s;
        stateLinkList = addState(GL_FOG_START, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_END:
    {
        GLfixed param;
        param = static_cast<GLfixed>(*params);
        Fixed e(param);
        fog.end = e;
        stateLinkList = addState(GL_FOG_END, static_cast<unsigned int>(param), stateLinkList);
        state_counter++;
        break;
    }
    case GL_FOG_COLOR:
    {
        Fixed r(__GL_X_2_CLAMPX(params[0]));
        Fixed g(__GL_X_2_CLAMPX(params[1]));
        Fixed b(__GL_X_2_CLAMPX(params[2]));
        Fixed a(__GL_X_2_CLAMPX(params[3]));

        fog.cf.red   = r;
        fog.cf.green = g;
        fog.cf.blue  = b;
        fog.cf.alpha = a;

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

void Context::FrontFace(GLenum mode)
{
    front_face = mode;
}

/******************* fixed point ***************************/
void Context::Frustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    if (left == right || top == bottom || zNear <= 0x0000 || zFar <= 0x0000)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    Fixed le(left);
    Fixed ri(right);
    Fixed bo(bottom);
    Fixed to(top);
    Fixed zn(zNear);
    Fixed zf(zFar);

    GLfloat l = le;
    GLfloat r = ri;
    GLfloat b = bo;
    GLfloat t = to;
    GLfloat n = zn;
    GLfloat f = zf;

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
}

void Context::GenTextures(GLsizei n, GLuint *textures)
{
    if (n < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    TextureState *texobj = new TextureState[n];

    int i=0;
    while (i < n)
    {
        texvec.push_back(texobj[i]);
        *textures++ = texture_total_seq;
        ++texture_total_seq;
        ++i;
    }

    delete []texobj;
}

/******************* fixed point ***************************/
void Context::LightModelx(GLenum pname, GLfixed param)
{
    switch (pname)
    {
    case GL_LIGHT_MODEL_TWO_SIDE:
        light_model_two_side = (param != 0x0000);
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;
    }
}

/******************* fixed point ***************************/
void Context::LightModelxv(GLenum pname, const GLfixed *params)
{
    switch (pname)
    {
    case GL_LIGHT_MODEL_AMBIENT:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        light_model_ambient.red   = r;
        light_model_ambient.green = g;
        light_model_ambient.blue  = b;
        light_model_ambient.alpha = a;
        break;
    }
    default:
        LightModelf(pname, *params);
        break;

    }
}

/******************* fixed point ***************************/
void Context::Lightx(GLenum light, GLenum pname, GLfixed param)
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

        if (param < 0x0000 || param > 0x00800000)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            Fixed spot_exp(param);
            s_light[lightNO].source_spot_exponent = spot_exp;
        }
        break;

    case GL_SPOT_CUTOFF:
        if (param < 0x0000 || param > 0x005A0000 && param != 0x00B40000)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            Fixed cut_off(param);
            s_light[lightNO].source_spot_curoff = cut_off;
        }
        break;

    case GL_CONSTANT_ATTENUATION:
    {
        Fixed c_att(param);
        s_light[lightNO].source_constant_attenuation = c_att;
        break;
    }
    case GL_LINEAR_ATTENUATION:
    {
        Fixed l_att(param);
        s_light[lightNO].source_linear_attenuation = l_att;
        break;
    }
    case GL_QUADRATIC_ATTENUATION:
    {
        Fixed q_att(param);
        s_light[lightNO].source_quadratic_attenuation = q_att;
        break;
    }
    default:
        RecordError(GL_INVALID_ENUM);
        break;

    }
}

/******************* fixed point ***************************/
void Context::Lightxv(GLenum light, GLenum pname, const GLfixed *params)
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
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        s_light[lightNO].source_ambient.red   = r;
        s_light[lightNO].source_ambient.green = g;
        s_light[lightNO].source_ambient.blue  = b;
        s_light[lightNO].source_ambient.alpha = a;
        break;
    }
    case GL_DIFFUSE:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        s_light[lightNO].source_diffuse.red   = r;
        s_light[lightNO].source_diffuse.green = g;
        s_light[lightNO].source_diffuse.blue  = b;
        s_light[lightNO].source_diffuse.alpha = a;
        break;
    }
    case GL_SPECULAR:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        s_light[lightNO].source_specular.red   = r;
        s_light[lightNO].source_specular.green = g;
        s_light[lightNO].source_specular.blue  = b;
        s_light[lightNO].source_specular.alpha = a;
        break;
    }
    case GL_POSITION:
    {
        Fixed px(params[0]);
        Fixed py(params[1]);
        Fixed pz(params[2]);
        Fixed pw(params[3]);

        Vertex4 pos;
        pos.x = px;
        pos.y = py;
        pos.z = pz;
        pos.w = pw;

        MatrixTransformVec4(m_modelview_stack.CurrentMatrix(), pos);

        s_light[lightNO].source_position.x = pos.x;
        s_light[lightNO].source_position.y = pos.y;
        s_light[lightNO].source_position.z = pos.z;
        s_light[lightNO].source_position.w = pos.w;
        break;
    }
    case GL_SPOT_DIRECTION:
    {
        Fixed x(params[0]);
        Fixed y(params[1]);
        Fixed z(params[2]);

        Vector3 vec3;

        vec3.v_x = x;
        vec3.v_y = y;
        vec3.v_z = z;

        MatrixTransformVec3(m_modelview_stack.CurrentMatrix(), vec3);

        s_light[lightNO].source_spot_direction.v_x = vec3.v_x;
        s_light[lightNO].source_spot_direction.v_y = vec3.v_y;
        s_light[lightNO].source_spot_direction.v_z = vec3.v_z;

        break;
    }
    default:
        Lightx(light, pname, *params);
        break;
    }

}

void Context::LoadIdentity(void)
{
    CurrentMatrixStack()->LoadIdentity();
    RebuildMatrices();
}

/******************* fixed point ***************************/
void Context::LoadMatrixx(const GLfixed *m)
{
    Matrix4x4 mtx;
    Fixed *array = new Fixed[16];

    for (int i=0;i<16;++i)
    {
        array[i] = Fixed(m[i]);
        mtx.element[i] = array[i];
    }

    CurrentMatrixStack()->LoadMatrix(mtx);
    RebuildMatrices();

    delete [] array;
}

void Context::LogicOp(GLenum opcode)
{
    logic_op.op = opcode;

    stateLinkList = addState(GL_LOGIC_OP_MODE, static_cast<unsigned int>(opcode), stateLinkList);
    state_counter++;
}

/******************* fixed point ***************************/
void Context::Materialx(GLenum face, GLenum pname, GLfixed param)
{
    if (face != GL_FRONT_AND_BACK)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    switch (pname)
    {
    case GL_SHININESS:
        if (param < 0x0000 || param > 0x00800000)
        {
            RecordError(GL_INVALID_VALUE);
        }
        else
        {
            Fixed shine(param);
            m_light.material_shininess = shine;
        }

        break;

    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }
}

/******************* fixed point ***************************/
void Context::Materialxv(GLenum face, GLenum pname, const GLfixed *params)
{
    if (face != GL_FRONT_AND_BACK)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    switch (pname)
    {
    case GL_AMBIENT:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        m_light.material_ambient.red   = r;
        m_light.material_ambient.green = g;
        m_light.material_ambient.blue  = b;
        m_light.material_ambient.alpha = a;
        break;
    }
    case GL_DIFFUSE:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        m_light.material_diffuse.red   = r;
        m_light.material_diffuse.green = g;
        m_light.material_diffuse.blue  = b;
        m_light.material_diffuse.alpha = a;
        break;
    }
    case GL_AMBIENT_AND_DIFFUSE:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        m_light.material_ambient.red   = r;
        m_light.material_ambient.green = g;
        m_light.material_ambient.blue  = b;
        m_light.material_ambient.alpha = a;

        m_light.material_diffuse.red   = r;
        m_light.material_diffuse.green = g;
        m_light.material_diffuse.blue  = b;
        m_light.material_diffuse.alpha = a;
        break;
    }
    case GL_SPECULAR:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        m_light.material_specular.red   = r;
        m_light.material_specular.green = g;
        m_light.material_specular.blue  = b;
        m_light.material_specular.alpha = a;
        break;
    }
    case GL_EMISSION:
    {
        Fixed r(params[0]);
        Fixed g(params[1]);
        Fixed b(params[2]);
        Fixed a(params[3]);

        m_light.material_emission.red   = r;
        m_light.material_emission.green = g;
        m_light.material_emission.blue  = b;
        m_light.material_emission.alpha = a;
        break;
    }
    default:
        Materialf(face, pname, *params);
        break;
    }
}

void Context::MatrixMode(GLenum mode)
{
    switch (mode)
    {
    case GL_MODELVIEW:
        m_current_stack = &m_modelview_stack;
        break;

    case GL_PROJECTION:
        m_current_stack = &m_projection_stack;
        break;

    case GL_TEXTURE:
        m_current_stack = &m_texture_stack[active_texture];
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        return;
    }

    m_matrix_mode = mode;
}

/******************* fixed point ***************************/
void Context::MultMatrixx(const GLfixed *m)
{
    Matrix4x4 mtx;
    Fixed *array = new Fixed[16];

    for (int i=0;i<16;++i)
    {
        array[i] = Fixed(m[i]);
        mtx.element[i] = array[i];
    }

    CurrentMatrixStack()->MultMatrix(mtx);
    RebuildMatrices();

    delete [] array;
}

/******************* fixed point ***************************/
void Context::Normal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    Fixed n_x(nx);
    Fixed n_y(ny);
    Fixed n_z(nz);

    current_normal.nx = n_x;
    current_normal.ny = n_y;
    current_normal.nz = n_z;
}

void Context::NormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
    switch(type)
    {
        case GL_FIXED:
        case GL_SHORT:
        case GL_BYTE:
        case GL_FLOAT:
            break;

        default:
            RecordError(GL_INVALID_ENUM);
            return;
    }

    if (stride < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }
    normal_array.type = type;
    normal_array.stride = stride;
    normal_array.ptr = pointer;
}

/******************* fixed point ***************************/
void Context::Orthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    Fixed le(left);
    Fixed ri(right);
    Fixed bo(bottom);
    Fixed to(top);
    Fixed zn(zNear);
    Fixed zf(zFar);

    GLfloat l = le;
    GLfloat r = ri;
    GLfloat b = bo;
    GLfloat t = to;
    GLfloat n = zn;
    GLfloat f = zf;

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

}

/******************* fixed point ***************************/
void Context::PolygonOffsetx(GLfixed factor, GLfixed units)
{
    Fixed ft(factor);
    Fixed unit(units);

    depth_offset.factor = ft;
    depth_offset.units = unit;
}

void Context::PopMatrix(void)
{
    CurrentMatrixStack()->PopMatrix();
    RebuildMatrices();
}

void Context::PushMatrix(void)
{
    CurrentMatrixStack()->PushMatrix();
}

/******************* fixed point ***************************/
void Context::Rotatex(GLfixed fx_angle, GLfixed fx_x, GLfixed fx_y, GLfixed fx_z)
{
    Fixed fa(fx_angle);
    Fixed fx(fx_x);
    Fixed fy(fx_y);
    Fixed fz(fx_z);

    GLfloat angle = fa;
    GLfloat x = fx;
    GLfloat y = fy;
    GLfloat z = fz;

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

}

/******************* fixed point ***************************/
void Context::Scalex(GLfixed fx_x, GLfixed fx_y, GLfixed fx_z)
{
    Fixed fx(fx_x);
    Fixed fy(fx_y);
    Fixed fz(fx_z);

    GLfloat x = fx;
    GLfloat y = fy;
    GLfloat z = fz;

    Matrix4x4 matrix;

    matrix.element[ELM4(0, 0)] = x;
    matrix.element[ELM4(1, 1)] = y;
    matrix.element[ELM4(2, 2)] = z;

    CurrentMatrixStack()->MultMatrix(matrix);
    RebuildMatrices();
}

void Context::Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (width < 0 || height < 0)
    {
		RecordError(GL_INVALID_VALUE);
		return;
    }

    scissor_test.left = x;
    scissor_test.bottom = y;
    scissor_test.width = width;
    scissor_test.height = height;

    //stateLinkList = addState(GL_LOGIC_OP_MODE, static_cast<unsigned int>(true), stateLinkList);
    state_counter+= 4;
}

void Context::ShadeModel(GLenum mode)
{
    shade_model = mode;
}

void Context::StencilFunc(GLenum func, GLint ref, GLuint mask)
{
    stencil_test.func = func;
    stencil_test.ref_val = ref;
    stencil_test.mask = mask;

    //stateLinkList = addState(GL_LOGIC_OP_MODE, static_cast<unsigned int>(true), stateLinkList);
    state_counter+= 3;
}

void Context::StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
    stencil_test.sfail = fail;
    stencil_test.dfail = zfail;
    stencil_test.dpass = zpass;

    //stateLinkList = addState(GL_LOGIC_OP_MODE, static_cast<unsigned int>(true), stateLinkList);
    state_counter+= 3;
}

void Context::TexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if (size < 2 || size > 5)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    switch(type)
    {
        case GL_FIXED:
        case GL_SHORT:
        case GL_BYTE:
        case GL_FLOAT:
            break;

        default:
            RecordError(GL_INVALID_ENUM);
            return;
    }

    if (stride < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    texture_coord_array.size = size;
    texture_coord_array.type = type;
    texture_coord_array.stride = stride;
    texture_coord_array.ptr = pointer;
}

/******************* fixed point ***************************/
void Context::TexEnvx(GLenum target, GLenum pname, GLfixed param)
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

    //ate_counter++;
}

/******************* fixed point ***************************/
void Context::TexEnvxv(GLenum target, GLenum pname, const GLfixed *params)
{
    switch (target)
    {
    case GL_TEXTURE_ENV:

        switch (pname)
        {
        case GL_TEXTURE_ENV_COLOR:
        {
            Fixed r(__GL_X_2_CLAMPX(params[0]));
            Fixed g(__GL_X_2_CLAMPX(params[1]));
            Fixed b(__GL_X_2_CLAMPX(params[2]));
            Fixed a(__GL_X_2_CLAMPX(params[3]));

            float red   = r;
            float green = g;
            float blue  = b;
            float alpha = a;

            unsigned int cf = __GL_FLOAT_2_S(red)<<24|__GL_FLOAT_2_S(green)<<16|__GL_FLOAT_2_S(blue)<<8|__GL_FLOAT_2_S(alpha);

            texture_state[0].env_color = cf;
            texvec[texture_state[0].texture_bindID].env_color = cf;
            break;
        }
        default:
            TexEnvx(target, pname, *params);
            break;

        }
        break;

    default:
        RecordError(GL_INVALID_ENUM);
        break;

    }
}

/******************* fixed point ***************************/
void Context::TexParameterx(GLenum target, GLenum pname, GLfixed param)
{
    if (target != GL_TEXTURE_2D)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    TexParameteri(target, pname, (GLint)param);
}

/******************* fixed point ***************************/
void Context::Translatex(GLfixed fx_x, GLfixed fx_y, GLfixed fx_z)
{
    Fixed fx(fx_x);
    Fixed fy(fx_y);
    Fixed fz(fx_z);

    GLfloat x = fx;
    GLfloat y = fy;
    GLfloat z = fz;

    Matrix4x4 matrix;

    matrix.element[ELM4(0, 3)] = x;
    matrix.element[ELM4(1, 3)] = y;
    matrix.element[ELM4(2, 3)] = z;

    CurrentMatrixStack()->MultMatrix(matrix);
    RebuildMatrices();
}

void Context::VertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    if (size < 2 || size > 5)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    switch(type)
    {
        case GL_FIXED:
        case GL_SHORT:
        case GL_BYTE:
        case GL_FLOAT:
            break;

        default:
            RecordError(GL_INVALID_ENUM);
            return;
    }

    if (stride < 0)
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    vertex_array.size = size;
    vertex_array.type = type;
    vertex_array.stride = stride;
    vertex_array.ptr = pointer;
}

void Context::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (width < 0 || height < 0)
    {
		RecordError(GL_INVALID_VALUE);
		return;
    }

    vp.Ox = x + width/2;
    vp.Oy = y + height/2;
    vp.width = width;
    vp.height = height;
}

void Context::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    if (target != GL_TEXTURE_2D)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    if (level < 0 || level >= EGL_MAX_LEVELS)
    {
        RecordError(GL_INVALID_ENUM);
        return;
    }

    if (border != 0 || width < 0 || height < 0 || !IsPowerOf2(width) || !IsPowerOf2(height))
    {
        RecordError(GL_INVALID_VALUE);
        return;
    }

    GLint externalFormat = (GLint)format;

    // TODO : check type between internalformat and externalformat
    if (internalformat != externalFormat)
    {
        RecordError(GL_INVALID_OPERATION);
        return;
    }

    // TODO : image data should be copied into buffer of mipmap arrays
    TextureImage *temp = new TextureImage();
    temp->border = border;
    temp->internal_format = internalformat;
    temp->format = format;
    temp->level = level;
    temp->type = type;
    temp->width = width;
    temp->height = height;

    int biSizeImage = 0;

    switch (format)
    {
        case GL_RGB:
            biSizeImage = width*height*3;
            break;

        case GL_RGBA:
            biSizeImage = width*height*4;
            break;

        case GL_ALPHA:
        case GL_LUMINANCE:
            biSizeImage = width*height*1;
            break;
        case GL_LUMINANCE_ALPHA:
            break;

        default:
            biSizeImage = width*height*3;
            break;
    }

    unsigned char * image = new unsigned char[biSizeImage];
    unsigned char * origin_ptr = (unsigned char*)pixels;

    for (int i=0;i<biSizeImage;i++)
    {
        image[i] = *(origin_ptr+i);
    }

    temp->data = image;

    texvec[texture_state[0].texture_bindID].teximage = temp;

    // TODO : update mipmap arrays

}

/*****************************************************************************************/
/*                                 non-implemented functions                             */
/*****************************************************************************************/
void Context::BindBuffer(GLenum target, GLuint buffer)
{

}

void Context::BufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{

}

void Context::BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{

}

void Context::ClipPlanex(GLenum plane, const GLfixed *equation)
{

}

void Context::ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{

}

void Context::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{

}

void Context::CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{

}

void Context::CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{

}

void Context::CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{

}

void Context::DeleteBuffers(GLsizei n, const GLuint *buffers)
{

}

void Context::DepthMask(GLboolean flag)
{

}

void Context::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{

    if (count < 0)
    {
        RecordError(GL_INVALID_VALUE);
    }
    else if (type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_BYTE)
    {
        RecordError(GL_INVALID_ENUM);
    }
    else
    {
        draw_mode = mode;
        draw_element.count = count;
        draw_element.type = type;
        draw_element.indices = indices;
    }

    /*  Init GL state table  */
    draw_init_table();

    /*  Perform DrawElements command  */
    drawelements_fixed_pipeline();

    /*  Update the Tiled linked-list  */
    refresh_display_list();
}

void Context::GetBooleanv(GLenum pname, GLboolean *params)
{

}

void Context::GetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{

}

void Context::GetClipPlanex(GLenum pname, GLfixed eqn[4])
{

}

void Context::GenBuffers(GLsizei n, GLuint *buffers)
{

}

GLenum Context::GetError(void)
{

}

bool Context::Context::GetFixedv(GLenum pname, GLfixed *params)
{

}

void Context::GetIntegerv(GLenum pname, GLint *params)
{

}

bool Context::GetLightxv(GLenum light, GLenum pname, GLfixed *params)
{

}

bool Context::GetMaterialxv(GLenum face, GLenum pname, GLfixed *params)
{

}

void Context::GetPointerv(GLenum pname, void **params)
{

}

void Context::GetTexEnviv(GLenum env, GLenum pname, GLint *params)
{

}

bool Context::GetTexEnvxv(GLenum env, GLenum pname, GLfixed *params)
{

}

void Context::GetTexParameteriv(GLenum target, GLenum pname, GLint *params)
{

}

void Context::GetTexParameterxv(GLenum target, GLenum pname, GLfixed *params)
{

}

const GLubyte* Context::GetString(GLenum name)
{

}

void Context::Hint(GLenum target, GLenum mode)
{

}

GLboolean Context::IsBuffer(GLuint buffer)
{

}

GLboolean Context::IsEnabled(GLenum cap)
{

}

GLboolean Context::IsTexture(GLuint texture)
{

}

void Context::LineWidthx(GLfixed width)
{

}

void Context::MultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{

}

void Context::PixelStorei(GLenum pname, GLint param)
{

}

void Context::PointParameterx(GLenum pname, GLfixed param)
{

}

void Context::PointParameterxv(GLenum pname, const GLfixed *params)
{

}

void Context::PointSizex(GLfixed size)
{

}

void Context::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{

}

void Context::SampleCoveragex(GLclampx value, GLboolean invert)
{

}

void Context::StencilMask(GLuint mask)
{

}

void Context::TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{

}
