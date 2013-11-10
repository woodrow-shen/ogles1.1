// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : outfile.cpp
// Purpose : Debugging FILE handler
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 19-07-2008   Shye-Tzeng  Shen    initial version
//
// ===================================================================

#include "ogles.h"

FILE * APIdumper;
FILE * Ctxdumper;
FILE * Drvdumper;
FILE * GMdumper;
FILE * RMdumper;
FILE * UGdumper;
FILE * Cbufdumper;
FILE * CSVhanlder;
FILE * CSVglfunc;

void CreateDebugFile()
{
#ifdef APIFUNC_DEG
    APIdumper = fopen("GLAPI_DUMP.txt","w");
    CSVglfunc = fopen("GLAPI_stat.csv","w");
    fprintf(CSVglfunc, ",Percentage , Num of GL calls\n");
#endif
#ifdef PRINT_TRANSFORMATION
    Ctxdumper = fopen("Context_DUMP.txt","w");
#endif

    Drvdumper = fopen("Driver_DUMP.txt", "w");

    GMdumper = fopen("Geometry.txt", "w");
    RMdumper = fopen("Simulation_report.txt", "w");
#ifdef UGLIBDEG
    UGdumper = fopen("UGLIB_DUMP.txt", "w");
#endif

#ifdef COLORBUF_DEG
    Cbufdumper = fopen("Color_Buf.txt", "w");
#endif
#ifdef GENERATE_STAT_CSV
    CSVhanlder = fopen("Simulation_stat.csv", "w");
    fprintf(CSVhanlder, ",Num of TILE,Num of Vertex,Num of Triangles,Culled Triangles,Covered Tiles,Removed_kick_triangles,Triangle Size,"
                        "TriangleList Size,Num of Tile Node,DisplayList Size,Num of State Node,StateList Size,Texture hit,Texture miss,"
                        "Texture hit rate,Visited pixels,Processed pixels,Shaded pixels,Average scan rate,"
                        "StateIO,TriangleIO,Depth buffer BW[R],Depth buffer BW[W],Color buffer BW[R],Color buffer BW[W],"
                        "Texture Read,Total bandwidth\n");
#endif
}

void CloseFile()
{
    printf("%f , %d total:%d\n", static_cast<float>(glfunc.ClearColor)/glfunc.Total, glfunc.ClearColor, glfunc.Total);
#ifdef APIFUNC_DEG
    fclose(APIdumper);

    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ActiveTexture)/glfunc.Total, glfunc.ActiveTexture);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.AlphaFunc)/glfunc.Total, glfunc.AlphaFunc);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.BindTexture)/glfunc.Total, glfunc.BindTexture);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.BlendFunc)/glfunc.Total, glfunc.BlendFunc);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Clear)/glfunc.Total, glfunc.Clear);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ClearColor)/glfunc.Total, glfunc.ClearColor);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ClearDepth)/glfunc.Total, glfunc.ClearDepth);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ClearStencil)/glfunc.Total, glfunc.ClearStencil);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Color4)/glfunc.Total, glfunc.Color4);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ColorPointer)/glfunc.Total, glfunc.ColorPointer);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.CullFace)/glfunc.Total, glfunc.CullFace);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DeleteTextures)/glfunc.Total, glfunc.DeleteTextures);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DepthFunc)/glfunc.Total, glfunc.DepthFunc);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DepthRange)/glfunc.Total, glfunc.DepthRange);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Disable)/glfunc.Total, glfunc.Disable);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DisableClientState)/glfunc.Total, glfunc.DisableClientState);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DrawArrays)/glfunc.Total, glfunc.DrawArrays);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.DrawElements)/glfunc.Total, glfunc.DrawElements);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Enable)/glfunc.Total, glfunc.Enable);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.EnableClientState)/glfunc.Total, glfunc.EnableClientState);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Finish)/glfunc.Total, glfunc.Finish);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Flush)/glfunc.Total, glfunc.Flush);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Fog)/glfunc.Total, glfunc.Fog);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Fogv)/glfunc.Total, glfunc.Fogv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.FrontFace)/glfunc.Total, glfunc.FrontFace);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Frustum)/glfunc.Total, glfunc.Frustum);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.GenTextures)/glfunc.Total, glfunc.GenTextures);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.LightModel)/glfunc.Total, glfunc.LightModel);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.LightModelv)/glfunc.Total, glfunc.LightModelv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Light)/glfunc.Total, glfunc.Light);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Lightv)/glfunc.Total, glfunc.Lightv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.LoadIdentity)/glfunc.Total, glfunc.LoadIdentity);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.LoadMatrix)/glfunc.Total, glfunc.LoadMatrix);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.LogicOp)/glfunc.Total, glfunc.LogicOp);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Material)/glfunc.Total, glfunc.Material);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Materialv)/glfunc.Total, glfunc.Materialv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.MatrixMode)/glfunc.Total, glfunc.MatrixMode);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.MultMatrix)/glfunc.Total, glfunc.MultMatrix);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Normal3)/glfunc.Total, glfunc.Normal3);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.NormalPointer)/glfunc.Total, glfunc.NormalPointer);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Ortho)/glfunc.Total, glfunc.Ortho);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.PolygonOffset)/glfunc.Total, glfunc.PolygonOffset);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.PopMatrix)/glfunc.Total, glfunc.PopMatrix);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.PushMatrix)/glfunc.Total, glfunc.PushMatrix);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Rotate)/glfunc.Total, glfunc.Rotate);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Scale)/glfunc.Total, glfunc.Scale);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Scissor)/glfunc.Total, glfunc.Scissor);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.ShadeModel)/glfunc.Total, glfunc.ShadeModel);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.StencilFunc)/glfunc.Total, glfunc.StencilFunc);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.StencilOp)/glfunc.Total, glfunc.StencilOp);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.TexCoordPointer)/glfunc.Total, glfunc.TexCoordPointer);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.TexEnv)/glfunc.Total, glfunc.TexEnv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.TexEnvv)/glfunc.Total, glfunc.TexEnvv);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.TexImage2D)/glfunc.Total, glfunc.TexImage2D);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.TexParameter)/glfunc.Total, glfunc.TexParameter);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Translate)/glfunc.Total, glfunc.Translate);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.VertexPointer)/glfunc.Total, glfunc.VertexPointer);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Viewport)/glfunc.Total, glfunc.Viewport);
    fprintf(CSVglfunc, ",%f,%d\n",static_cast<float>(glfunc.Total)/glfunc.Total, glfunc.Total);

    fclose(CSVglfunc);
#endif
#ifdef PRINT_TRANSFORMATION
    fclose(Ctxdumper);
#endif

    fclose(Drvdumper);

    fclose(GMdumper);
    fclose(RMdumper);
#ifdef UGLIBDEG
    fclose(UGdumper);
#endif
#ifdef COLORBUF_DEG
    fclose(Cbufdumper);
#endif
#ifdef GENERATE_STAT_CSV
    fclose(CSVhanlder);
#endif
}

int frame_count = 0;
GLfuncstat glfunc;
