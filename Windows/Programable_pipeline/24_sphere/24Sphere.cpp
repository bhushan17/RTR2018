#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include"vmath.h"
#include"Sphere.h"
using namespace vmath;

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib,"Sphere.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")


//******* Macros **********//
#define WIN_WIDTH          800
#define WIN_HEIGHT         600



enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//**** Global variable declaration ************//
bool bFullScreen = false;
DWORD dwStyle;
HWND ghWnd = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;


// Shaders Varible
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint glShaderProgramObject;

GLuint gVao_sphere, gVbo_sphere_element;
GLuint gVbo_sphere_position, gVbo_sphere_normal;
GLuint modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
GLuint ld_Uniform, la_Uniform, ls_Uniform, lightPosition_Uniform;
GLuint ka_Uniform, kd_Uniform, ks_Uniform;
GLuint LKeyPress_Uniform, materialShinyness_Uniform;

int gNumVertices;
int gNumElements;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

float lightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float lightDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };


float MaterialAmbient[4]; //= { 0.0f,0.0f,0.0f,0.0f };
float MaterialDiffuse[4]; //= { 1.0f,1.0f,1.0f,1.0f };
float MaterialSpecular[4]; //= { 1.0f,1.0f,1.0f,1.0f };
float MaterialShianyness;// = 128.0f;

mat4 perspectiveProjectionMatrix;  //this is from Vmath

GLfloat angle_cube = 0.0f;
bool bLight = false;
bool animation = false;

GLfloat LightAngle = 0.0f;


float firstRotation = 0.0f, secondRotation = 0.0f;
int xrotation = false, yrotation = false, zrotation = false;


//********* Global function declaration ***********//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//****** Function Declaration ************//

	int intialize(void);
	void display(void);
	void update(void);

	//******* Variable Declaration ***********//
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	bool bDone = false;
	int iRet = 0;

	//*******************Open Log File ***************//
	if (fopen_s(&gpFile, "Log.txt", "w+") != 0)
	{
		MessageBox(NULL, TEXT("Log file can not be created.!!!"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else {
		fprintf(gpFile, "Log File Created Successfully.!!! \n");
	}
	fclose(gpFile);

	//********* Intialization of WNDCLASSEX *********//
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


	fopen_s(&gpFile, "Log.txt", "a+");
	fprintf(gpFile, "Register class start !!!\n");
	fclose(gpFile);

	// ****** Register above class *************//
	RegisterClassEx(&wndclass);

	fopen_s(&gpFile, "Log.txt", "a+");
	fprintf(gpFile, "Register class complete !!!\n");
	fclose(gpFile);

	//********* Craete Window ********************//
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Bhushan's Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghWnd = hwnd;
	fopen_s(&gpFile, "Log.txt", "a+");
	iRet = intialize();

	if (iRet == -1) {

		fprintf(gpFile, "ChoosePixelFormat() Failed !!!\n");
		DestroyWindow(hwnd);
		fclose(gpFile);
		exit(0);
	}
	else if (iRet == -2) {

		fprintf(gpFile, "SetPixelFormat() Failed !!!\n");
		DestroyWindow(hwnd);
		fclose(gpFile);
		exit(0);
	}
	else if (iRet == -3) {

		fprintf(gpFile, "wgCreateContex() Failed !!!\n");
		DestroyWindow(hwnd);
		fclose(gpFile);
		exit(0);
	}
	else if (iRet == -4) {

		fprintf(gpFile, "wglMakeCurrent() Failed !!!\n");
		DestroyWindow(hwnd);
		fclose(gpFile);
		exit(0);
	}
	else {

		fprintf(gpFile, "\n Intialization Successfully.!!!\n");

	}

	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);



	//************** GAme loop ************************//
	while (false == bDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (WM_QUIT == msg.message) {
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			//****** Play Game ****************//
			if (true == gbActiveWindow) {
				// Here call update
				update();
			}
			else {
				//here call Dispaly for double buffering
				//Play the game actually
			}
			display();
		}
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lParam)
{
	//******** Function Declaration ****************//
	void ToggelFullScreen(void);
	void resize(int, int);
	void update(void);
	void unintialize(void);

	//code

	switch (iMsg)
	{
	case WM_CREATE:
		//MessageBox(hwnd, TEXT("This is WM_CREATE."), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_ESCAPE:
			//MessageBox(hwnd, TEXT("This is VK_ESCAPE"), TEXT("Bhushan msg"), MB_OK);
			DestroyWindow(hwnd);
			break;

		case 0x46:
			ToggelFullScreen();
			break;
		}
		break;
	case WM_CHAR:
		switch (wparam)
		{
		case 'L':
		case 'l':
			if (bLight == false)
			{
				bLight = true;
			}
			else {
				bLight = false;
			}
			break;
		case 'A':
		case 'a':
			if (animation == false)
			{
				//Do nothing;
				animation = true;
			}
			else {
				animation = false;

			}
			break;
		case 'X':
		case 'x':
			xrotation = true;
			yrotation = false;
			zrotation = false;
			break;
		case 'Y':
		case 'y':
			xrotation = false;
			yrotation = true;
			zrotation = false;
			break;
		case 'Z':
		case 'z':
			xrotation = false;
			yrotation = false;
			zrotation = true;
			break;

		}
		break;
	case WM_LBUTTONDOWN:
		//MessageBox(hwnd, TEXT("This is WM_LBUTTONDOWN"), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_RBUTTONDOWN:
		//MessageBox(hwnd, TEXT("This is WM_RBUTTONDOWN"), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_DESTROY:
		unintialize();
		//MessageBox(hwnd, TEXT("This is WM_DESTROY"), TEXT("Bhushan msg"), MB_OK);
		PostQuitMessage(0);
		break;

	case WM_SETFOCUS:
		//MessageBox(hwnd, TEXT("This is WM_SETFOCUS"), TEXT("Bhushan msg"), MB_OK);
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		//MessageBox(hwnd, TEXT("This is WM_KILLFOCUS"), TEXT("Bhushan msg"), MB_OK);
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_ERASEBKGND:
		return(0);

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wparam, lParam));
}

