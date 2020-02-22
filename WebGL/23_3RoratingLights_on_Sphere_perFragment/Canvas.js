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

var RedlightAmbient = [ 0.0,0.0,0.0];
var RedlightDiffuse= [ 1.0,0.0,0.0];
var RedlightSpecular = [ 1.0,0.0,0.0];
var RedlightPosition = [ 0.0,0.0,-3.0,1.0 ];

var GreenlightAmbient = [ 0.0,0.0,0.0];
var GreenlightDiffuse = [ 0.0,1.0,0.0];
var GreenlightSpecular = [ 0.0,1.0,0.0];
var GreenlightPosition = [ 0.0,0.0,-3.0,1.0 ];

var BluelightAmbient = [ 0.0,0.0,0.0];
var BluelightDiffuse = [ 0.0,0.0,1.0];
var BluelightSpecular = [ 0.0,0.0,1.0];
var BluelightPosition = [ 0.0,0.0,-3.0,1.0 ];


var materialAmbient = [ 0.0,0.0,0.0];
var materialDiffuse = [ 1.0,1.0,1.0];
var materialSpecular = [ 1.0,1.0,1.0];
var materialShinyness = 128.0;

var RedfirstRotation = 0.0, RedsecondRotation = 0.0;
var GreenfirstRotation = 0.0, GreensecondRotation = 0.0;
var BluefirstRotation = 0.0, BluesecondRotation = 0.0;

var LightAnglezero = 0.0;
var LightAngleone = 0.0;
var LightAngletwo = 0.0;

var sphere = null;

var perspectiveProjectionMatrix;

var gVao_sphere, gVbo_sphere_element;
var gVbo_sphere_position, gVbo_sphere_normal;
var modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
var red_ld_Uniform, red_la_Uniform, red_ls_Uniform, red_lightPosition_Uniform;
var green_ld_Uniform, green_la_Uniform, green_ls_Uniform, green_lightPosition_Uniform;
var blue_ld_Uniform, blue_la_Uniform, blue_ls_Uniform, blue_lightPosition_Uniform;
var ka_Uniform, kd_Uniform, ks_Uniform;
var LKeyPress_Uniform, materialShinyness_Uniform;

