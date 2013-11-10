/* ---------------------------------------------------------------------------------------------------------*/
/* Like hardware
/* with culling(we disable it now) and clipping (do this before perspective division)
/* MODIFY LIGHTING  954: Color[3] = MatEmission[3] + (MatAmbient[3] * LightModelAmbient[3]);
/*                  1036: fi==1
/* MODIFY lighting with its formula
/* MODIFY VIEWPORT AND PERSPECTIVE DIVISION (VERTEX'S NORMALIZED DEVICE COORD)
/* written by Shunfan
/* ---------------------------------------------------------------------------------------------------------*/
#include "geometry.h"

Geometry gm;


//int main()
//{
//    int i, j;
//    GMdumper = fopen("Geometry.txt", "w");
//		gm_data.TriangleCounter=0;
//    gm_data.vtxIndex=0;
//    /*
//    Lighting 					= GL_LIGHTING;
//    Normalize 				= GL_NORMALIZE;
//    VertexArray 			= GL_VERTEX_ARRAY;
//    ColorArray 				= GL_COLOR_ARRAY;
//    TextureCoordArray = GL_TEXTURE_COORD_ARRAY;
//		NormalArray 			= GL_NORMAL_ARRAY;
//    CullFace 					= GL_CULL_FACE;
//    ShadeModel 				= GL_SHADE_MODEL;
//    DrawMode = 0;   		//TRIANGLE
//		posDrawValid=1;
//		Lighting = false;
//		ColorArray = true;
//		CullFace = true;		//do cull?
//    CullFaceMode=false; //0:GL_BACK; 1:GL_FRONT
//    FrontFace=true;     //true:GL_CCW; false:GL_CW
//    */
//    gm_data.initialize();
//		gm_data.do_GM();
//
//		if(FAINALLINKDEBUG)
//		{
//			triangle_node Pointer;
//			Pointer = (gm_data.triangle_list)->next_triangle;
//
//			while (Pointer!=NULL)
//			{
//				for(int address_count=0;address_count<48;address_count++)
//			    fprintf(GMdumper,"TriVtx_data= %6.3f\n",*(Pointer->VertexDATA+address_count));
//			  fprintf(GMdumper,"\n");
//
//			  tile_node Pointer2;
//				Pointer2 = (gm_data.triangle_list)->next_triangle->tiledata_ptr;
//				while (Pointer2!=NULL)
//				{
//					fprintf(GMdumper,"SerialNumber= %d   ",Pointer2->SN_and_TN>>16));
//					fprintf(GMdumper,"TileNumber  = %d\n" ,(short)Pointer2->SN_and_TN);
//					Pointer2 = Pointer2->next_tile;
//				}
//				Pointer = Pointer->next_triangle;
//				fprintf(GMdumper,"\n");
//			}
//		}
//		//system("PAUSE");
//	  return 0;
//}

Geometry::Geometry()
{
// input vertex
    InputVertex[0]=0;
    InputVertex[1]=0;
    InputVertex[2]=0;
    InputVertex[3]=1;

//--------Tranformation----------
//(1)Initialization of model-view, projection, and texture
    for (i=0; i<16; i++)
    {
        if ((i%5)==0)
        {
            ModelViewMat [i]=1;
            ProjectionMat[i]=1;
            TextureMat	 [i]=1;
        }
        else
        {
            ModelViewMat[i] =0;
            ProjectionMat[i]=0;
            TextureMat[i]	  =0;
        }
    }
//(2)inversed model-view
    for (i=0; i<9; i++)
    {
        if ((i%4)==0)
            InverseM[i]=1;
        else
            InverseM[i]=0;
    }
//--------Viewport----------
    ViewportOrigin[0]=	WINDOW_DEFAULT_WIDTH/2;
    ViewportOrigin[1]=  WINDOW_DEFAULT_HEIGHT/2;
    ViewportScale[0] =	WINDOW_DEFAULT_WIDTH/2;
    ViewportScale[1] =  WINDOW_DEFAULT_HEIGHT/2;
    DepthRangeFactor=(VPFAR-VPNEAR)/2;
    DepthRangeBase  =(VPNEAR+VPFAR)/2;
//--------normal xformation----------
    Normalize=false;
    RescaleNormal=false;
//--------shade model----------
    ShadeModel=GL_SMOOTH;   //SMOOTH =0;  FLAT =1
//--------pre_clipping---------
    pre_clipping_count = 0;
//--------face culling---------
    CullFace=false;			//do cull?
    CullFaceMode=GL_BACK; //0:GL_BACK; 1:GL_FRONT
    FrontFace=GL_CCW;     //true:GL_CCW; false:GL_CW
    cullcount=0;
//--------Lighting-------------
//(1)lighting state
    Lighting=false;

//(2)lighting parameters-material
    MatAmbient[0] =0.2;
    MatAmbient[1] =(0.2);
    MatAmbient[2] =0.2;
    MatAmbient[3] =(1.0);
    MatDiffuse[0] =0.8;
    MatDiffuse[1] =(0.8);
    MatDiffuse[2] =0.8;
    MatDiffuse[3] =(1.0);
    MatSpecular[0]=(0.0);
    MatSpecular[1]=(0.0);
    MatSpecular[2]=(0.0);
    MatSpecular[3]=(1.0);
    MatEmission[0]=(0.0);
    MatEmission[1]=(0.0);
    MatEmission[2]=(0.0);
    MatEmission[3]=(1.0);
    Shininess=(0.0); //range [0.0~128.0]
//(3)light source
    LiAmbient[0]=(0.0);
    LiAmbient[1]=(0.0);
    LiAmbient[2]=(0.0);
    LiAmbient[3]=(1.0);
    LiDiffuse[0]=(1.0);
    LiDiffuse[1]=(1.0);  //light 0
    LiDiffuse[2]=(1.0);
    LiDiffuse[3]=(1.0);
    LiSpecular[0]=(1.0);
    LiSpecular[1]=(1.0);//light 0
    LiSpecular[2]=(1.0);
    LiSpecular[3]=(1.0);
    Position[0]=(0.0);
    Position[1]=(0.0);
    Position[2]=(1.0);
    Position[3]=(0.0);

//(4)lighting model
    LightModelAmbient[0]=(0.2);
    LightModelAmbient[1]=(0.2);
    LightModelAmbient[2]=(0.2);
    LightModelAmbient[3]=(1.0);
    //LightModelTwoSide=false;
//(5)Att Spot
    Att = 1.0;
    Spot = 1.0;

//-------primitive assembly-------
    vtxIndex = 0;
    TriangleReady = 0;
    TriangleCounter = 0;
    AvailableTriangleCounter = 0;
    float TriVtxA[12]={0};
    float TriVtxB[12]={0};
    float TriVtxC[12]={0};
    float procVtxA[12]={0};
    float procVtxB[12]={0};
    float procVtxC[12]={0};
//-------clipping--------
    ClipReady = 0;
    clipcount = 0;
//-------tile counter-----
    tile_counter = 0;
//-------others-----------
    float TriVtx_data={0};
    float VertexDATA[36]={0};
    triangle_list = (triangle_node) malloc (sizeof(struct triangleData));
    if (triangle_list != NULL)
    {
        for (i=0;i<36;i++)
            triangle_list -> VertexDATA[i] = 0;
        triangle_list -> tiledata_ptr = NULL;
        triangle_list -> next_triangle = NULL;
    }
    triangle_iterator = triangle_list;
//-------clipcalid--------
    clipvalid = true;
    send_vertex=0;
    kick_small_area_count = 0;

    triangleListSize = 0;
    triangleSize = 0;

    frameDEG = false;
}

/* this task should be done by driver */
void Geometry::release_triangle_list()
{
    triangle_node tmpNode;

    if (triangle_list->next_triangle == NULL)
    {
        printf("Geometry::release_triangle_list(): Triangle List is empty.\n");
        return;
    }

    tmpNode = triangle_list->next_triangle;
    while (tmpNode != NULL)
    {
        tile_node TilePtr = tmpNode->tiledata_ptr;

        while (TilePtr != NULL)
        {
            tile_node tmpNode = TilePtr;
            TilePtr = tmpNode->next_tile;
            free(tmpNode);
        }
        triangle_list->next_triangle = tmpNode->next_triangle;
        free(tmpNode);
        tmpNode = triangle_list->next_triangle;
    }

    triangle_list->next_triangle = NULL;
}

/* For a DrawArrays command */
void Geometry::initialize()
{
    vtxIndex = 0;
    pre_clipping_count = 0;
    TriangleReady=0;
    TriangleCounter=0;
    //AvailableTriangleCounter = 0;

    int i;

    for (i=0;i<12;i++)
    {
        TriVtxA[i]=0;
        TriVtxB[i]=0;
        TriVtxC[i]=0;
        procVtxA[i]=0;
        procVtxB[i]=0;
        procVtxC[i]=0;
    }

    ClipReady = 0;
    clipvalid = true;
}

