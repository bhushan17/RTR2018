

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
							
	[window setTitle:@"macOS OpenGL Bhushan's Cube Pyramid Texture Window"];
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
	
	GLuint vao_pyramid_BRK;
	GLuint vbo_position_pyramid_BRK;
	GLuint vao_cube_BRK;
	GLuint vbo_position_cube_BRK;
	GLuint vbo_pyramid_texture_BRK, vbo_cube_texture_BRK;
	GLuint mvpUniform_BRK;
	GLfloat angle_pyramid_BRK , angle_cube_BRK;
	vmath::mat4 perspectiveProjectioMatrix_BRK;
	
	GLuint pyramid_texture_BRK;
	GLuint cube_texture_BRK;
	
	GLuint texture_sampler_uniform_BRK;
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
		"void main(void)" \
		"{" \
		"vec3 tex=vec3(texture(u_sampler,out_texquard));"\
		"fragColor = vec4(tex,1.0f);" \
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
	
	//load texture
	pyramid_texture_BRK = [self loadTextureFromBMPFile:"Stone.bmp"];
	cube_texture_BRK = [self loadTextureFromBMPFile:"Vijay_Kundali.bmp"];
	
	// *** vertices, colors, shader attribs, vbo_position_pyramid_BRK, vao_pyramid_BRK intiallize ****
	
	const GLfloat pyramidVertices[] = { 0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
        
										0.0f, 1.0f, 0.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,
        
										0.0f, 1.0f, 0.0f,
										1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
        
										0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, 1.0f	};

	const GLfloat pyramidTexCord[] = { 0.5f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										0.5f, 1.0f,
										1.0f, 0.0f,
										0.0f, 0.0f,
										0.5f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f ,
										0.5f, 1.0f,
										1.0f, 0.0f,
										0.0f, 0.0f };


	const GLfloat cubeVertices[] = { 1.0, 1.0, -1.0,
										-1.0, 1.0, -1.0,
										-1.0, 1.0, 1.0,
										1.0, 1.0, 1.0,
										1.0, -1.0, 1.0,
										-1.0, -1.0, 1.0,
										-1.0, -1.0, -1.0,
										1.0, -1.0, -1.0,
										1.0, 1.0, 1.0,
										-1.0, 1.0, 1.0,
										-1.0, -1.0, 1.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
										-1.0, -1.0, -1.0,
										-1.0, 1.0, -1.0,
										1.0, 1.0, -1.0,
										1.0, 1.0, -1.0,
										1.0, 1.0, 1.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
										-1.0, 1.0, 1.0,
										-1.0, 1.0, -1.0,
										-1.0, -1.0, -1.0,
										-1.0, -1.0, 1.0	};

	const GLfloat cubeTextCord[] = { 0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f };

										
	
	glGenVertexArrays(1,&vao_pyramid_BRK);
	glBindVertexArray(vao_pyramid_BRK);
	
	glGenBuffers(1, &vbo_position_pyramid_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	
	
	glGenBuffers(1, &vbo_pyramid_texture_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_texture_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCord), pyramidTexCord, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	
	
	
	
	
	//Create Rectangle vao
	glGenVertexArrays(1, &vao_cube_BRK);
	glBindVertexArray(vao_cube_BRK);
	glGenBuffers(1, &vbo_position_cube_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_cube_texture_BRK);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texture_BRK);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextCord), cubeTextCord, GL_STATIC_DRAW);
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
    angle_pyramid_BRK = 0.0f;
    angle_cube_BRK = 0.0f;
    
    
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
-(void)update
{
	if (angle_pyramid_BRK > 360)
	{
		angle_pyramid_BRK = 0.0f;
		angle_cube_BRK = 0.0f;
	}
	else {
		angle_pyramid_BRK = angle_pyramid_BRK + 2.0f;
		angle_cube_BRK = angle_cube_BRK + 2.0f;
	}
    
    [super update];
    
}

