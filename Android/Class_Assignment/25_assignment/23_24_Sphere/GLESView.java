package com.matrix_android.Sphere24;

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
	private int ld_Uniform, la_Uniform, ls_Uniform, lightPosition_Uniform;
	private int ka_Uniform, kd_Uniform, ks_Uniform;
	private int LKeyPress_Uniform, materialShinyness_Uniform;
	
	private float rotatae = 0.05f;
		
	float lightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	float lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	float lightPosition[] = { 0.0f,0.0f,0.0f,1.0f };


	float []MaterialAmbient= new float[4];// = { 0.0f,0.0f,0.0f,0.0f };
	float []MaterialDiffuse= new float[4];// = { 1.0f,1.0f,1.0f,1.0f };
	float []MaterialSpecular= new float[4];// = { 1.0f,1.0f,1.0f,1.0f };
	float MaterialShianyness = 128.0f;
	
	
	
	
	private float[]perspectiveProjectionMatrix = new float[16]; // 4x4 matrix
	
		
	private float LightAngle = 0.0f;
	private boolean bLight = false;
	private boolean animation = false;
	
	private float firstRotation = 0.0f, secondRotation = 0.0f;
    private boolean xrotation = false, yrotation = false, zrotation = false;
	
	private int numVertices;
    private int numElements;
	private int gheight;
	private int gwidth;
	
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
		xrotation = false;
				yrotation = false;
				zrotation = true;
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
		final int x1 = (int) e1.getX();
        final int y1 = (int) e1.getY();
		final int x2 = (int) e2.getX();
        final int y2 = (int) e2.getY();
		
		float diffY = e2.getY() - e1.getY();
		float diffX = e2.getX() - e1.getX();
		
		if(Math.abs(diffX) > Math.abs(diffY))
		{   //right or left swipe
	
			
			if(Math.abs(diffX) > 5 && Math.abs(velocityX) > 5)
			{
				xrotation = false;
				yrotation = true;
				zrotation = false;
					
				if(diffX > 0)
				{
					//onswipe right
					if(rotatae > 0)
						rotatae = -1.0f * rotatae;
					
				}
				else
				{
					if(rotatae < 0)
						rotatae = -1.0f * rotatae;
						
					//onSwipe Left
					
				}
			 
			}
			
		}
		
		else
		{ 	// up or down swipe
	
			
			
			if(Math.abs(diffY) > 5 && Math.abs(velocityY) > 5)
			{
				xrotation = true;
				yrotation = false;
				zrotation = false;
					
			 if(diffY > 0)
				{
					if(rotatae < 0)
						rotatae = -1.0f * rotatae;
					//onswipe bottom
					
				}
				else
				{
					//onSwipe top
					if(rotatae > 0)
						rotatae = -1.0f * rotatae;
				}
			}
			
		}
		
		return(true);
	}
	
	private void update()
	{
		if (animation == true)
		{
			
		}
		
		LightAngle = LightAngle + rotatae;
		firstRotation = 2 * (float)Math.cos(LightAngle);
		secondRotation =  2 * (float)Math.sin(LightAngle);
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public void onLongPress(MotionEvent e)
	{
		//setGravity(Gravity.RIGHT);
		Unintialize();
		System.exit(0);
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		//setGravity(Gravity.BOTTOM);
		
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
		"uniform int u_Lkeypress;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform vec4 u_lightPosition;" +
		"out vec3 Transformednormal;" +
		"out vec3 lightDirection;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"Transformednormal=mat3(u_view_matrix * u_model_matrix) * vNormal;" +
		"lightDirection = vec3(u_lightPosition - eye_coordinate);" +
		"viewer_vector =vec3(-eye_coordinate.xyz);" +
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
		"uniform int u_Lkeypress;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_materialShinyness;" +
		"vec3 phong_ads_light;" +
		"in vec3 Transformednormal;" +
		"in vec3 lightDirection;" +
		"in vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);" +
		"vec3 NormalizelightDirection = normalize(lightDirection);" +
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);" +
		"float TN_dot_ld = max(dot(NormalizelightDirection,NormalizeTransformednormal),0.0f);" +
		"vec3 reflection_vector = reflect(-NormalizelightDirection,NormalizeTransformednormal);" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 diffuse = u_ld * u_kd * TN_dot_ld;" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
		"phong_ads_light = ambient + diffuse + specular;" +
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
		
		la_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
		ld_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		ls_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
		lightPosition_Uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightPosition");
		
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
		
		GLES32.glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
		
	}
	
	
	
	
	private void resize(int width,int height)
	{
		if(height < 1)
			height = 1;
		
		GLES32.glViewport(0,0,width,height);
		
		
		
		Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f, (float)width / (float)height, 0.1f, 100.0f);
		gheight = height;
		gwidth = width;
		
	}
	
		
	
	
	

