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

var gVertexShaderObject;
var gFragmentShaderObject;
var glShaderProgramObject;

var vao_pyramid;
var vbo_position_pyramid, vbo_normal_pyramid;
var modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
var red_ld_Uniform, red_la_Uniform, red_ls_Uniform, red_lightPosition_Uniform;
var blue_ld_Uniform, blue_la_Uniform, blue_ls_Uniform, blue_lightPosition_Uniform;
var ka_Uniform, kd_Uniform, ks_Uniform;
var LKeyPress_Uniform, materialShinyness_Uniform;

var perspectiveProjectionMatrix;  //this is from Vmath


var RedlightAmbient = [ 0.0,0.0,0.0 ];
var RedlightDiffuse = [ 1.0,0.0,0.0 ];
var RedlightSpecular = [ 1.0,0.0,0.0 ];
var RedlightPosition = [ -2.0,0.0,0.0,1.0 ];

var BluelightAmbient = [ 0.0,0.0,0.0 ];
var BluelightDiffuse = [ 0.0,0.0,1.0 ];
var BluelightSpecular = [ 0.0,0.0,1.0 ];
var BluelightPosition = [ 2.0,0.0,0.0,1.0 ];


var materialAmbient = [ 0.0,0.0,0.0 ];
var materialDiffuse = [ 1.0,1.0,1.0 ];
var materialSpecular = [ 1.0,1.0,1.0 ];
var materialShinyness = 128.0;




var angle = 0.0;

