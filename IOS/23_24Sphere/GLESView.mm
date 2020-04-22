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


float lightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float lightDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };


float MaterialAmbient[4]; //= { 0.0f,0.0f,0.0f,0.0f };
float MaterialDiffuse[4]; //= { 1.0f,1.0f,1.0f,1.0f };
float MaterialSpecular[4]; //= { 1.0f,1.0f,1.0f,1.0f };
float MaterialShianyness;// = 128.0f;

GLfloat LightAngle = 0.0f;


float firstRotation = 0.0f, secondRotation = 0.0f;
bool xrotation = false, yrotation = false, zrotation = false;


GLuint vertexShaderObject_BRK;
GLuint fragmentShaderObject_BRK;
GLuint shaderProgramObject_BRK;

GLuint vao_Sphere_BRK;
GLuint vbo_Sphere_Vertex_BRK;
GLuint vboLine_Sphere_normal_BRK;

bool bLight_BRK ;
GLuint modelMatrix_Uniform_BRK, viewMatrix_Uniform_BRK, projection_Uniform_BRK;
GLuint ld_Uniform_BRK, la_Uniform_BRK, ls_Uniform_BRK, lightPosition_Uniform_BRK;
GLuint ka_Uniform_BRK, kd_Uniform_BRK, ks_Uniform_BRK;
GLuint LKeyPress_Uniform_BRK, materialShinyness_Uniform_BRK;


vmath::mat4 perspectiveProjectionMatrix_BRK;

GLuint gNumOfVertices;

int gwidth,gheight;

int rotation = 0;

