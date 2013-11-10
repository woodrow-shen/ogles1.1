#include <stdio.h>
#include <assert.h>
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
#include <windows.h>
#include <TCHAR.h>
#elif defined(EGL_ON_LINUX)
#include <time.h>
#endif
#include "ogles.h"
#include "uglu.h"
#include "offviewer.h"
#include "glm.h"
#include "app.h"
#include "sdl_cylinder.h"

/******************************************************************************
 Benchmarks option for rendering
 ------------------------------------------------------------------------------
 $ Triangle         1       $ Space Shuttle     7       $ bunny69K      13
 $ SolidCube        2       $ Textured Cube     8       $ porsche       14
 $ SolidSphere      3       $ wow               9       $ al man        15
 $ Teapot           4       $ f-16              10      $ castle        16
 $ Dragon           5       $ elephant          11      $ flowers       17
 $ Tank             6       $ venus             12      $ galleon       18
 ------------------------------------------------------------------------------
 $ head             19      $ soccerball        25      $ DrawElements  31
 $ San Angeles      20      $ pig               26      $ GLU Cylinder  32
 $ Blending         21      $ pawn              27      $ Vase          33
 $ Transparency     22      $ eagle             28
 $ Apocalypse       23      $ dolphins          29
 $ cow              24      $ complex_scene     30
******************************************************************************/
#define OBJ4			"./data/teapot.obj"
#define OBJ9            "./data/wow.obj"
#define OBJ10			"./data/f-16.obj"
#define OBJ11			"./data/elephal.obj"
#define OBJ12           "./data/venus.obj"
#define OBJ13			"./data/bunny69k.obj"
#define OBJ14			"./data/porsche.obj"
#define OBJ15			"./data/al.obj"
#define OBJ16			"./data/castle.obj"
#define OBJ17			"./data/flowers.obj"
#define OBJ18			"./data/galleon.obj"
#define OBJ19			"./data/head.obj"
#define OBJ23			"./data/apocalypse.obj"
#define OBJ24			"./data/cow.obj"
#define OBJ25			"./data/soccerball.obj"
#define OBJ26			"./data/pig.obj"
#define OBJ27			"./data/pawn.obj"
#define OBJ28			"./data/eagle.obj"
#define OBJ29			"./data/dolphins.obj"
#define OBJ33           "./data/Vase.obj"

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
#define OBJNAME			OBJ16
#define DrawObjectKind  16
#define CameraChanged   1

#define FRAME           1
#endif
#define EnableAA        1
#define TEXTURE_NUMBER  10
#define T(x) (pmodel->triangles[(x)])

/******************************************************************************
 Functions declaration
******************************************************************************/
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#endif

/******************************************************************************
 Global variables
******************************************************************************/

// Variable set in the message handler to finish the demo
bool	g_bDemoDone = false;
GLuint  texture[TEXTURE_NUMBER] = {0};
U8      *bitmap = NULL;
GLMmodel *pmodel = NULL;

static int frames = 0;
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
const static int updateTime = 1000;
static int lastTime = 0;
#elif defined(EGL_ON_LINUX)
time_t lastTime = time(0), updateTime = time(0);
#endif
static double FPS = 0.f;

/*!****************************************************************************
 $ Function		CalcFPS
 $ Input        void
 $ Return		void
 $ Description	Calate average FPS
******************************************************************************/
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
void CalcFPS(void)
{
    frames++;
    if (GetTickCount()-lastTime>updateTime)
    {
        FPS = ((double)frames/(double)(GetTickCount()-lastTime))*1000.0;
        printf("FPS:%f\n", FPS);
        fprintf(RMdumper, "FPS:%f\n", FPS);
        lastTime = GetTickCount();
        frames = 0;
    }
}
#elif defined(EGL_ON_LINUX)
void CalcFPS(void)
{
    frames++;
	time(&updateTime);
	if (updateTime > lastTime)
	{
		FPS = ((double)frames/(double)(updateTime-lastTime));
        printf("FPS:%f\n", FPS);
        fprintf(RMdumper, "FPS:%f\n", FPS);
        lastTime = updateTime;
        frames = 0;
	}
}
#endif

/*!****************************************************************************
 $ Function		TestEGLError
 $ Input	    pszLocation		location in the program where the error took
								place. ie: function name
 $ Return		bool			true if no EGL error was detected
 $ Description	Tests for an EGL error and prints it
******************************************************************************/
#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
bool TestEGLError(HWND hWnd, char* pszLocation)
{
    /*
    	eglGetError returns the last error that has happened using egl,
    	not the status of the last called function. The user has to
    	check after every single egl call or at least once every frame.

    EGLint iErr = eglGetError();
    if (iErr != EGL_SUCCESS)
    {
    	TCHAR pszStr[256];
    	_stprintf(pszStr, _T("%s failed (%d).\n"), pszLocation, iErr);
    	MessageBox(hWnd, pszStr, _T("Error"), MB_OK|MB_ICONEXCLAMATION);
    	return false;
    }
    */
    return true;
}
#endif

/*!****************************************************************************
 $ Function		loadTextures (For test 8)
 $ Input        filename
 $ Return		bool	        returns a pointer to image data if successed
 $ Description	Texture loading and texture functions setting
******************************************************************************/
bool loadSimpleTextures()
{
    _BITMAPINFO *info1 = NULL,*info2 = NULL;
    U8 *tex1 = NULL, *tex2 = NULL;
    tex1 = LoadDIBitmap("./data/sign.bmp", &info1);
    tex2 = LoadDIBitmap("./data/crate.bmp", &info2);

    if (!tex1)
        return false;
    if (!tex2)
        return false;

    glGenTextures(1, &texture[0]);

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info1->bmiHeader.biWidth,
                 info1->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex1);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texture[1]);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info2->bmiHeader.biWidth,
                 info2->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex2);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(tex1);
    free(tex2);
    free(info1);
    free(info2);
    return true;
}

/*!****************************************************************************
 $ Function		loadTextures (For test 22)
 $ Input        filename
 $ Return		bool	        returns a pointer to image data if successed
 $ Description	Texture loading and texture functions setting
******************************************************************************/
bool loadTextures(char *filename)
{
    _BITMAPINFO *info;
    bitmap = LoadDIBitmap(filename, &info);
    //bitmap = loadBMP(filename);

    if (!bitmap)
        return false;

    glGenTextures(1, texture);

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info->bmiHeader.biWidth,
                 info->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 bitmap);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(bitmap);
    free(info);

    return true;
}

