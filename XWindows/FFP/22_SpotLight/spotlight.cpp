#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/gl.h>
#include<GL/glu.h>

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

GLXContext gGLXContext;
bool bLight = false;
float cuboid_rotation_angle = 0.0f;
//light configuration parameter
GLfloat LightAmbient[] = { 0.0,0.0,0.0,1.0 };
GLfloat LightDiffuse[] = { 1.0,1.0,1.0,1.0 };
GLfloat LightSpecular[] = { 1.0,1.0,1.0,1.0 };
GLfloat LightPosition[] = { 0.0,3.0,0.0,1.0 };
GLfloat globAmb[] = { 0.05,0.05,0.05,1.0 };
//GLfloat spot_direction[] = { 0.0,-1.0,0.184406244 };

GLfloat spot_direction[] = { 0.0,-1.0,0.0 };

static float spotAngle = 10.0;
static float xMove = 0.0, zMove = 0.0;
static float spotExponent = 2.0;

//entry-point function
int main(void)
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void update();

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
	char keys[26];

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
					XLookupString(&event.xkey,keys,sizeof(keys),NULL,NULL);
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
					switch(keys[0])
						{
							case 't':
							if(spotExponent > 0.0)
								spotExponent -= 0.1;
							
							break;
						case 'T':
							spotExponent += 0.1;
							break;
							
						case 'B' :
							if(spotAngle<90.0)
								spotAngle += 1.0;
							break;
						case 'A' :
							if(spotAngle>0.0)
								spotAngle -= 1.0;
							break;
						case 'R' :
							if(zMove>-4.0)
								zMove -= 0.1;
							break;
						case 'V' :
							if(zMove<4.0)
								zMove += 0.1;
							break;
						case 'S':
							if(xMove >-4.0)
								xMove -=0.1;
							break;
						case 'H':
							if(xMove <4.0)
								xMove +=0.1;
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

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glEnable(GL_LIGHT0);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globAmb);

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


	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 20.0f);
}


void update()
{
	

}

void display(void){

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	gluLookAt(0.0,4.0,6.0,0.0,0.0,0.0,0.0,1.0,0.0);
	glPushMatrix();
	glTranslatef(xMove, 0.0f, zMove);
	
	
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
	glPopMatrix();
	GLUquadric *quadric = gluNewQuadric();
	//Draw 10 x 10 array of multi-colore sphere
	int i,j;
	for(i=0;i<9;i++)
	{
		for(j=0;j<9;j++)
		{
			glPushMatrix();
			glTranslatef(-4.0+i,0.0,-4.0+j);
			//Ambient and diffuse color of the sphere specified to alternate
			if ((i + j) % 3 == 0) {
				glColor3f(1.0, 0.0, 0.0);
			}
			else if ((i + j) % 3 == 1) {
				glColor3f(0.0, 1.0, 0.0);
			}
			else {
				glColor3f(0.0, 0.0, 1.0);
			}
			
			//glColor3f(1.0, 0.0, 1.0);
			gluSphere(quadric,0.4,60,60);
	
			glPopMatrix();
		}
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