@implementation GLESView
{
	EAGLContext *eaglContext;
	
	GLuint defaultFramebuffer;
	GLuint colorRenderbuffer;
	GLuint depthRenderbuffer;
	
	id displayLink;
	NSInteger animationFrameInterval;
	BOOL isAnimating;
	
  
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
        "precision highp int;"\
        "precision highp float;" \
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
        "precision highp int;" \
        "precision highp float;" \
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
         
         
         
         std::vector<float>circlevertex;
         
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
                 
                 //angle = (2 * M_PI *index) / numpoints;
                 float x = (radius * sin(theta) * cos(phy));
                 float y = (radius * sin(theta) * sin(phy));
                 float z = (radius * cos(theta));
                 
                 
                 
                 circlevertex.push_back(x);
                 circlevertex.push_back(y);
                 circlevertex.push_back(z);
                 
                 
                 
                 gNumOfVertices++;
                 
                 x = (radius * sin(theta+(thetainc/2)) * cos(phy-(phyinc/2)));
                 y = (radius * sin(theta+(thetainc/2)) * sin(phy-(phyinc/2)));
                 z = (radius * cos(theta+(thetainc/2)));
                 
                
                 
                 circlevertex.push_back(x);
                 circlevertex.push_back(y);
                 circlevertex.push_back(z);
                 
                 
                 
                 gNumOfVertices++;
                 
                 x = (radius * sin(theta+(thetainc/2)) * cos(phy+(phyinc/2)));
                 y = (radius * sin(theta+(thetainc/2)) * sin(phy+(phyinc/2)));
                 z = (radius * cos(theta+(thetainc/2)));
                 
                 
                 
                 circlevertex.push_back(x);
                 circlevertex.push_back(y);
                 circlevertex.push_back(z);
                 
                 
                 
                 gNumOfVertices++;
                 
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
		glClearColor(0.2f,0.2f,0.2f,1.0f); //blue
				
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
    
    bLight_BRK = TRUE;
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

-(void) draw24Sphere
{

    // Material Configuration
    /*GLfloat MaterialAmbient[4];
    GLfloat MaterialDiffuse[4];
    GLfloat MaterialSpecular[4];
    GLfloat MaterialShianyness[1];*/
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    vmath::mat4 translateMatrix = vmath::mat4::identity();
   // vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    

    if (true == xrotation)
    {
        lightPosition[0] = 0.0f;
        lightPosition[1] = firstRotation;
        lightPosition[2] = secondRotation;
    }
    else if (true == yrotation)
    {
        lightPosition[0] = firstRotation;
        lightPosition[1] = 0.0f;
        lightPosition[2] = secondRotation;
    }
    else if (true == zrotation)
    {
        lightPosition[0] = firstRotation;
        lightPosition[1] = secondRotation;
        lightPosition[2] = -3.0f;
    }
    else
    {
        lightPosition[0] = 0.0f;
        lightPosition[1] = 0.0f;
        lightPosition[2] = 0.0f;
    }

    if (bLight_BRK == TRUE)
    {
        glUniform1i(LKeyPress_Uniform_BRK, 1);
        glUniform3fv(la_Uniform_BRK, 1, lightAmbient);
        glUniform3fv(ld_Uniform_BRK, 1, lightDiffuse);
        glUniform3fv(ls_Uniform_BRK, 1, lightSpecular);
        glUniform4fv(lightPosition_Uniform_BRK, 1, lightPosition);

    }
    else
    {
        glUniform1i(LKeyPress_Uniform_BRK, 0);
    }
    

    //************* 1st Sphere on 1st column, emerald **************//

    

    MaterialAmbient[0] = 0.0215;
    MaterialAmbient[1] = 0.1745;
    MaterialAmbient[2] = 0.0215;
    MaterialAmbient[3] = 1.0;


    MaterialDiffuse[0] = 0.07568;
    MaterialDiffuse[1] = 0.61424;
    MaterialDiffuse[2] = 0.07568;
    MaterialDiffuse[3] = 1.0f;


    MaterialSpecular[0] = 0.633;
    MaterialSpecular[1] = 0.727811;
    MaterialSpecular[2] = 0.633;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);
    
    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;
    //modelMatrix = normalize( modelMatrix);

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,(5*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 2nd Sphere on 1st column, jade **************//

    
    MaterialAmbient[0] = 0.135;
    MaterialAmbient[1] = 0.2225;
    MaterialAmbient[2] = 0.1575;
    MaterialAmbient[3] = 1.0;


    MaterialDiffuse[0] = 0.54;
    MaterialDiffuse[1] = 0.89;
    MaterialDiffuse[2] = 0.63;
    MaterialDiffuse[3] = 1.0f;


    MaterialSpecular[0] = 0.316228;
    MaterialSpecular[1] = 0.316228;
    MaterialSpecular[2] = 0.316228;
    MaterialSpecular[3] = 1.0;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    MaterialShianyness = 0.6 * 128;


    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,(4*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 3rd Sphere on 1st column, obsidian **************//

    MaterialAmbient[0] = 0.05375;
    MaterialAmbient[1] = 0.05;
    MaterialAmbient[2] = 0.06625;
    MaterialAmbient[3] = 1.0;


    MaterialDiffuse[0] = 0.18275;
    MaterialDiffuse[1] = 0.17;
    MaterialDiffuse[2] = 0.22525;
    MaterialDiffuse[3] = 1.0f;


    MaterialSpecular[0] = 0.332741;
    MaterialSpecular[1] = 0.328634;
    MaterialSpecular[2] = 0.346435;
    MaterialSpecular[3] = 1.0;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    MaterialShianyness = 0.6 * 128;


    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,(3*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 4th Sphere on 1st column, pearl **************//

    
    MaterialAmbient[0] = 0.25;
    MaterialAmbient[1] = 0.20725;
    MaterialAmbient[2] = 0.20725;
    MaterialAmbient[3] = 1.0;


    MaterialDiffuse[0] = 1.0;
    MaterialDiffuse[1] = 0.829;
    MaterialDiffuse[2] = 0.829;
    MaterialDiffuse[3] = 1.0f;


    MaterialSpecular[0] = 0.296648;
    MaterialSpecular[1] = 0.296648;
    MaterialSpecular[2] = 0.296648;
    MaterialSpecular[3] = 1.0;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    MaterialShianyness = 0.6 * 128;


    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,(2*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 5th Sphere on 1st column, ruby **************//

    
    MaterialAmbient[0] = 0.1745;
    MaterialAmbient[1] = 0.01175;
    MaterialAmbient[2] = 0.01175;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.61424;
    MaterialDiffuse[1] = 0.04136;
    MaterialDiffuse[2] = 0.04136;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.727811;
    MaterialSpecular[1] = 0.626959;
    MaterialSpecular[2] = 0.626959;
    MaterialSpecular[3] = 1.0;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    MaterialShianyness = 0.6 * 128;


    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,(1*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 6th Sphere on 1st column, turquoise **************//

    
    MaterialAmbient[0] = 0.1;
    MaterialAmbient[1] = 0.18725;
    MaterialAmbient[2] = 0.1745;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.396;
    MaterialDiffuse[1] = 0.74151;
    MaterialDiffuse[2] = 0.69102;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.297254;
    MaterialSpecular[1] = 0.30829;
    MaterialSpecular[2] = 0.306678;
    MaterialSpecular[3] = 1.0;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    MaterialShianyness = 0.6 * 128;


    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport(0,0,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);




    //************* 1st Sphere on 2nd column, brass **************//

    MaterialAmbient[0] = 0.329412;
    MaterialAmbient[1] = 0.223529;
    MaterialAmbient[2] = 0.027451;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.780392;
    MaterialDiffuse[1] = 0.568627;
    MaterialDiffuse[2] = 0.113725;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.992157;
    MaterialSpecular[1] = 0.941176;
    MaterialSpecular[2] = 0.807843;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 2nd Sphere on 2nd column, bronze **************//

    MaterialAmbient[0] = 0.2125;
    MaterialAmbient[1] = 0.1275;
    MaterialAmbient[2] = 0.054;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.714;
    MaterialDiffuse[1] = 0.4284;
    MaterialDiffuse[2] = 0.18144;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.393548;
    MaterialSpecular[1] = 0.271906;
    MaterialSpecular[2] = 0.166721;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 3rd Sphere on 2nd column, chrome **************//

    MaterialAmbient[0] = 0.25;
    MaterialAmbient[1] = 0.25;
    MaterialAmbient[2] = 0.25;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.4;
    MaterialDiffuse[1] = 0.4;
    MaterialDiffuse[2] = 0.4;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.774597;
    MaterialSpecular[1] = 0.774597;
    MaterialSpecular[2] = 0.774597;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 4th Sphere on 2nd column, pearl **************//

    MaterialAmbient[0] = 0.19125;
    MaterialAmbient[1] = 0.0735;
    MaterialAmbient[2] = 0.0225;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.7038;
    MaterialDiffuse[1] = 0.27048;
    MaterialDiffuse[2] = 0.0828;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.256777;
    MaterialSpecular[1] = 0.137622;
    MaterialSpecular[2] = 0.086014;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 5th Sphere on 2nd column, ruby **************//

    MaterialAmbient[0] = 0.24725;
    MaterialAmbient[1] = 0.1995;
    MaterialAmbient[2] = 0.0745;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.75164;
    MaterialDiffuse[1] = 0.60648;
    MaterialDiffuse[2] = 0.22648;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.628281;
    MaterialSpecular[1] = 0.555802;
    MaterialSpecular[2] = 0.366065;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 6th Sphere on 2nd column, silver **************//

    MaterialAmbient[0] = 0.19225;
    MaterialAmbient[1] = 0.19225;
    MaterialAmbient[2] = 0.19225;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.50754;
    MaterialDiffuse[1] = 0.50754;
    MaterialDiffuse[2] = 0.50754;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.508273;
    MaterialSpecular[1] = 0.508273;
    MaterialSpecular[2] = 0.508273;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((1*gwidth)/4,0,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);






    //************* 1st Sphere on 3nd column, black **************//

    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.01;
    MaterialDiffuse[1] = 0.01;
    MaterialDiffuse[2] = 0.01;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.5;
    MaterialSpecular[1] = 0.5;
    MaterialSpecular[2] = 0.5;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 2nd Sphere on 3nd column, cyan **************//

    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.1;
    MaterialAmbient[2] = 0.06;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.0;
    MaterialDiffuse[1] = 0.50980392;
    MaterialDiffuse[2] = 0.50980392;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.50196078;
    MaterialSpecular[1] = 0.50196078;
    MaterialSpecular[2] = 0.50196078;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 3rd Sphere on 3nd column, green **************//
    
    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.1;
    MaterialDiffuse[1] = 0.35;
    MaterialDiffuse[2] = 0.1;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.45;
    MaterialSpecular[1] = 0.55;
    MaterialSpecular[2] = 0.45;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);




    //************* 4th Sphere on 3nd column, red **************//
    
    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.5;
    MaterialDiffuse[1] = 0.0;
    MaterialDiffuse[2] = 0.0;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.7;
    MaterialSpecular[1] = 0.6;
    MaterialSpecular[2] = 0.6;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 5th Sphere on 3nd column, white **************//
    
    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.55;
    MaterialDiffuse[1] = 0.55;
    MaterialDiffuse[2] = 0.55;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.70;
    MaterialSpecular[1] = 0.70;
    MaterialSpecular[2] = 0.70;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 6th Sphere on 3nd column, plastic **************//
    
    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.5;
    MaterialDiffuse[1] = 0.5;
    MaterialDiffuse[2] = 0.0;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.6;
    MaterialSpecular[1] = 0.6;
    MaterialSpecular[2] = 0.5;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((2*gwidth)/4,0,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);







    //************* 1st Sphere on 4th column, black **************//


    MaterialAmbient[0] = 0.02;
    MaterialAmbient[1] = 0.02;
    MaterialAmbient[2] = 0.02;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.01;
    MaterialDiffuse[1] = 0.01;
    MaterialDiffuse[2] = 0.01;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.4;
    MaterialSpecular[1] = 0.4;
    MaterialSpecular[2] = 0.4;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);



    //************* 2nd Sphere on 4th column, cyan **************//


    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.05;
    MaterialAmbient[2] = 0.05;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.4;
    MaterialDiffuse[1] = 0.5;
    MaterialDiffuse[2] = 0.5;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.04;
    MaterialSpecular[1] = 0.7;
    MaterialSpecular[2] = 0.7;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);




    //************* 3rd Sphere on 4th column, green **************//

    
    MaterialAmbient[0] = 0.0;
    MaterialAmbient[1] = 0.05;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.4;
    MaterialDiffuse[1] = 0.5;
    MaterialDiffuse[2] = 0.4;
    MaterialDiffuse[3] = 1.0f;

    MaterialSpecular[0] = 0.04;
    MaterialSpecular[1] = 0.7;
    MaterialSpecular[2] = 0.04;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);


    //************* 4th Sphere on 4th column, red **************//

    MaterialAmbient[0] = 0.05;
    MaterialAmbient[1] = 0.0;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.5;
    MaterialDiffuse[1] = 0.4;
    MaterialDiffuse[2] = 0.4;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.7;
    MaterialSpecular[1] = 0.04;
    MaterialSpecular[2] = 0.04;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);







    //************* 5th Sphere on 4th column, white **************//

    MaterialAmbient[0] = 0.05;
    MaterialAmbient[1] = 0.05;
    MaterialAmbient[2] = 0.05;
    MaterialAmbient[3] = 1.0;
    
    MaterialDiffuse[0] = 0.5;
    MaterialDiffuse[1] = 0.5;
    MaterialDiffuse[2] = 0.5;
    MaterialDiffuse[3] = 1.0f;
    
    MaterialSpecular[0] = 0.7;
    MaterialSpecular[1] = 0.7;
    MaterialSpecular[2] = 0.7;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);




    //************* 6th Sphere on 4th column, plastic **************//



    MaterialAmbient[0] = 0.05;
    MaterialAmbient[1] = 0.05;
    MaterialAmbient[2] = 0.0;
    MaterialAmbient[3] = 1.0;
    

    MaterialDiffuse[0] = 0.5;
    MaterialDiffuse[1] = 0.5;
    MaterialDiffuse[2] = 0.4;
    MaterialDiffuse[3] = 1.0f;
    

    MaterialSpecular[0] = 0.7;
    MaterialSpecular[1] = 0.7;
    MaterialSpecular[2] = 0.04;
    MaterialSpecular[3] = 1.0;


    MaterialShianyness = 0.6 * 128;

    glUniform3fv(ka_Uniform_BRK, 1, MaterialAmbient);
    glUniform3fv(kd_Uniform_BRK, 1, MaterialDiffuse);
    glUniform3fv(ks_Uniform_BRK, 1, MaterialSpecular);
    glUniform1f(materialShinyness_Uniform_BRK, MaterialShianyness);

    modelMatrix = vmath::mat4::identity();
    viewMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Matrix multiplication
    translateMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //scaleMatrix = scale(0.3f, 0.3f, 0.3f);

    modelMatrix = modelMatrix * translateMatrix;
    modelMatrix = modelMatrix * scaleMatrix;

    //Send necessary Matrix to shader in respective uniform
    glUniformMatrix4fv(modelMatrix_Uniform_BRK, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrix_Uniform_BRK, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projection_Uniform_BRK, 1, GL_FALSE, perspectiveProjectionMatrix_BRK);

    glViewport((3*gwidth)/4,0,gwidth/6,gheight/6);
    glBindVertexArray(vao_Sphere_BRK);
    glDrawArrays(GL_TRIANGLES, 0, gNumOfVertices/3);
    glBindVertexArray(0);




}






-(void)drawView:(id)sender
{

	//code
	[EAGLContext setCurrentContext:eaglContext];
	
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	
  glUseProgram(shaderProgramObject_BRK);
   
   [self draw24Sphere];
       
   glUseProgram(0);
   
  LightAngle = LightAngle + 1.0f;
   firstRotation = 3 * cos(LightAngle);
   secondRotation =  3 * sin(LightAngle);
    
      
    if(rotation == 1)
    {
        xrotation = true;
        yrotation = false;
        zrotation = false;
    }
    else if(rotation == 2)
    {
        xrotation = false;
        yrotation = true;
        zrotation = false;
        
    }
    else if(rotation == 3)
    {
        xrotation = false;
        yrotation = false;
        zrotation = true;
    }
    
    
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
		
    gwidth = width;
    gheight = height;
    
    
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
    
    if(rotation>3)
    {
        rotation=0;
    }
    else
        rotation++;
    
    
    
    
	
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{

    
    if(bLight_BRK == TRUE)
        bLight_BRK = FALSE;
    else
        bLight_BRK = TRUE;
    
    
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



	
		
		
		
		
