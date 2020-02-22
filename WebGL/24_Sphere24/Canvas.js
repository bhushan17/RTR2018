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

var lightAmbient=[0.0,0.0,0.0];
var lightDiffuse=[1.0,1.0,1.0];
var lightSpecular=[1.0,1.0,1.0];
var lightPosition=[100.0,100.0,100.0,1.0];


var MaterialAmbient=[0.0,0.0,0.0];
var MaterialDiffuse=[1.0,1.0,1.0];
var MaterialSpecular=[1.0,1.0,1.0];
var MaterialShianyness=50.0;


var gwidth;
var gheight;
var sphere = null;

var perspectiveProjectionMatrix;

var modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
var ld_Uniform, ld_Uniform, ls_Uniform, lightPosition_Uniform;
var ka_Uniform, kd_Uniform, ks_Uniform, materialShinyness_Uniform;
var LKeyPress_Uniform;

var xrotation = false, yrotation = false, zrotation = false, bLight = false;
var firstRotation = 0.0, secondRotation = 0.0, LightAngle = 0.0;
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
			
		case 120:
		case 88:
			xrotation = true;
			yrotation = false;
			zrotation = false;
			break;
		case 121:
		case 89:
			xrotation = false;
			yrotation = true;
			zrotation = false;
			break;
		case 122:
		case 90:
			xrotation = false;
			yrotation = false;
			zrotation = true;
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
	
	modelMatrix_Uniform = gl.getUniformLocation(shaderProgramObject,"u_model_matrix");
	viewMatrix_Uniform =  gl.getUniformLocation(shaderProgramObject,"u_view_matrix");
	projection_Uniform =  gl.getUniformLocation(shaderProgramObject,"u_projection_matrix");
	
	LKeyPress_Uniform = gl.getUniformLocation(shaderProgramObject,"u_Lkeypress");
	
	la_Uniform = gl.getUniformLocation(shaderProgramObject, "u_La"); 
	ld_Uniform= gl.getUniformLocation(shaderProgramObject,"u_Ld");
	ls_Uniform = gl.getUniformLocation(shaderProgramObject,"u_Ls");
	lightPosition_Uniform = gl.getUniformLocation(shaderProgramObject,"u_light_position");
	
	ka_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
	kd_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
	ks_Uniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");

	materialShinyness_Uniform = gl.getUniformLocation(shaderProgramObject,"u_material_shininess");
	
	sphere = new Mesh();
	makeSphere(sphere,2.0,30,30)
	
	gl.enable(gl.DEPTH_TEST);
	
	gl.depthFunc(gl.LEQUAL);
	
	gl.enable(gl.CULL_FACE);
	//set clear color 
	gl.clearColor(0.2,0.2,0.2,1.0); //blue
	
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
	
	gwidth = canvas.width;
	gheight = canvas.height;
	
	//set the view to match
	gl.viewport(0,0,canvas.width, canvas.height);
	
	
	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0);

		
}



