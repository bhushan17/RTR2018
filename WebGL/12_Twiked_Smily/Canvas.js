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
var uniform_variable;


var vao_rectangle;
var vbo_rectangle;
var vbo_texture;

var mvpUniform;
var uniform_texture0_sampler;
var smily_texture=0;

var perspectiveProjectionMatrix;
var keypress = 0;

var textcoord = new Float32Array([
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0
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
		case 48:
			keypress = 0;
			break;
		case 49:
			keypress = 1;
			break;	
		case 50:
			keypress = 2;
			break;
		case 51:
			keypress = 3;
			break;	
		case 52:
			keypress = 4;
			break;
		default:
			keypress = 0;
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
		"in vec2 vTexture0_Coord;" +
		"out vec2 out_texture0_coord;"+
		"uniform mat4 u_mvp_matrix;" +
		"void main(void)" +
		"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_texture0_coord = vTexture0_Coord;" +
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
	"in vec2 out_texture0_coord;"+
	"uniform highp sampler2D u_texture0_sampler;"+
	"out vec4 fragColor;" +
	"uniform vec4 variable;"+
		
		"void main(void)" +
		"{" +
		"fragColor = texture(u_texture0_sampler,out_texture0_coord) + variable;" +
		
		
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");
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
	uniform_texture0_sampler=gl.getUniformLocation(shaderProgramObject, "u_texture0_sampler");
	uniform_variable=gl.getUniformLocation(shaderProgramObject, "variable");
	
	
	// ****** vertices, colors, shader attribs, vbo_triangle, vao_triangle intialization ****
	
											
	var rectangleVertices = new Float32Array([ 
										 1.0, 1.0, 0.0,
										-1.0, 1.0, 0.0,
										-1.0, -1.0, 0.0,
										1.0, -1.0, 0.0 
										]);
										
										
			
	vao_rectangle=gl.createVertexArray();
	gl.bindVertexArray(vao_rectangle);
	
	vbo_rectangle=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_rectangle);
	gl.bufferData(gl.ARRAY_BUFFER, rectangleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z co-ordinate in our rectangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	
	
	vbo_texture=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER, textcoord, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,
							2, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);
	
	//Depth test will always be enable
	gl.enable(gl.DEPTH_TEST);
	
	//gl.enable(gl.COLOR_LOGIC_OP);
	
	// We will always cull back faces for better performance
	gl.enable(gl.CULL_FACE);
	
	//set clear color 
	gl.clearColor(0.0,0.0,0.0,1.0); //blue
	
	
	//Load Smily texture
	smily_texture = gl.createTexture();
	smily_texture.image = new Image();
	smily_texture.image.src = "Smiley.png";
	smily_texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, smily_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE,smily_texture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER,gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER,gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
		
	}
	
	
	
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
	var arr = new Float32Array([1.0,1.0,1.0,1.0]);
	
	if(keypress == 0)
	{
		textcoord[0] = 1.0;
		textcoord[1] = 1.0;
		textcoord[2] = 1.0;
		textcoord[3] = 1.0;
		textcoord[4] = 1.0;
		textcoord[5] = 1.0;
		textcoord[6] = 1.0;
		textcoord[7] = 1.0;
	}
	else if(keypress == 1)
	{
		textcoord[0] = 0.5;
		textcoord[1] = 0.5;
		textcoord[2] = 0.0;
		textcoord[3] = 0.5;
		textcoord[4] = 0.0;
		textcoord[5] = 0.0;
		textcoord[6] = 0.5;
		textcoord[7] = 0.0;
	}
	else if(keypress == 2)
	{
		textcoord[0] = 1.0;
		textcoord[1] = 1.0;
		textcoord[2] = 0.0;
		textcoord[3] = 1.0;
		textcoord[4] = 0.0;
		textcoord[5] = 0.0;
		textcoord[6] = 1.0;
		textcoord[7] = 0.0;
	}
	else if(keypress == 3)
	{
		textcoord[0] = 2.0;
		textcoord[1] = 2.0;
		textcoord[2] = 0.0;
		textcoord[3] = 2.0;
		textcoord[4] = 0.0;
		textcoord[5] = 0.0;
		textcoord[6] = 2.0;
		textcoord[7] = 0.0;
	}
	else if(keypress == 4)
	{
		textcoord[0] = 0.5;
		textcoord[1] = 0.5;
		textcoord[2] = 0.5;
		textcoord[3] = 0.5;
		textcoord[4] = 0.5;
		textcoord[5] = 0.5;
		textcoord[6] = 0.5;
		textcoord[7] = 0.5;
	}
	else 
	{
		keypress = 0;
	}
	
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	var modelViewMatrix=mat4.create();
	var modelViewProjectionMatrix=mat4.create();
	
		
	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);
	
	mat4.translate(modelViewMatrix,modelViewMatrix,[0.0,0.0,-3.0]);
	mat4.multiply(modelViewProjectionMatrix,perspectiveProjectionMatrix,modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false,modelViewProjectionMatrix);
	
	if((keypress > 0)&&(keypress < 5))
	{
		gl.bindTexture(gl.TEXTURE_2D, smily_texture);
	    gl.uniform1i(uniform_texture0_sampler,0);
		arr[0] = 0.0;
		arr[1] = 0.0;
		arr[2] = 0.0;
		arr[3] = 0.0;
		gl.uniform4fv(uniform_variable,arr);
	}
	else{
		arr[0] = 1.0;
		arr[1] = 1.0;
		arr[2] = 1.0;
		arr[3] = 1.0;
		gl.uniform4fv(uniform_variable,arr);
	}
	
	
	gl.bindVertexArray(vao_rectangle);
	
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER, textcoord, gl.DYNAMIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0,
							2, 
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	
	gl.bindVertexArray(null);
	
	
	gl.useProgram(null);
	//animation loop
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	if(vao_rectangle)
	{
		gl.deleteVertexArray(vao_rectangle);
		vao_rectangle = null;
	}
	
	if(vbo_texture)
	{
		gl.deleteBuffer(vbo_texture);
		vbo_texture=null;
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