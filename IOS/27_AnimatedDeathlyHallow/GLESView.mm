//
// GLESView.m
//Window-BlueColor
//
//
#import <OpenGLES/ES3/gl.h>
#import <OPenGLES/ES3/glext.h>
#import <vector>
#import "vmath.h"
#import "GLESView.h"
#include"Sphere.h"


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
    
    GLuint vaoLine_BRK;
    GLuint vboLine_Position_BRK;
    
    
    GLuint vboLine_color_BRK;
    GLuint mvpUniform;
    
    vmath::mat4 perspectiveProjectioMatrix;
    
    
    
}

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
        "precision highp float;"\
        "precision highp int;"\
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
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
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
        "#version 300 es"\
        "\n" \
        "precision highp float;"\
        "precision highp int;"\
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
                    printf( "Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                    
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

	//code
	[EAGLContext setCurrentContext:eaglContext];
	
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
    
   
    
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
    
    
    
    
   
  
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	[eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
    
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



	
		
		
		
		
