

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
							
	[window setTitle:@"macOS OpenGL Bhushan's Smily Texture Window"];
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
	
	
	
	GLuint vaoRectangle_BRK;
	GLuint vbo_Rectangleposition_BRK;
	GLuint vbo_Smilytexture_BRK;
	GLuint mvpUniform_BRK;
	
	vmath::mat4 perspectiveProjectioMatrix_BRK;
	

	GLuint Smilytexture_BRK;
	GLuint pressdigit_BRK;
	GLuint texture_sampler_uniform_BRK;
	GLuint QuardColor_BRK;
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
		"in vec2 vTexquard;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_texquard;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texquard = vTexquard;" \
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
		"in vec2 out_texquard;" \
		"uniform sampler2D u_sampler;" \
		"uniform vec4 whiteColor;" \
		"void main(void)" \
		"{" \
		"vec3 tex=vec3(texture(u_sampler,out_texquard));"\
		"fragColor = vec4(tex,1.0f);" \
		"fragColor = fragColor + whiteColor;" \
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
	glBindAttribLocation(shaderProgramObject_BRK, VDG_ATTRIBUTE_TEXTURE0, "vTexquard");
	
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
	mvpUniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_mvp_matrix");
	texture_sampler_uniform_BRK = glGetUniformLocation(shaderProgramObject_BRK, "u_sampler");
	QuardColor_BRK = glGetUniformLocation(shaderProgramObject_BRK, "whiteColor");
	
	//load texture
	
	Smilytexture_BRK = [self loadTextureFromBMPFile:"Smiley.bmp"];
	
	
	const GLfloat rectangleVertices[] = { 1.0f, 1.0f, 0.0f,
											-1.0f, 1.0f, 0.0f,
											-1.0f, -1.0f, 0.0f,
											1.0f, -1.0f, 0.0f };

	const GLfloat rectangleTextCord[] = { 1.0f, 1.0f,
											0.0f, 1.0f,
											0.0f, 0.0f,
											1.0f, 0.0f };

										
	
		
	
	//Create Rectangle vao
	glGenVertexArrays(1, &vaoRectangle_BRK);
	glBindVertexArray(vaoRectangle_BRK);
	glGenBuffers(1, &vbo_Rectangleposition_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Rectangleposition_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_Smilytexture_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Smilytexture_BRK);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), NULL,GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	glClearDepth(1.0f);
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	//We will always cull back faces for better performance
	glEnable(GL_CULL_FACE);
	
	//enable texture
	glEnable(GL_TEXTURE_2D);
	
	//set background colors
	glClearColor(0.0f,0.0f,0.0f,0.0f); // Blue
	
	//set projection matrix to identity matrix
	perspectiveProjectioMatrix_BRK = vmath::mat4::identity();
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
	CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
	CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	CVDisplayLinkStart(displayLink);
    
    pressdigit_BRK = 0;
    
    [super prepareOpenGL];
}


