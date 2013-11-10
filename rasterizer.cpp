/**********************************************************
//  The rendering module simulator
//  Writed by Jhe-Yu Liu
//  Email: elvis@casmail.ee.ncku.edu.tw
**********************************************************/

#include "rasterizer.h"

Rasterizer rm;

//Plane format
//Z     InvW    Tex0U   Tex0V   Tex1U   Tex1V   COLR    COLG    COLB    COLA    FOG
//0     1       2       3       4       5       6       7       8       9       10

//Vertex format
//X     Y       Z       InvW    Tex0U   Tex0V   Tex1U   Tex1V   COLR    COLG    COLB    COLA    FOG
//0     1       2       3       4       5       6       7       8       9       10      11      12

#define INPUT_SIZE 11
#define PLANE_SIZE 9
#define BARYCENTRIC_METHOD
//#define CENTER_SCAN_METHOD

Rasterizer::Rasterizer()
{
    int i,j;
    for (i = 0;i < TEX_CACHE_ENTRY_X*TEX_CACHE_ENTRY_Y;i++)
        TexCache.valid[i] = false;
//    for (j = 0;j < WINDOW_DEFAULT_HEIGHT;j++)
//        for (i = 0;i < WINDOW_DEFAULT_WIDTH;i++)
//        {
//            ColorBuffer[j][i][0] = 0;
//            ColorBuffer[j][i][1] = 0;
//            ColorBuffer[j][i][2] = 0;
//            ColorBuffer[j][i][3] = 0;
//        }

    TexCache.TexCacheHit = 0;
    TexCache.TexCacheMiss = 0;
    TexCache.TexCacheColdMiss = 0;

    validstamp = 0;
    scanstamp = 0;
    coldscan = 0;
    invalidstamp = 0;
    validpixel = 0;
    scanpixel = 0;
    shadedpixel = 0;
    depthBufferBW[0] = 0;
    depthBufferBW[1] = 0;
    colorBufferBW[0] = 0;
    colorBufferBW[1] = 0;
    textureBW = 0;

    triCount = 0;
    CurWidth = 0;
    CurHeight = 0;

//    for (i = 0;i < 4;i++)
//    {
//        FogColor[i] = 0;
//        TexEnvColor[i] = 0;
//        ColorClearVal[i] = 0;
//    }

    // Reset All GL States
    // Enable/Disable
    TexMappingEn = false;
    MultiSampleEn = false;
    pattern = 2;
    BlendingEn = false;
    FogEn = false;
    DepthTestEn = false;
    AlphaTestEn = false;
    ScissorTestEn = false;
    StencilTestEn = false;
    LogicOpTestEn = false;
    // Alpha Test
    AlphaTestMode = GL_ALWAYS;
    AlphaRef = 0.0;
    // Scissor Test

    // Stencil Test

    // Depth Test
    DepthTestMode = GL_LESS;
    DepthRef = 1.0;
    // Blending Unit

    BlendingSrcMode = GL_SRC_ALPHA;
    BlendingDstMode = GL_ONE_MINUS_SRC_ALPHA;

    // Logic OP Unit

    // Fog Unit

    // Texture Unit
    TextureInternalFormat = GL_RGB;
    TexFilterMode = GL_NEAREST;
    TexWrapModeS = GL_REPEAT;
    TexWrapModeT = GL_REPEAT;
    TexEnvMode = GL_MODULATE;

    if (TEXDEBUG)
        TEXDEBUGfp = fopen("TexDebug.txt","w");
    if (PIXEL_GENERATE_DEBUG)
        PIXEL_GENERATE_DEBUGfp = fopen("PixelGenerateDebug.txt","w");
    if (SCAN_ALGO_DEBUG)
        SCAN_ALGO_DEBUGfp = fopen("ScanAlgoDebug.txt","w");
}

void Rasterizer::TriangleSetup(float* vertexA, float* vertexB, float* vertexC)
{
#ifdef BARYCENTRIC_METHOD
    // TODO (elvis#1#): Sub pixel
    int a;
    float constantC;

    Edge[0][0] = vertexA[1]-vertexB[1];
    Edge[0][1] = vertexA[0]-vertexB[0];
    Edge[1][0] = vertexB[1]-vertexC[1];
    Edge[1][1] = vertexB[0]-vertexC[0];
    Edge[2][0] = vertexC[1]-vertexA[1];
    Edge[2][1] = vertexC[0]-vertexA[0];

    constantC = Edge[0][1]*Edge[1][0]-Edge[0][0]*Edge[1][1];
    area2 = 1/constantC;

#else
    int a;

    float constantC;
    float vectorU[16];
    float vectorV[16];

    for (a = 0;a <= INPUT_SIZE;a++)
    {
        vectorU[a] = *(vertexA+a) - *(vertexB+a);
        vectorV[a] = *(vertexC+a) - *(vertexB+a);
    }

    constantC = -vectorU[0]*vectorV[1]+vectorU[1]*vectorV[0];

    for (a = 0;a<= PLANE_SIZE;a++)
    {
        plane[a][0] = (vectorU[1]*vectorV[a+2]-vectorU[a+2]*vectorV[1])/constantC;
        plane[a][1] = -(vectorU[0]*vectorV[a+2]-vectorU[a+2]*vectorV[0])/constantC;
    }

    Edge[0][0] = *(vertexA+1)-*(vertexB+1);
    Edge[0][1] = *(vertexA+0)-*(vertexB+0);
    Edge[1][0] = *(vertexB+1)-*(vertexC+1);
    Edge[1][1] = *(vertexB+0)-*(vertexC+0);
    Edge[2][0] = *(vertexC+1)-*(vertexA+1);
    Edge[2][1] = *(vertexC+0)-*(vertexA+0);

#endif

}

