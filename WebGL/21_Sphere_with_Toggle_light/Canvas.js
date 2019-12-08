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

var vertexShaderObject_perFragment;
var fragmentShaderObject_perFragment;
var shaderProgramObject_perFragment;

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var light_ambient=[0.0,0.0,0.0];
var light_diffuse=[1.0,1.0,1.0];
var light_specular=[1.0,1.0,1.0];
var light_position=[100.0,100.0,100.0,1.0];


var material_ambient=[0.0,0.0,0.0];
var material_diffuse=[1.0,1.0,1.0];
var material_specular=[1.0,1.0,1.0];
var material_shininess=50.0;


var sphere = null;

var perspectiveProjectionMatrix;

var modelMatrixUniform_perFragment, viewMatrixUniform_perFragment, projectionMatrixUniform_perFragment;
var laUniform_perFragment, ldUniform_perFragment, lsUniform_perFragment, lightPositionUniform_perFragment;
var kaUniform_perFragment, kdUniform_perFragment, ksUniform_perFragment, materialShininessUniform_perFragment;
var LKeyPressdUniform_perFragment;

var modelMatrixUniform, viewMatrixUniform, projectionMatrixUniform;
var laUniform, ldUniform, lsUniform, lightPositionUniform;
var kaUniform, kdUniform, ksUniform, materialShininessUniform;
var LKeyPressdUniform;

