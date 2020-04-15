

// Headers
#import <Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>
#import<QuartzCore/CVDisplayLink.h>
#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>
#import "vmath.h"
#include"Sphere.h"

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

// 'C' style global function declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

//Global variables
FILE *gpFile=NULL;

GLfloat LightAnglezero = 0.0f;
GLfloat LightAngleone = 0.0f;
GLfloat LightAngletwo = 0.0f;

float RedfirstRotation = 0.0f, RedsecondRotation = 0.0f;
float GreenfirstRotation = 0.0f, GreensecondRotation = 0.0f;
float BluefirstRotation = 0.0f, BluesecondRotation = 0.0f;

float RedlightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float RedlightDiffuse[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightSpecular[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };

float GreenlightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float GreenlightDiffuse[4] = { 0.0f,1.0f,0.0f,1.0f };
float GreenlightSpecular[4] = { 0.0f,1.0f,0.0f,1.0f };
float GreenlightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };

float BluelightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float BluelightDiffuse[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightSpecular[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };


float materialAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float materialDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialShinyness = 128.0f;

int gNumVertices;
int gNumElements;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
//interface declaration
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

//Entry-point function 
int main(int argc, const char * argv[])
{
	//code
	NSAutoreleasePool *pPool=[[NSAutoreleasePool alloc]init];

	NSApp=[NSApplication sharedApplication];

	[NSApp setDelegate:[[AppDelegate alloc]init]];

	[NSApp run];

	[pPool release];

	return(0);
}

//interface implementation
@implementation AppDelegate
{
@private
	NSWindow *window;
	GLView *glView;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	//code
	//log file
	NSBundle *mainBundle=[NSBundle mainBundle];
	NSString *appDirName=[mainBundle bundlePath];
	NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
	NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];
	
	const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
	
	gpFile=fopen(pszLogFileNameWithPath,"w");
	if(gpFile==NULL)
	{
		printf("Can Not create Log File.\nExitting ...\n");
		[self release];
		[NSApp terminate:self];
	}
	
	fprintf(gpFile, "Program Is Started Successfully\n");
	
	//window
	NSRect win_rect;
	win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
	
	//create simple window 
	window=[[NSWindow alloc] initWithContentRect:win_rect
				styleMask:NSWindowStyleMaskTitled |
				NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
				NSWindowStyleMaskResizable
							backing:NSBackingStoreBuffered
							defer:NO];
							
	[window setTitle:@"macOS 3Rotating Lights PerVertex on Sphere Bhushan's Window"];
	[window center];
	
	glView=[[GLView alloc]initWithFrame:win_rect];
	
	[window setContentView:glView];
	[window setDelegate:self];
	[window makeKeyAndOrderFront:self];
	
}

-(void)applicationWillTerminate:(NSNotification *)notification
{
	//code
	fprintf(gpFile, "Program Is Terminated Successfully\n");
	
	if(gpFile)
	{
		fclose(gpFile);
		gpFile=NULL;
	}
}

-(void)windowWillClose:(NSNotification *)notification
{
	//code
	[NSApp terminate:self];
	
}

-(void)dealloc
{
	//code
	[glView release];
	
	[window release];
	
	[super dealloc];
	
}
@end

@implementation GLView
{
@private
	CVDisplayLinkRef displayLink;
	
	GLuint vertexShaderObject_BRK;
	GLuint fragmentShaderObject_BRK;
	GLuint shaderProgramObject_BRK;
	
	GLuint vao_Sphere_BRK;
	GLuint vbo_Sphere_Vertex_BRK;
    GLuint vboLine_Sphere_normal_BRK;
	GLuint vbo_sphere_element_BRK;
    
    bool bLight_BRK ;
	
    
	GLuint modelMatrix_Uniform_BRK, viewMatrix_Uniform_BRK, projection_Uniform_BRK;
	GLuint red_ld_Uniform_BRK, red_la_Uniform_BRK, red_ls_Uniform_BRK, red_lightPosition_Uniform_BRK;
	GLuint green_ld_Uniform_BRK, green_la_Uniform_BRK, green_ls_Uniform_BRK, green_lightPosition_Uniform_BRK;
	GLuint blue_ld_Uniform_BRK, blue_la_Uniform_BRK, blue_ls_Uniform_BRK, blue_lightPosition_Uniform_BRK;
	GLuint ka_Uniform_BRK, kd_Uniform_BRK, ks_Uniform_BRK;
	GLuint LKeyPress_Uniform_BRK, materialShinyness_Uniform_BRK;
    
	vmath::mat4 perspectiveProjectionMatrix_BRK;
    
    GLuint gNumOfVertices;
}

