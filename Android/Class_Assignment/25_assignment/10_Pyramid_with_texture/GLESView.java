package com.matrix_android.Pyramid_with_texture;

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

import android.graphics.BitmapFactory;
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
	private int[]vao_pyramid = new int[1];
	private int[]vao_cube = new int[1];
	private int[]vbo_position_pyramid = new int[1];
	private int[]vbo_position_cube = new int[1];
	
	

	private int[]texturePyramid = new int[1];
	private int[]vbo_texture_pyramid = new int[1];
	
	private int[]textureCube = new int[1];
	private int[]vbo_texture_cube = new int[1];
	
	private int samplerUniform;
	
	private float angle_pyramid = 0.0f;
	private float  angle_cube = 0.0f;
	private float pyramidspeed = 0.0f;
	private float cubespeed = 0.0f;
	
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
	public boolean onDoubleTap(MotionEvent ev)
	{
		//setGravity(Gravity.LEFT);
		final int x = (int) ev.getX();
        final int y = (int) ev.getY();
		
		if((x > 1300) && (x < 1830)&&(y > 300) && (y < 790))
		{
			cubespeed = cubespeed + 2.0f;
		}
		//cubespeed = cubespeed + 5.0f;
		
		 
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
		
		final int x = (int) ev.getX();
        final int y = (int) ev.getY();
		
		if((x > 500) && (x < 750)&&(y > 500) && (y < 750))
		{
			pyramidspeed = pyramidspeed + 3.0f;
		}
		//System.out.println("RTR: doubletap x = " + x);
		//System.out.println("RTR: doubletap y = " + y);
		
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
		final int x = (int) ev.getX();
        final int y = (int) ev.getY();
		//setGravity(Gravity.RIGHT);
		if((x > 1300) && (x < 1830)&&(y > 300) && (y < 790))
		{
			cubespeed = 0.0f;
		}
		if((x > 500) && (x < 750)&&(y > 500) && (y < 750))
		{
			pyramidspeed = 0.0f;
		}
	}
	
	//abstract method from OnGestureListner so must be implemented
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		//setGravity(Gravity.BOTTOM);
		final int x1 = (int) e1.getX();
        final int y1 = (int) e1.getY();
		final int x2 = (int) e2.getX();
        final int y2 = (int) e2.getY();
		
		
		if((x1 > 400) && (x1 < 950)&&(y1 > 500) && (y1 < 750))
		{
			if((y1 > y2))
			{
				pyramidspeed = pyramidspeed + 0.1f;
			}
			else if((y2 > y1))
			{
				pyramidspeed = pyramidspeed - 0.1f;
			}
		}
		else if((x1 > 1300) && (x1 < 1830)&&(y1 > 300) && (y1 < 790))
		{
			if((y1 > y2))
			{
				cubespeed = cubespeed + 0.1f;
			}
			else if((y2 > y1))
			{
				cubespeed = cubespeed - 0.1f;
			}
		}
		else{
			Unintialize();
			System.exit(0);
		}
		
		if(cubespeed < 0.0f)
			cubespeed = 0.0f;
		
		if(pyramidspeed < 0.0f)
			pyramidspeed = 0.0f;
		
		
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
		final float[]pyramidVertices = new float[]
		{ 0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										0.0f, 1.0f, 0.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,
										0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, -1.0f,
										1.0f, -1.0f, -1.0f,
										0.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										-1.0f, -1.0f, -1.0f	};
		
		
		final float[]pyramidTexCord = new float[]
		{ 0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f ,
		0.5f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f };
				
		
		
		final float[]cubeVertices = new float[]
		{ 1.0f, 1.0f, -1.0f,
										-1.0f, 1.0f, -1.0f,
										-1.0f, 1.0f, 1.0f,
										1.0f, 1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										-1.0f, -1.0f, 1.0f,
										-1.0f, -1.0f, -1.0f,
										1.0f, -1.0f, -1.0f,
										1.0f, 1.0f, 1.0f,
										-1.0f, 1.0f, 1.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, 1.0f, -1.0f,
										1.0f, 1.0f, -1.0f,
										1.0f, 1.0f, -1.0f,
										1.0f, 1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,
										-1.0f, 1.0f, 1.0f,
										-1.0f, 1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, 1.0f	};
		
		
		final float[]cubeTextCord = new float[]
		{ 0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f };
		
		
		//Create vao_pyramid
		GLES32.glGenVertexArrays(1, vao_pyramid, 0);
		GLES32.glBindVertexArray(vao_pyramid[0]);
		
		//Create and bind vbo_position_pyramid
		GLES32.glGenBuffers(1,vbo_position_pyramid,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_pyramid[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(pyramidVertices);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		
		
		
		
		
		
		
		
		//Create and bind vbo_texture_pyramid
		GLES32.glGenBuffers(1,vbo_texture_pyramid,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture_pyramid[0]);
		
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(pyramidTexCord.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(pyramidTexCord);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidTexCord.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		GLES32.glBindVertexArray(0);
		
		
			
		
		texturePyramid[0]=loadTexture(R.raw.stone);
		
		
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		
		
		Matrix.setIdentityM(perspectiveProjection,0);
		
	}
	
	private int loadTexture(int imageFileResourceId)
	{
		int []texture= new int[1];
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inScaled = false;
		
		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(),imageFileResourceId, options);
		
		//GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
		GLES32.glGenTextures(1, texture,0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		
		//GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		//GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);
		
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0 );
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
		
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
		if (angle_pyramid > 360)
		{
			angle_pyramid = 0.0f;
			//angle_rectangle = 0.0f;
		}
		else {
			angle_pyramid = angle_pyramid + pyramidspeed;
			angle_cube = angle_cube + cubespeed;
		}
	}
	private void display()
	{
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
		Matrix.translateM(modelViewMatrix,0,-2.0f, 0.0f, -6.0f);
		
		Matrix.setRotateM(rotateMatrix,0,angle_pyramid, 0.0f, 1.0f, 0.0f);
		
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelViewMatrix,0,modelViewMatrix,0,rotateMatrix,0);
		Matrix.multiplyMM(modelViewProjectionMatrix,0,perspectiveProjection,0,modelViewMatrix,0);
		
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texturePyramid[0]);
		GLES32.glUniform1i(samplerUniform, 0);
		
		
		//Send necessary Matrix to shader in respective uniform
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix,0);
		
		//bind wit vao (This will avoid many repeated binding with vbo)
		GLES32.glBindVertexArray(vao_pyramid[0]);
		
		//Draw necessary seen
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);
		//unbind vao
		GLES32.glBindVertexArray(0);

		//unusal program

		GLES32.glUseProgram(0);
					
		requestRender();
	}
	
	private void Unintialize()
	{
		if(vbo_position_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_pyramid,0);
			vbo_position_pyramid[0]=0;
		}
		
		if(vbo_position_cube[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position_cube,0);
			vbo_position_cube[0]=0;
		}
		
		if(vao_pyramid[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vao_pyramid,0);
			vao_pyramid[0]=0;
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