var bLKeyPressed= false;

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
			if (bLKeyPressed == false)
			{
				bLKeyPressed = true;
			}
			else {
				bLKeyPressed = false;
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
	
	//vertex shader 
	var vertexShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"precision highp int;"+
	"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform int u_Lkeypress;" +
		"uniform vec4 u_red_lightPosition;" +
		"uniform vec4 u_green_lightPosition;" +
		"uniform vec4 u_blue_lightPosition;" +
		"out vec3 Transformednormal;" +
		"out vec3 RedlightDirection;" +
		"out vec3 GreenlightDirection;" +
		"out vec3 BluelightDirection;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"Transformednormal=(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
		"RedlightDirection =(vec3(u_red_lightPosition - eye_coordinate));" +
		"GreenlightDirection =(vec3(u_green_lightPosition - eye_coordinate));" +
		"BluelightDirection =(vec3(u_blue_lightPosition - eye_coordinate));" +
		"viewer_vector = normalize(vec3(-eye_coordinate.xyz));" +
		"}" +
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
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
			unintialize();
		}
	}
	
	// fragment shader
	var fragmentShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"precision highp float;"+
	"precision highp int;"+
	"out vec4 fragColor;" +
	"in vec3 Red_phong_ads_light;" +
	"in vec3 Green_phong_ads_light;" +
	"in vec3 Blue_phong_ads_light;" +
	"uniform int u_Lkeypress;" +
	"uniform vec3 u_red_la;" +
	"uniform vec3 u_red_ld;" +
	"uniform vec3 u_red_ls;" +
	"uniform vec3 u_green_la;"+
	"uniform vec3 u_green_ld;" +
	"uniform vec3 u_green_ls;" +
	"uniform vec3 u_blue_la;" +
	"uniform vec3 u_blue_ld;" +
	"uniform vec3 u_blue_ls;" +
	"uniform vec3 u_ka;" +
	"uniform vec3 u_kd;" +
	"uniform vec3 u_ks;" +
	"uniform float u_materialShinyness;" +
	"vec3 phong_ads_light;" +
	"in vec3 Transformednormal;" +
	"in vec3 RedlightDirection;" +
	"in vec3 GreenlightDirection;" +
	"in vec3 BluelightDirection;" +
	"in vec3 viewer_vector;" +
	"void main(void)" +
	"{" +
	"if(u_Lkeypress==1)" +
	"{" +
	"vec3 NormalizeTransformednormal = normalize(Transformednormal);"+
	"vec3 NormalizeRedlightDirection = normalize(RedlightDirection);"+
	"vec3 NormalizeGreenlightDirection = normalize(GreenlightDirection);"+
	"vec3 NormalizeBluelightDirection = normalize(BluelightDirection);"+
	"vec3 Normalizeviewer_vector = normalize(viewer_vector);"+

	"float TN_dot_red_ld = max(dot(NormalizeRedlightDirection,NormalizeTransformednormal),0.0f);" +
	"float TN_dot_green_ld = max(dot(NormalizeGreenlightDirection,NormalizeTransformednormal),0.0f);" +
	"float TN_dot_blue_ld = max(dot(NormalizeBluelightDirection,NormalizeTransformednormal),0.0f);" +
	"vec3 Redreflection_vector = reflect(-NormalizeRedlightDirection,NormalizeTransformednormal);" +
	"vec3 Greenreflection_vector = reflect(-NormalizeGreenlightDirection,NormalizeTransformednormal);" +
	"vec3 Bluereflection_vector = reflect(-NormalizeBluelightDirection,NormalizeTransformednormal);" +

	"vec3 Redambient = u_red_la * u_ka;" +
	"vec3 Reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" +
	"vec3 Redspecular = u_red_ls * u_ks * pow(max(dot(Redreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
	"vec3 Red_phong_ads_light = Redambient + Reddiffuse + Redspecular;"+
	"vec3 Greenambient = u_green_la * u_ka;" +
	"vec3 Greendiffuse = u_green_ld * u_kd * TN_dot_green_ld;" +
	"vec3 Greenspecular = u_green_ls * u_ks * pow(max(dot(Greenreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
	"vec3 Green_phong_ads_light = Greenambient + Greendiffuse + Greenspecular;"+
	"vec3 Blueambient = u_blue_la * u_ka;" +
	"vec3 Bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" +
	"vec3 Bluespecular = u_blue_ls * u_ks * pow(max(dot(Bluereflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" +
	"vec3 Blue_phong_ads_light = Blueambient + Bluediffuse + Bluespecular;"+
	"phong_ads_light = Red_phong_ads_light + Green_phong_ads_light + Blue_phong_ads_light;" +
	"}" +
	"else" +
	"{" +
	"phong_ads_light = vec3(1.0f,1.0f,1.0f);" +
	"}" +

	"fragColor = vec4(phong_ads_light,1.0);" +
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");
	
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
	
	modelMatrix_Uniform = gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
	viewMatrix_Uniform =  gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
	projection_Uniform =  gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");
	
	LKeyPress_Uniform = gl.getUniformLocation(shaderProgramObject,"u_Lkeypress");
	
	red_la_Uniform = gl.getUniformLocation(shaderProgramObject, "u_red_la"); 
	red_ld_Uniform= gl.getUniformLocation(shaderProgramObject,"u_red_ld");
	red_ls_Uniform = gl.getUniformLocation(shaderProgramObject,"u_red_ls");
	red_lightPosition_Uniform = gl.getUniformLocation(shaderProgramObject,"u_red_lightPosition");
	
	green_la_Uniform = gl.getUniformLocation(shaderProgramObject, "u_green_la"); 
	green_ld_Uniform= gl.getUniformLocation(shaderProgramObject,"u_green_ld");
	green_ls_Uniform = gl.getUniformLocation(shaderProgramObject,"u_green_ls");
	green_lightPosition_Uniform = gl.getUniformLocation(shaderProgramObject,"u_green_lightPosition");
	
	blue_la_Uniform = gl.getUniformLocation(shaderProgramObject, "u_blue_la"); 
	blue_ld_Uniform= gl.getUniformLocation(shaderProgramObject,"u_blue_ld");
	blue_ls_Uniform = gl.getUniformLocation(shaderProgramObject,"u_blue_ls");
	blue_lightPosition_Uniform = gl.getUniformLocation(shaderProgramObject,"u_blue_lightPosition");
	
	ka_Uniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kd_Uniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ks_Uniform = gl.getUniformLocation(shaderProgramObject, "u_ks");

	materialShinyness_Uniform = gl.getUniformLocation(shaderProgramObject,"u_materialShinyness");
	
	
	
	
	sphere = new Mesh();
	makeSphere(sphere,2.0,30,30)
	
	gl.enable(gl.DEPTH_TEST);
	
	gl.depthFunc(gl.LEQUAL);
	
	gl.enable(gl.CULL_FACE);
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