-(id)initWithFrame:(NSRect)frame;
{
	//code
	self=[super initWithFrame:frame];
	
	if(self)
	{
		[[self window]setContentView:self];
		
		NSOpenGLPixelFormatAttribute attrs[]=
		{
			//Must specify the 4.1 Core Profile to use OpenGL 4.1
			NSOpenGLPFAOpenGLProfile,
			NSOpenGLProfileVersion4_1Core,
			// Specify the display ID to associate the GL context with (main display for now)
			NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
			NSOpenGLPFANoRecovery,
			NSOpenGLPFAAccelerated,
			NSOpenGLPFAColorSize, 24,
			NSOpenGLPFADepthSize, 24,
			NSOpenGLPFAAlphaSize,8,
			NSOpenGLPFADoubleBuffer,
		0}; //last 0 is must

	NSOpenGLPixelFormat *pixelFormat=[[[NSOpenGLPixelFormat alloc]
		initWithAttributes:attrs]autorelease];
		
	if(pixelFormat==nil)
	{
		fprintf(gpFile, "No Valid OpenGL Pixel Format Is Available Exitting ...");
		[self release];
		[NSApp terminate:self];
	}
	
	NSOpenGLContext *glContext=[[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil]autorelease];
	
	[self setPixelFormat:pixelFormat];
	
	[self setOpenGLContext:glContext]; //it automatically release the older context, if present, and sets the newer one

	}
	return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
	//code
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
	
	[self drawView];
	
	[pool release];
	
	return(kCVReturnSuccess);
	
}