void update(void)
{
	if (animation == true)
	{
		if (angle_cube > 360)
		{

			angle_cube = 0.0f;
		}
		else {

			angle_cube = angle_cube + 2.0f;
		}
	}

	LightAngle = LightAngle + 0.05f;
	firstRotation = 3 * cos(LightAngle);
	secondRotation =  3 * sin(LightAngle);


}

int intialize(void)
{
	fprintf(gpFile, "Inside Intialization.!!!\n");
	// ******* Function declaration **********//
	void resize(int, int);
	void unintialize(void);
	//****** Variable declaration ************//

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;


	fclose(gpFile);
	//******** code **********************//
	//*********** Intialize pfd structure **********//
	memset((void*)&pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	fopen_s(&gpFile, "Log.txt", "a+");
	fprintf(gpFile, "Inside Intialization after pfd structructure.!!!\n");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");

	ghdc = GetDC(ghWnd);


	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0) {
		return (-1);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		return (-2);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL) {
		return(-3);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
		return(-4);
	}
	GLenum result;
	result = glewInit();

	if (result != GLEW_OK)
	{
		fprintf(gpFile, "glewInit failed !!!");
		unintialize();
		//DestroyWindow();
	}

	fprintf(gpFile, "Shader Code start");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");

	//Define VertexShader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Specify/Write vertex shader code

	const GLchar *VertexShaderSourseCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform int u_Lkeypress;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_lightPosition;" \
		"out vec3 Transformednormal;" \
		"out vec3 lightDirection;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" \
		"Transformednormal=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"lightDirection = vec3(u_lightPosition - eye_coordinate);" \
		"viewer_vector =vec3(-eye_coordinate.xyz);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";



	fprintf(gpFile, "\n Vertex Shader code start");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");

	//Specify above code to Vertex Shader object
	glShaderSource(gVertexShaderObject, 1, (GLchar const**)&VertexShaderSourseCode, NULL);

	GLint iShaderCompileStatus = 0;
	GLint iInfologLength = 0;
	GLchar *szInfoLog = NULL;

	fprintf(gpFile, "\n Vertex Shader source code attached");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");

	//Compile the Vertex shader
	glCompileShader(gVertexShaderObject);
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	fprintf(gpFile, "\n Vertex Shader compile error checking start");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");


	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfologLength, &written, szInfoLog);

				fprintf(gpFile, "\n Vertex shader compilation error : %s = ", szInfoLog);
				fclose(gpFile);
				fopen_s(&gpFile, "Log.txt", "a+");
				free(szInfoLog);
				unintialize();
				DestroyWindow(ghWnd);
				exit(0);

			}
		}
	}





	//Define FragmentShader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Specify/Write fragment shader code

	const GLchar *FragmentShaderSourseCode =
		"#version 450 core" \
		"\n"
		"out vec4 fragColor;" \
		"uniform int u_Lkeypress;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShinyness;" \
		"vec3 phong_ads_light;" \
		"in vec3 Transformednormal;" \
		"in vec3 lightDirection;" \
		"in vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \
		"vec3 NormalizeTransformednormal = normalize(Transformednormal);"
		"vec3 NormalizelightDirection = normalize(lightDirection);"
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);"
		"float TN_dot_ld = max(dot(NormalizelightDirection,NormalizeTransformednormal),0.0f);" \
		"vec3 reflection_vector = reflect(-NormalizelightDirection,NormalizeTransformednormal);" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 diffuse = u_ld * u_kd * TN_dot_ld;" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0f,1.0f,1.0f);" \
		"}" \
		"fragColor = vec4(phong_ads_light,1.0);" \
		"}";
	//Specify above code to Fragment Shader object
	glShaderSource(gFragmentShaderObject, 1, (GLchar const**)&FragmentShaderSourseCode, NULL);

	iShaderCompileStatus = 0;
	iInfologLength = 0;


	//Compile the Fragment shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfologLength, &written, szInfoLog);

				fprintf(gpFile, "\n Fragment shader compilation error \n : %s = ", szInfoLog);
				fclose(gpFile);
				fopen_s(&gpFile, "Log.txt", "a+");
				free(szInfoLog);
				unintialize();
				DestroyWindow(ghWnd);
				exit(0);

			}
		}
	}

	// Create Shader Program Object
	glShaderProgramObject = glCreateProgram();

	//Attach Vertex Shader to the Shader Program
	glAttachShader(glShaderProgramObject, gVertexShaderObject);

	//Attach Fragnebt Shader
	glAttachShader(glShaderProgramObject, gFragmentShaderObject);

	// Prelinking binding to Vertex attribute
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");

	//Link the Shader Program
	glLinkProgram(glShaderProgramObject);

	GLint iProgramLinkStatus = 0;
	iInfologLength = 0;
	//ErrorCheking
	glGetProgramiv(glShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);

	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(glShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfologLength);
		if (iInfologLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfologLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(glShaderProgramObject, iInfologLength, &written, szInfoLog);


				fprintf(gpFile, "\n Program link : %s = ", szInfoLog);
				fclose(gpFile);
				fopen_s(&gpFile, "Log.txt", "a+");
				free(szInfoLog);
				unintialize();
				DestroyWindow(ghWnd);
				exit(0);
			}
		}
	}

	//Post linking retriving/getting uniform location 
	modelMatrix_Uniform = glGetUniformLocation(glShaderProgramObject, "u_model_matrix");
	viewMatrix_Uniform = glGetUniformLocation(glShaderProgramObject, "u_view_matrix");
	projection_Uniform = glGetUniformLocation(glShaderProgramObject, "u_projection_matrix");

	la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_la");
	ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ld");
	ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ls");
	lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_lightPosition");

	ka_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ka");
	kd_Uniform = glGetUniformLocation(glShaderProgramObject, "u_kd");
	ks_Uniform = glGetUniformLocation(glShaderProgramObject, "u_ks");
	materialShinyness_Uniform = glGetUniformLocation(glShaderProgramObject, "u_materialShinyness");
	LKeyPress_Uniform = glGetUniformLocation(glShaderProgramObject, "u_Lkeypress");

	//Ortho fixfunction program



	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//Create Rectangle vao
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);

	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	



	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);


	return(0);
}

