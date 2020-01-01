#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include"vmath.h"

#include "texture.h"
using namespace vmath;

//******** Linker to link the OpenGl Libraray **********//

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


GLuint modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
GLuint ld_Uniform, la_Uniform, ls_Uniform, lightPosition_Uniform;
GLuint ka_Uniform, kd_Uniform, ks_Uniform;
GLuint LKeyPress_Uniform, materialShinyness_Uniform;

GLuint vao_cube;
GLuint vbo_position_cube, vbo_texture_cube, vbo_normal_cube, vbo_color_cube;
GLuint textureCube, samplerUniform;;

float lightAmbient[4] = { 0.25f,0.25f,0.25f,0.1f };
float lightDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightPosition[4] = { 100.0f,100.0f,100.0f,1.0f };


float materialAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float materialDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialShinyness = 128.0f;

mat4 perspectiveProjectionMatrix;  //this is from Vmath

GLfloat angle_cube = 0.0f;
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
		if (angle_cube > 360)
		{

			angle_cube = 0.0f;
		}
		else {

			angle_cube = angle_cube + 0.5f;
		}
	}
}



BOOL loadTexture(GLuint *texture, TCHAR imageResouceID[])
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResouceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);
	}

	return (bStatus);
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
		"in vec2 vTexquard;" \
		"in vec3 vNormal;" \
		"in vec4 vColor;" \
		"uniform int u_Lkeypress;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_lightPosition;" \
		"out vec3 Transformednormal;" \
		"out vec3 lightDirection;" \
		"out vec3 viewer_vector;" \
		"out vec2 out_texquard;" \
		"out vec4 out_color;" \
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
		"out_texquard = vTexquard;" \
		"out_color = vColor;" \
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
		"uniform sampler2D u_sampler;" \
		"uniform float u_materialShinyness;" \
		"vec3 phong_ads_light;" \
		"in vec3 Transformednormal;" \
		"in vec3 lightDirection;" \
		"in vec3 viewer_vector;" \
		"in vec2 out_texquard;" \
		"in vec4 out_color;" \
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
		/*"fragColor = vec4(phong_ads_light,1.0);" \*/
		"fragColor =  vec4(phong_ads_light,1.0) * texture(u_sampler, out_texquard) * out_color; "
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
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexquard");
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

	samplerUniform = glGetUniformLocation(glShaderProgramObject, "u_sampler");

	//Ortho fixfunction program


	const GLfloat cubeVertices[] = { 1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0f, 1.0f,
										-1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0f, 0.0f,
										-1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0f, 0.0f,
										1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0f, 1.0f,

										1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0,-1.0,0.0, 1.0f, 1.0f,
										-1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0,-1.0,0.0, 0.0f, 1.0f,
										-1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0,-1.0,0.0, 0.0f, 0.0f,
										1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0,-1.0,0.0, 1.0f, 0.0f,

										1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0,0.0,1.0, 0.0f, 0.0f,
										-1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0,0.0,1.0, 1.0f, 0.0f,
										-1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0,0.0,1.0, 1.0f, 1.0f,
										1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0,0.0,1.0, 0.0f, 1.0f,

										1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0,0.0,-1.0, 1.0f, 0.0f,
										-1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0,0.0,-1.0, 1.0f, 1.0f,
										-1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0,0.0,-1.0, 0.0f, 1.0f,
										1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0,0.0,-1.0, 0.0f, 0.0f,

										1.0, 1.0, -1.0, 1.0, 0.0, 1.0, 1.0,0.0,0.0, 1.0f, 0.0f,
										1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0,0.0,0.0, 1.0f, 1.0f,
										1.0, -1.0, 1.0, 1.0, 0.0, 1.0, 1.0,0.0,0.0, 0.0f, 1.0f,
										1.0, -1.0, -1.0, 1.0, 0.0, 1.0, 1.0,0.0,0.0, 0.0f, 0.0f,

										-1.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0,0.0,0.0, 0.0f, 0.0f,
										-1.0, 1.0, -1.0, 1.0, 1.0, 0.0, -1.0,0.0,0.0, 1.0f, 0.0f,
										-1.0, -1.0, -1.0, 1.0, 1.0, 0.0, -1.0,0.0,0.0, 1.0f, 1.0f,
										-1.0, -1.0, 1.0 , 1.0, 1.0, 0.0, -1.0,0.0,0.0, 0.0f, 1.0f
	
										};

	/*const GLfloat cubeColor[] = { 1.0, 0.0, 0.0,
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
										1.0, 1.0, 0.0 };

	const GLfloat cubeNormals[] = { 0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,
										0.0, 1.0, 0.0,

										0.0,-1.0,0.0,
										0.0,-1.0,0.0,
										0.0,-1.0,0.0,
										0.0,-1.0,0.0,

										0.0,0.0,1.0,
										0.0,0.0,1.0,
										0.0,0.0,1.0,
										0.0,0.0,1.0,

										0.0,0.0,-1.0,
										0.0,0.0,-1.0,
										0.0,0.0,-1.0,
										0.0,0.0,-1.0,

										1.0,0.0,0.0,
										1.0,0.0,0.0,
										1.0,0.0,0.0,
										1.0,0.0,0.0,

										-1.0,0.0,0.0,
										-1.0,0.0,0.0,
										-1.0,0.0,0.0,
										-1.0,0.0,0.0 };



	const GLfloat cubeTextCord[] = { 0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,

										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f,

										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,

										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,

										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,

										0.0f, 0.0f,
										1.0f, 0.0f,
										1.0f, 1.0f,
										0.0f, 1.0f };*/

	



	//Create Rectangle vao
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);


	
	loadTexture(&textureCube, MAKEINTRESOURCE(IDBITMAP_MARBLE));


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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

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
	translateMatrix = translate(0.0f, 0.0f, -6.0f);
	//rotationMatrix = rotate(angle_cube, angle_cube, angle_cube);
	rotationMatrix = rotate(angle_cube, 1.0f, 0.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	rotationMatrix = rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	translateMatrix = translateMatrix * rotationMatrix;

	modelMatrix = modelMatrix * translateMatrix;


	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(modelMatrix_Uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrix_Uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_Uniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == TRUE)
	{
		glUniform1i(LKeyPress_Uniform, 1);
		glUniform3fv(la_Uniform, 1, lightAmbient);
		glUniform3fv(ld_Uniform, 1, lightDiffuse);
		glUniform3fv(ls_Uniform, 1, lightSpecular);
		glUniform4fv(lightPosition_Uniform, 1, lightPosition);

		glUniform3fv(ka_Uniform, 1, materialAmbient);
		glUniform3fv(kd_Uniform, 1, materialDiffuse);
		glUniform3fv(ks_Uniform, 1, materialSpecular);
		glUniform1f(materialShinyness_Uniform, materialShinyness);


	}
	else
	{
		glUniform1i(LKeyPress_Uniform, 0);
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureCube);
	glUniform1i(samplerUniform, 0);

	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao_cube);



	//Draw necessary seen
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
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

	if (vbo_position_cube) {
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	if (vao_cube) {
		glDeleteBuffers(1, &vao_cube);
		vao_cube = 0;
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