void Rasterizer::PixelGenerate(float* vertexA, float* vertexB, float* vertexC)
{
#ifdef BARYCENTRIC_METHOD

    int a = 0;
    int b,c,x,y;
    bool EarlyDepthTest = true;
    float u[4],v[4];
    float EdgeTest[27];
    bool EdgeStatus[3];
    bool EffectStatus, LastEffectStatus,EffectDraw[4];
    bool RowStatus, LastRowStatus;
    int count = 0;
    int vcount = 0;
    float DepthCheck;

    // Bondary Box in tile
    int LX, RX;
    int LY, HY;

    RowStatus = false;
    LastRowStatus = false;
    EffectP = 0;

    //find the lowest y value
    c = ((int)*(vertexB+1)<(int)*(vertexC+1))?(int)*(vertexB+1):(int)*(vertexC+1);
    b = ((int)*(vertexA+1)<c)?((int)*(vertexA+1)):c;
    LY = (CurHeight>b)?CurHeight:b;

    //find the leftest x value
    c = ((int)*(vertexB+0)<(int)*(vertexC+0))?(int)*(vertexB+0):(int)*(vertexC+0);
    b = ((int)*(vertexA+0)<c)?((int)*(vertexA+0)):c;
    LX = (CurWidth>b)?CurWidth:b;

    //find the highest y value
    c = ((int)*(vertexB+1)>(int)*(vertexC+1))?(int)*(vertexB+1):(int)*(vertexC+1);
    b = ((int)*(vertexA+1)>c)?((int)*(vertexA+1)):c;
    HY = (CurHeight+TILEHEIGHT-1<b)?CurHeight+TILEHEIGHT-1:b;

    //find the most right x value
    c = ((int)*(vertexB+0)>(int)*(vertexC+0))?(int)*(vertexB+0):(int)*(vertexC+0);
    b = ((int)*(vertexA+0)>c)?((int)*(vertexA+0)):c;
    RX = (CurWidth+TILEWIDTH-1<b)?CurWidth+TILEWIDTH-1:b;


    bool centerPointHit = false;
    int  centerX = 0;
    int  traversalState = 0;
    int  traversalDirection = 0;    //1:left 2:right
    int  iteratorLeft = 0, iteratorRight = 0;
    int  rowLSkipCounter = 0, rowRSkipCounter = 0;

    centerX = (LX+RX)/2;

    if (LX>RX) goto scan_over;

#ifdef CENTER_SCAN_METHOD

    for (y = LY;y <= HY;y++)
    {
        LastRowStatus = RowStatus;
        RowStatus = false;
        EffectStatus = false;
        LastEffectStatus = false;

        bool firstHitLeft = false,firstHitRight = false;

        switch (traversalState)
        {
        case 0:
            // Phase 1
            // scan from left ->
            for (iteratorLeft = LX;iteratorLeft<=RX;iteratorLeft++)
            {
                count++;
                ++scanpixel;
                //First edge test on four corner point
                EdgeTest[0] = (iteratorLeft+0.5-*(vertexB+0))*Edge[0][0]-
                              (y+0.5-*(vertexB+1))*Edge[0][1];
                EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[3] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[4] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
                //Second edge test on four corner point
                EdgeTest[5] = (iteratorLeft+0.5-*(vertexC+0))*Edge[1][0]-
                              (y+0.5-*(vertexC+1))*Edge[1][1];
                EdgeTest[6] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
                EdgeTest[7] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[8] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[9] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
                //Third edge test on four corner point
                EdgeTest[10] = (iteratorLeft+0.5-*(vertexA+0))*Edge[2][0]-
                               (y+0.5-*(vertexA+1))*Edge[2][1];
                EdgeTest[11] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
                EdgeTest[12] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[13] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[14] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;

                LastEffectStatus = EffectStatus;

                EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
                EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
                EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
                EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];

                if (MultiSampleEn == false)
                {
                    EdgeTest[15] = EdgeTest[0];
                    EdgeTest[16] = EdgeTest[5];
                    EdgeTest[17] = EdgeTest[10];
                    EffectDraw[0] = EdgeTest[0]>=0 & EdgeTest[5]>=0 & EdgeTest[10]>=0;
                }

                if (EffectStatus)
                {
                    RowStatus = true;
                    if (EffectDraw[0])
                    {
                        vcount++;
                        validpixel++;
                        if (iteratorLeft==LX) firstHitLeft = true;

                        EffectPixBuffer[EffectP][0] = iteratorLeft;
                        EffectPixBuffer[EffectP][1] = y;
                        u[0] = EdgeTest[17]*area2;
                        u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                        v[0] = EdgeTest[15]*area2;
                        v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                        for (a = 2;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                        for (a = 4;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];//perspective correction

                        if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                            fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                    (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                    (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                        EffectP++;

                        if (iteratorLeft==centerX)
                        {
                            centerPointHit = true;
                        }
                    }
                }

                if (iteratorLeft>=centerX && LastEffectStatus==false && EffectStatus==false)
                    break;
                if (LastEffectStatus==true && EffectStatus==false)
                    break;

            }

            if (iteratorLeft>=centerX && LastEffectStatus==false && EffectStatus==false)
            {
                // scan from right <-
                for (iteratorRight = RX;iteratorRight > centerX;iteratorRight--)
                {

                    count++;
                    ++scanpixel;
                    //First edge test on four corner point
                    EdgeTest[0] = (iteratorRight+0.5-*(vertexB+0))*Edge[0][0]-
                                  (y+0.5-*(vertexB+1))*Edge[0][1];
                    EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
                    EdgeTest[2] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
                    EdgeTest[3] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
                    EdgeTest[4] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
                    //Second edge test on four corner point
                    EdgeTest[5] = (iteratorRight+0.5-*(vertexC+0))*Edge[1][0]-
                                  (y+0.5-*(vertexC+1))*Edge[1][1];
                    EdgeTest[6] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
                    EdgeTest[7] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
                    EdgeTest[8] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
                    EdgeTest[9] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
                    //Third edge test on four corner point
                    EdgeTest[10] = (iteratorRight+0.5-*(vertexA+0))*Edge[2][0]-
                                   (y+0.5-*(vertexA+1))*Edge[2][1];
                    EdgeTest[11] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
                    EdgeTest[12] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
                    EdgeTest[13] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
                    EdgeTest[14] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;

                    LastEffectStatus = EffectStatus;

                    EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
                    EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
                    EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
                    EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];

                    if (MultiSampleEn == false)
                    {
                        EdgeTest[15] = EdgeTest[0];
                        EdgeTest[16] = EdgeTest[5];
                        EdgeTest[17] = EdgeTest[10];
                        EffectDraw[0] = EdgeTest[0]>=0 & EdgeTest[5]>=0 & EdgeTest[10]>=0;
                    }

                    if (EffectStatus)
                    {
                        RowStatus = true;
                        if (EffectDraw[0])
                        {
                            vcount++;
                            validpixel++;
                            if (iteratorRight==RX) firstHitRight = true;

                            EffectPixBuffer[EffectP][0] = iteratorRight;
                            EffectPixBuffer[EffectP][1] = y;
                            u[0] = EdgeTest[17]*area2;
                            u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                            v[0] = EdgeTest[15]*area2;
                            v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                            for (a = 2;a <= 12;a++)
                                EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                            for (a = 4;a <= 12;a++)
                                EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];//perspective correction

                            if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                                fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                        (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                        (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                            EffectP++;
                        }
                    }

                    if (LastEffectStatus==true && EffectStatus==false)
                        break;
                }

                if (firstHitRight==true && LastEffectStatus==true && EffectStatus==false)
                {
                    //centerX = (RX+iteratorRight)/2;
                    centerX = RX;
                    traversalState = 1;
                    traversalDirection = 2;
                }
                else
                    traversalState = 0;

            }
            else if (firstHitLeft==true && centerPointHit==false && LastEffectStatus==true && EffectStatus==false)
            {
                //centerX = (LX+iteratorLeft)/2;
                centerX = LX;
                traversalState = 1;
                traversalDirection = 1;
            }
            else if (centerPointHit==true)
                traversalState = 1;
            else
                traversalState = 0;

            break;

        case 1:
            // Phase 2
            // scan : left <- center
            for (iteratorLeft=centerX;iteratorLeft >= LX;iteratorLeft--)
            {
                count++;
                ++scanpixel;
                //First edge test on four corner point
                EdgeTest[0] = (iteratorLeft+0.5-*(vertexB+0))*Edge[0][0]-
                              (y+0.5-*(vertexB+1))*Edge[0][1];
                EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[3] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[4] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
                //Second edge test on four corner point
                EdgeTest[5] = (iteratorLeft+0.5-*(vertexC+0))*Edge[1][0]-
                              (y+0.5-*(vertexC+1))*Edge[1][1];
                EdgeTest[6] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
                EdgeTest[7] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[8] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[9] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
                //Third edge test on four corner point
                EdgeTest[10] = (iteratorLeft+0.5-*(vertexA+0))*Edge[2][0]-
                               (y+0.5-*(vertexA+1))*Edge[2][1];
                EdgeTest[11] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
                EdgeTest[12] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[13] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[14] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;

                LastEffectStatus = EffectStatus;

                EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
                EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
                EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
                EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];

                if (MultiSampleEn == false)
                {
                    EdgeTest[15] = EdgeTest[0];
                    EdgeTest[16] = EdgeTest[5];
                    EdgeTest[17] = EdgeTest[10];
                    EffectDraw[0] = EdgeTest[0]>=0 & EdgeTest[5]>=0 & EdgeTest[10]>=0;
                }

                if (EffectStatus)
                {
                    RowStatus = true;
                    if (EffectDraw[0])
                    {
                        vcount++;
                        validpixel++;
                        EffectPixBuffer[EffectP][0] = iteratorLeft;
                        EffectPixBuffer[EffectP][1] = y;
                        u[0] = EdgeTest[17]*area2;
                        u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                        v[0] = EdgeTest[15]*area2;
                        v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                        for (a = 2;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                        for (a = 4;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];//perspective correction

                        if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                            fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                    (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                    (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                        EffectP++;
                    }
                }

                if (LastEffectStatus==true && EffectStatus==false)
                    break;
            }

            // scan : center -> right
            for (iteratorRight=centerX+1;iteratorRight <= RX;iteratorRight++)
            {
                count++;
                ++scanpixel;
                //First edge test on four corner point
                EdgeTest[0] = (iteratorRight+0.5-*(vertexB+0))*Edge[0][0]-
                              (y+0.5-*(vertexB+1))*Edge[0][1];
                EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[3] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[4] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
                //Second edge test on four corner point
                EdgeTest[5] = (iteratorRight+0.5-*(vertexC+0))*Edge[1][0]-
                              (y+0.5-*(vertexC+1))*Edge[1][1];
                EdgeTest[6] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
                EdgeTest[7] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[8] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[9] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
                //Third edge test on four corner point
                EdgeTest[10] = (iteratorRight+0.5-*(vertexA+0))*Edge[2][0]-
                               (y+0.5-*(vertexA+1))*Edge[2][1];
                EdgeTest[11] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
                EdgeTest[12] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[13] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
                EdgeTest[14] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;

                LastEffectStatus = EffectStatus;

                EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
                EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
                EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
                EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];

                if (MultiSampleEn == false)
                {
                    EdgeTest[15] = EdgeTest[0];
                    EdgeTest[16] = EdgeTest[5];
                    EdgeTest[17] = EdgeTest[10];
                    EffectDraw[0] = EdgeTest[0]>=0 & EdgeTest[5]>=0 & EdgeTest[10]>=0;
                }

                if (EffectStatus)
                {
                    RowStatus = true;
                    if (EffectDraw[0])
                    {
                        vcount++;
                        validpixel++;
                        EffectPixBuffer[EffectP][0] = iteratorRight;
                        EffectPixBuffer[EffectP][1] = y;
                        u[0] = EdgeTest[17]*area2;
                        u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                        v[0] = EdgeTest[15]*area2;
                        v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                        for (a = 2;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                        for (a = 4;a <= 12;a++)
                            EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];//perspective correction

                        if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                            fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                    (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                    (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                        EffectP++;
                    }
                }

                if (LastEffectStatus==true && EffectStatus==false)
                    break;
            }

            break;

        default:
            break;

        }

        if (LastRowStatus==true && RowStatus==false)
            break;
    }

#else

    for (y = LY;y <= HY;y++)
    {//printf("y:%d, LY:%d, HY:%d\n", y, LY, HY);
        LastRowStatus = RowStatus;
        RowStatus = false;
        EffectStatus = false;
        LastEffectStatus = false;
        for (x = LX;x <= RX;x++)
        {
            count++;
            ++scanpixel;
            //scanstamp++;
            //First edge test on four corner point
            EdgeTest[0] = (x+0.5-*(vertexB+0))*Edge[0][0]-
                          (y+0.5-*(vertexB+1))*Edge[0][1];
            EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
            EdgeTest[2] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
            EdgeTest[3] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
            EdgeTest[4] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
            //Second edge test on four corner point
            EdgeTest[5] = (x+0.5-*(vertexC+0))*Edge[1][0]-
                          (y+0.5-*(vertexC+1))*Edge[1][1];
            EdgeTest[6] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
            EdgeTest[7] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
            EdgeTest[8] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
            EdgeTest[9] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
            //Third edge test on four corner point
            EdgeTest[10] = (x+0.5-*(vertexA+0))*Edge[2][0]-
                           (y+0.5-*(vertexA+1))*Edge[2][1];
            EdgeTest[11] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
            EdgeTest[12] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
            EdgeTest[13] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
            EdgeTest[14] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;

            LastEffectStatus = EffectStatus;

            EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
            EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
            EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
            EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];

            if(MultiSampleEn == false)
            {
                EdgeTest[15] = EdgeTest[0];
                EdgeTest[16] = EdgeTest[5];
                EdgeTest[17] = EdgeTest[10];
                EffectDraw[0] = EdgeTest[0]>=0 & EdgeTest[5]>=0 & EdgeTest[10]>=0;
            }
            else
            {
                if (pattern == 0)
                {
//                    if(((Edge[0][0]*Edge[0][1]>=0)+(Edge[1][0]*Edge[1][1]>=0)+(Edge[2][0]*Edge[2][1]>=0))<=0)
//                    {
                        EdgeTest[15] = EdgeTest[0] + Edge[0][0]/4 + Edge[0][1]/4;
                        EdgeTest[16] = EdgeTest[5] + Edge[1][0]/4 + Edge[1][1]/4;
                        EdgeTest[17] = EdgeTest[10] + Edge[2][0]/4 + Edge[2][1]/4;
                        EdgeTest[18] = EdgeTest[0] - Edge[0][0]/4 - Edge[0][1]/4;
                        EdgeTest[19] = EdgeTest[5] - Edge[1][0]/4 - Edge[1][1]/4;
                        EdgeTest[20] = EdgeTest[10] - Edge[2][0]/4 - Edge[2][1]/4;
//                    }
//                    else
//                    {
//                        EdgeTest[15] = EdgeTest[0] + Edge[0][0]/4 - Edge[0][1]/4;
//                        EdgeTest[16] = EdgeTest[5] + Edge[1][0]/4 - Edge[1][1]/4;
//                        EdgeTest[17] = EdgeTest[10] + Edge[2][0]/4 - Edge[2][1]/4;
//                        EdgeTest[18] = EdgeTest[0] - Edge[0][0]/4 + Edge[0][1]/4;
//                        EdgeTest[19] = EdgeTest[5] - Edge[1][0]/4 + Edge[1][1]/4;
//                        EdgeTest[20] = EdgeTest[10] - Edge[2][0]/4 + Edge[2][1]/4;
//                    }
                    EffectDraw[0] = EdgeTest[15]>=0 & EdgeTest[16]>=0 & EdgeTest[17]>=0;
                    EffectDraw[1] = EdgeTest[18]>=0 & EdgeTest[19]>=0 & EdgeTest[20]>=0;
                }
                else if(pattern == 1)
                {
                    EdgeTest[15] = EdgeTest[0] + Edge[0][0]/4 + Edge[0][1]/4;
                    EdgeTest[16] = EdgeTest[5] + Edge[1][0]/4 + Edge[1][1]/4;
                    EdgeTest[17] = EdgeTest[10] + Edge[2][0]/4 + Edge[2][1]/4;
                    EdgeTest[18] = EdgeTest[0] - Edge[0][0]/4 - Edge[0][1]/4;
                    EdgeTest[19] = EdgeTest[5] - Edge[1][0]/4 - Edge[1][1]/4;
                    EdgeTest[20] = EdgeTest[10] - Edge[2][0]/4 - Edge[2][1]/4;
                    EdgeTest[21] = EdgeTest[0] + Edge[0][0]/4 - Edge[0][1]/4;
                    EdgeTest[22] = EdgeTest[5] + Edge[1][0]/4 - Edge[1][1]/4;
                    EdgeTest[23] = EdgeTest[10] + Edge[2][0]/4 - Edge[2][1]/4;
                    EdgeTest[24] = EdgeTest[0] - Edge[0][0]/4 + Edge[0][1]/4;
                    EdgeTest[25] = EdgeTest[5] - Edge[1][0]/4 + Edge[1][1]/4;
                    EdgeTest[26] = EdgeTest[10] - Edge[2][0]/4 + Edge[2][1]/4;
                    EffectDraw[0] = EdgeTest[15]>=0 & EdgeTest[16]>=0 & EdgeTest[17]>=0;
                    EffectDraw[1] = EdgeTest[18]>=0 & EdgeTest[19]>=0 & EdgeTest[20]>=0;
                    EffectDraw[2] = EdgeTest[21]>=0 & EdgeTest[22]>=0 & EdgeTest[23]>=0;
                    EffectDraw[3] = EdgeTest[24]>=0 & EdgeTest[25]>=0 & EdgeTest[26]>=0;
                }
                else if(pattern == 2)
                {
                    EdgeTest[15] = EdgeTest[0] + Edge[0][0]/8*3 + Edge[0][1]/8;
                    EdgeTest[16] = EdgeTest[5] + Edge[1][0]/8*3 + Edge[1][1]/8;
                    EdgeTest[17] = EdgeTest[10] + Edge[2][0]/8*3 + Edge[2][1]/8;
                    EdgeTest[18] = EdgeTest[0] - Edge[0][0]/8*3 - Edge[0][1]/8;
                    EdgeTest[19] = EdgeTest[5] - Edge[1][0]/8*3 - Edge[1][1]/8;
                    EdgeTest[20] = EdgeTest[10] - Edge[2][0]/8*3 - Edge[2][1]/8;
                    EdgeTest[21] = EdgeTest[0] + Edge[0][0]/8 - Edge[0][1]/8*3;
                    EdgeTest[22] = EdgeTest[5] + Edge[1][0]/8 - Edge[1][1]/8*3;
                    EdgeTest[23] = EdgeTest[10] + Edge[2][0]/8 - Edge[2][1]/8*3;
                    EdgeTest[24] = EdgeTest[0] - Edge[0][0]/8 + Edge[0][1]/8*3;
                    EdgeTest[25] = EdgeTest[5] - Edge[1][0]/8 + Edge[1][1]/8*3;
                    EdgeTest[26] = EdgeTest[10] - Edge[2][0]/8 + Edge[2][1]/8*3;
                    EffectDraw[0] = EdgeTest[15]>=0 & EdgeTest[16]>=0 & EdgeTest[17]>=0;
                    EffectDraw[1] = EdgeTest[18]>=0 & EdgeTest[19]>=0 & EdgeTest[20]>=0;
                    EffectDraw[2] = EdgeTest[21]>=0 & EdgeTest[22]>=0 & EdgeTest[23]>=0;
                    EffectDraw[3] = EdgeTest[24]>=0 & EdgeTest[25]>=0 & EdgeTest[26]>=0;
                }
            }

//            if (x == 281 & y == 96)
//            {
//                printf("%10.6f %10.6f %10.6f %10.6f %10.6f %d\n",EdgeTest[15],EdgeTest[16],EdgeTest[17],
//                       Edge[0][0],Edge[0][1],EffectDraw[0]);
//                printf("(%f %f),(%f %f),(%f %f)\n",vertexA[0],vertexA[1],vertexB[0],vertexB[1],vertexC[0],vertexC[1]);
//            }

            if (EffectStatus)
            {
                //++validstamp;
                RowStatus = true;
                if (EffectDraw[0])
                {
                    vcount++;
                    validpixel++;
                    EffectPixBuffer[EffectP][0] = x;
                    EffectPixBuffer[EffectP][1] = y;
                    u[0] = EdgeTest[17]*area2;
                    u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                    v[0] = EdgeTest[15]*area2;
                    v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];//perspective correction

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }
                if (EffectDraw[1] && MultiSampleEn)
                {
                    //vcount++;
                    //validpixel++;
                    EffectPixBuffer[EffectP][0] = x;
                    EffectPixBuffer[EffectP][1] = y+TILEHEIGHT;
                    u[0] = EdgeTest[20]*area2;
                    u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                    v[0] = EdgeTest[18]*area2;
                    v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }
                if (EffectDraw[2] && MultiSampleEn && pattern>=1)
                {
                    //vcount++;
                    //validpixel++;
                    EffectPixBuffer[EffectP][0] = x+TILEWIDTH;
                    EffectPixBuffer[EffectP][1] = y;
                    u[0] = EdgeTest[23]*area2;
                    u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                    v[0] = EdgeTest[21]*area2;
                    v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }
                if (EffectDraw[3] && MultiSampleEn && pattern>=1)
                {
                    //vcount++;
                    //validpixel++;
                    EffectPixBuffer[EffectP][0] = x+TILEWIDTH;
                    EffectPixBuffer[EffectP][1] = y+TILEHEIGHT;
                    u[0] = EdgeTest[26]*area2;
                    u[0] = (u[0]>1)?1:(u[0]<0)?0:u[0];
                    v[0] = EdgeTest[24]*area2;
                    v[0] = (v[0]>1)?1:(v[0]<0)?0:v[0];
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + u[0]*(vertexB[a]-vertexA[a]) + v[0]*(vertexC[a] - vertexA[a]);

                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }
            }
            else
            {
                //invalidstamp++;

                if ((LastEffectStatus == true)&(EffectStatus == false))
                    break;
                if (vcount == 0)
                    coldscan++;
            }
        }
        if ((LastRowStatus == true)&(RowStatus == false))
            break;
    }

#endif
scan_over:

    if (SCAN_ALGO_DEBUG)
    {
        triCount++;
        fprintf(SCAN_ALGO_DEBUGfp,"------- A Tile begin at (%d,%d) -------\n",CurWidth,CurHeight);
        fprintf(SCAN_ALGO_DEBUGfp,"Tile Bondary Box:  X:(%d~%d)  Y:(%d~%d)\n",LX,RX,LY,HY);
        fprintf(SCAN_ALGO_DEBUGfp,"Valid pixel: %d\n",vcount);
        fprintf(SCAN_ALGO_DEBUGfp,"Scaned pixel: %d\n",count);
        fprintf(SCAN_ALGO_DEBUGfp,"Average Scan Rate in this Tile: %2.2f\n", (float)vcount/count);
        fprintf(SCAN_ALGO_DEBUGfp,"triCount: %d\n",triCount);
        fprintf(SCAN_ALGO_DEBUGfp,"---------------------------------------\n");

    }

    if (PIXEL_GENERATE_DEBUG)
        if (vcount > 0)
        {
            fprintf(PIXEL_GENERATE_DEBUGfp,"Valid pixel: %d\n",vcount);
            fprintf(PIXEL_GENERATE_DEBUGfp,"Scaned pixel: %d\n",count);
            fprintf(PIXEL_GENERATE_DEBUGfp,"Tile Bondary Box:  X:(%d~%d)  Y:(%d~%d)\n",LX,RX,LY,HY);
            fprintf(PIXEL_GENERATE_DEBUGfp,"/*************A Tile begin at (%d,%d)**************/\n\n",CurWidth,CurHeight);
        }
#else

    int a = 0;
    int b,c,x,y;
    bool EarlyDepthTest = true;
    float EdgeTest[15];
    float PixelStamp[4][16];
    bool EdgeStatus[3];
    bool EffectStatus, LastEffectStatus,EffectDraw[4];
    bool RowStatus, LastRowStatus;
    int count = 0;
    int vcount = 0;
    float DepthCheck;

    // Bondary Box in tile
    int LX, RX;
    int LY, HY;

    RowStatus = false;
    LastRowStatus = false;
    EffectP = 0;

    //find the lowest y value
    c = ((int)*(vertexB+1)<(int)*(vertexC+1))?(int)*(vertexB+1):(int)*(vertexC+1);
    b = ((int)*(vertexA+1)<c)?((int)*(vertexA+1)):c;
    LY = (CurHeight>b)?CurHeight:b;

    //find the leftest x value
    c = ((int)*(vertexB+0)<(int)*(vertexC+0))?(int)*(vertexB+0):(int)*(vertexC+0);
    b = ((int)*(vertexA+0)<c)?((int)*(vertexA+0)):c;
    LX = (CurWidth>b)?CurWidth:b;

    //find the highest y value
    c = ((int)*(vertexB+1)>(int)*(vertexC+1))?(int)*(vertexB+1):(int)*(vertexC+1);
    b = ((int)*(vertexA+1)>c)?((int)*(vertexA+1)):c;
    HY = (CurHeight+TILEHEIGHT-1<b)?CurHeight+TILEHEIGHT-1:b;

    //find the most right x value
    c = ((int)*(vertexB+0)>(int)*(vertexC+0))?(int)*(vertexB+0):(int)*(vertexC+0);
    b = ((int)*(vertexA+0)>c)?((int)*(vertexA+0)):c;
    RX = (CurWidth+TILEWIDTH-1<b)?CurWidth+TILEWIDTH-1:b;

    for (y = (LY>>1);y <= (HY>>1);y++)
    {
        LastRowStatus = RowStatus;
        RowStatus = false;
        EffectStatus = false;
        LastEffectStatus = false;
        for (x = (LX>>1);x <= (RX>>1);x++)
        {
            count++;
            scanstamp++;
            //First edge test on four corner point
            EdgeTest[0] = ((x<<1)+1-*(vertexB+0))*Edge[0][0]-
                          ((y<<1)+1-*(vertexB+1))*Edge[0][1];
            EdgeTest[1] = EdgeTest[0] + Edge[0][0] + Edge[0][1];
            EdgeTest[2] = EdgeTest[0] - Edge[0][0] - Edge[0][1];
            EdgeTest[3] = EdgeTest[0] + Edge[0][0] - Edge[0][1];
            EdgeTest[4] = EdgeTest[0] - Edge[0][0] + Edge[0][1];
            //Second edge test on four corner point
            EdgeTest[5] = ((x<<1)+1-*(vertexC+0))*Edge[1][0]-
                          ((y<<1)+1-*(vertexC+1))*Edge[1][1];
            EdgeTest[6] = EdgeTest[5] + Edge[1][0] + Edge[1][1];
            EdgeTest[7] = EdgeTest[5] - Edge[1][0] - Edge[1][1];
            EdgeTest[8] = EdgeTest[5] + Edge[1][0] - Edge[1][1];
            EdgeTest[9] = EdgeTest[5] - Edge[1][0] + Edge[1][1];
            //Third edge test on four corner point
            EdgeTest[10] = ((x<<1)+1-*(vertexA+0))*Edge[2][0]-
                           ((y<<1)+1-*(vertexA+1))*Edge[2][1];
            EdgeTest[11] = EdgeTest[10] + Edge[2][0] + Edge[2][1];
            EdgeTest[12] = EdgeTest[10] - Edge[2][0] - Edge[2][1];
            EdgeTest[13] = EdgeTest[10] + Edge[2][0] - Edge[2][1];
            EdgeTest[14] = EdgeTest[10] - Edge[2][0] + Edge[2][1];

            LastEffectStatus = EffectStatus;

            EdgeStatus[0] = EdgeTest[1]>=0 | EdgeTest[2]>=0 | EdgeTest[3]>=0 | EdgeTest[4]>=0;
            EdgeStatus[1] = EdgeTest[6]>=0 | EdgeTest[7]>=0 | EdgeTest[8]>=0 | EdgeTest[9]>=0;
            EdgeStatus[2] = EdgeTest[11]>=0 | EdgeTest[12]>=0 | EdgeTest[13]>=0 | EdgeTest[14]>=0;
            EffectStatus = EdgeStatus[0] & EdgeStatus[1] & EdgeStatus[2];
            if (EffectStatus)
            {
                RowStatus = true;
                vcount++;
                validstamp++;
                scanpixel+=4;

                EdgeTest[1] = EdgeTest[0] - Edge[0][0]/2 + Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[5] - Edge[1][0]/2 + Edge[1][1]/2;
                EdgeTest[3] = EdgeTest[10] - Edge[2][0]/2 + Edge[2][1]/2;
                EffectDraw[0] = EdgeTest[1]>=0 & EdgeTest[2]>=0 & EdgeTest[3]>=0;

                EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 + Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[5] + Edge[1][0]/2 + Edge[1][1]/2;
                EdgeTest[3] = EdgeTest[10] + Edge[2][0]/2 + Edge[2][1]/2;
                EffectDraw[1] = EdgeTest[1]>=0 & EdgeTest[2]>=0 & EdgeTest[3]>=0;

                EdgeTest[1] = EdgeTest[0] - Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[5] - Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[3] = EdgeTest[10] - Edge[2][0]/2 - Edge[2][1]/2;
                EffectDraw[2] = EdgeTest[1]>=0 & EdgeTest[2]>=0 & EdgeTest[3]>=0;

                EdgeTest[1] = EdgeTest[0] + Edge[0][0]/2 - Edge[0][1]/2;
                EdgeTest[2] = EdgeTest[5] + Edge[1][0]/2 - Edge[1][1]/2;
                EdgeTest[3] = EdgeTest[10] + Edge[2][0]/2 - Edge[2][1]/2;
                EffectDraw[3] = EdgeTest[1]>=0 & EdgeTest[2]>=0 & EdgeTest[3]>=0;

                if (EffectDraw[0])
                {
                    validpixel++;
                    EffectPixBuffer[EffectP][0] = (x<<1);
                    EffectPixBuffer[EffectP][1] = (y<<1);
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + plane[a-2][0]*(EffectPixBuffer[EffectP][0]+0.5 - vertexA[0])
                                                      + plane[a-2][1]*(EffectPixBuffer[EffectP][1]+0.5 - vertexA[1]);


                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }

                if (EffectDraw[1])
                {
                    validpixel++;
                    EffectPixBuffer[EffectP][0] = (x<<1)+1;
                    EffectPixBuffer[EffectP][1] = (y<<1);
                    for (a = 2;a <= 12;a++)

                        EffectPixBuffer[EffectP][a] = vertexA[a] + plane[a-2][0]*(EffectPixBuffer[EffectP][0]+0.5 - vertexA[0])
                                                      + plane[a-2][1]*(EffectPixBuffer[EffectP][1]+0.5 - vertexA[1]);


                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }

                if (EffectDraw[2])
                {
                    validpixel++;
                    EffectPixBuffer[EffectP][0] = (x<<1);
                    EffectPixBuffer[EffectP][1] = (y<<1)+1;
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + plane[a-2][0]*(EffectPixBuffer[EffectP][0]+0.5 - vertexA[0])
                                                      + plane[a-2][1]*(EffectPixBuffer[EffectP][1]+0.5 - vertexA[1]);


                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }

                if (EffectDraw[3])
                {
                    validpixel++;
                    EffectPixBuffer[EffectP][0] = (x<<1)+1;
                    EffectPixBuffer[EffectP][1] = (y<<1)+1;
                    for (a = 2;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = vertexA[a] + plane[a-2][0]*(EffectPixBuffer[EffectP][0]+0.5 - vertexA[0])
                                                      + plane[a-2][1]*(EffectPixBuffer[EffectP][1]+0.5 - vertexA[1]);


                    for (a = 4;a <= 12;a++)
                        EffectPixBuffer[EffectP][a] = EffectPixBuffer[EffectP][a]/EffectPixBuffer[EffectP][3];

                    if (PIXEL_GENERATE_DEBUG&PIXEL_DATA_DEBUG)
                        fprintf(PIXEL_GENERATE_DEBUGfp,"Postition:(%3d,%3d,%6.2f),Color:(%03x %03x %03x %4.2f)\n",
                                (int)EffectPixBuffer[EffectP][0],(int)EffectPixBuffer[EffectP][1],EffectPixBuffer[EffectP][2],
                                (int)EffectPixBuffer[EffectP][8],(int)EffectPixBuffer[EffectP][9],(int)EffectPixBuffer[EffectP][10],EffectPixBuffer[EffectP][11]);
                    EffectP++;
                }
            }
            else
            {
                invalidstamp++;
                if ((LastEffectStatus == true)&(EffectStatus == false))
                    break;
                if (vcount == 0)
                    coldscan++;
            }
        }
        if ((LastRowStatus == true)&(RowStatus == false))
            break;
    }
    if (SCAN_ALGO_DEBUG)
    {
        triCount++;
        fprintf(SCAN_ALGO_DEBUGfp,"------- A Tile begin at (%d,%d) -------\n",CurWidth,CurHeight);
        fprintf(SCAN_ALGO_DEBUGfp,"Tile Bondary Box:  X:(%d~%d)  Y:(%d~%d)\n",LX,RX,LY,HY);
        fprintf(SCAN_ALGO_DEBUGfp,"Valid pixel: %d\n",vcount);
        fprintf(SCAN_ALGO_DEBUGfp,"Scaned pixel: %d\n",count);
        fprintf(SCAN_ALGO_DEBUGfp,"Average Scan Rate in this Tile: %2.2f\n", (float)vcount/count);
        fprintf(SCAN_ALGO_DEBUGfp,"triCount: %d\n",triCount);
        fprintf(SCAN_ALGO_DEBUGfp,"---------------------------------------\n");

    }

    if (PIXEL_GENERATE_DEBUG)
        if (vcount > 0)
        {
            fprintf(PIXEL_GENERATE_DEBUGfp,"Valid pixel: %d\n",vcount);
            fprintf(PIXEL_GENERATE_DEBUGfp,"Scaned pixel: %d\n",count);
            fprintf(PIXEL_GENERATE_DEBUGfp,"Tile Bondary Box:  X:(%d~%d)  Y:(%d~%d)\n",LX,RX,LY,HY);
            fprintf(PIXEL_GENERATE_DEBUGfp,"/*************A Tile begin at (%d,%d)**************/\n\n",CurWidth,CurHeight);
        }
#endif

}

U32 Rasterizer::CalcTexAdd(short int us,short int ub,short int uo,short int vs,short int vb,short int vo)
{
    return (vs*TEX_CACHE_BLOCK_SIZE_ROOT*TEX_CACHE_ENTRY_Y+vb*TEX_CACHE_BLOCK_SIZE_ROOT+vo)*TexWidth+
           us*TEX_CACHE_BLOCK_SIZE_ROOT*TEX_CACHE_ENTRY_X+ub*TEX_CACHE_BLOCK_SIZE_ROOT+uo;
}

U32 Rasterizer::GetTexColor(const unsigned short u, const unsigned short v)
{
    int i,j;
    unsigned short tag, pos_cache, pos_block, U_Block, V_Block, U_Offset, V_Offset, U_Super, V_Super;

    U_Super = u >> (TEX_CACHE_BLOCK_SIZE_ROOT_LOG + TEX_CACHE_ENTRY_X_LOG);
    V_Super = v >> (TEX_CACHE_BLOCK_SIZE_ROOT_LOG + TEX_CACHE_ENTRY_Y_LOG);
    U_Block = (u >> TEX_CACHE_BLOCK_SIZE_ROOT_LOG) & (TEX_CACHE_ENTRY_X - 1);
    V_Block = (v >> TEX_CACHE_BLOCK_SIZE_ROOT_LOG) & (TEX_CACHE_ENTRY_Y - 1);
    U_Offset = u & (TEX_CACHE_BLOCK_SIZE_ROOT - 1);
    V_Offset = v & (TEX_CACHE_BLOCK_SIZE_ROOT - 1);
    tag = (int)(V_Super << 8)|(U_Super&0x00ff);
    pos_cache = V_Block * TEX_CACHE_ENTRY_X + U_Block;
    pos_block = V_Offset * TEX_CACHE_BLOCK_SIZE_ROOT + U_Offset;

    ///*************** Texture cache hit *************
    if ((TexCache.valid[pos_cache] == true) && (TexCache.pos[pos_cache] == tag))
    {
        TexCache.TexCacheHit++;
        return TexCache.color[pos_cache][pos_block];
    }
    else///*********** Texture cache miss *************
    {
        TexCache.TexCacheMiss++;
        if (TexCache.valid[pos_cache] == false)
            TexCache.TexCacheColdMiss++;
        TexCache.valid[pos_cache] = true;
        TexCache.pos[pos_cache] = tag;
        for (j = 0;j < TEX_CACHE_BLOCK_SIZE_ROOT;j++)
        {
            for (i = 0;i < TEX_CACHE_BLOCK_SIZE_ROOT;i++)
            {
                U8 *texPtr;
                U8 texel[4];

                if (TextureInternalFormat == GL_RGB)
                {
                    texPtr = TEXfp+CalcTexAdd(U_Super,U_Block,i,V_Super,V_Block,j)*3;

                    texel[0] = *texPtr++; //R
                    texel[1] = *texPtr++; //G
                    texel[2] = *texPtr++; //B
                    texel[3] = 0xff;

                    TexCache.color[pos_cache][j*TEX_CACHE_BLOCK_SIZE_ROOT+i] = (texel[0] << 24) | (texel[1] << 16) | (texel[2] << 8 | texel[3]);
                }
                else if (TextureInternalFormat == GL_RGBA)
                {
                    texPtr = TEXfp+CalcTexAdd(U_Super,U_Block,i,V_Super,V_Block,j)*4;

                    texel[0] = *texPtr++; //R
                    texel[1] = *texPtr++; //G
                    texel[2] = *texPtr++; //B
                    texel[3] = *texPtr++; //A
                    //printf("R:%X, A:%X\n", texel[0], texel[3]);
                    TexCache.color[pos_cache][j*TEX_CACHE_BLOCK_SIZE_ROOT+i] = (texel[0] << 24) | (texel[1] << 16) | (texel[2] << 8 | texel[3]);
                }
                else if (TextureInternalFormat == GL_LUMINANCE)
                {
                    texPtr = TEXfp+CalcTexAdd(U_Super,U_Block,i,V_Super,V_Block,j)*1;

                    texel[0] = *texPtr++; //L
                    texel[1] = texel[0];  //
                    texel[2] = texel[0];  //
                    texel[3] = 0xff;

                    TexCache.color[pos_cache][j*TEX_CACHE_BLOCK_SIZE_ROOT+i] = (texel[0] << 24) | (texel[1] << 16) | (texel[2] << 8 | texel[3]);
                }
            }
        }
        textureBW += 64;

        return TexCache.color[pos_cache][pos_block];
    }
}

void Rasterizer::TextureMapping()
{
    int i;
    float TexU, TexV;
    U8 TexUC, TexVC;
    float u_ratio, v_ratio;
    U32 TexColor[2][2];
    U8 color[4];

    if (TEXDEBUG)
        fprintf(TEXDEBUGfp,"\nXXX--YYY--UU.UUUU--VV.VVVV----------------\n");
    for (i = 0;i < EffectP;i++)
    {
        //find absolutely position in texture image
        TexU = EffectPixBuffer[i][4]*TexWidth;
        TexV = EffectPixBuffer[i][5]*TexHeight;

        if (TEXDEBUG)
            fprintf(TEXDEBUGfp,"%3d  %3d  %7.7f  %7.7f\n",
                    (int)EffectPixBuffer[i][0],(int)EffectPixBuffer[i][1],TexU,TexV);

        if (TexU < 0)
        {
            printf("OU:%4.10f->TexU:%4.10f\n", EffectPixBuffer[i][4], TexU);
            TexU = 0;
        }
        if (TexV < 0)
        {
            printf("OV:%4.10f->TexV:%4.10f\n", EffectPixBuffer[i][5], TexV);
            TexV = 0;
        }

        ///*********Texture Wrap mode***************************
        if (TexWrapModeS == GL_REPEAT)
            TexU = fmod(TexU,TexWidth);
        //fmod: mod in floating point format, %(mod) can only be used under integer format
        else if (TexWrapModeS == GL_CLAMP_TO_EDGE)
            TexU = (TexU < TexWidth-1)?((TexU > 0)?TexU:0):TexWidth-1;
        //0 <= TexU <= TexWidth-1
        else
            fprintf(stderr,"Wrong Texture Wrap mode in x-axis!!");

        if (TexWrapModeT == GL_REPEAT)
            TexV = fmod(TexV,TexHeight);
        else if (TexWrapModeT == GL_CLAMP_TO_EDGE)
            TexV = (TexV < TexHeight-1)?((TexV > 0)?TexV:0):TexHeight-1;
        //0 <= TexV <= TexHeight-1
        else
            fprintf(stderr,"%x Wrong Texture Wrap mode %x in y-axis!!\n",&TexWrapModeT,TexWrapModeT);
        ///*******************************************************
        if (TEXDEBUG)
            fprintf(TEXDEBUGfp,"%3d  %3d  %7.4f  %7.4f",
                    (int)EffectPixBuffer[i][0],(int)EffectPixBuffer[i][1],TexU,TexV);

        //nearest filter
        if (TexFilterMode == GL_NEAREST)
        {
            TexColor[0][0] = GetTexColor((unsigned short)TexU, (unsigned short)TexV);
            color[0] = (TexColor[0][0]&0xff000000) >> 24;// R
            color[1] = (TexColor[0][0]&0x00ff0000) >> 16;// G
            color[2] = (TexColor[0][0]&0x0000ff00) >> 8; // B
            color[3] = TexColor[0][0]&0x000000ff;      // A
        }
        //bilinear filter
        else if (TexFilterMode == GL_LINEAR)
        {
            TexUC = (unsigned short)floor(TexU);// floor: Round down value
            TexVC = (unsigned short)floor(TexV);

            if (TEXDEBUG) fprintf(TEXDEBUGfp," (TexUC:%d, TexVC:%d) ", TexUC, TexVC);

            u_ratio = TexU - TexUC;
            v_ratio = TexV - TexVC;
            TexColor[0][0] = GetTexColor(TexUC, TexVC);
            TexColor[1][0] = GetTexColor(((TexUC+1)%TexWidth), TexVC);
            TexColor[0][1] = GetTexColor(TexUC, ((TexVC+1)%TexHeight));
            TexColor[1][1] = GetTexColor(((TexUC+1)%TexWidth), ((TexVC+1)%TexHeight));
            color[0] = (((TexColor[0][0]&0xff000000) >> 24)*(1-u_ratio)+
                        ((TexColor[1][0]&0xff000000) >> 24)*u_ratio)*(1-v_ratio)+
                       (((TexColor[0][1]&0xff000000) >> 24)*(1-u_ratio)+
                        ((TexColor[1][1]&0xff000000) >> 24)*u_ratio)*(v_ratio);
            color[1] = (((TexColor[0][0]&0x00ff0000) >> 16)*(1-u_ratio)+
                        ((TexColor[1][0]&0x00ff0000) >> 16)*u_ratio)*(1-v_ratio)+
                       (((TexColor[0][1]&0x00ff0000) >> 16)*(1-u_ratio)+
                        ((TexColor[1][1]&0x00ff0000) >> 16)*u_ratio)*(v_ratio);
            color[2] = (((TexColor[0][0]&0x0000ff00) >> 8)*(1-u_ratio)+
                        ((TexColor[1][0]&0x0000ff00) >> 8)*u_ratio)*(1-v_ratio)+
                       (((TexColor[0][1]&0x0000ff00) >> 8)*(1-u_ratio)+
                        ((TexColor[1][1]&0x0000ff00) >> 8)*u_ratio)*(v_ratio);
            color[3] = ((TexColor[0][0]&0x000000ff)*(1-u_ratio)+
                        (TexColor[1][0]&0x000000ff)*u_ratio)*(1-v_ratio)+
                       ((TexColor[0][1]&0x000000ff)*(1-u_ratio)+
                        (TexColor[1][1]&0x000000ff)*u_ratio)*(v_ratio);
        }
        else
            fprintf(stderr,"Wrong Texture filter mode!!");

        if (TEXDEBUG)
            fprintf(TEXDEBUGfp,"  %2x %2x %2x %2x",color[0],color[1],color[2],color[3]);

        ///********** Texture Environment mode ***********
        switch (TexEnvMode)
        {
        case GL_MODULATE:
            EffectPixBuffer[i][8] = (int)(EffectPixBuffer[i][8]*color[0])>>8;
            EffectPixBuffer[i][9] = (int)(EffectPixBuffer[i][9]*color[1])>>8;
            EffectPixBuffer[i][10] = (int)(EffectPixBuffer[i][10]*color[2])>>8;
            if (TextureInternalFormat == GL_RGBA)
                EffectPixBuffer[i][11] = (int)(EffectPixBuffer[i][11]*color[3])>>8;
            break;
        case GL_REPLACE:
            EffectPixBuffer[i][8] = color[0];
            EffectPixBuffer[i][9] = color[1];
            EffectPixBuffer[i][10] = color[2];
            if (TextureInternalFormat == GL_RGBA)
                EffectPixBuffer[i][11] = color[3];
            break;
        case GL_DECAL:
            if (TextureInternalFormat == GL_RGB)
            {
                EffectPixBuffer[i][8] = color[0];
                EffectPixBuffer[i][9] = color[1];
                EffectPixBuffer[i][10] = color[2];
            }
            else if (TextureInternalFormat == GL_RGBA)
            {
                EffectPixBuffer[i][8] = EffectPixBuffer[i][8]*(~color[3])+color[0]*color[3];
                EffectPixBuffer[i][9] = EffectPixBuffer[i][9]*(~color[3])+color[1]*color[3];
                EffectPixBuffer[i][10] = EffectPixBuffer[i][10]*(~color[3])+color[2]*color[3];
            }
            break;
        case GL_ADD:
            EffectPixBuffer[i][8] = ((EffectPixBuffer[i][8]+color[0]) > 255)?255:(EffectPixBuffer[i][8]+color[0]);
            EffectPixBuffer[i][9] = ((EffectPixBuffer[i][9]+color[1]) > 255)?255:(EffectPixBuffer[i][9]+color[1]);
            EffectPixBuffer[i][10] = ((EffectPixBuffer[i][10]+color[2]) > 255)?255:(EffectPixBuffer[i][10]+color[2]);
            if (TextureInternalFormat == GL_RGBA)
                EffectPixBuffer[i][11] = (int)(EffectPixBuffer[i][11]*color[3])>>8;
            break;
        case GL_BLEND:
            EffectPixBuffer[i][8] = (int)(EffectPixBuffer[i][8]*(~color[0]))>>8 + ((color[0]*TexEnvColor[0])>>8);
            EffectPixBuffer[i][9] = (int)(EffectPixBuffer[i][9]*(~color[1]))>>8 + ((color[1]*TexEnvColor[1])>>8);
            EffectPixBuffer[i][10] = (int)(EffectPixBuffer[i][10]*(~color[2]))>>8 + ((color[2]*TexEnvColor[2])>>8);
            if (TextureInternalFormat == GL_RGBA)
                EffectPixBuffer[i][11] = (int)(EffectPixBuffer[i][11]*color[3])>>8;
            break;
        default:
            fprintf(stderr,"Wrong Texture environment mode!!");
            break;
        }

        if (TEXDEBUG)
            fprintf(TEXDEBUGfp,"  %2x %2x %2x %2x\n",
                    (int)EffectPixBuffer[i][8],(int)EffectPixBuffer[i][9],(int)EffectPixBuffer[i][10],(int)EffectPixBuffer[i][11]);
    }
}

void Rasterizer::PerFragmentOp()
{
    int i,j,a;
    bool AlphaTest = true;
    bool DepthTest = true;
    for (i = 0;i < EffectP;i++)
    {
        ///**********Fog**********************
        if (FogEn)
        {
            EffectPixBuffer[i][8] = EffectPixBuffer[i][8]*EffectPixBuffer[i][12]+(1-EffectPixBuffer[i][12])*FogColor[0];
            EffectPixBuffer[i][9] = EffectPixBuffer[i][9]*EffectPixBuffer[i][12]+(1-EffectPixBuffer[i][12])*FogColor[1];
            EffectPixBuffer[i][10] = EffectPixBuffer[i][10]*EffectPixBuffer[i][12]+(1-EffectPixBuffer[i][12])*FogColor[2];
            EffectPixBuffer[i][11] = EffectPixBuffer[i][11]*EffectPixBuffer[i][12]+(1-EffectPixBuffer[i][12])*FogColor[3];
        }

        ///**********Alpha Test*****************
        if (AlphaTestEn == true)
        {
            if (AlphaTestMode == GL_NEVER)
                AlphaTest = false;
            else if (AlphaTestMode == GL_LESS)
                AlphaTest = EffectPixBuffer[i][11] < AlphaRef;
            else if (AlphaTestMode == GL_EQUAL)
                AlphaTest = EffectPixBuffer[i][11] == AlphaRef;
            else if (AlphaTestMode == GL_LEQUAL)
                AlphaTest = EffectPixBuffer[i][11] <= AlphaRef;
            else if (AlphaTestMode == GL_GREATER)
                AlphaTest = EffectPixBuffer[i][11] > AlphaRef;
            else if (AlphaTestMode == GL_NOTEQUAL)
                AlphaTest = EffectPixBuffer[i][11] != AlphaRef;
            else if (AlphaTestMode == GL_GEQUAL)
                AlphaTest = EffectPixBuffer[i][11] >= AlphaRef;
            else if (AlphaTestMode == GL_ALWAYS)
                AlphaTest = true;
            else
                fprintf(stderr,"Wrong Alpha Test mode!!\n");
            if (AlphaTest == false)
                continue;
        }

        ///****************Depth test*********************
        if (DepthTestEn == true)
        {
            if (DepthTestMode == GL_NEVER)
                DepthTest = false;
            else if (DepthTestMode == GL_LESS)
                DepthTest = EffectPixBuffer[i][2] < TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_EQUAL)
                DepthTest = EffectPixBuffer[i][2] == TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_LEQUAL)
                DepthTest = EffectPixBuffer[i][2] <= TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_GREATER)
                DepthTest = EffectPixBuffer[i][2] > TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_NOTEQUAL)
                DepthTest = EffectPixBuffer[i][2] != TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_GEQUAL)
                DepthTest = EffectPixBuffer[i][2] >= TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth];
            else if (DepthTestMode == GL_ALWAYS)
                DepthTest = true;
            else
                fprintf(stderr,"Wrong Depth Test mode!!\n");

            if (DepthTest == false)
                continue;
            else
                TDepthBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth] = EffectPixBuffer[i][2];
        }

        /***************Alpha Blending**************/
        if (BlendingEn == true)
        {
//            if (frame_count==137 && tileDEG)
//                blendCount++;

            switch (BlendingSrcMode)
            {
            case GL_ZERO:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = 0;
                break;
            case GL_ONE:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = 0xff;
                break;
            case GL_DST_COLOR:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][a];
                break;
            case GL_ONE_MINUS_DST_COLOR:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = ~TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][a];
                break;
            case GL_SRC_ALPHA:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = (unsigned char)EffectPixBuffer[i][11];
                break;
            case GL_ONE_MINUS_SRC_ALPHA:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = ~((unsigned char)EffectPixBuffer[i][11]);
                break;
            case GL_DST_ALPHA:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3];
                break;
            case GL_ONE_MINUS_DST_ALPHA:
                for (a=0;a<4;a++)
                    BlendingSrc[a] = ~TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3];
                break;
            case GL_SRC_ALPHA_SATURATE:
                for (a=0;a<3;a++)
                {
                    BlendingSrc[a] = std::min((unsigned char)EffectPixBuffer[i][11],
                                              (unsigned char)~TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3]);
                }
                BlendingSrc[3] = 0xff;
                break;
            default:
                fprintf(stderr,"Wrong Blending Function in Src Argument!!\n");
                break;
            }
            switch (BlendingDstMode)
            {
            case GL_ZERO:
                for (a=0;a<4;a++)
                    BlendingDst[a] = 0;
                break;
            case GL_ONE:
                for (a=0;a<4;a++)
                    BlendingDst[a] = 0xff;
                break;
            case GL_SRC_COLOR:
                for (a=0;a<4;a++)
                    BlendingDst[a] = (unsigned char)EffectPixBuffer[i][a+8];
                break;
            case GL_ONE_MINUS_SRC_COLOR:
                for (a=0;a<4;a++)
                    BlendingDst[a] = ~(unsigned char)EffectPixBuffer[i][a+8];;
                break;
            case GL_SRC_ALPHA:
                for (a=0;a<4;a++)
                    BlendingDst[a] = (unsigned char)EffectPixBuffer[i][11];
                break;
            case GL_ONE_MINUS_SRC_ALPHA:
                for (a=0;a<4;a++)
                    BlendingDst[a] = ~((unsigned char)EffectPixBuffer[i][11]);
                break;
            case GL_DST_ALPHA:
                for (a=0;a<4;a++)
                    BlendingDst[a] = TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3];
                break;
            case GL_ONE_MINUS_DST_ALPHA:
                for (a=0;a<4;a++)
                    BlendingDst[a] = ~TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3];
                break;
            default:
                fprintf(stderr,"Wrong Blending Function in Dst Argument!!\n");
                break;
            }
            for (a=0;a<4;a++)
            {
                EffectPixBuffer[i][a+8] = (((int)EffectPixBuffer[i][a+8]*BlendingSrc[a])>>8)+
                                          ((TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][a]*BlendingDst[a])>>8);

                EffectPixBuffer[i][a+8] = (EffectPixBuffer[i][a+8]>=255.0) ? 255.0 : EffectPixBuffer[i][a+8];
            }
        }

        ///*************Update tile color buffer**********************
        TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][0]
        = (unsigned char)EffectPixBuffer[i][8];// R
        TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][1]
        = (unsigned char)EffectPixBuffer[i][9];// G
        TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][2]
        = (unsigned char)EffectPixBuffer[i][10];// B
        TColorBuffer[(int)EffectPixBuffer[i][1]-CurHeight][(int)EffectPixBuffer[i][0]-CurWidth][3]
        = (unsigned char)EffectPixBuffer[i][11];// A

        ++shadedpixel;
    }
}

