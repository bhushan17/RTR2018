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

#include<SOIL/SOIL.h>

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

GLuint texture_smily;
GLXContext gGLXContext;
int pressdigit = 0;
//entry-point function
int main(void)
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();

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
					//XLookupString(&event.xkey,keys,sizeof(keys),NULL,NULL);
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
						//case 0x49:
						//case VK_NUMPAD1:
						case '1':
							glEnable(GL_TEXTURE_2D);
							pressdigit = 1;
							break;

						//case 0x50:
						//case VK_NUMPAD2:
						case '2':
							glEnable(GL_TEXTURE_2D);
							pressdigit = 2;
							break;

						case 51:
						//case VK_NUMPAD3:
							glEnable(GL_TEXTURE_2D);
							pressdigit = 3;
							break;

						case 52:
						//case VK_NUMPAD4:
							glEnable(GL_TEXTURE_2D);
							pressdigit = 4;
							break;

						default:
							glDisable(GL_TEXTURE_2D);
							pressdigit = 0;
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
			
		//update();
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
	bool loadTexture(GLuint *texture, const char *path);
	// Code

 	// Creating Rendering Contex(HGLRC) using gpXVisualInfo
	gGLXContext = glXCreateContext(gpDisplay,
					gpXVisualInfo,
					NULL,
					GL_TRUE);

	glXMakeCurrent(gpDisplay,gWindow, gGLXContext);
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	loadTexture(&texture_smily, "Smiley.bmp");
	resize(giWindowWidth, giWindowHeight);
	
	glEnable(GL_TEXTURE_2D);
}


bool loadTexture(GLuint *texture, const char *path)
{
	
	//HBITMAP hBitmap = NULL;
	//BITMAP bmp;
	bool bStatus = false;
	int width;
	int height;
        unsigned char* imagedata = NULL;

	//hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResouceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	imagedata = SOIL_load_image(path, &width, &height,0,SOIL_LOAD_RGB);

	if(imagedata == NULL)
	{
		bStatus = false;
		return (bStatus);
	}
	else{
		bStatus = true;
	}
	

		//bStatus = TRUE;
		//GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE,imagedata);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		//DeleteObject(hBitmap);
		SOIL_free_image_data(imagedata);


	return (bStatus);
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


void display(void){

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);
	//glRotatef(triangle_rotation_angle, 0.0f, 1.0f, 0.0f);


	glBindTexture(GL_TEXTURE_2D, texture_smily);

	
	

	
	glBegin(GL_QUADS);

	if (pressdigit == 1) {
		glTexCoord2f(0.0f, 0.0f);
		//glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.0f);
		//glTexCoord2f(0.0f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		//glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.5f);
		//glTexCoord2f(0.5f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	else if (pressdigit == 2) {

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	else if(pressdigit == 3){
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(2.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(2.0f, 2.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 2.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	else if (pressdigit == 4) {
		
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	}
	else {

		//glColor3f(1, 1, 1);
		glVertex3f(1.0f, 1.0f, 0.0f);
		
		glVertex3f(-1.0f, 1.0f, 0.0f);
		
		glVertex3f(-1.0f, -1.0f, 0.0f);
		
		glVertex3f(1.0f, -1.0f, 0.0f);
	}

	glEnd();

	



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
