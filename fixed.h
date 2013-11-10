#ifndef FIXED_H_INCLUDED
#define FIXED_H_INCLUDED

#include "ogles.h"

#define Q24D8_INT_BITS	    24
#define Q24D8_FRAC_BITS	    8
#define Q24D8_ZERO		    0x00000000		/* S24.8 0.0 */
#define Q24D8_ONE		    0x00000100		/* S24.8 1.0 */
#define Q24D8_TWO		    0x00000200		/* S24.8 2.0 */
#define Q24D8_HALF		    0x00000080		/* S24.8 0.5 */

#define Q28D4_INT_BITS	    28
#define Q28D4_FRAC_BITS	    4
#define Q28D4_ZERO		    0x00000000		/* S28.4 0.0 */
#define Q28D4_ONE		    0x00000010		/* S28.4 1.0 */
#define Q28D4_TWO		    0x00000020		/* S28.4 2.0 */
#define Q28D4_HALF		    0x00000008		/* S28.4 0.5 */

#define _X_PINF		        0x7fffffff	    /* +inf */
#define _X_MINF		        0x80000000	    /* -inf */

#define _X_ADD(a,b) 	    ((a)+(b))
#define _X_SUB(a,b) 	    ((a)-(b))

#define Q24D8_MUL(a,b)		((GLfixed)((((long long)(a))*(b))>>Q24D8_FRAC_BITS))
#define Q24D8_DIV(a,b) 	    ((GLfixed)((((long long)(a))<<Q24D8_FRAC_BITS)/(b)))
#define Q24D8_DIVI(a,b) 	((GLfixed)((a)/(b)))
#define Q24D8_INT(a)		((a) >= 0 ? (a)>>Q24D8_FRAC_BITS : -(-(a)>>Q24D8_FRAC_BITS))
#define Q24D8_INTP(a)		((a)>>Q24D8_FRAC_BITS) //  a/= 2^16
#define Q24D8_FLOOR(a)		((a)&~((1<<Q24D8_FRAC_BITS)-1))
#define Q24D8_FRAC(a)		((a)-Q24D8_FLOOR(a))
#define Q24D8_TRUNC(a) 	    _INT_2_X(Q24D8_INT(a))
#define Q24D8_CEIL(a) 		Q24D8_FLOOR((a)+((1<<Q24D8_FRAC_BITS)-1))
#define Q24D8_RND(a)		Q24D8_INT((a)+Q24D8_HALF)
#define Q24D8_ICEIL(a)		Q24D8_INT((a)+((1<<Q24D8_FRAC_BITS)-1))
#define Q24D8_2_FLOAT(a)	((GLfloat)(a)/(1<<Q24D8_FRAC_BITS))
#define Q24D8_2_CLAMPX(a)	((a) < Q24D8_ZERO ? Q24D8_ZERO : (a) > Q24D8_ONE ? Q24D8_ONE : (a))
#define _INT_2_24D8(a)		((GLfixed)((a)<<Q24D8_FRAC_BITS))

#define Q28D4_MUL(a,b)		((GLfixed)((((long long)(a))*(b))>>Q28D4_FRAC_BITS))
#define Q28D4_DIV(a,b) 	    ((GLfixed)((((long long)(a))<<Q28D4_FRAC_BITS)/(b)))
#define Q28D4_DIVI(a,b) 	((GLfixed)((a)/(b)))
#define Q28D4_INT(a)		((a) >= 0 ? (a)>>Q28D4_FRAC_BITS : -(-(a)>>Q28D4_FRAC_BITS))
#define Q28D4_INTP(a)		((a)>>Q28D4_FRAC_BITS) //  a/= 2^16
#define Q28D4_FLOOR(a)		((a)&~((1<<Q28D4_FRAC_BITS)-1))
#define Q28D4_FRAC(a)		((a)-Q28D4_FLOOR(a))
#define Q28D4_TRUNC(a) 	    _INT_2_X(Q28D4_INT(a))
#define Q28D4_CEIL(a) 		Q28D4_FLOOR((a)+((1<<Q28D4_FRAC_BITS)-1))
#define Q28D4_RND(a)		Q28D4_INT((a)+Q28D4_HALF)
#define Q28D4_ICEIL(a)		Q28D4_INT((a)+((1<<Q28D4_FRAC_BITS)-1))
#define Q28D4_2_FLOAT(a)	((GLfloat)(a)/(1<<Q28D4_FRAC_BITS))
#define Q28D4_2_CLAMPX(a)	((a) < Q28D4_ZERO ? Q28D4_ZERO : (a) > Q28D4_ONE ? Q28D4_ONE : (a))
#define _INT_2_28D4(a)		((GLfixed)((a)<<Q28D4_FRAC_BITS))

