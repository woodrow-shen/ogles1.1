// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : gl.cpp
// Purpose : Client API entry points
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 08-04-2003	Hans-Martin Will	initial version
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================


#include "context.h"
#include "ogles.h"

using namespace EGL;

#define CONTEXT_EXEC(func)                                  \
	Context * context = Context::GetCurrentContext();		\
	if (context)                                            \
	{														\
		context->func;										\
	}


#define CONTEXT_EXEC_RETURN(func)                           \
	Context * context = Context::GetCurrentContext();		\
	if (context)                                            \
	{														\
		return context->func;								\
	}                                                       \
	else                                                    \
	{														\
		return 0;											\
	}



GLAPI void APIENTRY glActiveTexture (GLenum texture)
{
    fprintf(APIdumper, "glActiveTexture(%#X); \n", texture);
    glfunc.ActiveTexture++;
    glfunc.Total++;
    CONTEXT_EXEC(ActiveTexture(texture));
}

GLAPI void APIENTRY glAlphaFunc (GLenum func, GLclampf ref)
{
    fprintf(APIdumper, "glAlphaFunc(%#X, %f); \n", func, ref);
    glfunc.AlphaFunc++;
    glfunc.Total++;
    CONTEXT_EXEC(AlphaFunc(func, ref));
}

GLAPI void APIENTRY glAlphaFuncx (GLenum func, GLclampx ref)
{
    fprintf(APIdumper, "glAlphaFuncx(%#X, %f); \n", func, ref);
    glfunc.AlphaFunc++;
    glfunc.Total++;
    CONTEXT_EXEC(AlphaFuncx(func, ref));
}

GLAPI void APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
    CONTEXT_EXEC(BindBuffer(target, buffer));
}

GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture)
{
    fprintf(APIdumper, "glBindTexture(%#X, %u); \n", target, texture);
    glfunc.BindTexture++;
    glfunc.Total++;
    CONTEXT_EXEC(BindTexture(target, texture));
}

GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor)
{
    fprintf(APIdumper, "glBlendFunc(%#X, %#X); \n", sfactor, dfactor);
    glfunc.BlendFunc++;
    glfunc.Total++;
    CONTEXT_EXEC(BlendFunc(sfactor, dfactor));
}

GLAPI void APIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{

    CONTEXT_EXEC(BufferData(target, size, data, usage));
}

GLAPI void APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    CONTEXT_EXEC(BufferSubData(target, offset, size, data));
}

GLAPI void APIENTRY glClear (GLbitfield mask)
{
    fprintf(APIdumper, "glClear(%#X); \n", mask);
    glfunc.Clear++;
    glfunc.Total++;
    CONTEXT_EXEC(Clear(mask));
}

GLAPI void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    fprintf(APIdumper, "glClearColor(%f, %f, %f, %f); \n", red, green, blue, alpha);
    glfunc.ClearColor++;
    glfunc.Total++;
    CONTEXT_EXEC(ClearColor(red, green, blue, alpha));
}

GLAPI void APIENTRY glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    fprintf(APIdumper, "glClearColorx(%08X, %08X, %08X, %08X); \n", red, green, blue, alpha);
    glfunc.ClearColor++;
    glfunc.Total++;
    CONTEXT_EXEC(ClearColorx(red, green, blue, alpha));
}

GLAPI void APIENTRY glClearDepthf (GLclampf depth)
{
    fprintf(APIdumper, "glClearDepthf(%f); \n", depth);
    glfunc.ClearDepth++;
    glfunc.Total++;
    CONTEXT_EXEC(ClearDepthf(depth));
}

GLAPI void APIENTRY glClearDepthx (GLclampx depth)
{
    fprintf(APIdumper, "glClearDepthx(%f); \n", depth);
    glfunc.ClearDepth++;
    glfunc.Total++;
    CONTEXT_EXEC(ClearDepthx(depth));
}

GLAPI void APIENTRY glClearStencil (GLint s)
{
    fprintf(APIdumper, "glClearStencil(%d); \n", s);
    glfunc.ClearStencil++;
    glfunc.Total++;
    CONTEXT_EXEC(ClearStencil(s));
}

