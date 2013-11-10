#ifndef UGLU_H_INCLUDED
#define UGLU_H_INCLUDED

#include "ogles.h"

#define PI_ 3.14159265358979323846
#define __f2x(f)	((int)(f*65536))
#define __mulx(a,b)	((((__int64)(a))*(b))>>16)
#define __divx(a,b)	(((__int64)(a)<<16)/(b))

//static const unsigned short __gl_sin_tab[] = {
//#include "gl_sin.h"
//};

#ifdef __GL_FLOAT
//-----------------------------------------------------------------------------
//  UG Library from Vincent, actually it's like GLU
//-----------------------------------------------------------------------------
void ugSolidTorusf(GLfloat irr, GLfloat orr, GLint sides, GLint rings)
{
    GLint i, j, k, triangles;
    GLfloat s, t, x, y, z, twopi, nx, ny, nz;
	GLfloat sin_s, cos_s, cos_t, sin_t, twopi_s, twopi_t;
	GLfloat twopi_sides, twopi_rings;
    static GLfloat* v, *n;
    static GLfloat parms[4];
    GLfloat* p, *q;

    if (v)
	{
		if (parms[0] != irr || parms[1] != orr || parms[2] != sides || parms[3] != rings)
		{
			free(v);
			free(n);
			n = v = 0;

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glNormalPointer(GL_FLOAT, 0, 0);
		}
    }

    if (!v)
	{
		parms[0] = irr;
		parms[1] = orr;
		parms[2] = (GLfloat)sides;
		parms[3] = (GLfloat)rings;

		p = v = (GLfloat*)malloc(sides*(rings+1)*2*3*sizeof *v);
		q = n = (GLfloat*)malloc(sides*(rings+1)*2*3*sizeof *n);

        twopi = 2.0f * (GLfloat)PI_;
		twopi_sides = twopi/sides;
		twopi_rings = twopi/rings;

        for (i = 0; i < sides; i++)
		{
			for (j = 0; j <= rings; j++)
			{
				for (k = 1; k >= 0; k--)
				{
					s = (i + k) % sides + 0.5f;
					t = (GLfloat)( j % rings);

					twopi_s= s*twopi_sides;
					twopi_t = t*twopi_rings;

					cos_s = (GLfloat)cos(twopi_s);
					sin_s = (GLfloat)sin(twopi_s);

					cos_t = (GLfloat)cos(twopi_t);
					sin_t = (GLfloat)sin(twopi_t);

					x = (orr+irr*(GLfloat)cos_s)*(GLfloat)cos_t;
					y = (orr+irr*(GLfloat)cos_s)*(GLfloat)sin_t;
					z = irr * (GLfloat)sin_s;

					*p++ = x;
					*p++ = y;
					*p++ = z;

					nx = (GLfloat)cos_s*(GLfloat)cos_t;
					ny = (GLfloat)cos_s*(GLfloat)sin_t;
					nz = (GLfloat)sin_s;

					*q++ = nx;
					*q++ = ny;
					*q++ = nz;
				}
			}
		}
    }

    glVertexPointer(3, GL_FLOAT, 0, v);
    glNormalPointer(GL_FLOAT, 0, n);

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

	triangles = (rings + 1) * 2;

    for(i = 0; i < sides; i++)
		glDrawArrays(GL_TRIANGLE_STRIP, triangles * i, triangles);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

static GLfloat boxvec[6][3] =
{
   {-1.0, 0.0, 0.0},
   {0.0, 1.0, 0.0},
   {1.0, 0.0, 0.0},
   {0.0, -1.0, 0.0},
   {0.0, 0.0, 1.0},
   {0.0, 0.0, -1.0}
};

static GLushort boxndex [12][3] =
{
	{0, 1, 2},
	{0, 2, 3},
	{3, 2, 6},
	{3, 6, 7},
	{6, 4, 7},
	{6, 5, 4},
	{4, 5, 1},
	{4, 1, 0},
	{2, 1, 5},
	{2, 5, 6},
	{3, 7, 4},
	{3, 4, 0}
};

static GLushort wireboxndex[6][4] =
{
   {0, 1, 2, 3},
   {3, 2, 6, 7},
   {7, 6, 5, 4},
   {4, 5, 1, 0},
   {5, 6, 2, 1},
   {7, 4, 0, 3}
};

void ugSolidBox(GLfloat Width, GLfloat Depth, GLfloat Height)
{
	int i;
	GLfloat v[8][3];

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = - Width/ 2.0f;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = Width / 2.0f;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -Depth / 2.0f;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = Depth / 2.0f;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -Height / 2.0f;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = Height / 2.0f;

	glVertexPointer(3, GL_FLOAT, 0, v);
	glEnableClientState (GL_VERTEX_ARRAY);

	for (i = 0; i < 6; i++)
	{
		glNormal3f(boxvec[i][0], boxvec[i][1], boxvec[i][2]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, boxndex[i*2]);
	}

	glDisableClientState (GL_VERTEX_ARRAY);
}

void PlotTubePoints(GLfloat radius, GLfloat height, GLshort stacks, GLshort slices, GLfloat* v, GLfloat* n)
{

	float theta_step = 2.0f*((GLfloat)PI_)/((float)slices);
	float height_step = height/((float)stacks);

	float theta, h1, h2;
	float c_theta, s_theta;

	float* v_ptr = v;
	float* n_ptr = n;

	int i, j;

	for ( i = 0; i < stacks; i++)
	{

		h1 = 0.0f + ((float)i) * height_step;
		h2 = h1 + height_step;

		for (j = 0; j <= slices; j++)
		{
			if (j != slices)
				theta = 0.0f - (theta_step * j);
			else
				theta = 0.0;

			c_theta = (float)cos(theta);
			s_theta = (float)sin(theta);


			*v_ptr++ = c_theta * radius;
			*v_ptr++ = s_theta * radius;
			*v_ptr++ = h1;

			*n_ptr++ = c_theta;
			*n_ptr++ = s_theta;
			*n_ptr++ = 0.0f;

			*v_ptr++ = *(v_ptr - 3);
			*v_ptr++ = *(v_ptr - 3);
			*v_ptr++ = h2;

			*n_ptr++ = c_theta;
			*n_ptr++ = s_theta;
			*n_ptr++ = 0.0f;

		}
	}
}

void ugSolidTube(GLfloat radius, GLfloat height, GLshort stacks, GLshort slices)
{
    static GLfloat *v, *n;
    static GLfloat parms[4];
	int i, triangles;

	if ((radius <= 0.0)||(height <= 0.0) || (stacks < 1)||( slices < 3))
		return;

	if (v)
	{
		if ((parms[0] != radius) || (parms[1] != height) ||
									(parms[2] !=(float) stacks) || (parms[3] != (float)slices))
		{

			free(v);
			free(n);
			v = n = NULL;

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glNormalPointer(GL_FLOAT, 0, 0);
		}
	}

    if (!v)
	{
		parms[0] = radius;
		parms[1] = height;
		parms[2] = (GLfloat)stacks;
		parms[3] = (GLfloat)slices;

		v = (float*)malloc(stacks*(slices+1)*2*3*sizeof *v);
		n = (float*)malloc(stacks*(slices+1)*2*3*sizeof *n);


		if ((!v)||(!n))				// bail if either of the mallocs failed
		{
			if (v)
				free(v);
			if (n)
				free(n);

			return;
		}
		PlotTubePoints(radius, height, stacks, slices, v, n);
	}

	glVertexPointer(3, GL_FLOAT, 0, v);
	glNormalPointer(GL_FLOAT, 0, n);

	glEnableClientState (GL_NORMAL_ARRAY);
	glEnableClientState (GL_VERTEX_ARRAY);

	triangles = (slices + 1) * 2;

	for(i = 0; i < stacks; i++)
		glDrawArrays(GL_TRIANGLE_STRIP, i * triangles, triangles);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void PlotDiskPoints(GLfloat inner_radius, GLfloat outer_radius, GLshort rings, GLshort slices, GLfloat* v)
{

	float theta_step = 2.0f* ((float)PI_)/slices;

	float r_step = (outer_radius - inner_radius)/((float)rings);
	float theta, r1, r2;
	float c_theta, s_theta;

	int i, j ;

	float* ptr = v;

	for (i = 0; i < rings; i++)
	{
		r1 = inner_radius + (r_step * i);
		r2 = r1 + r_step;


		for (j = 0; j <= slices; j++)
		{
			if (j != slices)
				theta = 0.0f + (theta_step * j);
			else
				theta = 0.0f;

			c_theta = (float)cos(theta);
			s_theta = (float)sin(theta);


			*ptr++ = c_theta * r1;
			*ptr++ = s_theta * r1;
			*ptr++ = 0.0;

			*ptr++ = c_theta * r2;
			*ptr++ = s_theta * r2;
			*ptr++ = 0.0;

		}
	}
}

void ugSolidDisk(GLfloat inner_radius, GLfloat outer_radius, GLshort rings, GLshort slices)
{
    static GLfloat* v;
    static GLfloat parms[4];
	int i, triangles;

	if ((rings <= 0)&&(slices <= 2)&&(inner_radius < 0.0) && (outer_radius <= inner_radius)) // minimum needed to draw the object
		return;

	if (v)
	{
		if ((parms[0] != inner_radius) || (parms[1] != outer_radius) ||
										(parms[2] !=(float) rings) || (parms[3] != (float)slices))
		{

			free(v);
			v = NULL;

			glVertexPointer(3, GL_FLOAT, 0, 0);
		}
	}

    if (!v)
	{
		parms[0] = inner_radius;
		parms[1] = outer_radius;
		parms[2] = (GLfloat)rings;
		parms[3] = (GLfloat)slices;

		v = (float*)malloc(rings*(slices+1)*2*3*sizeof *v);

		if (!v)				// bail if the malloc failed
			return;

		PlotDiskPoints(inner_radius, outer_radius, rings, slices, v);
	}

	glNormal3f(0.0, 0.0, 1.0);

	glVertexPointer(3, GL_FLOAT, 0, v);
	glEnableClientState (GL_VERTEX_ARRAY);

	triangles = (slices + 1) * 2;

	for(i = 0; i < rings; i++)
		glDrawArrays(GL_TRIANGLE_STRIP, i * triangles, triangles);

	glDisableClientState (GL_VERTEX_ARRAY);

}

void ugSolidConef(GLfloat base, GLfloat height, GLint slices, GLint stacks)
{
    GLint i, j;
    GLfloat twopi, nx, ny, nz;
    static GLfloat* v, *n;
    static GLfloat parms[4];
    GLfloat* p, *q;

    if (v)
	{
		if (parms[0] != base || parms[1] != height || parms[2] != slices || parms[3] != stacks)
		{
			free(v);
			free(n);

			n = v = 0;

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glNormalPointer(GL_FLOAT, 0, 0);
		}
    }

    if ((!v) && (height != 0.0f))
	{
		float phi = (float)atan(base/height);
		float cphi = (GLfloat)cos(phi);
		float sphi= (GLfloat)sin(phi);

		parms[0] = base;
		parms[1] = height;
		parms[2] = (GLfloat)slices;
		parms[3] = (GLfloat)stacks;

		p = v = (float*)malloc(stacks*(slices+1)*2*3*sizeof *v);
		q = n = (float*)malloc(stacks*(slices+1)*2*3*sizeof *n);

        twopi = 2.0f * ((float)PI_);

        for (i = 0; i < stacks; i++)
		{
			GLfloat r = base*(1.0f - (GLfloat)i /stacks);
			GLfloat r1 = base*(1.0f - (GLfloat)(i+1.0)/stacks);
			GLfloat z = height*i /stacks;
			GLfloat z1 = height*(1.0f+i) /stacks;

			for (j = 0; j <= slices; j++)
			{
				GLfloat theta = j == slices ? 0.f : (GLfloat) j /slices*twopi;
				GLfloat ctheta = (GLfloat)cos(theta);
				GLfloat stheta = (GLfloat)sin(theta);

				nx = ctheta;
				ny = stheta;
				nz = sphi;

				*p++ = r1*nx;
				*p++ = r1*ny;
				*p++ = z1;

				*q++ = nx*cphi;
				*q++ = ny*cphi;
				*q++ = nz;

				*p++ = r*nx;
				*p++ = r*ny;
				*p++ = z;


				*q++ = nx*cphi;
				*q++ = ny*cphi;
				*q++ = nz;

			}
		}
    }

    glVertexPointer(3, GL_FLOAT, 0, v);
    glNormalPointer(GL_FLOAT, 0, n);

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

    for(i = 0; i < stacks; i++)
		glDrawArrays(GL_TRIANGLE_STRIP, i*(slices+1)*2, (slices+1)*2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void ugSolidCubef(GLfloat size)
{
    static GLfloat v[108];	   // 108 =  6*18

    static const GLfloat cubev[108] =
	{
		-1., -1., 1.,	/* front */
		 1., -1., 1.,
		-1.,  1., 1.,

		 1., -1., 1.,
		 1.,  1., 1.,
		-1.,  1., 1.,

		-1.,  1., -1.,	/* back */
		 1., -1., -1.,
		-1., -1., -1.,

		-1.,  1., -1.,
		 1.,  1., -1.,
		 1., -1., -1.,

		-1., -1., -1.,	/* left */
		-1., -1.,  1.,
		-1.,  1., -1.,

		-1., -1.,  1.,
		-1.,  1.,  1.,
		-1.,  1., -1.,

		 1., -1.,  1.,	/* right */
		 1., -1., -1.,
		 1.,  1.,  1.,

		 1., -1., -1.,
		 1.,  1., -1.,
		 1.,  1.,  1.,

		-1.,  1.,  1.,	/* top */
		 1.,  1.,  1.,
		-1.,  1., -1.,

		 1.,  1.,  1.,
		 1.,  1., -1.,
		-1.,  1., -1.,

		-1., -1., -1.,	/* bottom */
		 1., -1., -1.,
		-1., -1.,  1.,

		 1., -1., -1.,
		 1., -1.,  1.,
		-1., -1.,  1.,
    };

    static const GLfloat cuben[108] =
	{
		0., 0., 1.,	/* front */
		0., 0., 1.,
		0., 0., 1.,

		0., 0., 1.,
		0., 0., 1.,
		0., 0., 1.,

		0., 0., -1.,	/* back */
		0., 0., -1.,
		0., 0., -1.,

		0., 0., -1.,
		0., 0., -1.,
		0., 0., -1.,

		-1., 0., 0.,	/* left */
		-1., 0., 0.,
		-1., 0., 0.,

		-1., 0., 0.,
		-1., 0., 0.,
		-1., 0., 0.,

		1., 0., 0.,	/* right */
		1., 0., 0.,
		1., 0., 0.,

		1., 0., 0.,
		1., 0., 0.,
		1., 0., 0.,

		0., 1., 0.,	/* top */
		0., 1., 0.,
		0., 1., 0.,

		0., 1., 0.,
		0., 1., 0.,
		0., 1., 0.,

		0., -1., 0.,	/* bottom */
		0., -1., 0.,
		0., -1., 0.,

		0., -1., 0.,
		0., -1., 0.,
		0., -1., 0.,
    };

    int i;
	size /= 2;

    for(i = 0; i < 108; i++)
		v[i] = cubev[i] * size;

    glVertexPointer(3, GL_FLOAT, 0, v);
    glNormalPointer(GL_FLOAT, 0, cuben);

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

    glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

void PlotSpherePoints(GLfloat radius, GLint stacks, GLint slices, GLfloat* v, GLfloat* n)
{

    GLint i, j;
    GLfloat slicestep, stackstep;

    stackstep = ((GLfloat)PI) / stacks;
    slicestep = 2.0f * ((GLfloat)PI) / slices;

    for (i = 0; i < stacks; ++i)
    {
        GLfloat a = i * stackstep;
        GLfloat b = a + stackstep;

        GLfloat s0 =  (GLfloat)sin(a);
        GLfloat s1 =  (GLfloat)sin(b);

        GLfloat c0 =  (GLfloat)cos(a);
        GLfloat c1 =  (GLfloat)cos(b);

        for (j = 0; j <= slices; ++j)
        {
            GLfloat c = j * slicestep;
            GLfloat x = (GLfloat)cos(c);
            GLfloat y = (GLfloat)sin(c);

            GLfloat vx,vy,vz,nx,ny,nz;

            nx = *n = x * s0;
            vx = *v = *n * radius;

            n++;
            v++;

            ny = *n = y * s0;
            vy  = *v = *n * radius;

            n++;
            v++;

            nz = *n = c0;
            vz = *v = *n * radius;
#ifdef UGLIBDEG
            fprintf(UGdumper,"V: (%f,%f,%f)\n", vx , vy , vz);
            fprintf(UGdumper,"N: (%f,%f,%f)\n", nx , ny, nz);
#endif
            n++;
            v++;

            nx = *n = x * s1;
            vx = *v = *n * radius;

            n++;
            v++;

            ny = *n = y * s1;
            vy = *v = *n * radius;

            n++;
            v++;

            nz = *n = c1;
            vz = *v = *n * radius;

            n++;
            v++;
#ifdef UGLIBDEG
            fprintf(UGdumper,"V: (%f,%f,%f)\n", vx , vy , vz);
            fprintf(UGdumper,"N: (%f,%f,%f)\n", nx , ny, nz);
#endif
        }
    }
}

void ugSolidSpheref(GLfloat radius, GLint slices, GLint stacks)
{
    GLint i, triangles;
    static GLfloat* v, *n;
    static GLfloat parms[3];

    if (v)
    {
        if (parms[0] != radius || parms[1] != slices || parms[2] != stacks)
        {
            free(v);
            free(n);

            n = v = 0;

            glVertexPointer(3, GL_FLOAT, 0, 0);
            glNormalPointer(GL_FLOAT, 0, 0);
        }
    }

    if (!v)
    {
        parms[0] = radius;
        parms[1] = (GLfloat)slices;
        parms[2] = (GLfloat)stacks;

        v = (GLfloat*)malloc(stacks*(slices+1)*2*3*(sizeof *v));
        n = (GLfloat*)malloc(stacks*(slices+1)*2*3*(sizeof *n));

        PlotSpherePoints(radius, stacks, slices, v, n);

    }

    glVertexPointer(3, GL_FLOAT, 0, v);
    glNormalPointer(GL_FLOAT, 0, n);

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

    triangles = (slices + 1) * 2;

    for (i = 0; i < stacks; i++)
        glDrawArrays(GL_TRIANGLE_STRIP, i * triangles, triangles);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}

static void __identf(GLfloat m[])
{
    memset(m, 0, sizeof m[0]*16);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void ugluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f)
{
    GLfloat m[4][4];
    GLfloat s, cot, dz = f - n;
    GLfloat rad = fovy/2.f*PI/180.f;

    s = sin(rad);
    if (dz == 0 || s == 0 || aspect == 0)
    {
        printf("ugluPerspectivef fail.\n");
        return;
    }
    cot = cos(rad)/s;

    __identf(&m[0][0]);
    m[0][0] = cot/aspect;
    m[1][1] = cot;
    m[2][2] = -(f + n)/dz;
    m[3][2] = -1.f;
    m[2][3] = -2.f*f*n/dz;
    m[3][3] = 0.f;

    GLfloat mtx[16];
    __identf(mtx);

    int i,j;
    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            mtx[ELM4(i, j)] = m[i][j];
        }
    }

#ifdef UGLIBDEG
    fprintf(UGdumper,"-------------------- Projected Matrix by ugluPerspectivef -------------------\n");
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[3][0], m[3][1], m[3][2], m[3][3]);
    fprintf(UGdumper,"-----------------------------------------------------------------------------\n");

    fprintf(UGdumper,"-------------------- NEW Matrix by ugluPerspectivef -------------------------\n");
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(0, 0)], mtx[ELM4(0, 1)], mtx[ELM4(0, 2)], mtx[ELM4(0, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(1, 0)], mtx[ELM4(1, 1)], mtx[ELM4(1, 2)], mtx[ELM4(1, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(2, 0)], mtx[ELM4(2, 1)], mtx[ELM4(2, 2)], mtx[ELM4(2, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(3, 0)], mtx[ELM4(3, 1)], mtx[ELM4(3, 2)], mtx[ELM4(3, 3)]);
    fprintf(UGdumper,"-----------------------------------------------------------------------------\n");
#endif

    glMultMatrixf(&mtx[0]);
}

static void normalizef(float v[3])
{
    float r;

    r = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (r == 0.0) return;
    r = 1.f/r;

    v[0] *= r;
    v[1] *= r;
    v[2] *= r;
}

static void crossf(float v0[3], float v1[3], float r[3])
{
    r[0] = v0[1]*v1[2] - v0[2]*v1[1];
    r[1] = v0[2]*v1[0] - v0[0]*v1[2];
    r[2] = v0[0]*v1[1] - v0[1]*v1[0];
}

void ugluLookAtf(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
                 GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz)
{
    GLfloat forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    normalizef(forward);

#ifdef UGLIBDEG
    fprintf(UGdumper, "forward:(%f, %f, %f)\n", forward[0], forward[1], forward[2]);
#endif

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    crossf(forward, up, side);

#ifdef UGLIBDEG
    fprintf(UGdumper, "side:(%f, %f, %f)\n", side[0], side[1], side[2]);
#endif

    normalizef(side);
    crossf(side, forward, up);

#ifdef UGLIBDEG
    fprintf(UGdumper, "up:(%f, %f, %f)\n", up[0], up[1], up[2]);
#endif

    __identf(&m[0][0]);
    m[0][0] = side[0];
    m[0][1] = side[1];
    m[0][2] = side[2];

    m[1][0] = up[0];
    m[1][1] = up[1];
    m[1][2] = up[2];

    m[2][0] = -forward[0];
    m[2][1] = -forward[1];
    m[2][2] = -forward[2];

    GLfloat mtx[16];
    __identf(mtx);

    int i,j;
    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            mtx[ELM4(i, j)] = m[i][j];
        }
    }

#ifdef UGLIBDEG
    fprintf(UGdumper,"-------------------- ModelView Matrix by ugluLookAtf ------------------------\n");
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    fprintf(UGdumper, "(%f , %f , %f , %f)\n", m[3][0], m[3][1], m[3][2], m[3][3]);
    fprintf(UGdumper,"-----------------------------------------------------------------------------\n");

    fprintf(UGdumper,"-------------------- NEW Matrix by ugluLookAtf ------------------------------\n");
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(0, 0)], mtx[ELM4(0, 1)], mtx[ELM4(0, 2)], mtx[ELM4(0, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(1, 0)], mtx[ELM4(1, 1)], mtx[ELM4(1, 2)], mtx[ELM4(1, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(2, 0)], mtx[ELM4(2, 1)], mtx[ELM4(2, 2)], mtx[ELM4(2, 3)]);
    fprintf(UGdumper,"(%f , %f , %f , %f)\n", mtx[ELM4(3, 0)], mtx[ELM4(3, 1)], mtx[ELM4(3, 2)], mtx[ELM4(3, 3)]);
    fprintf(UGdumper,"-----------------------------------------------------------------------------\n");
#endif

    glMultMatrixf(&mtx[0]);
    glTranslatef(-eyex, -eyey, -eyez);
}

#else // __GL_FIXED

GLfixed __cosx(GLfixed a)
{
    GLfixed v;
    /* reduce to [0,1) */
    while (a < 0) a += 2*__f2x(__PI);
    a *= __f2x(1.0f/(2.f*__PI));
    a >>= 16;
    a += 0x4000;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? __gl_sin_tab[0x3ff - (a & 0x3ff)] : __gl_sin_tab[a & 0x3ff];
    v = __mulx(v,__f2x(1));
    return (a & 0x800) ? -v : v;
}

GLfixed __sinx(GLfixed a)
{
    GLfixed v;

    /* reduce to [0,1) */
    while (a < 0) a += 2*__f2x(__PI);
    a *= __f2x(1.0f/(2.f*__PI));
    a >>= 16;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? __gl_sin_tab[0x3ff - (a & 0x3ff)] : __gl_sin_tab[a & 0x3ff];
    v = __mulx(v,__f2x(1));
    return (a & 0x800) ? -v : v;
}

static void __identx(GLfixed m[])
{
    memset(m, 0, sizeof m[0]*16);
    m[0] = m[5] = m[10] = m[15] = __f2x(1.0f);
}

GLfixed __sqrtx(GLfixed a)
{
    GLfixed s;
    int i;
    s = (a + __f2x(1)) >> 1;
    /* 6 iterations to converge */
    for (i = 0; i < 6; i++)
        s = (s + __divx(a, s)) >> 1;
    return s;
}

void normalizex(GLfixed* v)
{
    float r;

    r = __sqrtx(__mulx(v[0],v[0]) + __mulx(v[1],v[1]) + __mulx(v[2],v[2]));
    if (r == 0.0f) return;
    r = __divx(1.f,r);

    v[0] *= r;
    v[1] *= r;
    v[2] *= r;
}

void crossx(GLfixed* v0, GLfixed* v1, GLfixed* r)
{
    r[0] = __mulx(v0[1], v1[2]) - __mulx(v0[2], v1[1]);
    r[1] = __mulx(v0[2], v1[0]) - __mulx(v0[0], v1[2]);
    r[2] = __mulx(v0[0], v1[1]) - __mulx(v0[1], v1[0]);
}

void ugluLookAtx(GLfixed eyex, GLfixed eyey, GLfixed eyez, GLfixed centerx,
                 GLfixed centery, GLfixed centerz, GLfixed upx, GLfixed upy, GLfixed upz)
{
    GLfixed forward[3], side[3], up[3];
    GLfixed m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    normalizex(forward);

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    crossx(forward, up, side);

    normalizex(side);
    crossx(side, forward, up);

    __identx(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixx(&m[0][0]);
    glTranslatex(-eyex, -eyey, -eyez);
}

void ugluPerspectivex(GLfixed fovy, GLfixed aspect, GLfixed n, GLfixed f)
{
    GLfixed m[4][4];
    GLfixed s, cot, dz = f - n;
    GLfixed rad = __mulx(fovy,  __f2x(__PI/360.f));

    s = __sinx(rad);
    if (dz == 0 || s == 0 || aspect == 0) return;

    cot = __cosx(rad)/s;

    __identx(&m[0][0]);
    m[0][0] = __divx(cot, aspect);
    m[1][1] = cot;
    m[2][2] = -__divx((f + n), dz);
    m[2][3] = -__f2x(1);
    m[3][2] = __divx(-2*__mulx(f,n), dz);
    m[3][3] = 0;
    glMultMatrixx(&m[0][0]);
}

#endif //__GL_FLOAT or __GL_FIXED

#endif // UGLU_H_INCLUDED
