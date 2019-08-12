#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

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

//entry-point function
int main(void)
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	
	//variable decllaration
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;

	static XFontStruct *pxfontstruct = NULL;
	static GC gc;
	static XGCValues gcValues;
	XColor text_color;
	char str[255]="HELLO WORLD !!!";
	int strlength;
	static int strWidth;
	static int fontHeight;

	//code
	CreateWindow();
	
	//Message Loop
	XEvent event;
	KeySym keysym;
	char keys[26];

	while(1)
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
						XFreeGC(gpDisplay, gc);
						XUnloadFont(gpDisplay, pxfontstruct->fid);
						uninitialize();
						exit(0);
				}
				
				switch(keys[0])
				{
					case 'F':
					case 'f':
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
				pxfontstruct = XLoadQueryFont(gpDisplay, "fixed");
				break;
			case Expose:
				printf("ERROR : Print 1.\n");
				gc = XCreateGC(gpDisplay, gWindow, 0, &gcValues);
				printf("ERROR : Print 2 .\n");
				XSetFont(gpDisplay, gc, pxfontstruct->fid);
				

				XAllocNamedColor(gpDisplay, gColormap, "green", &text_color, &text_color);
				
				XSetForeground(gpDisplay, gc, text_color.pixel);
				strlength = strlen(str);
				
				strWidth=XTextWidth(pxfontstruct, str, strlength);
				
				fontHeight = pxfontstruct->ascent+pxfontstruct->descent;
				
				XDrawString(gpDisplay, gWindow, gc, ((winWidth/2) - (strWidth/2)), ((winHeight/2)-(fontHeight/2)), str, strlength);
				
				break;
			case DestroyNotify:
				break;
			case 33:
				XFreeGC(gpDisplay, gc);
				XUnloadFont(gpDisplay, pxfontstruct->fid);
				uninitialize();
				exit(0);
			default:
				break;
		}
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

	gpXVisualInfo=(XVisualInfo*)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR : Unable To Allocate Memory For Visual Info.\n Exitting Now ...\n");
		uninitialize();
		exit(1);
	}

	XMatchVisualInfo(gpDisplay, defaultScreen,defaultDepth, TrueColor, gpXVisualInfo);
	if(gpXVisualInfo == NULL)
	{
		printf("ERROR : Unable To Get A Visual.\n Exitting Now ... \n");
		uninitialize();
		exit(1);
	}

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap=XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask= ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow=XCreateWindow(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight, 0,gpXVisualInfo->depth, InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);

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

 
					
