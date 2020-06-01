package com.matrix_android.Framebuffer;

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




public class GLESView extends GLSurfaceView implements  OnGestureListener, OnDoubleTapListener,  GLSurfaceView.Renderer
{
    private GestureDetector gestureDetector; 
	private final Context context;
	
	//Java neither have unsignint not gluint
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject1;
	private int shaderProgramObject2;
	
	//Java doesn't have & operator, we create array of 1 & pass name of array which behave as address.
	
	
	private float  angle_cube = 0.0f;
	private float  angle_framebuffer = 0.0f;
	private float cubespeed = 0.0f;
	private float cubeframbufferspeed = 0.0f;
	
	private int[]vao_cube = new int[1];
	
	private int[]vbo_position_cube = new int[1];
	private int[]vbo_texture = new int[1];
	private int[]fbo = new int[1];
	private int[]color_texture = new int[1];
	private int[]depth_texture = new int[1];
	
	
	private int          program1;
	private int          program2;
	private int          vao;
	private int          position_buffer;
	//private int          index_buffer;
	//private int          fbo;
	//private int          color_texture;
	//private int          depth_texture;
	private int           mv_location;
	private int           proj_location;
	private int          mv_location2;
	private int          proj_location2;
	
	private int WIDTH, HEIGHT;
	
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
		
		cubespeed = cubespeed + 2.0f;
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
		
			cubespeed = 0.0f;
		
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
		
		final String vs_source = String.format
	    ("#version 320 es" +
		"\n" +      
        "precision highp float;"+		
		"in vec4 position;"+
		"in vec2 texcoord;"+
		"out VS_OUT"+
		"{"+
		"    vec4 color;"+
		"    vec2 texcoord;"+
		"} vs_out;"+
		"uniform mat4 mv_matrix;"+
		"uniform mat4 proj_matrix;"+
		"void main(void)"+
		"{"+
		"    gl_Position = proj_matrix * mv_matrix * position;"+
		"    vs_out.color = vec4(0.0, 0.0, 0.0, 1.0);"+
		"    vs_out.texcoord = texcoord;"+
		"}");
		
		
		final String fs_source1 = String.format
		("#version 320 es" +
		"\n" +
		"precision highp float;"+
		"in VS_OUT"+
		"{"+
		"    vec4 color;"+
		"    vec2 texcoord;"+
		"} fs_in;"+
		"out vec4 color;"+
		"void main(void)"+
		"{"+
		"    color = vec4(1.0, 0.0, 0.0, 1.0) ;  "+
		"}");
		
		
		final String fs_source2= String.format
	    ("#version 320 es" +
		"\n" +
		"precision highp float;"+
		"uniform sampler2D tex;"+
		"out vec4 color;"+
		"in VS_OUT "+
		"{"+
		"    vec4 color; "+
		"    vec2 texcoord; "+
		"} fs_in;"+
		"void main(void) "+
		"{ "+
		"    color =  texture(tex, fs_in.texcoord); "+
		"}");
	
	
		// Vertex Shader
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		
		GLES32.glShaderSource(vertexShaderObject,vs_source);
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
		GLES32.glShaderSource(fragmentShaderObject,fs_source1);
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
		shaderProgramObject1 = GLES32.glCreateProgram();
		
		//Attach Vertex Shader to the Shader Program
		GLES32.glAttachShader(shaderProgramObject1, vertexShaderObject);
		
		//Attach Fragnebt Shader
		GLES32.glAttachShader(shaderProgramObject1, fragmentShaderObject);
		
		// Prelinking binding to Vertex attribute
		GLES32.glBindAttribLocation(shaderProgramObject1, GLESMacros.AMC_ATTRIBUTE_POSITION, "position");
		GLES32.glBindAttribLocation(shaderProgramObject1, GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, "texcoord");
		
		//Link the Shader Program
		GLES32.glLinkProgram(shaderProgramObject1);
		
		int[]iProgramLinkStatus = new int[1];
		iInfolog[0] = 0;
		
		SzInfolog = null;
		
