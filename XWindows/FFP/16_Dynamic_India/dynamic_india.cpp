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
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	

	
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




void drawplane()
{
	glBegin(GL_POLYGON);
	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	glVertex3f(0, 0.05, 0);
	glVertex3f(0.2, 0.05, 0);
	glVertex3f(0.25, 0, 0);
	glVertex3f(0.2, -0.05, 0);
	glVertex3f(0, -0.05, 0);

	glEnd();

	// Write IAF
	// I

	glColor3d(1, 1, 1);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	// I
	glVertex3f(0.05, 0.02, 0);
	glVertex3f(0.05, -0.02, 0);
	glVertex3f(0.03, 0.02, 0);
	glVertex3f(0.07, 0.02, 0);
	glVertex3f(0.03, -0.02, 0);
	glVertex3f(0.07, -0.02, 0);

	//A
	glVertex3f(0.095, -0.003, 0);
	glVertex3f(0.123, -0.003, 0);
	glVertex3f(0.085, -0.025, 0);
	glVertex3f(0.105, 0.025, 0);
	glVertex3f(0.105, 0.025, 0);
	glVertex3f(0.13, -0.025, 0);

	//F
	glVertex3f(0.15, 0.023, 0);
	glVertex3f(0.175, 0.023, 0);
	glVertex3f(0.15, 0.0, 0);
	glVertex3f(0.175, 0.0, 0);
	glVertex3f(0.15, 0.025, 0);
	glVertex3f(0.15, -0.025, 0);

	glEnd();


	//draw wing1

	float x, y;

	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);

	for (double i = 0; i <= 1.5;) {
		glBegin(GL_TRIANGLES);
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, 0.06 + y);

		i = i + 0.3;
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);
		glVertex2d(x, 0.06 + y);

		glVertex2d(0.03, 0.06);
		glEnd();

	}

	//draw wing2

	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	for (double i = 0; i <= 1.5;) {
		glBegin(GL_TRIANGLES);
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, -0.06 - y);
		i = i + 0.3;
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, -0.06 - y);

		glVertex2d(0.03, -0.06);
		glEnd();

	}

	//draw exaust part

	glBegin(GL_QUADS);
	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	glVertex3f(-0.02f, 0.05, 0);
	glVertex3f(-0.01, 0.05, 0);

	glVertex3f(-0.01, -0.05, 0);
	glVertex3f(-0.02f, -0.05, 0);
	glEnd();

	//draw Flag

	glBegin(GL_QUADS);
	glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
	glVertex3f(-0.029, 0.0375, 0);
	glVertex3f(-0.029, 0.0125, 0);

	glVertex3f(-0.12, 0.0125, 0);
	glVertex3f(-0.12, 0.0375, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(-0.029, 0.0125, 0);
	glVertex3f(-0.029, -0.0125, 0);
	glVertex3f(-0.12, -0.0125, 0);
	glVertex3f(-0.12, 0.0125, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.029, -0.0125, 0);
	glVertex3f(-0.029, -0.0375, 0);

	glVertex3f(-0.12, -0.0375, 0);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.12, -0.0125, 0);
	glEnd();
}





