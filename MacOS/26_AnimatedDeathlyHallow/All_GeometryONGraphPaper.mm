

// Headers
#import <Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>
#import<QuartzCore/CVDisplayLink.h>
#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>
#import "vmath.h"

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
							
	[window setTitle:@"macOS OpenGL All Geometry On Graph Paper Bhushan's Window"];
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
	
	GLuint vaoLine_BRK;
	GLuint vboLine_Position_BRK;
	
	
	GLuint vboLine_color_BRK;
	GLuint mvpUniform;
	
	vmath::mat4 perspectiveProjectioMatrix;
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
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;" \
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
		"in vec4 out_color;" \
		"void main(void)" \
		"{" \
		"fragColor = out_color;" \
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
	mvpUniform = glGetUniformLocation(shaderProgramObject_BRK, "u_mvp_matrix");
	
	// *** vertices, colors, shader attribs, vboLine_Position_BRK, vaoLine_BRK intiallize ****
	
	
										
	
										
	
	glGenVertexArrays(1,&vaoLine_BRK);
	glBindVertexArray(vaoLine_BRK);
	
	glGenBuffers(1, &vboLine_Position_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vboLine_Position_BRK);
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	
	
	glGenBuffers(1, &vboLine_color_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vboLine_color_BRK);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	
	
	
	
	
	
	
	glClearDepth(1.0f);
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	//We will always cull back faces for better performance
	glEnable(GL_CULL_FACE);
	
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
	
    perspectiveProjectioMatrix=vmath::perspective(45.0f,width/height,0.1f,100.0f);
	
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
	float x_coordinate = -1.05f, y_coordinate = -1.05f;
	int index;
    
    float incentry = (1 - sqrt(5.0f)) / (1 + sqrt(5.0f));
    static float lineycoordinate = 1.0f;
    static float triangletranslatex = 1.0f;
    static float triangletranslate = 0.0f;
    static float circletranslate = 0.0f;
    static float linetranslate = 0.0f;
    
    static float trianglerotationangle = 0.0f;
    static float circlerotationangle = 0.0f;
    
    int trianglestatus = 0;
    int circlestatus = 0;
    
    
	[[self openGLContext]makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
    
    //Start using OpenGL program object
       glUseProgram(shaderProgramObject_BRK);
       
       //OpenGL Drawing
       //set modelview & modelviewprojection matrices to identity
      vmath::mat4 modeltranslate = vmath::mat4::identity();
      vmath::mat4 modelrotate = vmath::mat4::identity();
       vmath::mat4 modelViewMatrix = vmath::mat4::identity();
       vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
    modeltranslate = vmath::translate(1-triangletranslate, -1+triangletranslate, -12 * triangletranslate);
    modelrotate = vmath::rotate(trianglerotationangle, 0.0f,1.0f,0.0f);
    
    modelViewMatrix = modeltranslate  * modelrotate;
       //multiply the modelview and orthographic matrix to get modelviewprojection matrix
       modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix; // ORDER IS Important
       
       //pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variables
       //whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
       glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
       
       //****bind vaoLine_BRK ***
       glBindVertexArray(vaoLine_BRK);
       
    
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

      GLfloat trianglelineVertices[9];
      trianglelineVertices[0] = 0.0f;
      trianglelineVertices[1] = 1.0;
      trianglelineVertices[2] = 0.0f;
      trianglelineVertices[3] = -sqrt(0.75f);
      trianglelineVertices[4] = -0.5f;
      trianglelineVertices[5] = 0.0f;
      trianglelineVertices[6] = sqrt(0.75f);
      trianglelineVertices[7] = -0.5f;
      trianglelineVertices[8] = 0.0f;
      

      glBindBuffer(GL_ARRAY_BUFFER, vboLine_Position_BRK);
      glBufferData(GL_ARRAY_BUFFER, sizeof(trianglelineVertices), trianglelineVertices, GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glBindBuffer(GL_ARRAY_BUFFER, vboLine_color_BRK);
      glBufferData(GL_ARRAY_BUFFER, sizeof(linec), linec, GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glLineWidth(2.0f);
      glDrawArrays(GL_LINE_LOOP, 0, 3);
    
      if(triangletranslate < 0.7)
      {
          triangletranslate = triangletranslate + 0.001f;
      }
      else if(triangletranslate < 1.0f)
      {
          triangletranslate = triangletranslate + 0.001f;
          trianglestatus = 1;
      }
      else{
          triangletranslate = 1.0f;
          trianglestatus = 1;
      }
    
    
      // Draw Circle
    
       if(trianglestatus == 1)
       {
           if(circletranslate < 0.7)
           {
               circletranslate = circletranslate + 0.001f;
           }
           else if(circletranslate < 1.0f)
           {
               circletranslate = circletranslate + 0.001f;
               circlestatus = 1;
           }
           else
           {
               circletranslate = 1.0f;
               circlestatus = 1;
           }
       
    
    
    
      modeltranslate = vmath::mat4::identity();
     modelrotate = vmath::mat4::identity();
     modelViewMatrix = vmath::mat4::identity();
     modelViewProjectionMatrix = vmath::mat4::identity();
    
    modeltranslate = vmath::translate(-1 +circletranslate, (incentry + 0.37f) * circletranslate, -12.0f * circletranslate);
    modelrotate = vmath::rotate(circlerotationangle, 0.0f,1.0f,0.0f);
    modelViewMatrix = modeltranslate  * modelrotate;
    modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix;
    
     glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    
    int  numpoints = 1000;

    GLfloat circlevertex[6];
    GLfloat circlcolor[6] = { 1,1,1,1,1,1 };
    float radius = 0.5f;
    float angle = 0.0f;
    
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
        
        glBindBuffer(GL_ARRAY_BUFFER, vboLine_Position_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circlevertex), circlevertex, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboLine_color_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circlcolor), circlcolor, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 2);

    }
    
       }
    
    //Draw line
    
    if(circlestatus)
    {
        if(linetranslate<1.0f)
        {
            linetranslate = linetranslate + 0.001f;
            lineycoordinate = lineycoordinate - 0.002f;
        }
        else{
            linetranslate = 1.0f;
            lineycoordinate = -1.0f;
        }
        
        
        modeltranslate = vmath::mat4::identity();
            modelrotate = vmath::mat4::identity();
            modelViewMatrix = vmath::mat4::identity();
            modelViewProjectionMatrix = vmath::mat4::identity();
           
           modeltranslate = vmath::translate(0.0f, 1 - linetranslate, -12.0f * linetranslate);
           
        modelViewMatrix = modeltranslate ;
           modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix;
           
            glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
        
        
        GLfloat lineVertices[6] = { 0.0f,1.0f,0.0f,
                                            0.0f,-0.5f,0.0f };

        GLfloat linecolor[6] = { 1.0f,1.0f,1.0f,
                                            1.0f,1.0f,1.0f };
                                            
        glBindBuffer(GL_ARRAY_BUFFER, vboLine_Position_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboLine_color_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 2);
        
        
    }
    
    if(trianglerotationangle > 360)
    {
        trianglerotationangle = 0.0f;
        circlerotationangle = 0.0f;
    }
    else{
        trianglerotationangle = trianglerotationangle + 2.0f;
        circlerotationangle = circlerotationangle + 2.0f;
        
    }
    
    
       
       
       
       
       //*** unbind vaoLine_BRK ***
       glBindVertexArray(0);
           
       //stop using OpenGL program object
       glUseProgram(0);
    
    
    
	
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
	//destroy vaoLine_BRK
	if(vaoLine_BRK)
	{
		glDeleteVertexArrays(1, &vaoLine_BRK);
		vaoLine_BRK = 0;
	}
	
	//destroy vboLine_Position_BRK
	if(vboLine_Position_BRK)
	{
		glDeleteBuffers(1,&vboLine_Position_BRK);
		vboLine_Position_BRK = 0;
	}
	
	if(vboLine_color_BRK)
	{
		glDeleteBuffers(1,&vboLine_color_BRK);
		vboLine_color_BRK = 0;
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


	
	