function update()
{
	LightAnglezero = LightAnglezero - 0.02;
	LightAngleone = LightAngleone + 0.02;
	LightAngletwo = LightAngleone - 0.02;
	RedfirstRotation = 6 * Math.cos(LightAnglezero);
	RedsecondRotation = 6 * Math.sin(LightAnglezero);
	GreenfirstRotation = 6 * Math.cos(LightAngleone);
	GreensecondRotation = 6 * Math.sin(LightAngleone);
	BluefirstRotation = 6 * Math.cos(LightAngletwo);
	BluesecondRotation = 6 * Math.sin(LightAngletwo);
}


function draw()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	
	RedlightPosition[0] = RedfirstRotation;
	RedlightPosition[2] = RedsecondRotation;

	GreenlightPosition[1] = GreenfirstRotation;
	GreenlightPosition[2] = GreensecondRotation;

	BluelightPosition[1] = BluefirstRotation;
	BluelightPosition[0] = BluesecondRotation;
	
	
	if (bLKeyPressed == true)
	{
		gl.uniform1i(LKeyPress_Uniform,1);
		gl.uniform3fv(red_la_Uniform,RedlightAmbient);
		gl.uniform3fv(red_ld_Uniform,RedlightDiffuse);
		gl.uniform3fv(red_ls_Uniform,RedlightSpecular);
		gl.uniform4fv(red_lightPosition_Uniform,RedlightPosition);
		
		gl.uniform3fv(green_la_Uniform,GreenlightAmbient);
		gl.uniform3fv(green_ld_Uniform,GreenlightDiffuse);
		gl.uniform3fv(green_ls_Uniform,GreenlightSpecular);
		gl.uniform4fv(green_lightPosition_Uniform,GreenlightPosition);
				
		gl.uniform3fv(blue_la_Uniform,BluelightAmbient);
		gl.uniform3fv(blue_la_Uniform,BluelightDiffuse);
		gl.uniform3fv(blue_ls_Uniform,BluelightSpecular);
		gl.uniform4fv(blue_lightPosition_Uniform,BluelightPosition);
	
		gl.uniform3fv(ka_Uniform, materialAmbient);
		gl.uniform3fv(kd_Uniform, materialDiffuse);
		gl.uniform3fv(ks_Uniform, materialSpecular);
		gl.uniform1f(materialShinyness_Uniform, materialShinyness);
		
	}
	else
	{
		gl.uniform1i(LKeyPress_Uniform,0);
	}
	
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	
	mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-5.0]);
		
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);
	
	sphere.draw();
		
	
	gl.useProgram(null);
	//animation loop
	update();
	
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	
	
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
		
		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
	
}

function degToRad(degrees)
{
	//code
	return(degrees*Math.PI/180);
}
