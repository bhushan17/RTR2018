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


bool bLight = false;
float angleOfXrotation = 0.0f;
float angleOfYrotation = 0.0f;
float angleOfZrotation = 0.0f;
//light configuration parameter
GLint keyPress = 0;

//light configuration parameter
GLfloat LightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
//GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f,0.0f,0.0f,1.0f };


GLfloat Light_Model_Ambient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat Light_Model_Local_Viewer[] = { 0.0f };



GLUquadric *quadric[24] = { NULL };
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
						case 'L':
						case 'l':
							if (bLight == false)
							{
								bLight = true;
								glEnable(GL_LIGHTING);
							}
							else {
								bLight = false;
								glDisable(GL_LIGHTING);
							}
							break;
						case 'X':
						case 'x':
							keyPress = 1;
							angleOfXrotation = 0.0f;
							break;
						case 'Y':
						case 'y':
							keyPress = 2;
							angleOfYrotation = 0.0f;
							break;
						case 'Z':
						case 'z':
							keyPress = 3;
							angleOfZrotation = 0.0f;
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
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);



	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++) {
		quadric[i] = gluNewQuadric();
	}
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


	if (width <= height)
	{
		glOrtho(0.0f, //left
			15.5f, //right
			0.0f, //bottom
			(15.5f * ((GLfloat)height / (GLfloat)width)), //top
			-10.0f,//near
			10.0f//far
		);
	}
	else {
		glOrtho(0.0f,
			(15.5f * ((GLfloat)width / (GLfloat)height)),
			0.0f,
			15.5f,
			-10.0f,
			10.0f);
	}

}


void update()
{
	/*if (angleOfXrotation > 360)
	{
	    angleOfXrotation = 0.0f;
	}
	else {
	    angleOfXrotation = angleOfXrotation + 2.0f;
	}
	if (angleOfYrotation > 360)
	{
		angleOfYrotation = 0.0f;
	}
	else {
		angleOfYrotation = angleOfYrotation + 2.0f;
	}
	if (angleOfZrotation > 360)
	{		
		angleOfZrotation = 0.0f;
	}
	else {		
		angleOfZrotation = angleOfZrotation + 2.0f;
	}*/
	angleOfXrotation = angleOfXrotation + 0.009f;
	angleOfYrotation = angleOfYrotation + 0.009f;
	angleOfZrotation = angleOfZrotation + 0.009f;

}