int Geometry::do_GM(void)
{
    int outIndex = 0;
    float* input[16]={NULL}, * output[16]={NULL}, ** Temporary = new float *[16];
    for (int i=0;i<16;i++)
        Temporary[i] = new float [16];

//---------------------transformation-----------------------
    transformation ();
    float tempVtx[12];
    if (TRANDEBUG||frameDEG)
    {
        fprintf(GMdumper,"\n\n");
        fprintf(GMdumper,"InputVertex = %7.7f %7.7f %7.7f %7.7f\n",InputVertex[0],InputVertex[1],InputVertex[2],InputVertex[3]);
        fprintf(GMdumper,"InputNormal = %7.7f %7.7f %7.7f\n",InputNormal[0],InputNormal[1],InputNormal[2]);
        fprintf(GMdumper,"EyeCoords = %7.7f %7.7f %7.7f %7.7f\n",EyeCoords[0],EyeCoords[1],EyeCoords[2],EyeCoords[3]);
        fprintf(GMdumper,"ClipCoords = %7.7f %7.7f %7.7f %7.7f\n",ClipCoords[0],ClipCoords[1],ClipCoords[2],ClipCoords[3]);
        fprintf(GMdumper,"EyeNormal = %7.7f %7.7f %7.7f \n",EyeNormal[0],EyeNormal[1],EyeNormal[2]);
    }

//---------------------lighting-----------------------------
    lighting();
    if (LIGHTINGDEBUG||frameDEG)
    {
        fprintf(GMdumper,"Color= %f %f %f %f (R:%d,G:%d,B:%d)\n",Color[0],Color[1],Color[2],Color[3],(int)(Color[0]*255.0), (int)(Color[1]*255.0), (int)(Color[2]*255.0));
        if (Color[0] < 0.0 || Color[0] > 1.0)
            fprintf(GMdumper,"CBUG:R=%f\n", Color[0]);
        if (Color[1] < 0.0 || Color[1] > 1.0)
            fprintf(GMdumper,"CBUG:G=%f\n", Color[1]);
        if (Color[2] < 0.0 || Color[2] > 1.0)
            fprintf(GMdumper,"CBUG:B=%f\n", Color[2]);
        if (Color[3] < 0.0 || Color[3] > 1.0)
            fprintf(GMdumper,"CBUG:A=%f\n", Color[3]);
    }

//---------------------texture_transformation---------------
    texture_transformation();
    if (TEXTRANDEBUG)
    {
        fprintf(GMdumper,"TexCoords= %f %f %f %f\n",TexCoords[0],TexCoords[1],TexCoords[2],TexCoords[3]);
    }

//---------------------primitive_assembly-------------------
    if (VTXINDEXBUG||frameDEG)
    {
        fprintf(GMdumper,"vtxIndex= %d \n",vtxIndex);
    }
    primitive_assembly();
    if (PRIMASSBUG||frameDEG)
    {
        fprintf(GMdumper,"DrawMode= %d \n",DrawMode);
        fprintf(GMdumper,"TriangleReady= %d \n",TriangleReady);
        fprintf(GMdumper,"TriangleCounter= %d \n",TriangleCounter);
    }
    if (TriangleReady)
    {
        if (TriVtxABCBUG||frameDEG)
        {
            fprintf(GMdumper,"TriVtxA= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n"
                    ,TriVtxA[0],TriVtxA[1],TriVtxA[2],TriVtxA[3]
                    ,TriVtxA[4],TriVtxA[5],TriVtxA[6],TriVtxA[7]
                    ,TriVtxA[8],TriVtxA[9],TriVtxA[10],TriVtxA[11]);

            fprintf(GMdumper,"TriVtxB= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n"
                    ,TriVtxB[0],TriVtxB[1],TriVtxB[2],TriVtxB[3]
                    ,TriVtxB[4],TriVtxB[5],TriVtxB[6],TriVtxB[7]
                    ,TriVtxB[8],TriVtxB[9],TriVtxB[10],TriVtxB[11]);

            fprintf(GMdumper,"TriVtxC= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n\n"
                    ,TriVtxC[0],TriVtxC[1],TriVtxC[2],TriVtxC[3]
                    ,TriVtxC[4],TriVtxC[5],TriVtxC[6],TriVtxC[7]
                    ,TriVtxC[8],TriVtxC[9],TriVtxC[10],TriVtxC[11]);
        }
//---------------------PRE-clipping-----------------

        pre_clipping();
//---------------------shade model------------------

        if (ShadeModel==GL_FLAT)
        {
            //fprintf(GMdumper,"FLAT OK\n");
            for (i=8; i<12; i++)
            {
                TriVtxA[i]=TriVtxC[i];
                TriVtxB[i]=TriVtxC[i];
            }
        }
        if (TriangleReady)
        {

//            if (TriVtxA[3] <0 || TriVtxB[3] <0 || TriVtxC[3] <0 )
//            {
//                printf("Wc < 0\n");
//                printf("VertexA = %f %f %f %f\n", TriVtxA[0],TriVtxA[1],TriVtxA[2],TriVtxA[3]);
//                printf("VertexB = %f %f %f %f\n", TriVtxB[0],TriVtxB[1],TriVtxB[2],TriVtxB[3]);
//                printf("VertexC = %f %f %f %f\n", TriVtxC[0],TriVtxC[1],TriVtxC[2],TriVtxC[3]);
//
//                return 0;
//            }

//---------------------clipping---------------------
            TriVtxA[13] = (float)CalcCC2(TriVtxA);//printf("%f\n",TriVtxA[13]);
            TriVtxB[13] = (float)CalcCC2(TriVtxB);//printf("%f\n",TriVtxB[13]);
            TriVtxC[13] = (float)CalcCC2(TriVtxC);//printf("%f\n",TriVtxC[13]);

            unsigned char outcode[3];
            bool volume_inside = false;

            outcode[0] = (unsigned char)TriVtxA[13];
            outcode[1] = (unsigned char)TriVtxB[13];
            outcode[2] = (unsigned char)TriVtxC[13];

            if ((outcode[0]|outcode[1]|outcode[2])==0)
                volume_inside = true;

            if (!volume_inside)
            {
                input[0] = TriVtxA;
                input[1] = TriVtxB;
                input[2] = TriVtxC;
                if (frameDEG)
                    fprintf(GMdumper,"TRIANGLE CLIPPING...\n");
                clipping(input, output, Temporary);
            }
            else
            {
                clipcount = 1;
            }

            if (clipcount>=0)
            {
                for (int clipi=clipcount; clipi>0; clipi--)
                {
                    outIndex++;
                    if (frameDEG)
                        fprintf(GMdumper,"volume_inside:%d\n", volume_inside);
                    if (!volume_inside)
                    {
                        //fprintf(GMdumper,"set output_list \n");

                        TriVtxA[ 0]=output_list[0][ 0];
                        TriVtxB[ 0]=output_list[outIndex][ 0];
                        TriVtxC[ 0]=output_list[outIndex+1][ 0];
                        TriVtxA[ 1]=output_list[0][ 1];
                        TriVtxB[ 1]=output_list[outIndex][ 1];
                        TriVtxC[ 1]=output_list[outIndex+1][ 1];
                        TriVtxA[ 2]=output_list[0][ 2];
                        TriVtxB[ 2]=output_list[outIndex][ 2];
                        TriVtxC[ 2]=output_list[outIndex+1][ 2];
                        TriVtxA[ 3]=output_list[0][ 3];
                        TriVtxB[ 3]=output_list[outIndex][ 3];
                        TriVtxC[ 3]=output_list[outIndex+1][ 3];
                        TriVtxA[ 4]=output_list[0][ 4];
                        TriVtxB[ 4]=output_list[outIndex][ 4];
                        TriVtxC[ 4]=output_list[outIndex+1][ 4];
                        TriVtxA[ 5]=output_list[0][ 5];
                        TriVtxB[ 5]=output_list[outIndex][ 5];
                        TriVtxC[ 5]=output_list[outIndex+1][ 5];
                        TriVtxA[ 6]=output_list[0][ 6];
                        TriVtxB[ 6]=output_list[outIndex][ 6];
                        TriVtxC[ 6]=output_list[outIndex+1][ 6];
                        TriVtxA[ 7]=output_list[0][ 7];
                        TriVtxB[ 7]=output_list[outIndex][ 7];
                        TriVtxC[ 7]=output_list[outIndex+1][ 7];
                        TriVtxA[ 8]=output_list[0][ 8];
                        TriVtxB[ 8]=output_list[outIndex][ 8];
                        TriVtxC[ 8]=output_list[outIndex+1][ 8];
                        TriVtxA[ 9]=output_list[0][ 9];
                        TriVtxB[ 9]=output_list[outIndex][ 9];
                        TriVtxC[ 9]=output_list[outIndex+1][ 9];
                        TriVtxA[10]=output_list[0][10];
                        TriVtxB[10]=output_list[outIndex][10];
                        TriVtxC[10]=output_list[outIndex+1][10];
                        TriVtxA[11]=output_list[0][11];
                        TriVtxB[11]=output_list[outIndex][11];
                        TriVtxC[11]=output_list[outIndex+1][11];

                        for (int i=4;i<12;++i)
                        {
                            TriVtxA[i] = CLAMP(TriVtxA[i], 0.0, 1.0);
                            TriVtxB[i] = CLAMP(TriVtxB[i], 0.0, 1.0);
                            TriVtxC[i] = CLAMP(TriVtxC[i], 0.0, 1.0);
                        }
                    }
//---------------------perspective_division---------
                    perspective_division();

                    if (PERSPECTIVEBUG||frameDEG)
                    {
                        fprintf(GMdumper,"TriVtxA= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n"
                                ,TriVtxA[0],TriVtxA[1],TriVtxA[2],TriVtxA[3]
                                ,TriVtxA[4],TriVtxA[5],TriVtxA[6],TriVtxA[7]
                                ,TriVtxA[8],TriVtxA[9],TriVtxA[10],TriVtxA[11]);

                        fprintf(GMdumper,"TriVtxB= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n"
                                ,TriVtxB[0],TriVtxB[1],TriVtxB[2],TriVtxB[3]
                                ,TriVtxB[4],TriVtxB[5],TriVtxB[6],TriVtxB[7]
                                ,TriVtxB[8],TriVtxB[9],TriVtxB[10],TriVtxB[11]);

                        fprintf(GMdumper,"TriVtxC= \n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n\n"
                                ,TriVtxC[0],TriVtxC[1],TriVtxC[2],TriVtxC[3]
                                ,TriVtxC[4],TriVtxC[5],TriVtxC[6],TriVtxC[7]
                                ,TriVtxC[8],TriVtxC[9],TriVtxC[10],TriVtxC[11]);

                        fprintf(GMdumper,"%x\n", outcode[0]);
                        fprintf(GMdumper,"%x\n", outcode[1]);
                        fprintf(GMdumper,"%x\n", outcode[2]);

                        fprintf(GMdumper,"NormalizeVertexA = %7.3f %7.3f %7.3f %7.3f\n",TriVtxA[0],TriVtxA[1],TriVtxA[2],TriVtxA[3]);
                        fprintf(GMdumper,"NormalizeVertexB = %7.3f %7.3f %7.3f %7.3f\n",TriVtxB[0],TriVtxB[1],TriVtxB[2],TriVtxB[3]);
                        fprintf(GMdumper,"NormalizeVertexC = %7.3f %7.3f %7.3f %7.3f\n",TriVtxC[0],TriVtxC[1],TriVtxC[2],TriVtxC[3]);
                    }
//---------------------Viewport---------------------
                    Viewport();
                    if (VIEWPORTBUG||frameDEG)
                    {
                        fprintf(GMdumper,"TriVtxA_ViewportCoords= %f %f %f %f\n",TriVtxA[0],TriVtxA[1],TriVtxA[2],TriVtxA[3]);
                        fprintf(GMdumper,"TriVtxB_ViewportCoords= %f %f %f %f\n",TriVtxB[0],TriVtxB[1],TriVtxB[2],TriVtxB[3]);
                        fprintf(GMdumper,"TriVtxC_ViewportCoords= %f %f %f %f\n",TriVtxC[0],TriVtxC[1],TriVtxC[2],TriVtxC[3]);
                    }

//---------------------culling----------------------
                    culling();
//---------------check triangle order---------------------
                    if (clipvalid)
                    {
                        int  area_sign =  (TriVtxA[0]*TriVtxB[1] - TriVtxB[0]*TriVtxA[1])
                                          +(TriVtxB[0]*TriVtxC[1] - TriVtxC[0]*TriVtxB[1])
                                          +(TriVtxC[0]*TriVtxA[1] - TriVtxA[0]*TriVtxC[1]);

                        int  small_area = 0;

#if KICKSMALLAREA
                        if ( abs(area_sign) <1)
                        {
                            //printf("small area and kick it");
                            small_area = 1;
                            kick_small_area_count++;
                            //printf("kick count = %d\n",kick_small_area_count);
                        }
#endif

                        if (small_area==0)
                        {
                            if (area_sign < 0)
                            {
                                tempVtx[0] =TriVtxB[0] ;
                                tempVtx[1] =TriVtxB[1] ;
                                tempVtx[2] =TriVtxB[2] ;
                                tempVtx[3] =TriVtxB[3] ;
                                tempVtx[4] =TriVtxB[4] ;
                                tempVtx[5] =TriVtxB[5] ;
                                tempVtx[6] =TriVtxB[6] ;
                                tempVtx[7] =TriVtxB[7] ;
                                tempVtx[8] =TriVtxB[8] ;
                                tempVtx[9] =TriVtxB[9] ;
                                tempVtx[10]=TriVtxB[10];
                                tempVtx[11]=TriVtxB[11];

                                TriVtxB[0] =TriVtxC[0] ;
                                TriVtxB[1] =TriVtxC[1] ;
                                TriVtxB[2] =TriVtxC[2] ;
                                TriVtxB[3] =TriVtxC[3] ;
                                TriVtxB[4] =TriVtxC[4] ;
                                TriVtxB[5] =TriVtxC[5] ;
                                TriVtxB[6] =TriVtxC[6] ;
                                TriVtxB[7] =TriVtxC[7] ;
                                TriVtxB[8] =TriVtxC[8] ;
                                TriVtxB[9] =TriVtxC[9] ;
                                TriVtxB[10]=TriVtxC[10];
                                TriVtxB[11]=TriVtxC[11];

                                TriVtxC[0] =tempVtx[0] ;
                                TriVtxC[1] =tempVtx[1] ;
                                TriVtxC[2] =tempVtx[2] ;
                                TriVtxC[3] =tempVtx[3] ;
                                TriVtxC[4] =tempVtx[4] ;
                                TriVtxC[5] =tempVtx[5] ;
                                TriVtxC[6] =tempVtx[6] ;
                                TriVtxC[7] =tempVtx[7] ;
                                TriVtxC[8] =tempVtx[8] ;
                                TriVtxC[9] =tempVtx[9] ;
                                TriVtxC[10]=tempVtx[10];
                                TriVtxC[11]=tempVtx[11];
                            }
                            if (CheckorderBUG)
                            {
                                fprintf(GMdumper,"area_sign= %d \n",area_sign);
                                fprintf(GMdumper,"TriVtxA_order= %f %f %f \n",TriVtxA[0],TriVtxA[1],TriVtxA[2]);
                                fprintf(GMdumper,"TriVtxB_order= %f %f %f \n",TriVtxB[0],TriVtxB[1],TriVtxB[2]);
                                fprintf(GMdumper,"TriVtxC_order= %f %f %f \n",TriVtxC[0],TriVtxC[1],TriVtxC[2]);
                            }
//---------------------tile divider-----------------
                            tile_divider(&search_count,&tile_count_num,triangle_list,TriVtxA,TriVtxB,TriVtxC);
                            AvailableTriangleCounter++;
                        }
                    }
                } // for-loop
                (send_vertex=1);
            } // if (clipcount>=0)
            else
                outIndex = 0;
        }
        else
        {
            if (CHECKPRECLIPPING)
                fprintf(GMdumper,"do pre-clipping\n\n");
            (send_vertex=0);
        }
    }
    else
    {
        (send_vertex=0);
    }

    for (int i=0;i<16;i++)
        delete[] Temporary[i];
    delete[] Temporary;

    if (send_vertex==1) return 1;
    else return 0;
}

