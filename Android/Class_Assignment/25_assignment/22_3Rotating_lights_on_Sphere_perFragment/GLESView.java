package com.matrix_android.Rotating_lights_on_Sphere_perFragment;

//added by user

import android.content.Context;


import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener; 

import android.opengl.GLSurfaceView; // The view which gives us OPenGL Surface
import android.opengl.GLES32; //OpenGL ES 3.2 Version
import javax.microedition.khronos.opengles.GL10; //
import javax.microedition.khronos.egl.EGLConfig; // Embedded System supports EGL packages

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.ByteOrder;

import android.opengl.Matrix;

import 	java.lang.Math;
import java.nio.ShortBuffer;


public class GLESView extends GLSurfaceView implements  OnGestureListener, OnDoubleTapListener,  GLSurfaceView.Renderer
{
    private GestureDetector gestureDetector; 
	private final Context context;
	
	//Java neither have unsignint not gluint
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	//Java doesn't have & operator, we create array of 1 & pass name of array which behave as address.
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	
		
	private FloatBuffer positionBuffer;
	private FloatBuffer normalBuffer;
	private int modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
			
	private int red_ld_Uniform, red_la_Uniform, red_ls_Uniform, red_lightPosition_Uniform;
	private int green_ld_Uniform, green_la_Uniform, green_ls_Uniform, green_lightPosition_Uniform;
	private int blue_ld_Uniform, blue_la_Uniform, blue_ls_Uniform, blue_lightPosition_Uniform;
	private int ka_Uniform, kd_Uniform, ks_Uniform;
	private int LKeyPress_Uniform, materialShinyness_Uniform;
	
	
		
	float RedlightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float RedlightDiffuse[] = { 1.0f,0.0f,0.0f,1.0f };
	float RedlightSpecular[] = { 1.0f,0.0f,0.0f,1.0f };
	float RedlightPosition[] = { 0.0f,0.0f,0.0f,1.0f };

	float GreenlightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float GreenlightDiffuse[] = { 0.0f,1.0f,0.0f,1.0f };
	float GreenlightSpecular[] = { 0.0f,1.0f,0.0f,1.0f };
	float GreenlightPosition[] = { 0.0f,0.0f,0.0f,1.0f };

	float BluelightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float BluelightDiffuse[] = { 0.0f,0.0f,1.0f,1.0f };
	float BluelightSpecular[] = { 0.0f,0.0f,1.0f,1.0f };
	float BluelightPosition[] = { 0.0f,0.0f,0.0f,1.0f };


	float materialAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	float materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	float materialShinyness = 128.0f;
	
	
	
	
	private float[]perspectiveProjectionMatrix = new float[16]; // 4x4 matrix
	
		
	private float LightAnglezero = 0.0f;
	private float  LightAngleone = 0.0f;
	private float  LightAngletwo = 0.0f;
	
	float RedfirstRotation = 0.0f, RedsecondRotation = 0.0f;
	float GreenfirstRotation = 0.0f, GreensecondRotation = 0.0f;
	float BluefirstRotation = 0.0f, BluesecondRotation = 0.0f;
	private boolean bLight = false;
	private boolean animation = false;
	
	private int numVertices;
    private int numElements;
	
	GLESView(Context drawcontext)
	{
		super(drawcontext);
		context = drawcontext;
		
		//setGravity(Gravity.CENTER);
		setEGLContextClientVersion(3); //Tell to GLES that my view supprts 3
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		gestureDetector=new GestureDetector(context, this, null, false);//this means 'handler' i.e who is going to handler
		gestureDetector.setOnDoubleTapListener(this);//this means 'handler' i.e who is going to handle
		
	}
	