void Rasterizer::ClearStatistics()
{
    int i,j;
    for (i = 0;i < TEX_CACHE_ENTRY_X*TEX_CACHE_ENTRY_Y;i++)
        TexCache.valid[i] = false;
//    for (j = 0;j < WINDOW_DEFAULT_HEIGHT;j++)
//        for (i = 0;i < WINDOW_DEFAULT_WIDTH;i++)
//        {
//            ColorBuffer[j][i][0] = 0;
//            ColorBuffer[j][i][1] = 0;
//            ColorBuffer[j][i][2] = 0;
//            ColorBuffer[j][i][3] = 0;
//        }

    TexCache.TexCacheHit = 0;
    TexCache.TexCacheMiss = 0;
    TexCache.TexCacheColdMiss = 0;

    validstamp = 0;
    scanstamp = 0;
    coldscan = 0;
    invalidstamp = 0;
    validpixel = 0;
    scanpixel = 0;
    shadedpixel = 0;
    depthBufferBW[0] = 0;
    depthBufferBW[1] = 0;
    colorBufferBW[0] = 0;
    colorBufferBW[1] = 0;
    textureBW = 0;

    triCount = 0;
    CurWidth = 0;
    CurHeight = 0;
}

void Rasterizer::SetStateReg(unsigned short id, unsigned int value)
{
    switch (id)
    {
    case GL_BLEND:
        BlendingEn = (bool)value;
        break;

    case GL_BLEND_SRC:
        BlendingSrcMode = value;
        break;

    case GL_BLEND_DST:
        BlendingDstMode = value;
        break;

    case GL_FOG:
        FogEn = (bool)value;
        break;

    case GL_FOG_COLOR:
        FogColor[0] = (value&0xff000000)>>24;
        FogColor[1] = (value&0x00ff0000)>>16;
        FogColor[2] = (value&0x0000ff00)>>8;
        FogColor[3] = (value&0x000000ff);
        //printf("FogColor[0]:%d, FogColor[1]:%d, FogColor[2]:%d, FogColor[3]:%d\n", FogColor[0], FogColor[1], FogColor[2], FogColor[3]);
        break;

    case GL_DEPTH_TEST:
        DepthTestEn = (bool)value;
        break;

    case GL_DEPTH_FUNC:
        DepthTestMode = value;
        break;

    case GL_ALPHA_TEST:
        AlphaTestEn = (bool)value;
        break;

    case GL_ALPHA_TEST_FUNC:
        AlphaTestMode = value;
        break;

    case GL_ALPHA_TEST_REF:
        AlphaRef = (value);
        if (AlphaRef >= 256)
            AlphaRef = 255;
        //printf("GL_ALPHA_TEST_REF:%X\n", AlphaRef);
        //AlphaRef = __GL_S_2_FLOAT(value);
        break;

    case GL_SCISSOR_TEST:
        ScissorTestEn = (bool)value;
        break;

    case GL_STENCIL_TEST:
        StencilTestEn = (bool)value;
        break;

    case GL_COLOR_LOGIC_OP:
        LogicOpTestEn = (bool)value;
        break;

    case GL_TEXTURE_2D:
        TexMappingEn = (bool)value;
        break;

    case GL_TEXTURE_HEIGHT:
        TexHeight = value;
        //printf("GL_TEXTURE_HEIGHT:%d\n", value);
        break;

    case GL_TEXTURE_WIDTH:
        TexWidth = value;
        //printf("GL_TEXTURE_WIDTH:%d\n", value);
        break;

    case GL_TEXTURE_FILTER:
        TexFilterMode = value;
        //printf("GL_TEXTURE_FILTER:%X\n", value);
        break;

    case GL_TEXTURE_WRAP_S:
        TexWrapModeS = value;
        //printf("GL_TEXTURE_WRAP_S:%X\n", value);
        break;

    case GL_TEXTURE_WRAP_T:
        TexWrapModeT = value;
        //printf("GL_TEXTURE_WRAP_T:%X\n", value);
        break;

    case GL_TEXTURE_ENV_MODE:
        TexEnvMode = value;
        //printf("GL_TEXTURE_ENV_MODE:%X\n", value);
        break;

    case GL_TEXTURE_BASE:
        TEXfp = (U8*)(value);
        break;

    case GL_TEXTURE_ENV_COLOR:
        TexEnvColor[0] = (value&0xff000000)>>24;
        TexEnvColor[1] = (value&0x00ff0000)>>16;
        TexEnvColor[2] = (value&0x0000ff00)>>8;
        TexEnvColor[3] = (value&0x000000ff);
        //printf("TexEnvColor[0]:%d, TexEnvColor[1]:%d, TexEnvColor[2]:%d, TexEnvColor[3]:%d\n", TexEnvColor[0], TexEnvColor[1], TexEnvColor[2], TexEnvColor[3]);
        break;

    case GL_TEXTURE_FORMAT:
        TextureInternalFormat = value;
        break;

    case GL_MULTISAMPLE:
        MultiSampleEn = (bool)value;
        break;

    case GL_COLOR_CLEAR_VALUE:
        ColorClearVal[0] = (value&0xff000000)>>24;
        ColorClearVal[1] = (value&0x00ff0000)>>16;
        ColorClearVal[2] = (value&0x0000ff00)>>8;
        ColorClearVal[3] = (value&0x000000ff);
        break;

    case GL_DEPTH_CLEAR_VALUE:

        break;

    default:
        printf("Undefined ID:0X%04X\n", id);
        break;
    }
}