void Geometry::tile_divider(int *search_count,int *tile_count_num,triangle_node  triangle_list,float *TriVtxA_clipped,float *TriVtxB_clipped,float  *TriVtxC_clipped)
{
    int tileNO;
    int tile_counter;
    tile_counter = 0;
    *search_count = 0;
    *tile_count_num = 0;

    float X1 = TriVtxA_clipped[0];
    float X2 = TriVtxB_clipped[0];
    float X3 = TriVtxC_clipped[0];
    float Y1 = TriVtxA_clipped[1];
    float Y2 = TriVtxB_clipped[1];
    float Y3 = TriVtxC_clipped[1];

    float C1 = Y2 * X1 - X2 * Y1;     // C值最大為480*640 (至少19-bit)
    float C2 = Y3 * X2 - X3 * Y2;
    float C3 = Y1 * X3 - X1 * Y3;
    float DX12 = X1 - X2;
    float DX23 = X2 - X3;
    float DX31 = X3 - X1;
    float DY12 = Y1 - Y2;
    float DY23 = Y2 - Y3;
    float DY31 = Y3 - Y1;

// Bounding rectangle; round lower bound down to block size
    int   checkMinX =((int)(Min(X1, X2, X3)));
    int   checkMinY =((int)(Min(Y1, Y2, Y3)));
    int   checkMaxX =((int)(Max(X1, X2, X3)))+1;
    int   checkMaxY =((int)(Max(Y1, Y2, Y3)))+1;

    int		minx_temp = (( checkMinX ) & ~(TILEWIDTH  - 1))-TILEWIDTH;
    int		miny_temp = (( checkMinY ) & ~(TILEHEIGHT - 1))-TILEHEIGHT;
    int		maxx_temp = (( checkMaxX ) +  (TILEWIDTH  - 1) & ~(TILEWIDTH - 1));
    int		maxy_temp = (( checkMaxY ) +  (TILEHEIGHT - 1) & ~(TILEHEIGHT - 1));

    int   minx = (minx_temp < 0)? 0 : ( (minx_temp > WINDOW_DEFAULT_WIDTH) ? WINDOW_DEFAULT_WIDTH  : minx_temp );
    int   miny = (miny_temp < 0)? 0 : ( (miny_temp > WINDOW_DEFAULT_HEIGHT)? WINDOW_DEFAULT_HEIGHT : miny_temp );
    int   maxx = (maxx_temp < 0)? 0 : ( (maxx_temp > WINDOW_DEFAULT_WIDTH) ? WINDOW_DEFAULT_WIDTH  : maxx_temp );
    int   maxy = (maxy_temp < 0)? 0 : ( (maxy_temp > WINDOW_DEFAULT_HEIGHT)? WINDOW_DEFAULT_HEIGHT : maxy_temp );

    if (maxx==minx) maxx=maxx+1;
    if (maxy==miny) maxy=maxy+1;

    float	TriVtx_data[36];
    for (int t=0;t<12;t++)
    {
        TriVtx_data[t]   =TriVtxA[t] ;
        TriVtx_data[t+12]=TriVtxB[t] ;
        TriVtx_data[t+24]=TriVtxC[t] ;
    }

    triangle_node newtri_node;
    newtri_node = (triangle_node) malloc (sizeof(struct triangleData));
    if (newtri_node != NULL)
    {
        for (i=0;i<36;i++)
            newtri_node -> VertexDATA[i] = TriVtx_data[i];
        newtri_node -> tiledata_ptr = NULL;
        newtri_node -> next_triangle = NULL;
    }
    triangle_node current_triangle,previous_triangle,post_triangle;
    previous_triangle = NULL;
    current_triangle = triangle_list->next_triangle;

    while (current_triangle!=NULL)
    {
        previous_triangle = current_triangle;
        current_triangle  = current_triangle->next_triangle;
    }
    if (previous_triangle == NULL)
    {
        // This case are only used for first triangle
        triangle_list->next_triangle = newtri_node;
        post_triangle = triangle_list->next_triangle;
    }
    else if (previous_triangle->next_triangle==NULL)
    {
        previous_triangle->next_triangle = newtri_node;
        post_triangle     = previous_triangle->next_triangle;
    }

    //triangleListSize += sizeof(triangleData);
    triangleListSize += 60;
    triangleSize += 60; // 5 entry *4 bytes *3 (vertices/per triangle)

    for (int y = miny; y < maxy; y += TILEHEIGHT)
    {
        for (int x = minx; x < maxx; x += TILEWIDTH)
        {
            float  edge12 = C1 + DY12 * x - DX12 * y;
            float  edge23 = C2 + DY23 * x - DX23 * y;
            float  edge31 = C3 + DY31 * x - DX31 * y;

            float  E1_1=edge12;
            float  E1_2=edge12 + (DY12 * TILEWIDTH);
            float  E1_3=edge12 - (DX12 * TILEHEIGHT);
            float  E1_4=edge12 + (DY12 * TILEWIDTH) - (DX12 * TILEHEIGHT);

            float  E2_1=edge23;
            float  E2_2=edge23 + (DY23 * TILEWIDTH);
            float  E2_3=edge23 - (DX23 * TILEHEIGHT);
            float  E2_4=edge23 + (DY23 * TILEWIDTH) - (DX23 * TILEHEIGHT);

            float  E3_1=edge31;
            float  E3_2=edge31 + (DY31 * TILEWIDTH);
            float  E3_3=edge31 - (DX31 * TILEHEIGHT);
            float  E3_4=edge31 + (DY31 * TILEWIDTH) - (DX31 * TILEHEIGHT);

            int  pass1_1=0;     //若大於0 則pass1 = edge12[2]
            int  pass1_2=0;
            if  	   (E1_1>0)   pass1_1=pass1_1+1;
            //else if  (E1_1<=0) 	 pass1_2=pass1_2+1;
            if  	   (E1_2>0)   pass1_1=pass1_1+1;
            //else if  (E1_2<=0) 	 pass1_2=pass1_2+1;
            if  	   (E1_3>0)   pass1_1=pass1_1+1;
            //else if  (E1_3<=0) 	 pass1_2=pass1_2+1;
            if  	   (E1_4>0)   pass1_1=pass1_1+1;
            //else if  (E1_4<=0) 	 pass1_2=pass1_2+1;

            int  pass2_1=0;     //若大於0 則pass1 = edge12[2]
            int  pass2_2=0;
            if  	   (E2_1>0)   pass2_1=pass2_1+1;
            //else if  (E2_1<=0) 	 pass2_2=pass2_2+1;
            if  	   (E2_2>0)   pass2_1=pass2_1+1;
            //else if  (E2_2<=0) 	 pass2_2=pass2_2+1;
            if  	   (E2_3>0)   pass2_1=pass2_1+1;
            //else if  (E2_3<=0) 	 pass2_2=pass2_2+1;
            if  	   (E2_4>0)   pass2_1=pass2_1+1;
            //else if  (E2_4<=0) 	 pass2_2=pass2_2+1;

            int  pass3_1=0;     //若大於0 則pass1 = edge12[2]
            int  pass3_2=0;
            if  	   (E3_1>0)   pass3_1=pass3_1+1;
            //else if  (E3_1<=0) 	 pass3_2=pass3_2+1;
            if  	   (E3_2>0)   pass3_1=pass3_1+1;
            //else if  (E3_2<=0) 	 pass3_2=pass3_2+1;
            if  	   (E3_3>0)   pass3_1=pass3_1+1;
            //else if  (E3_3<=0) 	 pass3_2=pass3_2+1;
            if  	   (E3_4>0)   pass3_1=pass3_1+1;
            //else if  (E3_4<=0) 	 pass3_2=pass3_2+1;
//---------------------------------------------------------------------------------------------------------
            if ( (pass1_1 == 0 || pass2_1 == 0 || pass3_1 == 0) ) //&& (pass1_2 == 0 || pass2_2 == 0 || pass3_2 == 0) )
            {
                *search_count = *search_count + 1;
            }
            else
            {
                tileNO = (x+y*WINDOW_DEFAULT_WIDTH/TILEHEIGHT)/TILEWIDTH ;
                tile_node new_node;
                new_node = (tile_node) malloc (sizeof(struct tileData));
                if (new_node != NULL)
                {
                    //new_node -> pointer_triangle = NewVertexNode -> VertexDATA;
                    new_node -> SN_and_TN = (tile_counter<<16) | tileNO;
                    new_node -> next_tile = NULL;
                }

                triangleListSize += 4;

                tile_counter = tile_counter + 1;
                *search_count = *search_count + 1;
                *tile_count_num = *tile_count_num + 1;

                tile_node current_tile,previous_tile;
                previous_tile = NULL;
                current_tile = post_triangle->tiledata_ptr;

                while (current_tile!=NULL)
                {
                    previous_tile=current_tile;
                    current_tile=current_tile->next_tile;
                }
                if (previous_tile == NULL)
                    post_triangle->tiledata_ptr = new_node;
                else if (previous_tile->next_tile==NULL)
                    previous_tile->next_tile = new_node;
            }
        }//X 迴圈結束
    }//y 迴圈結束

    if (TILEDIVIDERDEG||frameDEG)
    {
        triangle_node TriNodePtr;
        TriNodePtr = post_triangle;

        while (TriNodePtr!=NULL)
        {
            for (int address_count=0;address_count<36;address_count++)
                fprintf(GMdumper,"TriVtx_data= %6.3f\n",*(TriNodePtr->VertexDATA+address_count));
            fprintf(GMdumper,"\n");

            tile_node TileNodePtr;
            TileNodePtr = TriNodePtr->tiledata_ptr;
            while (TileNodePtr!=NULL)
            {
                fprintf(GMdumper,"SerialNumber= %d   ", TileNodePtr->SN_and_TN>>16);
                fprintf(GMdumper,"TileNumber  = %d\n" , (short)TileNodePtr->SN_and_TN);
                TileNodePtr = TileNodePtr->next_tile;
            }
            TriNodePtr = TriNodePtr->next_triangle;
            fprintf(GMdumper,"\n");
        }
    }

    return;
}