-(GLuint)loadTextureFromBMPFile:(const char *)texFileName
{
	NSBundle *mainBundle=[NSBundle mainBundle];
	NSString *appDirName=[mainBundle bundlePath];
	NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
	NSString *textureFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];
	
	NSImage *bmpImage=[[NSImage alloc]initWithContentsOfFile:textureFileNameWithPath];
	if(!bmpImage)
	{
		NSLog(@"can't find %@", textureFileNameWithPath);
		return(0);
	}
	
	CGImageRef cgImage = [bmpImage CGImageForProposedRect:nil context:nil hints:nil];
	int w = (int)CGImageGetWidth(cgImage);
	int h = (int)CGImageGetHeight(cgImage);
	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
	void* pixels = (void*)CFDataGetBytePtr(imageData);
	
	GLuint bmpTexture;
	glGenTextures(1,&bmpTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Set 1 rather than default 4, for better performance
	
	glBindTexture(GL_TEXTURE_2D, bmpTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGBA,
					w,
					h,
					0,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					pixels);
					
	//create mipmaps for this texture for better image quality
	glGenerateMipmap(GL_TEXTURE_2D);
	
	CFRelease(imageData);
	return(bmpTexture);
	
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
	
    perspectiveProjectioMatrix_BRK=vmath::perspective(45.0f,width/height,0.1f,100.0f);
	
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
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Start using OpenGL program object
	glUseProgram(shaderProgramObject_BRK);
	
	GLfloat rectangle_textquard[8];
    GLfloat quardcoloruniformarray[4]={0.0f,0.0f,0.0f,0.0f};
             
    
             
             
	if (pressdigit_BRK == 1) {
		rectangle_textquard[0] = 0.5f;
		rectangle_textquard[1] = 0.5f;
		rectangle_textquard[2] = 0.0f;
		rectangle_textquard[3] = 0.5f;
		rectangle_textquard[4] = 0.0f;
		rectangle_textquard[5] = 0.0f;
		rectangle_textquard[6] = 0.5f;
		rectangle_textquard[7] = 0.0f;
		
	}
	else if (pressdigit_BRK == 2) {

		rectangle_textquard[0] = 1.0f;
		rectangle_textquard[1] = 1.0f;
		rectangle_textquard[2] = 0.0f;
		rectangle_textquard[3] = 1.0f;
		rectangle_textquard[4] = 0.0f;
		rectangle_textquard[5] = 0.0f;
		rectangle_textquard[6] = 1.0f;
		rectangle_textquard[7] = 0.0f;
		
	}
	else if (pressdigit_BRK == 3) {
		rectangle_textquard[0] = 2.0f;
		rectangle_textquard[1] = 2.0f;
		rectangle_textquard[2] = 0.0f;
		rectangle_textquard[3] = 2.0f;
		rectangle_textquard[4] = 0.0f;
		rectangle_textquard[5] = 0.0f;
		rectangle_textquard[6] = 2.0f;
		rectangle_textquard[7] = 0.0f;
		
	}
	else if (pressdigit_BRK == 4) {
		rectangle_textquard[0] = 0.5f;
		rectangle_textquard[1] = 0.5f;
		rectangle_textquard[2] = 0.5f;
		rectangle_textquard[3] = 0.5f;
		rectangle_textquard[4] = 0.5f;
		rectangle_textquard[5] = 0.5f;
		rectangle_textquard[6] = 0.5f;
		rectangle_textquard[7] = 0.5f;
		
	}
	else {
		
		rectangle_textquard[0] = 0.0f;
		rectangle_textquard[1] = 0.0;
		rectangle_textquard[2] = 0.0f;
		rectangle_textquard[3] = 0.0f;
		rectangle_textquard[4] = 0.0f;
		rectangle_textquard[5] = 0.0f;
		rectangle_textquard[6] = 0.0f;
		rectangle_textquard[7] = 0.0f;
		
		
	}
	
	
	//OpenGL Drawing
	//set modelview & modelviewprojection matrices to identity
	vmath::mat4 translateMatrix = vmath::mat4::identity();
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    	
	//Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	
	
	translateMatrix = translateMatrix * rotationMatrix;
	modelViewMatrix = modelViewMatrix * translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectioMatrix_BRK * modelViewMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform_BRK, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind with texture
    if((pressdigit_BRK > 0) && (pressdigit_BRK < 5))
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Smilytexture_BRK);
        glUniform1i(texture_sampler_uniform_BRK, 0);
        quardcoloruniformarray[0]=0.0f;
        quardcoloruniformarray[1]=0.0f;
        quardcoloruniformarray[2]=0.0f;
        quardcoloruniformarray[3]=0.0f;
        glUniform4fv(QuardColor_BRK, 1,quardcoloruniformarray);
    }
    else{
        quardcoloruniformarray[0]=1.0f;
        quardcoloruniformarray[1]=1.0f;
        quardcoloruniformarray[2]=1.0f;
        quardcoloruniformarray[3]=1.0f;
        glUniform4fv(QuardColor_BRK,1, quardcoloruniformarray);
        
    }
	
	
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vaoRectangle_BRK);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_Smilytexture_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_textquard), rectangle_textquard, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw necessary seen
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	//unbind vao
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
		case 49:
		//case VK_NUMPAD1:
			//glEnable(GL_TEXTURE_2D);
			pressdigit_BRK = 1;
			break;

		case 50:
		//case VK_NUMPAD2:
			//glEnable(GL_TEXTURE_2D);
			pressdigit_BRK = 2;
			break;

		case 51:
		//case VK_NUMPAD3:
			//glEnable(GL_TEXTURE_2D);
			pressdigit_BRK = 3;
			break;

		case 52:
		//case VK_NUMPAD4:
			//glEnable(GL_TEXTURE_2D);
			pressdigit_BRK = 4;
			break;

		default:
			//glDisable(GL_TEXTURE_2D);
			pressdigit_BRK = 5;
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
	
	
	if(vaoRectangle_BRK)
	{
		glDeleteVertexArrays(1, &vaoRectangle_BRK);
		vaoRectangle_BRK = 0;
	}
	
	//destroy vbo_Rectangleposition_BRK
	if(vbo_Rectangleposition_BRK)
	{
		glDeleteBuffers(1,&vbo_Rectangleposition_BRK);
		vbo_Rectangleposition_BRK = 0;
	}
	
	if(vbo_Smilytexture_BRK)
	{
		glDeleteBuffers(1,&vbo_Smilytexture_BRK);
		vbo_Smilytexture_BRK = 0;
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


	
	