void Rasterizer::ClearBuffer(unsigned int mask)
{
    if (mask & GL_COLOR_BUFFER_BIT)
    {
        for (int i = 0;i<TILEHEIGHT*2;i++)
            for (int j = 0;j<TILEWIDTH*2;j++)
            {
                TColorBuffer[i][j][0] = ColorClearVal[0];
                TColorBuffer[i][j][1] = ColorClearVal[1];
                TColorBuffer[i][j][2] = ColorClearVal[2];
                TColorBuffer[i][j][3] = ColorClearVal[3];
            }
    }

    if (mask & GL_DEPTH_BUFFER_BIT)
    {
        for (int i = 0;i<TILEHEIGHT*2;i++)
            for (int j = 0;j<TILEWIDTH*2;j++)
            {
                TDepthBuffer[i][j] = DepthRef;
            }
    }

    if (mask & GL_STENCIL_BUFFER_BIT)
    {

    }
}

//this bmp format doesn't include compression method and palette
void Rasterizer::DumpImage(int ImageType, char *filename)
{
    FILE *CLRfp;
    int i,j;

    if (ImageType == 1)
    {
        CLRfp = fopen(filename,"wb");
        for (i=0;i<54;i++)
        {
            switch (i)
            {
            default:
                putc (0x00, CLRfp);
                break;
            case 0:
                putc (0x42, CLRfp);
                break;
            case 1:
                putc (0x4D, CLRfp);
                break;
            case 2:
                putc (0x38, CLRfp);
                break;
            case 3:
                putc (0x10, CLRfp);
                break;
            case 0xA:
                putc (0x36, CLRfp);
                break;
            case 0xE:
                putc (0x28, CLRfp);
                break;
            case 0x12:
                putc (WINDOW_DEFAULT_WIDTH&0x000000FF, CLRfp);
                break;
            case 0x13:
                putc ((WINDOW_DEFAULT_WIDTH>>8)&0x000000FF, CLRfp);
                break;
            case 0x14:
                putc ((WINDOW_DEFAULT_WIDTH>>16)&0x000000FF, CLRfp);
                break;
            case 0x15:
                putc ((WINDOW_DEFAULT_WIDTH>>24)&0x000000FF, CLRfp);
                break;
            case 0x16:
                putc (WINDOW_DEFAULT_HEIGHT&0x000000FF, CLRfp);
                break;
            case 0x17:
                putc ((WINDOW_DEFAULT_HEIGHT>>8)&0x000000FF, CLRfp);
                break;
            case 0x18:
                putc ((WINDOW_DEFAULT_HEIGHT>>16)&0x000000FF, CLRfp);
                break;
            case 0x19:
                putc ((WINDOW_DEFAULT_HEIGHT>>24)&0x000000FF, CLRfp);
                break;
            case 0x1A:
                putc (0x01, CLRfp);
                break;
            case 0x1C:
                putc (0x18, CLRfp);
                break;
            case 0x26:
                putc (0x12, CLRfp);
                break;
            case 0x27:
                putc (0x0B, CLRfp);
                break;
            case 0x2A:
                putc (0x12, CLRfp);
                break;
            case 0x2B:
                putc (0x0B, CLRfp);
                break;
            }
        }
        for (i=0;i<WINDOW_DEFAULT_HEIGHT;i++)
            for (j=0;j<WINDOW_DEFAULT_WIDTH;j++)
            {
                putc((int)ColorBuffer[i][j][2], CLRfp);// B
                putc((int)ColorBuffer[i][j][1], CLRfp);// G
                putc((int)ColorBuffer[i][j][0], CLRfp);// R
            }
        fclose(CLRfp);
    }
    else if (ImageType == 2)
    {
        CLRfp = fopen(filename,"wb");
        for (i=WINDOW_DEFAULT_HEIGHT-1;i>=0;i--)
            for (j=0;j<WINDOW_DEFAULT_WIDTH;j++)
            {
                putc((int)ColorBuffer[i][j][0], CLRfp);// R
                putc((int)ColorBuffer[i][j][1], CLRfp);// G
                putc((int)ColorBuffer[i][j][2], CLRfp);// B
                putc((int)(DepthBuffer[i][j]*255), CLRfp);
            }
        fclose(CLRfp);
    }
}