void display(void){

	static float itranslate = -0.65f;
	static float ntranslate = 1.4f;
	static float i2translate = -1.3f;
	static float atranslate = 0.7f;

	static float planetranslate = -1.7f;

	static float dsaffron_translate[3] = { 0,0,0 };
	static float dgreen_translate[3] = { 0,0,0 };

	static bool iDone = false;
	static bool nDone = false;
	static bool dDone = false;
	static bool i2Done = false;
	static bool aDone = false;
	static bool flag = false;
	
	static bool downplanedone = false;

	bool x1done, x2done, x2i2cross;
	x1done = x2done = x2i2cross = false;


	static float Upper_plane_xtranslation = -1.7f;
	static float Upper_plane_ytranslation = 1.1f;

	static float Upper_plane_rotation_angle = -90.0f;

	static float down_plane_xtranslation = -1.7f;
	static float down_plane_ytranslation = -1.1f;
	//static float down_plane_translation_angle = 180.0f;
	static float down_plane_rotation_angle = 90.0f;
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (downplanedone == false)
	{
		if (dDone) {

			// Draw Down Plane

			

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//glTranslatef(down_plane_xtranslation + (0.56 * sin(down_plane_translation_angle)), down_plane_ytranslation + (1.1 * sin(down_plane_translation_angle)), -2.0f);
			glTranslatef(down_plane_xtranslation, down_plane_ytranslation, -2.0f);
			glRotatef(down_plane_rotation_angle, 0, 0, 1);
			//*****************************************************************************************************
			// Draw Plane
			
			drawplane();

			
		

	// Draw Upper Plane


			

			// Draw Plane
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(Upper_plane_xtranslation, Upper_plane_ytranslation, -2.0f);
			glRotatef(Upper_plane_rotation_angle, 0, 0, 1);

			drawplane();
		
			
		}

	}
		
	if (dDone) {

		if (down_plane_xtranslation < -0.91) {


			down_plane_ytranslation = down_plane_ytranslation + 0.0035;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.3f;

			
		}
		else {
			downplanedone = true;
		}

		if (down_plane_xtranslation > 0.73) {
			downplanedone = false;
			down_plane_ytranslation = down_plane_ytranslation - 0.0035;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.3f;

		}
				
		if (Upper_plane_xtranslation < -0.91) {


			Upper_plane_ytranslation = Upper_plane_ytranslation - 0.0035;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.3f;
		}

		if (Upper_plane_xtranslation > 0.73) {

			Upper_plane_ytranslation = Upper_plane_ytranslation + 0.0035;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.3f;

		}

		if (planetranslate < -0.74) {
			down_plane_xtranslation = down_plane_xtranslation + 0.0025f;
			Upper_plane_xtranslation = Upper_plane_xtranslation + 0.0025f;
		}
		else
		{
			down_plane_xtranslation = down_plane_xtranslation + 0.0038f;
			Upper_plane_xtranslation = Upper_plane_xtranslation + 0.0038f;
		}
		

	}
	










	//Draw I


	if (itranslate < 0.001)
		itranslate = itranslate + 0.000025;
	else
		iDone = true;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(itranslate, 0.0f, -2.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);
	glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
	glVertex3f(-0.85, 0.6, 0);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.85, -0.6, 0);
	glEnd();

	//Draw N

	if (aDone) {
		if (ntranslate > 0)
			ntranslate = ntranslate - 0.000072;
		else
			nDone = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, ntranslate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.6, -0.6, 0);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.6, 0.6, 0);

		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.6, 0.6, 0);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.3, -0.6, 0);

		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.3, -0.6, 0);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.3, 0.6, 0);
		glEnd();
	}

	

	//Draw D

	if (i2Done) {

		if (dsaffron_translate[0] < (1 + ((255.0 / 255.0) / 20000)))
		{
			dsaffron_translate[0] = dsaffron_translate[0] + ((255.0 / 255.0) / 20000);
			dsaffron_translate[1] = dsaffron_translate[1] + ((153.0 / 255.0) / 20000);
			dsaffron_translate[2] = dsaffron_translate[2] + ((51.0 / 255.0) / 20000);
			dgreen_translate[0] = dgreen_translate[0] + ((18.0 / 255.0) / 20000);
			dgreen_translate[1] = dgreen_translate[1] + ((136.0 / 255.0) / 20000);
			dgreen_translate[2] = dgreen_translate[2] + ((7.0 / 255.0) / 20000);

		}
		else {

			dsaffron_translate[0] = 255.0 / 255.0;
			dsaffron_translate[1] = 153.0 / 255.0;
			dsaffron_translate[2] = 51.0 / 255.0;
			dgreen_translate[0] = 18.0 / 255.0;
			dgreen_translate[1] = 136.0 / 255.0;
			dgreen_translate[2] = 7.0 / 255.0;
			dDone = true;

		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(-0.15, -0.6, 0);
		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(-0.15, 0.6, 0);

		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(-0.2, 0.588, 0);
		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(0.1, 0.588, 0);


		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(0.1, 0.6, 0);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(0.1, -0.6, 0);

		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(0.1, -0.588, 0);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(-0.2, -0.588, 0);
		glEnd();
	}



	//Draw I2

	if (nDone) {
		if (i2translate < 0)
			i2translate = i2translate + 0.00008;
		else
			i2Done = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, i2translate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(0.35, 0.6, 0);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(0.35, -0.6, 0);
		glEnd();
	}


	


	//Draw A
	if (iDone) {
		if (atranslate > 0)
			atranslate = atranslate - 0.000025;
		else
			aDone = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(atranslate, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);

	
		if (x2done == false) {
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.6, -0.6, 0);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
		}

		
		if (x1done == false) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.9, -0.6, 0);
		}
		

		glEnd();
	}

	//Middle Plane start
	if (dDone) {
		if (planetranslate < -0.74) {
			planetranslate = planetranslate + 0.0025;
			
		    
		}
		else if (planetranslate < 1.5) {
			planetranslate = planetranslate + 0.0038;
			flag = true;
		}
		

			


		// Draw Plane
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(planetranslate, 0.0f, -2.0f);

		glBegin(GL_POLYGON);
		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		glVertex3f(0, 0.05, 0);
		glVertex3f(0.2, 0.05, 0);
		glVertex3f(0.25, 0, 0);
		glVertex3f(0.2, -0.05, 0);
		glVertex3f(0, -0.05, 0);

		glEnd();

		// Write IAF
		// I
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//glTranslatef(planetranslate, 0.0, -2.0f);
		glColor3d(1, 1, 1);
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		// I
		glVertex3f(0.05, 0.02, 0);
		glVertex3f(0.05, -0.02, 0);
		glVertex3f(0.03, 0.02, 0);
		glVertex3f(0.07, 0.02, 0);
		glVertex3f(0.03, -0.02, 0);
		glVertex3f(0.07, -0.02, 0);

		
		//A
		glVertex3f(0.095, -0.003, 0);
		glVertex3f(0.123, -0.003, 0);
		glVertex3f(0.085, -0.025, 0);
		glVertex3f(0.105, 0.025, 0);
		glVertex3f(0.105, 0.025, 0);
		glVertex3f(0.13, -0.025, 0);

		//F
		glVertex3f(0.15, 0.023, 0);
		glVertex3f(0.175, 0.023, 0);
		glVertex3f(0.15, 0.0, 0);
		glVertex3f(0.175, 0.0, 0);
		glVertex3f(0.15, 0.025, 0);
		glVertex3f(0.15, -0.025, 0);
		
		glEnd();
		

		//draw wing1

		float x, y;

		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);

		for (double i = 0; i <= 1.5;) {
			glBegin(GL_TRIANGLES);
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, 0.06 + y);

			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);
			glVertex2d(x, 0.06 + y);

			glVertex2d(0.03, 0.06);
			glEnd();

		}

		//draw wing2

		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		for (double i = 0; i <= 1.5;) {
			glBegin(GL_TRIANGLES);
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, -0.06 - y);
			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, -0.06 - y);

			glVertex2d(0.03, -0.06);
			glEnd();

		}

		//draw exaust part
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(planetranslate -0.02f, 0.0f, -2.0f);

		glBegin(GL_QUADS);
		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		glVertex3f(0, 0.05, 0);
		glVertex3f(0.01, 0.05, 0);

		glVertex3f(0.01, -0.05, 0);
		glVertex3f(0, -0.05, 0);
		glEnd();

		//draw Flag
		if (flag)
		{
			
			static float x = -0.03f;
			static float x1, x2 = -0.855f;
			static bool flag_blur = false;

			if (planetranslate < 1.5) {
				x1 = x + planetranslate;
			}
			else {
				flag_blur = true;
			}
				

			if (planetranslate > 1.5 && x2 < 0.667) {
				x2 = x2 + 0.00247;
				
			}

			if (x2 > 0.667 ) {
				x2done = true;
				
			}
			if (x2 > 0.1)
			{
				x2i2cross = true;
			}
			if (x2 > 0.35)
			{
				x2done = true;
			}

			if (planetranslate > 1.5  && x1 > 0.82) {
				
				x1 = x1 - 0.0011;
			}

			if (planetranslate > 1.5 && x1 < 0.82) {
				
				x1done = true;
			}
				
				
			if (flag_blur) {


				// safron
				static float flagsafron[3] = { 255.0 / 255.0 , 153.0 / 255.0, 51.0 / 255.0 };

				flagsafron[0] = flagsafron[0] - ((255.0 / 255.0) / 650);
				flagsafron[1] = flagsafron[1] - ((153.0 / 255.0) / 650);
				flagsafron[2] = flagsafron[2] - ((51.0 / 255.0) / 650);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glTranslatef(0.0, 0.0f, -2.0f);

				
				glBegin(GL_QUADS);
				glColor3f(flagsafron[0], flagsafron[1], flagsafron[2]);
				glVertex3f(0.667, 0.0375, 0);
				glVertex3f(0.667, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(x2, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, 0.0375, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
				glVertex3f(0.667, 0.0375, 0);
				glVertex3f(0.667, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(0.82, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, 0.0375, 0);
				glEnd();




				glBegin(GL_QUADS);
				glColor3f(flagsafron[0], flagsafron[1], flagsafron[2]);
				glVertex3f(x1, 0.0375, 0);
				glVertex3f(x1, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(0.82, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, 0.0375, 0);
				glEnd();


				// white

				static float flagwhite = 255.0 / 255.0;

				flagwhite = flagwhite - ((255.0 / 255.0) / 650);
				

				glBegin(GL_QUADS);
				glColor3f(flagwhite, flagwhite, flagwhite);
				glVertex3f(0.667, 0.0125, 0);
				glVertex3f(0.667, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0125, 0);
				glVertex3f(x2, 0.0125, 0);
				glEnd();



				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
				glVertex3f(0.667, 0.0125, 0);
				glVertex3f(0.667, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0125, 0);
				glVertex3f(0.82, 0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(flagwhite, flagwhite, flagwhite);
				glVertex3f(x1, 0.0125, 0);
				glVertex3f(x1, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0125, 0);
				glVertex3f(0.82, 0.0125, 0);
				glEnd();



				// green

				static float flaggreen[3] = { 18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0 };

				flaggreen[0] = flaggreen[0] - ((18.0 / 255.0) / 650);
				flaggreen[1] = flaggreen[1] - ((136.0 / 255.0) / 650);
				flaggreen[2] = flaggreen[2] - ((7.0 / 255.0) / 650);



				glBegin(GL_QUADS);
				glColor3f(flaggreen[0], flaggreen[1], flaggreen[2]);
				glVertex3f(0.667, -0.0125, 0);
				glVertex3f(0.667, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0375, 0);
				glVertex3f(x2, -0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(0.667, -0.0125, 0);
				glVertex3f(0.667, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0375, 0);
				glVertex3f(0.82, -0.0125, 0);
				glEnd();


				glBegin(GL_QUADS);
				glColor3f(flaggreen[0], flaggreen[1], flaggreen[2]);
				glVertex3f(x1, -0.0125, 0);
				glVertex3f(x1, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0375, 0);
				glVertex3f(0.82, -0.0125, 0);
				glEnd();

			}
			else {


				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glTranslatef(0.0, 0.0f, -2.0f);

				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
				glVertex3f(x1, 0.0375, 0);
				glVertex3f(x1, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(x2, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, 0.0375, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(1, 1, 1);
				glVertex3f(x1, 0.0125, 0);
				glVertex3f(x1, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0125, 0);
				glVertex3f(x2, 0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(x1, -0.0125, 0);
				glVertex3f(x1, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0375, 0);
				glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(x2, -0.0125, 0);
				glEnd();
			}
		}
		else {
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(planetranslate - 0.02f, 0.0f, -2.0f);

			glBegin(GL_QUADS);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(-0.009, 0.0375, 0);
			glVertex3f(-0.009, 0.0125, 0);

			glVertex3f(-0.1, 0.0125, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, 0.0375, 0);
			glEnd();

			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			glVertex3f(-0.009, 0.0125, 0);
			glVertex3f(-0.009, -0.0125, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, -0.0125, 0);
			glVertex3f(-0.1, 0.0125, 0);
			glEnd();

			glBegin(GL_QUADS);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(-0.009, -0.0125, 0);
			glVertex3f(-0.009, -0.0375, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, -0.0375, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(-0.1, -0.0125, 0);
			glEnd();
		}
		

	}

	




	//Draw I2

	if (nDone) {
		/*if (i2translate < 0)
			i2translate = i2translate + 0.008;
		else
			i2Done = true;*/

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, i2translate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		if (x2i2cross) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.35, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.35, -0.6, 0);

		}
		
		glEnd();
	}


	
	//Draw A
	if (iDone) {
		/*if (atranslate > 0)
			atranslate = atranslate - 0.0025;
		else
			aDone = true;*/

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(atranslate, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);

		
		if (x2done == true) {
		//if (x2done) {
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.6, -0.6, 0);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
		}


		if (x1done == true) {
		//if (x2done) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.9, -0.6, 0);
		}


		glEnd();
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