void Geometry::culling(void)
{
    bool 	cw; // wether the triangle is cw facing
    float sign;
    float vectorAB[3]={0};
    float vectorAC[3]={0};
    float triangle_normal[3]={0};
    float eye_vector[3]={0,0,1};

#if 1  //vector method
    vectorAB[0] = TriVtxB[0] - TriVtxA[0];
    vectorAB[1] = TriVtxB[1] - TriVtxA[1];
    vectorAB[2] = TriVtxB[2] - TriVtxA[2];
    vectorAC[0] = TriVtxC[0] - TriVtxA[0];
    vectorAC[1] = TriVtxC[1] - TriVtxA[1];
    vectorAC[2] = TriVtxC[2] - TriVtxA[2];
    triangle_normal[0] = vectorAB[1]*vectorAC[2] - vectorAC[1]* vectorAB[2];
    triangle_normal[1] = vectorAB[2]*vectorAC[0] - vectorAC[2]* vectorAB[0];
    triangle_normal[2] = vectorAB[0]*vectorAC[1] - vectorAC[0]* vectorAB[1];
    sign =   triangle_normal[0]*eye_vector[0]  + triangle_normal[1]*eye_vector[1]
             + triangle_normal[2]*eye_vector[2] ;
#endif

#if 0  //area method (after viewport-window coordinate)
    sign = (TriVtxA[0]*TriVtxB[1] - TriVtxB[0]*TriVtxA[1])
           +(TriVtxB[0]*TriVtxC[1] - TriVtxC[0]*TriVtxB[1])
           +(TriVtxC[0]*TriVtxA[1] - TriVtxA[0]*TriVtxC[1]);
#endif
    //fprintf(GMdumper,"CULLING : Sign = %f\n", sign);
    if (sign < 0)       // cw = backface culling
        cw = true;
    else if (sign >= 0)
        cw = false;
    else
        cw = sign < 0;

//    if (sign < -(1/1024))       // cw = backface culling
//        cw = true;
//    else if (sign >= (1/1024))
//        cw = false;
//    else
//        cw = sign < 0;

    bool backFace = cw ^ (FrontFace==GL_CW); //true:GL_CW; false:GL_CCW,  bool backFace = cw ^ (FrontFace==GL_CW);
    if (CullFace)
    {
        bool cull_mode = (CullFaceMode==GL_BACK )? 0 : 1;

        if ((cull_mode) ^ backFace) //0:GL_BACK; 1:GL_FRONT
        {
            //fprintf(GMdumper,"Execute back face culling \n");
            clipvalid = false;
            cullcount++;
        }
        else
        {
            //fprintf(GMdumper,"NOT ..... back face culling \n");
            clipvalid = true;
        }
    }
    //fprintf(GMdumper,"cull count = %d \n",cullcount);
}