GLAPI void APIENTRY glClipPlanex(GLenum plane, const GLfixed *equation)
{
    CONTEXT_EXEC(ClipPlanex(plane, equation));
}

GLAPI void APIENTRY glClipPlanef(GLenum plane, const GLfloat *equation)
{
    CONTEXT_EXEC(ClipPlanef(plane, equation));
}

GLAPI void APIENTRY glClientActiveTexture (GLenum texture)
{
    fprintf(APIdumper, "glClientActiveTexture(%#X); \n", texture);
    CONTEXT_EXEC(ClientActiveTexture(texture));
}

GLAPI void APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    fprintf(APIdumper, "glColor4f(%f, %f, %f, %f); \n", red, green, blue, alpha);
    glfunc.Color4++;
    glfunc.Total++;
    CONTEXT_EXEC(Color4f(red, green, blue, alpha));
}

GLAPI void APIENTRY glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    fprintf(APIdumper, "glColor4x(%08X, %08X, %08X, %08X); \n", red, green, blue, alpha);
    glfunc.Color4++;
    glfunc.Total++;
    CONTEXT_EXEC(Color4x(red, green, blue, alpha));
}

GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    CONTEXT_EXEC(ColorMask(red, green , blue, alpha));
}

GLAPI void APIENTRY glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    fprintf(APIdumper, "glColorPointer(%d, %#X, %d, %#X); \n", size, type, stride, pointer);
    glfunc.ColorPointer++;
    glfunc.Total++;
    CONTEXT_EXEC(ColorPointer(size, type, stride, pointer));
}

GLAPI void APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{

    CONTEXT_EXEC(CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data));
}

GLAPI void APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    CONTEXT_EXEC(CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data));
}

GLAPI void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    CONTEXT_EXEC(CopyTexImage2D(target, level, internalformat, x, y, width, height, border));
}

GLAPI void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    CONTEXT_EXEC(CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height));
}

GLAPI void APIENTRY glCullFace (GLenum mode)
{
    fprintf(APIdumper, "glCullFace(%#X); \n", mode);
    glfunc.CullFace++;
    glfunc.Total++;
    CONTEXT_EXEC(CullFace(mode));
}

GLAPI void APIENTRY glDeleteBuffers(GLsizei n, const GLuint *buffers)
{
    CONTEXT_EXEC(DeleteBuffers(n, buffers));
}

GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures)
{
    fprintf(APIdumper, "glDeleteTextures(%d, %#X); \n", n, textures);
    glfunc.DeleteTextures++;
    glfunc.Total++;
    CONTEXT_EXEC(DeleteTextures(n, textures));
}

GLAPI void APIENTRY glDepthFunc (GLenum func)
{
    fprintf(APIdumper, "glDepthFunc(%#X); \n", func);
    glfunc.DepthFunc++;
    glfunc.Total++;
    CONTEXT_EXEC(DepthFunc(func));
}

GLAPI void APIENTRY glDepthMask (GLboolean flag)
{
    CONTEXT_EXEC(DepthMask(flag));
}

GLAPI void APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar)
{
    fprintf(APIdumper, "glDepthRangef(%f, %f); \n", zNear, zFar);
    glfunc.DepthRange++;
    glfunc.Total++;
    CONTEXT_EXEC(DepthRangef(zNear, zFar));
}

GLAPI void APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar)
{
    fprintf(APIdumper, "glDepthRangex(%d, %d); \n", zNear, zFar);
    glfunc.DepthRange++;
    glfunc.Total++;
    CONTEXT_EXEC(DepthRangex(zNear, zFar));
}

GLAPI void APIENTRY glDisable (GLenum cap)
{
    fprintf(APIdumper, "glDisable(%#X); \n", cap);
    glfunc.Disable++;
    glfunc.Total++;
    CONTEXT_EXEC(Disable(cap));
}

GLAPI void APIENTRY glDisableClientState (GLenum array)
{
    fprintf(APIdumper, "glDisableClientState(%#X); \n", array);
    glfunc.DisableClientState++;
    glfunc.Total++;
    CONTEXT_EXEC(DisableClientState(array));
}

GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    fprintf(APIdumper, "glDrawArrays(%#X, %d, %d); \n", mode, first, count);
    glfunc.DrawArrays++;
    glfunc.Total++;
    CONTEXT_EXEC(DrawArrays(mode, first, count));
}

GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
    fprintf(APIdumper, "glDrawElements(%#X, %d, %#X, %#X); \n", mode, count, type, indices);
    glfunc.DrawElements++;
    glfunc.Total++;
    CONTEXT_EXEC(DrawElements(mode, count, type, indices));
}

GLAPI void APIENTRY glEnable (GLenum cap)
{
    fprintf(APIdumper, "glEnable(%#X); \n", cap);
    glfunc.Enable++;
    glfunc.Total++;
    CONTEXT_EXEC(Enable(cap));
}

GLAPI void APIENTRY glEnableClientState (GLenum array)
{
    fprintf(APIdumper, "glEnableClientState(%#X); \n", array);
    glfunc.EnableClientState++;
    glfunc.Total++;
    CONTEXT_EXEC(EnableClientState(array));
}

GLAPI void APIENTRY glFinish (void)
{
    fprintf(APIdumper, "glFinish(); \n");
    glfunc.Finish++;
    glfunc.Total++;
    CONTEXT_EXEC(Finish());
}

GLAPI void APIENTRY glFlush (void)
{
    fprintf(APIdumper, "glFlush(); \n");
    glfunc.Flush++;
    glfunc.Total++;
    CONTEXT_EXEC(Flush());
}

GLAPI void APIENTRY glFogf (GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glFogf(%#X, %f); \n", pname, param);
    glfunc.Fog++;
    glfunc.Total++;
    CONTEXT_EXEC(Fogf(pname, param));
}

GLAPI void APIENTRY glFogfv (GLenum pname, const GLfloat *params)
{
    fprintf(APIdumper, "glFogfv(%#X, %f, %f, %f, %f); \n", pname, params[0], params[1], params[2], params[3]);
    glfunc.Fogv++;
    glfunc.Total++;
    CONTEXT_EXEC(Fogfv(pname, params));
}

GLAPI void APIENTRY glFogx (GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glFogx(%#X, %d); \n", pname, param);
    glfunc.Fog++;
    glfunc.Total++;
    CONTEXT_EXEC(Fogx(pname, param));
}

GLAPI void APIENTRY glFogxv (GLenum pname, const GLfixed *params)
{
    fprintf(APIdumper, "glFogxv(%#X, %08X, %08X, %08X, %08X); \n", pname, params[0], params[1], params[2], params[3]);
    glfunc.Fogv++;
    glfunc.Total++;
    CONTEXT_EXEC(Fogxv(pname, params));
}

GLAPI void APIENTRY glFrontFace (GLenum mode)
{
    fprintf(APIdumper, "glFrontFace(%#X); \n", mode);
    glfunc.FrontFace++;
    glfunc.Total++;
    CONTEXT_EXEC(FrontFace(mode));
}

GLAPI void APIENTRY glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    fprintf(APIdumper, "glFrustumf(%f, %f, %f, %f, %f, %f); \n", left, right, bottom, top, zNear, zFar);
    glfunc.Frustum++;
    glfunc.Total++;
    CONTEXT_EXEC(Frustumf(left, right, bottom, top, zNear, zFar));
}

GLAPI void APIENTRY glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    fprintf(APIdumper, "glFrustumx(%08X, %08X, %08X, %08X, %08X, %08X); \n", left, right, bottom, top, zNear, zFar);
    glfunc.Frustum++;
    glfunc.Total++;
    CONTEXT_EXEC(Frustumx(left, right, bottom, top, zNear, zFar));
}

GLAPI void APIENTRY glGetBooleanv(GLenum pname, GLboolean *params)
{
    CONTEXT_EXEC(GetBooleanv(pname, params));
}

GLAPI void APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
    CONTEXT_EXEC(GetBufferParameteriv(target, pname, params));
}