void display(void){

	//code
	void draw24Sphere(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Cuboid

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPress == 1) {
		//angleOfYrotation = 0.0f;
		//angleOfZrotation = 0.0f;
		LightPosition[0] = 0.0f;
		LightPosition[1] = angleOfXrotation;
		LightPosition[2] = 0.0f;
		glRotatef(angleOfXrotation, 1.0f, 0.0f, 0.0f);
		//angleOfXrotation = angleOfXrotation + 2.0f;
	}
	else if (keyPress == 2) {
		//angleOfXrotation = 0.0f;
		//angleOfZrotation = 0.0f;
		LightPosition[0] = 0.0f;
		LightPosition[1] = 0.0f;
		LightPosition[2] = angleOfYrotation;
		glRotatef(angleOfYrotation, 0.0f, 1.0f, 0.0f);
		//angleOfYrotation = angleOfYrotation + 2.0f;
	}
	else if (keyPress == 3) {
		//angleOfYrotation = 0.0f;
		//angleOfXrotation = 0.0f;
		LightPosition[0] = angleOfZrotation;
		LightPosition[1] = 0.0f;
		LightPosition[2] = 0.0f;
		glRotatef(angleOfZrotation, 0.0f, 0.0f, 1.0f);
		//angleOfZrotation = angleOfZrotation + 2.0f;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	draw24Sphere();

	glXSwapBuffers(gpDisplay, gWindow);
}



void draw24Sphere() {

	// Material Configuration
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShianyness[1];


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//************* 1st Sphere on 1st column, emerald **************//
	MaterialAmbient[0] = 0.0215;
	MaterialAmbient[1] = 0.1745; 
	MaterialAmbient[2] = 0.0215;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.07568;
	MaterialDiffuse[1] = 0.61424;
	MaterialDiffuse[2] = 0.07568;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT,GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.633;
	MaterialSpecular[1] = 0.727811;
	MaterialSpecular[2] = 0.633;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] =  0.6 * 128 ;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 14.0f, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);

	//************* 2nd Sphere on 1st column, jade **************//
	MaterialAmbient[0] = 0.135;
	MaterialAmbient[1] = 0.2225;
	MaterialAmbient[2] = 0.1575;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.54;
	MaterialDiffuse[1] = 0.89;
	MaterialDiffuse[2] = 0.63;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.316228;
	MaterialSpecular[1] = 0.316228;
	MaterialSpecular[2] = 0.316228;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 11.4f, 0.0f);
	gluSphere(quadric[1], 1.0f, 30, 30);


	//************* 3rd Sphere on 1st column, obsidian **************//
	MaterialAmbient[0] = 0.05375;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.06625;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.18275;
	MaterialDiffuse[1] = 0.17;
	MaterialDiffuse[2] = 0.22525;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.332741;
	MaterialSpecular[1] = 0.328634;
	MaterialSpecular[2] = 0.346435;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.3* 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 8.9f, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);


	//************* 4th Sphere on 1st column, pearl **************//
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.20725;
	MaterialAmbient[2] = 0.20725;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 1.0;
	MaterialDiffuse[1] = 0.829;
	MaterialDiffuse[2] = 0.829;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.296648;
	MaterialSpecular[1] = 0.296648;
	MaterialSpecular[2] = 0.296648;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.088 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 6.4f, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);


	//************* 5th Sphere on 1st column, ruby **************//
	MaterialAmbient[0] = 0.1745;
	MaterialAmbient[1] = 0.01175;
	MaterialAmbient[2] = 0.01175;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.61424;
	MaterialDiffuse[1] = 0.04136;
	MaterialDiffuse[2] = 0.04136;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.727811;
	MaterialSpecular[1] = 0.626959;
	MaterialSpecular[2] = 0.626959;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 3.9f, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);

	//************* 6th Sphere on 1st column, turquoise **************//
	MaterialAmbient[0] = 0.1;
	MaterialAmbient[1] = 0.18725;
	MaterialAmbient[2] = 0.1745;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.396;
	MaterialDiffuse[1] = 0.74151;
	MaterialDiffuse[2] = 0.69102;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.297254;
	MaterialSpecular[1] = 0.30829;
	MaterialSpecular[2] = 0.306678;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 1.4f, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);









	//************* 1st Sphere on 2nd column, brass **************//
	MaterialAmbient[0] = 0.329412;
	MaterialAmbient[1] = 0.223529;
	MaterialAmbient[2] = 0.027451;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.780392;
	MaterialDiffuse[1] = 0.568627;
	MaterialDiffuse[2] = 0.113725;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.992157;
	MaterialSpecular[1] = 0.941176;
	MaterialSpecular[2] = 0.807843;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.21794872 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 14.0f, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);

	//************* 2nd Sphere on 2nd column, bronze **************//
	MaterialAmbient[0] = 0.2125;
	MaterialAmbient[1] = 0.1275;
	MaterialAmbient[2] = 0.054;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.714;
	MaterialDiffuse[1] = 0.4284;
	MaterialDiffuse[2] = 0.18144;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.393548;
	MaterialSpecular[1] = 0.271906;
	MaterialSpecular[2] = 0.166721;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.2 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 11.4f, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);


	//************* 3rd Sphere on 2nd column, chrome **************//
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.25;
	MaterialAmbient[2] = 0.25;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.774597;
	MaterialSpecular[1] = 0.774597;
	MaterialSpecular[2] = 0.774597;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 8.9f, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);


	//************* 4th Sphere on 2nd column, pearl **************//
	MaterialAmbient[0] = 0.19125;
	MaterialAmbient[1] = 0.0735;
	MaterialAmbient[2] = 0.0225;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.7038;
	MaterialDiffuse[1] = 0.27048;
	MaterialDiffuse[2] = 0.0828;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.256777;
	MaterialSpecular[1] = 0.137622;
	MaterialSpecular[2] = 0.086014;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 6.4f, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);


	//************* 5th Sphere on 2nd column, ruby **************//
	MaterialAmbient[0] = 0.24725;
	MaterialAmbient[1] = 0.1995;
	MaterialAmbient[2] = 0.0745;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.75164;
	MaterialDiffuse[1] = 0.60648;
	MaterialDiffuse[2] = 0.22648;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.628281;
	MaterialSpecular[1] = 0.555802;
	MaterialSpecular[2] = 0.366065;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 3.9f, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);

	//************* 6th Sphere on 2nd column, silver **************//
	MaterialAmbient[0] = 0.19225;
	MaterialAmbient[1] = 0.19225;
	MaterialAmbient[2] = 0.19225;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.50754;
	MaterialDiffuse[1] = 0.50754;
	MaterialDiffuse[2] = 0.50754;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.508273;
	MaterialSpecular[1] = 0.508273;
	MaterialSpecular[2] = 0.508273;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 1.4f, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);









	//************* 1st Sphere on 3nd column, black **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.5;
	MaterialSpecular[1] = 0.5;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 14.0f, 0.0f);
	gluSphere(quadric[12], 1.0f, 30, 30);

	//************* 2nd Sphere on 3nd column, cyan **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.1;
	MaterialAmbient[2] = 0.06;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.0;
	MaterialDiffuse[1] = 0.50980392;
	MaterialDiffuse[2] = 0.50980392;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.50196078;
	MaterialSpecular[1] = 0.50196078;
	MaterialSpecular[2] = 0.50196078;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 11.4f, 0.0f);
	gluSphere(quadric[13], 1.0f, 30, 30);


	//************* 3rd Sphere on 3nd column, green **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.1;
	MaterialDiffuse[1] = 0.35;
	MaterialDiffuse[2] = 0.1;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.45;
	MaterialSpecular[1] = 0.55;
	MaterialSpecular[2] = 0.45;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 8.9f, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);


	//************* 4th Sphere on 3nd column, red **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.0;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.6;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 6.4f, 0.0f);
	gluSphere(quadric[15], 1.0f, 30, 30);


	//************* 5th Sphere on 3nd column, white **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.55;
	MaterialDiffuse[1] = 0.55;
	MaterialDiffuse[2] = 0.55;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.70;
	MaterialSpecular[1] = 0.70;
	MaterialSpecular[2] = 0.70;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 3.9f, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);

	//************* 6th Sphere on 3nd column, plastic **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.6;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 1.4f, 0.0f);
	gluSphere(quadric[17], 1.0f, 30, 30);






	//************* 1st Sphere on 4th column, black **************//
	MaterialAmbient[0] = 0.02;
	MaterialAmbient[1] = 0.02;
	MaterialAmbient[2] = 0.02;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.4;
	MaterialSpecular[1] = 0.4;
	MaterialSpecular[2] = 0.4;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 14.0f, 0.0f);
	gluSphere(quadric[18], 1.0f, 30, 30);

	//************* 2nd Sphere on 4th column, cyan **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 11.4f, 0.0f);
	gluSphere(quadric[19], 1.0f, 30, 30);


	//************* 3rd Sphere on 4th column, green **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 8.9f, 0.0f);
	gluSphere(quadric[20], 1.0f, 30, 30);


	//************* 4th Sphere on 4th column, red **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.04;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 6.4f, 0.0f);
	gluSphere(quadric[21], 1.0f, 30, 30);


	//************* 5th Sphere on 4th column, white **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 3.9f, 0.0f);
	gluSphere(quadric[22], 1.0f, 30, 30);

	//************* 6th Sphere on 4th column, plastic **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 1.4f, 0.0f);
	gluSphere(quadric[23], 1.0f, 30, 30);








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