-(void)prepareOpenGL
{
	//code
	//OpenGL Info
	
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	[[self openGLContext]makeCurrentContext];
	
	GLint swapInt=1;
	[[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	//*** VERTEX SHADER *****
	//create shader
	vertexShaderObject_BRK=glCreateShader(GL_VERTEX_SHADER);
	
	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
	"#version 410"\
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
		"uniform vec3 u_green_la;" \
		"uniform vec3 u_green_ld;" \
		"uniform vec3 u_green_ls;" \
		"uniform vec4 u_green_lightPosition;" \
		"uniform vec3 u_blue_la;" \
		"uniform vec3 u_blue_ld;" \
		"uniform vec3 u_blue_ls;" \
		"uniform vec4 u_blue_lightPosition;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShinyness;" \
		"out vec3 phong_ads_light;" \
		"out vec3 Red_phong_ads_light;" \
		"out vec3 Green_phong_ads_light;" \
		"out vec3 Blue_phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 Transformednormal=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 RedlightDirection =normalize(vec3(u_red_lightPosition - eye_coordinate));" \
		"vec3 GreenlightDirection =normalize(vec3(u_green_lightPosition - eye_coordinate));" \
		"vec3 BluelightDirection =normalize(vec3(u_blue_lightPosition - eye_coordinate));" \
		"float TN_dot_red_ld = max(dot(RedlightDirection,Transformednormal),0.0f);" \
		"float TN_dot_green_ld = max(dot(GreenlightDirection,Transformednormal),0.0f);" \
		"float TN_dot_blue_ld = max(dot(BluelightDirection,Transformednormal),0.0f);" \
		"vec3 Redreflection_vector = reflect(-RedlightDirection,Transformednormal);" \
		"vec3 Greenreflection_vector = reflect(-GreenlightDirection,Transformednormal);" \
		"vec3 Bluereflection_vector = reflect(-BluelightDirection,Transformednormal);" \
		"vec3 viewer_vector = normalize(vec3(-eye_coordinate.xyz));" \
		"vec3 Redambient = u_red_la * u_ka;" \
		"vec3 Reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" \
		"vec3 Redspecular = u_red_ls * u_ks * pow(max(dot(Redreflection_vector , viewer_vector ),0.0f),u_materialShinyness);" \
		"Red_phong_ads_light = Redambient + Reddiffuse + Redspecular;"\
		"vec3 Greenambient = u_green_la * u_ka;" \
		"vec3 Greendiffuse = u_green_ld * u_kd * TN_dot_green_ld;" \
		"vec3 Greenspecular = u_green_ls * u_ks * pow(max(dot(Greenreflection_vector , viewer_vector ),0.0f),u_materialShinyness);" \
		"Green_phong_ads_light = Greenambient + Greendiffuse + Greenspecular;"\
		"vec3 Blueambient = u_blue_la * u_ka;" \
		"vec3 Bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" \
		"vec3 Bluespecular = u_blue_ls * u_ks * pow(max(dot(Bluereflection_vector , viewer_vector ),0.0f),u_materialShinyness);" \
		"Blue_phong_ads_light = Blueambient + Bluediffuse + Bluespecular;"\
		"phong_ads_light = Red_phong_ads_light + Green_phong_ads_light + Blue_phong_ads_light;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";
	
	glShaderSource(vertexShaderObject_BRK, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	
	// compile shader
	glCompileShader(vertexShaderObject_BRK);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(vertexShaderObject_BRK, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_BRK, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if(szInfoLog!=NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject_BRK, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	// *** FRAGMENT SHADER ***
	//re-intialize
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	
	//create shader
	fragmentShaderObject_BRK = glCreateShader(GL_FRAGMENT_SHADER);
	
	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
	"#version 410" \
	"\n" \
    "out vec4 fragColor;" \
		"in vec3 phong_ads_light;" \
		"uniform int u_Lkeypress;" \
		"void main(void)" \
		"{" \
		"fragColor = vec4(phong_ads_light,1.0);" \
		"}";
		
		
	glShaderSource(fragmentShaderObject_BRK,1,(const GLchar **)&fragmentShaderSourceCode,NULL);
	
	//compile shader 
	glCompileShader(fragmentShaderObject_BRK);
	glGetShaderiv(fragmentShaderObject_BRK, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_BRK, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if(szInfoLog!= NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject_BRK, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile,"Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	// **** Shader Program *****
	// create
	shaderProgramObject_BRK = glCreateProgram();
	
	//attach vertex shader and fragment shader
	glAttachShader(shaderProgramObject_BRK, vertexShaderObject_BRK);
	
	//attach fragment shader
	glAttachShader(shaderProgramObject_BRK, fragmentShaderObject_BRK);
	
	//ore-link binding of shader program object with vertex shader position attribute
    glBindAttribLocation(shaderProgramObject_BRK, VDG_ATTRIBUTE_VERTEX, "vPosition");
    glBindAttribLocation(shaderProgramObject_BRK, VDG_ATTRIBUTE_NORMAL, "vNormal");
	
    
	//link shader
	glLinkProgram(shaderProgramObject_BRK);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(shaderProgramObject_BRK, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if(iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_BRK, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_BRK, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	// grt MVP uniform location
   
	modelMatrix_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_model_matrix");
	viewMatrix_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_view_matrix");
	projection_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_projection_matrix");

	red_la_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_red_la");
	red_ld_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_red_ld");
	red_ls_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_red_ls");
	red_lightPosition_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_red_lightPosition");

	green_la_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_green_la");
	green_ld_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_green_ld");
	green_ls_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_green_ls");
	green_lightPosition_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_green_lightPosition");


	blue_la_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_blue_la");
	blue_ld_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_blue_ld");
	blue_ls_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_blue_ls");
	blue_lightPosition_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_blue_lightPosition");

	ka_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_ka");
	kd_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_kd");
	ks_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_ks");
	materialShinyness_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_materialShinyness");
	LKeyPress_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_Lkeypress");

    
	// *** vertices, colors, shader attribs, vbo_Sphere_Vertex_BRK, vao_Sphere_BRK intiallize ****
	
    
     getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	
	glGenVertexArrays(1,&vao_Sphere_BRK);
	glBindVertexArray(vao_Sphere_BRK);
	
	glGenBuffers(1, &vbo_Sphere_Vertex_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Vertex_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	
	
	

    glGenBuffers(1, &vboLine_Sphere_normal_BRK);
    glBindBuffer(GL_ARRAY_BUFFER, vboLine_Sphere_normal_BRK);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	 glGenBuffers(1, &vbo_sphere_element_BRK);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_BRK);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);


	glBindVertexArray(0);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//We will always cull back faces for better performance
	//glEnable(GL_CULL_FACE);
	
	//set background colors
	glClearColor(0.0f,0.0f,0.0f,0.0f); // Blue
	
	//set projection matrix to identity matrix
	perspectiveProjectionMatrix_BRK = vmath::mat4::identity();
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
	CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	CVDisplayLinkStart(displayLink);
    
     bLight_BRK = false;
    [super prepareOpenGL];
}

-(void)reshape
{
	//code
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
	NSRect rect=[self bounds];
	
	GLfloat width=rect.size.width;
	GLfloat height=rect.size.height;
	
	if(height==0)
		height = 1;
	
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
	
    perspectiveProjectionMatrix_BRK=vmath::perspective(45.0f,(float)width/(float)height,0.1f,100.0f);
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    [super reshape];
}

-(void)drawRect:(NSRect)dirtyrect
{
	//code
	[self drawView];
}

-(void)drawView
{
	//code
	[[self openGLContext]makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
    // variable declarations
    static float angle_rotate = 0.0f;
    
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderProgramObject_BRK);
	
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 translateMatrix = vmath::mat4::identity();
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
    
    translateMatrix = vmath::translate(0.0f,0.0f,-2.0f);
   // rotationMatrix =  vmath::rotate(angle_rotate, angle_rotate, angle_rotate);
	
	translateMatrix = translateMatrix * rotationMatrix;
	modelMatrix = modelMatrix * translateMatrix;
	
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);
	
    
    
    
    if (bLight_BRK == TRUE)
    {
        glUniform1i(LKeyPress_Uniform_BRK, 1);
		glUniform3fv(red_la_Uniform_BRK, 1 , RedlightAmbient );
		glUniform3fv(red_ld_Uniform_BRK, 1, RedlightDiffuse);
		glUniform3fv(red_ls_Uniform_BRK, 1, RedlightSpecular);
		glUniform4fv(red_lightPosition_Uniform_BRK, 1, RedlightPosition);

		glUniform3fv(green_la_Uniform_BRK, 1, GreenlightAmbient);
		glUniform3fv(green_ld_Uniform_BRK, 1, GreenlightDiffuse);
		glUniform3fv(green_ls_Uniform_BRK, 1, GreenlightSpecular);
		glUniform4fv(green_lightPosition_Uniform_BRK, 1, GreenlightPosition);

		glUniform3fv(blue_la_Uniform_BRK, 1, BluelightAmbient);
		glUniform3fv(blue_ld_Uniform_BRK, 1, BluelightDiffuse);
		glUniform3fv(blue_ls_Uniform_BRK, 1, BluelightSpecular);
		glUniform4fv(blue_lightPosition_Uniform_BRK, 1, BluelightPosition);

		glUniform3fv(ka_Uniform_BRK, 1, materialAmbient);
		glUniform3fv(kd_Uniform_BRK, 1, materialDiffuse);
		glUniform3fv(ks_Uniform_BRK, 1, materialSpecular);
		glUniform1f(materialShinyness_Uniform_BRK, materialShinyness);
    }
    else
    {
        glUniform1i(LKeyPress_Uniform_BRK, 0);
    }
	//****bind vao_Sphere_BRK ***
    glBindVertexArray(vao_Sphere_BRK);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_BRK);
    glDrawElements(GL_TRIANGLES, gNumElements,GL_UNSIGNED_SHORT,0);
	glBindVertexArray(0);
		
	glUseProgram(0);
    
	RedlightPosition[0] = RedfirstRotation;
	RedlightPosition[2] = RedsecondRotation;

	GreenlightPosition[1] = GreenfirstRotation;
	GreenlightPosition[2] = GreensecondRotation;

	BluelightPosition[1] = BluefirstRotation;
	BluelightPosition[0] = BluesecondRotation;
	
	
	LightAnglezero = LightAnglezero + 0.02f;
	LightAngleone = LightAngleone - 0.02f;
	LightAngletwo = LightAngleone + 0.02f;
	RedfirstRotation = 10 * cos(LightAnglezero);
	RedsecondRotation = 10 * sin(LightAnglezero);
	GreenfirstRotation = 10 * cos(LightAngleone);
	GreensecondRotation = 10 * sin(LightAngleone);
	BluefirstRotation = 100 * cos(LightAngletwo);
	BluesecondRotation = 100 * sin(LightAngletwo);
	
	CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(BOOL)acceptsFirstResponder
{
	//code 
	[[self window]makeFirstResponder:self];
	return(YES);
}

-(void)keyDown:(NSEvent*)theEvent
{
	//code
	int key=(int)[[theEvent characters]characterAtIndex:0];
	switch(key)
	{
		case 27: //ESC key
			[self release];
			[NSApp terminate:self];
			break;
		case 'F':
		case 'f':
			[[self window ]toggleFullScreen:self];
			break;
        case 'L':
        case 'l':
            if (bLight_BRK == false)
            {
                bLight_BRK = true;
            }
            else {
                bLight_BRK = false;
            }
            break;
		default:
			break;
	}
}

-(void)mouseDown:(NSEvent*)theEvent
{
	//code
}

-(void)mouseDragged:(NSEvent*)theEvent
{
	//code
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
	//code
}

-(void)dealloc
{
	//code
	//destroy vao_Sphere_BRK
	if(vao_Sphere_BRK)
	{
		glDeleteVertexArrays(1, &vao_Sphere_BRK);
		vao_Sphere_BRK = 0;
	}
	
	//destroy vbo_Sphere_Vertex_BRK
	if(vbo_Sphere_Vertex_BRK)
	{
		glDeleteBuffers(1,&vbo_Sphere_Vertex_BRK);
		vbo_Sphere_Vertex_BRK = 0;
	}
	
	if(vboLine_Sphere_normal_BRK)
	{
		glDeleteBuffers(1,&vboLine_Sphere_normal_BRK);
		vboLine_Sphere_normal_BRK = 0;
	}
	
	
	
	
	//detach vertex shader from shader program object
	glDetachShader(shaderProgramObject_BRK, vertexShaderObject_BRK);
	//detach fragment shader from shader program object
	glDetachShader(shaderProgramObject_BRK, fragmentShaderObject_BRK);
	
	//delete vertex shader object
	glDeleteShader(vertexShaderObject_BRK);
	vertexShaderObject_BRK = 0;
	//delete fragment shader object
	glDeleteShader(fragmentShaderObject_BRK);
	fragmentShaderObject_BRK = 0;
	
	//delete shader program object
	glDeleteProgram(shaderProgramObject_BRK);
	shaderProgramObject_BRK = 0;
	
	CVDisplayLinkStop(displayLink);
	CVDisplayLinkRelease(displayLink);
	
	[super dealloc];
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *pNow, const CVTimeStamp *pOutputTime, CVOptionFlags flagsIn, CVOptionFlags *pFlagsOut, void *pDisplayLinkContext)
{
	CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
	return(result);
}


	
	