GLAPI void APIENTRY glGetClipPlanef(GLenum pname, GLfloat eqn[4])
{
    CONTEXT_EXEC(GetClipPlanef(pname, eqn));
}

GLAPI void APIENTRY glGetClipPlanex(GLenum pname, GLfixed eqn[4])
{
    CONTEXT_EXEC(GetClipPlanex(pname, eqn));
}

GLAPI void APIENTRY glGenBuffers(GLsizei n, GLuint *buffers)
{
    CONTEXT_EXEC(GenBuffers(n, buffers));
}

GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures)
{
    fprintf(APIdumper, "glGenTextures(%d, %#X); \n", n, textures);
    glfunc.GenTextures++;
    glfunc.Total++;
    CONTEXT_EXEC(GenTextures(n, textures));
}

GLAPI GLenum APIENTRY glGetError (void)
{
    CONTEXT_EXEC_RETURN(GetError());
}

GLAPI void APIENTRY glGetFixedv(GLenum pname, GLfixed *params)
{
    CONTEXT_EXEC(GetFixedv(pname, params));
}

GLAPI void APIENTRY glGetFloatv(GLenum pname, GLfloat *params)
{
    CONTEXT_EXEC(GetFloatv(pname, params));
}

GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *params)
{
    CONTEXT_EXEC(GetIntegerv(pname, params));
}

GLAPI void APIENTRY glGetLightfv(GLenum light, GLenum pname, GLfloat *params)
{
    CONTEXT_EXEC(GetLightfv(light, pname, params));
}

GLAPI void APIENTRY glGetLightxv(GLenum light, GLenum pname, GLfixed *params)
{
    CONTEXT_EXEC(GetLightxv(light, pname, params));
}

GLAPI void APIENTRY glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{
    CONTEXT_EXEC(GetMaterialfv(face, pname, params));
}

GLAPI void APIENTRY glGetMaterialxv(GLenum face, GLenum pname, GLfixed *params)
{
    CONTEXT_EXEC(GetMaterialxv(face, pname, params));
}

GLAPI void APIENTRY glGetPointerv(GLenum pname, void **params)
{
    CONTEXT_EXEC(GetPointerv(pname, params));
}

GLAPI const GLubyte * APIENTRY glGetString (GLenum name)
{
    CONTEXT_EXEC_RETURN(GetString(name));
}

GLAPI void APIENTRY glGetTexEnviv(GLenum env, GLenum pname, GLint *params)
{
    CONTEXT_EXEC(GetTexEnviv(env, pname, params));
}

GLAPI void APIENTRY glGetTexEnvfv(GLenum env, GLenum pname, GLfloat *params)
{
    CONTEXT_EXEC(GetTexEnvfv(env, pname, params));
}

GLAPI void APIENTRY glGetTexEnvxv(GLenum env, GLenum pname, GLfixed *params)
{
    CONTEXT_EXEC(GetTexEnvxv(env, pname, params));
}

GLAPI void APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint *params)
{
    CONTEXT_EXEC(GetTexParameteriv(target, pname, params));
}

GLAPI void APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
    CONTEXT_EXEC(GetTexParameterfv(target, pname, params));
}

GLAPI void APIENTRY glGetTexParameterxv(GLenum target, GLenum pname, GLfixed *params)
{
    CONTEXT_EXEC(GetTexParameterxv(target, pname, params));
}

GLAPI void APIENTRY glHint (GLenum target, GLenum mode)
{
    CONTEXT_EXEC(Hint(target, mode));
}

GLAPI GLboolean APIENTRY glIsBuffer(GLuint buffer)
{
    CONTEXT_EXEC_RETURN(IsBuffer(buffer));
}

GLAPI GLboolean APIENTRY glIsEnabled(GLenum cap)
{
    CONTEXT_EXEC_RETURN(IsEnabled(cap));
}

GLAPI GLboolean APIENTRY glIsTexture(GLuint texture)
{
    CONTEXT_EXEC_RETURN(IsTexture(texture));
}

