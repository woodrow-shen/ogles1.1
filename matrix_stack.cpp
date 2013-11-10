// ===================================================================
// CASLAB 3D Graphics Group
// Project Name:
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name :
// Purpose :
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 19-07-2008   Shye-Tzeng  Shen    modified version
//
// ===================================================================

#include "stdafx.h"
#include "matrix_stack.h"

using namespace EGL;


MatrixStack :: MatrixStack(I32 maxStackElements)
        :	m_StackSize(maxStackElements),
        m_StackPointer(0)
{
    m_Stack = new Matrix4x4[maxStackElements];
}


MatrixStack :: ~MatrixStack()
{
    delete[] m_Stack;
}


bool MatrixStack :: PopMatrix(void)
{
    if (m_StackPointer > 0)
    {
        --m_StackPointer;
        return true;
    }
    else
    {
        return false;
    }
}

bool MatrixStack :: PushMatrix(void)
{

    if (m_StackPointer < m_StackSize - 1)
    {
        m_Stack[m_StackPointer + 1] = m_Stack[m_StackPointer];
        ++m_StackPointer;
        return true;
    }
    else
    {
        return false;
    }
}


void MatrixStack :: MultMatrix(const Matrix4x4& matrix)
{
    Matrix4x4 tempM;
    int i,j,k;

    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            float sum = 0;
            for (k=0;k<4;k++)
            {
                //printf("(%d,%d):%f, (%d,%d):%f\n", i,k,CurrentMatrix().element[ELM4(i,k)], k,j,matrix.element[ELM4(k,j)]);
                sum += __GL_F_MUL(CurrentMatrix().element[ELM4(i,k)], matrix.element[ELM4(k,j)]);
            }
            tempM.element[ELM4(i,j)] = sum;
        }
    }
    LoadMatrix(tempM);
}

void MatrixStack :: MultMatrix(const GLfloat *m)
{
    Matrix4x4 tempM;
    int i,j,k;

    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            float sum = 0;
            for (k=0;k<4;k++)
            {
                sum += __GL_F_MUL(CurrentMatrix().element[ELM4(i,k)], m[ELM4(k,j)]);
            }
            tempM.element[ELM4(i,j)] = sum;
        }
    }
    LoadMatrix(tempM);
}

void MatrixStack :: LoadIdentity(void)
{
#ifdef __GL_FLOAT
    CurrentMatrix().element[ELM4(0,1)] = CurrentMatrix().element[ELM4(0,2)] = CurrentMatrix().element[ELM4(0,3)] = __GL_F_ZERO;
    CurrentMatrix().element[ELM4(1,0)] = CurrentMatrix().element[ELM4(1,2)] = CurrentMatrix().element[ELM4(1,3)] = __GL_F_ZERO;
    CurrentMatrix().element[ELM4(2,0)] = CurrentMatrix().element[ELM4(2,1)] = CurrentMatrix().element[ELM4(2,3)] = __GL_F_ZERO;
    CurrentMatrix().element[ELM4(3,0)] = CurrentMatrix().element[ELM4(3,1)] = CurrentMatrix().element[ELM4(3,2)] = __GL_F_ZERO;
    CurrentMatrix().element[ELM4(0,0)] = CurrentMatrix().element[ELM4(1,1)] = CurrentMatrix().element[ELM4(2,2)] = CurrentMatrix().element[ELM4(3,3)] = __GL_F_ONE;
#else

#endif

}


void MatrixStack :: LoadMatrix(const Matrix4x4& matrix)
{
    int i;
    for (i=0;i<16;i++)
    {
        CurrentMatrix().element[i] = matrix.element[i];
    }
}

void MatrixStack :: LoadMatrix(const GLfloat *m)
{
    int i;
    for (i=0;i<16;i++)
    {
        CurrentMatrix().element[i] = m[i];
    }
}


