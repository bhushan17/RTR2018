//Gloable variable

var canvas = null;
var gl = null;  //webgl contextual
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;


const WebGLMacros = //When whole 'WebGLMacros' is 'const', all inside it are automatically 'const'
{
	VDG_ATTRIBUTE_VERTEX:0,
	VDG_ATTRIBUTE_COLOR:1,
	VDG_ATTRIBUTE_NORMAL:2,
	VDG_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject1;
var shaderProgramObject2;

var vao_cube;
var  vbo_position_cube;
var vbo_texcord;
var index_buffer;
var fbo;
var color_texture;
var depth_texture;



var  vbo_color_cube;
var angle=0.0;
var mvpUniform;
var mv_location;
var proj_location;
var proj_location1;
var mv_location1;
var WIDTH, HEIGHT;

var perspectiveProjectionMatrix;


// To start animation : To have requestAnimationFrame() to be called "cross-browser" comatible

var requestAnimationFrame = 
window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
window.oRequestAnimationFrame ||
window.msRequestAnimationFrame;

// To stop animation : To have cancelAnimationFrame() to be called "cross-browser" compatible
var cancelAnimationFrame =
window.cancelAnimationFrame ||
window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
window.mozCancelRequestAnimationFrame  || window.mozCancelAnimationFrame  ||
window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame ;


//onload function
function main()
{
	//get <canvas> elementFromPoint
	canvas = document.getElementById("AMC");
	if(!canvas)
		console.log("obtaining Canvas Failed\n");
	else
		console.log("Obtaining Canvas Succeeded\n");
	
	//print canvas width and height on console
	console.log("Canvas Width : "+canvas.width+" And Canvas Height : "+canvas.height);
	
	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;
	
	
	//register keyboard's keydown event handler
	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);
	
	//intialize WebGL
	init();
	
	//start drawing here as warning-up
	resize();
	draw();
	
	
}

function keyDown(event)
{
	//code
	switch(event.keyCode)
	{
		case 70://for 'F' or 'f'
			toggleFullScreen();
			
			break;
		case 27: // Escape
			//Unitialize();
			unintialize();
			//close our application's tab
			window.close(); //may not work in Firefox but work in Safari and chrome
			break;
			
	}
	//codePointAt
	//alert("A key Is Pressed");
}

function mouseDown()
{
	//code
	//alert("Mouse Is Clicked");
}


function toggleFullScreen()
{
	//code 
	var fullscreen_element =
	document.fullscreenElement ||
	document.webkitFullscreenElement ||
	document.mozFullScreenElement ||
	document.msFullscreenElement ||
	null;
	
	//if not fullscreenElement
	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullScreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullscreen = true;
		
	}
	else //if already fullscreenElement
	{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		else if(document.msExitFullscreen)
			document.msExitFullscreen();
		bFullscreen = false;
	}
}