-(void)drawView
{
	//code
	[[self openGLContext]makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Start using OpenGL program object
	glUseProgram(shaderProgramObject_BRK);
	
	//OpenGL Drawing
	//set modelview & modelviewprojection matrices to identity
	vmath::mat4 translateMatrix = vmath::mat4::identity();
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::translate(-2.0f,0.0f,-6.0f);
	rotationMatrix = vmath::rotate(angle_pyramid_BRK, 0.0f, 1.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;
	modelViewMatrix = modelViewMatrix * translateMatrix;
	//multiply the modelview and orthographic matrix to get modelviewprojection matrix
	modelViewProjectionMatrix = perspectiveProjectioMatrix_BRK * modelViewMatrix; // ORDER IS Important
	
	//pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variables
	//whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(mvpUniform_BRK, 1, GL_FALSE, modelViewProjectionMatrix);
	
	//bind with texture
	glBindTexture(GL_TEXTURE_2D,pyramid_texture_BRK);
	
	
	//****bind vao_pyramid_BRK ***
	glBindVertexArray(vao_pyramid_BRK);
	
	//*** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLES, 0,12); // 3(each with its x,y,z) vertices in triangleVertices array
	
	//*** unbind vao_pyramid_BRK ***
	glBindVertexArray(0);
	
	
	//Intialize above matrix to Identity;
	modelViewMatrix = vmath::mat4::identity();
	translateMatrix = vmath::mat4::identity();
	rotationMatrix = vmath::mat4::identity();
	modelViewProjectionMatrix = vmath::mat4::identity();

	//Do necessary Matrix multiplication
    translateMatrix = vmath::translate(2.0f, 0.0f, -6.0f);
	rotationMatrix = vmath::rotate(angle_cube_BRK, angle_cube_BRK, angle_cube_BRK);
	
	translateMatrix = translateMatrix * rotationMatrix;
	modelViewMatrix = modelViewMatrix * translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectioMatrix_BRK * modelViewMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform_BRK, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind with texture
	glBindTexture(GL_TEXTURE_2D,cube_texture_BRK);
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao_cube_BRK);



	//Draw necessary seen
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//unbind vao
	glBindVertexArray(0);
	
	
	//stop using OpenGL program object 
	glUseProgram(0);
	
	CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    if (angle_pyramid_BRK > 360)
    {
        angle_pyramid_BRK = 0.0f;
        angle_cube_BRK = 0.0f;
    }
    else {
        angle_pyramid_BRK = angle_pyramid_BRK + 2.0f;
        angle_cube_BRK = angle_cube_BRK + 2.0f;
    }
	
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
	//destroy vao_pyramid_BRK
	if(vao_pyramid_BRK)
	{
		glDeleteVertexArrays(1, &vao_pyramid_BRK);
		vao_pyramid_BRK = 0;
	}
	
	//destroy vbo_position_pyramid_BRK
	if(vbo_position_pyramid_BRK)
	{
		glDeleteBuffers(1,&vbo_position_pyramid_BRK);
		vbo_position_pyramid_BRK = 0;
	}
	
	if(vbo_pyramid_texture_BRK)
	{
		glDeleteBuffers(1,&vbo_pyramid_texture_BRK);
		vbo_pyramid_texture_BRK = 0;
	}
	
	if(vao_cube_BRK)
	{
		glDeleteVertexArrays(1, &vao_cube_BRK);
		vao_cube_BRK = 0;
	}
	
	//destroy vbo_position_cube_BRK
	if(vbo_position_cube_BRK)
	{
		glDeleteBuffers(1,&vbo_position_cube_BRK);
		vbo_position_cube_BRK = 0;
	}
	
	if(vbo_cube_texture_BRK)
	{
		glDeleteBuffers(1,&vbo_cube_texture_BRK);
		vbo_cube_texture_BRK = 0;
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


	
	