namespace EGL
{
    /********************** Q15.16 **********************/
    class Fixed
    {
    public:
        inline Fixed()
        {
            m_value = 0;
        }

        inline Fixed(GLfixed value)
        {
            m_value = value;
        }

        inline Fixed& operator=(const Fixed& other)
        {
            m_value = other.m_value;
            return *this;
        }

        inline Fixed operator+(const Fixed& other) const
        {
            return Fixed(__GL_X_ADD(m_value, other.m_value));
        }

        inline Fixed& operator+=(const Fixed& other)
        {
            m_value = __GL_X_ADD(m_value, other.m_value);
            return *this;
        }

        inline Fixed operator-(const Fixed& other) const
        {
            return Fixed(__GL_X_SUB(m_value, other.m_value));
        }

        inline Fixed& operator-=(const Fixed& other)
        {
            m_value = __GL_X_SUB(m_value, other.m_value);
            return *this;
        }

        inline Fixed operator*(const Fixed& other) const
        {
            return Fixed(__GL_X_MUL(m_value, other.m_value));
        }

        inline Fixed operator*(const GLfixed& other) const
        {
            return m_value*other;
        }

        inline I64 Mul(const Fixed& other) const
        {
            return ((I64) m_value * (I64) other.m_value);
        }

        inline Fixed& operator*=(const Fixed& other)
        {
            m_value = __GL_X_MUL(m_value, other.m_value);
            return *this;
        }

        inline Fixed operator/(const Fixed& other) const
        {
            return Fixed(__GL_X_DIV(m_value, other.m_value));
        }

        inline Fixed operator%(const Fixed& other) const
        {
            return Fixed((m_value%other.m_value)<<__GL_X_FRAC_BITS);
        }

        inline Fixed& operator/=(const Fixed& other)
        {
            m_value = __GL_X_DIV(m_value, other.m_value);
            return *this;
        }

        inline bool operator==(const Fixed& other) const
        {
            return m_value == other.m_value;
        }

        inline Fixed operator>>(const int other) const
        {
            return m_value >> other;
        }

        inline Fixed operator<<(const int other) const
        {
            return m_value << other;
        }

        inline bool operator>(const Fixed& other) const
        {
            return m_value > other.m_value;
        }

        inline bool operator>=(const Fixed& other) const
        {
            return m_value >= other.m_value;
        }

        inline bool operator>=(const GLfixed& other) const
        {
            return m_value >= other;
        }

        inline bool operator>(const GLfixed& other) const
        {
            return m_value > other;
        }

        inline bool operator<(const Fixed& other) const
        {
            return m_value < other.m_value;
        }

        inline bool operator<=(const Fixed& other) const
        {
            return m_value <= other.m_value;
        }

        inline bool operator<(const GLfixed& other) const
        {
            return m_value < other;
        }

        inline bool operator!=(const Fixed& other) const
        {
            return m_value != other.m_value;
        }

        inline Fixed operator&(const Fixed& other) const
        {
            return Fixed(m_value & other.m_value);
        }

        inline Fixed operator|(const Fixed& other) const
        {
            return Fixed(m_value | other.m_value);
        }

        inline Fixed operator~()
        {
            return Fixed(~m_value);
        }

        /* get fraction parts from fixed point */
        inline GLfixed FractionFromFixed()
        {
            return Fixed(m_value & ((1 << __GL_X_FRAC_BITS) - 1));
        }

        inline I32 Round()
        {
            return Fixed((m_value + __GL_X_ONE/2) >> __GL_X_FRAC_BITS);
        }

        /* get integer by round (四捨五入) */
        inline GLfixed NearestInt()
        {
            return Fixed((m_value + (__GL_X_ONE/2 - 1)) & 0xffff0000);
        }

        /* round-off of fraction parts (捨棄小數) */
        inline int ToInt()
        {
            return m_value>>__GL_X_FRAC_BITS;
        }

        operator float()
        {
            return static_cast<float>(static_cast<double>(m_value) * (1.0f/static_cast<float>(__GL_X_ONE)));
        }

        operator GLfixed()
        {
            return m_value;
        }


    private:
        GLfixed m_value;
    };

    /********************** Q24.8 **********************/
    class Fixed24D8
    {
    public:
        inline Fixed24D8()
        {
            m_value = 0;
        }

