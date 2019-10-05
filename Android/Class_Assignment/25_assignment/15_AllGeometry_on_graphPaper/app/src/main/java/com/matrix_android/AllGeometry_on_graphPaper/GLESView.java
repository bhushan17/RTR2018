package com.matrix_android.AllGeometry_on_graphPaper;

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
	private int[]vao = new int[1];
	private int[]vbo_position = new int[1];
	private int[]vbo_color = new int[1];
	
	private float[]lineVertices = new float[6];
	private float[]lineColor = new float[6];
	
	private FloatBuffer positionBuffer;
	private FloatBuffer colorBuffer;
	private int mvpUniform;
	private float[]perspectiveProjection = new float[16]; // 4x4 matrix
	
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
		
	}
	
	//our Custom method
	
	private void intialize()
	{
		
		
		// Vertex Shader
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		final String vertexShaderSourceCode = String.format
		("#version 320 es" +
		"\n" +
		"in vec4 vPosition;" +
		"in vec4 vColor;"+
		"uniform mat4 u_mvp_matrix;" +
		"out vec4 out_color;"+
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_color = vColor;"+
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
		"\n"+
		"precision highp float;"+
		"out vec4 fragColor;" +
		"in vec4 out_color;"+
		"void main(void)" +
		"{" +
		"fragColor = out_color;" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_COLOR, "vColor");
		
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
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		
		
		
		//Create vao
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);
		
		//Create and bind vbo_position_pyramid
		GLES32.glGenBuffers(1,vbo_position,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(lineVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		
		
		
		
		
		
		
		
		//Create and bind vbo_color
		GLES32.glGenBuffers(1,vbo_color,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byte2Buffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(lineColor);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineColor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
				
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		
		Matrix.setIdentityM(perspectiveProjection,0);
		
	}
	
	
	
	
	private void resize(int width,int height)
	{
		if(height < 1)
			height = 1;
		
		GLES32.glViewport(0,0,width,height);
		
		
		
		Matrix.perspectiveM(perspectiveProjection,0,45.0f, (float)width / (float)height, 0.1f, 100.0f);
		
		
	}
	
	private void display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		//Declaration of Matrix
		float[]modelViewMatrix = new float[16];
		float[]rotateMatrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		int index;
		float x_coordinate = -1.05f, y_coordinate = -1.05f;
		
		//Intialize above matrix to Identity;
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(rotateMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		//Matrix.setIdentityM(translateMatrix,0);
		
		//Do necessary transformation
		Matrix.translateM(modelViewMatrix,0,0.0f, 0.0f, -2.5f);
						
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelViewMatrix,0,modelViewMatrix,0,rotateMatrix,0);
		Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjection,0,modelViewMatrix,0);
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix,0);
		
		//bind wit vao (This will avoid many repeated binding with vbo)
		GLES32.glBindVertexArray(vao[0]);
		
		
		//Draw triangle by line
		
		float []linec = new float[9];
		linec[0] = 1.0f;
		linec[1] = 1.0f;
		linec[2] = 1.0f;
		linec[3] = 1.0f;
		linec[4] = 1.0f;
		linec[5] = 1.0f;
		linec[6] = 1.0f;
		linec[7] = 1.0f;
		linec[8] = 1.0f;

		float []triangleVertices = new float[9]; 
		triangleVertices[0] = 0.0f;
		triangleVertices[1] = 1.0f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = -(float)Math.sqrt(0.75f);
		triangleVertices[4] = -0.5f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = (float)Math.sqrt(0.75f);
		triangleVertices[7] = -0.5f;
		triangleVertices[8] = 0.0f;
	
	
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(triangleVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
			
		
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		 ByteBuffer byte2Buffer = ByteBuffer.allocateDirect(linec.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(linec);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, linec.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
		GLES32.glLineWidth(2.0f);
		GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3);
		
		//circle dra by line
		int  numpoints = 1000;

		float []circlevertex= new float[6];
		float []circlcolor= new float[]{ 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
		float radius = 0.5f;
		float angle = 0.0f;
		int i = 0, j = 1, k = 2;
		
		//Inner circle
		for (index = 0; index < numpoints; index++)
		{
			angle = (float)((2 * Math.PI *index) / numpoints);
			circlevertex[0] = (float)(radius * Math.cos(angle));
			circlevertex[1] = (float)(radius * Math.sin(angle));
			circlevertex[2] = 0;


			angle = (float)((2 * Math.PI * (index+1)) / numpoints);
			circlevertex[3] = (float)(radius * Math.cos(angle));
			circlevertex[4] = (float)(radius * Math.sin(angle));
			circlevertex[5] = 0;
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			byteBuffer = ByteBuffer.allocateDirect(circlevertex.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byteBuffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			positionBuffer = byteBuffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			positionBuffer.put(circlevertex);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			positionBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circlevertex.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);






			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			 byte2Buffer = ByteBuffer.allocateDirect(circlcolor.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byte2Buffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			colorBuffer = byte2Buffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			colorBuffer.put(circlcolor);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			colorBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circlcolor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glLineWidth(2.0f);
			GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 2);

		}
		
		
		
		//Outer Circle
		for (index = 0; index < numpoints; index++)
		{
			angle = (float)((2 * Math.PI *index) / numpoints);
			circlevertex[0] =  (float)Math.cos(angle);
			circlevertex[1] =  (float)Math.sin(angle);
			circlevertex[2] = 0;


			angle = (float)((2 * Math.PI * (index+1)) / numpoints);
			circlevertex[3] =  (float)Math.cos(angle);
			circlevertex[4] =  (float)Math.sin(angle);
			circlevertex[5] = 0;

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			byteBuffer = ByteBuffer.allocateDirect(circlevertex.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byteBuffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			positionBuffer = byteBuffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			positionBuffer.put(circlevertex);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			positionBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circlevertex.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);






			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			 byte2Buffer = ByteBuffer.allocateDirect(circlcolor.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byte2Buffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			colorBuffer = byte2Buffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			colorBuffer.put(circlcolor);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			colorBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circlcolor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glLineWidth(2.0f);
			GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 2);

		}
		
		//Square by line
	
		float []reccolor=new float[] { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
		

		float []rectvertex= new float[12];
		rectvertex[0] = -(float)Math.sqrt(0.75f);
		rectvertex[1] = 0.5f;
		rectvertex[2] = 0.0f;
		rectvertex[3] = -(float)Math.sqrt(0.75f);
		rectvertex[4] = -0.5f;
		rectvertex[5] = 0.0f;
		rectvertex[6] = (float)Math.sqrt(0.75f);
		rectvertex[7] = -0.5f;
		rectvertex[8] = 0.0f;
		rectvertex[9] = (float)Math.sqrt(0.75f);
		rectvertex[10] = 0.5f;
		rectvertex[11] = 0.0f;


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			byteBuffer = ByteBuffer.allocateDirect(rectvertex.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byteBuffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			positionBuffer = byteBuffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			positionBuffer.put(rectvertex);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			positionBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectvertex.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);






			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
			//Step 1
			//allocate the buffer directly from native memory(not from VM memory)
			 byte2Buffer = ByteBuffer.allocateDirect(reccolor.length * 4);
			
			//Step 2
			//Arrange the Buffer byte order in native byte order
			byte2Buffer.order(ByteOrder.nativeOrder());
			
			//Step 3
			//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
			colorBuffer = byte2Buffer.asFloatBuffer();
			
			//Step 4
			//Now put your arrays in to this coocked buffer
			colorBuffer.put(reccolor);
			
			//Step 5
			//Set the array at there 0th position of Buffer
			colorBuffer.position(0);
			
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, reccolor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glLineWidth(2.0f);
			GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 4);
		
		
		
		
		
		lineVertices[0] = 0.0f;
		lineVertices[1] = 1.0f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.0f;
		lineVertices[4] = -1.0f;
		lineVertices[5] = 0.0f;
		
		lineColor[0] = 0.0f;
		lineColor[1] = 1.0f;
		lineColor[2] = 0.0f;
		lineColor[3] = 0.0f;
		lineColor[4] = 1.0f;
		lineColor[5] = 0.0f;
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(lineVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
			
		
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		 byte2Buffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(lineColor);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineColor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
		GLES32.glLineWidth(2.0f);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
		
		
		lineVertices[0] = -1.0f;
		lineVertices[1] = 0.0f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 1.0f;
		lineVertices[4] = 0.0f;
		lineVertices[5] = 0.0f;
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(lineVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
			
		
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byte2Buffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(lineColor);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineColor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
		GLES32.glLineWidth(2.0f);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
		
		
		lineColor[0] = 0.0f;
		lineColor[1] = 0.3f;
		lineColor[2] = 0.0f;
		lineColor[3] = 0.0f;
		lineColor[4] = 0.3f;
		lineColor[5] = 0.0f;
		
		for (index = 0; index <= 40; index++)
		{
			x_coordinate = x_coordinate + 0.05f;
			y_coordinate = y_coordinate + 0.05f;

			if ((x_coordinate * 100) == 0)
			{

				continue;
			}
			else {
				lineVertices[0] = x_coordinate;
				lineVertices[1] = -1.0f;
				lineVertices[2] = 0.0f;
				lineVertices[3] = x_coordinate;
				lineVertices[4] = 1.0f;
				lineVertices[5] = 0.0f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(lineVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
			
		
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byte2Buffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(lineColor);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineColor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
		GLES32.glLineWidth(2.0f);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		

				lineVertices[0] = -1.0f;
				lineVertices[1] = y_coordinate;
				lineVertices[2] = 0.0f;
				lineVertices[3] = 1.0f;
				lineVertices[4] = y_coordinate;
				lineVertices[5] = 0.0f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(lineVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineVertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
			
		
		
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byte2Buffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byte2Buffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		colorBuffer = byte2Buffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		colorBuffer.put(lineColor);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		colorBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, lineColor.length * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
		GLES32.glLineWidth(2.0f);
		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);


			
			}

		}
		
		
		
		
					
		//unbind vao
		GLES32.glBindVertexArray(0);

		//unusal program

		GLES32.glUseProgram(0);
					
		requestRender();
	}
	
	private void Unintialize()
	{
		if(vbo_position[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position,0);
			vbo_position[0]=0;
		}
		
		if(vao[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vao,0);
			vao[0]=0;
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