function init()
{
	//code 
	//get WebGL2.0 context
	gl = canvas.getContext("webgl2");
	if(gl==null) //failed to get context
	{
		console.log("Failed to get the rendering context for WebGL");
		return;
	}
	
	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;
	
	//vertex shader 
	var vertexShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec4 position;"+
		"in vec2 texcoord;"+
		"out  vec4 color;"+
		"out  vec2 texcoord1;"+
		
		"uniform mat4 mv_matrix;"+
		"uniform mat4 proj_matrix;"+
		"void main(void)"+
		"{"+
		"    gl_Position = proj_matrix * mv_matrix * position;"+
		"    color = vec4(0.0, 0.0, 0.0, 1.0);"+
		"    texcoord1 = texcoord;"+
		"}";
	vertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			unitialize();
		}
	}
	
	// fragment shader
	var fragmentShaderSourceCode1=
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"in vec4 color;"+
	"in  vec2 texcoord1;"+
		
		"out vec4 color1;"+
		"void main(void)"+
		"{"+
		"    color1 = vec4(1.0, 0.0, 0.0, 1.0) ;  "+
		"}";
		
		
		
	var fragmentShaderSourceCode2=
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"out vec4 color1;"+
		"in vec4 color; "+
		"in    vec2 texcoord1; "+
		"uniform sampler2D tex;"+
		"void main(void) "+
		"{ "+
		"    color1 =  texture(tex, texcoord1); "+
		"}";
		
		
	fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode1);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length>0)
		{
			alert(error);
			unintialize();
		}
	}
	
	//shader program
	shaderProgramObject1=gl.createProgram();
	gl.attachShader(shaderProgramObject1, vertexShaderObject);
	gl.attachShader(shaderProgramObject1, fragmentShaderObject);
	
	//pre-link binding of shader program object with vertex shader attributes 
	gl.bindAttribLocation(shaderProgramObject1,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"position");
	gl.bindAttribLocation(shaderProgramObject1,WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,"texcoord");
	
	//linking
	gl.linkProgram(shaderProgramObject1);
	if(!gl.getProgramParameter(shaderProgramObject1, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject1);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	gl.deleteShader(vertexShaderObject);
	gl.deleteShader(fragmentShaderObject);
	
	
	vertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			unitialize();
		}
	}
	
	
	fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode2);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length>0)
		{
			alert(error);
			unintialize();
		}
	}
	
	
	shaderProgramObject2=gl.createProgram();
	gl.attachShader(shaderProgramObject2, vertexShaderObject);
	gl.attachShader(shaderProgramObject2, fragmentShaderObject);
	
	//pre-link binding of shader program object with vertex shader attributes 
	gl.bindAttribLocation(shaderProgramObject2,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"position");
	gl.bindAttribLocation(shaderProgramObject2,WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,"texcoord");
	
	//linking
	gl.linkProgram(shaderProgramObject2);
	if(!gl.getProgramParameter(shaderProgramObject2, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject2);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	gl.deleteShader(vertexShaderObject);
	gl.deleteShader(fragmentShaderObject);
	
	
	
	
	
	// get MVP uniform location 
	//mvpUniform=gl.getUniformLocation(shaderProgramObject1, "u_mvp_matrix");
	mv_location = gl.getUniformLocation(shaderProgramObject1, "mv_matrix");
	proj_location = gl.getUniformLocation(shaderProgramObject1, "proj_matrix");
	mv_location2 = gl.getUniformLocation(shaderProgramObject2, "mv_matrix");
	proj_location2 = gl.getUniformLocation(shaderProgramObject2, "proj_matrix");
	// ****** vertices, colors, shader attribs,  ****
	
	
	
	/*										
	var cubeVertices = new Float32Array([ 
										 1.0, 1.0, -1.0,
										-1.0, 1.0, -1.0,
										-1.0, 1.0, 1.0,
										1.0, 1.0, 1.0,
										1.0, -1.0, 1.0,
										-1.0, -1.0, 1.0,
										-1.0, -1.0, -1.0,
										1.0, -1.0, -1.0,
										1.0, 1.0, 1.0,
										-1.0, 1.0, 1.0,
										-1.0, -1.0, 1.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
										-1.0, -1.0, -1.0,
										-1.0, 1.0, -1.0,
										1.0, 1.0, -1.0,
										1.0, 1.0, -1.0,
										1.0, 1.0, 1.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
										-1.0, 1.0, 1.0,
										-1.0, 1.0, -1.0,
										-1.0, -1.0, -1.0,
										-1.0, -1.0, 1.0 
										]);
										
										
	var cubeColor = new Float32Array([ 1.0, 0.0, 0.0,
										1.0, 0.0, 0.0,
										1.0, 0.0, 0.0,
										1.0, 0.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 0.0, 1.0,
										0.0, 0.0, 1.0,
										0.0, 0.0, 1.0,
										0.0, 0.0, 1.0,
										0.0, 1.0, 1.0,
										0.0, 1.0, 1.0,
										0.0, 1.0, 1.0,
										0.0, 1.0, 1.0,
										1.0, 0.0, 1.0,
										1.0, 0.0, 1.0,
										1.0, 0.0, 1.0,
										1.0, 0.0, 1.0,
										1.0, 1.0, 0.0,
										1.0, 1.0, 0.0,
										1.0, 1.0, 0.0,
										1.0, 1.0, 0.0 ]);
										

	*/
	
	/*var vertex_indices = new Float32Array([
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
	]);
	
	
	var vertex_data = new Float32Array([
	
		// Position                 Tex Coord
	   -0.25, -0.25,  0.25,      0.0, 1.0,
	   -0.25, -0.25, -0.25,      0.0, 0.0,
		0.25, -0.25, -0.25,      1.0, 0.0,

		0.25, -0.25, -0.25,      1.0, 0.0,
		0.25, -0.25,  0.25,      1.0, 1.0,
	   -0.25, -0.25,  0.25,      0.0, 1.0,

		0.25, -0.25, -0.25,      0.0, 0.0,
		0.25,  0.25, -0.25,      1.0, 0.0,
		0.25, -0.25,  0.25,      0.0, 1.0,

		0.25,  0.25, -0.25,      1.0, 0.0,
		0.25,  0.25,  0.25,      1.0, 1.0,
		0.25, -0.25,  0.25,      0.0, 1.0,

		0.25,  0.25, -0.25,      1.0, 0.0,
	   -0.25,  0.25, -0.25,      0.0, 0.0,
		0.25,  0.25,  0.25,      1.0, 1.0,

	   -0.25,  0.25, -0.25,      0.0, 0.0,
	   -0.25,  0.25,  0.25,      0.0, 1.0,
		0.25,  0.25,  0.25,      1.0, 1.0,

	   -0.25,  0.25, -0.25,      1.0, 0.0,
	   -0.25, -0.25, -0.25,      0.0, 0.0,
	   -0.25,  0.25,  0.25,      1.0, 1.0,

	   -0.25, -0.25, -0.25,      0.0, 0.0,
	   -0.25, -0.25,  0.25,      0.0, 1.0,
	   -0.25,  0.25,  0.25,      1.0, 1.0,

	   -0.25,  0.25, -0.25,      0.0, 1.0,
		0.25,  0.25, -0.25,      1.0, 1.0,
		0.25, -0.25, -0.25,      1.0, 0.0,

		0.25, -0.25, -0.25,      1.0, 0.0,
	   -0.25, -0.25, -0.25,      0.0, 0.0,
	   -0.25,  0.25, -0.25,      0.0, 1.0,

	   -0.25, -0.25,  0.25,      0.0, 0.0,
		0.25, -0.25,  0.25,      1.0, 0.0,
		0.25,  0.25,  0.25,      1.0, 1.0,

		0.25,  0.25,  0.25,      1.0, 1.0,
	   -0.25,  0.25,  0.25,      0.0, 1.0,
	   -0.25, -0.25,  0.25,      0.0, 0.0,
	]);*/
	

	var cubeVertices = new Float32Array([
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0,
		-1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		
		1.0, -1.0, 1.0,
		-1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		
		1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0, 1.0, -1.0,
		1.0, 1.0, -1.0,
		
		1.0, 1.0, -1.0,
		1.0, 1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, -1.0, -1.0,
		
		-1.0, 1.0, 1.0,
		-1.0, 1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0, -1.0, 1.0 ]);
				
		
		var cubeTextCord = new Float32Array([
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0  ]);		
	
	
	vao_cube=gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
	
	vbo_position_cube=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);
	
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z co-ordinate in our rectangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	vbo_texcord=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcord);
	gl.bufferData(gl.ARRAY_BUFFER, cubeTextCord, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,
							2, //3 is for X,Y,Z co-ordinate in our rectangleVertices array
							gl.FLOAT,
							false,0, 0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	/*index_buffer=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, index_buffer);
	gl.bufferData(gl.ARRAY_BUFFER, vertex_indices, gl.STATIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);*/
	
	
	fbo = gl.createFramebuffer();
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
	
	color_texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, color_texture);
	gl.texStorage2D(gl.TEXTURE_2D, 1, gl.RGBA8, 512, 512);
	
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	
	depth_texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, depth_texture);
	gl.texStorage2D(gl.TEXTURE_2D, 1, gl.DEPTH_COMPONENT32F, 512, 512);
	
	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D,color_texture, 0);
	gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D,depth_texture, 0);
	
	
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);
	
	gl.bindVertexArray(null);
	
	gl.enable(gl.DEPTH_TEST);
	
	//set clear color 
	gl.clearColor(0.0,0.0,0.0,1.0); //blue
	
	//intialize projection matrix
	perspectiveProjectionMatrix=mat4.create();
}