void Rasterizer::DumpDepthImage(int ImageType, char *filename)
{
    FILE *DPHfp;
    int i,j;
    if (ImageType == 1)
    {
        DPHfp = fopen(filename,"wb");
        for (i=0;i<54;i++)
        {
            switch (i)
            {
            default:
                putc (0x00, DPHfp);
                break;
            case 0:
                putc (0x42, DPHfp);
                break;
            case 1:
                putc (0x4D, DPHfp);
                break;
            case 2:
                putc (0x38, DPHfp);
                break;
            case 3:
                putc (0x10, DPHfp);
                break;
            case 0xA:
                putc (0x36, DPHfp);
                break;
            case 0xE:
                putc (0x28, DPHfp);
                break;
            case 0x12:
                putc (WINDOW_DEFAULT_WIDTH&0x000000FF, DPHfp);
                break;
            case 0x13:
                putc ((WINDOW_DEFAULT_WIDTH>>8)&0x000000FF, DPHfp);
                break;
            case 0x14:
                putc ((WINDOW_DEFAULT_WIDTH>>16)&0x000000FF, DPHfp);
                break;
            case 0x15:
                putc ((WINDOW_DEFAULT_WIDTH>>24)&0x000000FF, DPHfp);
                break;
            case 0x16:
                putc (WINDOW_DEFAULT_HEIGHT&0x000000FF, DPHfp);
                break;
            case 0x17:
                putc ((WINDOW_DEFAULT_HEIGHT>>8)&0x000000FF, DPHfp);
                break;
            case 0x18:
                putc ((WINDOW_DEFAULT_HEIGHT>>16)&0x000000FF, DPHfp);
                break;
            case 0x19:
                putc ((WINDOW_DEFAULT_HEIGHT>>24)&0x000000FF, DPHfp);
                break;
            case 0x1A:
                putc (0x01, DPHfp);
                break;
            case 0x1C:
                putc (0x18, DPHfp);
                break;
            case 0x26:
                putc (0x12, DPHfp);
                break;
            case 0x27:
                putc (0x0B, DPHfp);
                break;
            case 0x2A:
                putc (0x12, DPHfp);
                break;
            case 0x2B:
                putc (0x0B, DPHfp);
                break;
            }

        }
        for (i=0;i<WINDOW_DEFAULT_HEIGHT;i++)
            for (j=0;j<WINDOW_DEFAULT_WIDTH;j++)
            {
                putc(int(DepthBuffer[i][j]*255), DPHfp);// B
                putc(int(DepthBuffer[i][j]*255), DPHfp);// G
                putc(int(DepthBuffer[i][j]*255), DPHfp);// R
            }
        fclose(DPHfp);
    }
    else if (ImageType == 2)
    {
        DPHfp = fopen(filename,"wb");
        for (i=WINDOW_DEFAULT_HEIGHT-1;i>=0;i--)
            for (j=0;j<WINDOW_DEFAULT_WIDTH;j++)
                putc(int(DepthBuffer[i][j]*255), DPHfp);
        fclose(DPHfp);
    }
}