/*!****************************************************************************
 $ Function		loadTextures (For test 30)
 $ Input        filename
 $ Return		bool	        returns a pointer to image data if successed
 $ Description	Texture loading and texture functions setting
******************************************************************************/
bool loadSceneTextures()
{
    _BITMAPINFO *info1 = NULL, *info2 = NULL, *info3 = NULL, *info4 = NULL;
    U8 *tex1 = NULL, *tex2 = NULL, *tex3 = NULL, *tex4 = NULL;
    tex1 = LoadDIBitmap("./data/SAND.bmp", &info1);
    tex2 = LoadDIBitmap("./data/SKY.bmp", &info2);
    tex3 = LoadDIBitmap("./data/Himax_LOGO.bmp", &info3);
    tex4 = LoadDIBitmap("./data/Palm.bmp", &info4);

    if (!tex1)
        return false;
    if (!tex2)
        return false;
    if (!tex3)
        return false;
    if (!tex4)
        return false;

    glGenTextures(1, &texture[0]);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info1->bmiHeader.biWidth,
                 info1->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex1);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texture[1]);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info2->bmiHeader.biWidth,
                 info2->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex2);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texture[2]);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info3->bmiHeader.biWidth,
                 info3->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex3);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texture[3]);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info4->bmiHeader.biWidth,
                 info4->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 tex4);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(tex1);
    free(tex2);
    free(tex3);
    free(tex4);
    free(info1);
    free(info2);
    free(info3);
    free(info4);
    return true;
}

#if (defined(EGL_ON_WIN32) || defined(EGL_ON_WINCE))
/*!****************************************************************************
 $ Function		WinMain
 $ Input			hInstance		Application instance from OS
 $ Input			hPrevInstance	Always NULL
 $ Input			lpCmdLine		command line from OS
 $ Input			nCmdShow		Specifies how the window is to be shown
 $ Return		int				result code to OS
 $ Description	Main function of the program
******************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // Windows variables
    WNDCLASSEX          wcex;
    HWND                hwnd;
    HDC                 hDC;
    HGLRC               hRC;
    MSG                 msg;
    BOOL                bQuit = FALSE;

    // All App. variables
    float               theta = 0.0f;
    float               xrot = 0.0f;
    float               yrot = 0.0f;
    float               xtran = 0.0f;
    bool                xright = true;
    long                tickcount = 0;
    bool                frameDone = false;

    // EGL variables
    EGLDisplay			eglDisplay	= 0;
    EGLConfig			eglConfig	= 0;
    EGLSurface			eglSurface	= 0;
    EGLContext			eglContext	= 0;
    NativeWindowType	eglWindow	= 0;
    //EGLint				pi32ConfigAttribs[128];

    /*
    	Step 0 - Create a NativeWindowType that we can use for OpenGL ES output
    */

    /* register window class */

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

    // Register the windows class
    /*WNDCLASS sWC;
    sWC.style = CS_HREDRAW | CS_VREDRAW;
    sWC.lpfnWndProc = WndProc;
    sWC.cbClsExtra = 0;
    sWC.cbWndExtra = 0;
    sWC.hInstance = hInstance;
    sWC.hIcon = 0;
    sWC.hCursor = 0;
    sWC.lpszMenuName = 0;
    sWC.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    sWC.lpszClassName = WINDOW_CLASS;
    ATOM registerClass = RegisterClass(&sWC);
    if (!registerClass)
    {
    	MessageBox(0, _T("Failed to register the window class"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
    }*/

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL|ES Applications",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          660,
                          520,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    eglWindow = hwnd;
    // Create the eglWindow
    /*RECT	sRect;
    SetRect(&sRect, 0, 0, WINDOW_WINDOW_DEFAULT_WIDTH, WINDOW_WINDOW_DEFAULT_HEIGHT);
    AdjustWindowRectEx(&sRect, WS_CAPTION | WS_SYSMENU, false, 0);
    hWnd = CreateWindow( WINDOW_CLASS, _T("Initialization"), WS_VISIBLE | WS_SYSMENU,
    					 0, 0, WINDOW_WINDOW_DEFAULT_WIDTH, WINDOW_WINDOW_DEFAULT_HEIGHT, NULL, NULL, hInstance, NULL);
    eglWindow = hWnd;
    */
    CreateDebugFile();
    /*
        Set PixelFormat Description, and Get GDI parameters
    */
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    // Get the associated device context
    hDC = GetDC(hwnd);
    if (!hDC)
    {
        MessageBox(0, _T("Failed to create the device context"), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
        goto cleanup;
    }

    /* set the pixel format for the DC */
//    ZeroMemory(&pfd, sizeof(pfd));
//
//    pfd.nSize = sizeof(pfd);
//    pfd.nVersion = 1;
//    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
//                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//    pfd.iPixelType = PFD_TYPE_RGBA;
//    pfd.cColorBits = 24;
//    pfd.cDepthBits = 16;
//    pfd.iLayerType = PFD_MAIN_PLANE;
//
//    iFormat = ChoosePixelFormat(*hDC, &pfd);
//
//    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
//    *hRC = wglCreateContext(*hDC);
//
//    wglMakeCurrent(*hDC, *hRC);

    /*
    	Step 1 - Get the default display.
    	EGL uses the concept of a "display" which in most environments
    	corresponds to a single physical screen. Since we usually want
    	to draw to the main screen or only have a single screen to begin
    	with, we let EGL pick the default display.
    	Querying other displays is platform specific.
    */
    eglDisplay = eglGetDisplay((NativeDisplayType)hDC);

    if (eglDisplay == EGL_NO_DISPLAY)
        eglDisplay = eglGetDisplay((NativeDisplayType) EGL_DEFAULT_DISPLAY);


    /*
    	Step 2 - Initialize EGL.
    	EGL has to be initialized with the display obtained in the
    	previous step. We cannot use other EGL functions except
    	eglGetDisplay and eglGetError before eglInitialize has been
    	called.
    	If we're not interested in the EGL version number we can just
    	pass NULL for the second and third parameters.
    */
    EGLint iMajorVersion, iMinorVersion;
//	if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
//	{
//		MessageBox(0, _T("eglInitialize() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
//		goto cleanup;
//	}


    /*
    	Step 3 - Specify the required configuration attributes.
    	An EGL "configuration" describes the pixel format and type of
    	surfaces that can be used for drawing.
    	For now we just want to use a 16 bit RGB surface that is a
    	Window surface, i.e. it will be visible on screen. The list
    	has to contain key/value pairs, terminated with EGL_NONE.
    */
//	i = 0;
//	pi32ConfigAttribs[i++] = EGL_RED_SIZE;
//	pi32ConfigAttribs[i++] = 5;
//	pi32ConfigAttribs[i++] = EGL_GREEN_SIZE;
//	pi32ConfigAttribs[i++] = 6;
//	pi32ConfigAttribs[i++] = EGL_BLUE_SIZE;
//	pi32ConfigAttribs[i++] = 5;
//	pi32ConfigAttribs[i++] = EGL_ALPHA_SIZE;
//	pi32ConfigAttribs[i++] = 0;
//	pi32ConfigAttribs[i++] = EGL_SURFACE_TYPE;
//	pi32ConfigAttribs[i++] = EGL_WINDOW_BIT;
//	pi32ConfigAttribs[i++] = EGL_NONE;


    /*
    	Step 4 - Find a config that matches all requirements.
    	eglChooseConfig provides a list of all available configurations
    	that meet or exceed the requirements given as the second
    	argument. In most cases we just want the first config that meets
    	all criteria, so we can limit the number of configs returned to 1.
    */
//	int iConfigs;
//	if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
//	{
//		MessageBox(0, _T("eglChooseConfig() failed."), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
//		goto cleanup;
//	}


    /*
    	Step 5 - Create a surface to draw to.
    	Use the config picked in the previous step and the native window
    	handle when available to create a window surface. A window surface
    	is one that will be visible on screen inside the native display (or
    	fullscreen if there is no windowing system).
    	Pixmaps and pbuffers are surfaces which only exist in off-screen
    	memory.
    */
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, NULL);

