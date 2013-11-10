/**********************************************************
//  The geometry module simulator
//  Writed by Shun-Fan Tsai
//  Email: shunfan@casmail.ee.ncku.edu.tw
//
//  This module executes matrix transformation,
//  lighting calu, primitive assembly, culling & clipping,
//  tile divider
//
**********************************************************/
#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include "ogles.h"
//#include "fixed.h"

struct tileData
{
  unsigned int      SN_and_TN;
  struct tileData * next_tile;
};
typedef struct tileData * tile_node;

struct triangleData
{
  float  VertexDATA[36];
  struct tileData 	  * tiledata_ptr;
  struct triangleData * next_triangle;
};
typedef struct triangleData * triangle_node;

struct Geometry
{
		triangle_node   triangle_list;
		triangle_node   triangle_iterator;
//-----------Input vertex data---------------
		float    		InputVertex[4];
		float   		InputNormal[3];
		float    		InputColor[4];
		float    		InputTexCoord[4];

//----------Transformation set---------------
//(1)
		int   	 		TriangleCounter;
		int   	 		AvailableTriangleCounter;
		float 	 		EyeCoords[4];
		float 	 		ClipCoords[4];
		float 	 		EyeNormal[4];
//(2)current matrices
		float    		ModelViewMat[16];
		float    		ProjectionMat[16];
		float    		TextureMat[16];
//(3)normal xformation
		float    		InverseM[9]; 			 // Inversed upper model-view matrix
		GLboolean       Normalize;
		GLboolean       RescaleNormal;

//-------------Lighting set------------------
//(1)material
		float    		MatAmbient[4];  // acm
		float    		MatDiffuse[4];  // dcm
		float    		MatSpecular[4]; // scm
		float    		MatEmission[4]; // ecm
		float    		Shininess; 		  // srm
//(2)light source
		float    		LiAmbient[4]; 	 // acli
		float    		LiDiffuse[4]; 	 // dcli
		float    		LiSpecular[4];	 // scli
		float    		Position[4]; 	 	 // Ppli
//(3)lighting state--
		GLboolean       Lighting;
//(4)lighting model--
		float    		LightModelAmbient[4]; // acs
		//GLboolean       LightModelTwoSide; // ¢übs
//(5)color after lighting--
		float 	 		Color[4];
//(6)calculate att*spot (from API)
		float 	 		Att;
		float 	 		Spot;
//------Perspective division set-----------
		float    		normalized_device_coord[3];
		float 	 		perspective_w;
		float 	 		perspectiveA_w;
		float 	 		perspectiveB_w;
		float 	 		perspectiveC_w;
//------Texture Transformation set-----------
		float 	 		TexCoords[4];

//----------Pre-Culling set------------------
		int 		 	check_outside[6];
		int      		pre_clipping_count;
//--------------Culling set------------------
		GLboolean       CullFace;
		GLenum   		CullFaceMode;
		GLenum   		FrontFace;
		int				cullcount;
//--------------Clipping set-----------------
		float    		tmpVtx00[16], tmpVtx01[16], tmpVtx02[16], tmpVtx03[16];
		float    		tmpVtx04[16], tmpVtx05[16], tmpVtx06[16], tmpVtx07[16];
		float    		tmpVtx08[16], tmpVtx09[16], tmpVtx10[16], tmpVtx11[16];
		float    		tmpVtx12[16], tmpVtx13[16], tmpVtx14[16], tmpVtx15[16];
		//float    		* Temporary[16];
		GLboolean       ClipReady;
		int             clipcount;
		float           output_list[6][16];
//--------Primitive assembly set-------------
		GLboolean       TriangleReady;
		float 	 		TriVtxA[16] , TriVtxB[16] , TriVtxC[16] ;
		float 	 		procVtxA[12], procVtxB[12], procVtxC[12];
		int 		 	vtxIndex;
		GLenum   		DrawMode;	 //0:GL_TRIANGLES  1:GL_TRIANGLE_STRIP  2:GL_TRIANGLE_FAN
//---------------View Port set---------------
		float    		ViewportOrigin[2]; // Ox=x+w/2, Oy=y+h/2
		float    		ViewportScale[2];  // w/2, h/2
		float    		DepthRangeFactor;  // (f-n)/2
		float    		DepthRangeBase; 	 // (n+f)/2
//---------------TILE divider----------------
		int   	 		tile_count_num;
		int			 	tile_counter;
		int   	 		search_count;
        int             kick_small_area_count;
        int             triangleListSize;
        int             triangleSize;

//---------------others set------------------
//(1)i
		int      		i;
		bool            frameDEG;
//(2)shade model
		GLenum   		ShadeModel;//{GL_SMOOTH=0,GL_FLAT};

//(7)CLIPVALID
		bool     		clipvalid;
		int				send_vertex;

		Geometry  		(void);
    //function declaration----------------------------------
		int  			do_GM(void);
		void 			transformation(void);
		void  			lighting(void);
		void  			texture_transformation(void);
		void  			primitive_assembly(void);
		void  			Viewport(void);
		void 			tile_divider(int * search_count,int * tile_count_num, triangle_node triangle_list, float * TriVtxA_clipped, float * TriVtxB_clipped, float * TriVtxC_clipped);
		void      	    perspective_division(void);
		void  			MatrixMult(float * matrix, float * input, float * output) ;
		void  			Normalize3(float * Vin	 , float * Vout) ;
		void  			Direction(float * from	 , float * to, float * result);
		float 			VectorDot(float * v1		 , float * v2 );
		float 			CLAMP(float value, float min, float max);
		void  			ClipCoordsToWindowCoords	 (float * pos, float * info);
		float 			Min(float a, float b, float c);
		float 			Max(float a, float b, float c);
		void 			pre_clipping(void);
		int 			CalcCC(int * check_outside ,float * vertex);
		void 			culling(void);
		void 			clipping(float * input[16], float * output[16], float ** Temporary);
		void            release_triangle_list();
		void            initialize();
		float 	        Coeff4q28(float num, float denom);
		float 	        Interpolate(float x0f, float x1f, float coeff4q28);
		void 		    InterpolateVtx(float * result, const float * inside, const float * outside, float coeff4q28);
		unsigned int 	CalcCC2(float * vertex);
		void            ClearStatistics();
};

extern Geometry gm;

#endif
