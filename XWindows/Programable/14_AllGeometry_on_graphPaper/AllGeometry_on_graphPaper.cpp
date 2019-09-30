#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

#include<SOIL/SOIL.h>

#include "vmath.h"


#define CHECKIMAGEWIDTH   64
#define CHECKIMAGEHEIGHT  64
// namespaces
using namespace std;
using namespace vmath;

//Global variable
bool bFullscreen=false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight=600;


enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display* , GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext;


// Shaders Varible
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint glShaderProgramObject;

GLuint vao;
GLuint vbo_position, vbo_color;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;  //this is from Vmath

//entry-point function
int main(void)
{
	//function prototype
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void unintialize(void);
	void update(void);

	void resize(int, int);
	void display(void);
	void intialize(void);
	
	//variable decllaration
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	bool bDone = false;
	//code
	printf("\n Calling Create Window.");
	CreateWindow();
	printf("\n Completion of create Window");
	printf("\n Calling Intialization.");	
	intialize();
	printf("\n Completion of Intialization");
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
	unintialize();
	return(0);


}

void CreateWindow(void)
{
	printf("\nInside Creat Window");
	//function prorttypes
	void unintialize(void);

	//variable declaring
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	GLXFBConfig *pGLXFBConfig = NULL; // Array of FBConfig
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo = NULL;
	int iNumberOfFBConfigs = 0;

	int bestFrameBufferConfig = -1;
	int bestNumberofSample = -1;
	int worstFrameBufferConfig = -1;
	int worstNumberofSample = 999;

	//OpenGL Variable declaration 
	static int frameBufferAttribute[]={GLX_X_RENDERABLE, True,
					   GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
					   GLX_RENDER_TYPE, GLX_RGBA_BIT,
					   GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
					   GLX_DOUBLEBUFFER, True,
					   GLX_RED_SIZE, 8,
					   GLX_GREEN_SIZE, 8,
					   GLX_BLUE_SIZE, 8,
					   GLX_ALPHA_SIZE, 8,
					   GLX_DEPTH_SIZE,24,
					   GLX_STENCIL_SIZE, 8,
					   None};
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Open X Display.\nExitting Now ...\n");
		unintialize();
		exit(1);
	}

	defaultScreen=XDefaultScreen(gpDisplay);
	defaultDepth=DefaultDepth(gpDisplay, defaultScreen);


	pGLXFBConfig = glXChooseFBConfig(gpDisplay,defaultScreen, frameBufferAttribute, &iNumberOfFBConfigs);

	printf("%d matching FBConfig ", iNumberOfFBConfigs);

	for(int i=0; i<iNumberOfFBConfigs; i++)
	{
		//See for each obtain frame buffer config get temporary Visual info
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
		
		//It is use to check the capability of calling following 2 call
		if(pTempXVisualInfo){
			//GetNumber of Sample Buffer from Respective FBConfig
			int sampleBuffer, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffer);

			// GetNumber of samples from FBConfig
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);

			//more the number of Samples & sample Buffer
			//more the eligible FBConfig is so Do the comparision
			if (bestFrameBufferConfig < 0 || sampleBuffer && samples > bestNumberofSample){
				bestFrameBufferConfig = i;
				bestNumberofSample = samples;
			}
			
			if(worstFrameBufferConfig < 0 || !sampleBuffer || samples < worstNumberofSample){
				worstFrameBufferConfig =i;
				worstNumberofSample = samples;
			}
		}
		XFree(pTempXVisualInfo);
	}

	printf("\n Completion of for Loop in Create Window.");
	
	// Now Assign the found Best one 
	bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
	
	//Now assign the same best to global onr
	gGLXFBConfig = bestGLXFBConfig;

	//free the obtain GLXFBConfig array
	XFree(pGLXFBConfig);

	//Accordingly Now get the Best Visual
	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	
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
		unintialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "GraphPaper XWindow");

	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
	printf("\n Exiting Create Window.");
}



