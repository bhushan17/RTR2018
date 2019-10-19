#define UNICODE
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
GLuint red_ld_Uniform, red_la_Uniform, red_ls_Uniform, red_lightPosition_Uniform;
GLuint green_ld_Uniform, green_la_Uniform, green_ls_Uniform, green_lightPosition_Uniform;
GLuint blue_ld_Uniform, blue_la_Uniform, blue_ls_Uniform, blue_lightPosition_Uniform;
GLuint ka_Uniform, kd_Uniform, ks_Uniform;
GLuint LKeyPress_Uniform, materialShinyness_Uniform;

int gNumVertices;
int gNumElements;
GLfloat LightAnglezero = 0.0f;
GLfloat LightAngleone = 0.0f;
GLfloat LightAngletwo = 0.0f;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

float RedfirstRotation = 0.0f, RedsecondRotation = 0.0f;
float GreenfirstRotation = 0.0f, GreensecondRotation = 0.0f;
float BluefirstRotation = 0.0f, BluesecondRotation = 0.0f;

float RedlightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float RedlightDiffuse[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightSpecular[4] = { 1.0f,0.0f,0.0f,1.0f };
float RedlightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };

float GreenlightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float GreenlightDiffuse[4] = { 0.0f,1.0f,0.0f,1.0f };
float GreenlightSpecular[4] = { 0.0f,1.0f,0.0f,1.0f };
float GreenlightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };

float BluelightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float BluelightDiffuse[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightSpecular[4] = { 0.0f,0.0f,1.0f,1.0f };
float BluelightPosition[4] = { 0.0f,0.0f,0.0f,1.0f };


float materialAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float materialDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialShinyness = 128.0f;

mat4 perspectiveProjectionMatrix;  //this is from Vmath


bool bLight = false;
bool animation = false;
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
		if (LightAnglezero > 360)
		{

			LightAnglezero = 0.0f;
		}
		else {

			//LightAnglezero = LightAnglezero + 0.01f;
		}
	}

	LightAnglezero = LightAnglezero - 0.02f;
	LightAngleone = LightAngleone + 0.02f;
	LightAngletwo = LightAngleone - 0.02f;
	RedfirstRotation = 10 * cos(LightAnglezero);
	RedsecondRotation = 10 * sin(LightAnglezero);
	GreenfirstRotation = 10 * cos(LightAngleone);
	GreensecondRotation = 10 * sin(LightAngleone);
	BluefirstRotation = 10 * cos(LightAngletwo);
	BluesecondRotation = 10 * sin(LightAngletwo);
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
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_Lkeypress;" \
		"uniform vec4 u_red_lightPosition;" \
		"uniform vec4 u_green_lightPosition;" \
		"uniform vec4 u_blue_lightPosition;" \

		"out vec3 Transformednormal;" \
		"out vec3 RedlightDirection;" \


		"out vec3 GreenlightDirection;" \


		"out vec3 BluelightDirection;" \

		"out vec3 viewer_vector;" \


		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \




		"vec4 eye_coordinate = u_view_matrix * u_model_matrix * vPosition;" \
		"Transformednormal=(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"RedlightDirection =(vec3(u_red_lightPosition - eye_coordinate));" \
		"GreenlightDirection =(vec3(u_green_lightPosition - eye_coordinate));" \
		"BluelightDirection =(vec3(u_blue_lightPosition - eye_coordinate));" \
		"viewer_vector = normalize(vec3(-eye_coordinate.xyz));" \

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

				fprintf(gpFile, "\n VS : %s = ", szInfoLog);
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
		
		"in vec3 Red_phong_ads_light;" \
		"in vec3 Green_phong_ads_light;" \
		"in vec3 Blue_phong_ads_light;" \
		"uniform int u_Lkeypress;" \

		"uniform vec3 u_red_la;" \
		"uniform vec3 u_red_ld;" \
		"uniform vec3 u_red_ls;" \
		
		"uniform vec3 u_green_la;" \
		"uniform vec3 u_green_ld;" \
		"uniform vec3 u_green_ls;" \
		
		"uniform vec3 u_blue_la;" \
		"uniform vec3 u_blue_ld;" \
		"uniform vec3 u_blue_ls;" \
		
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShinyness;" \
		
		"vec3 phong_ads_light;" \

		"in vec3 Transformednormal;" \
		"in vec3 RedlightDirection;" \


		"in vec3 GreenlightDirection;" \


		"in vec3 BluelightDirection;" \
		"in vec3 viewer_vector;" \

		"void main(void)" \
		"{" \
		"if(u_Lkeypress==1)" \
		"{" \

		"vec3 NormalizeTransformednormal = normalize(Transformednormal);"
		"vec3 NormalizeRedlightDirection = normalize(RedlightDirection);"
		"vec3 NormalizeGreenlightDirection = normalize(GreenlightDirection);"
		"vec3 NormalizeBluelightDirection = normalize(BluelightDirection);"
		"vec3 Normalizeviewer_vector = normalize(viewer_vector);"


		"float TN_dot_red_ld = max(dot(NormalizeRedlightDirection,NormalizeTransformednormal),0.0f);" \
		"float TN_dot_green_ld = max(dot(NormalizeGreenlightDirection,NormalizeTransformednormal),0.0f);" \
		"float TN_dot_blue_ld = max(dot(NormalizeBluelightDirection,NormalizeTransformednormal),0.0f);" \
		"vec3 Redreflection_vector = reflect(-NormalizeRedlightDirection,NormalizeTransformednormal);" \
		"vec3 Greenreflection_vector = reflect(-NormalizeGreenlightDirection,NormalizeTransformednormal);" \
		"vec3 Bluereflection_vector = reflect(-NormalizeBluelightDirection,NormalizeTransformednormal);" \

		"vec3 Redambient = u_red_la * u_ka;" \
		"vec3 Reddiffuse = u_red_ld * u_kd * TN_dot_red_ld;" \
		"vec3 Redspecular = u_red_ls * u_ks * pow(max(dot(Redreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" \
		"vec3 Red_phong_ads_light = Redambient + Reddiffuse + Redspecular;"\
		"vec3 Greenambient = u_green_la * u_ka;" \
		"vec3 Greendiffuse = u_green_ld * u_kd * TN_dot_green_ld;" \
		"vec3 Greenspecular = u_green_ls * u_ks * pow(max(dot(Greenreflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" \
		"vec3 Green_phong_ads_light = Greenambient + Greendiffuse + Greenspecular;"\
		"vec3 Blueambient = u_blue_la * u_ka;" \
		"vec3 Bluediffuse = u_blue_ld * u_kd * TN_dot_blue_ld;" \
		"vec3 Bluespecular = u_blue_ls * u_ks * pow(max(dot(Bluereflection_vector , Normalizeviewer_vector ),0.0f),u_materialShinyness);" \
		"vec3 Blue_phong_ads_light = Blueambient + Bluediffuse + Bluespecular;"\
		"phong_ads_light = Red_phong_ads_light + Green_phong_ads_light + Blue_phong_ads_light;" \
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

				fprintf(gpFile, "\n FS : %s = ", szInfoLog);
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

	red_la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_la");
	red_ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_ld");
	red_ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_ls");
	red_lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_red_lightPosition");

	green_la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_green_la");
	green_ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_green_ld");
	green_ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_green_ls");
	green_lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_green_lightPosition");


	blue_la_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_la");
	blue_ld_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_ld");
	blue_ls_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_ls");
	blue_lightPosition_Uniform = glGetUniformLocation(glShaderProgramObject, "u_blue_lightPosition");

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);

	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);





	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);


	return(0);
}


