package com.matrix_android.Cube_with_Light;

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


public class GLESView extends GLSurfaceView implements  OnGestureListener, OnDoubleTapListener,  GLSurfaceView.Renderer
{
    private GestureDetector gestureDetector; 
	private final Context context;
	
	//Java neither have unsignint not gluint
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	//Java doesn't have & operator, we create array of 1 & pass name of array which behave as address.
	private int[]vao_cube = new int[1];
	private int[]vbo_position_cube = new int[1];
	private int[]vbo_normal_cube = new int[1];
	
		
	private FloatBuffer positionBuffer;
	private FloatBuffer normalBuffer;
	private int mvUniform, pUniform, ldUniform,kdUniform, lightPositionUniform, LKeyPress;;
	private float[]perspectiveProjectionMatrix = new float[16]; // 4x4 matrix
	
		
	private float angle_cube = 0.0f;
	private boolean bLight = false;
	private boolean animation = false;
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
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_mv_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_Lkeypress;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec4 u_light_position;" +
		"out vec3 diffusecolor;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_mv_matrix * vPosition;" +
		"mat3 normalMatrix = mat3(transpose(inverse(u_mv_matrix)));"+
		"vec3 Tnormal=normalize(normalMatrix * vNormal);" +
		"vec3 S =normalize((vec3(u_light_position) - eye_coordinate.xyz));" +
		"diffusecolor = u_ld * u_kd * dot(S,Tnormal);" +
		"}" +
		"gl_Position = u_projection_matrix * u_mv_matrix * vPosition;" +
		
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
		"in vec3 diffusecolor;" +
		"uniform int u_Lkeypress;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"fragColor = vec4(diffusecolor,1.0);" +
		"}" +
		"else" +
		"{" +
		"fragColor = vec4(1.0f,1.0f,1.0f,1.0f);" +
		"}" +
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
		mvUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mv_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");
		LKeyPress = GLES32.glGetUniformLocation(shaderProgramObject, "u_Lkeypress");
		
			
		final float cubeVertices[] = { 
										1.0f, 1.0f, -1.0f,
										-1.0f, 1.0f, -1.0f,
										-1.0f, 1.0f, 1.0f,
										1.0f, 1.0f, 1.0f,
										
										1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										
										1.0f, 1.0f, 1.0f,
										-1.0f, 1.0f, 1.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										
										1.0f, 1.0f, -1.0f,
										-1.0f, 1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
										1.0f, -1.0f, -1.0f,
										
										1.0f, 1.0f, -1.0f,
										1.0f, 1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,
										
										-1.0f, 1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, 1.0f,
										-1.0f, 1.0f, 1.0f };

	final float cubeNormals[] = { 0.0f, 1.0f, 0.0f,
										0.0f, 1.0f, 0.0f,
										0.0f, 1.0f, 0.0f,
										0.0f, 1.0f, 0.0f,

										0.0f,-1.0f,0.0f,
										0.0f,-1.0f,0.0f,
										0.0f,-1.0f,0.0f,
										0.0f,-1.0f,0.0f,

										0.0f,0.0f,1.0f,
										0.0f,0.0f,1.0f,
										0.0f,0.0f,1.0f,
										0.0f,0.0f,1.0f,

										0.0f,0.0f,-1.0f,
										0.0f,0.0f,-1.0f,
										0.0f,0.0f,-1.0f,
										0.0f,0.0f,-1.0f,

										1.0f,0.0f,0.0f,
										1.0f,0.0f,0.0f,
										1.0f,0.0f,0.0f,
										1.0f,0.0f,0.0f,

										-1.0f,0.0f,0.0f,
										-1.0f,0.0f,0.0f,
										-1.0f,0.0f,0.0f,
										-1.0f,0.0f,0.0f };

	
		//Create vao_cube
		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);
		
		//Create and bind vbo_position_pyramid
		GLES32.glGenBuffers(1,vbo_position_cube,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(cubeVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
				
		//Create and bind vbo_normal_cube
		GLES32.glGenBuffers(1,vbo_normal_cube,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_cube[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byte2Buffer = ByteBuffer.allocateDirect(cubeNormals.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		normalBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		normalBuffer.put(cubeNormals);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		normalBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeNormals.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
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
		float[]modelViewMatrix = new float[16];
		float[]translateMatrix = new float[16];
		float[]rotationMatrix = new float[16];
		float[]rotation2Matrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		
		
		//Intialize above matrix to Identity;
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(translateMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(rotation2Matrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		
		
		//Do necessary transformation
		Matrix.translateM(modelViewMatrix,0,0.0f, 0.0f, -6.0f);
						
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 1.0f, 0.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 1.0f,0.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		Matrix.setRotateM(rotation2Matrix,0,angle_cube, 0.0f, 0.0f,1.0f);
		Matrix.multiplyMM(rotationMatrix,0,rotation2Matrix,0,rotationMatrix,0);
		
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelViewMatrix,0,modelViewMatrix,0,rotationMatrix,0);
		
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(mvUniform, 1, false, modelViewMatrix,0);
		GLES32.glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix,0);
		
		
		
		if (bLight == true)
		{
			GLES32.glUniform1i(LKeyPress, 1);
			GLES32.glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f); // White color
			GLES32.glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f); // material property gray
			GLES32.glUniform4f(lightPositionUniform, 0.0f, 0.0f, -1.0f,1.0f);
		}
		else
		{
			GLES32.glUniform1i(LKeyPress, 0);
		}
		
		
		//bind wit vao_cube (This will avoid many repeated binding with vbo)
		GLES32.glBindVertexArray(vao_cube[0]);
				
			//Draw necessary seen
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);		
		
							
		//unbind vao_cube
		GLES32.glBindVertexArray(0);

		//unusal program

		GLES32.glUseProgram(0);
		
							
		requestRender();
	}
	
	private void Unintialize()
	{
		if(vbo_position_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_cube,0);
			vbo_position_cube[0]=0;
		}
		
		if(vao_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vao_cube,0);
			vao_cube[0]=0;
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