void intialize(void)
{
	printf("\nInside Intialize");
	//function Declaration
	void unintialize(void);
	void resize(int, int);

	
	// Code

	GLenum result;
	


	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");

	if(glXCreateContextAttribsARB == NULL)
	{
		printf("Not Getting GLX !!!");
		unintialize();
		exit(0);
	}

	const int Attrib[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
				GLX_CONTEXT_MINOR_VERSION_ARB, 5,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
				None};

	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, Attrib);


 	if(!gGLXContext){

		//If not obtain the Highest one Specify the lowest one 
		// It will give the highest one known by him

		const int AnotherAttrib[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
						GLX_CONTEXT_MINOR_VERSION_ARB, 0,
						None};

		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, true, AnotherAttrib);
	}

	//Check whether obtained contex is really H/W rendering or not
	if(!glXIsDirect(gpDisplay, gGLXContext))
	{
		printf("The obtain rendering cntext is not H/W Rendering Context ! ");
	}
	else{
		printf("The obtain rendering cntext is H/W Rendering Context ! ");
	}

	
	//Make the obtain Context as Current OpenGL context
	glXMakeCurrent(gpDisplay,gWindow, gGLXContext);

	result = glewInit();

	if (result != GLEW_OK)
	{
		
		unintialize();
		//DestroyWindow();
	}

	

	//Define VertexShader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Specify/Write vertex shader code

	const GLchar *VertexShaderSourseCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;" \
		"}";

	//"gl_Position=vec4(0,0,0,0);" \

	

	//Specify above code to Vertex Shader object
	glShaderSource(gVertexShaderObject, 1, (GLchar const**)&VertexShaderSourseCode, NULL);

	GLint iShaderCompileStatus = 0;
	GLint iInfologLength = 0;
	GLchar *szInfoLog = NULL;

	
	//Compile the Vertex shader
	glCompileShader(gVertexShaderObject);
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	


	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfologLength, &written, szInfoLog);

				
				free(szInfoLog);
				unintialize();
				
				exit(0);

			}
		}
	}





	//Define FragmentShader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Specify/Write fragment shader code

	const GLchar *FragmentShaderSourseCode =
		"#version 450 core" \
		"\n"
		"out vec4 fragColor;" \
		"in vec4 out_color;" \
		"void main(void)" \
		"{" \
		"fragColor = out_color;" \
		"}";
	//Specify above code to Fragment Shader object
	glShaderSource(gFragmentShaderObject, 1, (GLchar const**)&FragmentShaderSourseCode, NULL);

	iShaderCompileStatus = 0;
	iInfologLength = 0;


	//Compile the Fragment shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfologLength, &written, szInfoLog);

				
				free(szInfoLog);
				unintialize();
				
				exit(0);

			}
		}
	}

	// Create Shader Program Object
	glShaderProgramObject = glCreateProgram();

	//Attach Vertex Shader to the Shader Program
	glAttachShader(glShaderProgramObject, gVertexShaderObject);

	//Attach Fragnebt Shader
	glAttachShader(glShaderProgramObject, gFragmentShaderObject);

	// Prelinking binding to Vertex attribute
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");

	//Link the Shader Program
	glLinkProgram(glShaderProgramObject);

	GLint iProgramLinkStatus = 0;
	iInfologLength = 0;
	//ErrorCheking
	glGetProgramiv(glShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);

	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(glShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(glShaderProgramObject, iInfologLength, &written, szInfoLog);


				
				free(szInfoLog);
				unintialize();
				
				exit(0);
			}
		}
	}

	//Post linking retriving/getting uniform location 
	mvpUniform = glGetUniformLocation(glShaderProgramObject, "u_mvp_matrix");

	//Ortho fixfunction program
	/*const GLfloat triangleVertices[] = { 0.0f,1.0f,0.0f,
										0.0f,-1.0f,0.0f };*/
										
	/*{ 0.0f,1.0f,0.0f,
										-1.0f,-1.0f,0.0f,
										1.0f,-1.0f,0.0f };*/

	//Create voa/,
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);


	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);





	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	

	perspectiveProjectionMatrix = mat4::identity();
	
	resize(giWindowWidth, giWindowHeight);
}





void resize(int width, int height){

	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix= perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	
}

void update (void)
{
	

}

