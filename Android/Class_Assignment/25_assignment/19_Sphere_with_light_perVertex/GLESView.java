package com.matrix_android.Sphere_with_light_perVertex;

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
	
	
		
	float lightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	float lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	float lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };


	float materialAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
	float materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	float materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	float materialShinyness = 128.0f;
	
	
	
	
	private float[]perspectiveProjectionMatrix = new float[16]; // 4x4 matrix
	
		
	private float angle_cube = 0.0f;
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
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec4 u_lightPosition;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_materialShinyness;" +
		"out vec3 phong_ads_light;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"vec3 Transformednormal=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
		"vec3 lightDirection =normalize(vec3(u_lightPosition - eye_coordinate));" +
		"float TN_dot_ld = max(dot(lightDirection,Transformednormal),0.0f);" +
		"vec3 reflection_vector = reflect(-lightDirection,Transformednormal);" +
		"vec3 viewer_vector = normalize(vec3(-eye_coordinate.xyz));" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 diffuse = u_ld * u_kd * TN_dot_ld;" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector , viewer_vector ),0.0f),u_materialShinyness);" +
		"phong_ads_light = ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" +
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
		"\n" +
		"out vec4 fragColor;" +
		"in vec3 phong_ads_light;" +
		"void main(void)" +
		"{" +
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
			if (angle_cube > 360)
			{

				angle_cube = 0.0f;
			}
			else {

				angle_cube = angle_cube + 1.0f;
			}
		}
		
	}
	
	
	
	
	private void display()
	{
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
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
						
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 1.0f, 0.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 1.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 0.0f,1.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelMatrix,0,modelMatrix,0,rotationMatrix,0);
		
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(modelMatrix_Uniform, 1, false, modelMatrix,0);
		GLES32.glUniformMatrix4fv(viewMatrix_Uniform, 1, false, viewMatrix,0);
		GLES32.glUniformMatrix4fv(projection_Uniform, 1, false, perspectiveProjectionMatrix,0);
		
		
		
		if (bLight == true)
		{
			GLES32.glUniform1i(LKeyPress_Uniform, 1);
							
			GLES32.glUniform3fv(la_Uniform, 1 , lightAmbient,0 );
			GLES32.glUniform3fv(ld_Uniform, 1, lightDiffuse,0);
			GLES32.glUniform3fv(ls_Uniform, 1, lightSpecular,0);
			GLES32.glUniform4fv(lightPosition_Uniform, 1, lightPosition,0);

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