//void Rasterizer::TileInitialize()
//{
//    int i,j;
//    EffectP = 0;
//    for (i = 0;i<TILEHEIGHT;i++)
//        for (j = 0;j<TILEWIDTH;j++)
//        {
//            TDepthBuffer[i][j] = DepthRef;
//            TColorBuffer[i][j][0] = ColorClearVal[0];
//            TColorBuffer[i][j][1] = ColorClearVal[1];
//            TColorBuffer[i][j][2] = ColorClearVal[2];
//            TColorBuffer[i][j][3] = ColorClearVal[3];
//        }
//}

void Rasterizer::EndTile(U32 * ColorBuf)
{
    int i,j;
    for (i = 0;i < TILEHEIGHT;i++)
        for (j = 0;j < TILEWIDTH;j++)
        {
            U32 R,G,B;

            if ((i == 0 || j == 0) && TILELINE)
            {
                ColorBuffer[i+CurHeight][j+CurWidth][0] = R = 0;
                ColorBuffer[i+CurHeight][j+CurWidth][1] = G = 255;
                ColorBuffer[i+CurHeight][j+CurWidth][2] = B = 255;

                colorBufferBW[1] += 4;
            }
            else
            {
                if (MultiSampleEn)
                {
                    if (pattern == 0)
                    {
                        ColorBuffer[i+CurHeight][j+CurWidth][0] = R = (TColorBuffer[i][j][0]+TColorBuffer[i+TILEHEIGHT][j][0])/2;
                        ColorBuffer[i+CurHeight][j+CurWidth][1] = G = (TColorBuffer[i][j][1]+TColorBuffer[i+TILEHEIGHT][j][1])/2;
                        ColorBuffer[i+CurHeight][j+CurWidth][2] = B = (TColorBuffer[i][j][2]+TColorBuffer[i+TILEHEIGHT][j][2])/2;
                        DepthBuffer[i+CurHeight][j+CurWidth] = 1.0-(TDepthBuffer[i][j]+TDepthBuffer[i][j+TILEHEIGHT])/2;

                        colorBufferBW[1] += 4;
                        depthBufferBW[1] += 2;
                    }
                    else if (pattern >= 1)
                    {
                        ColorBuffer[i+CurHeight][j+CurWidth][0] = R =
                                    (TColorBuffer[i][j][0]+TColorBuffer[i+TILEHEIGHT][j][0]+TColorBuffer[i][j+TILEWIDTH][0]+TColorBuffer[i+TILEHEIGHT][j+TILEWIDTH][0])/4;
                        ColorBuffer[i+CurHeight][j+CurWidth][1] = G =
                                    (TColorBuffer[i][j][1]+TColorBuffer[i+TILEHEIGHT][j][1]+TColorBuffer[i][j+TILEWIDTH][1]+TColorBuffer[i+TILEHEIGHT][j+TILEWIDTH][1])/4;
                        ColorBuffer[i+CurHeight][j+CurWidth][2] = B =
                                    (TColorBuffer[i][j][2]+TColorBuffer[i+TILEHEIGHT][j][2]+TColorBuffer[i][j+TILEWIDTH][2]+TColorBuffer[i+TILEHEIGHT][j+TILEWIDTH][2])/4;
                        DepthBuffer[i+CurHeight][j+CurWidth] =
                            1.0-(TDepthBuffer[i][j]+TDepthBuffer[i][j+TILEHEIGHT]+TDepthBuffer[i+TILEWIDTH][j]+TDepthBuffer[i+TILEWIDTH][j+TILEHEIGHT])/4;

                        colorBufferBW[1] += 4;
                        depthBufferBW[1] += 2;
                    }
                }
                else
                {
                    ColorBuffer[i+CurHeight][j+CurWidth][0] = R = TColorBuffer[i][j][0];
                    ColorBuffer[i+CurHeight][j+CurWidth][1] = G = TColorBuffer[i][j][1];
                    ColorBuffer[i+CurHeight][j+CurWidth][2] = B = TColorBuffer[i][j][2];
                    DepthBuffer[i+CurHeight][j+CurWidth] = 1.0-TDepthBuffer[i][j];

                    colorBufferBW[1] += 4;
                    depthBufferBW[1] += 2;
                }
            }

            U32 color = R<<24|G<<16|B<<8|0x00;
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
            ColorBuf[(i+CurHeight)*WINDOW_DEFAULT_WIDTH+(j+CurWidth)] = color;
#endif

        }

    ///******** Next tile *******
    CurWidth = CurWidth + TILEWIDTH;
    if (CurWidth >= WINDOW_DEFAULT_WIDTH)
    {
        CurWidth = 0;
        CurHeight = CurHeight + TILEHEIGHT;
    }
}