void display(void){

	//code
	float x_coordinate = -1.05f, y_coordinate = -1.05f;
	int index;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary transformation

	//Do necessary Matrix multiplication
	modelViewMatrix = translate(0.0f, 0.0f, -2.5f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	//This waas done internally by gluaurtho in FFP

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao);

	//Similarly Bind with any texture
	GLfloat triangleVertices[] = { 0.0f,1.0f,0.0f,
										0.0f,-1.0f,0.0f };

	GLfloat linecolor[6] = { 0.0f,1.0f,0.0f,
										0.0f,1.0f,0.0f };

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 2);

	triangleVertices[0] = -1.0f;
	triangleVertices[1] = 0.0f;
	triangleVertices[2] = 0.0f;
	triangleVertices[3] = 1.0f;
	triangleVertices[4] = 0.0f;
	triangleVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 2);
	

	linecolor[0] = 0.0f;
	linecolor[1] = 0.3f;
	linecolor[2] = 0.0f;
	linecolor[3] = 0.0f;
	linecolor[4] = 0.3f;
	linecolor[5] = 0.0f;

	for (index = 0; index <= 40; index++)
	{
		x_coordinate = x_coordinate + 0.05f;
		y_coordinate = y_coordinate + 0.05f;

		if (int(x_coordinate * 100) == 0)
		{

			continue;
		}
		else {
			triangleVertices[0] = x_coordinate;
			triangleVertices[1] = -1.0f;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = x_coordinate;
			triangleVertices[4] = 1.0f;
			triangleVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, 0, 2);

		

			triangleVertices[0] = -1.0f;
			triangleVertices[1] = y_coordinate;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = 1.0f;
			triangleVertices[4] = y_coordinate;
			triangleVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, 0, 2);


			
		}

	}
	//Draw triangle by line
	GLfloat linec[9];
	linec[0] = 1.0f;
	linec[1] = 1.0f;
	linec[2] = 1.0f;
	linec[3] = 1.0f;
	linec[4] = 1.0f;
	linec[5] = 1.0f;
	linec[6] = 1.0f;
	linec[7] = 1.0f;
	linec[8] = 1.0f;

	GLfloat lineVertices[9]; 
	lineVertices[0] = 0.0f;
	lineVertices[1] = 1.0;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -sqrt(0.75f);
	lineVertices[4] = -0.5f;
	lineVertices[5] = 0.0f;
	lineVertices[6] = sqrt(0.75f);
	lineVertices[7] = -0.5f;
	lineVertices[8] = 0.0f;
	

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linec), linec, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 3);
	
	int  numpoints = 1000;

	GLfloat circlevertex[6];
	GLfloat circlcolor[6] = { 1,1,1,1,1,1 };
	float radius = 0.5f;
	float angle = 0.0f;
	int i = 0, j = 1, k = 2;
	//Inner circle
	for (index = 0; index < numpoints; index++)
	{
		angle = (2 * M_PI *index) / numpoints;
		circlevertex[0] = (radius * cos(angle));
		circlevertex[1] = (radius * sin(angle));
		circlevertex[2] = 0;


		angle = (2 * M_PI * (index+1)) / numpoints;
		circlevertex[3] = (radius * cos(angle));
		circlevertex[4] = (radius * sin(angle));
		circlevertex[5] = 0;
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlevertex), circlevertex, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlcolor), circlcolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);

	}
	
	//Outer Circle
	for (index = 0; index < numpoints; index++)
	{
		angle = (2 * M_PI *index) / numpoints;
		circlevertex[0] =  cos(angle);
		circlevertex[1] =  sin(angle);
		circlevertex[2] = 0;


		angle = (2 * M_PI * (index + 1)) / numpoints;
		circlevertex[3] =  cos(angle);
		circlevertex[4] =  sin(angle);
		circlevertex[5] = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlevertex), circlevertex, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlcolor), circlcolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);

	}

	//Square by line
	
	GLfloat reccolor[12] = { 1,1,1,1,1,1,1,1,1,1,1,1 };
	

	GLfloat rectvertex[12];
	rectvertex[0] = -sqrt(0.75f);
	rectvertex[1] = 0.5;
	rectvertex[2] = 0.0f;
	rectvertex[3] = -sqrt(0.75f);
	rectvertex[4] = -0.5f;
	rectvertex[5] = 0.0f;
	rectvertex[6] = sqrt(0.75f);
	rectvertex[7] = -0.5f;
	rectvertex[8] = 0.0f;
	rectvertex[9] = sqrt(0.75f);
	rectvertex[10] = 0.5;
	rectvertex[11] = 0.0f;


	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectvertex), rectvertex, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(reccolor), reccolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 4);



	//unbind vao
	glBindVertexArray(0);

	//unusal program

	glUseProgram(0);


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

void unintialize(void)
{

	GLXContext currentGLXContext = glXGetCurrentContext();
	
	if (vbo_position) {
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vao) {
		glDeleteBuffers(1, &vao);
		vao = 0;
	}


	//safe release shader
	if (glShaderProgramObject) {
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(glShaderProgramObject);
		//ask program How many shader attached with you

		glGetProgramiv(glShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShader = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if (pShader) {
			glGetAttachedShaders(glShaderProgramObject, shaderCount, &shaderCount, pShader);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//Detached one by one
				glDetachShader(glShaderProgramObject, pShader[shaderNumber]);
				//delete detach shader
				glDeleteShader(pShader[shaderNumber]);
				pShader[shaderNumber] = 0;
				
			}
			free(pShader);
		}
		glDeleteProgram(glShaderProgramObject);
		glShaderProgramObject = 0;
		glUseProgram(0);
	}
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
