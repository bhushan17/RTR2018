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
    
    GLuint vao_Sphere_BRK;
    GLuint vbo_Sphere_Vertex_BRK;
    GLuint vboLine_Sphere_normal_BRK;
    
    bool bLight_BRK ;
    GLuint mvUniform_BRK, pUniform_BRK, ldUniform_BRK,kdUniform_BRK, lightPositionUniform_BRK, LKeyPress_BRK;
    
    
    vmath::mat4 perspectiveProjectioMatrix;
    
    GLuint gNumOfVertices;
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
    "precision highp float;" \
    "precision highp int;" \
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
     "#version 300 es" \
     "\n" \
    "precision highp float;" \
    "precision highp int;" \
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
                 printf("Shader Program Link Log : %s\n", szInfoLog);
                 free(szInfoLog);
                 [self release];
                 
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
     //float angle = 0.0f;
     float phyinc = 0.1;
     float thetainc = 0.1;
     gNumOfVertices = 0;
     
     float theta = 0.5f;
     
    for(theta = 0.0f; theta <= 180.0f; theta += thetainc)
    {
         for (float phy = 0.0f; phy <= 360.0f; phy += phyinc)
         {
             float vec1[3], vec2[3], vec3[3];
             //float* normal;
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
             /*float v1[3],v2[3];
             
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
             circleNormal.push_back(normal[2]);*/
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
        
        
	
	glClearDepthf(1.0f);
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	//depth test to do
	glDepthFunc(GL_LEQUAL);
	//We will always cull back faces for better performance
	//glEnable(GL_CULL_FACE);
		
			
		//clear color
		glClearColor(0.0f,0.0f,1.0f,1.0f); //blue
				
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
	 static float angle_rotate = 0.0f;
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
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);
        
    glUseProgram(0);
    
    if(angle_rotate<360)
        angle_rotate = angle_rotate + 2.5f;
    else
        angle_rotate = 0.0f;
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
    if(bLight_BRK == TRUE)
        bLight_BRK = FALSE;
    else
        bLight_BRK = TRUE;
	
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
	//destroy vao
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



	
		
		
		
		