void draw24Sphere() {

	// Material Configuration
	//Declaration of Matrix
		float[]modelMatrix = new float[16];
		float[]viewMatrix = new float[16];
		float[]translateMatrix = new float[16];
		float[]scaleMatrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		
	//Intialize above matrix to Identity;
	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);
	
	
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
		lightPosition[2] = -1.0f;
	}
	else
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
	}

	if (bLight == true)
	{
		GLES32.glUniform1i(LKeyPress_Uniform, 1);
		GLES32.glUniform3fv(la_Uniform, 1, lightAmbient,0);
		GLES32.glUniform3fv(ld_Uniform, 1, lightDiffuse,0);
		GLES32.glUniform3fv(ls_Uniform, 1, lightSpecular,0);
		GLES32.glUniform4fv(lightPosition_Uniform, 1, lightPosition,0);

	}
	else
	{
		GLES32.glUniform1i(LKeyPress_Uniform, 0);
	}
	
	

	//************* 1st Sphere on 1st column, emerald **************//

	

	MaterialAmbient[0] = 0.0215f;
	MaterialAmbient[1] = 0.1745f;
	MaterialAmbient[2] = 0.0215f;
	MaterialAmbient[3] = 1.0f;


	MaterialDiffuse[0] = 0.07568f;
	MaterialDiffuse[1] = 0.61424f;
	MaterialDiffuse[2] = 0.07568f;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.633f;
	MaterialSpecular[1] = 0.727811f;
	MaterialSpecular[2] = 0.633f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-4.5f, 2.3f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);
	
	GLES32.glViewport(0,(5*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen 
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	
	//************* 2nd Sphere on 1st column, jade **************//
	
	MaterialAmbient[0] = 0.135f;
	MaterialAmbient[1] = 0.2225f;
	MaterialAmbient[2] = 0.1575f;
	MaterialAmbient[3] = 1.0f;


	MaterialDiffuse[0] = 0.54f;
	MaterialDiffuse[1] = 0.89f;
	MaterialDiffuse[2] = 0.63f;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.316228f;
	MaterialSpecular[1] = 0.316228f;
	MaterialSpecular[2] = 0.316228f;
	MaterialSpecular[3] = 1.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6f * 128.0f;


	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-4.5f, 1.5f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport(0,(4*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 3rd Sphere on 1st column, obsidian **************//

	MaterialAmbient[0] = 0.05375f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.06625f;
	MaterialAmbient[3] = 1.0f;


	MaterialDiffuse[0] = 0.18275f;
	MaterialDiffuse[1] = 0.17f;
	MaterialDiffuse[2] = 0.22525f;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.332741f;
	MaterialSpecular[1] = 0.328634f;
	MaterialSpecular[2] = 0.346435f;
	MaterialSpecular[3] = 1.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6f * 128.0f;


	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-4.5f, 0.7f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		

	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport(0,(3*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 4th Sphere on 1st column, pearl **************//

	MaterialAmbient[0] = 0.25f;
	MaterialAmbient[1] = 0.20725f;
	MaterialAmbient[2] = 0.20725f;
	MaterialAmbient[3] = 1.0f;


	MaterialDiffuse[0] = 1.0f;
	MaterialDiffuse[1] = 0.829f;
	MaterialDiffuse[2] = 0.829f;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.296648f;
	MaterialSpecular[1] = 0.296648f;
	MaterialSpecular[2] = 0.296648f;
	MaterialSpecular[3] = 1.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6f * 128.0f;


	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	
	//Matrix.translateM(translateMatrix,0,-4.5f, -0.1f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport(0,(2*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 5th Sphere on 1st column, ruby **************//

	

	MaterialAmbient[0] = 0.1745f;
	MaterialAmbient[1] = 0.01175f;
	MaterialAmbient[2] = 0.01175f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.61424f;
	MaterialDiffuse[1] = 0.04136f;
	MaterialDiffuse[2] = 0.04136f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.727811f;
	MaterialSpecular[1] = 0.626959f;
	MaterialSpecular[2] = 0.626959f;
	MaterialSpecular[3] = 1.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6f * 128.0f;


	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-4.5f, -1.2f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport(0,(1*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 6th Sphere on 1st column, turquoise **************//

	

	MaterialAmbient[0] = 0.1f;
	MaterialAmbient[1] = 0.18725f;
	MaterialAmbient[2] = 0.1745f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.396f;
	MaterialDiffuse[1] = 0.74151f;
	MaterialDiffuse[2] = 0.69102f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.297254f;
	MaterialSpecular[1] = 0.30829f;
	MaterialSpecular[2] = 0.306678f;
	MaterialSpecular[3] = 1.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6f * 128.0f;


	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-4.5f, -2.3f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport(0,0,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);




	
	//************* 1st Sphere on 2nd column, brass **************//

	

	MaterialAmbient[0] = 0.329412f;
	MaterialAmbient[1] = 0.223529f;
	MaterialAmbient[2] = 0.027451f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.780392f;
	MaterialDiffuse[1] = 0.568627f;
	MaterialDiffuse[2] = 0.113725f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.992157f;
	MaterialSpecular[1] = 0.941176f;
	MaterialSpecular[2] = 0.807843f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, 1.0f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);


    GLES32.glViewport((1*gwidth)/4,(5*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 2nd Sphere on 2nd column, bronze **************//


	MaterialAmbient[0] = 0.2125f;
	MaterialAmbient[1] = 0.1275f;
	MaterialAmbient[2] = 0.054f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.714f;
	MaterialDiffuse[1] = 0.4284f;
	MaterialDiffuse[2] = 0.18144f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.393548f;
	MaterialSpecular[1] = 0.271906f;
	MaterialSpecular[2] = 0.166721f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, 0.6f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	

	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);
	
	GLES32.glViewport((1*gwidth)/4,(4*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 3rd Sphere on 2nd column, chrome **************//


	MaterialAmbient[0] = 0.25f;
	MaterialAmbient[1] = 0.25f;
	MaterialAmbient[2] = 0.25f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.4f;
	MaterialDiffuse[1] = 0.4f;
	MaterialDiffuse[2] = 0.4f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.774597f;
	MaterialSpecular[1] = 0.774597f;
	MaterialSpecular[2] = 0.774597f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, 0.2f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);


	GLES32.glViewport((1*gwidth)/4,(3*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 4th Sphere on 2nd column, pearl **************//

	

	MaterialAmbient[0] = 0.19125f;
	MaterialAmbient[1] = 0.0735f;
	MaterialAmbient[2] = 0.0225f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.7038f;
	MaterialDiffuse[1] = 0.27048f;
	MaterialDiffuse[2] = 0.0828f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.256777f;
	MaterialSpecular[1] = 0.137622f;
	MaterialSpecular[2] = 0.086014f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, -0.2f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((1*gwidth)/4,(2*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 5th Sphere on 2nd column, ruby **************//

	

	MaterialAmbient[0] = 0.24725f;
	MaterialAmbient[1] = 0.1995f;
	MaterialAmbient[2] = 0.0745f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.75164f;
	MaterialDiffuse[1] = 0.60648f;
	MaterialDiffuse[2] = 0.22648f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.628281f;
	MaterialSpecular[1] = 0.555802f;
	MaterialSpecular[2] = 0.366065f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, -0.6f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((1*gwidth)/4,(1*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 6th Sphere on 2nd column, silver **************//



	MaterialAmbient[0] = 0.19225f;
	MaterialAmbient[1] = 0.19225f;
	MaterialAmbient[2] = 0.19225f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.50754f;
	MaterialDiffuse[1] = 0.50754f;
	MaterialDiffuse[2] = 0.50754f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.508273f;
	MaterialSpecular[1] = 0.508273f;
	MaterialSpecular[2] = 0.508273f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,-0.5f, -1.0f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((1*gwidth)/4,0,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);






	
	//************* 1st Sphere on 3nd column, black **************//


	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.01f;
	MaterialDiffuse[1] = 0.01f;
	MaterialDiffuse[2] = 0.01f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.5f;
	MaterialSpecular[1] = 0.5f;
	MaterialSpecular[2] = 0.5f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	//Matrix.translateM(translateMatrix,0,0.5f, 1.0f, -7.0f);
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,(5*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 2nd Sphere on 3nd column, cyan **************//

	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.1f;
	MaterialAmbient[2] = 0.06f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.0f;
	MaterialDiffuse[1] = 0.50980392f;
	MaterialDiffuse[2] = 0.50980392f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.50196078f;
	MaterialSpecular[1] = 0.50196078f;
	MaterialSpecular[2] = 0.50196078f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,(4*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 3rd Sphere on 3nd column, green **************//


	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.1f;
	MaterialDiffuse[1] = 0.35f;
	MaterialDiffuse[2] = 0.1f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.45f;
	MaterialSpecular[1] = 0.55f;
	MaterialSpecular[2] = 0.45f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,(3*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);





	//************* 4th Sphere on 3nd column, red **************//


	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.5f;
	MaterialDiffuse[1] = 0.0f;
	MaterialDiffuse[2] = 0.0f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.7f;
	MaterialSpecular[1] = 0.6f;
	MaterialSpecular[2] = 0.6f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,(2*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 5th Sphere on 3nd column, white **************//


	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.55f;
	MaterialDiffuse[1] = 0.55f;
	MaterialDiffuse[2] = 0.55f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.70f;
	MaterialSpecular[1] = 0.70f;
	MaterialSpecular[2] = 0.70f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	

	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,(1*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 6th Sphere on 3nd column, plastic **************//


	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.5f;
	MaterialDiffuse[1] = 0.5f;
	MaterialDiffuse[2] = 0.0f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.6f;
	MaterialSpecular[1] = 0.6f;
	MaterialSpecular[2] = 0.5f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((2*gwidth)/4,0,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);







	//************* 1st Sphere on 4th column, black **************//


	
	MaterialAmbient[0] = 0.02f;
	MaterialAmbient[1] = 0.02f;
	MaterialAmbient[2] = 0.02f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.01f;
	MaterialDiffuse[1] = 0.01f;
	MaterialDiffuse[2] = 0.01f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.4f;
	MaterialSpecular[1] = 0.4f;
	MaterialSpecular[2] = 0.4f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,(5*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);



	//************* 2nd Sphere on 4th column, cyan **************//
	
	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.05f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.4f;
	MaterialDiffuse[1] = 0.5f;
	MaterialDiffuse[2] = 0.5f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.04f;
	MaterialSpecular[1] = 0.7f;
	MaterialSpecular[2] = 0.7f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,(4*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);




	//************* 3rd Sphere on 4th column, green **************//


	MaterialAmbient[0] = 0.0f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.4f;
	MaterialDiffuse[1] = 0.5f;
	MaterialDiffuse[2] = 0.4f;
	MaterialDiffuse[3] = 1.0f;

	MaterialSpecular[0] = 0.04f;
	MaterialSpecular[1] = 0.7f;
	MaterialSpecular[2] = 0.04f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,(3*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);


	//************* 4th Sphere on 4th column, red **************//

	
	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.0f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.5f;
	MaterialDiffuse[1] = 0.4f;
	MaterialDiffuse[2] = 0.4f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.7f;
	MaterialSpecular[1] = 0.04f;
	MaterialSpecular[2] = 0.04f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,(2*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);







	//************* 5th Sphere on 4th column, white **************//
	
	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.05f;
	MaterialAmbient[3] = 1.0f;
	
	MaterialDiffuse[0] = 0.5f;
	MaterialDiffuse[1] = 0.5f;
	MaterialDiffuse[2] = 0.5f;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.7f;
	MaterialSpecular[1] = 0.7f;
	MaterialSpecular[2] = 0.7f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
		
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,(1*gheight)/6,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);




	//************* 6th Sphere on 4th column, plastic **************//

	MaterialAmbient[0] = 0.05f;
	MaterialAmbient[1] = 0.05f;
	MaterialAmbient[2] = 0.0f;
	MaterialAmbient[3] = 1.0f;
	

	MaterialDiffuse[0] = 0.5f;
	MaterialDiffuse[1] = 0.5f;
	MaterialDiffuse[2] = 0.4f;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.7f;
	MaterialSpecular[1] = 0.7f;
	MaterialSpecular[2] = 0.04f;
	MaterialSpecular[3] = 1.0f;


	MaterialShianyness = 0.6f * 128.0f;

	GLES32.glUniform3fv(ka_Uniform, 1, MaterialAmbient,0);
	GLES32.glUniform3fv(kd_Uniform, 1, MaterialDiffuse,0);
	GLES32.glUniform3fv(ks_Uniform, 1, MaterialSpecular,0);
	GLES32.glUniform1f(materialShinyness_Uniform, MaterialShianyness);
	
	Matrix.setIdentityM(modelMatrix,0);
	Matrix.setIdentityM(viewMatrix,0);
	Matrix.setIdentityM(translateMatrix,0);
	Matrix.setIdentityM(scaleMatrix,0);
	Matrix.setIdentityM(modelViewProjectionMatrix,0);

	//Do necessary Matrix multiplication
	Matrix.translateM(translateMatrix,0,0.0f, 0.0f, -2.0f);
	Matrix.scaleM(scaleMatrix,0,0.3f, 0.3f, 0.3f);
	
	Matrix.multiplyMM(modelMatrix,0,translateMatrix,0,modelMatrix,0);
	Matrix.multiplyMM(modelMatrix,0,scaleMatrix,0,modelMatrix,0);
	
	//Send necessary Matrix to shader in respective uniform
	GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
	GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
	GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);

	GLES32.glViewport((3*gwidth)/4,0,gwidth/4,gheight/6);
	GLES32.glBindVertexArray(vao_sphere[0]);
	//Draw necessary seen
	GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
	GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
	//unbind vao
	GLES32.glBindVertexArray(0);




}


	
	
	
	
	private void display()
	{
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		draw24Sphere();
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



