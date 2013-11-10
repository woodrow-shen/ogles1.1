/**********************************************************
//  The rendering module simulator
//  Writed by Jhe-Yu Liu
//  Email: elvis@casmail.ee.ncku.edu.tw
//
//  Basically it has four function for rendering.
//  They are <TriangleSetup>, <PixelGenerate>, <TextureMapping>,
//  and <PerfragmentOP>.
//
//
**********************************************************/
#ifndef RASTERIZER_H_INCLUDED
#define RASTERIZER_H_INCLUDED

#include "ogles.h"

class Rasterizer
{
public:
    float   plane[11][3];
    float   Edge[3][2];
    float   EffectPixBuffer[TILEHEIGHT*TILEWIDTH*4][16];
    float   area2;
    U8      TColorBuffer[TILEHEIGHT*2][TILEWIDTH*2][4];
    U8      ColorBuffer[WINDOW_DEFAULT_HEIGHT][WINDOW_DEFAULT_WIDTH][4];
    float   TDepthBuffer[TILEHEIGHT*2][TILEWIDTH*2];
    float   DepthBuffer[WINDOW_DEFAULT_HEIGHT][WINDOW_DEFAULT_WIDTH];
    I32     CurWidth, CurHeight;
    U32     EffectP;
    U32     TextureInternalFormat;
    U32     CalcTexAdd(short us,short ub,short uo,short vs,short vb,short vo);

    Rasterizer();
    //OpenGL Context state
    //Enable/Disable Units
    bool    TexMappingEn,BlendingEn,FogEn;
    bool    DepthTestEn, AlphaTestEn, ScissorTestEn, StencilTestEn, LogicOpTestEn, MultiSampleEn;
    U8      pattern;        //0:diagonal    1:OGSS  2:RGSS
    //Alpha Test
    U32     AlphaTestMode;
    U32     AlphaRef;
    //Scissor Test
    U32     SciWidth;
    U32     SciHeight;
    U32     SciLeft;
    U32     SciBottom;
    //Depth Test
    U32     DepthTestMode;
    float   DepthRef;
    //Stencil Test
    U32     StenMode;
    U32     StenMask;
    float   StenRef;
    U32     StenSfail;
    U32     StenDfail;
    U32     StenDpass;
    //Blending Unit
    U32     BlendingSrcMode;
    U32     BlendingDstMode;
    U8      BlendingSrc[4];
    U8      BlendingDst[4];
    //Logic OP Unit
    U32     LogicOP;
    //Fog Unit
    U32     FogMode;
    float   FogDensity;
    float   FogStart;
    float   FogEnd;
    U8      FogColor[4];
    //Texture Unit
    U32     TexHeight,TexWidth;
    U32     TexFilterMode;  //Texture filtering mode
    U32     TexWrapModeS,TexWrapModeT;
    U32     TexEnvMode;
    U8      TexEnvColor[4];
    //Clear
    U8      ColorClearVal[4];

    U8      *TEXfp;
    FILE    *TEXDEBUGfp;
    FILE    *PIXEL_GENERATE_DEBUGfp;
    FILE    *SCAN_ALGO_DEBUGfp;

    void    TriangleSetup(float* vertexA, float* vertexB, float* vertexC);
    void    PixelGenerate(float* vertexA, float* vertexB, float* vertexC);
    U32     GetTexColor(const unsigned short u, const unsigned short v);
    void    TextureMapping();
    void    PerFragmentOp();
    void    ClearStatistics();
    void    SetStateReg(unsigned short id, unsigned int value);
    //void    TileInitialize();
    void    ClearBuffer(unsigned int mask);
    void    DumpImage(int ImageType, char *filename);
    void    DumpDepthImage(int ImageType, char *filename);
    void    EndTile(U32 * ColorBuf);

    //stastic data
    int     validstamp, scanstamp, coldscan, invalidstamp;
    int     validpixel, scanpixel, shadedpixel;

    int     depthBufferBW[2]; //Read,Write
    int     colorBufferBW[2];
    int     textureBW;
    int     triCount;
    U32     blendCount;

    bool    tileDEG;

    struct
    {
        bool valid[TEX_CACHE_ENTRY_X*TEX_CACHE_ENTRY_Y];
        int pos[TEX_CACHE_ENTRY_X*TEX_CACHE_ENTRY_Y];
        int color[TEX_CACHE_ENTRY_X*TEX_CACHE_ENTRY_Y][TEX_CACHE_BLOCK_SIZE];
        int TexCacheHit;
        int TexCacheMiss;
        int TexCacheColdMiss;
    }TexCache;
};

extern Rasterizer rm;

#endif
