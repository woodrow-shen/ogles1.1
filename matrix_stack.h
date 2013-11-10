#ifndef MATRIX_STACK_H_INCLUDED
#define MATRIX_STACK_H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : matrix_stack.h
// Purpose : Matrix Stack Class
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

    class MatrixStack
    {

    public:

        MatrixStack(I32 maxStackElements = 2);
        ~MatrixStack();

        bool PopMatrix(void);
        bool PushMatrix(void);

        void MultMatrix(const Matrix4x4 &matrix);
        void MultMatrix(const GLfloat *m);

        void LoadIdentity(void);

        void LoadMatrix(const Matrix4x4 &matrix);
        void LoadMatrix(const GLfloat *m);


        inline Matrix4x4 & CurrentMatrix()
        {
            return m_Stack[m_StackPointer];
        }

        inline I32 GetStackSize() const
        {
            return m_StackSize;
        }

        inline I32 GetCurrentStackSize() const
        {
            return m_StackPointer + 1;
        }

    private:

        Matrix4x4	*m_Stack;
        I32			m_StackPointer;
        I32			m_StackSize;
    };
}

#endif // MATRIX_STACK_H_INCLUDED