void Geometry::clipping(float * input[16], float * output[16], float ** Temporary)
{
    int plane;

    float ** vilist = input;
    float ** volist = output;

    float * vprev, * vnext;
    int 	i, icnt = 3, ocnt = 0; // input count = 3 for an triangle
    float ** nextTemporary = Temporary;

    unsigned int cc = 0;

    for (i = 0; i < icnt; ++i)
    {
        cc |= (int)(vilist[i])[13];
    }

    for (plane = 0; plane < 6; plane++)
    {
        int c, p = 1 << plane;
        int coord = plane >> 1;
        bool inside, prev_inside;

        if (!(cc & p)) continue;

        cc = 0;
        ocnt = 0;
        vnext = vilist[icnt - 1];

        inside = !((int)vnext[13] & p);

        for (c = 0; c < icnt; c++)
        {

            float * voutside, * vinside;
            prev_inside = inside;
            vprev = vnext;
            vnext = vilist[c];
            inside = !((int)vnext[13] & p);

            if (inside ^ prev_inside)
            {
                if (inside)
                {
                    vinside = vnext;
                    voutside = vprev;
                }
                else
                {
                    voutside = vnext;
                    vinside = vprev;
                }

                float ci = vinside[coord];
                float wi = vinside[3];
                float co = voutside[coord];
                float wo = voutside[3];
//printf("!(plane & 1) = %d %d",!(plane & 1),plane);
                if (!(plane & 1))
                {
                    wi = -wi;
                    wo = -wo;
                }

                float num = wi - ci;
                float denom = co - ci - wo + wi;

                float * newVertex;
                newVertex = *nextTemporary++;

                float coeff = Coeff4q28(num, denom);

                if (coeff < (0.5))
                    InterpolateVtx(newVertex, vinside, voutside, coeff);
                else
                    InterpolateVtx(newVertex, voutside, vinside, (1) - coeff);

                //newVertex.m_ClipCoords[coord] = newVertex.m_ClipCoords.w();
                newVertex[13] = (float)CalcCC2(newVertex);
                cc |= (int)newVertex[13];

                volist[ocnt++] = newVertex;
            }

            if (inside /*&& c != icnt-1*/)
            {
                volist[ocnt++] = vilist[c];
                cc |= (int)(vilist[c])[13];
            }
        }

        {
            // swap input and output lists
            float ** vtlist;
            vtlist = vilist;
            vilist = volist;
            volist = vtlist;
        }
        // icnt is the total number of vertices after clipping
        icnt = ocnt;

    }
    // set output parameter
    int outCount = icnt-2;  // # of triangles = # of vtx - 2
    for (i=0; i<icnt; i++)   // copy the output vertex list
    {
        for (int j=0; j<16; j++)
        {
            output_list[i][j]=vilist[i][j];
            //printf("vilist[%d][%d] = %f\n",i,j,vilist[i][j]);
        }
        //printf("vilist[%d][13] = %f\n",i,vilist[i][13]);
    } // vilist is the output vertex list

    clipcount = outCount;

}

void Geometry::pre_clipping(void)
{
    int temp1[6]= {0},temp2[6]= {0},temp3[6]= {0};
    CalcCC(temp1,TriVtxA);
    CalcCC(temp2,TriVtxB);
    CalcCC(temp3,TriVtxC);
    if (  (temp1[0]&temp2[0]&temp3[0] )
            ||(temp1[1]&temp2[1]&temp3[1] )
            ||(temp1[2]&temp2[2]&temp3[2] )
            ||(temp1[3]&temp2[3]&temp3[3] )
            ||(temp1[4]&temp2[4]&temp3[4] )
            ||(temp1[5]&temp2[5]&temp3[5] )  )
    {
        TriangleReady=0;
        pre_clipping_count++;
    }
    if (PRECLIPPINGBUG||frameDEG)
    {
        fprintf(GMdumper,"temp1= %d %d %d %d %d %d\n",temp1[0],temp1[1],temp1[2],temp1[3],temp1[4],temp1[5]);
        fprintf(GMdumper,"temp2= %d %d %d %d %d %d\n",temp2[0],temp2[1],temp2[2],temp2[3],temp2[4],temp2[5]);
        fprintf(GMdumper,"temp3= %d %d %d %d %d %d\n",temp3[0],temp3[1],temp3[2],temp3[3],temp3[4],temp3[5]);
        fprintf(GMdumper,"TriangleReady= %d \n",TriangleReady);
        fprintf(GMdumper,"pre_clipping_count= %d \n",pre_clipping_count);

    }
}

int Geometry::CalcCC(int *check_outside ,float * vertex)
{
    if ((vertex[0] < -vertex[3]))
        check_outside[0] = 1;
    if ((vertex[0] >  vertex[3]))
        check_outside[1] = 1;
    if ((vertex[1] < -vertex[3]))
        check_outside[2] = 1;
    if ((vertex[1] >  vertex[3]))
        check_outside[3] = 1;
    if ((vertex[2] < -vertex[3]))
        check_outside[4] = 1;
    if ((vertex[2] >  vertex[3]))
        check_outside[5] = 1;
    return check_outside[6];
}