        inline Fixed24D8(GLfixed value)
        {
            m_value = value;
        }

        inline Fixed24D8& operator=(const Fixed24D8& other)
        {
            m_value = other.m_value;
            return *this;
        }

        inline Fixed24D8 operator+(const Fixed24D8& other) const
        {
            return Fixed24D8(_X_ADD(m_value, other.m_value));
        }

        inline Fixed24D8& operator+=(const Fixed24D8& other)
        {
            m_value = _X_ADD(m_value, other.m_value);
            return *this;
        }

        inline Fixed24D8 operator-(const Fixed24D8& other) const
        {
            return Fixed24D8(_X_SUB(m_value, other.m_value));
        }

        inline Fixed24D8& operator-=(const Fixed24D8& other)
        {
            m_value = _X_SUB(m_value, other.m_value);
            return *this;
        }

        inline Fixed24D8 operator*(const Fixed24D8& other) const
        {
            return Fixed24D8(Q24D8_MUL(m_value, other.m_value));
        }

        inline Fixed24D8 operator*(const GLfixed& other) const
        {
            return m_value*other;
        }

        inline I64 Mul(const Fixed24D8& other) const
        {
            return ((I64) m_value * (I64) other.m_value);
        }

        inline Fixed24D8& operator*=(const Fixed24D8& other)
        {
            m_value = Q24D8_MUL(m_value, other.m_value);
            return *this;
        }

        inline Fixed24D8 operator/(const Fixed24D8& other) const
        {
            return Fixed24D8(Q24D8_DIV(m_value, other.m_value));
        }

        inline Fixed24D8 operator%(const Fixed24D8& other) const
        {
            return Fixed24D8((m_value%other.m_value)<<Q24D8_FRAC_BITS);
        }

        inline Fixed24D8& operator/=(const Fixed24D8& other)
        {
            m_value = Q24D8_DIV(m_value, other.m_value);
            return *this;
        }

        inline bool operator==(const Fixed24D8& other) const
        {
            return m_value == other.m_value;
        }

        inline Fixed24D8 operator>>(const int other) const
        {
            return m_value >> other;
        }

        inline Fixed24D8 operator<<(const int other) const
        {
            return m_value << other;
        }

        inline bool operator>(const Fixed24D8& other) const
        {
            return m_value > other.m_value;
        }

        inline bool operator>=(const Fixed24D8& other) const
        {
            return m_value >= other.m_value;
        }

        inline bool operator>=(const GLfixed& other) const
        {
            return m_value >= other;
        }

        inline bool operator>(const GLfixed& other) const
        {
            return m_value > other;
        }

        inline bool operator<(const Fixed24D8& other) const
        {
            return m_value < other.m_value;
        }

        inline bool operator<=(const Fixed24D8& other) const
        {
            return m_value <= other.m_value;
        }

        inline bool operator<(const GLfixed& other) const
        {
            return m_value < other;
        }

        inline bool operator!=(const Fixed24D8& other) const
        {
            return m_value != other.m_value;
        }

        inline Fixed24D8 operator&(const Fixed24D8& other) const
        {
            return Fixed24D8(m_value & other.m_value);
        }

        inline Fixed24D8 operator|(const Fixed24D8& other) const
        {
            return Fixed24D8(m_value | other.m_value);
        }

        inline Fixed24D8 operator~()
        {
            return Fixed24D8(~m_value);
        }

        /* get fraction parts from fixed point */
        inline GLfixed FractionFromFixed24D8()
        {
            return Fixed24D8(m_value & ((1 << Q24D8_FRAC_BITS) - 1));
        }

        /* get integer by round (四捨五入) */
        inline I32 Round()
        {
            return Fixed24D8((m_value + Q24D8_ONE/2) >> Q24D8_FRAC_BITS);
        }

        /* round-off of fraction parts (捨棄小數) */
        inline GLfixed NearestInt()
        {
            return Fixed24D8((m_value + (Q24D8_ONE/2 - 1)) & 0xffffff00);
        }

        inline int ToInt()
        {
            return m_value>>Q24D8_FRAC_BITS;
        }

        operator float()
        {
            return static_cast<float>(static_cast<double>(m_value) * (1.0f/static_cast<float>(Q24D8_ONE)));
        }

        operator GLfixed()
        {
            return m_value;
        }


    private:
        GLfixed m_value;
    };