GLAPI void APIENTRY glLightModelf (GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glLightModelf(%#X, %f); \n", pname, param);
    glfunc.LightModel++;
    glfunc.Total++;
    CONTEXT_EXEC(LightModelf(pname, param));
}

GLAPI void APIENTRY glLightModelfv (GLenum pname, const GLfloat *params)
{
    fprintf(APIdumper, "glLightModelfv(%#X, %f, %f, %f, %f); \n", pname, params[0], params[1], params[2], params[3]);
    glfunc.LightModelv++;
    glfunc.Total++;
    CONTEXT_EXEC(LightModelfv(pname, params));
}

GLAPI void APIENTRY glLightModelx (GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glLightModelx(%#X, %d); \n", pname, param);
    glfunc.LightModel++;
    glfunc.Total++;
    CONTEXT_EXEC(LightModelx(pname, param));
}

GLAPI void APIENTRY glLightModelxv (GLenum pname, const GLfixed *params)
{
    fprintf(APIdumper, "glLightModelxv(%#X, %08X, %08X, %08X, %08X); \n", pname, params[0], params[1], params[2], params[3]);
    glfunc.LightModelv++;
    glfunc.Total++;
    CONTEXT_EXEC(LightModelxv(pname, params));
}

GLAPI void APIENTRY glLightf (GLenum light, GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glLightf(%#X, %#X, %f); \n", light, pname, param);
    glfunc.Light++;
    glfunc.Total++;
    CONTEXT_EXEC(Lightf(light, pname, param));
}

GLAPI void APIENTRY glLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
    fprintf(APIdumper, "glLightfv(%#X, %#X, %#X); \n", light, pname, params);
    glfunc.Lightv++;
    glfunc.Total++;
    CONTEXT_EXEC(Lightfv(light, pname, params));
}

GLAPI void APIENTRY glLightx (GLenum light, GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glLightx(%#X, %#X, %d); \n", light, pname, param);
    glfunc.Light++;
    glfunc.Total++;
    CONTEXT_EXEC(Lightx(light, pname, param));
}

GLAPI void APIENTRY glLightxv (GLenum light, GLenum pname, const GLfixed *params)
{
    fprintf(APIdumper, "glLightxv(%#X, %#X, %#X); \n", light, pname, params);
    glfunc.Lightv++;
    glfunc.Total++;
    CONTEXT_EXEC(Lightxv(light, pname, params));
}

GLAPI void APIENTRY glLineWidth (GLfloat width)
{
    CONTEXT_EXEC(LineWidth(width));
}

GLAPI void APIENTRY glLineWidthx (GLfixed width)
{
    CONTEXT_EXEC(LineWidthx(width));
}

GLAPI void APIENTRY glLoadIdentity (void)
{
    fprintf(APIdumper, "glLoadIdentity(); \n");
    glfunc.LoadIdentity++;
    glfunc.Total++;
    CONTEXT_EXEC(LoadIdentity());
}

GLAPI void APIENTRY glLoadMatrixf (const GLfloat *m)
{
    fprintf(APIdumper, "glLoadMatrixf(%#X); \n", m);
    glfunc.LoadMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(LoadMatrixf(m));
}

GLAPI void APIENTRY glLoadMatrixx (const GLfixed *m)
{
    fprintf(APIdumper, "glLoadMatrixx(%#X); \n", m);
    glfunc.LoadMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(LoadMatrixx(m));
}

GLAPI void APIENTRY glLogicOp (GLenum opcode)
{
    fprintf(APIdumper, "glLogicOp(%#X); \n", opcode);
    glfunc.LogicOp++;
    glfunc.Total++;
    CONTEXT_EXEC(LogicOp(opcode));
}

GLAPI void APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glMaterialf(%#X, %#X, %f); \n", face, pname, param);
    glfunc.Material++;
    glfunc.Total++;
    CONTEXT_EXEC(Materialf(face, pname, param));
}

GLAPI void APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
    fprintf(APIdumper, "glMaterialfv(%#X, %#X, %f, %f, %f, %f); \n", face, pname, params[0], params[1], params[2], params[3]);
    glfunc.Materialv++;
    glfunc.Total++;
    CONTEXT_EXEC(Materialfv(face, pname, params));
}

GLAPI void APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glMaterialx((%#X, %#X, %d); \n", face, pname, param);
    glfunc.Material++;
    glfunc.Total++;
    CONTEXT_EXEC(Materialx(face, pname, param));
}

GLAPI void APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
{
    fprintf(APIdumper, "glMaterialxv(%#X, %#X, %08X, %08X, %08X, %08X); \n", face, pname, params[0], params[1], params[2], params[3]);
    glfunc.Materialv++;
    glfunc.Total++;
    CONTEXT_EXEC(Materialxv(face, pname, params));
}

GLAPI void APIENTRY glMatrixMode (GLenum mode)
{
    fprintf(APIdumper, "glMatrixMode(%#X); \n", mode);
    glfunc.MatrixMode++;
    glfunc.Total++;
    CONTEXT_EXEC(MatrixMode(mode));
}

GLAPI void APIENTRY glMultMatrixf (const GLfloat *m)
{
    fprintf(APIdumper, "glMultMatrixf(%#X); \n", m);
    glfunc.MultMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(MultMatrixf(m));
}

GLAPI void APIENTRY glMultMatrixx (const GLfixed *m)
{
    fprintf(APIdumper, "glMultMatrixx(%#X); \n", m);
    glfunc.MultMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(MultMatrixx(m));
}

GLAPI void APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    CONTEXT_EXEC(MultiTexCoord4f(target, s, t, r, q));
}

GLAPI void APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    CONTEXT_EXEC(MultiTexCoord4x(target, s, t, r, q));
}

GLAPI void APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    fprintf(APIdumper, "glNormal3f(%f, %f, %f); \n", nx, ny, nz);
    glfunc.Normal3++;
    glfunc.Total++;
    CONTEXT_EXEC(Normal3f(nx, ny, nz));
}