void Geometry::perspective_division(void)
{
    perspectiveA_w = 1/TriVtxA[3];
    perspectiveB_w = 1/TriVtxB[3];
    perspectiveC_w = 1/TriVtxC[3];

    TriVtxA[0] = TriVtxA[0] * perspectiveA_w;
    TriVtxA[1] = TriVtxA[1] * perspectiveA_w;
    TriVtxA[2] = TriVtxA[2] * perspectiveA_w;
    TriVtxA[3] = perspectiveA_w;
    TriVtxA[4] = TriVtxA[4]  * perspectiveA_w;
    TriVtxA[5] = TriVtxA[5]  * perspectiveA_w;
    TriVtxA[6] = TriVtxA[6]  * perspectiveA_w;
    TriVtxA[7] = TriVtxA[7]  * perspectiveA_w;
    TriVtxA[8] = TriVtxA[8]  * perspectiveA_w;
    TriVtxA[9] = TriVtxA[9]  * perspectiveA_w;
    TriVtxA[10]= TriVtxA[10] * perspectiveA_w;
    TriVtxA[11]= TriVtxA[11] * perspectiveA_w;

    TriVtxB[0] = TriVtxB[0] * perspectiveB_w;
    TriVtxB[1] = TriVtxB[1] * perspectiveB_w;
    TriVtxB[2] = TriVtxB[2] * perspectiveB_w;
    TriVtxB[3] = perspectiveB_w;
    TriVtxB[4] = TriVtxB[4] * perspectiveB_w;
    TriVtxB[5] = TriVtxB[5] * perspectiveB_w;
    TriVtxB[6] = TriVtxB[6] * perspectiveB_w;
    TriVtxB[7] = TriVtxB[7] * perspectiveB_w;
    TriVtxB[8] = TriVtxB[8] * perspectiveB_w;
    TriVtxB[9] = TriVtxB[9] * perspectiveB_w;
    TriVtxB[10]= TriVtxB[10]* perspectiveB_w;
    TriVtxB[11]= TriVtxB[11]* perspectiveB_w;

    TriVtxC[0] = TriVtxC[0] * perspectiveC_w;
    TriVtxC[1] = TriVtxC[1] * perspectiveC_w;
    TriVtxC[2] = TriVtxC[2] * perspectiveC_w;
    TriVtxC[3] = perspectiveC_w;
    TriVtxC[4] = TriVtxC[4] * perspectiveC_w;
    TriVtxC[5] = TriVtxC[5] * perspectiveC_w;
    TriVtxC[6] = TriVtxC[6] * perspectiveC_w;
    TriVtxC[7] = TriVtxC[7] * perspectiveC_w;
    TriVtxC[8] = TriVtxC[8] * perspectiveC_w;
    TriVtxC[9] = TriVtxC[9] * perspectiveC_w;
    TriVtxC[10]= TriVtxC[10]* perspectiveC_w;
    TriVtxC[11]= TriVtxC[11]* perspectiveC_w;

}

void Geometry::Viewport(void)
{
    float ViewportInfo[6]={ ViewportOrigin[0], ViewportOrigin[1],
                            ViewportScale [0], ViewportScale [1],
                            DepthRangeFactor , DepthRangeBase
                          };
    ClipCoordsToWindowCoords(TriVtxA, ViewportInfo);
    ClipCoordsToWindowCoords(TriVtxB, ViewportInfo);
    ClipCoordsToWindowCoords(TriVtxC, ViewportInfo);

}

void Geometry::primitive_assembly(void)
{
    if (vtxIndex==0)
    {
        procVtxA[0]=ClipCoords[0];
        procVtxA[1]=ClipCoords[1];
        procVtxA[2]=ClipCoords[2];
        procVtxA[3]=ClipCoords[3];
        procVtxA[4]= TexCoords[0];
        procVtxA[5]= TexCoords[1];
        procVtxA[6]= TexCoords[2];
        procVtxA[7]= TexCoords[3];
        procVtxA[8]=     Color[0];
        procVtxA[9]=     Color[1];
        procVtxA[10]=    Color[2];
        procVtxA[11]=    Color[3];
    }
    else if (vtxIndex==1)
    {
        procVtxB[0]=ClipCoords[0];
        procVtxB[1]=ClipCoords[1];
        procVtxB[2]=ClipCoords[2];
        procVtxB[3]=ClipCoords[3];
        procVtxB[4]= TexCoords[0];
        procVtxB[5]= TexCoords[1];
        procVtxB[6]= TexCoords[2];
        procVtxB[7]= TexCoords[3];
        procVtxB[8]=     Color[0];
        procVtxB[9]=     Color[1];
        procVtxB[10]=    Color[2];
        procVtxB[11]=    Color[3];
    }
    else if (vtxIndex==2)
    {
        procVtxC[0]=ClipCoords[0];
        procVtxC[1]=ClipCoords[1];
        procVtxC[2]=ClipCoords[2];
        procVtxC[3]=ClipCoords[3];
        procVtxC[4]= TexCoords[0];
        procVtxC[5]= TexCoords[1];
        procVtxC[6]= TexCoords[2];
        procVtxC[7]= TexCoords[3];
        procVtxC[8]=     Color[0];
        procVtxC[9]=     Color[1];
        procVtxC[10]=    Color[2];
        procVtxC[11]=    Color[3];
    }
    else
        printf("!!!!!!!!vertex index error(input)!!!!!!!!\n");

    switch (DrawMode)
    {
    case GL_TRIANGLES:							//0:GL_TRIANGLES
        if (vtxIndex==2)
        {
            // the last vertex have been ready
            for (i=0; i<12; i++)
            {
                TriVtxA[i]= procVtxA[i];
                TriVtxB[i]= procVtxB[i];
                TriVtxC[i]= procVtxC[i];
            }
            TriangleReady=1;
            TriangleCounter++;
        }
        else
            TriangleReady=0;
        break;

    case GL_TRIANGLE_STRIP:						//1:GL_TRIANGLE_STRIP
        if (!TriangleCounter)
        {
            // the first triangle is not complete yet
            if (vtxIndex==2)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxA[i];
                    TriVtxB[i]=procVtxB[i];
                    TriVtxC[i]=procVtxC[i];
                }
                TriangleReady=1;
                TriangleCounter++;
            }
            else TriangleReady=0;
        }
        else if (TriangleCounter&1)
        {
            // even triangle
            if (vtxIndex==2)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxB[i];
                    TriVtxB[i]=procVtxA[i];
                    TriVtxC[i]=procVtxC[i];
                }
            }
            else if (vtxIndex==1)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxA[i];
                    TriVtxB[i]=procVtxC[i];
                    TriVtxC[i]=procVtxB[i];
                }
            }
            else if (vtxIndex==0)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxC[i];
                    TriVtxB[i]=procVtxB[i];
                    TriVtxC[i]=procVtxA[i];
                }
            }
            else printf("!!!!!!!!vertex index error(strip-even)!!!!!!!!\n");
            TriangleReady=1;
            TriangleCounter++;
        }
        else if (!(TriangleCounter&1))
        {
            // odd triangle
            if (vtxIndex==2)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxA[i];
                    TriVtxB[i]=procVtxB[i];
                    TriVtxC[i]=procVtxC[i];
                }
            }
            else if (vtxIndex==1)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxC[i];
                    TriVtxB[i]=procVtxA[i];
                    TriVtxC[i]=procVtxB[i];
                }
            }
            else if (vtxIndex==0)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=procVtxB[i];
                    TriVtxB[i]=procVtxC[i];
                    TriVtxC[i]=procVtxA[i];
                }
            }
            else
                printf("!!!!!!!!vertex index error(strip-odd)!!!!!!!!\n");

            TriangleReady=1;
            TriangleCounter++;
        }
        break;

    case GL_TRIANGLE_FAN:		//2:GL_TRIANGLE_FAN
        static float centerVtx[12];
        if (TriangleCounter==0)
        {
            // the first triangle is not complete yet
            if (vtxIndex==0)
            {
                // the first vertex is the center vertex
                //printf("the first vertex is the center vertex\n");
                for (i=0; i<12; i++) centerVtx[i]=procVtxA[i];
                TriangleReady=0;
            }
            else if (vtxIndex==2)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=centerVtx[i];
                    TriVtxB[i]=procVtxB[i];
                    TriVtxC[i]=procVtxC[i];
                }
                TriangleReady=1;
                TriangleCounter++;
            }
        }
        else
        {
            if (vtxIndex==2)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=centerVtx[i];
                    TriVtxB[i]=procVtxB[i];
                    TriVtxC[i]=procVtxC[i];
                }

            }
            else if (vtxIndex==1)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=centerVtx[i];
                    TriVtxB[i]=procVtxA[i];
                    TriVtxC[i]=procVtxB[i];
                }
            }
            else if (vtxIndex==0)
            {
                for (i=0; i<12; i++)
                {
                    TriVtxA[i]=centerVtx[i];
                    TriVtxB[i]=procVtxC[i];
                    TriVtxC[i]=procVtxA[i];
                }
            }
            TriangleReady=1;
            TriangleCounter++;
        }
        break;

    default:

        break;
    }
    vtxIndex = (vtxIndex + 1)%3;
}
void Geometry::texture_transformation(void)  //RETURN BACK  TexCoords
{
    //TextureCoordArray = GL_TEXTURE_COORD_ARRAY;
    MatrixMult (TextureMat, InputTexCoord, TexCoords);
}