void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display()
{
	//code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	GreenlightPosition[0] = RedfirstRotation;
	GreenlightPosition[2] = RedsecondRotation;

	RedlightPosition[1] = GreenfirstRotation;
	RedlightPosition[2] = GreensecondRotation;

	BluelightPosition[1] = BluefirstRotation;
	BluelightPosition[0] = BluesecondRotation;
	//Declaration of Matrix
	mat4 modelMatrix;
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
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	rotationMatrix = rotate(LightAnglezero,0.0f, 1.0f, 0.0f);
	//rotationMatrix = rotate(angle_cube, 1.0f, 0.0f, 0.0f);
	//lightPosition[0] = (rotationMatrix[0][0] * lightPosition[0])+ (rotationMatrix[0][1] * lightPosition[1]) + (rotationMatrix[0][2] * lightPosition[2]) + (rotationMatrix[0][3] * lightPosition[3]);
	//lightPosition[1] = (rotationMatrix[1][0] * lightPosition[0]) + (rotationMatrix[1][1] * lightPosition[1]) + (rotationMatrix[1][2] * lightPosition[2]) + (rotationMatrix[1][3] * lightPosition[3]);
	//lightPosition[2] = (rotationMatrix[2][0] * lightPosition[0]) + (rotationMatrix[2][1] * lightPosition[1]) + (rotationMatrix[2][2] * lightPosition[2]) + (rotationMatrix[2][3] * lightPosition[3]);
	//lightPosition[3] = (rotationMatrix[3][0] * lightPosition[0]) + (rotationMatrix[3][1] * lightPosition[1]) + (rotationMatrix[3][2] * lightPosition[2]) + (rotationMatrix[3][3] * lightPosition[3]);

	/*rotationMatrix = rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	translateMatrix = translateMatrix * rotationMatrix;*/

	modelMatrix = modelMatrix * translateMatrix;


	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		glUniform1i(LKeyPress_Uniform, 1);
		glUniform3fv(red_la_Uniform, 1 , RedlightAmbient );
		glUniform3fv(red_ld_Uniform, 1, RedlightDiffuse);
		glUniform3fv(red_ls_Uniform, 1, RedlightSpecular);
		glUniform4fv(red_lightPosition_Uniform, 1, RedlightPosition);

		glUniform3fv(green_la_Uniform, 1, GreenlightAmbient);
		glUniform3fv(green_ld_Uniform, 1, GreenlightDiffuse);
		glUniform3fv(green_ls_Uniform, 1, GreenlightSpecular);
		glUniform4fv(green_lightPosition_Uniform, 1, GreenlightPosition);

		glUniform3fv(blue_la_Uniform, 1, BluelightAmbient);
		glUniform3fv(blue_ld_Uniform, 1, BluelightDiffuse);
		glUniform3fv(blue_ls_Uniform, 1, BluelightSpecular);
		glUniform4fv(blue_lightPosition_Uniform, 1, BluelightPosition);

		glUniform3fv(ka_Uniform, 1, materialAmbient);
		glUniform3fv(kd_Uniform, 1, materialDiffuse);
		glUniform3fv(ks_Uniform, 1, materialSpecular);
		glUniform1f(materialShinyness_Uniform, materialShinyness);
		
		
	}
	else
	{
		glUniform1i(LKeyPress_Uniform, 0);
	}
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(gVao_sphere);



	//Draw necessary seen
	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	//unbind vao
	glBindVertexArray(0);

	//unusal program

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
