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

GLuint vao_pyramid;
GLuint vbo_position_pyramid, vbo_normal_pyramid;
GLuint modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
GLuint red_ld_Uniform, red_la_Uniform, red_ls_Uniform, red_lightPosition_Uniform;
GLuint blue_ld_Uniform, blue_la_Uniform, blue_ls_Uniform, blue_lightPosition_Uniform;
GLuint ka_Uniform, kd_Uniform, ks_Uniform;
GLuint LKeyPress_Uniform, materialShinyness_Uniform;

mat4 perspectiveProjectionMatrix;  //this is from Vmath


float RedlightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float RedlightDiffuse[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightSpecular[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightPosition[4] = { -2.0f,0.0f,0.0f,1.0f };

float BluelightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float BluelightDiffuse[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightSpecular[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightPosition[4] = { 2.0f,0.0f,0.0f,1.0f };


float materialAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float materialDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialShinyness = 128.0f;




GLfloat angle_pyramid = 0.0f;
bool bLight = false;
bool animation = false;


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
						case 'L':
						case 'l':
							if (bLight == false)
							{
								bLight = true;
							}
							else {
								bLight = false;
							}
							break;

						case 'A':
						case 'a':
							if (animation == false)
							{
								//Do nothing;
								animation = true;
							}
							else {
								animation = false;
								
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
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_Lkeypress;" \
		"uniform vec3 u_red_la;" \
		"uniform vec3 u_red_ld;" \
		"uniform vec3 u_red_ls;" \
		"uniform vec4 u_red_lightPosition;" \
		"uniform vec3 u_blue_la;" \
		"uniform vec3 u_blue_ld;" \
		"uniform vec3 u_blue_ls;" \
		"uniform vec4 u_blue_lightPosition;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShinyness;" \
		"out vec3 phong_ads_redlight;" \
		"out vec3 phong_ads_bluelight;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 Transformednormal=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 redlightDirection =normalize(vec3(u_red_lightPosition - eye_coordinate));" \
		"vec3 bluelightDirection =normalize(vec3(u_blue_lightPosition - eye_coordinate));" \
		"float TN_dot_red_ld = max(dot(redlightDirection,Transformednormal),0.0f);" \
		"float TN_dot_blue_ld = max(dot(bluelightDirection,Transformednormal),0.0f);" \
		"vec3 red_reflection_vector = reflect(-redlightDirection,Transformednormal);" \
		"vec3 blue_reflection_vector = reflect(-bluelightDirection,Transformednormal);" \
		"vec3 viewer_vector = normalize(vec3(-eye_coordinate.xyz));" \
		"vec3 redambient = u_red_la * u_ka;" \
		"vec3 reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" \
		"vec3 redspecular = u_red_ls * u_ks * pow(max(dot(red_reflection_vector , viewer_vector ),0.0f),u_materialShinyness);" \
		"phong_ads_redlight = redambient + reddiffuse + redspecular;" \
		"vec3 blueambient = u_blue_la * u_ka;" \
		"vec3 bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" \
		"vec3 bluespecular = u_blue_ls * u_ks * pow(max(dot(blue_reflection_vector , viewer_vector ),0.0f),u_materialShinyness);" \
		"phong_ads_bluelight = blueambient + bluediffuse + bluespecular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_redlight = vec3(1.0f,1.0f,1.0f);" \
		"phong_ads_bluelight = vec3(1.0f,1.0f,1.0f);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";




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
		"out vec4 redfragColor;" \
		"out vec4 bluefragColor;" \
		"in vec3 phong_ads_redlight;" \
		"in vec3 phong_ads_bluelight;" \
		"void main(void)" \
		"{" \
		"bluefragColor = vec4(phong_ads_bluelight ,1.0);" \
		"redfragColor = vec4(phong_ads_redlight,1.0);" \
		"redfragColor = redfragColor + bluefragColor;"\
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
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");

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
	modelMatrix_Uniform = glGetUniformLocation(glShaderProgramObject, "u_model_matrix");
	viewMatrix_Uniform = glGetUniformLocation(glShaderProgramObject, "u_view_matrix");
	projection_Uniform = glGetUniformLocation(glShaderProgramObject, "u_projection_matrix");

	red_la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_la");
	red_ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_ld");
	red_ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_ls");
	red_lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_lightPosition");

	blue_la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_la");
	blue_ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_ld");
	blue_ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_ls");
	blue_lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_lightPosition");

	ka_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ka");
	kd_Uniform = glGetUniformLocation(glShaderProgramObject, "u_kd");
	ks_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ks");
	materialShinyness_Uniform = glGetUniformLocation(glShaderProgramObject, "u_materialShinyness");
	LKeyPress_Uniform = glGetUniformLocation(glShaderProgramObject, "u_Lkeypress");

	//Ortho fixfunction program



	const GLfloat pyramidVertices[] = { 0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,

										0.0f, 1.0f, 0.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,


										0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, -1.0f,
										1.0f, -1.0f, -1.0f,

										0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										-1.0f, -1.0f, -1.0f };

	const GLfloat pyramidNormals[] = { 0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
										
										0.894427, 0.447214, 0.0,
										0.894427, 0.447214, 0.0,
										0.894427, 0.447214, 0.0,
										
										0.0, 0.447214, -0.894427,
										0.0, 0.447214, -0.894427,
										0.0, 0.447214, -0.894427,
										

										-0.894427, 0.447214, 0.0,
										-0.894427, 0.447214, 0.0,
										-0.894427, 0.447214, 0.0
										 };



	//Create Rectangle vao
	glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);

	glGenBuffers(1, &vbo_position_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);

	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);





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
	if (angle_pyramid > 360)
	{
		angle_pyramid = 0.0f;
		
	}
	else {
		angle_pyramid = angle_pyramid + 0.05f;
		
	}
}

void display(void){

	//code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 translateMatrix;
	mat4 rotationMatrix;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrix = rotate(0.0f, angle_pyramid, 0.0f);
	//rotationMatrix = rotate(angle_cube, 1.0f, 0.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	/*rotationMatrix = rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	translateMatrix = translateMatrix * rotationMatrix;*/

	modelMatrix = modelMatrix * translateMatrix;


	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == true)
	{
		glUniform1i(LKeyPress_Uniform, 1);
		glUniform3fv(red_la_Uniform, 1, RedlightAmbient);
		glUniform3fv(red_ld_Uniform, 1, RedlightDiffuse);
		glUniform3fv(red_ls_Uniform, 1, RedlightSpecular);
		glUniform4fv(red_lightPosition_Uniform, 1, RedlightPosition);

		glUniform3fv(blue_la_Uniform, 1, BluelightAmbient);
		glUniform3fv(blue_ld_Uniform, 1, BluelightDiffuse);
		glUniform3fv(blue_ls_Uniform, 1, BluelightSpecular);
		glUniform4fv(blue_lightPosition_Uniform, 1, BluelightPosition);

		glUniform3fv(ka_Uniform, 1, materialAmbient);
		glUniform3fv(kd_Uniform, 1, materialDiffuse);
		glUniform3fv(ks_Uniform, 1, materialSpecular);
		glUniform1f(materialShinyness_Uniform, materialShinyness);


	}
	else
	{
		glUniform1i(LKeyPress_Uniform, 0);
	}
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao_pyramid);



	//Draw necessary seen
	glDrawArrays(GL_TRIANGLES, 0, 12);
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
	
	if (vbo_position_pyramid) {
		glDeleteBuffers(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}

	if (vbo_normal_pyramid) {
		glDeleteBuffers(1, &vbo_normal_pyramid);
		vbo_normal_pyramid = 0;
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
