//
// GLESView.m
//Window-BlueColor
//
//
#import <OpenGLES/ES3/gl.h>
#import <OPenGLES/ES3/glext.h>

#import "vmath.h"
#import "GLESView.h"

enum
{
	VDG_ATTRIBUTE_VERTEX =0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

@implementation GLESView
{
	EAGLContext *eaglContext;
	
	GLuint defaultFramebuffer;
	GLuint colorRenderbuffer;
	GLuint depthRenderbuffer;
	
	id displayLink;
	NSInteger animationFrameInterval;
	BOOL isAnimating;

    GLuint vertexShaderObject_BRK;
    GLuint fragmentShaderObject_BRK;
    GLuint shaderProgramObject_BRK;
    
    GLuint vao_pyramid_BRK;
    GLuint vbo_position_pyramid_BRK;
    GLuint vao_cube_BRK;
    GLuint vbo_position_cube_BRK;
    GLuint vbo_color_pyramid_BRK, vbo_color_cube_BRK;
    GLuint mvpUniform;
    GLfloat angle_triangle , angle_rectangle;
    vmath::mat4 perspectiveProjectioMatrix;}

-(id)initWithFrame:(CGRect)frame;
{
	//code
	self=[super initWithFrame:frame];
	
	if(self)
	{
	
		CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
		eaglLayer.opaque=YES;
		eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
									kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat,nil];
							
		eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
		if(eaglContext == nil)
		{
			[self release];
			return(nil);
		}
		
		[EAGLContext setCurrentContext:eaglContext];
		
		glGenFramebuffers(1,&defaultFramebuffer);
		glGenRenderbuffers(1,&colorRenderbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		
		[eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
		
		GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindFramebuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            glDeleteFramebuffers(1,&defaultFramebuffer);
            glDeleteRenderbuffers(1,&colorRenderbuffer);
            glDeleteRenderbuffers(1,&depthRenderbuffer);
            
            return(nil);
        }
		printf("Renderer : %s | GL Version : %s  |GLSL Version : %s\n",
				glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
				
		//hard coded intialization
		isAnimating = NO;
		animationFrameInterval = 60;  //default since iOS 8.2
		
		
        //*** VERTEX SHADER *****
        //create shader
        vertexShaderObject_BRK=glCreateShader(GL_VERTEX_SHADER);
        
        //provide source code to shader
        const GLchar *vertexShaderSourceCode =
        "#version 300 es"\
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
                    printf( "Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
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
        "#version 300 es" \
        "\n" \
        "precision highp float;"
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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
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
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
                }
            }
        }
        
        // grt MVP uniform location
        mvpUniform = glGetUniformLocation(shaderProgramObject_BRK, "u_mvp_matrix");
        
        // *** vertices, colors, shader attribs, vbo_position_pyramid_BRK, vao_pyramid_BRK intiallize ****
        
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
                                            -1.0f, -1.0f, -1.0f    };

        const GLfloat pyramidColor[] = { 1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f,
                                            1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f,
                                            1.0f, 0.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f,
                                            0.0f, 1.0f, 0.0f ,
                                            1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f };


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
                                            -1.0, -1.0, 1.0    };

        const GLfloat cubeColor[] = { 1.0, 0.0, 0.0,
                                            1.0, 0.0, 0.0,
                                            1.0, 0.0, 0.0,
                                            1.0, 0.0, 0.0,
                                            0.0, 1.0, 0.0,
                                            0.0, 1.0, 0.0,
                                            0.0, 1.0, 0.0,
                                            0.0, 1.0, 0.0,
                                            0.0, 0.0, 1.0,
                                            0.0, 0.0, 1.0,
                                            0.0, 0.0, 1.0,
                                            0.0, 0.0, 1.0,
                                            0.0, 1.0, 1.0,
                                            0.0, 1.0, 1.0,
                                            0.0, 1.0, 1.0,
                                            0.0, 1.0, 1.0,
                                            1.0, 0.0, 1.0,
                                            1.0, 0.0, 1.0,
                                            1.0, 0.0, 1.0,
                                            1.0, 0.0, 1.0,
                                            1.0, 1.0, 0.0,
                                            1.0, 1.0, 0.0,
                                            1.0, 1.0, 0.0,
                                            1.0, 1.0, 0.0};
                                            
        
        glGenVertexArrays(1,&vao_pyramid_BRK);
        glBindVertexArray(vao_pyramid_BRK);
        
        glGenBuffers(1, &vbo_position_pyramid_BRK);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
        glBindBuffer(GL_ARRAY_BUFFER, 0 );
        
        
        glGenBuffers(1, &vbo_color_pyramid_BRK);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColor), pyramidColor, GL_STATIC_DRAW);
        glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
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


        glGenBuffers(1, &vbo_color_cube_BRK);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube_BRK);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
        glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        
        
        
        
        
        
	
	glClearDepthf(1.0f);
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	//We will always cull back faces for better performance
	//glEnable(GL_CULL_FACE);
		
			
		//clear color
		glClearColor(0.0f,0.0f,0.0f,1.0f); //blue
				
		//set projection matrix to identity matrix
		perspectiveProjectioMatrix = vmath::mat4::identity();

		
		//GESTURE RECOGNITION
		//Tap gesture code
		UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
		[singleTapGestureRecognizer setNumberOfTapsRequired:1];
		[singleTapGestureRecognizer setNumberOfTouchesRequired:1]; //touch of 1 finger
		[singleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:singleTapGestureRecognizer];
		
		UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
		[doubleTapGestureRecognizer setNumberOfTapsRequired:2];
		[doubleTapGestureRecognizer setNumberOfTouchesRequired:1];//touch of 1 finger
		[doubleTapGestureRecognizer setDelegate:self];
		[self addGestureRecognizer:doubleTapGestureRecognizer];
		
		//this will allow to differentiate between single tap and double tap
		[singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
		
		//swipe gesture
		UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
		[self addGestureRecognizer:swipeGestureRecognizer];
		
		//long-press gesture
		UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
		[self addGestureRecognizer:longPressGestureRecognizer];
	}
	return(self);
}