		//ErrorCheking
		GLES32.glGetProgramiv(shaderProgramObject1,GLES32.GL_LINK_STATUS,iProgramLinkStatus,0);
		if (iProgramLinkStatus[0] == 0)
		{
			GLES32.glGetProgramiv(shaderProgramObject1, GLES32.GL_INFO_LOG_LENGTH, iInfolog,0);
			if(iInfolog[0]>0)
			{
				SzInfolog = GLES32.glGetProgramInfoLog(shaderProgramObject1);
				System.out.println("RTR: ProgramObject compile log:" + SzInfolog);
				Unintialize();
				System.exit(0);
			}
		}
		
		GLES32.glDeleteShader(vertexShaderObject);
		GLES32.glDeleteShader(fragmentShaderObject);
		
		
		
		
		
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		
		GLES32.glShaderSource(vertexShaderObject,vs_source);
		GLES32.glCompileShader(vertexShaderObject);
		
		//int[]ishaderCompileStatus = new int[1];
		//int[] iInfolog = new int[1];
		
		SzInfolog = null;
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
		GLES32.glShaderSource(fragmentShaderObject,fs_source2);
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
		shaderProgramObject2 = GLES32.glCreateProgram();
		
		//Attach Vertex Shader to the Shader Program
		GLES32.glAttachShader(shaderProgramObject2, vertexShaderObject);
		
		//Attach Fragnebt Shader
		GLES32.glAttachShader(shaderProgramObject2, fragmentShaderObject);
		
		// Prelinking binding to Vertex attribute
		GLES32.glBindAttribLocation(shaderProgramObject2, GLESMacros.AMC_ATTRIBUTE_POSITION, "position");
		GLES32.glBindAttribLocation(shaderProgramObject2, GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, "texcoord");
		
		//Link the Shader Program
		GLES32.glLinkProgram(shaderProgramObject2);
		
		//int[]iProgramLinkStatus = new int[1];
		iInfolog[0] = 0;
		
		SzInfolog = null;
		
		//ErrorCheking
		GLES32.glGetProgramiv(shaderProgramObject2,GLES32.GL_LINK_STATUS,iProgramLinkStatus,0);
		if (iProgramLinkStatus[0] == 0)
		{
			GLES32.glGetProgramiv(shaderProgramObject2, GLES32.GL_INFO_LOG_LENGTH, iInfolog,0);
			if(iInfolog[0]>0)
			{
				SzInfolog = GLES32.glGetProgramInfoLog(shaderProgramObject2);
				System.out.println("RTR: ProgramObject compile log:" + SzInfolog);
				Unintialize();
				System.exit(0);
			}
		}
		
		GLES32.glDeleteShader(vertexShaderObject);
		GLES32.glDeleteShader(fragmentShaderObject);
		
				
		//Post linking retriving/getting uniform location 
		//mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject1, "u_mvp_matrix");
		
		mv_location = GLES32.glGetUniformLocation(shaderProgramObject1, "mv_matrix");
		proj_location = GLES32.glGetUniformLocation(shaderProgramObject1, "proj_matrix");
		mv_location2 = GLES32.glGetUniformLocation(shaderProgramObject2, "mv_matrix");
		proj_location2 = GLES32.glGetUniformLocation(shaderProgramObject2, "proj_matrix");
		
		//triangle position vertices
		//Array declarartion & inline intialize so no need to give array size
		/*
		
			
		
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
		
		
		final float[]cubeColor = new float[]
		{ 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f};
		
		
		*/
	/*	final float[]vertex_indices= new float[]
	{
		0, 1, 2,
		2, 1, 3,
		2, 3, 4,
		4, 3, 5,
		4, 5, 6,
		6, 5, 7,
		6, 7, 0,
		0, 7, 1,
		6, 0, 2,
		2, 4, 6,
		7, 5, 3,
		7, 3, 1
	};

	final float[]vertex_data = new float[]
	{
		// Position                 Tex Coord
	   -0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
	   -0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      1.0f, 1.0f,
	   -0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
		0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

		0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
	   -0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

	   -0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
	   -0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

	   -0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
	   -0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
	   -0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

	   -0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
	   -0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
	   -0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

	   -0.25f,  0.25f, -0.25f,      0.0f, 1.0f,
		0.25f,  0.25f, -0.25f,      1.0f, 1.0f,
		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

		0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
	   -0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
	   -0.25f,  0.25f, -0.25f,      0.0f, 1.0f,

	   -0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
		0.25f, -0.25f,  0.25f,      1.0f, 0.0f,
		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

		0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
	   -0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
	   -0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
	};*/
		
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
	
		
				