function draw24Sphere() {

	// Material Configuration
	/*GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShianyness[1];*/
		
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var scaleMatrix = mat4.create();

	if (true == xrotation)
	{
		lightPosition[0] = 0.0;
		lightPosition[1] = firstRotation;
		lightPosition[2] = secondRotation;
	}
	else if (true == yrotation)
	{
		lightPosition[0] = firstRotation;
		lightPosition[1] = 0.0;
		lightPosition[2] = secondRotation;
	}
	else if (true == zrotation)
	{
		lightPosition[0] = firstRotation;
		lightPosition[1] = secondRotation;
		lightPosition[2] = -3.0;
	}
	else
	{
		lightPosition[0] = 0.0;
		lightPosition[1] = 0.0;
		lightPosition[2] = 0.0;
	}

	if (bLight == true)
	{
		gl.uniform1i(LKeyPress_Uniform, 1);
		gl.uniform3fv(la_Uniform,  lightAmbient);
		gl.uniform3fv(ld_Uniform,  lightDiffuse);
		gl.uniform3fv(ls_Uniform,  lightSpecular);
		gl.uniform4fv(lightPosition_Uniform, lightPosition);

	}
	else
	{
		gl.uniform1i(LKeyPress_Uniform, 0);
	}
	

	//************* 1st Sphere on 1st column, emerald **************//

	

	MaterialAmbient[0] = 0.0215;
	MaterialAmbient[1] = 0.1745;
	MaterialAmbient[2] = 0.0215;
	//MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.07568;
	MaterialDiffuse[1] = 0.61424;
	MaterialDiffuse[2] = 0.07568;
	//MaterialDiffuse[3] = 1.0;


	MaterialSpecular[0] = 0.633;
	MaterialSpecular[1] = 0.727811;
	MaterialSpecular[2] = 0.633;
	//MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);
	
	gl.viewport(0,(5*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 2nd Sphere on 1st column, jade **************//

	
	MaterialAmbient[0] = 0.135;
	MaterialAmbient[1] = 0.2225;
	MaterialAmbient[2] = 0.1575;
	//MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.54;
	MaterialDiffuse[1] = 0.89;
	MaterialDiffuse[2] = 0.63;
	//MaterialDiffuse[3] = 1.0;


	MaterialSpecular[0] = 0.316228;
	MaterialSpecular[1] = 0.316228;
	MaterialSpecular[2] = 0.316228;
	//MaterialSpecular[3] = 1.0;

	
	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);
	
	gl.viewport(0,(4*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();

	//************* 3rd Sphere on 1st column, obsidian **************//

	MaterialAmbient[0] = 0.05375;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.06625;
	//MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.18275;
	MaterialDiffuse[1] = 0.17;
	MaterialDiffuse[2] = 0.22525;
	//MaterialDiffuse[3] = 1.0;


	MaterialSpecular[0] = 0.332741;
	MaterialSpecular[1] = 0.328634;
	MaterialSpecular[2] = 0.346435;
	//MaterialSpecular[3] = 1.0;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport(0,(3*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 4th Sphere on 1st column, pearl **************//

	
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.20725;
	MaterialAmbient[2] = 0.20725;
	//MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 1.0;
	MaterialDiffuse[1] = 0.829;
	MaterialDiffuse[2] = 0.829;
	//MaterialDiffuse[3] = 1.0;


	MaterialSpecular[0] = 0.296648;
	MaterialSpecular[1] = 0.296648;
	MaterialSpecular[2] = 0.296648;
	//MaterialSpecular[3] = 1.0;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport(0,(2*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 5th Sphere on 1st column, ruby **************//

	
	MaterialAmbient[0] = 0.1745;
	MaterialAmbient[1] = 0.01175;
	MaterialAmbient[2] = 0.01175;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.61424;
	MaterialDiffuse[1] = 0.04136;
	MaterialDiffuse[2] = 0.04136;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.727811;
	MaterialSpecular[1] = 0.626959;
	MaterialSpecular[2] = 0.626959;
	//MaterialSpecular[3] = 1.0;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport(0,(1*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 6th Sphere on 1st column, turquoise **************//

	
	MaterialAmbient[0] = 0.1;
	MaterialAmbient[1] = 0.18725;
	MaterialAmbient[2] = 0.1745;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.396;
	MaterialDiffuse[1] = 0.74151;
	MaterialDiffuse[2] = 0.69102;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.297254;
	MaterialSpecular[1] = 0.30829;
	MaterialSpecular[2] = 0.306678;
	//MaterialSpecular[3] = 1.0;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport(0,0,gwidth/6,gheight/6);
	sphere.draw();




	//************* 1st Sphere on 2nd column, brass **************//

	MaterialAmbient[0] = 0.329412;
	MaterialAmbient[1] = 0.223529;
	MaterialAmbient[2] = 0.027451;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.780392;
	MaterialDiffuse[1] = 0.568627;
	MaterialDiffuse[2] = 0.113725;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.992157;
	MaterialSpecular[1] = 0.941176;
	MaterialSpecular[2] = 0.807843;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 2nd Sphere on 2nd column, bronze **************//

	MaterialAmbient[0] = 0.2125;
	MaterialAmbient[1] = 0.1275;
	MaterialAmbient[2] = 0.054;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.714;
	MaterialDiffuse[1] = 0.4284;
	MaterialDiffuse[2] = 0.18144;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.393548;
	MaterialSpecular[1] = 0.271906;
	MaterialSpecular[2] = 0.166721;
	//MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 3rd Sphere on 2nd column, chrome **************//

	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.25;
	MaterialAmbient[2] = 0.25;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.774597;
	MaterialSpecular[1] = 0.774597;
	MaterialSpecular[2] = 0.774597;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 4th Sphere on 2nd column, pearl **************//

	MaterialAmbient[0] = 0.19125;
	MaterialAmbient[1] = 0.0735;
	MaterialAmbient[2] = 0.0225;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.7038;
	MaterialDiffuse[1] = 0.27048;
	MaterialDiffuse[2] = 0.0828;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.256777;
	MaterialSpecular[1] = 0.137622;
	MaterialSpecular[2] = 0.086014;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 5th Sphere on 2nd column, ruby **************//

	MaterialAmbient[0] = 0.24725;
	MaterialAmbient[1] = 0.1995;
	MaterialAmbient[2] = 0.0745;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.75164;
	MaterialDiffuse[1] = 0.60648;
	MaterialDiffuse[2] = 0.22648;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.628281;
	MaterialSpecular[1] = 0.555802;
	MaterialSpecular[2] = 0.366065;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 6th Sphere on 2nd column, silver **************//

	MaterialAmbient[0] = 0.19225;
	MaterialAmbient[1] = 0.19225;
	MaterialAmbient[2] = 0.19225;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.50754;
	MaterialDiffuse[1] = 0.50754;
	MaterialDiffuse[2] = 0.50754;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.508273;
	MaterialSpecular[1] = 0.508273;
	MaterialSpecular[2] = 0.508273;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((1*gwidth)/4,0,gwidth/6,gheight/6);
	sphere.draw();

	//************* 1st Sphere on 3nd column, black **************//

	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.5;
	MaterialSpecular[1] = 0.5;
	MaterialSpecular[2] = 0.5;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 2nd Sphere on 3nd column, cyan **************//

	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.1;
	MaterialAmbient[2] = 0.06;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.0;
	MaterialDiffuse[1] = 0.50980392;
	MaterialDiffuse[2] = 0.50980392;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.50196078;
	MaterialSpecular[1] = 0.50196078;
	MaterialSpecular[2] = 0.50196078;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 3rd Sphere on 3nd column, green **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.1;
	MaterialDiffuse[1] = 0.35;
	MaterialDiffuse[2] = 0.1;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.45;
	MaterialSpecular[1] = 0.55;
	MaterialSpecular[2] = 0.45;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();





	//************* 4th Sphere on 3nd column, red **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.0;
	MaterialDiffuse[2] = 0.0;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.6;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 5th Sphere on 3nd column, white **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.55;
	MaterialDiffuse[1] = 0.55;
	MaterialDiffuse[2] = 0.55;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.70;
	MaterialSpecular[1] = 0.70;
	MaterialSpecular[2] = 0.70;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 6th Sphere on 3nd column, plastic **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.0;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.6;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.5;
	//MaterialSpecular[3] = 1.0;

	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((2*gwidth)/4,0,gwidth/6,gheight/6);
	sphere.draw();






	//************* 1st Sphere on 4th column, black **************//


	MaterialAmbient[0] = 0.02;
	MaterialAmbient[1] = 0.02;
	MaterialAmbient[2] = 0.02;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.4;
	MaterialSpecular[1] = 0.4;
	MaterialSpecular[2] = 0.4;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform, MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();



	//************* 2nd Sphere on 4th column, cyan **************//


	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();




	//************* 3rd Sphere on 4th column, green **************//

	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	//MaterialDiffuse[3] = 1.0;

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();


	//************* 4th Sphere on 4th column, red **************//

	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.04;
	MaterialSpecular[2] = 0.04;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();







	//************* 5th Sphere on 4th column, white **************//

	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	//MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	//MaterialDiffuse[3] = 1.0;
	
	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	sphere.draw();




	//************* 6th Sphere on 4th column, plastic **************//



	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	//MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	//MaterialDiffuse[3] = 1.0;
	

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	//MaterialSpecular[3] = 1.0;


	gl.uniform3fv(ka_Uniform,  MaterialAmbient);
	gl.uniform3fv(kd_Uniform,  MaterialDiffuse);
	gl.uniform3fv(ks_Uniform,  MaterialSpecular);
	gl.uniform1f(materialShinyness_Uniform, MaterialShianyness);

	mat4.identity(modelMatrix);
	mat4.identity(viewMatrix);
	mat4.identity(translateMatrix);
	
	
	//Do necessary Matrix multiplication
	mat4.translate(modelMatrix,modelMatrix,[0.0, 0.0, -6.0]);
	
	//Send necessary Matrix to shader in respective uniform
	gl.uniformMatrix4fv(modelMatrix_Uniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrix_Uniform,false,viewMatrix);
	gl.uniformMatrix4fv(projection_Uniform, false, perspectiveProjectionMatrix);

	gl.viewport((3*gwidth)/4,0,gwidth/6,gheight/6);
	sphere.draw();

}



function draw()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	draw24Sphere();
		
	gl.useProgram(null);
	//animation loop
	LightAngle = LightAngle + 0.05;
	firstRotation = 6 * Math.cos(LightAngle);
	secondRotation =  6 * Math.sin(LightAngle);
	
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