void Geometry::lighting (void)     //RETURN BACK Color[0]~Color[1]~Color[2]~Color[3]
{
//---------------------------------------------------------------------------------------------------//
    /* lighting vertex */
    float  VPpli[3];
    int  	 mode=1; // 0: unlit, 1: front, 2: back
    if (Lighting)
    {

        // emissived and lighting model ambient
        Color[0] = MatEmission[0] + (MatAmbient[0] * LightModelAmbient[0]);
        Color[1] = MatEmission[1] + (MatAmbient[1] * LightModelAmbient[1]);
        Color[2] = MatEmission[2] + (MatAmbient[2] * LightModelAmbient[2]);
        Color[3] = MatEmission[3] + (MatAmbient[3] * LightModelAmbient[3]);

        if (frameDEG)
            fprintf(GMdumper, "Lighting->Init Color: %f, %f, %f, %f\n", Color[0], Color[1], Color[2], Color[3]);

//--------------calculate normal------------------
        // normalized normal
        float normEyeN[3];
        if (Normalize)
        {
            Normalize3(EyeNormal, normEyeN);
        }
        else
        {
            normEyeN[0]=EyeNormal[0];
            normEyeN[1]=EyeNormal[1];
            normEyeN[2]=EyeNormal[2];
        }
        // front/back normal
        for (int i=0; i<3; i++)
            normEyeN[i] = (mode == 1) ? normEyeN[i] : -normEyeN[i];
        if (frameDEG)
            fprintf(GMdumper, "Lighting->Normal: %f, %f, %f\n", normEyeN[0], normEyeN[1], normEyeN[2]);
//-----------------do normalization(VPpli)---------
        // calculate VPpli
        Direction (EyeCoords, Position, VPpli);
        if (frameDEG)
        {
            fprintf(GMdumper, "Lighting->Position: %f, %f, %f, %f\n", Position[0], Position[1], Position[2], Position[3]);
            fprintf(GMdumper, "Lighting->VPpli: %f, %f, %f\n", VPpli[0], VPpli[1], VPpli[2]);
        }
        // normalize VPpli
        float sqLength =    (VPpli[0] * VPpli[0])
                            + (VPpli[1] * VPpli[1])
                            + (VPpli[2] * VPpli[2]); // keep squared length around for later
        float invSqLength =  (1/sqrt(sqLength));
        VPpli[0] =  (VPpli[0] * invSqLength);
        VPpli[1] =  (VPpli[1] * invSqLength);
        VPpli[2] =  (VPpli[2] * invSqLength);
        if (frameDEG)
            fprintf(GMdumper, "Lighting->Normalized VPpli: %f, %f, %f\n", VPpli[0], VPpli[1], VPpli[2]);
//-------------------------------------------------
        // calculate spoti
        float SpotDirection[] = {0,0,-1.0};
        /*        float normDir[3];
                float cosine;
                if (SpotCutoff != (180.0))
                {
                    Normalize3(SpotDirection, normDir);
                    cosine = -(VectorDot(VPpli, normDir));
                    if (cosine < cos(SpotCutoff)) //from API
                        spot = 0;
                    else
                        spot =  pow(cosine, SpotExponent);   //do exponentiation
                }
                else
                		spot = 1.0;
        */
//-------------------------------------------------
        // calculate att
        /*float ConstAtt = 1.0;
        float LinAtt = 0.0;
        float QuadAtt = 0.0;
              float length =  sqrt(sqLength);   						 //do sqrt
                if (Position[3] != 0) // if Ppli.w !=0 calculate att
                    Att =  ( 1/( ConstAtt +  (LinAtt * length) +  (QuadAtt * sqLength) ));  //do inverse
          			else
          					Att = 1.0;
        */
//-------------------------------------------------
        float attspot = Att*Spot;
        // calculate ambient component
        float ambientC[4] = {  ((MatAmbient[0] * LiAmbient[0]) ),
                               ((MatAmbient[1] * LiAmbient[1]) ),
                               ((MatAmbient[2] * LiAmbient[2]) ),
                               ((MatAmbient[3] * LiAmbient[3]) )
                            };
        if (frameDEG)
        {
            fprintf(GMdumper, "Acm: %f, %f, %f, %f\n", MatAmbient[0], MatAmbient[1], MatAmbient[2], MatAmbient[3]);
            fprintf(GMdumper, "Acli: %f, %f, %f, %f\n", LiAmbient[0], LiAmbient[1], LiAmbient[2], LiAmbient[3]);
            fprintf(GMdumper, "ambientC: %f, %f, %f, %f\n", ambientC[0], ambientC[1], ambientC[2], ambientC[3]);
        }
//----------------------------------------------------------------------------------------
        // calculate diffuse component
        float diffuseFactor = VectorDot(normEyeN, VPpli);
        if (diffuseFactor < 0)  diffuseFactor = 0;

        bool  fi;
        if (diffuseFactor!=0)	fi=1;
        else					fi=0;

        float diffuseC[4]  = {0, 0, 0, 0};
        float specularC[4] = {0, 0, 0, 0};

        diffuseC[0] =  ((diffuseFactor * (MatDiffuse[0]*LiDiffuse[0])));
        diffuseC[1] =  ((diffuseFactor * (MatDiffuse[1]*LiDiffuse[1])));
        diffuseC[2] =  ((diffuseFactor * (MatDiffuse[2]*LiDiffuse[2])));
        diffuseC[3] =  ((diffuseFactor * (MatDiffuse[3]*LiDiffuse[3])));

        if (frameDEG)
        {
            fprintf(GMdumper, "diffuseFactor: %f\n", diffuseFactor);
            fprintf(GMdumper, "diffuseC: %f, %f, %f, %f\n", diffuseC[0], diffuseC[1], diffuseC[2], diffuseC[3]);
        }

        // calculate specular component
        // calculate h^
        if (diffuseFactor != 0)
        {
            float h[3] = { VPpli[0], VPpli[1], VPpli[2]+ 1.0 };
            float normH[3];
            Normalize3(h, normH);

            float specularFactor = VectorDot(normEyeN, normH);
            if (specularFactor < 0)  specularFactor = 0;

            if (frameDEG)
            {
                fprintf(GMdumper, "Shininess: %f\n", Shininess);
                fprintf(GMdumper, "specularFactor: %f\n", specularFactor);
            }

            specularFactor =  pow(specularFactor, Shininess);

            specularC[0]= ( specularFactor * ((MatSpecular[0] * LiSpecular[0])));
            specularC[1]= ( specularFactor * ((MatSpecular[1] * LiSpecular[1])));
            specularC[2]= ( specularFactor * ((MatSpecular[2] * LiSpecular[2])));
            specularC[3]= ( specularFactor * ((MatSpecular[3] * LiSpecular[3])));

            if (frameDEG)
                fprintf(GMdumper, "specularFactor: %f\n", specularFactor);
        }
        else
        {
            specularC[0]= 0;
            specularC[1]= 0;
            specularC[2]= 0;
            specularC[3]= 0;
        }

        if (frameDEG)
            fprintf(GMdumper, "specularC: %f, %f, %f, %f\n", specularC[0], specularC[1], specularC[2], specularC[3]);

        Color[0] = CLAMP ( (Color[0]+ attspot*(ambientC[0]+diffuseC[0]+specularC[0])), 0, 1.0 );
        Color[1] = CLAMP ( (Color[1]+ attspot*(ambientC[1]+diffuseC[1]+specularC[1])), 0, 1.0 );
        Color[2] = CLAMP ( (Color[2]+ attspot*(ambientC[2]+diffuseC[2]+specularC[2])), 0, 1.0 );
        Color[3] = CLAMP ( (Color[3]+ attspot*(ambientC[3]+diffuseC[3]+specularC[3])), 0, 1.0 );
    }

    // lighting disabled
    else
    {
        Color[0]=InputColor[0];
        Color[1]=InputColor[1];
        Color[2]=InputColor[2];
        Color[3]=InputColor[3];
    }
}

