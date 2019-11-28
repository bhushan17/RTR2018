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
var shaderProgramObject;

var vao_pyramid,vao_cube;
var vbo_position_pyramid, vbo_position_cube;

var vbo_color_pyramid, vbo_color_cube;
var angle=0.0;
var mvpUniform;


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
	"in vec4 vPosition;" +
		"in vec4 vColor;" +
		"uniform mat4 u_mvp_matrix;" +
		"out vec4 out_color;" +
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_color = vColor;" +
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
	var fragmentShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"out vec4 fragColor;" +
		"in vec4 out_color;" +
		"void main(void)" +
		"{" +
		"fragColor = out_color;" +
		"}";
	fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
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
	shaderProgramObject=gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);
	
	//pre-link binding of shader program object with vertex shader attributes 
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
	
	//linking
	gl.linkProgram(shaderProgramObject);
	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	// get MVP uniform location 
	mvpUniform=gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");
	
	// ****** vertices, colors, shader attribs, vbo_position_pyramid, vao_pyramid intialization ****
	var pyramidVertices = new Float32Array([
											0.0, 1.0, 0.0,
										-1.0, -1.0, 1.0,
										1.0, -1.0, 1.0,
										0.0, 1.0, 0.0,
										1.0, -1.0, 1.0,
										1.0, -1.0, -1.0,
										0.0, 1.0, 0.0,
										-1.0, -1.0, -1.0,
										1.0, -1.0, -1.0,
										0.0, 1.0, 0.0,
										-1.0, -1.0, 1.0,
										-1.0, -1.0, -1.0
											]);
	
	var pyramidColor = new Float32Array([ 1.0, 0.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 0.0, 1.0,
										1.0, 0.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 0.0, 1.0,
										1.0, 0.0, 0.0,
										0.0, 0.0, 1.0,
										0.0, 1.0, 0.0 ,
										1.0, 0.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 0.0, 1.0 ]);
											
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
										
	vao_pyramid=gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);
	
	vbo_position_pyramid=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z co-ordinate in our pyramidVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	vbo_color_pyramid=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidColor, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_COLOR,
							3, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	
	gl.bindVertexArray(null);
	
	
	
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
	
	
	
	
	
	vbo_color_cube=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cubeColor, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_COLOR,
							3, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
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
	
	
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);

		
}

function draw()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	var modelViewMatrix=mat4.create();
	var modelViewProjectionMatrix=mat4.create();
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[-1.5,0.0,-6.0]);
	mat4.rotateY(modelViewMatrix,modelViewMatrix,degToRad(angle));
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false,modelViewProjectionMatrix);
	
	gl.bindVertexArray(vao_pyramid);
	
	gl.drawArrays(gl.TRIANGLES,0,12);
	
	gl.bindVertexArray(null);
	
	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[1.5,0.0,-6.0]);
	mat4.scale(modelViewMatrix,modelViewMatrix,[0.8,0.8,0.8]);
	mat4.rotateX(modelViewMatrix,modelViewMatrix,degToRad(angle));
	
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false,modelViewProjectionMatrix);
	
	gl.bindVertexArray(vao_cube);
	
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);
	
		
	gl.bindVertexArray(null);
	
	
	gl.useProgram(null);
	//animation loop
	angle=angle+2.0;
	
	if(angle > 360)
		angle=0.0;
	
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}
	
	if(vbo_position_pyramid)
	{
		gl.deleteBuffer(vbo_position_pyramid);
		vbo_position_pyramid=null;
	}
	
	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject=null;
		}
		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject=null;
		}
		
		gl.deletProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
	
}

function degToRad(degrees)
{
	//code
	return(degrees*Math.PI/180);
}