		//Create vao_cube
		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);
		
		//Create and bind vbo_position_cube
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
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
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
		
		
		
		GLES32.glGenBuffers(1,vbo_texture,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(cubeTextCord.length * 4);
		
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(cubeTextCord);
		
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTextCord.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0 , 0 );
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
		
		
		
		
		
		
		//Create and bind vbo_color_cube
		/*GLES32.glGenBuffers(1,index_buffer,0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, index_buffer[0]);
		//we need to send Bufferdata but we have array so we need to create buffer first
		//in 5 steps
		//Step 1
		//allocate the buffer directly from native memory(not from VM memory)
		byteBuffer = ByteBuffer.allocateDirect(vertex_indices.length * 4);
		//Step 2
		//Arrange the Buffer byte order in native byte order
		byteBuffer.order(ByteOrder.nativeOrder());
		//Step 3
		//Create the float type Buffer & Convert our Bytetype Buffer in to float type Buffer
		positionBuffer = byteBuffer.asFloatBuffer();
		//Step 4
		//Now put your arrays in to this coocked buffer
		positionBuffer.put(vertex_indices);
		//Step 5
		//Set the array at there 0th position of Buffer
		positionBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertex_indices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);*/
		
		
		
		GLES32.glGenFramebuffers(1,fbo,0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);
		
		GLES32.glGenTextures(1, color_texture,0);
	    GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);
		GLES32.glTexStorage2D(GLES32.GL_TEXTURE_2D, 9, GLES32.GL_RGBA8, 512, 512);
		
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		
		GLES32.glGenTextures(1,depth_texture,0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, depth_texture[0]);
		GLES32.glTexStorage2D(GLES32.GL_TEXTURE_2D, 9, GLES32.GL_DEPTH_COMPONENT32F, 512, 512);
		
		
		
		GLES32.glFramebufferTexture(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, color_texture[0], 0);
		GLES32.glFramebufferTexture(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, depth_texture[0], 0);
		
		
		//final float[] draw_buffers = new float[] { GLES32.GL_COLOR_ATTACHMENT0 };
	    //GLES32.glDrawBuffers(1, draw_buffers,0);
		//GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
		GLES32.glBindVertexArray(0);
		
		//GLenum []draw_buffers = new GLenum[]{ GL_COLOR_ATTACHMENT0 };
	    //glDrawBuffers(1, draw_buffers);
		
		
		
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
		WIDTH = width;
		HEIGHT = height;
		
	}
	private void update()
	{
		if (angle_cube > 360)
		{
			angle_cube = 0.0f;
			//angle_rectangle = 0.0f;
		}
		else {
			
			angle_cube = angle_cube + cubespeed;
		}
	}
	private void display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		
		
		//Declaration of Matrix
		float[]modelViewMatrix = new float[16];
		float[]modelViewframbufferMatrix = new float[16];
		float[]rotateMatrix = new float[16];
		float[]rotationFrambufferMatrix = new float[16];
		float[]modelViewProjectionMatrix = new float[16];
		
		
		//Intialize above matrix to Identity;
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(rotateMatrix,0);
		Matrix.setIdentityM(modelViewframbufferMatrix,0);
		Matrix.setIdentityM(rotationFrambufferMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		//Matrix.setIdentityM(translateMatrix,0);
		
		



		float[]rotateMatrix2 = new float[16];
		Matrix.setIdentityM(rotateMatrix2,0);
		
		float[]rotateMatrixframe2 = new float[16];
		Matrix.setIdentityM(rotateMatrixframe2,0);
		
		//Matrix.setIdentityM(translateMatrix,0);
		
		//Do necessary transformation
		Matrix.translateM(modelViewMatrix,0,0.0f, 0.0f, -6.0f);
		Matrix.translateM(modelViewframbufferMatrix,0,0.0f, 0.0f, -6.0f);
		
		Matrix.setRotateM(rotateMatrix2,0,angle_cube, 1.0f, 0.0f,0.0f);
		Matrix.multiplyMM(rotateMatrix,0,rotateMatrix2,0,rotateMatrix,0);
		
		Matrix.setRotateM(rotateMatrix2,0,angle_cube, 0.0f, 1.0f,0.0f);
		Matrix.multiplyMM(rotateMatrix,0,rotateMatrix2,0,rotateMatrix,0);
		
		Matrix.setRotateM(rotateMatrix2,0,angle_cube, 0.0f, 0.0f,1.0f);
		Matrix.multiplyMM(rotateMatrix,0,rotateMatrix2,0,rotateMatrix,0);
		
		//
		Matrix.setRotateM(rotateMatrixframe2,0,angle_cube, 1.0f, 0.0f,0.0f);
		Matrix.multiplyMM(rotationFrambufferMatrix,0,rotateMatrixframe2,0,rotationFrambufferMatrix,0);
		
		Matrix.setRotateM(rotateMatrixframe2,0,angle_cube, 0.0f, 1.0f,0.0f);
		Matrix.multiplyMM(rotationFrambufferMatrix,0,rotateMatrixframe2,0,rotationFrambufferMatrix,0);
		
		Matrix.setRotateM(rotateMatrixframe2,0,angle_cube, 0.0f, 0.0f,1.0f);
		Matrix.multiplyMM(rotationFrambufferMatrix,0,rotateMatrixframe2,0,rotationFrambufferMatrix,0);
		
		
		//Do necessary Matrix multiplication
		Matrix.multiplyMM(modelViewMatrix,0,modelViewMatrix,0,rotateMatrix,0);
		Matrix.multiplyMM(modelViewframbufferMatrix,0,modelViewframbufferMatrix,0,rotationFrambufferMatrix,0);
		
		
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);
		final float[] color = new float[] { 0.4f,0.4f,0.4f,1.0f };
		GLES32.glViewport(0,0,512,512);
		GLES32.glClearBufferfv(GLES32.GL_COLOR, 0,color,0);
		GLES32.glClearBufferfi(GLES32.GL_DEPTH_STENCIL, 0, 1, 0);
		
		
		GLES32.glUseProgram(shaderProgramObject1);
		
		GLES32.glUniformMatrix4fv(proj_location, 1, false, perspectiveProjection,0);
	    GLES32.glUniformMatrix4fv(mv_location, 1, false, modelViewMatrix,0);
		
		//GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 36);
		
		GLES32.glBindVertexArray(vao_cube[0]);
		
		
		
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);
		
		GLES32.glBindVertexArray(0);
		
		GLES32.glUseProgram(0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
		
		
		GLES32.glViewport(0,0,WIDTH,HEIGHT);
		
		GLES32.glUseProgram(shaderProgramObject2);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);

		GLES32.glUniformMatrix4fv(proj_location2, 1, false, perspectiveProjection,0);
	    GLES32.glUniformMatrix4fv(mv_location2, 1, false, modelViewframbufferMatrix,0);
		
		GLES32.glBindVertexArray(vao_cube[0]);
		
		
		
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);
		
		
		
		GLES32.glBindVertexArray(0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);



		GLES32.glUseProgram(0);
					
		requestRender();
	}
	
	private void Unintialize()
	{
		
		
		
		
		if(shaderProgramObject1 != 0)
		{
			int[] shaderCount = new int[1];
			int[] shader = new int[shaderCount[0]];
			int shaderNumber ;
			
			GLES32.glUseProgram(shaderProgramObject1);
			//ask program How many shader attached with you
			GLES32.glGetProgramiv(shaderProgramObject1, GLES32.GL_ATTACHED_SHADERS, shaderCount,0);
			
			GLES32.glGetAttachedShaders(shaderProgramObject1, shaderCount[0], shaderCount, 0 , shader,0);
			
			for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++)
			{
				//Detached one by one
				GLES32.glDetachShader(shaderProgramObject1, shader[shaderNumber]);
				//delete detach shader
				GLES32.glDeleteShader(shader[shaderNumber]);
				shader[shaderNumber] = 0;
				
			}
		
			GLES32.glDeleteProgram(shaderProgramObject1);
			shaderProgramObject1 = 0;
			GLES32.glUseProgram(0);
		
		}
	}
	
}