void Geometry::transformation(void)//RETURN BACK  EyeCoords~ClipCoords~EyeNormal
{
    // model-view xform
    MatrixMult (ModelViewMat, InputVertex, EyeCoords);
    // projection xform
    MatrixMult (ProjectionMat, EyeCoords, ClipCoords);
    // normal xform

    float	temp4x4[16] = {0};
    float	temp4[4] = {0};

    temp4[0]=InputNormal[0];
    temp4[1]=InputNormal[1];
    temp4[2]=InputNormal[2];

    // transpose the inversed M
    temp4x4[0]=InverseM[0];
    temp4x4[1]=InverseM[1];
    temp4x4[2]=InverseM[2];
    temp4x4[4]=InverseM[3];
    temp4x4[5]=InverseM[4];
    temp4x4[6]=InverseM[5];
    temp4x4[8]=InverseM[6];
    temp4x4[9]=InverseM[7];
    temp4x4[10]=InverseM[8];

    MatrixMult (temp4x4, temp4, EyeNormal);

    if (MATRIXDEBUG||frameDEG)
    {
        fprintf(GMdumper,"\n---------------------------- ModelView Matrix ------------------------------------------\n");
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ModelViewMat[ELM4(0, 0)], ModelViewMat[ELM4(0, 1)], ModelViewMat[ELM4(0, 2)], ModelViewMat[ELM4(0, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ModelViewMat[ELM4(1, 0)], ModelViewMat[ELM4(1, 1)], ModelViewMat[ELM4(1, 2)], ModelViewMat[ELM4(1, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ModelViewMat[ELM4(2, 0)], ModelViewMat[ELM4(2, 1)], ModelViewMat[ELM4(2, 2)], ModelViewMat[ELM4(2, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ModelViewMat[ELM4(3, 0)], ModelViewMat[ELM4(3, 1)], ModelViewMat[ELM4(3, 2)], ModelViewMat[ELM4(3, 3)]);
        fprintf(GMdumper,"----------------------------------------------------------------------------------------\n");

        fprintf(GMdumper,"---------------------------- Projection Matrix -----------------------------------------\n");
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ProjectionMat[ELM4(0, 0)], ProjectionMat[ELM4(0, 1)], ProjectionMat[ELM4(0, 2)], ProjectionMat[ELM4(0, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ProjectionMat[ELM4(1, 0)], ProjectionMat[ELM4(1, 1)], ProjectionMat[ELM4(1, 2)], ProjectionMat[ELM4(1, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ProjectionMat[ELM4(2, 0)], ProjectionMat[ELM4(2, 1)], ProjectionMat[ELM4(2, 2)], ProjectionMat[ELM4(2, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", ProjectionMat[ELM4(3, 0)], ProjectionMat[ELM4(3, 1)], ProjectionMat[ELM4(3, 2)], ProjectionMat[ELM4(3, 3)]);
        fprintf(GMdumper,"----------------------------------------------------------------------------------------\n");

        fprintf(GMdumper,"---------------------------- Inversed M after Transpose --------------------------------\n");
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", temp4x4[ELM4(0, 0)], temp4x4[ELM4(0, 1)], temp4x4[ELM4(0, 2)], temp4x4[ELM4(0, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", temp4x4[ELM4(1, 0)], temp4x4[ELM4(1, 1)], temp4x4[ELM4(1, 2)], temp4x4[ELM4(1, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", temp4x4[ELM4(2, 0)], temp4x4[ELM4(2, 1)], temp4x4[ELM4(2, 2)], temp4x4[ELM4(2, 3)]);
        fprintf(GMdumper,"(%f , %f , %f , %f)\n", temp4x4[ELM4(3, 0)], temp4x4[ELM4(3, 1)], temp4x4[ELM4(3, 2)], temp4x4[ELM4(3, 3)]);
        fprintf(GMdumper,"----------------------------------------------------------------------------------------\n\n");
    }

}

void Geometry::MatrixMult (float * matrix, float * input, float * output)
{
    for (i=0;i<4;i++)
        output[i] = 0;
    for (i=0;i<4;i++)
    {
        output[0] = input[i] * matrix[i*4]   + output[0];
        output[1] = input[i] * matrix[i*4+1] + output[1];
        output[2] = input[i] * matrix[i*4+2] + output[2];
        output[3] = input[i] * matrix[i*4+3] + output[3];
    }
}

void Geometry::Normalize3(float *Vin,float *Vout) 		//do inverse here
{
    float lengthSq;
    lengthSq = (Vin[0] * Vin[0]) + (Vin[1] * Vin[1]) + (Vin[2] * Vin[2]);
    Vout[0] =	( Vin[0] *  (1/sqrt(lengthSq)) );
    Vout[1] = ( Vin[1] *  (1/sqrt(lengthSq)) );
    Vout[2] = ( Vin[2] *  (1/sqrt(lengthSq)) );
}

void  Geometry::Direction(float * from, float * to, float * result)
{
      result[0] =  (to[0] * from[3]) - (from[0] * to[3]);
      result[1] =  (to[1] * from[3]) - (from[1] * to[3]);
      result[2] =  (to[2] * from[3]) - (from[2] * to[3]);
//    result[0] =  (to[0]) - (from[0] );
//    result[1] =  (to[1]) - (from[1] );
//    result[2] =  (to[2]) - (from[2] );
}

float Geometry::VectorDot ( float * v1, float * v2 )
{
    return (v1[0] * v2[0])+ (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

float Geometry::CLAMP(float value, float min, float max)
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
void Geometry::ClipCoordsToWindowCoords(float* pos, float* info)
{
    float    ViewportOrigin[2]= {info[0], info[1]};
    float    ViewportScale[2] = {info[2], info[3]};
    float    DepthRangeFactor = info[4];
    float    DepthRangeBase   = info[5];
    // perform depth division
    float 	 x = pos[0];
    float 	 y = pos[1];
    float	   z = pos[2];
    float	   w = pos[3];
# if 0
    // don't use this because we will snap to 1/16th pixel further down
    // fix possible rounding problems
    if (x < -w)	x = -w;
    if (x > w)	x = w;
    if (y < -w)	y = -w;
    if (y > w)	y = w;
    if (z < -w)	z = -w;
    if (z > w)	z = w;
#endif
    //fprintf(GMdumper, "pos: %f %f %f %f\n", x, y, z, w);
    //fprintf(GMdumper, "VP: %f %f %f %f\n", ViewportOrigin[0], ViewportOrigin[1], ViewportScale[0], ViewportScale[1]);
    pos[2] = CLAMP( ((z * DepthRangeFactor) + DepthRangeBase), 0, 1.0);
    pos[1] = (y * ViewportScale[1] ) + ViewportOrigin[1];
    pos[0] = (x * ViewportScale[0] ) + ViewportOrigin[0];
}

float Geometry::Min(float a, float b, float c)
{
    float d = (a < b) ? a : b;
    return (d < c) ? d : c;
}

float Geometry::Max(float a, float b, float c)
{
    float d = (a > b) ? a : b;
    return (d > c) ? d : c;
}

unsigned int Geometry::CalcCC2(float * vertex)
{
    return	((vertex[0] < -vertex[3])       |
            ((vertex[0] >  vertex[3]) << 1) |
            ((vertex[1] < -vertex[3]) << 2) |
            ((vertex[1] >  vertex[3]) << 3) |
            ((vertex[2] < -vertex[3]) << 4) |
            ((vertex[2] >  vertex[3]) << 5));
}


float Geometry::Coeff4q28(float num, float denom)
{
    float quot = ((num) /*<< 29*/) / denom;
    return ((quot /*+ 1*/) /*>> 1*/);
}

float Geometry::Interpolate(float x0f, float x1f, float coeff4q28)
{
    return x0f + (((x1f - x0f) * coeff4q28 /*+ (1 << 27)*/) /*>> 28*/);
}

void Geometry::InterpolateVtx(float * result, const float * inside, const float * outside, float coeff4q28)
{
    for (int index = 0; index < 16; ++index)
    {
        result[index] = Interpolate(inside[index], outside[index], coeff4q28);
    }
//	printf("%f\n",coeff4q28);
//	printf("i[0] = %f\ni[1] = %f\ni[2] = %f\n",inside[0],inside[1],inside[2]);
//	printf("o[0] = %f\no[1] = %f\no[2] = %f\n",outside[0],outside[1],outside[2]);
//	printf("result[0] = %f\nresult[1] = %f\nresult[2] = %f\n\n",result[0],result[1],result[2]);
}

void Geometry::ClearStatistics()
{
    AvailableTriangleCounter = 0;
    triangleSize = 0;
    triangleListSize = 0;
    cullcount = 0;

}