GLAPI void APIENTRY glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
{
    fprintf(APIdumper, "glNormal3x(%08X, %08X, %08X); \n", nx, ny, nz);
    glfunc.Normal3++;
    glfunc.Total++;
    CONTEXT_EXEC(Normal3x(nx, ny, nz));
}

GLAPI void APIENTRY glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
    fprintf(APIdumper, "glNormalPointer(%#X, %d, %#X); \n", type, stride, pointer);
    glfunc.NormalPointer++;
    glfunc.Total++;
    CONTEXT_EXEC(NormalPointer(type, stride, pointer));
}

GLAPI void APIENTRY glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    fprintf(APIdumper, "glOrthof(%f, %f, %f, %f, %f, %f); \n", left, right, bottom, top, zNear, zFar);
    glfunc.Ortho++;
    glfunc.Total++;
    CONTEXT_EXEC(Orthof(left, right, bottom, top, zNear, zFar));
}

GLAPI void APIENTRY glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    fprintf(APIdumper, "glOrthox(%08X, %08X, %08X, %08X, %08X, %08X); \n", left, right, bottom, top, zNear, zFar);
    glfunc.Ortho++;
    glfunc.Total++;
    CONTEXT_EXEC(Orthox(left, right, bottom, top, zNear, zFar));
}

GLAPI void APIENTRY glPixelStorei (GLenum pname, GLint param)
{
    CONTEXT_EXEC(PixelStorei(pname, param));
}

GLAPI void APIENTRY glPointParameterf(GLenum pname, GLfloat param)
{
    CONTEXT_EXEC(PointParameterf(pname, param));
}

GLAPI void APIENTRY glPointParameterfv(GLenum pname, const GLfloat *params)
{
    CONTEXT_EXEC(PointParameterfv(pname, params));
}

GLAPI void APIENTRY glPointParameterx(GLenum pname, GLfixed param)
{
    CONTEXT_EXEC(PointParameterx(pname, param));
}

GLAPI void APIENTRY glPointParameterxv(GLenum pname, const GLfixed *params)
{
    CONTEXT_EXEC(PointParameterxv(pname, params));
}

GLAPI void APIENTRY glPointSize (GLfloat size)
{
    CONTEXT_EXEC(PointSize(size));
}

