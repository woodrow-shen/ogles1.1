// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : dvr_impl.cpp
// Purpose : driver functions implementation for tile-based
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 25-08-2008   Shye-Tzeng  Shen    initial version
//
// ===================================================================

#include "drv_impl.h"

// --------------------------------------------------------------------------
// Debug Options
// --------------------------------------------------------------------------
//#define CTableDEG
#define TileRenderDEG
//#define DrawElemDEG
#define DrawArrayDEG
//#define DispListDEG
//#define RenderStateDEG

// --------------------------------------------------------------------------
// Global Variables definition
// --------------------------------------------------------------------------
Node        * displayList[TILE_COUNT]={NULL};
bool        dirtyTileMap[TILE_COUNT]={false};
RenderState * stateLinkList = new RenderState();
RenderState * stateIter = stateLinkList;
int         TileNodeSize = 0;
int         VertexCount = 0;
int         StateNodeSize = 0;
int         TriangleIO = 0;
int         StateIO = 0;
bool        isRenderDone = false;

using namespace EGL;

/**
 * C++ version char* style "itoa":
 */
char* itoa( int value, char* result, int base ) 
{
	// check that the base if valid
	if (base < 2 || base > 16) { *result = 0; return result; }

	char* out = result;
	int quotient = value;
	do {
		*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
		++out;
		quotient /= base;
	} while ( quotient );

	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';

	std::reverse( result, out );

	*out = 0;

	return result;
}

/*  Tile-based State Management Process
 *
 *  kind: 0x1->render state instructions
 *
 *        0x3->triangle pointer
 */
void add(unsigned char kind, unsigned int data, unsigned int index)
{
    Node * newnode;

    newnode = new Node();
    if (newnode==NULL)
    {
        printf("Node: Memory allocation failure.\n");
        exit(1);
    }
    if (displayList[index] == NULL)
    {
        displayList[index] = new Node();
        displayList[index]->next = NULL;
    }

    newnode->kind = kind;
    newnode->addr = data;
    newnode->next = NULL;

    if (displayList[index]->next == NULL)
    {
        displayList[index]->next = newnode;
    }
    else
    {
        Node * cur_temp, * pre_temp;
        for (cur_temp=displayList[index]->next; cur_temp; pre_temp = cur_temp, cur_temp=cur_temp->next );
        pre_temp->next = newnode;

    }

    return;
}

RenderState* addState(unsigned short id, unsigned int value, RenderState *top)
{
    RenderState * newnode;

    newnode = new RenderState();
    if (newnode==NULL)
    {
        printf("RenderState: Memory allocation failure.\n");
        exit(1);
    }

    newnode->ID    = id;
    newnode->Value = value;
    newnode->next  = NULL;

    top->next = newnode;

    return top->next;
}

void release_display_list(void)
{
    int i;
    Node * tempNode;

    for (i=TILE_COUNT-1; i>=0; --i)
    {
        //printf("%d tile delete...\n", i);
        if (displayList[i] == NULL)
            continue;
        tempNode = displayList[i]->next;
        while (tempNode != NULL)
        {
            Node * iter = tempNode;
            tempNode = iter->next;
            delete iter;
        }
        displayList[i]->next = NULL;
    }
}

void release_state_list(void)
{
    int i;
    RenderState * tempPtr;

    tempPtr = stateIter->next;
    while (tempPtr != NULL)
    {
        RenderState * iter  = tempPtr;
        tempPtr = iter->next;

        delete iter;
    }
    stateIter->next = NULL;
    stateLinkList = stateIter;
}

void refresh_display_list(bool lastdraw)
{
    unsigned int data = 0, triangle_addr = 0;
    unsigned short tileNo = 0, sn = 0;
    triangle_node TriNodePtr;
    tile_node TileNodePtr;
    RenderState * StatePtr;

    Context * ctx = Context::GetCurrentContext();
    StatePtr = stateIter->next;

    if (ctx->pre_state_counter != ctx->state_counter)
    {
#ifdef RenderStateDEG
        fprintf(Drvdumper, "//------------- State Information from StateLinkedList ------------//\n");
#endif
        while (StatePtr != NULL)
        {
#ifdef RenderStateDEG
            fprintf(Drvdumper, "  ID: 0X%04X  ,  VALUE:0X%08X \n", StatePtr->ID, StatePtr->Value);
#endif
            if (StatePtr->ID == GL_FOG_COLOR || StatePtr->ID == GL_TEXTURE_BINDING_2D)
            {
                for (int i=0;i<TILE_COUNT;i++)
                {
                    add(0x2, (StatePtr->ID)<<16, i);
                    add(0x2, StatePtr->Value, i);
                    TileNodeSize+=2;
                    StateNodeSize+=2;
                }
            }
            else
            {
                for (int i=0;i<TILE_COUNT;i++)
                {
                    add(0x1, (StatePtr->ID)<<16|StatePtr->Value, i);
                    TileNodeSize++;
                    StateNodeSize++;
                }
            }

            StatePtr = StatePtr->next;
        }
        //TODO: Release stateLinkList
        release_state_list();
    }
    ctx->pre_state_counter = ctx->state_counter;

#ifdef RenderStateDEG
    fprintf(Drvdumper, "//-----------------------------------------------------------------//\n\n");
#endif

    if (!lastdraw)
    {
        TriNodePtr = gm.triangle_iterator->next_triangle;
    //    triangle_addr = (unsigned int)(TriNodePtr->VertexDATA);
    //    printf("triangle_addr = %d\n", triangle_addr);
    //    float * iter = (float *)(triangle_addr);
    //    printf("%f\n", *iter);
        if (TriNodePtr == NULL)
        {
            fprintf(Drvdumper, "DrawArrays->refresh_display_list(): No any triangle could be update, it's culled!!\n");
            return;
        }

        while (TriNodePtr != NULL)
        {
            triangle_addr = (unsigned int)(TriNodePtr->VertexDATA);
            TileNodePtr = TriNodePtr->tiledata_ptr;

            while (TileNodePtr != NULL)
            {
                data = TileNodePtr->SN_and_TN;
                sn = data>>16;
                tileNo = (short)data;

                dirtyTileMap[tileNo] = true;
                add(0x3, triangle_addr, tileNo);
                TileNodeSize++;

                TileNodePtr = TileNodePtr->next_tile;
            }

            gm.triangle_iterator = TriNodePtr;
            TriNodePtr = TriNodePtr->next_triangle;
        }
    }

#ifdef DispListDEG
    fprintf(Drvdumper,"TILE Node Count  = %d      \n", TileNodeSize);
    fprintf(Drvdumper,"DisplayList Size = %d bytes\n", TileNodeSize*sizeof(Node));
    fprintf(Drvdumper,"PreState Changes = %d      \n", ctx->pre_state_counter);
    fprintf(Drvdumper,"State Changes    = %d      \n", ctx->state_counter);
    fprintf(Drvdumper,"---------------------------\n");
#endif

}

