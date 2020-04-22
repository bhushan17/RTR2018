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
                 printf( "Shader Program Link Log : %s\n", szInfoLog);
                 free(szInfoLog);
                 [self release];
                 
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
		perspectiveProjectionMatrix_BRK = vmath::mat4::identity();

		
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
    
    bLight_BRK = false;
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
      
    
   /* if(angle_rotate<360)
        angle_rotate = angle_rotate + 2.5f;
    else
        angle_rotate = 0.0f;
    
    */
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
	
	perspectiveProjectionMatrix_BRK = vmath::perspective(45.0f, fwidth/fheight,0.1f, 100.0f);
	
	
	
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



	
		
		
		
		