GLAPI void APIENTRY glPointSizex (GLfixed size)
{
    CONTEXT_EXEC(PointSizex(size));
}

GLAPI void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units)
{
    fprintf(APIdumper, "glPolygonOffset(%f, %f); \n", factor, units);
    glfunc.PolygonOffset++;
    glfunc.Total++;
    CONTEXT_EXEC(PolygonOffset(factor, units));
}

GLAPI void APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units)
{
    fprintf(APIdumper, "glPolygonOffsetx(%08X, %08X); \n", factor, units);
    glfunc.PolygonOffset++;
    glfunc.Total++;
    CONTEXT_EXEC(PolygonOffsetx(factor, units));
}

GLAPI void APIENTRY glPopMatrix (void)
{
    fprintf(APIdumper, "glPopMatrix(); \n");
    glfunc.PopMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(PopMatrix());
}

GLAPI void APIENTRY glPushMatrix (void)
{
    fprintf(APIdumper, "glPushMatrix(); \n");
    glfunc.PushMatrix++;
    glfunc.Total++;
    CONTEXT_EXEC(PushMatrix());
}

GLAPI void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
    CONTEXT_EXEC(ReadPixels(x, y, width, height, format, type, pixels));
}

GLAPI void APIENTRY glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    fprintf(APIdumper, "glRotatef(%f, %f, %f, %f); \n", angle, x, y, z);
    glfunc.Rotate++;
    glfunc.Total++;
    CONTEXT_EXEC(Rotatef(angle, x, y, z));
}

GLAPI void APIENTRY glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    fprintf(APIdumper, "glRotatex(%08X, %08X, %08X, %08X); \n", angle, x, y, z);
    glfunc.Rotate++;
    glfunc.Total++;
    CONTEXT_EXEC(Rotatex(angle, x, y, z));
}

GLAPI void APIENTRY glSampleCoverage (GLclampf value, GLboolean invert)
{
    CONTEXT_EXEC(SampleCoverage(value, invert));
}

GLAPI void APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert)
{
    CONTEXT_EXEC(SampleCoveragex(value, invert));
}

GLAPI void APIENTRY glScalef (GLfloat x, GLfloat y, GLfloat z)
{
    fprintf(APIdumper, "glScalef(%f, %f, %f); \n", x, y, z);
    glfunc.Scale++;
    glfunc.Total++;
    CONTEXT_EXEC(Scalef(x, y, z));
}

GLAPI void APIENTRY glScalex (GLfixed x, GLfixed y, GLfixed z)
{
    fprintf(APIdumper, "glScalex(%08X, %08X, %08X); \n", x, y, z);
    glfunc.Scale++;
    glfunc.Total++;
    CONTEXT_EXEC(Scalex(x, y, z));
}

GLAPI void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    fprintf(APIdumper, "glScissor(%d, %d, %d, %d); \n", x, y, width, height);
    glfunc.Scissor++;
    glfunc.Total++;
    CONTEXT_EXEC(Scissor(x, y, width, height));
}

GLAPI void APIENTRY glShadeModel (GLenum mode)
{
    fprintf(APIdumper, "glShadeModel(%#X); \n", mode);
    glfunc.ShadeModel++;
    glfunc.Total++;
    CONTEXT_EXEC(ShadeModel(mode));
}

GLAPI void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    fprintf(APIdumper, "glStencilFunc(%#X, %d, %u); \n", func, ref, mask);
    glfunc.StencilFunc++;
    glfunc.Total++;
    CONTEXT_EXEC(StencilFunc(func, ref, mask));
}

GLAPI void APIENTRY glStencilMask (GLuint mask)
{
    CONTEXT_EXEC(StencilMask(mask));
}

GLAPI void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    fprintf(APIdumper, "glStencilOp(%#X, %#X, %#X); \n", fail, zfail, zpass);
    glfunc.StencilOp++;
    glfunc.Total++;
    CONTEXT_EXEC(StencilOp(fail, zfail, zpass));
}

GLAPI void APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    fprintf(APIdumper, "glTexCoordPointer(%d, %#X, %d, %#X); \n", size, type, stride, pointer);
    glfunc.TexCoordPointer++;
    glfunc.Total++;
    CONTEXT_EXEC(TexCoordPointer(size, type, stride, pointer));
}