void tile_render()
{
    Context * ctx = Context::GetCurrentContext();

    /*  read displayList  */
    int i,j, tri_count=0;
	char buf[5];
	char output[32];
	memset(&output, 0, 32);
	snprintf(output, 32, "RenderFrame");

    // temporary solution of state for frame by frame, because some state information only would be set once at first in the application
//    rm.SetStateReg(GL_BLEND, ctx->blend_func.enable);
//    rm.SetStateReg(GL_BLEND_SRC, ctx->blend_func.src);
//    rm.SetStateReg(GL_BLEND_DST, ctx->blend_func.dst);
//    rm.SetStateReg(GL_FOG, ctx->fog.enable);
//    unsigned int cf = __GL_FLOAT_2_S(ctx->fog.cf.red)<<24|__GL_FLOAT_2_S(ctx->fog.cf.green)<<16|__GL_FLOAT_2_S(ctx->fog.cf.blue)<<8|__GL_FLOAT_2_S(ctx->fog.cf.alpha);
//    rm.SetStateReg(GL_FOG_COLOR, cf);
//    rm.SetStateReg(GL_FOG_DENSITY, ctx->fog.density);
//    rm.SetStateReg(GL_FOG_END, ctx->fog.end);
//    rm.SetStateReg(GL_FOG_START, ctx->fog.start);
//    rm.SetStateReg(GL_DEPTH_TEST, ctx->depth_test.enable);
//    rm.SetStateReg(GL_DEPTH_FUNC, ctx->depth_test.func);
//    rm.SetStateReg(GL_ALPHA_TEST, ctx->alpha_test.enable);
//    rm.SetStateReg(GL_ALPHA_TEST_FUNC, ctx->alpha_test.func);
//    rm.SetStateReg(GL_ALPHA_TEST_REF, ctx->alpha_test.ref_val);
//    rm.SetStateReg(GL_MULTISAMPLE, ctx->aa_enable);
//
//    if (ctx->texobj != NULL)
//    {
//        rm.SetStateReg(GL_TEXTURE_2D, static_cast<U32>(ctx->texobj->enable));
//        rm.SetStateReg(GL_TEXTURE_WIDTH, static_cast<U32>(ctx->texobj->teximage->width));
//        rm.SetStateReg(GL_TEXTURE_HEIGHT, static_cast<U32>(ctx->texobj->teximage->height));
//        float level_of_detail = static_cast<float>(ctx->texobj->teximage->level);
//        float c = (ctx->texobj->Mag_filter_mode == GL_LINEAR &&
//                   (ctx->texobj->Min_filter_mode == GL_NEAREST_MIPMAP_NEAREST || ctx->texobj->Min_filter_mode == GL_NEAREST_MIPMAP_LINEAR)) ? 0.5 : 0;
//        if (level_of_detail <= c)
//            rm.SetStateReg(GL_TEXTURE_FILTER, static_cast<U32>(ctx->texobj->Mag_filter_mode));
//        else
//            rm.SetStateReg(GL_TEXTURE_FILTER, static_cast<U32>(ctx->texobj->Min_filter_mode));
//        rm.SetStateReg(GL_TEXTURE_WRAP_S, static_cast<U32>(ctx->texobj->wrap_s));
//        rm.SetStateReg(GL_TEXTURE_WRAP_T, static_cast<U32>(ctx->texobj->wrap_t));
//        rm.SetStateReg(GL_TEXTURE_ENV_MODE, static_cast<U32>(ctx->texobj->env_mode));
//        rm.SetStateReg(GL_TEXTURE_ENV_COLOR, ctx->texobj->env_color);
//        rm.SetStateReg(GL_TEXTURE_BASE, (U32)ctx->texobj->teximage->data);
//        rm.SetStateReg(GL_TEXTURE_FORMAT, ctx->texobj->teximage->internal_format);
//    }

#ifdef TileRenderDEG
    fprintf(Drvdumper, "\n\n");
    fprintf(Drvdumper, "            ********************** \n");
    fprintf(Drvdumper, "            *  Per-tile Process  * \n");
    fprintf(Drvdumper, "            ********************** \n");
#endif

    for (i=0;i<TILE_COUNT;i++)
    {
#ifdef TileRenderDEG
        fprintf(Drvdumper,"##### %d-th TILE #####\n", i);
#endif
//        if (i==6)
//        {
//            rm.blendCount = 0;
//            rm.tileDEG = true;
//        }
//        else
//            rm.tileDEG = false;

        Node * TileIter = displayList[i];

        if (TileIter)
            TileIter = TileIter->next;

        rm.ClearBuffer(ctx->clear_buffer_mask);

        while (TileIter != NULL)
        {

            if (TileIter->kind == 0x1)
            {
                U16 ID = (TileIter->addr)>>16;
                U16 Value = TileIter->addr;
#ifdef TileRenderDEG
                fprintf(Drvdumper, "Set State: ID=0X%04X, VALUE=0X%04X\n", ID, Value);
#endif
//                if (frame_count==137 && i==6)
//                    printf("Set State: ID=0X%04X, VALUE=0X%04X\n", ID, Value);
                rm.SetStateReg(ID, static_cast<U32>(Value));
                StateIO+=8;

            }
            else if (TileIter->kind == 0x2)
            {
                U16 ID = (TileIter->addr)>>16;
                U32 Value;
#ifdef TileRenderDEG
                fprintf(Drvdumper, "Set State: ID=0X%04X, ", ID);
#endif
                TileIter = TileIter->next;
                Value = TileIter->addr;
#ifdef TileRenderDEG
                fprintf(Drvdumper, "VALUE=0X%08X \n", Value);
#endif
                if (ID==GL_TEXTURE_BINDING_2D)
                {
                    //fprintf(Drvdumper, "H:%d, W:%d\n", ctx->texvec[Value].teximage->height, ctx->texvec[Value].teximage->width);
                    rm.SetStateReg(GL_TEXTURE_WIDTH, static_cast<U32>( ctx->texvec[Value].teximage->width));
                    rm.SetStateReg(GL_TEXTURE_HEIGHT, static_cast<U32>( ctx->texvec[Value].teximage->height));
                    float level_of_detail = static_cast<float>( ctx->texvec[Value].teximage->level);
                    float c = ( ctx->texvec[Value].Mag_filter_mode == GL_LINEAR &&
                               ( ctx->texvec[Value].Min_filter_mode == GL_NEAREST_MIPMAP_NEAREST ||  ctx->texvec[Value].Min_filter_mode == GL_NEAREST_MIPMAP_LINEAR)) ? 0.5 : 0;
                    if (level_of_detail <= c)
                        rm.SetStateReg(GL_TEXTURE_FILTER, static_cast<U32>( ctx->texvec[Value].Mag_filter_mode));
                    else
                        rm.SetStateReg(GL_TEXTURE_FILTER, static_cast<U32>( ctx->texvec[Value].Min_filter_mode));
                    rm.SetStateReg(GL_TEXTURE_WRAP_S, static_cast<U32>( ctx->texvec[Value].wrap_s));
                    rm.SetStateReg(GL_TEXTURE_WRAP_T, static_cast<U32>( ctx->texvec[Value].wrap_t));
                    rm.SetStateReg(GL_TEXTURE_ENV_MODE, static_cast<U32>( ctx->texvec[Value].env_mode));
                    rm.SetStateReg(GL_TEXTURE_ENV_COLOR,  ctx->texvec[Value].env_color);
                    rm.SetStateReg(GL_TEXTURE_BASE, (U32) ctx->texvec[Value].teximage->data);
                    rm.SetStateReg(GL_TEXTURE_FORMAT,  ctx->texvec[Value].teximage->internal_format);
                    StateIO+=9*4*2;
                }
                else
                {
                    rm.SetStateReg(ID, static_cast<U32>(Value));
                    StateIO+=8;
                }
            }
            else
            {
//                if (frame_count==137 && i==6)
//                    printf("BlendEn:%d\n", rm.BlendingEn);

                tri_count++;
//if (i==70&&tri_count==26){
                float * vertex_data = (float *)TileIter->addr;
                float A[36];
#ifdef TileRenderDEG
                fprintf(Drvdumper,"//-------------------- %d th Triangle in Tile#%d --------------------//\n", tri_count, i);
                for (j=0;j<3;j++)
                {
                    fprintf(Drvdumper, "V%d: %f %f %f %f\n", j+1, vertex_data[j*12]  , vertex_data[j*12+1], vertex_data[j*12+2] , vertex_data[j*12+3]);
                    fprintf(Drvdumper, "T%d: %f %f %f %f\n", j+1, vertex_data[j*12+4], vertex_data[j*12+5], vertex_data[j*12+6] , vertex_data[j*12+7]);
                    fprintf(Drvdumper, "C%d: %f %f %f %f\n", j+1, vertex_data[j*12+8], vertex_data[j*12+9], vertex_data[j*12+10], vertex_data[j*12+11]);
                }

                fprintf(Drvdumper,"//-----------------------------------------------------------------//\n\n");
#endif
                for (j=0;j<36;j++)
                {
                    if (j%12==8 || j%12==9 || j%12==10 || j%12==11)
                    {
                        A[j] = 255 * vertex_data[j];
                    }
                    else
                    {
                        A[j] = vertex_data[j];
                    }

                }

                rm.TriangleSetup(A, A+12, A+24);
                rm.PixelGenerate(A, A+12, A+24);
                if (rm.TexMappingEn)
                    rm.TextureMapping();
                rm.PerFragmentOp();

                TriangleIO+=60;
//}
            }
            TileIter = TileIter->next;

//            if (frame_count==137 && i==6)
//                printf("BlendCount:%d\n", rm.blendCount);
        }

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
		rm.EndTile(ctx->m_draw_surface->GetColorBuffer());
#elif defined(EGL_ON_LINUX)
        rm.EndTile(0);
#endif
        tri_count = 0;
    }

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
    ctx->m_draw_surface->updateFrameBuffer();
#elif defined(EGL_ON_LINUX)
#endif

    fprintf(RMdumper, "=== %d-th Frame Simulation Statistics ===\n", frame_count);
    fprintf(RMdumper, "Num of ALL TILES         : %d      \n" , TILE_COUNT);
    fprintf(RMdumper, "Num of Input Vertex      : %d      \n" , VertexCount);
    fprintf(RMdumper, "Processed Triangles      : %d      \n" , gm.AvailableTriangleCounter);
    fprintf(RMdumper, "Culled Triangles         : %d      \n" , gm.cullcount);
    int coveredTiles = 0;
    for (int i=0;i<TILE_COUNT;i++)
        if (dirtyTileMap[i])
            coveredTiles++;
    fprintf(RMdumper, "Covered Tiles            : %d      \n" , coveredTiles);
    if (KICKSMALLAREA)
        fprintf(RMdumper, "Removed Small Triangles  : %d  \n" , gm.kick_small_area_count);
    fprintf(RMdumper, "Triangle Size            : %d bytes\n" , gm.triangleSize);
    fprintf(RMdumper, "TriangleList Size        : %d bytes\n" , gm.triangleListSize);
    fprintf(RMdumper, "Num of Tile Node         : %d      \n" , TileNodeSize);
    fprintf(RMdumper, "DisplayList Size         : %d bytes\n" , TileNodeSize*sizeof(Node));
    fprintf(RMdumper, "Num of State Node        : %d      \n" , StateNodeSize);
    fprintf(RMdumper, "StateList Size           : %d bytes\n" , StateNodeSize*sizeof(unsigned int));
    fprintf(RMdumper, "-----------------------------------\n");
    fprintf(RMdumper, "Texture hit              : %d\n"
            "Texture miss             : %d\n"
            "Texture hit rate         : %2.3f\n",
            rm.TexCache.TexCacheHit,
            rm.TexCache.TexCacheMiss,
            static_cast<double>(rm.TexCache.TexCacheHit*100)/(rm.TexCache.TexCacheHit+rm.TexCache.TexCacheMiss));

    fprintf(RMdumper, "Visited pixels           : %d \n", rm.scanpixel);
    fprintf(RMdumper, "Processed pixels (valid) : %d \n", rm.validpixel);
    fprintf(RMdumper, "Shaded pixels            : %d \n", rm.shadedpixel);
    fprintf(RMdumper, "-----------------------------------\n");
    if (USE_PIXELSTAMP)
    {
        fprintf(RMdumper, "@Use Plane equation method (for 2x2 stamp pattern)\n");
        fprintf(RMdumper, "Visited stamps           : %d \n", rm.scanstamp);
        fprintf(RMdumper, "Processed stamps (valid) : %d \n", rm.validstamp);
        fprintf(RMdumper, "Failed stamps (invalid)  : %d \n", rm.invalidstamp);
        fprintf(RMdumper, "Failed stamps before HIT : %d \n", rm.coldscan);
        fprintf(RMdumper, "Valid stamp rate         : %2.2f\n"
                "Cold scan miss rate      : %2.2f\n",
                (float)rm.validstamp/rm.scanstamp*100,
                (float)rm.coldscan/rm.invalidstamp*100);
        fprintf(RMdumper, "Average scan rate        : %2.2f\n",static_cast<double>(rm.validpixel)/rm.scanpixel*100.0);
    }
    else
    {
        fprintf(RMdumper, "@Use Barycentric method (no stamp)\n");
        fprintf(RMdumper, "Average scan rate        : %2.2f\n",static_cast<double>(rm.validpixel)/rm.scanpixel*100.0);
    }
    fprintf(RMdumper, "-----------------------------------\n");
    fprintf(RMdumper, "StateIO BW      : %d bytes\n", StateIO);
    fprintf(RMdumper, "TriangleIO BW   : %d bytes\n", TriangleIO);
    fprintf(RMdumper, "Depth buffer BW : %d [R] + %d [W] = %5.3f Mbytes\n", rm.depthBufferBW[0], rm.depthBufferBW[1],
        static_cast<double>(rm.depthBufferBW[0]+rm.depthBufferBW[1])/(1024.0*1024.0) );
    fprintf(RMdumper, "Color buffer BW : %d [R] + %d [W] = %5.3f Mbytes\n", rm.colorBufferBW[0], rm.colorBufferBW[1],
        static_cast<double>(rm.colorBufferBW[0]+rm.colorBufferBW[1])/(1024.0*1024.0) );
    fprintf(RMdumper, "Texture Read    : %d = %5.3f Mbytes\n", rm.textureBW, static_cast<double>(rm.textureBW)/(1024.0*1024.0) );
    float TR = static_cast<double>(rm.colorBufferBW[0]+rm.depthBufferBW[0]+rm.textureBW)/(1024.0*1024.0);
    float TW = static_cast<double>(rm.colorBufferBW[1]+rm.depthBufferBW[1])/(1024.0*1024.0);
    fprintf(RMdumper, "Total bandwidth : %5.3f [R] + %5.3f [W] + %5.3f [C] = %5.3f Mbytes\n\n", TR, TW, 0.0, TR+TW);

    //printf("%d %d %d %d\n", rm.scanstamp, rm.validstamp, rm.scanstamp-rm.validstamp, rm.invalidstamp);

    // save statistics to context
    ctx->measure_stat.num_of_tiles = TILE_COUNT;
    ctx->measure_stat.num_of_vertex = VertexCount;
    ctx->measure_stat.num_of_triangle = gm.AvailableTriangleCounter;
    ctx->measure_stat.culled_triangles = gm.cullcount;
    ctx->measure_stat.removed_kick_triangles = gm.kick_small_area_count;
    ctx->measure_stat.covered_tiles = coveredTiles;
    ctx->measure_stat.triangle_size = gm.triangleSize;
    ctx->measure_stat.triangle_list_size = gm.triangleListSize;
    ctx->measure_stat.num_of_tile_node = TileNodeSize;
    ctx->measure_stat.tiled_list_size = TileNodeSize*sizeof(Node);
    ctx->measure_stat.num_of_state_node = StateNodeSize;
    ctx->measure_stat.state_list_size = StateNodeSize*sizeof(RenderState);
    ctx->measure_stat.tex_cache_hit = rm.TexCache.TexCacheHit;
    ctx->measure_stat.tex_cache_miss = rm.TexCache.TexCacheMiss;
    ctx->measure_stat.tex_cache_hitrate = static_cast<double>(rm.TexCache.TexCacheHit*100)/(rm.TexCache.TexCacheHit+rm.TexCache.TexCacheMiss);
    ctx->measure_stat.visited_pixels = rm.scanpixel;
    ctx->measure_stat.processed_pixels = rm.validpixel;
    ctx->measure_stat.shaded_pixels = rm.shadedpixel;
    ctx->measure_stat.average_scan_rate = static_cast<double>(rm.validpixel)/rm.scanpixel*100.0;
    ctx->measure_stat.state_raster_bw = StateIO;
    ctx->measure_stat.triangle_raster_bw = TriangleIO;
    ctx->measure_stat.depth_buffer_bw[0] = rm.depthBufferBW[0];
    ctx->measure_stat.depth_buffer_bw[1] = rm.depthBufferBW[1];
    ctx->measure_stat.color_buffer_bw[0] = rm.colorBufferBW[0];
    ctx->measure_stat.color_buffer_bw[1] = rm.colorBufferBW[1];
    ctx->measure_stat.texture_mem_read = rm.textureBW;
    ctx->measure_stat.total_bandwidth = rm.depthBufferBW[0]+rm.colorBufferBW[0]+rm.textureBW+rm.depthBufferBW[1]+rm.colorBufferBW[1];

#ifdef GENERATE_STAT_CSV
    fprintf(CSVhanlder, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
                        "%2.2f,%d,%d,%d,%2.2f,%d,%d,%d,%d,%d,%d,%d,%d\n"
                        , frame_count
                        , ctx->measure_stat.num_of_tiles
                        , ctx->measure_stat.num_of_vertex
                        , ctx->measure_stat.num_of_triangle
                        , ctx->measure_stat.culled_triangles
                        , ctx->measure_stat.covered_tiles
                        , ctx->measure_stat.removed_kick_triangles
                        , ctx->measure_stat.triangle_size
                        , ctx->measure_stat.triangle_list_size
                        , ctx->measure_stat.num_of_tile_node
                        , ctx->measure_stat.tiled_list_size
                        , ctx->measure_stat.num_of_state_node
                        , ctx->measure_stat.state_list_size
                        , ctx->measure_stat.tex_cache_hit
                        , ctx->measure_stat.tex_cache_miss

                        , ctx->measure_stat.tex_cache_hitrate
                        , ctx->measure_stat.visited_pixels
                        , ctx->measure_stat.processed_pixels
                        , ctx->measure_stat.shaded_pixels
                        , ctx->measure_stat.average_scan_rate
                        , ctx->measure_stat.state_raster_bw
                        , ctx->measure_stat.triangle_raster_bw
                        , ctx->measure_stat.depth_buffer_bw[0]
                        , ctx->measure_stat.depth_buffer_bw[1]
                        , ctx->measure_stat.color_buffer_bw[0]
                        , ctx->measure_stat.color_buffer_bw[1]
                        , ctx->measure_stat.texture_mem_read
                        , ctx->measure_stat.total_bandwidth);
#endif

    if (FRAMEDUMP)
    {
        itoa(frame_count, buf, 10);
        strcat(output, buf);
        strcat(output, ".bmp");
        rm.DumpImage(1, output); // image type => 1:BMP ; 2:RAW
    }

    if (TEXDEBUG)
        fclose(rm.TEXDEBUGfp);
    if (PIXEL_GENERATE_DEBUG)
        fclose(rm.PIXEL_GENERATE_DEBUGfp);
    if (SCAN_ALGO_DEBUG)
        fclose(rm.SCAN_ALGO_DEBUGfp);
    // Release Triangle & Tiled list
    gm.release_triangle_list();
    gm.triangle_iterator = gm.triangle_list;
    release_display_list();

    // Clear Statistics per frame
    VertexCount = 0;
    TileNodeSize = 0;
    StateNodeSize = 0;
    StateIO = 0;
    TriangleIO = 0;
    for (int i=0;i<TILE_COUNT;i++)
        dirtyTileMap[i] = false;

    ctx->pre_state_counter = 0;
    ctx->state_counter = 0;
    ctx->texture_total_seq = 0;

    gm.ClearStatistics();
    rm.ClearStatistics();

    isRenderDone = true;
}

