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

var light_ambient=[0.0,0.0,0.0];
var light_diffuse=[1.0, 0.5, 0.05];
var light_diffuse1=[0.1, 0.1, 0.5];
var light_diffuse2=[0.7, 0.7, 0.7];
var light_specular=[1.0,1.0,1.0];
var light_position=[100.0,100.0,100.0,1.0];


var material_ambient=[0.0,0.0,0.0];
var material_diffuse=[1.0,1.0,1.0];
var material_specular=[1.0,1.0,1.0];
var material_shininess=50.0;


var sphere = null;

var perspectiveProjectionMatrix;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var laUniform, ldUniform, lsUniform, lightPositionUniform;
var kaUniform, kdUniform, ksUniform, materialShininessUniform;
var LKeyPressdUniform;

var bLKeyPressed= false;
var year = 0.0;
var day = 0.0;
var moon = 0.0;
var moonself = 0.0;
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
			if (bLKeyPressed == false)
			{
				bLKeyPressed = true;
			}
			else {
				bLKeyPressed = false;
			}
			break;
		case 65:
			if (animation == false)
			{
				//Do nothing;
				animation = true;
			}
			else {
				animation = false;

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
		"uniform int u_Lkeypress;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform vec4 u_light_position;" +
		"out vec3 Transformednormal;" +
		"out vec3 lightDirection;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
		"Transformednormal=mat3(u_view_matrix * u_model_matrix) * vNormal;" +
		"lightDirection = vec3(u_light_position - eye_coordinate);" +
		"viewer_vector =vec3(-eye_coordinate.xyz);" +
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
		"uniform int u_Lkeypress;" +
		"uniform vec3 u_La;" +
		"uniform vec3 u_Ld;" +
		"uniform vec3 u_Ls;" +
		"uniform vec3 u_Ka;" +
		"uniform vec3 u_Kd;" +
		"uniform vec3 u_Ks;" +
		"uniform float u_material_shininess;" +
		"vec3 phong_ads_light;" +
		"in vec3 Transformednormal;" +
		"in vec3 lightDirection;" +
		"in vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress==1)" +
		"{" +
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);" +
		"vec3 NormalizelightDirection = normalize(lightDirection);" +
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);" +
		"float TN_dot_ld = max(dot(NormalizelightDirection,NormalizeTransformednormal),0.0f);" +
		"vec3 reflection_vector = reflect(-NormalizelightDirection,NormalizeTransformednormal);" +
		"vec3 ambient = u_La * u_Ka;" +
		"vec3 diffuse = u_Ld * u_Kd * TN_dot_ld;" +
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector , Normalizeviewer_vector ),0.0f),u_material_shininess);" +
		"phong_ads_light = ambient + diffuse + specular;" +
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
	
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
	viewMatrixUniform =  gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
	projectionMatrixUniform =  gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");
	
	LKeyPressdUniform = gl.getUniformLocation(shaderProgramObject,"u_Lkeypress");
	
	laUniform = gl.getUniformLocation(shaderProgramObject, "u_La"); 
	ldUniform= gl.getUniformLocation(shaderProgramObject,"u_Ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject,"u_Ls");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject,"u_light_position");
	
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");

	materialShininessUniform = gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
	
	sphere = new Mesh();
	makeSphere(sphere,2.0,30,30)
	
	gl.enable(gl.DEPTH_TEST);
	
	gl.depthFunc(gl.LEQUAL);
	
	gl.enable(gl.CULL_FACE);
	//set clear color 
	gl.clearColor(0.3,0.3,0.3,1.0); //blue
	
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
	
	if (bLKeyPressed == true)
	{
		gl.uniform1i(LKeyPressdUniform,1);
		gl.uniform3fv(laUniform,light_ambient);
		gl.uniform3fv(ldUniform,light_diffuse);
		gl.uniform3fv(lsUniform,light_specular);
		gl.uniform4fv(lightPositionUniform,light_position);
		
		gl.uniform3fv(kaUniform, material_ambient);
		gl.uniform3fv(kdUniform, material_diffuse);
		gl.uniform3fv(ksUniform, material_specular);
		gl.uniform1f(materialShininessUniform, material_shininess);
		
	}
	else
	{
		gl.uniform1i(LKeyPressdUniform, 0);
	}
	
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	var FirstmodelMatrix=mat4.create();
	var SecondmodelMatrix=mat4.create();
	
	
	
	mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-8.0]);
			
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
	sphere.draw();
	
	
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(year));
	mat4.translate(modelMatrix,modelMatrix,[4.3,0.0,0.0]);
	mat4.scale(modelMatrix,modelMatrix,[0.5, 0.5, 0.5]);
	
	SecondmodelMatrix[0] = modelMatrix[0];
	SecondmodelMatrix[1] = modelMatrix[1];
	SecondmodelMatrix[2] = modelMatrix[2];
	SecondmodelMatrix[3] = modelMatrix[3];
	SecondmodelMatrix[4] = modelMatrix[4];
	SecondmodelMatrix[5] = modelMatrix[5];
	SecondmodelMatrix[6] = modelMatrix[6];
	SecondmodelMatrix[7] = modelMatrix[7];
	SecondmodelMatrix[8] = modelMatrix[8];
	SecondmodelMatrix[9] = modelMatrix[9];
	SecondmodelMatrix[10] = modelMatrix[10];
	SecondmodelMatrix[11] = modelMatrix[11];
	SecondmodelMatrix[12] = modelMatrix[12];
	SecondmodelMatrix[13] = modelMatrix[13];
	SecondmodelMatrix[14] = modelMatrix[14];
	SecondmodelMatrix[15] = modelMatrix[15];
	
	mat4.rotateX(modelMatrix,modelMatrix,degToRad(90));
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(day));
	
	if (bLKeyPressed == true)
	{
		gl.uniform3fv(ldUniform,light_diffuse1);
	}
	
	
	
	
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
	sphere.draw();
	
	
	modelMatrix[0] = SecondmodelMatrix[0];
	modelMatrix[1] = SecondmodelMatrix[1];
	modelMatrix[2] = SecondmodelMatrix[2];
	modelMatrix[3] = SecondmodelMatrix[3];
	modelMatrix[4] = SecondmodelMatrix[4];
	modelMatrix[5] = SecondmodelMatrix[5];
	modelMatrix[6] = SecondmodelMatrix[6];
	modelMatrix[7] = SecondmodelMatrix[7];
	modelMatrix[8] = SecondmodelMatrix[8];
	modelMatrix[9] = SecondmodelMatrix[9];
	modelMatrix[10] = SecondmodelMatrix[10];
	modelMatrix[11] = SecondmodelMatrix[11];
	modelMatrix[12] = SecondmodelMatrix[12];
	modelMatrix[13] = SecondmodelMatrix[13];
	modelMatrix[14] = SecondmodelMatrix[14];
	modelMatrix[15] = SecondmodelMatrix[15];
	
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(moon));
	mat4.translate(modelMatrix,modelMatrix,[4.0,0.0,0.0]);
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(90));
	mat4.rotateY(modelMatrix,modelMatrix,degToRad(moonself));
	
	
	mat4.scale(modelMatrix,modelMatrix,[0.5, 0.5, 0.5]);
		
	if (bLKeyPressed == true)
	{
		gl.uniform3fv(ldUniform,light_diffuse2);
	}	
		
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
	sphere.draw();
	
	
	gl.useProgram(null);
	//animation loop
	
	if (animation == true)
	{
		if (year < 360) {
			year = (year + 0.5);
		}
		else {
			year = 0;
		}

		if (day < 360) {

			day = (day + 2);

		}
		else {

			day = 0;

		}

		if (moon < 360) {
			moon = (moon + 2);
		}
		else {
			moon = 0;
		}

		if (moonself < 360) {
			moonself = (moonself + 2);
		}
		else {
			moonself = 0;
		}
	}
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