    /********************** Q28.4 **********************/
    class Fixed28D4
    {
    public:
        inline Fixed28D4()
        {
            m_value = 0;
        }

        inline Fixed28D4(GLfixed value)
        {
            m_value = value;
        }

        inline Fixed28D4& operator=(const Fixed28D4& other)
        {
            m_value = other.m_value;
            return *this;
        }

        inline Fixed28D4 operator+(const Fixed28D4& other) const
        {
            return Fixed28D4(_X_ADD(m_value, other.m_value));
        }

        inline Fixed28D4& operator+=(const Fixed28D4& other)
        {
            m_value = _X_ADD(m_value, other.m_value);
            return *this;
        }

        inline Fixed28D4 operator-(const Fixed28D4& other) const
        {
            return Fixed28D4(_X_SUB(m_value, other.m_value));
        }

        inline Fixed28D4& operator-=(const Fixed28D4& other)
        {
            m_value = _X_SUB(m_value, other.m_value);
            return *this;
        }

        inline Fixed28D4 operator*(const Fixed28D4& other) const
        {
            return Fixed28D4(Q28D4_MUL(m_value, other.m_value));
        }

        inline Fixed28D4 operator*(const GLfixed& other) const
        {
            return m_value*other;
        }

        inline I64 Mul(const Fixed28D4& other) const
        {
            return ((I64) m_value * (I64) other.m_value);
        }

        inline Fixed28D4& operator*=(const Fixed28D4& other)
        {
            m_value = Q28D4_MUL(m_value, other.m_value);
            return *this;
        }

        inline Fixed28D4 operator/(const Fixed28D4& other) const
        {
            return Fixed28D4(Q28D4_DIV(m_value, other.m_value));
        }

        inline Fixed28D4 operator%(const Fixed28D4& other) const
        {
            return Fixed28D4((m_value%other.m_value)<<Q28D4_FRAC_BITS);
        }

        inline Fixed28D4& operator/=(const Fixed28D4& other)
        {
            m_value = Q28D4_DIV(m_value, other.m_value);
            return *this;
        }

        inline bool operator==(const Fixed28D4& other) const
        {
            return m_value == other.m_value;
        }

        inline Fixed28D4 operator>>(const int other) const
        {
            return m_value >> other;
        }

        inline Fixed28D4 operator<<(const int other) const
        {
            return m_value << other;
        }

        inline bool operator>(const Fixed28D4& other) const
        {
            return m_value > other.m_value;
        }

        inline bool operator>=(const Fixed28D4& other) const
        {
            return m_value >= other.m_value;
        }

        inline bool operator>=(const GLfixed& other) const
        {
            return m_value >= other;
        }

        inline bool operator>(const GLfixed& other) const
        {
            return m_value > other;
        }

        inline bool operator<(const Fixed28D4& other) const
        {
            return m_value < other.m_value;
        }

        inline bool operator<=(const Fixed28D4& other) const
        {
            return m_value <= other.m_value;
        }

        inline bool operator<(const GLfixed& other) const
        {
            return m_value < other;
        }

        inline bool operator!=(const Fixed28D4& other) const
        {
            return m_value != other.m_value;
        }

        inline Fixed28D4 operator&(const Fixed28D4& other) const
        {
            return Fixed28D4(m_value & other.m_value);
        }

        inline Fixed28D4 operator|(const Fixed28D4& other) const
        {
            return Fixed28D4(m_value | other.m_value);
        }

        inline Fixed28D4 operator~()
        {
            return Fixed28D4(~m_value);
        }

        /* get fraction parts from fixed point */
        inline GLfixed FractionFromFixed28D4()
        {
            return Fixed28D4(m_value & ((1 << Q28D4_FRAC_BITS) - 1));
        }

        /* get integer by round (四捨五入) */
        inline I32 Round()
        {
            return Fixed28D4((m_value + Q28D4_ONE/2) >> Q28D4_FRAC_BITS);
        }

        /* round-off of fraction parts (捨棄小數) */
        inline GLfixed NearestInt()
        {
            return Fixed28D4((m_value + (Q28D4_ONE/2 - 1)) & 0xfffffff0);
        }

        inline int ToInt()
        {
            return m_value>>Q28D4_FRAC_BITS;
        }

        operator float()
        {
            return static_cast<float>(static_cast<double>(m_value) * (1.0f/static_cast<float>(Q28D4_ONE)));
        }

        operator GLfixed()
        {
            return m_value;
        }


    private:
        GLfixed m_value;
    };

}

#endif // FIXED_H_INCLUDED