void draw_init_table(void)
{
    Context * ctx = Context::GetCurrentContext();
    int i;

    /*  init geometry module  */

    //  set modelview matrix
    for (i=0;i<16;i++)
    {
        gm.ModelViewMat[i] = ctx->m_modelview_stack.CurrentMatrix().element[i];
    }

    //  set projection matrix
    for (i=0;i<16;i++)
    {
        gm.ProjectionMat[i] = ctx->m_projection_stack.CurrentMatrix().element[i];
    }

    //  set texture matrix
    for (i=0;i<16;i++)
    {
        gm.TextureMat[i] = ctx->m_texture_stack[ctx->getActiveTexture()].CurrentMatrix().element[i];
    }

#ifdef CTableDEG
//    if (frame_count==199)
//    {
        Matrix3x3 &mtx = ctx->m_inversed_modelview;
        fprintf(Ctxdumper,"Initialize various Matrices in a Draw Command ...\n");
        fprintf(Ctxdumper,"---------------------------- Inversed M --------------------------------\n");
        fprintf(Ctxdumper,"(%f , %f , %f)\n", mtx.element[ELM3(0, 0)], mtx.element[ELM3(0, 1)], mtx.element[ELM3(0, 2)]);
        fprintf(Ctxdumper,"(%f , %f , %f)\n", mtx.element[ELM3(1, 0)], mtx.element[ELM3(1, 1)], mtx.element[ELM3(1, 2)]);
        fprintf(Ctxdumper,"(%f , %f , %f)\n", mtx.element[ELM3(2, 0)], mtx.element[ELM3(2, 1)], mtx.element[ELM3(2, 2)]);
        fprintf(Ctxdumper,"------------------------------------------------------------------------\n");

        Matrix4x4& mv = ctx->m_modelview_stack.CurrentMatrix();
        fprintf(Ctxdumper,"---------------------------- ModelView Matrix ----------------------------\n");
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mv.element[ELM4(0, 0)], mv.element[ELM4(0, 1)], mv.element[ELM4(0, 2)], mv.element[ELM4(0, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mv.element[ELM4(1, 0)], mv.element[ELM4(1, 1)], mv.element[ELM4(1, 2)], mv.element[ELM4(1, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mv.element[ELM4(2, 0)], mv.element[ELM4(2, 1)], mv.element[ELM4(2, 2)], mv.element[ELM4(2, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", mv.element[ELM4(3, 0)], mv.element[ELM4(3, 1)], mv.element[ELM4(3, 2)], mv.element[ELM4(3, 3)]);
        fprintf(Ctxdumper,"-----------------------------------------------------------------------------------\n");

        Matrix4x4& m = ctx->m_projection_stack.CurrentMatrix();
        fprintf(Ctxdumper,"---------------------------- Projection Matrix ----------------------------\n");
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", m.element[ELM4(0, 0)], m.element[ELM4(0, 1)], m.element[ELM4(0, 2)], m.element[ELM4(0, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", m.element[ELM4(1, 0)], m.element[ELM4(1, 1)], m.element[ELM4(1, 2)], m.element[ELM4(1, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", m.element[ELM4(2, 0)], m.element[ELM4(2, 1)], m.element[ELM4(2, 2)], m.element[ELM4(2, 3)]);
        fprintf(Ctxdumper,"(%f , %f , %f , %f)\n", m.element[ELM4(3, 0)], m.element[ELM4(3, 1)], m.element[ELM4(3, 2)], m.element[ELM4(3, 3)]);
        fprintf(Ctxdumper,"-----------------------------------------------------------------------------------\n");
//    }
#endif

    //  set inversed matrix
    for (i=0;i<9;i++)
    {
        gm.InverseM[i] = ctx->m_inversed_modelview.element[i];
    }

    //  set normalize enable
    gm.Normalize = ctx->normalize_enable;

    //  set rescale normal enable
    gm.RescaleNormal = ctx->rescale_normal_enable;

    //  set lihgting enable
    gm.Lighting = ctx->lighting_enable;

    //  set materials
    gm.MatAmbient[0] = ctx->m_light.material_ambient.red;
    gm.MatAmbient[1] = ctx->m_light.material_ambient.green;
    gm.MatAmbient[2] = ctx->m_light.material_ambient.blue;
    gm.MatAmbient[3] = ctx->m_light.material_ambient.alpha;

    gm.MatDiffuse[0] = ctx->m_light.material_diffuse.red;
    gm.MatDiffuse[1] = ctx->m_light.material_diffuse.green;
    gm.MatDiffuse[2] = ctx->m_light.material_diffuse.blue;
    gm.MatDiffuse[3] = ctx->m_light.material_diffuse.alpha;

    gm.MatSpecular[0] = ctx->m_light.material_specular.red;
    gm.MatSpecular[1] = ctx->m_light.material_specular.green;
    gm.MatSpecular[2] = ctx->m_light.material_specular.blue;
    gm.MatSpecular[3] = ctx->m_light.material_specular.alpha;

    gm.MatEmission[0] = ctx->m_light.material_emission.red;
    gm.MatEmission[1] = ctx->m_light.material_emission.green;
    gm.MatEmission[2] = ctx->m_light.material_emission.blue;
    gm.MatEmission[3] = ctx->m_light.material_emission.alpha;

    gm.Shininess = ctx->m_light.material_shininess;

    //  set sources
    gm.LiAmbient[0] = ctx->s_light[0].source_ambient.red;
    gm.LiAmbient[1] = ctx->s_light[0].source_ambient.green;
    gm.LiAmbient[2] = ctx->s_light[0].source_ambient.blue;
    gm.LiAmbient[3] = ctx->s_light[0].source_ambient.alpha;

    gm.LiDiffuse[0] = ctx->s_light[0].source_diffuse.red;
    gm.LiDiffuse[1] = ctx->s_light[0].source_diffuse.green;
    gm.LiDiffuse[2] = ctx->s_light[0].source_diffuse.blue;
    gm.LiDiffuse[3] = ctx->s_light[0].source_diffuse.alpha;

    gm.LiSpecular[0] = ctx->s_light[0].source_specular.red;
    gm.LiSpecular[0] = ctx->s_light[0].source_specular.green;
    gm.LiSpecular[0] = ctx->s_light[0].source_specular.blue;
    gm.LiSpecular[0] = ctx->s_light[0].source_specular.alpha;

    gm.Position[0] = ctx->s_light[0].source_position.x;
    gm.Position[1] = ctx->s_light[0].source_position.y;
    gm.Position[2] = ctx->s_light[0].source_position.z;
    gm.Position[3] = ctx->s_light[0].source_position.w;

    //  set lighting model parameters
    gm.LightModelAmbient[0] = ctx->light_model_ambient.red;
    gm.LightModelAmbient[1] = ctx->light_model_ambient.green;
    gm.LightModelAmbient[2] = ctx->light_model_ambient.blue;
    gm.LightModelAmbient[3] = ctx->light_model_ambient.alpha;


    //  set cullface enable
    gm.CullFace = ctx->culling_enable;

    //  set cullface mode
    gm.CullFaceMode = ctx->cull_face;

    //  set front face
    gm.FrontFace = ctx->front_face;

    //  set drawarrays mode
    gm.DrawMode = ctx->draw_mode;

    //  set view port parameters
    gm.ViewportOrigin[0] = ctx->vp.Ox;
    gm.ViewportOrigin[1] = ctx->vp.Oy;
    gm.ViewportScale[0] = ctx->vp.width/2;
    gm.ViewportScale[1] = ctx->vp.height/2;
    gm.DepthRangeFactor = (ctx->vp.dp_range_far - ctx->vp.dp_range_near)/2;
    gm.DepthRangeBase = (ctx->vp.dp_range_far + ctx->vp.dp_range_near)/2;

    gm.ShadeModel = ctx->shade_model;

    gm.initialize();
}

void drawarrays_fixed_pipeline(void)
{
    Context * ctx = Context::GetCurrentContext();

    int current_index = 0;

    GLfixed * p_fixed_vertex = NULL;
    GLshort * p_short_vertex = NULL;
    GLbyte  * p_byte_vertex = NULL;
    GLfloat * p_float_vertex = NULL;

    GLfixed * p_fixed_normal = NULL;
    GLshort * p_short_normal = NULL;
    GLbyte  * p_byte_normal = NULL;
    GLfloat * p_float_normal = NULL;

    GLfixed * p_fixed_color = NULL;
    GLubyte * p_ubyte_color = NULL;
    GLfloat * p_float_color = NULL;

    GLfixed * p_fixed_texcoord = NULL;
    GLshort * p_short_texcoord = NULL;
    GLbyte  * p_byte_texcoord = NULL;
    GLfloat * p_float_texcoord = NULL;

    if (ctx->vertex_array.enable != GL_TRUE)
    {
        printf("PIPE error: Without enabling GL_VERTEX_ARRAY. \n");
        return;
    }

    switch (ctx->vertex_array.type)
    {
    case GL_FIXED:
        p_fixed_vertex = (GLfixed*)ctx->vertex_array.ptr;
        break;

    case GL_SHORT:
        p_short_vertex = (GLshort*)ctx->vertex_array.ptr;
        break;

    case GL_BYTE:
        p_byte_vertex = (GLbyte*)ctx->vertex_array.ptr;
        break;

    case GL_FLOAT:
        p_float_vertex = (GLfloat*)ctx->vertex_array.ptr;
        break;
    }

    if (ctx->normal_array.enable == GL_TRUE)
    {
        switch (ctx->normal_array.type)
        {
        case GL_FIXED:
            p_fixed_normal = (GLfixed*)ctx->normal_array.ptr;
            break;

        case GL_SHORT:
            p_short_normal = (GLshort*)ctx->normal_array.ptr;
            break;

        case GL_BYTE:
            p_byte_normal = (GLbyte*)ctx->normal_array.ptr;
            break;

        case GL_FLOAT:
            p_float_normal = (GLfloat*)ctx->normal_array.ptr;
            break;
        }
    }

    if (ctx->color_array.enable == GL_TRUE)
    {
        switch (ctx->color_array.type)
        {
        case GL_FIXED:
            p_fixed_color = (GLfixed*)ctx->color_array.ptr;
            break;

        case GL_UNSIGNED_BYTE:
            p_ubyte_color = (GLubyte*)ctx->color_array.ptr;
            break;

        case GL_FLOAT:
            p_float_color = (GLfloat*)ctx->color_array.ptr;
            break;
        }
    }

    if (ctx->texture_coord_array.enable == GL_TRUE)
    {
        switch (ctx->texture_coord_array.type)
        {
        case GL_FIXED:
            p_fixed_texcoord = (GLfixed*)ctx->texture_coord_array.ptr;
            break;

        case GL_SHORT:
            p_short_texcoord = (GLshort*)ctx->texture_coord_array.ptr;
            break;

        case GL_BYTE:
            p_byte_texcoord = (GLbyte*)ctx->texture_coord_array.ptr;
            break;

        case GL_FLOAT:
            p_float_texcoord = (GLfloat*)ctx->texture_coord_array.ptr;
            break;
        }
    }

    /*  Loop of entering GPU simulator and set vertex's data one by one  */
    int i,index;
    index = ctx->draw_array.first;
    for (i=index; i<ctx->draw_array.count+index; i++)
    {
#ifdef __GL_FLOAT
        GLfloat vx, vy, vz, vw;
        GLfloat cr, cg, cb, ca;
        GLfloat nx, ny, nz;
        GLfloat s, t, r, q;
#else
        GLfixed vx, vy, vz, vw;
        GLfixed cr, cb, cg, ca;
        GLfixed nx, ny, nz;
        GLfixed s, t, r, q;
#endif

        switch (ctx->vertex_array.size)
        {
        case 4:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
            {
                Fixed x(p_fixed_vertex[i*4]);
                Fixed y(p_fixed_vertex[i*4+1]);
                Fixed z(p_fixed_vertex[i*4+2]);
                Fixed w(p_fixed_vertex[i*4+3]);

                vx = x;
                vy = y;
                vz = z;
                vw = w;
                break;
            }
            case GL_SHORT:
//                        vx = __GL_INT_2_X(p_short_vertex[i*4]);
//                        vy = __GL_INT_2_X(p_short_vertex[i*4+1]);
//                        vz = __GL_INT_2_X(p_short_vertex[i*4+2]);
//                        vw = __GL_INT_2_X(p_short_vertex[i*4+3]);
                break;

            case GL_BYTE:
//                        vx = __GL_INT_2_X(p_byte_vertex[i*4]);
//                        vy = __GL_INT_2_X(p_byte_vertex[i*4+1]);
//                        vz = __GL_INT_2_X(p_byte_vertex[i*4+2]);
//                        vw = __GL_INT_2_X(p_byte_vertex[i*4+3]);
                break;

            case GL_FLOAT:
                vx = p_float_vertex[i*4];
                vy = p_float_vertex[i*4+1];
                vz = p_float_vertex[i*4+2];
                vw = p_float_vertex[i*4+3];
                break;
            }
            break;

        case 3:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
            {
                Fixed x(p_fixed_vertex[i*3]);
                Fixed y(p_fixed_vertex[i*3+1]);
                Fixed z(p_fixed_vertex[i*3+2]);

                vx = x;
                vy = y;
                vz = z;
                vw = 1.f;
                break;
            }
            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                vx = p_float_vertex[i*3];
                vy = p_float_vertex[i*3+1];
                vz = p_float_vertex[i*3+2];
                vw = 1.f;
                break;
            }
            break;

        case 2:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
            {
                Fixed x(p_fixed_vertex[i*2]);
                Fixed y(p_fixed_vertex[i*2+1]);

                vx = x;
                vy = y;
                vz = 0.f;
                vw = 1.f;
                break;
            }
            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                vx = p_float_vertex[i*2];
                vy = p_float_vertex[i*2+1];
                vz = 0.f;
                vw = 1.f;
                break;
            }
            break;

        }

        if (ctx->normal_array.enable == GL_TRUE)
        {
            switch (ctx->normal_array.type)
            {
            case GL_FIXED:
            {
                Fixed n_x(p_fixed_normal[i*3]);
                Fixed n_y(p_fixed_normal[i*3+1]);
                Fixed n_z(p_fixed_normal[i*3+2]);

                nx = n_x;
                ny = n_y;
                nz = n_z;
                break;
            }
            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                nx = p_float_normal[i*3];
                ny = p_float_normal[i*3+1];
                nz = p_float_normal[i*3+2];
                break;

            }
        }
        else
        {
            nx = ctx->current_normal.nx;
            ny = ctx->current_normal.ny;
            nz = ctx->current_normal.nz;
        }

        if (ctx->color_array.enable == GL_TRUE)
        {
            switch (ctx->color_array.type)
            {
            case GL_FIXED:
                break;

            case GL_UNSIGNED_BYTE:
            {
                GLfloat r = __GL_S_2_FLOAT(p_ubyte_color[i*4]);
                GLfloat g = __GL_S_2_FLOAT(p_ubyte_color[i*4+1]);
                GLfloat b = __GL_S_2_FLOAT(p_ubyte_color[i*4+2]);
                GLfloat a = __GL_S_2_FLOAT(p_ubyte_color[i*4+3]);

                cr = r;
                cg = g;
                cb = b;
                ca = a;
                break;
            }
            case GL_FLOAT:
                cr = p_float_color[i*4];
                cg = p_float_color[i*4+1];
                cb = p_float_color[i*4+2];
                ca = p_float_color[i*4+3];
                break;
            }
        }
        else
        {
            cr = ctx->current_color.red;
            cg = ctx->current_color.green;
            cb = ctx->current_color.blue;
            ca = ctx->current_color.alpha;
        }

        if (ctx->texture_coord_array.enable == GL_TRUE)
        {
            switch (ctx->texture_coord_array.size)
            {
            case 4:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[i*4];
                    t = p_float_texcoord[i*4+1];
                    r = p_float_texcoord[i*4+2];
                    q = p_float_texcoord[i*4+3];
                    break;
                }
                break;

            case 3:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[i*3];
                    t = p_float_texcoord[i*3+1];
                    r = p_float_texcoord[i*3+2];
                    q = 1.f;
                    break;
                }
                break;

            case 2:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[i*2];
                    t = p_float_texcoord[i*2+1];
                    r = 0.f;
                    q = 1.f;
                    break;
                }
                break;
            }
        }
        else
        {
            s = ctx->current_texture_coords.tu;
            t = ctx->current_texture_coords.tv;
            r = 0.f;
            q = 1.f;
        }

        if (ctx->color_material_enable == GL_TRUE)
        {
            gm.MatAmbient[0] = cr;
            gm.MatAmbient[1] = cg;
            gm.MatAmbient[2] = cb;
            gm.MatAmbient[3] = ca;

            gm.MatDiffuse[0] = cr;
            gm.MatDiffuse[1] = cg;
            gm.MatDiffuse[2] = cb;
            gm.MatDiffuse[3] = ca;
        }

        /*  put vertex data into HW input buffers  */
        gm.InputVertex[0] = vx;
        gm.InputVertex[1] = vy;
        gm.InputVertex[2] = vz;
        gm.InputVertex[3] = vw;

        gm.InputNormal[0] = nx;
        gm.InputNormal[1] = ny;
        gm.InputNormal[2] = nz;

        gm.InputColor[0] = cr;
        gm.InputColor[1] = cg;
        gm.InputColor[2] = cb;
        gm.InputColor[3] = ca;

        gm.InputTexCoord[0] = s;
        gm.InputTexCoord[1] = t;
        gm.InputTexCoord[2] = r;
        gm.InputTexCoord[3] = q;

//        if (frame_count==131)
//        {
//            gm.frameDEG = true;
//            //fprintf(Drvdumper, "X:%f Y:%f\n", vx, vy);
//            fprintf(GMdumper, "Target Frame:%d\n", frame_count);
//        }
//        else
//        {
//            gm.frameDEG = false;
//        }

        gm.do_GM();
        VertexCount++;

#ifdef DrawArrayDEG
        fprintf(Drvdumper,"//-------------------- %d th Vertex  --------------------//\n", i+1);
        fprintf(Drvdumper, "V:(%f, %f, %f, %f)\n", vx, vy, vz, vw);
        fprintf(Drvdumper, "C:(%f, %f, %f, %f)\n", cr, cg, cb, ca);
        fprintf(Drvdumper, "N:(%f, %f, %f)\n", nx, ny, nz);
        fprintf(Drvdumper, "T:(%f, %f. %f, %f)\n", s, t, r, q);
#endif

    } // end of DrawArrays Loop

#ifdef DrawArrayDEG
    fprintf(Drvdumper,"##-------------------- end of DrawArrays  --------------------##\n\n");
#endif

}

// for DrawElements
void drawelements_fixed_pipeline(void)
{
    Context * ctx = Context::GetCurrentContext();

    int current_index = 0;

    GLfixed * p_fixed_vertex = NULL;
    GLshort * p_short_vertex = NULL;
    GLbyte  * p_byte_vertex = NULL;
    GLfloat * p_float_vertex = NULL;

    GLfixed * p_fixed_normal = NULL;
    GLshort * p_short_normal = NULL;
    GLbyte  * p_byte_normal = NULL;
    GLfloat * p_float_normal = NULL;

    GLfixed * p_fixed_color = NULL;
    GLubyte * p_ubyte_color = NULL;
    GLfloat * p_float_color = NULL;

    GLfixed * p_fixed_texcoord = NULL;
    GLshort * p_short_texcoord = NULL;
    GLbyte  * p_byte_texcoord = NULL;
    GLfloat * p_float_texcoord = NULL;

    if (ctx->vertex_array.enable != GL_TRUE)
    {
        printf("PIPE error: Without enabling GL_VERTEX_ARRAY. \n");
        return;
    }

    switch (ctx->vertex_array.type)
    {
    case GL_FIXED:
        p_fixed_vertex = (GLfixed*)ctx->vertex_array.ptr;
        break;

    case GL_SHORT:
        p_short_vertex = (GLshort*)ctx->vertex_array.ptr;
        break;

    case GL_BYTE:
        p_byte_vertex = (GLbyte*)ctx->vertex_array.ptr;
        break;

    case GL_FLOAT:
        p_float_vertex = (GLfloat*)ctx->vertex_array.ptr;
        break;
    }

    if (ctx->normal_array.enable == GL_TRUE)
    {
        switch (ctx->normal_array.type)
        {
        case GL_FIXED:
            p_fixed_normal = (GLfixed*)ctx->normal_array.ptr;
            break;

        case GL_SHORT:
            p_short_normal = (GLshort*)ctx->normal_array.ptr;
            break;

        case GL_BYTE:
            p_byte_normal = (GLbyte*)ctx->normal_array.ptr;
            break;

        case GL_FLOAT:
            p_float_normal = (GLfloat*)ctx->normal_array.ptr;
            break;
        }
    }

    if (ctx->color_array.enable == GL_TRUE)
    {
        switch (ctx->color_array.type)
        {
        case GL_FIXED:
            p_fixed_color = (GLfixed*)ctx->color_array.ptr;
            break;

        case GL_UNSIGNED_BYTE:
            p_ubyte_color = (GLubyte*)ctx->color_array.ptr;
            break;

        case GL_FLOAT:
            p_float_color = (GLfloat*)ctx->color_array.ptr;
            break;
        }
    }

    if (ctx->texture_coord_array.enable == GL_TRUE)
    {
        switch (ctx->texture_coord_array.type)
        {
        case GL_FIXED:
            p_fixed_texcoord = (GLfixed*)ctx->texture_coord_array.ptr;
            break;

        case GL_SHORT:
            p_short_texcoord = (GLshort*)ctx->texture_coord_array.ptr;
            break;

        case GL_BYTE:
            p_byte_texcoord = (GLbyte*)ctx->texture_coord_array.ptr;
            break;

        case GL_FLOAT:
            p_float_texcoord = (GLfloat*)ctx->texture_coord_array.ptr;
            break;
        }
    }

    /*  Loop of entering GPU simulator and set vertex's data one by one  */
    int i;
    for (i=0; i<ctx->draw_element.count; i++)
    {
#ifdef __GL_FLOAT
        GLfloat vx, vy, vz, vw;
        GLfloat cr, cg, cb, ca;
        GLfloat nx, ny, nz;
        GLfloat s, t, r, q;
#else
        GLfixed vx, vy, vz, vw;
        GLfixed cr, cb, cg, ca;
        GLfixed nx, ny, nz;
        GLfixed s, t, r, q;
#endif

        unsigned short index;
        if (ctx->draw_element.type == GL_UNSIGNED_BYTE)
        {
            GLubyte * ptr = (GLubyte*)(ctx->draw_element.indices);
            index = *(ptr+i);
        }
        else
        {
            GLushort * ptr = (GLushort*)(ctx->draw_element.indices);
            index = *(ptr+i);
        }

        fprintf(Drvdumper,"face : %d\n",index);


        switch (ctx->vertex_array.size)
        {
        case 4:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
//                        vx = p_fixed_vertex[i*4];
//                        vy = p_fixed_vertex[i*4+1];
//                        vz = p_fixed_vertex[i*4+2];
//                        vw = p_fixed_vertex[i*4+3];
                break;

            case GL_SHORT:
//                        vx = __GL_INT_2_X(p_short_vertex[i*4]);
//                        vy = __GL_INT_2_X(p_short_vertex[i*4+1]);
//                        vz = __GL_INT_2_X(p_short_vertex[i*4+2]);
//                        vw = __GL_INT_2_X(p_short_vertex[i*4+3]);
                break;

            case GL_BYTE:
//                        vx = __GL_INT_2_X(p_byte_vertex[i*4]);
//                        vy = __GL_INT_2_X(p_byte_vertex[i*4+1]);
//                        vz = __GL_INT_2_X(p_byte_vertex[i*4+2]);
//                        vw = __GL_INT_2_X(p_byte_vertex[i*4+3]);
                break;

            case GL_FLOAT:
                vx = p_float_vertex[index*4];
                vy = p_float_vertex[index*4+1];
                vz = p_float_vertex[index*4+2];
                vw = p_float_vertex[index*4+3];
                break;
            }
            break;

        case 3:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
                break;

            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                vx = p_float_vertex[index*3];
                vy = p_float_vertex[index*3+1];
                vz = p_float_vertex[index*3+2];
                vw = 1.f;
                break;
            }
            break;

        case 2:
            switch (ctx->vertex_array.type)
            {
            case GL_FIXED:
                break;

            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                vx = p_float_vertex[index*2];
                vy = p_float_vertex[index*2+1];
                vz = 0.f;
                vw = 1.f;
                break;
            }
            break;

        }

        if (ctx->normal_array.enable == GL_TRUE)
        {
            switch (ctx->normal_array.type)
            {
            case GL_FIXED:
                break;

            case GL_SHORT:
                break;

            case GL_BYTE:
                break;

            case GL_FLOAT:
                nx = p_float_normal[index*3];
                ny = p_float_normal[index*3+1];
                nz = p_float_normal[index*3+2];
                break;

            }
        }
        else
        {
            nx = ctx->current_normal.nx;
            ny = ctx->current_normal.ny;
            nz = ctx->current_normal.nz;
        }

        if (ctx->color_array.enable == GL_TRUE)
        {
            switch (ctx->color_array.type)
            {
            case GL_FIXED:
                break;

            case GL_UNSIGNED_BYTE:
                break;

            case GL_FLOAT:
                cr = p_float_color[index*4];
                cg = p_float_color[index*4+1];
                cb = p_float_color[index*4+2];
                ca = p_float_color[index*4+3];
                break;
            }
        }
        else
        {
            cr = ctx->current_color.red;
            cg = ctx->current_color.green;
            cb = ctx->current_color.blue;
            ca = ctx->current_color.alpha;
        }

        if (ctx->texture_coord_array.enable == GL_TRUE)
        {
            switch (ctx->texture_coord_array.size)
            {
            case 4:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[index*4];
                    t = p_float_texcoord[index*4+1];
                    r = p_float_texcoord[index*4+2];
                    q = p_float_texcoord[index*4+3];
                    break;
                }
                break;

            case 3:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[index*3];
                    t = p_float_texcoord[index*3+1];
                    r = p_float_texcoord[index*3+2];
                    q = 1.f;
                    break;
                }
                break;

            case 2:
                switch (ctx->texture_coord_array.type)
                {
                case GL_FIXED:
                    break;

                case GL_SHORT:
                    break;

                case GL_BYTE:
                    break;

                case GL_FLOAT:
                    s = p_float_texcoord[index*2];
                    t = p_float_texcoord[index*2+1];
                    r = 0.f;
                    q = 1.f;
                    break;
                }
                break;
            }
        }
        else
        {
            s = ctx->current_texture_coords.tu;
            t = ctx->current_texture_coords.tv;
            r = 0.f;
            q = 1.f;
        }

        /*  put vertex data into HW pipeline functions  */
        gm.InputVertex[0] = vx;
        gm.InputVertex[1] = vy;
        gm.InputVertex[2] = vz;
        gm.InputVertex[3] = vw;

        gm.InputNormal[0] = nx;
        gm.InputNormal[1] = ny;
        gm.InputNormal[2] = nz;

        gm.InputColor[0] = cr;
        gm.InputColor[1] = cg;
        gm.InputColor[2] = cb;
        gm.InputColor[3] = ca;

        gm.InputTexCoord[0] = s;
        gm.InputTexCoord[1] = t;
        gm.InputTexCoord[2] = r;
        gm.InputTexCoord[3] = q;

        gm.do_GM();
        VertexCount++;

#ifdef DrawElemDEG
        fprintf(Drvdumper,"//-------------------- %d th Vertex  --------------------//\n", i+1);
        fprintf(Drvdumper, "V:(%f, %f, %f, %f)\n", vx, vy, vz, vw);
        fprintf(Drvdumper, "C:(%f, %f, %f, %f)\n", cr, cg, cb, ca);
        fprintf(Drvdumper, "N:(%f, %f, %f)\n", nx, ny, nz);
        fprintf(Drvdumper, "T:(%f, %f. %f, %f)\n", s, t, r, q);
#endif

    } // end of DrawElement Loop

#ifdef DrawElemDEG
    fprintf(Drvdumper,"##-------------------- end of DrawArrays  --------------------##\n\n");
#endif

}

void deallocate(void)
{
    delete stateLinkList;
}
