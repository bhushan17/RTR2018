#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/gl.h>
#include<GL/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>



//namespace
using namespace std;

//Global variable
bool bFullscreen=false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight=600;

GLuint pyramid_texture, cube_texture;
GLXContext gGLXContext;


float triangle_rotation_angle = 0.0f;
float cuboid_rotation_angle = 0.0f;
//entry-point function
int main(void)
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void update(void);

	void resize(int, int);
	void display(void);
	void intialize(void);
	
	//variable decllaration
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	bool bDone = false;
	//code
	CreateWindow();
	
	intialize();
	//Message Loop
	XEvent event;
	KeySym keysym;
	

	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			
			XNextEvent(gpDisplay, &event);
			switch(event.type)
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode,0,0);
					switch(keysym)
					{
						case XK_Escape:
							bDone = true;
							break;
						case XK_F:
						case XK_f:
							if(bFullscreen==false)
							{
								ToggleFullscreen();
								bFullscreen=true;
							}
							else{
								ToggleFullscreen();
								bFullscreen=false;
							}
							break;
						default:
							break;
					
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						default:
							break;
					}
					break;
				case MotionNotify:
					break;
				case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose:
					break;
				case DestroyNotify:
					break;
				case 33:
					bDone = true;
					break;
				default:
					break;
			}
		}
			
		update();
		display();
	}
	uninitialize();
	return(0);


}

void CreateWindow(void)
{
	
	//function prorttypes
	void uninitialize(void);

	//variable declaring
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	//OpenGL Variable declaration 
	static int frameBufferAttribute[]={GLX_RGBA,
					   GLX_DOUBLEBUFFER, True,
					   GLX_WINDOW_BIT,
					   GLX_RED_SIZE, 8,
					   GLX_GREEN_SIZE, 8,
					   GLX_BLUE_SIZE, 8,
					   GLX_ALPHA_SIZE, 8,
					   GLX_DEPTH_SIZE,24,
					   None};
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Open X Display.\nExitting Now ...\n");
		uninitialize();
		exit(1);
	}

	defaultScreen=XDefaultScreen(gpDisplay);
	defaultDepth=DefaultDepth(gpDisplay, defaultScreen);

	gpXVisualInfo=glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttribute);
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR : Unable To Get A Visual. \n Exitting Now ...\n");
		uninitialize();
		exit(1);
	}

	
	winAttribs.border_pixel=0;  // Default Color
	winAttribs.background_pixmap = 0; // No Image
	winAttribs.colormap=XCreateColormap(gpDisplay, 
				RootWindow(gpDisplay, gpXVisualInfo->screen), //Parent
				gpXVisualInfo->visual,  // Hardware
				AllocNone); // No Memory Allocated
	gColormap=winAttribs.colormap;    // Stored
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);  // HBRUSH, BLACK_BRUSH

	winAttribs.event_mask= ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow=XCreateWindow(gpDisplay, 
			      RootWindow(gpDisplay, gpXVisualInfo->screen),  // parent
  			      0, // X
			      0,  // Y
			      giWindowWidth,  //Width
			      giWindowHeight, //Height
			      0, // Border Width
			      gpXVisualInfo->depth, // Bit Depth 
			      InputOutput, // Window Can Do Both Input And OutPut
			      gpXVisualInfo->visual, // Hardware
			      styleMask, // Style  Mask
			      &winAttribs);

	if(!gWindow)
	{
		printf("ERROR : Failed To Creat Main Window.\n Exitting Now...\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "First XWindow");

	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}



void intialize(void)
{
	//function Declaration
	void unintialize(void);
	void resize(int, int);
	
	// Code

 	// Creating Rendering Contex(HGLRC) using gpXVisualInfo
	gGLXContext = glXCreateContext(gpDisplay,
					gpXVisualInfo,
					NULL,
					GL_TRUE);


	glXMakeCurrent(gpDisplay,gWindow, gGLXContext);
	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearDepth(1.0f);

	//glDepthFunc(GL_LEQUAL);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	

	
	resize(giWindowWidth, giWindowHeight);
	
	
}




void resize(int width, int height){

        if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height,0.1f,100.0f);
}