var bLKeyPressed= false;
var flag = false;
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
		case 118:
		case 86:
			if (flag == false)
			{
				flag = true;
			}
			else {
				flag = false;
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
	
	// ************************** PerFragment *********************
	//vertex shader 
	var vertexShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"precision highp int;"+
	"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform int u_Lkeypress_pf;" +
		"uniform mat4 u_model_matrix_pf;" +
		"uniform mat4 u_view_matrix_pf;" +
		"uniform mat4 u_projection_matrix_pf;" +
		"uniform vec4 u_light_position_pf;" +
		"out vec3 Transformednormal;" +
		"out vec3 lightDirection;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress_pf==1)" +
		"{" +
		"vec4 eye_coordinate = u_view_matrix_pf * u_model_matrix_pf * vPosition;" +
		"Transformednormal=mat3(u_view_matrix_pf * u_model_matrix_pf) * vNormal;" +
		"lightDirection = vec3(u_light_position_pf - eye_coordinate);" +
		"viewer_vector =vec3(-eye_coordinate.xyz);" +
		"}" +
		"gl_Position = u_projection_matrix_pf * u_view_matrix_pf * u_model_matrix_pf * vPosition;" +
		"}";
		
	vertexShaderObject_perFragment=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_perFragment, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject_perFragment);
	if(gl.getShaderParameter(vertexShaderObject_perFragment, gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject_perFragment);
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
		"uniform int u_Lkeypress_pf;" +
		"uniform vec3 u_La_pf;" +
		"uniform vec3 u_Ld_pf;" +
		"uniform vec3 u_Ls_pf;" +
		"uniform vec3 u_Ka_pf;" +
		"uniform vec3 u_Kd_pf;" +
		"uniform vec3 u_Ks_pf;" +
		"uniform float u_material_shininess_pf;" +
		"vec3 phong_ads_light;" +
		"in vec3 Transformednormal;" +
		"in vec3 lightDirection;" +
		"in vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"if(u_Lkeypress_pf==1)" +
		"{" +
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);" +
		"vec3 NormalizelightDirection = normalize(lightDirection);" +
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);" +
		"float TN_dot_ld = max(dot(NormalizelightDirection,NormalizeTransformednormal),0.0f);" +
		"vec3 reflection_vector = reflect(-NormalizelightDirection,NormalizeTransformednormal);" +
		"vec3 ambient = u_La_pf * u_Ka_pf;" +
		"vec3 diffuse = u_Ld_pf * u_Kd_pf * TN_dot_ld;" +
		"vec3 specular = u_Ls_pf * u_Ks_pf * pow(max(dot(reflection_vector , Normalizeviewer_vector ),0.0f),u_material_shininess_pf);" +
		"phong_ads_light = ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" +
		"}" +
		"fragColor = vec4(phong_ads_light,1.0);" +
		"}";
	
	fragmentShaderObject_perFragment=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_perFragment, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject_perFragment);
	if(gl.getShaderParameter(fragmentShaderObject_perFragment, gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObject_perFragment);
		if(error.length>0)
		{
			alert(error);
			unintialize();
		}
	}
	
	//shader program
	shaderProgramObject_perFragment=gl.createProgram();
	gl.attachShader(shaderProgramObject_perFragment, vertexShaderObject_perFragment);
	gl.attachShader(shaderProgramObject_perFragment, fragmentShaderObject_perFragment);
	
	//pre-link binding of shader program object with vertex shader attributes 
	gl.bindAttribLocation(shaderProgramObject_perFragment,WebGLMacros.VDG_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(shaderProgramObject_perFragment,WebGLMacros.VDG_ATTRIBUTE_NORMAL,"vNormal");
	
	//linking
	gl.linkProgram(shaderProgramObject_perFragment);
	if(!gl.getProgramParameter(shaderProgramObject_perFragment, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject_perFragment);
		if(error.length > 0)
		{
			alert(error);
			unintialize();
		}
	}
	
	// get MVP uniform location 	
	
	modelMatrixUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment,"u_model_matrix_pf");
	viewMatrixUniform_perFragment =  gl.getUniformLocation(shaderProgramObject_perFragment,"u_view_matrix_pf");
	projectionMatrixUniform_perFragment =  gl.getUniformLocation(shaderProgramObject_perFragment,"u_projection_matrix_pf");
	
	LKeyPressdUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment,"u_Lkeypress_pf");
	
	laUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment, "u_La_pf"); 
	ldUniform_perFragment= gl.getUniformLocation(shaderProgramObject_perFragment,"u_Ld_pf");
	lsUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment,"u_Ls_pf");
	lightPositionUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment,"u_light_position_pf");
	
	kaUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment, "u_Ka_pf");
	kdUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment, "u_Kd_pf");
	ksUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment, "u_Ks_pf");

	materialShininessUniform_perFragment = gl.getUniformLocation(shaderProgramObject_perFragment,"u_material_shininess_pf");
	
	
	// ******************** PerVertex ******************************
	
	//vertex shader 
	var vertexShaderSourceCode=
	"#version 300 es"+
	"\n"+
	"in vec4 vPosition;"+
	"in vec3 vNormal;"+
	"uniform mat4 u_model_matrix;"+
	"uniform mat4 u_view_matrix;"+
	"uniform mat4 u_projection_matrix;"+
	"uniform mediump int u_LKeyPressed;"+
	"uniform vec3 u_La;"+
	"uniform vec3 u_Ld;"+
	"uniform vec3 u_Ls;"+
	"uniform vec4 u_light_position;"+
	"uniform vec3 u_Ka;"+
	"uniform vec3 u_Kd;"+
	"uniform vec3 u_Ks;"+
	"uniform float u_material_shininess;"+
	"out vec3 phong_ads_color;"+
	"void main(void)" +
	"{" +
	"if(u_LKeyPressed==1)" +
	"{" +
	"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" +
	"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix)* vNormal);"+
	"vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinate.xyz);"+
	"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);"+
	"vec3 ambient = u_La*u_Ka;"+
	"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
	"vec3 reflection_vector = reflect(-light_direction, transformed_normals);"+
	"vec3 viewer_vector = normalize(-eye_coordinate.xyz);"+
	"vec3 specular = u_Ls*u_Ks*pow(max(dot(reflection_vector,viewer_vector),0.0),u_material_shininess);"+
	"phong_ads_color=ambient + diffuse + specular;"+
	"}" +
	"else"+
	"{"+
	"phong_ads_color=vec3(1.0,1.0,1.0);"+
	"}"+
	"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
	"}"
		
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
	"in vec3 phong_ads_color;" +
	"out vec4 FragColor;"+
	"void main(void)" +
	"{"+
	"FragColor = vec4(phong_ads_color,1.0);"+
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
	
	LKeyPressdUniform = gl.getUniformLocation(shaderProgramObject,"u_LKeyPressed");
	
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
	
	if (true == flag)
	{
		gl.useProgram(shaderProgramObject_perFragment);
	}
	else{
		gl.useProgram(shaderProgramObject);
	}
	
	
	if (bLKeyPressed == true)
	{
		if(true == flag)
		{
			gl.uniform1i(LKeyPressdUniform_perFragment,1);
			gl.uniform3fv(laUniform_perFragment,light_ambient);
			gl.uniform3fv(ldUniform_perFragment,light_diffuse);
			gl.uniform3fv(lsUniform_perFragment,light_specular);
			gl.uniform4fv(lightPositionUniform_perFragment,light_position);
		
			gl.uniform3fv(kaUniform_perFragment, material_ambient);
			gl.uniform3fv(kdUniform_perFragment, material_diffuse);
			gl.uniform3fv(ksUniform_perFragment, material_specular);
			gl.uniform1f(materialShininessUniform_perFragment, material_shininess);
		}
		else{
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
		
		
	}
	else
	{
		if(true==flag)
			gl.uniform1i(LKeyPressdUniform_perFragment, 0);
		else
			gl.uniform1i(LKeyPressdUniform, 0);
	}
	
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	
	mat4.translate(modelMatrix,modelMatrix,[0.0,0.0,-5.0]);
	
	if(true==flag)
	{
		gl.uniformMatrix4fv(modelMatrixUniform_perFragment, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform_perFragment,false,viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform_perFragment, false, perspectiveProjectionMatrix);
	}
	else
	{
		gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform,false,viewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
	}
	sphere.draw();
		
	
	gl.useProgram(null);
	//animation loop
	
	
	requestAnimationFrame(draw, canvas);
}

function unintialize()
{
	//code
	
	
	if(shaderProgramObject_perFragment)
	{
		if(fragmentShaderObject_perFragment)
		{
			gl.detachShader(shaderProgramObject_perFragment, fragmentShaderObject_perFragment);
			gl.deleteShader(fragmentShaderObject_perFragment);
			fragmentShaderObject_perFragment=null;
		}
		if(vertexShaderObject_perFragment)
		{
			gl.detachShader(shaderProgramObject_perFragment, vertexShaderObject_perFragment);
			gl.deleteShader(vertexShaderObject_perFragment);
			vertexShaderObject_perFragment=null;
		}
		
		gl.deleteProgram(shaderProgramObject_perFragment);
		shaderProgramObject_perFragment = null;
	}
	
}

function degToRad(degrees)
{
	//code
	return(degrees*Math.PI/180);
}