var bLight = false;
var animation = false;


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
		case 108:
		case 76:
			if (bLight == false)
			{
				bLight = true;
			}
			else {
				bLight = false;
			}
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
	
	
	//Specify/Write vertex shader code

	var VertexShaderSourseCode =
		"#version 300 es"+
		"\n"+
		"precision highp float;"+
		"precision highp int;"+
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_Lkeypress;" +
		"uniform vec3 u_red_la;" +
		"uniform vec3 u_red_ld;" +
		"uniform vec3 u_red_ls;" +
		"uniform vec4 u_red_lightPosition;" +
		"uniform vec3 u_blue_la;" +
		"uniform vec3 u_blue_ld;" +
		"uniform vec3 u_blue_ls;" +
		"uniform vec4 u_blue_lightPosition;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_materialShinyness;" +
		"out vec3 phong_ads_redlight;" +
		"out vec3 phong_ads_bluelight;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"vec3 Transformednormal=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
		"vec3 redlightDirection =normalize(vec3(u_red_lightPosition - eye_coordinate));" +
		"vec3 bluelightDirection =normalize(vec3(u_blue_lightPosition - eye_coordinate));" +
		"float TN_dot_red_ld = max(dot(redlightDirection,Transformednormal),0.0f);" +
		"float TN_dot_blue_ld = max(dot(bluelightDirection,Transformednormal),0.0f);" +
		"vec3 red_reflection_vector = reflect(-redlightDirection,Transformednormal);" +
		"vec3 blue_reflection_vector = reflect(-bluelightDirection,Transformednormal);" +
		"vec3 viewer_vector = normalize(vec3(-eye_coordinate.xyz));" +
		"vec3 redambient = u_red_la * u_ka;" +
		"vec3 reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" +
		"vec3 redspecular = u_red_ls * u_ks * pow(max(dot(red_reflection_vector , viewer_vector ),0.0f),u_materialShinyness);" +
		"phong_ads_redlight = redambient + reddiffuse + redspecular;" +
		"vec3 blueambient = u_blue_la * u_ka;" +
		"vec3 bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" +
		"vec3 bluespecular = u_blue_ls * u_ks * pow(max(dot(blue_reflection_vector , viewer_vector ),0.0f),u_materialShinyness);" +
		"phong_ads_bluelight = blueambient + bluediffuse + bluespecular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_redlight = vec3(1.0f,1.0f,1.0f);" +
		"phong_ads_bluelight = vec3(1.0f,1.0f,1.0f);" +
		"}" +
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
		"}";


	//Define VertexShader Object
	
	gVertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(gVertexShaderObject, VertexShaderSourseCode);
	gl.compileShader(gVertexShaderObject);
	if(gl.getShaderParameter(gVertexShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(gVertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	
	
	//Specify/Write fragment shader code

	var FragmentShaderSourseCode =
		"#version 300 es"+
		"\n"+
		"precision highp float;"+
		"precision highp int;"+
		"vec4 redfragColor;" +
		"vec4 bluefragColor;" +
		"out vec4 FragColor;"+
		"in vec3 phong_ads_redlight;" +
		"in vec3 phong_ads_bluelight;" +
		"void main(void)" +
		"{" +
		"bluefragColor = vec4(phong_ads_bluelight ,1.0);" +
		"redfragColor = vec4(phong_ads_redlight,1.0);" +
		"FragColor = redfragColor + bluefragColor;"+
		"}";
		
		
	gFragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(gFragmentShaderObject, FragmentShaderSourseCode);
	gl.compileShader(gFragmentShaderObject);
	if(gl.getShaderParameter(gFragmentShaderObject, gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(gFragmentShaderObject);
		if(error.length>0)
		{
			alert(error);
			unintialize();
		}
	}
	
	//shader program
	glShaderProgramObject=gl.createProgram();
	gl.attachShader(glShaderProgramObject, gVertexShaderObject);
	gl.attachShader(glShaderProgramObject, gFragmentShaderObject);
	
	//pre-link binding of shader program object with vertex shader attributes 
	gl.bindAttribLocation(glShaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(glShaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");
	
	//linking
	gl.linkProgram(glShaderProgramObject);
	if(!gl.getProgramParameter(glShaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(glShaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	
	
	//Post linking retriving/getting uniform location 
	modelMatrix_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_model_matrix");
	viewMatrix_Uniform =  gl.getUniformLocation(glShaderProgramObject,"u_view_matrix");
	projection_Uniform =  gl.getUniformLocation(glShaderProgramObject,"u_projection_matrix");
	
	LKeyPress_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_Lkeypress");
	
	red_la_Uniform = gl.getUniformLocation(glShaderProgramObject, "u_red_la"); 
	red_ld_Uniform= gl.getUniformLocation(glShaderProgramObject,"u_red_ld");
	red_ls_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_red_ls");
	red_lightPosition_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_red_lightPosition");
	
	blue_la_Uniform = gl.getUniformLocation(glShaderProgramObject, "u_blue_la"); 
	blue_ld_Uniform= gl.getUniformLocation(glShaderProgramObject,"u_blue_ld");
	blue_ls_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_blue_ls");
	blue_lightPosition_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_blue_lightPosition");
	
	ka_Uniform = gl.getUniformLocation(glShaderProgramObject, "u_ka");
	kd_Uniform = gl.getUniformLocation(glShaderProgramObject, "u_kd");
	ks_Uniform = gl.getUniformLocation(glShaderProgramObject, "u_ks");

	materialShinyness_Uniform = gl.getUniformLocation(glShaderProgramObject,"u_materialShinyness");
	
	

	//Ortho fixfunction program



	var pyramidVertices= new Float32Array([
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

	var pyramidNormals = new Float32Array([ 0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
										0.0, 0.447214, 0.894427,
										
										0.894427, 0.447214, 0.0,
										0.894427, 0.447214, 0.0,
										0.894427, 0.447214, 0.0,
										
										0.0, 0.447214, -0.894427,
										0.0, 0.447214, -0.894427,
										0.0, 0.447214, -0.894427,
										

										-0.894427, 0.447214, 0.0,
										-0.894427, 0.447214, 0.0,
										-0.894427, 0.447214, 0.0
										 ]);



	//Create Rectangle vao
	
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
	
	
	
	vbo_normal_pyramid=gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.VDG_ATTRIBUTE_NORMAL,
							3, //3 is for X,Y,Z co-ordinate in our triangleVertices array
							gl.FLOAT,
							false,0,0);
	gl.enableVertexAttribArray(WebGLMacros.VDG_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	
	
	
	gl.bindVertexArray(null);
	
	
	gl.enable(gl.DEPTH_TEST);
	
	gl.depthFunc(gl.LEQUAL);
	
	//gl.enable(gl.CULL_FACE);
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
	
	gl.useProgram(glShaderProgramObject);
	
	if (bLight == true)
	{
		gl.uniform1i(LKeyPress_Uniform,1);
		
		gl.uniform3fv(red_la_Uniform,RedlightAmbient);
		gl.uniform3fv(red_ld_Uniform,RedlightDiffuse);
		gl.uniform3fv(red_ls_Uniform,RedlightSpecular);
		gl.uniform4fv(red_lightPosition_Uniform,RedlightPosition);
		
		gl.uniform3fv(blue_la_Uniform,  BluelightAmbient);
		gl.uniform3fv(blue_ld_Uniform,  BluelightDiffuse);
		gl.uniform3fv(blue_ls_Uniform, BluelightSpecular);
		gl.uniform4fv(blue_lightPosition_Uniform, BluelightPosition);
		
		gl.uniform3fv(ka_Uniform,  materialAmbient);
		gl.uniform3fv(kd_Uniform,  materialDiffuse);
		gl.uniform3fv(ks_Uniform,  materialSpecular);
		gl.uniform1f(materialShinyness_Uniform, materialShinyness);
		
	}
	else
	{
		gl.uniform1i(LKeyPress_Uniform, 0);
	}
	
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	
	mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-6.0]);
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(angle));
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);
	
	gl.bindVertexArray(vao_pyramid);
	
	gl.drawArrays(gl.TRIANGLES,0,12);
	
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
	
	
	if(glShaderProgramObject)
	{
		if(gFragmentShaderObject)
		{
			gl.detachShader(glShaderProgramObject, gFragmentShaderObject);
			gl.deleteShader(gFragmentShaderObject);
			gFragmentShaderObject=null;
		}
		if(gVertexShaderObject)
		{
			gl.detachShader(glShaderProgramObject, gVertexShaderObject);
			gl.deleteShader(gVertexShaderObject);
			gVertexShaderObject=null;
		}
		
		gl.deleteProgram(glShaderProgramObject);
		glShaderProgramObject = null;
	}
	
}

function degToRad(degrees)
{
	//code
	return(degrees*Math.PI/180);
}
