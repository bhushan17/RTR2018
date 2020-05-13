

// Headers
#define UNICODE
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

float lightAmbient[4] = {0.0f,0.0f,0.0f,0.0f};
float lightDiffuse[4] = {1.0f,1.0f,1.0f,1.0f};
float lightSpecular[4] = {1.0f,1.0f,1.0f,1.0f};
float lightPosition[4] = {0.0f,0.0f,0.0f,1.0f};

float materialAmbient[4] = {0.0f,0.0f,0.0f,0.0f};
float materialDiffuse[4] = {1.0f,1.0f,1.0f,1.0f};
float materialSpecular[4] = {1.0f,1.0f,1.0f,1.0f};
float materialShinyness = 128.0f;

int gNumVertices;
int gNumElements;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLfloat sholder = 0.0f;
GLfloat elbo = 0.0f;


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
							
	[window setTitle:@"macOS OpenGL Sphere PerFragment Light Bhushan's Window"];
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
	GLuint ld_Uniform_BRK, la_Uniform_BRK, ls_Uniform_BRK, lightPosition_Uniform_BRK;
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
		"uniform int u_Lkeypress;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_lightPosition;" \
		"out vec3 Transformednormal;" \
		"out vec3 lightDirection;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" \
		"Transformednormal=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"lightDirection = vec3(u_lightPosition - eye_coordinate);" \
		"viewer_vector =vec3(-eye_coordinate.xyz);" \
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
		"uniform int u_Lkeypress;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShinyness;" \
		"vec3 phong_ads_light;" \
		"in vec3 Transformednormal;" \
		"in vec3 lightDirection;" \
		"in vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);"
		"vec3 NormalizelightDirection = normalize(lightDirection);"
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);"
		"float TN_dot_ld = max(dot(NormalizelightDirection,NormalizeTransformednormal),0.0f);" \
		"vec3 reflection_vector = reflect(-NormalizelightDirection,NormalizeTransformednormal);" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 diffuse = u_ld * u_kd * TN_dot_ld;" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
		"}" \
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

	la_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_la");
	ld_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_ld");
	ls_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_ls");
	lightPosition_Uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_lightPosition");

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices) , sphere_vertices, GL_STATIC_DRAW);
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
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

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
	vmath::mat4 scaleMatrix = vmath::mat4::identity();
	vmath::mat4 pushMatrix1 = vmath::mat4::identity();
	vmath::mat4 pushMatrix2 = vmath::mat4::identity();
    
    translateMatrix = vmath::translate(-1.0f,0.0f,-6.0f);
	modelMatrix = modelMatrix * translateMatrix;
	
	pushMatrix1 = modelMatrix;
	
    rotationMatrix =  vmath::rotate(sholder, 0.0f, 0.0f,1.0f);
	translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * rotationMatrix;
	
	pushMatrix2 = modelMatrix;
	
	scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
	
	modelMatrix = modelMatrix * translateMatrix * scaleMatrix;
	
	glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);
    
    
    
    if (bLight_BRK == TRUE)
    {
        glUniform1i(LKeyPress_Uniform_BRK, 1);
		glUniform3fv(la_Uniform_BRK, 1 , lightAmbient );
		glUniform3fv(ld_Uniform_BRK, 1, lightDiffuse);
		glUniform3fv(ls_Uniform_BRK, 1, lightSpecular);
		glUniform4fv(lightPosition_Uniform_BRK, 1, lightPosition);

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
    //glDrawArrays(GL_TRIANGLES, 0, 760);
	glBindVertexArray(0);
		
		
	modelMatrix = pushMatrix2;	
		
	

	translateMatrix = vmath::translate(2.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * translateMatrix;
	rotationMatrix = vmath::rotate(elbo, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix * rotationMatrix;
	
	translateMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * translateMatrix;
	//BRKStackPush(modelMatrix);
	scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);

	modelMatrix = modelMatrix  * scaleMatrix;


	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);
    

	
	//Draw necessary seen
	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindVertexArray(vao_Sphere_BRK);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element_BRK);
    glDrawElements(GL_TRIANGLES, gNumElements,GL_UNSIGNED_SHORT,0);
	//unbind vao
	glBindVertexArray(0);

	
		
		
		
	glUseProgram(0);
    
    /*
    if(angle_rotate<360)
        angle_rotate = angle_rotate + 2.5f;
    else
        angle_rotate = 0.0f;
	*/
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
		case 'S' :
			sholder = sholder + 1.0;
			break;
		case 's':
			sholder = sholder - 1.0;
			break;
		case 'E' :
			elbo = elbo + 1.0;
			break;
		case 'e' :
			elbo = elbo - 1.0;
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


	
	