/*
// Only override drawrect : if you perform custom drawing
// An empty implementation adversely affects performance during animationFrameInterval
-(void)drawRect:(CGRect)rect
{
	//Drawing code
}
*/

+(Class)layerClass
{
	//code
	return([CAEAGLLayer class]);
}

-(void)drawView:(id)sender
{

	//code
	[EAGLContext setCurrentContext:eaglContext];
	
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	    //Start using OpenGL program object
    glUseProgram(shaderProgramObject_BRK);
    
    //OpenGL Drawing
    //set modelview & modelviewprojection matrices to identity
    vmath::mat4 translateMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::translate(-2.0f,0.0f,-6.0f);
    rotationMatrix = vmath::rotate(angle_triangle, 0.0f, 1.0f, 0.0f);
    translateMatrix = translateMatrix * rotationMatrix;
    modelViewMatrix = modelViewMatrix * translateMatrix;
    //multiply the modelview and orthographic matrix to get modelviewprojection matrix
    modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix; // ORDER IS Important
    
    //pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variables
    //whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
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
    rotationMatrix = vmath::rotate(angle_rectangle, angle_rectangle, angle_rectangle);
    
    translateMatrix = translateMatrix * rotationMatrix;
    modelViewMatrix = modelViewMatrix * translateMatrix;
    modelViewProjectionMatrix = perspectiveProjectioMatrix * modelViewMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

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

	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if (angle_triangle > 360)
      {
          angle_triangle = 0.0f;
          angle_rectangle = 0.0f;
      }
      else {
          angle_triangle = angle_triangle + 2.0f;
          angle_rectangle = angle_rectangle + 2.0f;
      }
}

-(void)layoutSubviews
{
    
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    glGenRenderbuffers(1,&depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
    
    glViewport(0,0,width,height);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            
        }
        
	
	
	//glOrtho(left,right,bottom,top,near,far);
	GLfloat fwidth = (GLfloat)width;
	GLfloat fheight = (GLfloat)height;
	
	perspectiveProjectioMatrix = vmath::perspective(45.0f, fwidth/fheight,0.1f, 100.0f);
	
	
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("failed To Create Complete Framebuffer Object %x\n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
			
		}
		
	[self drawView:nil];
}

-(void)startAnimation
{
	if(!isAnimating)
	{
		displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
		[displayLink setPreferredFramesPerSecond:animationFrameInterval];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop]forMode:NSDefaultRunLoopMode];
		
		isAnimating=YES;
	}
}

-(void)stopAnimation
{
	if(isAnimating)
	{
		[displayLink invalidate];
		displayLink = nil;
		
		isAnimating = NO;
	}
}


	

// to become first responder
-(BOOL)acceptsFirstResponder
{
	//code
	return(YES);
	
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{

	//code
	
	
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{

	//code
	
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{

	
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{

	//code
	[self release];
	exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
	
	
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
    
    if(vbo_color_pyramid_BRK)
    {
        glDeleteBuffers(1,&vbo_color_pyramid_BRK);
        vbo_color_pyramid_BRK = 0;
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
    
    if(vbo_color_cube_BRK)
    {
        glDeleteBuffers(1,&vbo_color_cube_BRK);
        vbo_color_cube_BRK = 0;
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
	
	if(depthRenderbuffer)
	{
		glDeleteRenderbuffers(1,&depthRenderbuffer);
		depthRenderbuffer=0;
	}
	
	if(colorRenderbuffer)
	{
		glDeleteRenderbuffers(1,&colorRenderbuffer);
		colorRenderbuffer = 0;
	}
	
	if(defaultFramebuffer)
	{
		glDeleteFramebuffers(1,&defaultFramebuffer);
		defaultFramebuffer = 0;
	}
	
	if([EAGLContext currentContext]==eaglContext)
	{
		[EAGLContext setCurrentContext:nil];
	}
	[eaglContext release];
	eaglContext=nil;
	
	[super dealloc];
}

@end



	
		
		
		
		