//    if(eglSurface == EGL_NO_SURFACE)
//    {
//           eglGetError(); // Clear error
//           eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
//	}

    if (!TestEGLError(hwnd, "eglCreateWindowSurface"))
    {
        //goto cleanup;
    }


    /*
    	Step 6 - Create a context.
    	EGL has to create a context for OpenGL ES. Our OpenGL ES resources
    	like textures will only be valid inside this context
    	(or shared contexts)
    */
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
    if (!TestEGLError(hwnd, "eglCreateContext"))
    {
        //goto cleanup;
    }


    /*
    	Step 7 - Bind the context to the current thread and use our
    	window surface for drawing and reading.
    	Contexts are bound to a thread. This means you don't have to
    	worry about other threads and processes interfering with your
    	OpenGL ES application.
    	We need to specify a surface that will be the target of all
    	subsequent drawing operations, and one that will be the source
    	of read operations. They can be the same surface.
    */
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    if (!TestEGLError(hwnd, "eglMakeCurrent"))
    {
        //goto cleanup;
    }

    /* GL init procedure */
    switch (DrawObjectKind)
    {

    case 3:
    {
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float lightPosition[] = { 4.0f, 4.0f, 3.0f, 0.0f };

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);


        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);
        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        break;
    }

    case 4:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        //float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 5:
    {
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float material[] = { 0.5, 1.0, 0.5, 1.0 };

        float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        glShadeModel(GL_SMOOTH);;
        glEnable(GL_CULL_FACE);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);

        //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

        //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        //glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        break;
    }

    case 6:
        break;

    case 7:
        break;

    case 8:
    {
        float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

        if (!loadSimpleTextures())
        {
            printf("Error loading textures!\n");
            return 0;
        }

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glViewport(0, 0, width, height);
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        ugluLookAtf(
            0.0f, 0.0f, 3.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        );

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);

        break;
    }

    case 9:
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_ALPHA_TEST);

        glAlphaFunc(GL_GEQUAL, 0.5);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glViewport(0, 0, width, height);
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 10:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        break;
    }

    case 11:
    case 15:
    case 19:
    case 25:
    case 26:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 12:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float lightPosition[] = { 10.0f, 10.0f, 10.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 1.5f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 13:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        if (!CameraChanged)
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 14:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 16:
    case 17:
    case 18:
    case 27:
    case 28:
    case 29:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            //GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 20:
    {
        appInit();
        break;
    }

    case 21:
    {
        GLenum blendSrc[] =
        {
            GL_ONE,
            GL_ONE,
            GL_ONE,
            GL_SRC_ALPHA,
            GL_SRC_ALPHA,

            GL_ZERO,
            GL_ONE_MINUS_DST_COLOR,
            GL_ONE_MINUS_SRC_ALPHA,
            GL_DST_ALPHA,
            GL_SRC_ALPHA_SATURATE
        };

        GLenum blendDst[] =
        {
            GL_ZERO,
            GL_ONE,
            GL_ONE_MINUS_DST_ALPHA,
            GL_ONE,
            GL_ONE_MINUS_SRC_ALPHA,

            GL_SRC_COLOR,
            GL_ONE_MINUS_SRC_COLOR,
            GL_SRC_ALPHA,
            GL_SRC_COLOR,
            GL_SRC_COLOR
        };

        int curBlendMode = 9;

        glClearColor (0.25f, 0.25f, 0.25f, 1.0f);

        glEnable(GL_BLEND);
        glBlendFunc(blendSrc[curBlendMode], blendDst[curBlendMode]);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glViewport(0, 0, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
        glOrthof(0.0f, 3.0f, 0.0f, 3.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        break;
    }

    case 22:
    {
        float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

        if (!loadTextures("./data/zeus.bmp"))
        {
            printf("Error loading textures!\n");
            return 0;
        }

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);

        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        break;
    }
    case 23:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 1.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }
    case 24:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.5f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        break;
    }

    case 30:
    {
        if (!loadSceneTextures())
        {
            printf("Error loading textures!\n");
            return 0;
        }
        break;
    }

    case 31:
    {
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float lightPosition[] = { 4.0f, 4.0f, 3.0f, 0.0f };

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);
        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        break;
    }

    case 32:
    {
        init_scene(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
        break;
    }

    case 33:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glViewport(0, 0, width, height);
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 500.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    }// case-switch

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /*
                Step 8 - Draw something with OpenGL ES.
                At this point everything is initialized and we're ready to use
                OpenGL ES to draw something on the screen.
            */

            if (frame_count >= FRAME) frameDone = true;
            else ++frame_count;

            if (frameDone) continue;//goto cleanup;

            fprintf(APIdumper,"//-------------------- %d th Frame Instr --------------------//\n", frame_count);
            printf("%d th Frame start...\n", frame_count);

            if (EnableAA)
                glEnable(GL_MULTISAMPLE);
            else
                glDisable(GL_MULTISAMPLE);

            switch (DrawObjectKind)
            {
            case 1:
            {
//	GLfloat vtx[] = {
//					-0.5, -0.5, 0.0, 1.0,
//					0.3, 0.3, 0.0, 1.0,
// 					0.6, 0.0, 0.0, 1.0,
//					0.8, -0.5, 0.0, 1.0,
//					0.9, -0.9, 0.0, 1.0
//                    };
//
//	GLfloat c[] = {	0.9, 0.0, 0.0, 1.0,
//					0.0, 0.9, 0.0, 1.0,
//					0.0, 0.0, 0.9, 1.0,
//					0.9, 0.0, 0.0, 1.0,
//					0.0, 0.9, 0.0, 1.0};
//
//                glMatrixMode(GL_PROJECTION);
//                glLoadIdentity();
//
//                //glFrustumf(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);
//                glMatrixMode(GL_MODELVIEW);
//
//                //glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
//                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//
//                glEnableClientState(GL_VERTEX_ARRAY);
//                glVertexPointer(4, GL_FLOAT, 0, vtx);
//                glEnableClientState(GL_COLOR_ARRAY);
//                glColorPointer(4, GL_FLOAT, 0, c);
//
//                glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

                // Vertex Data
                GLfloat v[] = { 1.0, -1.0, 0.0,
                                0.0,  1.0, 0.0,
                                -1.0, -1.0, 0.0
                              };

                GLfloat pfColors[] = {(0.0f), (0.0f), (1.0f), (1.0f),
                                      (1.0f), (0.0f), (0.0f), (1.0f),
                                      (0.0f), (1.0f), (0.0f), (1.0f)
                                     };

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();

                glFrustumf(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);
                glMatrixMode(GL_MODELVIEW);

                //glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

                glLoadIdentity();
                //glPushMatrix();
                glTranslatef(0.5, 0.0, -2.0);
                glRotatef(theta, 1.0f, 1.0f, 0.0f);

                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, v);
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(4, GL_FLOAT, 0, pfColors);

                glDrawArrays(GL_TRIANGLES, 0, 3);

                //glFinish();
                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                theta += 1.0f;
                if (theta > 360.0f)
                    theta = 0.0f;
                break;
            }
            case 2:
            {
                GLfloat box[] =
                {
                    // FRONT
                    -0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // BACK
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // LEFT
                    -0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    // RIGHT
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // TOP
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // BOTTOM
                    -0.5f, -0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f, -0.5f,
                };

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);

                //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                //glClearDepthf(1.0f);

                glVertexPointer(3, GL_FLOAT, 0, box);
                glEnableClientState(GL_VERTEX_ARRAY);

                glShadeModel(GL_FLAT);;
                glEnable(GL_CULL_FACE);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();

                ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                ugluLookAtf(
                    0.0f, 0.0f, 3.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f);

                glRotatef(30.0, 1.0f, 0.0f, 0.0f);
                glRotatef(30.0, 0.0f, 1.0f, 0.0f);
                //glRotatef(xrot, 1.0f, 0.0f, 0.0f);
                //glRotatef(yrot, 0.0f, 1.0f, 0.0f);

                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
                glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

                glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
                glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                xrot += 0.3f;
                yrot += 0.4f;
                break;
            }

            case 3:
            {
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                ugluLookAtf(
                    0.0f, 0.0f, 4.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f);

                glPushMatrix();
                glRotatef(xrot, 1.0f, 0.0f, 0.0f);
                glRotatef(yrot, 0.0f, 1.0f, 0.0f);

                ugSolidSpheref(1.0f, 24, 24);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                xrot += 2.0f;
                yrot += 3.0f;

                break;
            }

            case 4:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                glRotatef(yrot, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                xrot += 2.0f;
                yrot += 3.0f;
                break;
            }

            case 5:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ("./data/dragon_vrip_res4.obj");
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
                ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                //glRotatef(45.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH );
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 6:
            {
                rotation[0] = 24.5f;

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                DrawOFF("m1413.off");

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                break;
            }

            case 7:

                rotation[0] = 29.0f;

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                DrawOFF("m1397.off");

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                break;

            case 8:
            {
                GLfloat box[] =
                {
                    // FRONT
                    -0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // BACK
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // LEFT
                    -0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    // RIGHT
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // TOP
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // BOTTOM
                    -0.5f, -0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f, -0.5f,
                };

                GLfloat texCoords[] =
                {
                    // FRONT
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // BACK
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // LEFT
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // RIGHT
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // TOP
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // BOTTOM
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f
                };

                glVertexPointer(3, GL_FLOAT, 0, box);
                glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//                glBindTexture(GL_TEXTURE_2D, texture[0]);
//
//                // SIGN BOX static
//                glPushMatrix();
//                glTranslatef(-1.0, 0.0, 0.0);
//                glRotatef(30.0, 1.0f, 0.0f, 0.0f);
//                glRotatef(30.0, 0.0f, 1.0f, 0.0f);
//
//                // FRONT AND BACK
//                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//                glNormal3f(0.0f, 0.0f, 1.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//                glNormal3f(0.0f, 0.0f, -1.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
//
//                // LEFT AND RIGHT
//                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
//                glNormal3f(-1.0f, 0.0f, 0.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
//                glNormal3f(1.0f, 0.0f, 0.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
//
//                // TOP AND BOTTOM
//                glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
//                glNormal3f(0.0f, 1.0f, 0.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
//                glNormal3f(0.0f, -1.0f, 0.0f);
//                glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
//                glPopMatrix();

                glBindTexture(GL_TEXTURE_2D, texture[1]);

                // WOOD BOX dynamic
                glPushMatrix();
                glTranslatef(xtran, 0.0, 0.0);
                glRotatef(xrot, 1.0f, 0.0f, 0.0f);
                glRotatef(yrot, 0.0f, 1.0f, 0.0f);

                // FRONT AND BACK
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glNormal3f(0.0f, 0.0f, -1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

                // LEFT AND RIGHT
                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
                glNormal3f(1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

                // TOP AND BOTTOM
                glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
                glNormal3f(0.0f, -1.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                xtran += xright ? 0.02f : -0.02f;
                if (xtran > 2.0)
                    xright = false;
                if (xtran < 0.0)
                    xright = true;

                xrot += 2.0f;
                yrot += 3.0f;
                break;
            }

            case 9:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                glTranslatef(0,0,-0.5);
                glRotatef(yrot, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                yrot += 3.0f;
                break;
            }

            case 10:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.5,0.0,0.0);
                //glRotatef(-90.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                break;
            }

            case 11:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(0,1.0,0.5);
                glRotatef(-90.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH );
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                break;
            }

            case 12:
            {
                if (!pmodel)
                {
                    //printf("Loading OBJ...\n");
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                if (CameraChanged)
                {
                    glRotatef(yrot, 0.0, 1.0, 0.0);
                    printf("yrot:%f\n", yrot);
                    glRotatef(-90.0, 1.0, 0.0, 0.0);
                }
                else
                    glRotatef(-90.0, 1.0, 0.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                yrot += 1.0f;
                break;
            }

            case 13:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                //glRotatef(-90.0, 1.0, 0.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 14:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 15:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 16:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                glRotatef(xrot, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                xrot += 1.0;
                break;
            }

            case 17:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                if (CameraChanged)
                    glTranslatef(0.0,-0.2,0.0);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 18:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                //glTranslatef(1.0,1.0,0.5);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH );
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 19:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                glTranslatef(0.0,0.0,-0.5);
                glRotatef(-90.0, 1.0, 0.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                break;
            }

            case 20:
            {
                appRender(tickcount, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);

                eglSwapBuffers(eglDisplay, eglSurface);
                CalcFPS();

                tickcount += 33;
                break;
            }

            case 21:
            {
                GLfloat rectangle[] =
                {
                    -1.0f, -0.25f,
                    1.0f, -0.25f,
                    -1.0f,  0.25f,
                    1.0f,  0.25f
                };

                glClear (GL_COLOR_BUFFER_BIT);
                glLoadIdentity();

                glVertexPointer(2, GL_FLOAT, 0, rectangle);

                glEnableClientState(GL_VERTEX_ARRAY);

                glPushMatrix();
                glTranslatef(1.5f, 2.0f, 0.0f);
                glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(0.7f, 1.5f, 0.0f);
                glRotatef(90.0f, 0.0f ,0.0f, 1.0f);
                glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(1.7f, 1.5f, 0.0f);
                glRotatef(90.0f, 0.0f ,0.0f, 1.0f);
                glColor4f(0.0f, 0.0f, 1.0f, 0.25f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(1.5f, 1.0f, 0.0f);
                glColor4f(1.0f, 1.0f, 0.0f, 0.75f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glPopMatrix();

                eglSwapBuffers(eglDisplay, eglSurface);
                CalcFPS();

                break;
            }

            case 22:
            {
                GLfloat box[] =
                {
                    // FRONT
                    -0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // BACK
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // LEFT
                    -0.5f, -0.5f,  0.5f,
                    -0.5f,  0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    -0.5f,  0.5f, -0.5f,
                    // RIGHT
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    // TOP
                    -0.5f,  0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    -0.5f,  0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                    // BOTTOM
                    -0.5f, -0.5f,  0.5f,
                    -0.5f, -0.5f, -0.5f,
                    0.5f, -0.5f,  0.5f,
                    0.5f, -0.5f, -0.5f,
                };

                GLfloat texCoords[] =
                {
                    // FRONT
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // BACK
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // LEFT
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // RIGHT
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    // TOP
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // BOTTOM
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f
                };

                glVertexPointer(3, GL_FLOAT, 0, box);
                glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glLoadIdentity();

                ugluLookAtf(
                    0.0f, 0.0f, 3.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f);

                glTranslatef(xtran, 0.0, 0.0);
                glRotatef(xrot, 1.0f, 0.0f, 0.0f);
                glRotatef(yrot, 0.0f, 1.0f, 0.0f);
//                glRotatef(60.0, 1.0f, 0.0f, 0.0f);
//                glRotatef(60.0, 0.0f, 1.0f, 0.0f);

                // FRONT AND BACK
                glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glNormal3f(0.0f, 0.0f, -1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

                // LEFT AND RIGHT
                glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
                glNormal3f(-1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
                glNormal3f(1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

                // TOP AND BOTTOM
                glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
                glNormal3f(0.0f, -1.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                xtran += xright ? 0.02f : -0.02f;
                if (xtran > 2.0)
                    xright = false;
                if (xtran < 0.0)
                    xright = true;

                xrot += 2.0f;
                yrot += 3.0f;
                break;
            }

            case 23:
            case 24:
            case 28:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();

                glmDraw(pmodel, GLM_SMOOTH );

                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 25:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                glTranslatef(0.0,0.0,-0.5);
                glmDraw(pmodel, GLM_SMOOTH  | GLM_MATERIAL );

                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 26:
            case 27:
            case 29:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL );

                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;

                break;
            }

            case 30:
            {
                GLfloat pyramid[] =
                {
                    0.0f, 3.0f, 0.0f,
                    -2.5f,-1.0f, 2.5f,
                    2.5f,-1.0f, 2.5f,

                    0.0f, 3.0f, 0.0f,
                    2.5f,-1.0f, 2.5f,
                    2.5f,-1.0f, -2.5f,

                    0.0f, 3.0f, 0.0f,
                    2.5f,-1.0f, -2.5f,
                    -2.5f,-1.0f, -2.5f,

                    0.0f, 3.0f, 0.0f,
                    -2.5f,-1.0f,-2.5f,
                    -2.5f,-1.0f, 2.5f
                };

                GLfloat pyramid_tex[] =
                {
                    0.5, 1.0,
                    0.0, 0.0,
                    1.0, 0.0,

                    0.5, 1.0,
                    0.0, 0.0,
                    1.0, 0.0,

                    0.5, 1.0,
                    0.0, 0.0,
                    1.0, 0.0,

                    0.5, 1.0,
                    0.0, 0.0,
                    1.0, 0.0,
                };

                GLfloat box[] =
                {
                    // FRONT
                    -10.0f, -5.0f, -10.0f,
                    10.0f, -5.0f,  -10.0f,
                    -10.0f,  8.0f, -10.0f,
                    10.0f,  8.0f, -10.0f,
                    // TOP
                    -10.0f, -1.0f,  1.0f,
                    10.0f, -1.0f,  1.0f,
                    -10.0f, -1.0f, -10.0f,
                    10.0f, -1.0f, -10.0f,
                    // R
                    0.5f, -0.5f,  0.5f,
                    0.5f,  0.5f,  0.5f,
                    0.5f, -0.5f, -0.5f,
                    0.5f,  0.5f, -0.5f,
                };

                GLfloat texCoords[] =
                {
                    // FRONT
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // TOP
                    0.0f, 0.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    // R
                    1.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                };

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);

                glEnable(GL_TEXTURE_2D);

                //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClearDepthf(1.0f);

                glVertexPointer(3, GL_FLOAT, 0, pyramid);
                glEnableClientState(GL_VERTEX_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, 0, pyramid_tex);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glEnable(GL_CULL_FACE);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();

                ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 20.0f);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                ugluLookAtf(
                    0.0f, 0.0f, 3.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f);

                glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture[2]);
                glTranslatef(3.0f,0.0f,-10.0f);
                glRotatef(45.0,0.0f,1.0f,0.0f);
                //glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
                glDrawArrays(GL_TRIANGLES, 0, 12);
                glPopMatrix();

                glVertexPointer(3, GL_FLOAT, 0, box);
                glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texture[1]);
                // FRONT AND BACK
                glColor4f(0.6, 0.8, 1.0, 1.0);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                glBindTexture(GL_TEXTURE_2D, texture[0]);
                // TOP AND BOTTOM
                glColor4f(0.2f, 0.8f, 0.2f, 1.0f);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
                glPopMatrix();

                glDisable(GL_TEXTURE_2D);

                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);

//                float lightPosition[] = { 0.0f, 1.0f, 1.0f, 0.0f };
//                glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJ16);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                };

                glPushMatrix();
                glTranslatef(0.0,0.0,-3.0);
                //glScalef(3.0,3.0,3.0);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();
                glmDelete(pmodel);
                pmodel = NULL;

                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJ28);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                };

                glPushMatrix();
                glTranslatef(2.0,4.0,-10.0);
                //glScalef(0.5,0.5,0.5);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH );
                glPopMatrix();
                glmDelete(pmodel);
                pmodel = NULL;

                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJ17);
                    if (!pmodel) exit(0);

                    glmUnitize(pmodel);
                    glmFacetNormals(pmodel);
                    glmVertexNormals(pmodel, 90.0);
                };

                glPushMatrix();
                glTranslatef(-2.0,0.0,-2.0);
                //glRotatef(-30.0, 0.0, 1.0, 0.0);
                //glScalef(0.5,0.5,0.5);
                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                break;
            }

            case 31:
            {
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //glEnable(GL_CULL_FACE);

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);

                ugluLookAtf(
                    0.0f, 0.0f, 3.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f);

                glTranslatef(0.0, -0.75, -1.0);

                float matDiffuse[] = {0.8f, 0.8f, 0.0f, 1.0f};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);

                glPushMatrix();
                glTranslatef(-0.8, 0.5, 0.0);
                glRotatef (xrot, 1.0, 0.0, 0.0);
                ugSolidTorusf(0.275, 0.5, 15, 15);
                glPopMatrix();

                matDiffuse[0] = 0.0f;
                matDiffuse[1] = 0.0f;
                matDiffuse[2] = 0.8f;
                matDiffuse[3] = 1.0f;
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
                glPushMatrix();
                glTranslatef(1.0, 0.5, 0.0);
                glRotatef (xrot, 0.0, 1.0, 0.0);
                ugSolidBox(1, 1, 1);
                glPopMatrix();

                matDiffuse[0] = 0.0f;
                matDiffuse[1] = 0.8f;
                matDiffuse[2] = 0.0f;
                matDiffuse[3] = 1.0f;
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
                glPushMatrix();
                glTranslatef(0.0, 1.0, -2.0);
                glRotatef (xrot, 0.0, 1.0, 0.0);
                glRotatef (270.0, 1.0, 0.0, 0.0);

                ugSolidConef(1.0, 2.0, 15, 15);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();
                xrot += 1.0;
                break;
            }

            case 32:
            {

                render_scene();
                eglSwapBuffers(eglDisplay,eglSurface);
                break;
            }

            case 33:
            {
                if (!pmodel)
                {
                    pmodel = glmReadOBJ(OBJNAME);
                    if (!pmodel) exit(0);
                    //glmUnitize(pmodel);
                    //glmFacetNormals(pmodel);
                    //glmVertexNormals(pmodel, 90.0);
                }

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glPushMatrix();
                glTranslatef(0,0,-420.0);
                //glRotatef(yrot, 0.0, 1.0, 0.0);

                glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
                glPopMatrix();

                eglSwapBuffers(eglDisplay,eglSurface);
                CalcFPS();

                glmDelete(pmodel);
                pmodel = NULL;
                yrot += 3.0f;

                break;
            }

            }// case-switch

        }// if-else
    }// while loop

    /*
    	Step 9 - Terminate OpenGL ES and destroy the window (if present).
    	eglTerminate takes care of destroying any context or surface created
    	with this display, so we don't need to call eglDestroySurface or
    	eglDestroyContext here.
    */