	//Handling onTouchEvent is the Most Impostant
	//Because It Triggers All Gesture And Tap Events
	
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		//code
		int eventaction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return(true);
	}
	
	//abstract method from OnDouble tapListener so must be implemented
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		//setGravity(Gravity.LEFT);
		if (bLight == false)
		{
			bLight = true;
		}
		else {
			bLight = false;
		}
		return(true);
	}
	
	//abstract method from OnDoubleTapListner so must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		
		//Do Not Write Any code Here Because Already Written 'onDoubleTap'
		return(true);
	}
	
	//abstract method from OnDoubleTapListener so must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		//setGravity(Gravity.FILL);
		
		return(true);
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onDown(MotionEvent e)
	{
		//Do Not Write Any code Here Because Already Written 'onSingleTapConfirmed'
		return(true);
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public void onLongPress(MotionEvent e)
	{
		//setGravity(Gravity.RIGHT);
		
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		//setGravity(Gravity.BOTTOM);
		Unintialize();
		System.exit(0);
		return (true);
	}
	
	//abstract method from OnGestureListner so must be implemented 
	@Override
	public void onShowPress(MotionEvent e)
	{
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		if (animation == false)
		{
				//Do nothing;
			animation = true;
		}
		else {
			animation = false;
				
		}
		
		
		return(true);
	}
	
	// Implement GLSurfaceView.Render
	
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		String version = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("RTR:" + version);
		intialize();
	}
	
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width,height);
	}
	
	@Override
	public void onDrawFrame(GL10 unused)
	{
		display();
		update();
	}
	
	//our Custom method
	
	private void intialize()
	{
		
		
		// Vertex Shader
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		final String vertexShaderSourceCode = String.format
		("#version 320 es" +
		"\n" +
		"precision lowp int;"+
		"precision highp float;"+
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_Lkeypress;" +
		"uniform vec4 u_red_lightPosition;" +
		"uniform vec4 u_green_lightPosition;" +
		"uniform vec4 u_blue_lightPosition;" +
		"out vec3 Transformednormal;" +
		"out vec3 RedlightDirection;" +
		"out vec3 GreenlightDirection;" +
		"out vec3 BluelightDirection;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"Transformednormal=(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
		"RedlightDirection =(vec3(u_red_lightPosition - eye_coordinate));" +
		"GreenlightDirection =(vec3(u_green_lightPosition - eye_coordinate));" +
		"BluelightDirection =(vec3(u_blue_lightPosition - eye_coordinate));" +
		"viewer_vector = normalize(vec3(-eye_coordinate.xyz));" +
		"}" +
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
		"}");
		
		GLES32.glShaderSource(vertexShaderObject,vertexShaderSourceCode);
		GLES32.glCompileShader(vertexShaderObject);
		
		int[]ishaderCompileStatus = new int[1];
		int[] iInfolog = new int[1];
		
		String SzInfolog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, ishaderCompileStatus, 0);
		if(ishaderCompileStatus[0] == 0)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfolog, 0);
			if(iInfolog[0]>0)
			{
				SzInfolog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("RTR: vertex shader compile log:" + SzInfolog);
				Unintialize();
				System.exit(0);
			}
		}
		
		
		// Fragment shader
		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		final String FragmentShaderSourseCode = String.format
		("#version 320 es" +
		"\n" +
		"precision highp float;"+
		"precision lowp int;"+
		"out vec4 fragColor;" +
		"in vec3 Red_phong_ads_light;" +
		"in vec3 Green_phong_ads_light;" +
		"in vec3 Blue_phong_ads_light;" +
		"uniform int u_Lkeypress;" +
		"uniform vec3 u_red_la;" +
		"uniform vec3 u_red_ld;" +
		"uniform vec3 u_red_ls;" +
		"uniform vec3 u_green_la;" +
		"uniform vec3 u_green_ld;" +
		"uniform vec3 u_green_ls;" +
		"uniform vec3 u_blue_la;" +
		"uniform vec3 u_blue_ld;" +
		"uniform vec3 u_blue_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_materialShinyness;" +
		"vec3 phong_ads_light;" +
		"in vec3 Transformednormal;" +
		"in vec3 RedlightDirection;" +
		"in vec3 GreenlightDirection;" +
		"in vec3 BluelightDirection;" +
		"in vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);"+
		"vec3 NormalizeRedlightDirection = normalize(RedlightDirection);"+
		"vec3 NormalizeGreenlightDirection = normalize(GreenlightDirection);"+
		"vec3 NormalizeBluelightDirection = normalize(BluelightDirection);"+
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);"+
		"float TN_dot_red_ld = max(dot(NormalizeRedlightDirection,NormalizeTransformednormal),0.0f);" +
		"float TN_dot_green_ld = max(dot(NormalizeGreenlightDirection,NormalizeTransformednormal),0.0f);" +
		"float TN_dot_blue_ld = max(dot(NormalizeBluelightDirection,NormalizeTransformednormal),0.0f);" +
		"vec3 Redreflection_vector = reflect(-NormalizeRedlightDirection,NormalizeTransformednormal);" +
		"vec3 Greenreflection_vector = reflect(-NormalizeGreenlightDirection,NormalizeTransformednormal);" +
		"vec3 Bluereflection_vector = reflect(-NormalizeBluelightDirection,NormalizeTransformednormal);" +
		"vec3 Redambient = u_red_la * u_ka;" +
		"vec3 Reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" +
		"vec3 Redspecular = u_red_ls * u_ks * pow(max(dot(Redreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
		"vec3 Red_phong_ads_light = Redambient + Reddiffuse + Redspecular;"+
		"vec3 Greenambient = u_green_la * u_ka;" +
		"vec3 Greendiffuse = u_green_ld * u_kd * TN_dot_green_ld;" +
		"vec3 Greenspecular = u_green_ls * u_ks * pow(max(dot(Greenreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
		"vec3 Green_phong_ads_light = Greenambient + Greendiffuse + Greenspecular;"+
		"vec3 Blueambient = u_blue_la * u_ka;" +
		"vec3 Bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" +
		"vec3 Bluespecular = u_blue_ls * u_ks * pow(max(dot(Bluereflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
		"vec3 Blue_phong_ads_light = Blueambient + Bluediffuse + Bluespecular;"+
		"phong_ads_light = Red_phong_ads_light + Green_phong_ads_light + Blue_phong_ads_light;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" +
		"}" +
		"fragColor = vec4(phong_ads_light,1.0);" +
		"}");
		
		GLES32.glShaderSource(fragmentShaderObject,FragmentShaderSourseCode);
		GLES32.glCompileShader(fragmentShaderObject);
		
		ishaderCompileStatus[0] = 0;
		iInfolog[0] = 0;
		
		SzInfolog = null;
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, ishaderCompileStatus, 0);
		if(ishaderCompileStatus[0] == 0)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfolog, 0);
			if(iInfolog[0]>0)
			{
				SzInfolog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("RTR: Fragment shader compile log:" + SzInfolog);
				Unintialize();
				System.exit(0);
			}
		}
			
		
		// Create shader program object
		shaderProgramObject = GLES32.glCreateProgram();
		
		//Attach Vertex Shader to the Shader Program
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		
		//Attach Fragnebt Shader
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		
		// Prelinking binding to Vertex attribute
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		
		//Link the Shader Program
		GLES32.glLinkProgram(shaderProgramObject);
		
		int[]iProgramLinkStatus = new int[1];
		iInfolog[0] = 0;
		
		SzInfolog = null;
		
		//ErrorCheking
		GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_LINK_STATUS,iProgramLinkStatus,0);
		if (iProgramLinkStatus[0] == 0)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfolog,0);
			if(iInfolog[0]>0)
			{
				SzInfolog = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("RTR: ProgramObject compile log:" + SzInfolog);
				Unintialize();
				System.exit(0);
			}
		}
		
		//Post linking retriving/getting uniform location 
		modelMatrix_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
		viewMatrix_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
		projection_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
		
		red_la_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_red_la");
		red_ld_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_red_ld");
		red_ls_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_red_ls");
		red_lightPosition_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_red_lightPosition");
		
		
		green_la_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_green_la");
		green_ld_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_green_ld");
		green_ls_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_green_ls");
		green_lightPosition_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_green_lightPosition");


		blue_la_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_blue_la");
		blue_ld_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_blue_ld");
		blue_ls_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_blue_ls");
		blue_lightPosition_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_blue_lightPosition");
			
		ka_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		kd_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		ks_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
		
				
		materialShinyness_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_materialShinyness");
		LKeyPress_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Lkeypress");
		
			
					
		Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

	
		// vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
		
				
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
		
	}
	
	
	
	
	private void resize(int width,int height)
	{
		if(height < 1)
			height = 1;
		
		GLES32.glViewport(0,0,width,height);
		
		
		
		Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f, (float)width / (float)height, 0.1f, 100.0f);
		
		
	}
	
	private void update()
	{
		if (animation == true)
		{
			LightAnglezero = LightAnglezero + 0.02f;
		LightAngleone = LightAngleone - 0.02f;
		LightAngletwo = LightAngleone + 0.02f;
		RedfirstRotation = 10 * (float)Math.cos(LightAnglezero);
		RedsecondRotation = 10 * (float)Math.sin(LightAnglezero);
		GreenfirstRotation = 10 * (float)Math.cos(LightAngleone);
		GreensecondRotation = 10 * (float)Math.sin(LightAngleone);
		BluefirstRotation = 100 * (float)Math.cos(LightAngletwo);
		BluesecondRotation = 100 * (float)Math.sin(LightAngletwo);
		}
		
	}
	
	
	
	
	private void display()
	{
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		
		RedlightPosition[0] = RedfirstRotation;
		RedlightPosition[2] = RedsecondRotation;

		GreenlightPosition[1] = GreenfirstRotation;
		GreenlightPosition[2] = GreensecondRotation;

		BluelightPosition[1] = BluefirstRotation;
		BluelightPosition[0] = BluesecondRotation;
		
		
		
		//Declaration of Matrix
		float[]modelMatrix = new float[16];
		float[]viewMatrix = new float[16];
		float[]translateMatrix = new float[16];
		float[]rotationMatrix = new float[16];
		float[]rotation2Matrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		
		
		//Intialize above matrix to Identity;
		Matrix.setIdentityM(modelMatrix,0);
		Matrix.setIdentityM(viewMatrix,0);
		Matrix.setIdentityM(translateMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(rotation2Matrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		
		
		//Do necessary transformation
		Matrix.translateM(modelMatrix,0,0.0f, 0.0f, -2.0f);
						
		/*Matrix.setRotateM(rotation2Matrix,0,angle_cube, 1.0f, 0.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 1.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 0.0f,1.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);*/
		
		//Do necessary Matrix multiplication
		//Matrix.multiplyMM(modelMatrix,0,modelMatrix,0,rotationMatrix,0);
		
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
		GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
		GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);
		
		
		
		if (bLight == true)
		{
			GLES32.glUniform1i(LKeyPress_Uniform, 1);
							
			GLES32.glUniform3fv(red_la_Uniform, 1 , RedlightAmbient,0 );
			GLES32.glUniform3fv(red_ld_Uniform, 1, RedlightDiffuse,0);
			GLES32.glUniform3fv(red_ls_Uniform, 1, RedlightSpecular,0);
			GLES32.glUniform4fv(red_lightPosition_Uniform, 1, RedlightPosition,0);

			GLES32.glUniform3fv(green_la_Uniform, 1, GreenlightAmbient,0);
			GLES32.glUniform3fv(green_ld_Uniform, 1, GreenlightDiffuse,0);
			GLES32.glUniform3fv(green_ls_Uniform, 1, GreenlightSpecular,0);
			GLES32.glUniform4fv(green_lightPosition_Uniform, 1, GreenlightPosition,0);

			GLES32.glUniform3fv(blue_la_Uniform, 1, BluelightAmbient,0);
			GLES32.glUniform3fv(blue_ld_Uniform, 1, BluelightDiffuse,0);
			GLES32.glUniform3fv(blue_ls_Uniform, 1, BluelightSpecular,0);
			GLES32.glUniform4fv(blue_lightPosition_Uniform, 1, BluelightPosition,0);

			GLES32.glUniform3fv(ka_Uniform, 1, materialAmbient,0);
			GLES32.glUniform3fv(kd_Uniform, 1, materialDiffuse,0);
			GLES32.glUniform3fv(ks_Uniform, 1, materialSpecular,0);
			GLES32.glUniform1f(materialShinyness_Uniform, materialShinyness);
					
		}
		else
		{
			GLES32.glUniform1i(LKeyPress_Uniform, 0);
		}
		
		
		// bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);

		//unusal program

		GLES32.glUseProgram(0);
		
							
		requestRender();
	}
	
	private void Unintialize()
	{
		// destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }
			
		
		if(shaderProgramObject != 0)
		{
			int[] shaderCount = new int[1];
			int[] shader = new int[shaderCount[0]];
			int shaderNumber ;
			
			GLES32.glUseProgram(shaderProgramObject);
			//ask program How many shader attached with you
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, shaderCount,0);
			
			GLES32.glGetAttachedShaders(shaderProgramObject, shaderCount[0], shaderCount, 0 , shader,0);
			
			for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++)
			{
				//Detached one by one
				GLES32.glDetachShader(shaderProgramObject, shader[shaderNumber]);
				//delete detach shader
				GLES32.glDeleteShader(shader[shaderNumber]);
				shader[shaderNumber] = 0;
				
			}
		
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
			GLES32.glUseProgram(0);
		
		}
	}
	
}



