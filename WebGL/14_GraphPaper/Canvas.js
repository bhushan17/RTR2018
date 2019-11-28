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

var vao_line;
var vbo_line;


var vbo_color_line;

var mvpUniform;


var perspectiveProjectionMatrix;

var lineVertices = new Float32Array([
											0.0,1.0,0.0, 
											0.0,-1.0,0.0  
											
											]);
											
var lineColor = new Float32Array([ 0.0,1.0,0.0,
										0.0,1.0,0.0
										 ]);

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
	
	// ****** vertices, colors, shader attribs, vbo_triangle, vao_triangle intialization ****
	
	
	
											
	
										
	vao_line=gl.createVertexArray();
	gl.bindVertexArray(vao_line);
	
	vbo_line=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	vbo_color_line=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineColor, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_COLOR,
							3, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	
	gl.bindVertexArray(null);
	
		
	//set clear color 
	gl.clearColor(1.0,1.0,1.0,1.0); //blue
	
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
	var x_coordinate = -1.05, y_coordinate = -1.05;
	var index;
	//code
	gl.clear(gl.COLOR_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	var modelViewMatrix=mat4.create();
	var modelViewProjectionMatrix=mat4.create();
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[0.0,0.0,-2.0]);
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false,modelViewProjectionMatrix);
	
	
	
	gl.bindVertexArray(vao_line);
	
	
	for (index = 0; index <= 40; index++)
	{
		gl.lineWidth(5);
		x_coordinate = x_coordinate + 0.05;
		y_coordinate = y_coordinate + 0.05;

					
			lineColor[0] = 0.0;
			lineColor[1] = 1.0;
			lineColor[2] = 0.0;
			lineColor[3] = 0.0;
			lineColor[4] = 1.0;
			lineColor[5] = 0.0;
	
			lineVertices[0] = x_coordinate;
			lineVertices[1] = -1.0;
			lineVertices[2] = 0.0;
			lineVertices[3] = x_coordinate;
			lineVertices[4] = 1.0;
			lineVertices[5] = 0.0;

			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line);
			gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.DYNAMIC_DRAW);
			gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_line);
			gl.bufferData(gl.ARRAY_BUFFER, lineColor, gl.DYNAMIC_DRAW);
			gl.bindBuffer(gl.ARRAY_BUFFER, null);
			
			gl.drawArrays(gl.LINES,0,2);

			
			

		

			lineVertices[0] = -1.0;
			lineVertices[1] = y_coordinate;
			lineVertices[2] = 0.0;
			lineVertices[3] = 1.0;
			lineVertices[4] = y_coordinate;
			lineVertices[5] = 0.0;

			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line);
			gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.DYNAMIC_DRAW);
			gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_line);
			gl.bufferData(gl.ARRAY_BUFFER, lineColor, gl.DYNAMIC_DRAW);
			gl.bindBuffer(gl.ARRAY_BUFFER, null);
			
			gl.drawArrays(gl.LINES,0,2);


			
		

	}
	
	
	lineVertices[0] = 0.0;
	lineVertices[1] = 1.0;
	lineVertices[2] = 0.0;
	lineVertices[3] = 0.0;
	lineVertices[4] = -1.0;
	lineVertices[5] = 0.0;
	
	lineColor[0] = 1.0;
	lineColor[1] = 0.0;
	lineColor[2] = 0.0;
	lineColor[3] = 1.0;
	lineColor[4] = 0.0;
	lineColor[5] = 0.0;
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineColor, gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.lineWidth(2.0);
	gl.drawArrays(gl.LINES,0,2);
	
	lineVertices[0] = -1.0;
	lineVertices[1] = 0.0;
	lineVertices[2] = 0.0;
	lineVertices[3] = 1.0;
	lineVertices[4] = 0.0;
	lineVertices[5] = 0.0;
	
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineVertices, gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_line);
	gl.bufferData(gl.ARRAY_BUFFER, lineColor, gl.DYNAMIC_DRAW);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.drawArrays(gl.LINES,0,2);
	
	gl.bindVertexArray(null);
	
		
	gl.useProgram(null);
	//animation loop
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	if(vao_triangle)
	{
		gl.deleteVertexArray(vao_triangle);
		vao_triangle = null;
	}
	
	if(vbo_triangle)
	{
		gl.deleteBuffer(vbo_triangle);
		vbo_triangle=null;
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