cleanup:
    if (DrawObjectKind == 8 || DrawObjectKind == 22)
        glDeleteTextures(1, texture);
    if (DrawObjectKind == 30)
        glDeleteTextures(4, texture);
    if (DrawObjectKind == 20)
    {
        appDeinit();
    }

    eglSaveSurfaceHM(eglSurface, "SurfaceBitMap.bmp");
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisplay, eglContext);
    eglDestroySurface(eglDisplay, eglSurface);

    /*
    	Step 10 - Destroy the eglWindow.
    	Again, this is platform specific and delegated to a separate function.
    */

// Release the device context
    if (hDC) ReleaseDC(hwnd, hDC);

// Destroy the eglWindow
    if (hwnd) DestroyWindow(hwnd);

    return msg.wParam;
}

/*!****************************************************************************
 $ Function		WndProc
 $ Input			hWnd		Handle to the window
 $ Input			message		Specifies the message
 $ Input			wParam		Additional message information
 $ Input			lParam		Additional message information
 $ Return		LRESULT		result code to OS
 $ Description	Processes messages for the main window
******************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handles the close message when a user clicks the quit icon of the window
    case WM_CLOSE:
        g_bDemoDone = true;
        CloseFile();
        PostQuitMessage(0);
        return 1;

    case WM_KEYDOWN:
        switch ( wParam )
        {
        case 0x42:// B key
            MessageBox (NULL, "B key test OK!!", "Event", MB_OK);
            break;
        }
        break;

    default:
        break;
    }

    // Calls the default window procedure for messages we did not handle
    return DefWindowProc(hWnd, message, wParam, lParam);
}


#elif defined (EGL_ON_LINUX)
int main(int argc, char **argv)
{
    // All App. variables
    float               theta = 0.0f;
    float               xrot = 0.0f;
    float               yrot = 40.0f;
    float               xtran = 0.0f;
    bool                xright = false;
    long                tickcount = 0;
    bool                frameDone = false;
    int             	DrawObjectKind = 1;
    int             	Frame = 1;
    bool 				CameraChanged = false;
    int 				CameraParam = 0;
    int 				temp;

    // EGL variables
    EGLDisplay			eglDisplay	= 0;
    EGLConfig			eglConfig	= 0;
    EGLSurface			eglSurface	= 0;
    EGLContext			eglContext	= 0;
    NativeWindowType	eglWindow	= 0;

    CreateDebugFile();

    printf("=== OpenGL|ES Applcation ===\n");
    DrawObjectKind = atoi(argv[1]);
    Frame = atoi(argv[2]);
    CameraParam = atoi(argv[3]);
    CameraChanged = static_cast<bool>(CameraParam);
    printf("Select %d-th Benchmark, Run %d Frames, CameraChanged:%d\n", DrawObjectKind, Frame, CameraChanged);

    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    printf("GL Init...\n");

    /* GL init procedure */
    switch (DrawObjectKind)
    {

    case 3:
    {
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float lightPosition[] = { 4.0f, 4.0f, 3.0f, 0.0f };

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);
        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        break;
    }

    case 4:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        //float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        break;
    }

    case 5:
    {
        float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        float matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float material[] = { 0.5, 1.0, 0.5, 1.0 };

        float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        glShadeModel(GL_SMOOTH);;
        glEnable(GL_CULL_FACE);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);

        //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

        //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        //glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        break;
    }

    case 6:

        break;

    case 7:

        break;

    case 8:
    {
        float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

        if (!loadTextures("./data/zeus.bmp"))
        {
            printf("Error loading textures!\n");
            return 0;
        }

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glViewport(0, 0, width, height);
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);

        break;
    }

    case 9:
    {
//        if (!loadTextures("./data/wow_2.bmp"))
//        {
//            printf("Error loading textures!\n");
//            return 0;
//        }
//
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_ALPHA_TEST);

        glAlphaFunc(GL_GEQUAL, 0.5);
        //glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glViewport(0, 0, width, height);
        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 10:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        break;
    }

    case 11:
    case 15:
    case 19:
    case 25:
    case 26:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }


    case 12:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float lightPosition[] = { 10.0f, 10.0f, 10.0f, 0.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 1.5f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 13:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        float lightPosition[] = { 1000.0f, 1000.0f, 1000.0f, 0.0f };

        if (!CameraChanged)
            glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 14:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 16:
    case 17:
    case 18:
    case 27:
    case 28:
    case 29:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (CameraChanged)
            ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);
        else
            ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        if (CameraChanged)
        {
            GLfloat EyePos[] = { 0.0f, 1.5f, 2.0f};
            //GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }
        else
        {
            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        }

        break;
    }

    case 20:
    {
        appInit();
        break;
    }

    case 21:
    {
        GLenum blendSrc[] =
        {
            GL_ONE,
            GL_ONE,
            GL_ONE,
            GL_SRC_ALPHA,
            GL_SRC_ALPHA,

            GL_DST_COLOR,
            GL_ONE_MINUS_DST_COLOR,
            GL_ONE_MINUS_SRC_ALPHA,
            GL_DST_ALPHA,
            GL_SRC_ALPHA_SATURATE
        };

        GLenum blendDst[] =
        {
            GL_ZERO,
            GL_ONE,
            GL_ONE_MINUS_DST_ALPHA,
            GL_ONE,
            GL_ONE_MINUS_SRC_ALPHA,

            GL_SRC_COLOR,
            GL_ONE_MINUS_SRC_COLOR,
            GL_SRC_ALPHA,
            GL_SRC_COLOR,
            GL_SRC_COLOR
        };

        int curBlendMode = 2;

        glClearColor (0.25f, 0.25f, 0.25f, 1.0f);
        glClearDepthf(1.0f);

        glEnable(GL_BLEND);
        glBlendFunc(blendSrc[curBlendMode], blendDst[curBlendMode]);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glViewport(0, 0, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
        glOrthof(0.0f, 3.0f, 0.0f, 3.0f, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        break;
    }

    case 22:
    {
        float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

        if (!loadTextures("./data/zeus.bmp"))
        {
            printf("Error loading textures!\n");
            return 0;
        }

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);

        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        break;
    }

    case 23:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(46.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 1.0f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        break;
    }

    case 24:
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        ugluPerspectivef(60.0, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.6, 0.8, 1.0, 1.0);
        glClearDepthf(1.0f);

        GLfloat EyePos[] = { 0.0f, 0.5f, 2.0f};
        ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        break;
    }


    }// case-switch

    while (1)
    {
        /*
        Step 8 - Draw something with OpenGL ES.
        At this point everything is initialized and we're ready to use
        OpenGL ES to draw something on the screen.
        */
        if (frame_count >= Frame) frameDone = true;
        else ++frame_count;

        if (frameDone) goto cleanup;

        fprintf(APIdumper,"//-------------------- %d th Frame Instr --------------------//\n", frame_count);
        printf("%d th Frame start...\n", frame_count);

        switch (DrawObjectKind)
        {
        case 1:
        {
//	GLfloat vtx[] = {
//					-0.5, -0.5, 0.0, 1.0,
//					0.3, 0.3, 0.0, 1.0,
// 					0.6, 0.0, 0.0, 1.0,
//					0.8, -0.5, 0.0, 1.0,
//					0.9, -0.9, 0.0, 1.0
//                    };
//
//	GLfloat c[] = {	0.9, 0.0, 0.0, 1.0,
//					0.0, 0.9, 0.0, 1.0,
//					0.0, 0.0, 0.9, 1.0,
//					0.9, 0.0, 0.0, 1.0,
//					0.0, 0.9, 0.0, 1.0};
//
//                glMatrixMode(GL_PROJECTION);
//                glLoadIdentity();
//
//                //glFrustumf(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);
//                glMatrixMode(GL_MODELVIEW);
//
//                //glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
//                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//
//                glEnableClientState(GL_VERTEX_ARRAY);
//                glVertexPointer(4, GL_FLOAT, 0, vtx);
//                glEnableClientState(GL_COLOR_ARRAY);
//                glColorPointer(4, GL_FLOAT, 0, c);
//
//                glDrawArrays(GL_TRIANGLE_FAN, 0, 5);

            // Vertex Data
            GLfloat v[] = { 1.0, -1.0, 0.0,
                            0.0,  1.0, 0.0,
                            -1.0, -1.0, 0.0
                          };

            GLfloat pfColors[] = {(0.0f), (0.0f), (1.0f), (1.0f),
                                  (1.0f), (0.0f), (0.0f), (1.0f),
                                  (0.0f), (1.0f), (0.0f), (1.0f)
                                 };

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glFrustumf(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);
            glMatrixMode(GL_MODELVIEW);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearDepthf(1.0f);
            //glClearColor(0.93f, 0.93f, 0.93f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

            glLoadIdentity();
            //glPushMatrix();
            glTranslatef(0.5, 0.0, -2.0);
            glRotatef(theta, 1.0f, 1.0f, 0.0f);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, v);
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT, 0, pfColors);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            //glFinish();
            eglSwapBuffers(eglDisplay,eglSurface);

            theta += 1.0f;
            if (theta > 360.0f)
                theta = 0.0f;
            break;
        }
        case 2:
        {
            GLfloat box[] =
            {
                // FRONT
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // BACK
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // LEFT
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                // RIGHT
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // TOP
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // BOTTOM
                -0.5f, -0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f, -0.5f, -0.5f,
            };

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            //glClearDepthf(1.0f);

            glVertexPointer(3, GL_FLOAT, 0, box);
            glEnableClientState(GL_VERTEX_ARRAY);

            glShadeModel(GL_FLAT);;
            glEnable(GL_CULL_FACE);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            ugluPerspectivef(45.0f, 1.0f * WINDOW_DEFAULT_WIDTH / WINDOW_DEFAULT_HEIGHT, 1.0f, 100.0f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearDepthf(1.0f);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ugluLookAtf(
                0.0f, 0.0f, 3.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);

            glRotatef(xrot, 1.0f, 0.0f, 0.0f);
            glRotatef(yrot, 0.0f, 1.0f, 0.0f);

            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

            eglSwapBuffers(eglDisplay,eglSurface);

            xrot += 0.3f;
            yrot += 0.4f;

            //                if (frame_count==1)
            //                    goto cleanup;

            break;
        }

        case 3:
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ugluLookAtf(
                0.0f, 0.0f, 4.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);

            glRotatef(xrot, 1.0f, 0.0f, 0.0f);
            glRotatef(yrot, 0.0f, 1.0f, 0.0f);

            ugSolidSpheref(1.0f, 24, 24);

            eglSwapBuffers(eglDisplay,eglSurface);

            xrot += 2.0f;
            yrot += 3.0f;

            break;
        }

        case 4:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/teapot.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(-90.0, 1.0, 0.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 5:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/dragon_vrip_res4.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            GLfloat EyePos[] = { 0.0f, 0.0f, 2.0f};
            ugluLookAtf(EyePos[0], EyePos[1], EyePos[2],
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(45.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH );
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 6:
        {
            rotation[0] = 24.5f;

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawOFF("m1413.off");

            eglSwapBuffers(eglDisplay,eglSurface);

            break;
        }

        case 7:

            rotation[0] = 29.0f;

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawOFF("m1397.off");

            eglSwapBuffers(eglDisplay,eglSurface);

            break;

        case 8:
        {
            GLfloat box[] =
            {
                // FRONT
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // BACK
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // LEFT
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                // RIGHT
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // TOP
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // BOTTOM
                -0.5f, -0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f, -0.5f, -0.5f,
            };

            GLfloat texCoords[] =
            {
                // FRONT
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                // BACK
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // LEFT
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // RIGHT
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // TOP
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                // BOTTOM
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f
            };

            glVertexPointer(3, GL_FLOAT, 0, box);
            glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();

            ugluLookAtf(
                0.0f, 0.0f, 3.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);

            glPushMatrix();
            glTranslatef(xtran, 0.0, 0.0);
            //glRotatef(xrot, 1.0f, 0.0f, 0.0f);
            //glRotatef(yrot, 0.0f, 1.0f, 0.0f);
            glRotatef(30.0, 1.0f, 0.0f, 0.0f);
            glRotatef(30.0, 0.0f, 1.0f, 0.0f);

            // FRONT AND BACK
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

            // LEFT AND RIGHT
            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

            // TOP AND BOTTOM
            glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            xtran += xright ? 0.02f : -0.02f;
            if (xtran > 2.0)
                xright = false;
            if (xtran < 0.0)
                xright = true;

            xrot += 2.0f;
            yrot += 3.0f;
            break;
        }

        case 9:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/wow.obj");
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            glTranslatef(0,0,-0.5);
            glRotatef(-90.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 10:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/f-16.obj");
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.5,0.0,0.0);
            //glRotatef(-90.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 11:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/elephal.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(0,1.0,0.5);
            glRotatef(-90.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH );
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 12:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/venus.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            if (CameraChanged)
            {
                glRotatef(yrot, 0.0, 1.0, 0.0);
                glRotatef(-90.0, 1.0, 0.0, 0.0);
            }
            else
                glRotatef(-90.0, 1.0, 0.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            
            yrot += 1.0f;
            break;
        }

        case 13:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/bunny69k.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(-90.0, 1.0, 0.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 14:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/porsche.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            glRotatef(-30.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 15:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/al.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(-30.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 16:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/castle.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(-30.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;
            break;
        }

        case 17:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/flowers.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            if (CameraChanged)
                glTranslatef(0.0,-0.2,0.0);
            //glRotatef(-30.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 18:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/galleon.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            //glTranslatef(1.0,1.0,0.5);
            //glRotatef(-30.0, 0.0, 1.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH );
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 19:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ("./data/head.obj");
                if (!pmodel) exit(0);

                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            glTranslatef(0.0,0.0,-0.5);
            glRotatef(-90.0, 1.0, 0.0, 0.0);

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);
            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 20:
        {
            appRender(tickcount, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);

            eglSwapBuffers(eglDisplay, eglSurface);
            tickcount += 33;
            break;
        }

        case 21:
        {
            GLfloat rectangle[] =
            {
                -1.0f, -0.25f,
                1.0f, -0.25f,
                -1.0f,  0.25f,
                1.0f,  0.25f
            };

            glClear (GL_COLOR_BUFFER_BIT);
            glLoadIdentity();

            glVertexPointer(2, GL_FLOAT, 0, rectangle);

            glEnableClientState(GL_VERTEX_ARRAY);

            glPushMatrix();
            glTranslatef(1.5f, 2.0f, 0.0f);
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0.7f, 1.5f, 0.0f);
            glRotatef(90.0f, 0.0f ,0.0f, 1.0f);
            glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(1.7f, 1.5f, 0.0f);
            glRotatef(90.0f, 0.0f ,0.0f, 1.0f);
            glColor4f(0.0f, 0.0f, 1.0f, 0.25f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(1.5f, 1.0f, 0.0f);
            glColor4f(1.0f, 1.0f, 0.0f, 0.75f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glPopMatrix();

            eglSwapBuffers(eglDisplay, eglSurface);

            break;
        }

        case 22:
        {
            GLfloat box[] =
            {
                // FRONT
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // BACK
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // LEFT
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                // RIGHT
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                // TOP
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                // BOTTOM
                -0.5f, -0.5f,  0.5f,
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f, -0.5f, -0.5f,
            };

            GLfloat texCoords[] =
            {
                // FRONT
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                // BACK
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // LEFT
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // RIGHT
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                // TOP
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                // BOTTOM
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f
            };

            glVertexPointer(3, GL_FLOAT, 0, box);
            glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();

            ugluLookAtf(
                0.0f, 0.0f, 3.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f);

            glTranslatef(xtran, 0.0, 0.0);
            glRotatef(xrot, 1.0f, 0.0f, 0.0f);
            glRotatef(yrot, 0.0f, 1.0f, 0.0f);

            // FRONT AND BACK
            glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

            // LEFT AND RIGHT
            glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

            // TOP AND BOTTOM
            glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
            glNormal3f(0.0f, -1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

            eglSwapBuffers(eglDisplay,eglSurface);

            xtran += xright ? 0.02f : -0.02f;
            if (xtran > 2.0)
                xright = false;
            if (xtran < 0.0)
                xright = true;

            xrot += 2.0f;
            yrot += 3.0f;
            break;

        }

        case 23:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ23);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }
        case 24:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ24);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;

            break;        	
        }
        case 28:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ28);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 25:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ25);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
            glTranslatef(0.0,0.0,-0.5);
            glmDraw(pmodel, GLM_SMOOTH  | GLM_MATERIAL );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;

            break;
        }

        case 26:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ26);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;
        
        	break;
        }
        case 27:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ27);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;
        
        	break;        	
        }
        case 29:
        {
            if (!pmodel)
            {
                pmodel = glmReadOBJ(OBJ29);
                if (!pmodel) exit(0);
                glmUnitize(pmodel);
                glmFacetNormals(pmodel);
                glmVertexNormals(pmodel, 90.0);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL );

            glPopMatrix();

            eglSwapBuffers(eglDisplay,eglSurface);

            glmDelete(pmodel);
            pmodel = NULL;
        
        	break;
        }

        }// case-switch
    }
cleanup:

    if (DrawObjectKind == 8 || DrawObjectKind == 22)
        glDeleteTextures(1, texture);

    if (DrawObjectKind == 20)
    {
        appDeinit();
    }
    
    CloseFile();

    printf("Program execution end.\n");

    //eglSaveSurfaceHM(eglSurface, "SurfaceBitMap.bmp");
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisplay, eglContext);
    //eglDestroySurface(eglDisplay, eglSurface);
}
#else

#error "Unsupported Operating System"

#endif