void update()
{
	if (triangle_rotation_angle > 360)
	{

		triangle_rotation_angle = 0.0f;
		cuboid_rotation_angle = 0.0f;
	}
	else {

		triangle_rotation_angle = triangle_rotation_angle + 0.009f;
		cuboid_rotation_angle = cuboid_rotation_angle + 0.009f;
	}

}

void display(void){

	int index, numpoints = 1000;
	float radius = (2 / (1 + sqrt(5.0f)));
	float angle = 0.0f;
	//float perimeter = 0.0f;
	//float incenterx = 1 / (1 + sqrt(5.0f));
	float incentery = (1 - sqrt(5.0f)) / (1 + sqrt(5.0f));
	//static float lineteanslatey = 1.0f;
	static float lineycoordinate = 1.0f;
	//static float lineycoordinate1 = 7.0f;
	//static float linezcoordinate = 0.0f;
	static float triangletranslatex = 1.0f;

	static float triangletranslate = 0.0f;
	static float circletranslate = 0.0f;
	static float linetranslate = 0.0f;

	//static float circletranslatex = -1.0f;
	//float slope = 1 + incentery;
	//static float ztranslation = 0.0f;

	static float trianglerotationangle = 0.0f;
	static float circlerotationangle = 0.0f;

	int trianglestatus = 0;
	int circlestatus = 0;



	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1 - triangletranslate, -1 + triangletranslate, -12 * triangletranslate);
	glRotatef(trianglerotationangle, 0.0f, 1.0f, 0.0f);

	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINE_LOOP);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	if (triangletranslate < 0.7f) {
		triangletranslate = triangletranslate + 0.00001f;
	}
	else if (triangletranslate < 1.0f) {
		triangletranslate = triangletranslate + 0.00001f;
		trianglestatus = 1;
	}
	else {
		triangletranslate = 1.0f;
		trianglestatus = 1;
	}

	if (trianglestatus == 1)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-1 + circletranslate, -1 + (incentery + 1) * circletranslate, -12.0 * circletranslate);
		glRotatef(circlerotationangle, 0.0f, 1.0f, 0.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f);
		for (index = 0; index < numpoints;index++)
		{
			angle = (2 * 3.148*index) / numpoints;
			glVertex3f(radius * cos(angle), radius * sin(angle), 0);
		}
		glEnd();



		if (circletranslate < 0.7f) {
			circletranslate = circletranslate + 0.00001f;

		}
		else if (circletranslate < 1.0f) {
			circletranslate = circletranslate + 0.00001f;
			circlestatus = 1;
		}
		else {
			circletranslate = 1.0f;
			circlestatus = 1;
		}


	}



	if (circlestatus) {

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 1 - linetranslate, -12.0f * linetranslate);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, lineycoordinate, 0.0f);

		glEnd();

		if (linetranslate < 1.0f) {
			linetranslate = linetranslate + 0.00001f;
			lineycoordinate = lineycoordinate - 0.00002f;
		}
		else {
			linetranslate = 1.0f;
			lineycoordinate = -1.0f;
		}

	}



	if (trianglerotationangle > 360)
	{
		trianglerotationangle = 0.0f;
		circlerotationangle = 0.0f;

	}
	else {
		trianglerotationangle = trianglerotationangle + 0.09f;
		circlerotationangle = circlerotationangle + 0.09f;
	}


	glXSwapBuffers(gpDisplay, gWindow);
}

void ToggleFullscreen(void)
{
	//Variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};

	//code
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));

	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bFullscreen ? 0 : 1;
	fullscreen=XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;

	XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);
}

void uninitialize(void)
{

	GLXContext currentGLXContext = glXGetCurrentContext();
	
	if((currentGLXContext != NULL) && (currentGLXContext == gGLXContext)){
		glXMakeCurrent(gpDisplay,0,0);
	
		if(gGLXContext){
			glXDestroyContext(gpDisplay,gGLXContext);
		}
	}

	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);
	}
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}

 
/*


*/					