int gwidth;
int gheight;

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	gwidth = width;
	gheight = height;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	if (width > height)
	{
		perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	}
	else {
		perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)height / (GLfloat)width, 0.1f, 100.0f);
	}
	

	
}



void draw24Sphere() {

	// Material Configuration
	/*GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShianyness[1];*/
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 translateMatrix;
	mat4 rotationMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 scaleMatrix;

	if (true == xrotation)
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = firstRotation;
		lightPosition[2] = secondRotation;
	}
	else if (true == yrotation)
	{
		lightPosition[0] = firstRotation;
		lightPosition[1] = 0.0f;
		lightPosition[2] = secondRotation;
	}
	else if (true == zrotation)
	{
		lightPosition[0] = firstRotation;
		lightPosition[1] = secondRotation;
		lightPosition[2] = -3.0f;
	}
	else
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
	}

	if (bLight == TRUE)
	{
		glUniform1i(LKeyPress_Uniform, 1);
		glUniform3fv(la_Uniform, 1, lightAmbient);
		glUniform3fv(ld_Uniform, 1, lightDiffuse);
		glUniform3fv(ls_Uniform, 1, lightSpecular);
		glUniform4fv(lightPosition_Uniform, 1, lightPosition);

	}
	else
	{
		glUniform1i(LKeyPress_Uniform, 0);
	}
	

	//************* 1st Sphere on 1st column, emerald **************//

	

	MaterialAmbient[0] = 0.0215;
	MaterialAmbient[1] = 0.1745;
	MaterialAmbient[2] = 0.0215;
	MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.07568;
	MaterialDiffuse[1] = 0.61424;
	MaterialDiffuse[2] = 0.07568;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.633;
	MaterialSpecular[1] = 0.727811;
	MaterialSpecular[2] = 0.633;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);
	
	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;
	//modelMatrix = normalize( modelMatrix);

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,(5*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 2nd Sphere on 1st column, jade **************//

	
	MaterialAmbient[0] = 0.135;
	MaterialAmbient[1] = 0.2225;
	MaterialAmbient[2] = 0.1575;
	MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.54;
	MaterialDiffuse[1] = 0.89;
	MaterialDiffuse[2] = 0.63;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.316228;
	MaterialSpecular[1] = 0.316228;
	MaterialSpecular[2] = 0.316228;
	MaterialSpecular[3] = 1.0;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6 * 128;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,(4*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 3rd Sphere on 1st column, obsidian **************//

	MaterialAmbient[0] = 0.05375;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.06625;
	MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 0.18275;
	MaterialDiffuse[1] = 0.17;
	MaterialDiffuse[2] = 0.22525;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.332741;
	MaterialSpecular[1] = 0.328634;
	MaterialSpecular[2] = 0.346435;
	MaterialSpecular[3] = 1.0;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6 * 128;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,(3*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 4th Sphere on 1st column, pearl **************//

	
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.20725;
	MaterialAmbient[2] = 0.20725;
	MaterialAmbient[3] = 1.0;


	MaterialDiffuse[0] = 1.0;
	MaterialDiffuse[1] = 0.829;
	MaterialDiffuse[2] = 0.829;
	MaterialDiffuse[3] = 1.0f;


	MaterialSpecular[0] = 0.296648;
	MaterialSpecular[1] = 0.296648;
	MaterialSpecular[2] = 0.296648;
	MaterialSpecular[3] = 1.0;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6 * 128;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,(2*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 5th Sphere on 1st column, ruby **************//

	
	MaterialAmbient[0] = 0.1745;
	MaterialAmbient[1] = 0.01175;
	MaterialAmbient[2] = 0.01175;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.61424;
	MaterialDiffuse[1] = 0.04136;
	MaterialDiffuse[2] = 0.04136;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.727811;
	MaterialSpecular[1] = 0.626959;
	MaterialSpecular[2] = 0.626959;
	MaterialSpecular[3] = 1.0;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6 * 128;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,(1*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 6th Sphere on 1st column, turquoise **************//

	
	MaterialAmbient[0] = 0.1;
	MaterialAmbient[1] = 0.18725;
	MaterialAmbient[2] = 0.1745;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.396;
	MaterialDiffuse[1] = 0.74151;
	MaterialDiffuse[2] = 0.69102;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.297254;
	MaterialSpecular[1] = 0.30829;
	MaterialSpecular[2] = 0.306678;
	MaterialSpecular[3] = 1.0;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	MaterialShianyness = 0.6 * 128;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport(0,0,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);




	//************* 1st Sphere on 2nd column, brass **************//

	MaterialAmbient[0] = 0.329412;
	MaterialAmbient[1] = 0.223529;
	MaterialAmbient[2] = 0.027451;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.780392;
	MaterialDiffuse[1] = 0.568627;
	MaterialDiffuse[2] = 0.113725;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.992157;
	MaterialSpecular[1] = 0.941176;
	MaterialSpecular[2] = 0.807843;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 2nd Sphere on 2nd column, bronze **************//

	MaterialAmbient[0] = 0.2125;
	MaterialAmbient[1] = 0.1275;
	MaterialAmbient[2] = 0.054;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.714;
	MaterialDiffuse[1] = 0.4284;
	MaterialDiffuse[2] = 0.18144;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.393548;
	MaterialSpecular[1] = 0.271906;
	MaterialSpecular[2] = 0.166721;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 3rd Sphere on 2nd column, chrome **************//

	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.25;
	MaterialAmbient[2] = 0.25;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.774597;
	MaterialSpecular[1] = 0.774597;
	MaterialSpecular[2] = 0.774597;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 4th Sphere on 2nd column, pearl **************//

	MaterialAmbient[0] = 0.19125;
	MaterialAmbient[1] = 0.0735;
	MaterialAmbient[2] = 0.0225;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.7038;
	MaterialDiffuse[1] = 0.27048;
	MaterialDiffuse[2] = 0.0828;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.256777;
	MaterialSpecular[1] = 0.137622;
	MaterialSpecular[2] = 0.086014;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 5th Sphere on 2nd column, ruby **************//

	MaterialAmbient[0] = 0.24725;
	MaterialAmbient[1] = 0.1995;
	MaterialAmbient[2] = 0.0745;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.75164;
	MaterialDiffuse[1] = 0.60648;
	MaterialDiffuse[2] = 0.22648;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.628281;
	MaterialSpecular[1] = 0.555802;
	MaterialSpecular[2] = 0.366065;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 6th Sphere on 2nd column, silver **************//

	MaterialAmbient[0] = 0.19225;
	MaterialAmbient[1] = 0.19225;
	MaterialAmbient[2] = 0.19225;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.50754;
	MaterialDiffuse[1] = 0.50754;
	MaterialDiffuse[2] = 0.50754;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.508273;
	MaterialSpecular[1] = 0.508273;
	MaterialSpecular[2] = 0.508273;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((1*gwidth)/4,0,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);







	//************* 1st Sphere on 3nd column, black **************//

	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.5;
	MaterialSpecular[1] = 0.5;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 2nd Sphere on 3nd column, cyan **************//

	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.1;
	MaterialAmbient[2] = 0.06;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.0;
	MaterialDiffuse[1] = 0.50980392;
	MaterialDiffuse[2] = 0.50980392;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.50196078;
	MaterialSpecular[1] = 0.50196078;
	MaterialSpecular[2] = 0.50196078;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 3rd Sphere on 3nd column, green **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.1;
	MaterialDiffuse[1] = 0.35;
	MaterialDiffuse[2] = 0.1;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.45;
	MaterialSpecular[1] = 0.55;
	MaterialSpecular[2] = 0.45;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);





	//************* 4th Sphere on 3nd column, red **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.0;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.6;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 5th Sphere on 3nd column, white **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.55;
	MaterialDiffuse[1] = 0.55;
	MaterialDiffuse[2] = 0.55;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.70;
	MaterialSpecular[1] = 0.70;
	MaterialSpecular[2] = 0.70;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 6th Sphere on 3nd column, plastic **************//
	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.6;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((2*gwidth)/4,0,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);







	//************* 1st Sphere on 4th column, black **************//


	MaterialAmbient[0] = 0.02;
	MaterialAmbient[1] = 0.02;
	MaterialAmbient[2] = 0.02;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.4;
	MaterialSpecular[1] = 0.4;
	MaterialSpecular[2] = 0.4;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,(5*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);



	//************* 2nd Sphere on 4th column, cyan **************//


	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,(4*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);




	//************* 3rd Sphere on 4th column, green **************//

	
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,(3*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);


	//************* 4th Sphere on 4th column, red **************//

	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.04;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,(2*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);







	//************* 5th Sphere on 4th column, white **************//

	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	
	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	
	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,(1*gheight)/6,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);




	//************* 6th Sphere on 4th column, plastic **************//



	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;


	MaterialShianyness = 0.6 * 128;

	glUniform3fv(ka_Uniform, 1, MaterialAmbient);
	glUniform3fv(kd_Uniform, 1, MaterialDiffuse);
	glUniform3fv(ks_Uniform, 1, MaterialSpecular);
	glUniform1f(materialShinyness_Uniform, MaterialShianyness);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	//scaleMatrix = scale(0.3f, 0.3f, 0.3f);

	modelMatrix = modelMatrix * translateMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glViewport((3*gwidth)/4,0,gwidth/6,gheight/6);
	glBindVertexArray(gVao_sphere);
	//Draw necessary seen
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);




}


void display()
{
	//code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	

	/*MaterialAmbient[0] = 0.0215;
	MaterialAmbient[1] = 0.1745;
	MaterialAmbient[2] = 0.0215;
	MaterialAmbient[3] = 1.0;

	MaterialDiffuse[0] = 0.07568;
	MaterialDiffuse[1] = 0.61424;
	MaterialDiffuse[2] = 0.07568;
	MaterialDiffuse[3] = 1.0f;

	MaterialSpecular[0] = 0.633;
	MaterialSpecular[1] = 0.727811;
	MaterialSpecular[2] = 0.633;
	MaterialSpecular[3] = 1.0;

	MaterialShianyness = 0.6 * 128;*/
	

	//Declaration of Matrix
/*	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 translateMatrix;
	mat4 rotationMatrix;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 0.0f, -2.0f);
	rotationMatrix = rotate(angle_cube, angle_cube, angle_cube);
	//rotationMatrix = rotate(angle_cube, 1.0f, 0.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	/*rotationMatrix = rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	translateMatrix = translateMatrix * rotationMatrix;*/

	/*modelMatrix = modelMatrix * translateMatrix;


	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);*/
	glUseProgram(glShaderProgramObject);

	
	//bind wit vao (This will avoid many repeated binding with vbo)
	/*glBindVertexArray(gVao_sphere);



	//Draw necessary seen
	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);*/
	
	//unusal program
	draw24Sphere();
	glUseProgram(0);

	SwapBuffers(ghdc);
}






void unintialize(void) {

	// ************* If screen is fullscreen then resize then Unintialize ***********//
	if (true == bFullScreen) {
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}


	if (gVao_sphere) {
		glDeleteBuffers(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	if (gVbo_sphere_position) {
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_sphere_normal) {
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	//safe release shader
	if (glShaderProgramObject) {
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(glShaderProgramObject);
		//ask program How many shader attached with you

		glGetProgramiv(glShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShader = (GLuint*)malloc(shaderCount * sizeof(GLuint));
		if (pShader) {
			glGetAttachedShaders(glShaderProgramObject, shaderCount, &shaderCount, pShader);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//Detached one by one
				glDetachShader(glShaderProgramObject, pShader[shaderNumber]);
				//delete detach shader
				glDeleteShader(pShader[shaderNumber]);
				pShader[shaderNumber] = 0;

			}
			free(pShader);
		}
		glDeleteProgram(glShaderProgramObject);
		glShaderProgramObject = 0;
		glUseProgram(0);
	}


	if (wglGetCurrentContext() == ghrc) {
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc) {
		ReleaseDC(ghWnd, ghdc);
		ghdc = NULL;
	}

	if (gpFile) {
		fprintf(gpFile, "Log file close Successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}


}

void ToggelFullScreen(void)
{
	MONITORINFO mI;
	if (bFullScreen == false)
	{
		dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		mI = { sizeof(MONITORINFO) };
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghWnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &mI))
			{
				SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghWnd,
					HWND_TOP,
					mI.rcMonitor.left,
					mI.rcMonitor.top,
					mI.rcMonitor.right - mI.rcMonitor.left,
					mI.rcMonitor.bottom - mI.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		bFullScreen = true;
	}
	else
	{
		SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = false;
	}
}