function resize()
{
	//code 
	if(bFullscreen==true)
	{
		canvas.width=window.innerWidth;
		canvas.height=window.innerHeight;
	}
	else
	{
		canvas.width=canvas_original_width;
		canvas.height=canvas_original_height;
	}
	
	//set the view to match
	gl.viewport(0,0,canvas.width, canvas.height);
	
	WIDTH = canvas.width;
	HEIGHT = canvas.height;
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);

		
}

function draw()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	//gl.useProgram(shaderProgramObject1);
	
	var modelViewMatrix=mat4.create();
	var modelViewframbufferMatrix=mat4.create();
	var modelViewProjectionMatrix=mat4.create();
	
	
	
	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[0.0,0.0,-3.0]);
	mat4.translate(modelViewframbufferMatrix,modelViewframbufferMatrix,[0.0,0.0,-3.0]);
	mat4.scale(modelViewMatrix,modelViewMatrix,[0.8,0.8,0.8]);
	mat4.scale(modelViewframbufferMatrix,modelViewframbufferMatrix,[0.8,0.8,0.8]);
	mat4.rotateX(modelViewMatrix,modelViewMatrix,degToRad(angle));
	mat4.rotateY(modelViewMatrix,modelViewMatrix,degToRad(angle));
	mat4.rotateZ(modelViewMatrix,modelViewMatrix,degToRad(angle));
	mat4.rotateX(modelViewframbufferMatrix,modelViewframbufferMatrix,degToRad(angle));
	mat4.rotateY(modelViewframbufferMatrix,modelViewframbufferMatrix,degToRad(angle));
	mat4.rotateZ(modelViewframbufferMatrix,modelViewframbufferMatrix,degToRad(angle));
	
	//mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
	var color = new Float32Array([ 0.4,0.4,0.4,1.0 ]);
	 
	gl.clearBufferfv(gl.COLOR, 0,color);
	gl.clearBufferfi(gl.DEPTH_STENCIL, 0.0, 1.0, 0.0);
	gl.viewport(0, 0, 512, 512);
	gl.useProgram(shaderProgramObject1);

	gl.uniformMatrix4fv(proj_location, false,perspectiveProjectionMatrix);
	gl.uniformMatrix4fv(mv_location, false,modelViewMatrix);
	
	//gl.drawArrays(gl.GL_TRIANGLES, 0, 36);
	gl.bindVertexArray(vao_cube);
	
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);

	gl.bindVertexArray(null);
	gl.bindFramebuffer(gl.FRAMEBUFFER,null);
	//gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.useProgram(null);
	
	
	
	gl.viewport(0, 0, WIDTH, HEIGHT);
	gl.useProgram(shaderProgramObject2);
	
	
	gl.uniformMatrix4fv(proj_location2, false, perspectiveProjectionMatrix);
	gl.uniformMatrix4fv(mv_location2, false, modelViewframbufferMatrix);

    gl.bindTexture(gl.TEXTURE_2D, color_texture);
	//gl.drawArrays(gl.GL_TRIANGLES, 0, 36);
	gl.bindVertexArray(vao_cube);
	
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);
	
	gl.bindVertexArray(null);
	gl.bindTexture(gl.TEXTURE_2D, null);
	gl.useProgram(null);
	
	
	
	//animation loop
	angle=angle+0.5;
	
	if(angle > 360)
		angle=0.0;
	
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	
	
	if(shaderProgramObject1)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject1, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject=null;
		}
		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject1, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject=null;
		}
		
		gl.deletProgram(shaderProgramObject1);
		shaderProgramObject1 = null;
	}
	
}

function degToRad(degrees)
{
	//code
	return(degrees*Math.PI/180);
}
