package com.matrix_android.CheckerBoard;

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

//import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;


public class GLESView extends GLSurfaceView implements  OnGestureListener, OnDoubleTapListener,  GLSurfaceView.Renderer
{
    private GestureDetector gestureDetector; 
	private final Context context;
	
	//Java neither have unsignint not gluint
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	//Java doesn't have & operator, we create array of 1 & pass name of array which behave as address.
	private int[]vao_checkerboard = new int[1];
	private int[]vbo_checkerboard_position = new int[1];
	
	
	

	private int[]textureCheckerBoard = new int[1];
	private int[]vbo_texture = new int[1];
		
	private int samplerUniform;
	
		
	private int mvpUniform;
	private float[]perspectiveProjection = new float[16]; // 4x4 matrix
	final int checkerimage_width = 64;
	final int checkerimage_hight = 64;
	
	byte[] checkimage= new byte[checkerimage_hight * checkerimage_width * 4 ];
	
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
	public boolean onDoubleTap(MotionEvent ev)
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
	public boolean onSingleTapConfirmed(MotionEvent ev)
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
	public void onLongPress(MotionEvent ev)
	{
		
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
		"in vec4 vPosition;" +
		"in vec2 vTexquard;"+
		
		"uniform mat4 u_mvp_matrix;" +
		"out vec2 out_texquard;"+
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_texquard = vTexquard;"+
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
		"in vec2 out_texquard;"+
		"uniform sampler2D u_sampler;"+
		"void main(void)" +
		"{" +
		"fragColor = texture(u_sampler,out_texquard);" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, "vTexquard");
		
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
		samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_sampler");
		
		//triangle position vertices
		//Array declarartion & inline intialize so no need to give array size
		final float[]rectangle_textquard = new float[]
		{ 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f };
		
		
		
				
			
		
		//Create vao_checkerboard
		GLES32.glGenVertexArrays(1, vao_checkerboard, 0);
		GLES32.glBindVertexArray(vao_checkerboard[0]);
		
		//Create and bind vbo_checkerboard_position
		GLES32.glGenBuffers(1,vbo_checkerboard_position,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_checkerboard_position[0]);
		
		
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 12 * 4, null, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		
		
		
		
		
		
		
		
		//Create and bind vbo_texture
		GLES32.glGenBuffers(1,vbo_texture,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangle_textquard.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(rectangle_textquard);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangle_textquard.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
			
		
		textureCheckerBoard[0]=loadTexture();
		
		
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		
		
		Matrix.setIdentityM(perspectiveProjection,0);
		
	}
	
	private void makecheckImage()
	{
		int i=0,j=0,c=0;
		
		for (i = 0; i < checkerimage_hight; i++)
		{
			for (j = 0; j < checkerimage_width; j++)
			{
			
			c=((i&8)^(j&8))*255;
			checkimage[(i*64+j)*4+0]=(byte)c;
			checkimage[(i*64+j)*4+1]=(byte)c;
			checkimage[(i*64+j)*4+2]=(byte)c;
			checkimage[(i*64+j)*4+3]=(byte)0xFF;
			}
		}
		
		
	}
	private int loadTexture()
	{
		makecheckImage();
		
		int []texture= new int[1];
		
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect( checkimage.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		
		
		
		byteBuffer.put(checkimage);
		byteBuffer.position(0);
		Bitmap bitmap = Bitmap.createBitmap(checkerimage_width,checkerimage_hight,Bitmap.Config.ARGB_8888);
		
		bitmap.copyPixelsFromBuffer(byteBuffer);
		//GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
		GLES32.glGenTextures(1, texture,0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
		//GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		//GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);
		
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0 );
		
		
		return texture[0];
		
	}
	
	
	
	private void resize(int width,int height)
	{
		if(height < 1)
			height = 1;
		
		GLES32.glViewport(0,0,width,height);
		
		
		
		Matrix.perspectiveM(perspectiveProjection,0,45.0f, (float)width / (float)height, 0.1f, 100.0f);
		
		
	}
	private void update()
	{
		
	}
	private void display()
	{
		
		float rectangle_vertices[] = { -2.0f, -1.0f, 0.0f,
											-2.0f, 1.0f, 0.0f,
											0.0f, 1.0f, 0.0f,
											0.0f, -1.0f, 0.0f };
											
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		//Declaration of Matrix
		float[]modelViewMatrix = new float[16];
		float[]rotateMatrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		
		
		//Intialize above matrix to Identity;
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(rotateMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		//Matrix.setIdentityM(translateMatrix,0);
		
		//Do necessary transformation
		Matrix.translateM(modelViewMatrix,0,0.0f, 0.0f, -3.0f);
		
		
		
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelViewMatrix,0,modelViewMatrix,0,rotateMatrix,0);
		Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjection,0,modelViewMatrix,0);
		
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureCheckerBoard[0]);
		GLES32.glUniform1i(samplerUniform, 0);
		
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix,0);
		
		//bind wit vao (This will avoid many repeated binding with vbo)
		GLES32.glBindVertexArray(vao_checkerboard[0]);
		
		
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangle_vertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(rectangle_vertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_checkerboard_position[0]);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangle_vertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		
		//Draw necessary seen
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		
		
		rectangle_vertices[0] = 1.0f ;
		rectangle_vertices[1] = -1.0f ;
		rectangle_vertices[2] =  0.0f;
		rectangle_vertices[3] = 1.0f;
		rectangle_vertices[4] = 1.0f;
		rectangle_vertices[5] = 0.0f;
		rectangle_vertices[6] = 2.41421f;
		rectangle_vertices[7] = 1.0f;
		rectangle_vertices[8] = -1.41421f;
		rectangle_vertices[9] = 2.41421f;
		rectangle_vertices[10] = -1.0f;
		rectangle_vertices[11] = -1.41421f;
		
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(rectangle_vertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_checkerboard_position[0]);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangle_vertices.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		
		
		
		//Draw necessary seen
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		
		//unbind vao
		GLES32.glBindVertexArray(0);

		//unusal program

		GLES32.glUseProgram(0);
					
		requestRender();
	}
	
	private void Unintialize()
	{
		if(vbo_checkerboard_position[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_checkerboard_position,0);
			vbo_checkerboard_position[0]=0;
		}
		
		if(vao_checkerboard[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vao_checkerboard,0);
			vao_checkerboard[0]=0;
		}
		
		if(vbo_texture[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texture,0);
			vbo_texture[0]=0;
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