GLAPI void APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glTexEnvf(%#X, %#X, %f); \n", target, pname, param);
    glfunc.TexEnv++;
    glfunc.Total++;
    CONTEXT_EXEC(TexEnvf(target, pname, param));
}

GLAPI void APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
    fprintf(APIdumper, "glTexEnvfv(%#X, %#X, %#X); \n", target, pname, params);
    glfunc.TexEnvv++;
    glfunc.Total++;
    CONTEXT_EXEC(TexEnvfv(target, pname, params));
}

GLAPI void APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glTexEnvx(%#X, %#X, %X); \n", target, pname, param);
    glfunc.TexEnv++;
    glfunc.Total++;
    CONTEXT_EXEC(TexEnvx(target, pname, param));
}

GLAPI void APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
{
    fprintf(APIdumper, "glTexEnvxv(%#X, %#X, %#X); \n", target, pname, params);
    glfunc.TexEnvv++;
    glfunc.Total++;
    CONTEXT_EXEC(TexEnvxv(target, pname, params));
}

GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    fprintf(APIdumper, "glTexParameteri(%#X, %d, %d, %d, %d, %d, %#X, %#X, %#X); \n", target, level, internalformat, width, height, border, format, type, pixels);
    glfunc.TexImage2D++;
    glfunc.Total++;
    CONTEXT_EXEC(TexImage2D(target, level, internalformat, width, height, border, format, type, pixels));
}

GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    fprintf(APIdumper, "glTexParameteri(%#X, %#X, %d); \n", target, pname, param);
    glfunc.TexParameter++;
    glfunc.Total++;
    CONTEXT_EXEC(TexParameteri(target, pname, param));
}

GLAPI void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    fprintf(APIdumper, "glTexParameterf(%#X, %#X, %X); \n", target, pname, param);
    glfunc.TexParameter++;
    glfunc.Total++;
    CONTEXT_EXEC(TexParameterf(target, pname, param));
}

GLAPI void APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
    fprintf(APIdumper, "glTexParameterx(%#X, %#X, %08X); \n", target, pname, param);
    glfunc.TexParameter++;
    glfunc.Total++;
    CONTEXT_EXEC(TexParameterx(target, pname, param));
}

GLAPI void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    CONTEXT_EXEC(TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels));
}

GLAPI void APIENTRY glTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    fprintf(APIdumper, "glTranslatef(%f, %f, %f); \n", x, y, z);
    glfunc.Translate++;
    glfunc.Total++;
    CONTEXT_EXEC(Translatef(x, y, z));
}

GLAPI void APIENTRY glTranslatex (GLfixed x, GLfixed y, GLfixed z)
{
    fprintf(APIdumper, "glTranslatex(%08X, %08X, %08X); \n", x, y, z);
    glfunc.Translate++;
    glfunc.Total++;
    CONTEXT_EXEC(Translatex(x, y, z));
}

GLAPI void APIENTRY glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    fprintf(APIdumper, "glVertexPointer(%d, %#X, %d, %#X); \n", size, type, stride, pointer);
    glfunc.VertexPointer++;
    glfunc.Total++;
    CONTEXT_EXEC(VertexPointer(size, type, stride, pointer));
}

GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    fprintf(APIdumper, "glViewport(%d, %d, %d, %d); \n", x, y, width, height);
    glfunc.Viewport++;
    glfunc.Total++;
    CONTEXT_EXEC(Viewport(x, y, width, height));
}

/*****************************************************************************************/
/*                                 OES extension functions                               */
/*****************************************************************************************/

/*
GLAPI GLbitfield APIENTRY glQueryMatrixxOES(GLfixed mantissa[16], GLint exponent[16])
{
    CONTEXT_EXEC_RETURN(QueryMatrixx(mantissa, exponent));
}
*/


/* OES_point_size_array
GLAPI void APIENTRY glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid *pointer)
{
    CONTEXT_EXEC(PointSizePointer(type, stride, pointer));
}
*/
