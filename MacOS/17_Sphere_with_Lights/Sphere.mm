

// Headers
#import <Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>
#import<QuartzCore/CVDisplayLink.h>
#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>
#import "vmath.h"
#import <vector>

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
							
	[window setTitle:@"macOS OpenGL Sphere with Light Bhushan's Window"];
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
    
    bool bLight_BRK ;
	GLuint mvUniform_BRK, pUniform_BRK, ldUniform_BRK,kdUniform_BRK, lightPositionUniform_BRK, LKeyPress_BRK;
    
    
	vmath::mat4 perspectiveProjectioMatrix;
    
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

float length(float* normalize)
{
    return sqrt(normalize[0]*normalize[0]+normalize[1]*normalize[1]+normalize[2]*normalize[2]);
}

float* calculateNormal(const float* v1, const float* v2)
{
    float cross[3] ={v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]};
    return &cross[0];
    
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
    "uniform mat4 u_mv_matrix;" \
    "uniform mat4 u_projection_matrix;" \
    "uniform int u_Lkeypress;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_kd;" \
    "uniform vec4 u_light_position;" \
    "out vec3 diffusecolor;" \
    "void main(void)" \
    "{" \
    "if(u_Lkeypress==1)" \
    "{" \
    "vec4 eye_coordinate = u_mv_matrix * vPosition;" \
    "mat3 normalMatrix = mat3(transpose(inverse(u_mv_matrix)));"\
    "vec3 Tnormal=normalize(normalMatrix * vNormal);" \
    "vec3 S =normalize((vec3(u_light_position) - eye_coordinate.xyz));" \
    "diffusecolor = u_ld * u_kd * dot(S,Tnormal);" \
    "}" \
    "gl_Position = u_projection_matrix * u_mv_matrix * vPosition;" \
    
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
    "in vec3 diffusecolor;" \
    "uniform int u_Lkeypress;" \
    "void main(void)" \
    "{" \
    "if(u_Lkeypress==1)" \
    "{"
    "fragColor = vec4(diffusecolor,1.0);" \
    "}"
    "else"
    "{"
    "fragColor = vec4(1.0f,1.0f,1.0f,1.0f);" \
    "}"
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
    mvUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_mv_matrix");
    pUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_projection_matrix");
    ldUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_ld");
    kdUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_kd");
    lightPositionUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_light_position");
    LKeyPress_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_Lkeypress");
    
	// *** vertices, colors, shader attribs, vbo_Sphere_Vertex_BRK, vao_Sphere_BRK intiallize ****
	
    
    
    std::vector<float>circlevertex;
    //GLfloat circlcolor[9] = { 1,1,1,1,1,1,1,1,1 };
    std::vector<float>circleColor;
    std::vector<float>circleNormal;
    //GLfloat *circleColor = NULL;
    float radius = 1.0f;
    float angle = 0.0f;
    float phyinc = 0.1;
    float thetainc = 0.1;
    gNumOfVertices = 0;
    
    float theta = 0.5f;
    
   for(theta = 0.0f; theta <= 180.0f; theta += thetainc)
   {
        for (float phy = 0.0f; phy <= 360.0f; phy += phyinc)
        {
            float vec1[3], vec2[3], vec3[3];
            float* normal;
            //angle = (2 * M_PI *index) / numpoints;
            float x = (radius * sin(theta) * cos(phy));
            float y = (radius * sin(theta) * sin(phy));
            float z = (radius * cos(theta));
            
            vec1[0] = x;
            vec1[1] = y;
            vec1[2] = z;
            
            circlevertex.push_back(x);
            circlevertex.push_back(y);
            circlevertex.push_back(z);
            
            circleColor.push_back(0.2);
            circleColor.push_back(0.0);
            circleColor.push_back(0.0);
            
            gNumOfVertices++;
            
            x = (radius * sin(theta+(thetainc/2)) * cos(phy-(phyinc/2)));
            y = (radius * sin(theta+(thetainc/2)) * sin(phy-(phyinc/2)));
            z = (radius * cos(theta+(thetainc/2)));
            
            vec2[0] = x;
            vec2[1] = y;
            vec2[2] = z;
            
            circlevertex.push_back(x);
            circlevertex.push_back(y);
            circlevertex.push_back(z);
            
            circleColor.push_back(0.2);
            circleColor.push_back(0.0);
            circleColor.push_back(0.0);
            
            gNumOfVertices++;
            
            x = (radius * sin(theta+(thetainc/2)) * cos(phy+(phyinc/2)));
            y = (radius * sin(theta+(thetainc/2)) * sin(phy+(phyinc/2)));
            z = (radius * cos(theta+(thetainc/2)));
            
            vec3[0] = x;
            vec3[1] = y;
            vec3[2] = z;
            
            circlevertex.push_back(x);
            circlevertex.push_back(y);
            circlevertex.push_back(z);
            
            circleColor.push_back(0.2);
            circleColor.push_back(0.0);
            circleColor.push_back(0.0);
            
            gNumOfVertices++;
            float v1[3],v2[3];
            
            v1[0] = vec2[0]-vec1[0];
            v1[1] = vec2[1]-vec1[1];
            v1[2] = vec2[2]-vec1[2];
            
            
            v2[0] = vec3[0]-vec2[0];
            v2[1] = vec3[1]-vec2[1];
            v2[2] = vec3[2]-vec2[2];
            
            normal = calculateNormal(&v1[0],&v2[0]);
            
            circleNormal.push_back(-normal[0]);
            circleNormal.push_back(-normal[1]);
            circleNormal.push_back(-normal[2]);
            
            circleNormal.push_back(normal[0]);
            circleNormal.push_back(normal[1]);
            circleNormal.push_back(normal[2]);
            
            circleNormal.push_back(normal[0]);
            circleNormal.push_back(normal[1]);
            circleNormal.push_back(normal[2]);
        }
   }
	
	glGenVertexArrays(1,&vao_Sphere_BRK);
	glBindVertexArray(vao_Sphere_BRK);
	
	glGenBuffers(1, &vbo_Sphere_Vertex_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Vertex_BRK);
	glBufferData(GL_ARRAY_BUFFER, gNumOfVertices * sizeof(float), circlevertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	
	
	/*glGenBuffers(1, &vboLine_color_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vboLine_color_BRK);
	glBufferData(GL_ARRAY_BUFFER, gNumOfVertices * sizeof(float), circleColor.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

    glGenBuffers(1, &vboLine_Sphere_normal_BRK);
    glBindBuffer(GL_ARRAY_BUFFER, vboLine_Sphere_normal_BRK);
    glBufferData(GL_ARRAY_BUFFER, gNumOfVertices * sizeof(float), circlevertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
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
	perspectiveProjectioMatrix = vmath::mat4::identity();
	
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
	
    perspectiveProjectioMatrix=vmath::perspective(45.0f,(float)width/(float)height,0.1f,100.0f);
	
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
	
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
    modelViewMatrix = vmath::translate(0.0f,0.0f,-6.0f);
    modelViewMatrix = modelViewMatrix * vmath::rotate(angle_rotate, angle_rotate, angle_rotate);
	
	modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvUniform_BRK, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(pUniform_BRK, 1, GL_FALSE, perspectiveProjectioMatrix);
    
    
    if (bLight_BRK == TRUE)
    {
        glUniform1i(LKeyPress_BRK, 1);
        glUniform3f(ldUniform_BRK, 1.0f, 1.0f, 1.0f); // White color
        glUniform3f(kdUniform_BRK, 0.5f, 0.5f, 0.5f); // material property gray
        glUniform4f(lightPositionUniform_BRK, 0.0f, 0.0f, -1.0f,1.0f);
    }
    else
    {
        glUniform1i(LKeyPress_BRK, 0);
    }
	//****bind vao_Sphere_BRK ***
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices);
	glBindVertexArray(0);
		
	glUseProgram(0);
    
    if(angle_rotate<360)
        angle_rotate = angle_rotate + 2.5f;
    else
        angle_rotate = 0.0f;
	
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


	
	

