
#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include"vmath.h"

using namespace vmath;

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "Winmm.lib")
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

GLuint vao;
GLuint vbo_position, vbo_color;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;  //this is from Vmath

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

int intialize(void)
{
	fprintf(gpFile, "Inside Intialization.!!!\n");
	// ******* Function declaration **********//
	void resize(int, int);
	void unintialize(void);
	void ToggelFullScreen(void);
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
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_color = vColor;" \
		"}";

	//"gl_Position=vec4(0,0,0,0);" \

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
		"in vec4 out_color;" \
		"void main(void)" \
		"{" \
		"fragColor = out_color;" \
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
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");

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
	mvpUniform = glGetUniformLocation(glShaderProgramObject, "u_mvp_matrix");

	//Ortho fixfunction program
	/*const GLfloat triangleVertices[] = { 0.0f,1.0f,0.0f,
										0.0f,-1.0f,0.0f };*/
										
	/*{ 0.0f,1.0f,0.0f,
										-1.0f,-1.0f,0.0f,
										1.0f,-1.0f,0.0f };*/

	//Create voa/,
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);


	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);


	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
	ToggelFullScreen();
	PlaySound(
		TEXT("Takat_watan_ki_hamse_hai.wav"),
		NULL,
		SND_ASYNC | SND_SYSTEM
	);

	return(0);
}


void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix= perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display()
{
	//code
	float x_coordinate = -1.05f, y_coordinate = -1.05f;
	int index;
	static float itranslate = -0.65f;
	static float ntranslate = 1.2f;
	static float i2translate = -1.2f;
	static float atranslate = 0.7f;

	static bool iDone = false;
	static bool nDone = false;
	static bool dDone = false;
	static bool i2Done = false;
	static bool aDone = false;
	static bool flag = false;

	static float dsaffron_translate[3] = { 0,0,0 };
	static float dgreen_translate[3] = { 0,0,0 };

	static float planetranslate = -1.7f;

	static bool downplanedone = false;

	bool x1done, x2done;
	x1done = x2done = false;


	static float Upper_plane_xtranslation = -1.7f;
	static float Upper_plane_ytranslation = 1.1f;

	static float Upper_plane_rotation_angle = -90.0f;

	static float down_plane_xtranslation = -1.7f;
	static float down_plane_ytranslation = -1.1f;
	//static float down_plane_translation_angle = 180.0f;
	static float down_plane_rotation_angle = 90.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 rotationMatrix = mat4::identity();
	//Intialize above matrix to Identity;
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	GLfloat lineVertices[6];
	GLfloat linecolor[6];

	GLfloat rectangleVertices[12];
	GLfloat rectanglecolor[12];

	GLfloat triangleVertices[9];
	GLfloat trianglecolor[9];


	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao);

	//**********************************  Plane ************************************************/
	
	if (downplanedone == false)
	{
		if (dDone) {

			modelViewMatrix = translate(down_plane_xtranslation, down_plane_ytranslation, -2.5f);
			rotationMatrix = rotate(down_plane_rotation_angle, 0.0f, 0.0f, 1.0f);
			modelViewMatrix = modelViewMatrix * rotationMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			rectangleVertices[0] = 0.0f;
			rectangleVertices[1] = 0.05f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = 0.2f;
			rectangleVertices[4] = 0.05f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = 0.2f;
			rectangleVertices[7] = -0.05f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 0.0f;
			rectangleVertices[10] = -0.05f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 186.0 / 255.0;
			rectanglecolor[1] = 226.0 / 255.0;
			rectanglecolor[2] = 238.0 / 255.0;
			rectanglecolor[3] = 186.0 / 255.0;
			rectanglecolor[4] = 226.0 / 255.0;
			rectanglecolor[5] = 238.0 / 255.0;
			rectanglecolor[6] = 186.0 / 255.0;
			rectanglecolor[7] = 226.0 / 255.0;
			rectanglecolor[8] = 238.0 / 255.0;
			rectanglecolor[9] = 186.0 / 255.0;
			rectanglecolor[10] = 226.0 / 255.0;
			rectanglecolor[11] = 238.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			triangleVertices[0] = 0.2f;
			triangleVertices[1] = 0.05f;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = 0.25f;
			triangleVertices[4] = 0.0f;
			triangleVertices[5] = 0.0f;
			triangleVertices[6] = 0.2f;
			triangleVertices[7] = -0.05f;
			triangleVertices[8] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			//draw wing1
			float x, y;

			

			for (double i = 0; i <= 1.5;) {

				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[0] = x;
				triangleVertices[1] = 0.06 + y;
				triangleVertices[2] = 0.0f;

				i = i + 0.3;
				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[3] = x;
				triangleVertices[4] = 0.06 + y;
				triangleVertices[5] = 0.0f;

				triangleVertices[6] = 0.03;
				triangleVertices[7] = 0.06;
				triangleVertices[8] = 0.0f;

				glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
			}
			//draw wing2
			for (double i = 0; i <= 1.5;) {

					x = 0.13 * cos(i);
					y = 0.03 * sin(i);

					triangleVertices[0] = x;
					triangleVertices[1] = -0.06 - y;
					triangleVertices[2] = 0.0f;

					i = i + 0.3;
					x = 0.13 * cos(i);
					y = 0.03 * sin(i);

					triangleVertices[3] = x;
					triangleVertices[4] = -0.06 - y;
					triangleVertices[5] = 0.0f;

					triangleVertices[6] = 0.03;
					triangleVertices[7] = -0.06;
					triangleVertices[8] = 0.0f;

					glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
					glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

			}

			// Write IAF
			glLineWidth(4.0f);
			//I
			lineVertices[0] = 0.05f;
			lineVertices[1] = 0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.05f;
			lineVertices[4] = -0.02f;
			lineVertices[5] = 0.0f;

			linecolor[0] = 1.0;
			linecolor[1] = 1.0;
			linecolor[2] = 1.0;
			linecolor[3] = 1.0;
			linecolor[4] = 1.0;
			linecolor[5] = 1.0;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.03f;
			lineVertices[1] = 0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.07f;
			lineVertices[4] = 0.02f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.03f;
			lineVertices[1] = -0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.07f;
			lineVertices[4] = -0.02f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			//A
			lineVertices[0] = 0.095f;
			lineVertices[1] = -0.003f;
			lineVertices[2] = 0.0f; 
			lineVertices[3] = 0.123f; 
			lineVertices[4] = -0.003;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.085f;
			lineVertices[1] = -0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.105f;
			lineVertices[4] = 0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.105f;
			lineVertices[1] = 0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.13f;
			lineVertices[4] = -0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			//F
			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.023f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.175f;
			lineVertices[4] = 0.023;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.0f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.175f;
			lineVertices[4] = 0.0f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.15f;
			lineVertices[4] = -0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);
					   
			//draw flag

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = 0.0375f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = 0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = 0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = 0.0375f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 153.0 / 255.0;
			rectanglecolor[2] = 51.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 152.0 / 255.0;
			rectanglecolor[5] = 51.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 153.0 / 255.0;
			rectanglecolor[8] = 51.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 153.0 / 255.0;
			rectanglecolor[11] = 51.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = 0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = -0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = -0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = 0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 255.0 / 255.0;
			rectanglecolor[2] = 255.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 255.0 / 255.0;
			rectanglecolor[5] = 255.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 255.0 / 255.0;
			rectanglecolor[8] = 255.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 255.0 / 255.0;
			rectanglecolor[11] = 255.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = -0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = -0.0375f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = -0.0375f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = -0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 18.0 / 255.0;
			rectanglecolor[1] = 136.0 / 255.0;
			rectanglecolor[2] = 7.0 / 255.0;
			rectanglecolor[3] = 18.0 / 255.0;
			rectanglecolor[4] = 136.0 / 255.0;
			rectanglecolor[5] = 7.0 / 255.0;
			rectanglecolor[6] = 18.0 / 255.0;
			rectanglecolor[7] = 136.0 / 255.0;
			rectanglecolor[8] = 7.0 / 255.0;
			rectanglecolor[9] = 18.0 / 255.0;
			rectanglecolor[10] = 136.0 / 255.0;
			rectanglecolor[11] = 7.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);





			// Draw Upper Plane

			modelViewMatrix = translate(Upper_plane_xtranslation, Upper_plane_ytranslation, -2.5f);
			rotationMatrix = rotate(Upper_plane_rotation_angle, 0.0f, 0.0f, 1.0f);
			modelViewMatrix = modelViewMatrix * rotationMatrix;
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			rectangleVertices[0] = 0.0f;
			rectangleVertices[1] = 0.05f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = 0.2f;
			rectangleVertices[4] = 0.05f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = 0.2f;
			rectangleVertices[7] = -0.05f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 0.0f;
			rectangleVertices[10] = -0.05f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 186.0 / 255.0;
			rectanglecolor[1] = 226.0 / 255.0;
			rectanglecolor[2] = 238.0 / 255.0;
			rectanglecolor[3] = 186.0 / 255.0;
			rectanglecolor[4] = 226.0 / 255.0;
			rectanglecolor[5] = 238.0 / 255.0;
			rectanglecolor[6] = 186.0 / 255.0;
			rectanglecolor[7] = 226.0 / 255.0;
			rectanglecolor[8] = 238.0 / 255.0;
			rectanglecolor[9] = 186.0 / 255.0;
			rectanglecolor[10] = 226.0 / 255.0;
			rectanglecolor[11] = 238.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			triangleVertices[0] = 0.2f;
			triangleVertices[1] = 0.05f;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = 0.25f;
			triangleVertices[4] = 0.0f;
			triangleVertices[5] = 0.0f;
			triangleVertices[6] = 0.2f;
			triangleVertices[7] = -0.05f;
			triangleVertices[8] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			//draw wing1
			



			for (double i = 0; i <= 1.5;) {

				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[0] = x;
				triangleVertices[1] = 0.06 + y;
				triangleVertices[2] = 0.0f;

				i = i + 0.3;
				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[3] = x;
				triangleVertices[4] = 0.06 + y;
				triangleVertices[5] = 0.0f;

				triangleVertices[6] = 0.03;
				triangleVertices[7] = 0.06;
				triangleVertices[8] = 0.0f;

				glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
			}
			//draw wing2
			for (double i = 0; i <= 1.5;) {

				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[0] = x;
				triangleVertices[1] = -0.06 - y;
				triangleVertices[2] = 0.0f;

				i = i + 0.3;
				x = 0.13 * cos(i);
				y = 0.03 * sin(i);

				triangleVertices[3] = x;
				triangleVertices[4] = -0.06 - y;
				triangleVertices[5] = 0.0f;

				triangleVertices[6] = 0.03;
				triangleVertices[7] = -0.06;
				triangleVertices[8] = 0.0f;

				glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
				glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

			}

			// Write IAF
			glLineWidth(4.0f);
			//I
			lineVertices[0] = 0.05f;
			lineVertices[1] = 0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.05f;
			lineVertices[4] = -0.02f;
			lineVertices[5] = 0.0f;

			linecolor[0] = 1.0;
			linecolor[1] = 1.0;
			linecolor[2] = 1.0;
			linecolor[3] = 1.0;
			linecolor[4] = 1.0;
			linecolor[5] = 1.0;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.03f;
			lineVertices[1] = 0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.07f;
			lineVertices[4] = 0.02f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.03f;
			lineVertices[1] = -0.02f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.07f;
			lineVertices[4] = -0.02f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			//A
			lineVertices[0] = 0.095f;
			lineVertices[1] = -0.003f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.123f;
			lineVertices[4] = -0.003;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.085f;
			lineVertices[1] = -0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.105f;
			lineVertices[4] = 0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.105f;
			lineVertices[1] = 0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.13f;
			lineVertices[4] = -0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			//F
			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.023f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.175f;
			lineVertices[4] = 0.023;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.0f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.175f;
			lineVertices[4] = 0.0f;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			lineVertices[0] = 0.15f;
			lineVertices[1] = 0.025f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 0.15f;
			lineVertices[4] = -0.025;
			lineVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINES, 0, 2);

			//draw flag

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = 0.0375f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = 0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = 0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = 0.0375f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 153.0 / 255.0;
			rectanglecolor[2] = 51.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 152.0 / 255.0;
			rectanglecolor[5] = 51.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 153.0 / 255.0;
			rectanglecolor[8] = 51.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 153.0 / 255.0;
			rectanglecolor[11] = 51.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = 0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = -0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = -0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = 0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 255.0 / 255.0;
			rectanglecolor[2] = 255.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 255.0 / 255.0;
			rectanglecolor[5] = 255.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 255.0 / 255.0;
			rectanglecolor[8] = 255.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 255.0 / 255.0;
			rectanglecolor[11] = 255.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = -0.029f;
			rectangleVertices[1] = -0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.029f;
			rectangleVertices[4] = -0.0375f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.12f;
			rectangleVertices[7] = -0.0375f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.12f;
			rectangleVertices[10] = -0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 18.0 / 255.0;
			rectanglecolor[1] = 136.0 / 255.0;
			rectanglecolor[2] = 7.0 / 255.0;
			rectanglecolor[3] = 18.0 / 255.0;
			rectanglecolor[4] = 136.0 / 255.0;
			rectanglecolor[5] = 7.0 / 255.0;
			rectanglecolor[6] = 18.0 / 255.0;
			rectanglecolor[7] = 136.0 / 255.0;
			rectanglecolor[8] = 7.0 / 255.0;
			rectanglecolor[9] = 18.0 / 255.0;
			rectanglecolor[10] = 136.0 / 255.0;
			rectanglecolor[11] = 7.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		}
	}

	if (dDone) {

		if (down_plane_xtranslation < -0.91) {


			down_plane_ytranslation = down_plane_ytranslation + 0.012;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.99f;

			//down_plane_exaust_xtranslation = down_plane_exaust_xtranslation + 0.004f - 0.0001f;
			//down_plane_exaust_ytranslation = down_plane_exaust_ytranslation + 0.0055 + 0.0001f;

			//down_plane_wing1_xtranslation = down_plane_xtranslation + 0.0004425;
			//down_plane_wing1_ytranslation = down_plane_ytranslation + 0.00055; // +0.0005775;

			//down_plane_wing1_xtranslation = down_plane_wing1_xtranslation + 0.004f + 0.0004f;
			//down_plane_wing2_xtranslation = down_plane_wing2_xtranslation + 0.004f - 0.0001f;
			//down_plane_wing1_ytranslation = down_plane_ytranslation + 0.06;
			//down_plane_wing2_ytranslation = down_plane_ytranslation - 0.06;
		}
		else {
			downplanedone = true;
		}

		if (down_plane_xtranslation > 0.8) {
			downplanedone = false;
			down_plane_ytranslation = down_plane_ytranslation - 0.012;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.99f;

		}

		down_plane_xtranslation = down_plane_xtranslation + 0.0087f;


		if (Upper_plane_xtranslation < -0.9) {


			Upper_plane_ytranslation = Upper_plane_ytranslation - 0.012;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.99f;
		}

		if (Upper_plane_xtranslation > 0.8) {

			Upper_plane_ytranslation = Upper_plane_ytranslation + 0.012;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.99f;

		}

		Upper_plane_xtranslation = Upper_plane_xtranslation + 0.0087f;

	}


	//*******************************Middle Plane start***********************************/


	if (dDone) {
		if (planetranslate < -0.74) {
			planetranslate = planetranslate + 0.0085;


		}
		else if (planetranslate < 2.5) {
			planetranslate = planetranslate + 0.0085;
			flag = true;
		}

		modelViewMatrix = translate(planetranslate, 0.0f, -2.5f);
		//rotationMatrix = rotate(down_plane_rotation_angle, 0.0f, 0.0f, 1.0f);
		//modelViewMatrix = modelViewMatrix * rotationMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		rectangleVertices[0] = 0.0f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.2f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.2f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.0f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectanglecolor[0] = 186.0 / 255.0;
		rectanglecolor[1] = 226.0 / 255.0;
		rectanglecolor[2] = 238.0 / 255.0;
		rectanglecolor[3] = 186.0 / 255.0;
		rectanglecolor[4] = 226.0 / 255.0;
		rectanglecolor[5] = 238.0 / 255.0;
		rectanglecolor[6] = 186.0 / 255.0;
		rectanglecolor[7] = 226.0 / 255.0;
		rectanglecolor[8] = 238.0 / 255.0;
		rectanglecolor[9] = 186.0 / 255.0;
		rectanglecolor[10] = 226.0 / 255.0;
		rectanglecolor[11] = 238.0 / 255.0;


		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		triangleVertices[0] = 0.2f;
		triangleVertices[1] = 0.05f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.25f;
		triangleVertices[4] = 0.0f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = 0.2f;
		triangleVertices[7] = -0.05f;
		triangleVertices[8] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		//draw wing1
		float x, y;



		for (double i = 0; i <= 1.5;) {

			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			triangleVertices[0] = x;
			triangleVertices[1] = 0.06 + y;
			triangleVertices[2] = 0.0f;

			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			triangleVertices[3] = x;
			triangleVertices[4] = 0.06 + y;
			triangleVertices[5] = 0.0f;

			triangleVertices[6] = 0.03;
			triangleVertices[7] = 0.06;
			triangleVertices[8] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
		}
		//draw wing2
		for (double i = 0; i <= 1.5;) {

			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			triangleVertices[0] = x;
			triangleVertices[1] = -0.06 - y;
			triangleVertices[2] = 0.0f;

			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			triangleVertices[3] = x;
			triangleVertices[4] = -0.06 - y;
			triangleVertices[5] = 0.0f;

			triangleVertices[6] = 0.03;
			triangleVertices[7] = -0.06;
			triangleVertices[8] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

		}

		// Write IAF
		glLineWidth(4.0f);
		//I
		lineVertices[0] = 0.05f;
		lineVertices[1] = 0.02f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.05f;
		lineVertices[4] = -0.02f;
		lineVertices[5] = 0.0f;

		linecolor[0] = 1.0;
		linecolor[1] = 1.0;
		linecolor[2] = 1.0;
		linecolor[3] = 1.0;
		linecolor[4] = 1.0;
		linecolor[5] = 1.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.03f;
		lineVertices[1] = 0.02f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.07f;
		lineVertices[4] = 0.02f;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.03f;
		lineVertices[1] = -0.02f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.07f;
		lineVertices[4] = -0.02f;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		//A
		lineVertices[0] = 0.095f;
		lineVertices[1] = -0.003f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.123f;
		lineVertices[4] = -0.003;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.085f;
		lineVertices[1] = -0.025f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.105f;
		lineVertices[4] = 0.025;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.105f;
		lineVertices[1] = 0.025f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.13f;
		lineVertices[4] = -0.025;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		//F
		lineVertices[0] = 0.15f;
		lineVertices[1] = 0.023f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.175f;
		lineVertices[4] = 0.023;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.15f;
		lineVertices[1] = 0.0f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.175f;
		lineVertices[4] = 0.0f;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);

		lineVertices[0] = 0.15f;
		lineVertices[1] = 0.025f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.15f;
		lineVertices[4] = -0.025;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, 2);


		//draw exaust part
		/*
		modelViewMatrix = translate(planetranslate - 0.02f, 0.0f, -2.5f);
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		rectangleVertices[0] = 0.0f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.01f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.01f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.0f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectanglecolor[0] = 186.0 / 255.0;
		rectanglecolor[1] = 226.0 / 255.0;
		rectanglecolor[2] = 238.0 / 255.0;
		rectanglecolor[3] = 186.0 / 255.0;
		rectanglecolor[4] = 226.0 / 255.0;
		rectanglecolor[5] = 238.0 / 255.0;
		rectanglecolor[6] = 186.0 / 255.0;
		rectanglecolor[7] = 226.0 / 255.0;
		rectanglecolor[8] = 238.0 / 255.0;
		rectanglecolor[9] = 186.0 / 255.0;
		rectanglecolor[10] = 226.0 / 255.0;
		rectanglecolor[11] = 238.0 / 255.0;


		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		triangleVertices[0] = 0.2f;
		triangleVertices[1] = 0.05f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.25f;
		triangleVertices[4] = 0.0f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = 0.2f;
		triangleVertices[7] = -0.05f;
		triangleVertices[8] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);*/



		//draw Flag
		if (flag)
		{

			static float x = -0.03f;
			static float x1, x2 = -0.855f;
			if (planetranslate < 1.5) {
				x1 = x + planetranslate;
			}
			//else


			if (planetranslate > 1.5 && x2 < 0.667) {
				x2 = x2 + 0.004;

			}

			if (x2 > 0.667) {
				x2done = true;

			}

			if (planetranslate > 1.5  && x1 > 0.82) {

				x1 = x1 - 0.002;
			}

			if (planetranslate > 1.5 && x1 < 0.82) {

				x1done = true;
			}

			modelViewMatrix = translate(0.0f, 0.0f, -2.5f);
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			//draw flag

			rectangleVertices[0] = x1;
			rectangleVertices[1] = 0.0375f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = x1;
			rectangleVertices[4] = 0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = x2;
			rectangleVertices[7] = 0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = x2;
			rectangleVertices[10] = 0.0375f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 153.0 / 255.0;
			rectanglecolor[2] = 51.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 153.0 / 255.0;
			rectanglecolor[5] = 51.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 153.0 / 255.0;
			rectanglecolor[8] = 51.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 153.0 / 255.0;
			rectanglecolor[11] = 51.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = x1;
			rectangleVertices[1] = 0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = x1;
			rectangleVertices[4] = -0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = x2;
			rectangleVertices[7] = -0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = x2;
			rectangleVertices[10] = 0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 255.0 / 255.0;
			rectanglecolor[2] = 255.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 255.0 / 255.0;
			rectanglecolor[5] = 255.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 255.0 / 255.0;
			rectanglecolor[8] = 255.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 255.0 / 255.0;
			rectanglecolor[11] = 255.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = x1;
			rectangleVertices[1] = -0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = x1;
			rectangleVertices[4] = -0.0375f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = x2;
			rectangleVertices[7] = -0.0375f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = x2;
			rectangleVertices[10] = -0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 18.0 / 255.0;
			rectanglecolor[1] = 136.0 / 255.0;
			rectanglecolor[2] = 7.0 / 255.0;
			rectanglecolor[3] = 18.0 / 255.0;
			rectanglecolor[4] = 136.0 / 255.0;
			rectanglecolor[5] = 7.0 / 255.0;
			rectanglecolor[6] = 18.0 / 255.0;
			rectanglecolor[7] = 136.0 / 255.0;
			rectanglecolor[8] = 7.0 / 255.0;
			rectanglecolor[9] = 18.0 / 255.0;
			rectanglecolor[10] = 136.0 / 255.0;
			rectanglecolor[11] = 7.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		else
		{
			modelViewMatrix = translate(planetranslate - 0.02f, 0.0f, -2.5f);
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

			rectangleVertices[0] = -0.009f;
			rectangleVertices[1] = 0.0375f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.009f;
			rectangleVertices[4] = 0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.1;
			rectangleVertices[7] = 0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.1;
			rectangleVertices[10] = 0.0375f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 153.0 / 255.0;
			rectanglecolor[2] = 51.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 153.0 / 255.0;
			rectanglecolor[5] = 51.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 153.0 / 255.0;
			rectanglecolor[8] = 51.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 153.0 / 255.0;
			rectanglecolor[11] = 51.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


			rectangleVertices[0] = -0.009f;
			rectangleVertices[1] = 0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.009f;
			rectangleVertices[4] = -0.0125f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.1;
			rectangleVertices[7] = -0.0125f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.1;
			rectangleVertices[10] = 0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 255.0 / 255.0;
			rectanglecolor[1] = 255.0 / 255.0;
			rectanglecolor[2] = 255.0 / 255.0;
			rectanglecolor[3] = 255.0 / 255.0;
			rectanglecolor[4] = 255.0 / 255.0;
			rectanglecolor[5] = 255.0 / 255.0;
			rectanglecolor[6] = 255.0 / 255.0;
			rectanglecolor[7] = 255.0 / 255.0;
			rectanglecolor[8] = 255.0 / 255.0;
			rectanglecolor[9] = 255.0 / 255.0;
			rectanglecolor[10] = 255.0 / 255.0;
			rectanglecolor[11] = 255.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			rectangleVertices[0] = -0.009f;
			rectangleVertices[1] = -0.0125f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = -0.009f;
			rectangleVertices[4] = -0.0375f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = -0.1;
			rectangleVertices[7] = -0.0375f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = -0.1;
			rectangleVertices[10] = -0.0125f;
			rectangleVertices[11] = 0.0f;

			rectanglecolor[0] = 18.0 / 255.0;
			rectanglecolor[1] = 136.0 / 255.0;
			rectanglecolor[2] = 7.0 / 255.0;
			rectanglecolor[3] = 18.0 / 255.0;
			rectanglecolor[4] = 136.0 / 255.0;
			rectanglecolor[5] = 7.0 / 255.0;
			rectanglecolor[6] = 18.0 / 255.0;
			rectanglecolor[7] = 136.0 / 255.0;
			rectanglecolor[8] = 7.0 / 255.0;
			rectanglecolor[9] = 18.0 / 255.0;
			rectanglecolor[10] = 136.0 / 255.0;
			rectanglecolor[11] = 7.0 / 255.0;


			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectanglecolor), rectanglecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		}
	}



	//*************************** INDIA ************************************************/

	if (itranslate < 0.001)
		itranslate = itranslate + 0.003;
	else
		iDone = true;

	modelViewMatrix = translate(itranslate, 0.0f, -2.5f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);


	// DRAW I
	//Similarly Bind with any texture
	

	lineVertices[0] = -0.85f;
	lineVertices[1] = 0.7f;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -0.85f;
	lineVertices[4] = -0.7f;
	lineVertices[5] = 0.0f;

	linecolor[0] = 255.0 / 255.0;
	linecolor[1] = 153.0 / 255.0;
	linecolor[2] = 51.0 / 255.0;
	linecolor[3] = 18.0 / 255.0;
	linecolor[4] = 136.0 / 255.0;
	linecolor[5] = 7.0 / 255.0;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINES, 0, 2);



	//Draw N

	if (aDone) {
		if (ntranslate > 0)
			ntranslate = ntranslate - 0.006;
		else
			nDone = true;


		modelViewMatrix = translate(0.0f, ntranslate, -2.5f);
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);


		lineVertices[0] = -0.6f;
		lineVertices[1] = -0.7f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -0.6f;
		lineVertices[4] = 0.7f;
		lineVertices[5] = 0.0f;

		linecolor[0] = 18.0 / 255.0;
		linecolor[1] = 136.0 / 255.0;
		linecolor[2] = 7.0 / 255.0;
		linecolor[3] = 255.0 / 255.0;
		linecolor[4] = 153.0 / 255.0;
		linecolor[5] = 51.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);


		lineVertices[0] = -0.6f;
		lineVertices[1] = 0.7f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -0.3f;
		lineVertices[4] = -0.7f;
		lineVertices[5] = 0.0f;

		linecolor[0] = 255.0 / 255.0;
		linecolor[1] = 153.0 / 255.0;
		linecolor[2] = 51.0 / 255.0;
		linecolor[3] = 18.0 / 255.0;
		linecolor[4] = 136.0 / 255.0;
		linecolor[5] = 7.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);


		lineVertices[0] = -0.3f;
		lineVertices[1] = -0.7f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -0.3f;
		lineVertices[4] = 0.7f;
		lineVertices[5] = 0.0f;

		linecolor[0] = 255.0 / 255.0;
		linecolor[1] = 153.0 / 255.0;
		linecolor[2] = 51.0 / 255.0;
		linecolor[3] = 18.0 / 255.0;
		linecolor[4] = 136.0 / 255.0;
		linecolor[5] = 7.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}

	//Draw D

	if (i2Done) {

		if (dsaffron_translate[0] < (1 + ((255.0 / 255.0) / 200)))
		{
			dsaffron_translate[0] = dsaffron_translate[0] + ((255.0 / 255.0) / 200);
			dsaffron_translate[1] = dsaffron_translate[1] + ((153.0 / 255.0) / 200);
			dsaffron_translate[2] = dsaffron_translate[2] + ((51.0 / 255.0) / 200);
			dgreen_translate[0] = dgreen_translate[0] + ((18.0 / 255.0) / 200);
			dgreen_translate[1] = dgreen_translate[1] + ((136.0 / 255.0) / 200);
			dgreen_translate[2] = dgreen_translate[2] + ((7.0 / 255.0) / 200);

		}
		else {

			dsaffron_translate[0] = 255.0 / 255.0;
			dsaffron_translate[1] = 153.0 / 255.0;
			dsaffron_translate[2] = 51.0 / 255.0;
			dgreen_translate[0] = 18.0 / 255.0;
			dgreen_translate[1] = 136.0 / 255.0;
			dgreen_translate[2] = 7.0 / 255.0;
			dDone = true;

		}


		lineVertices[0] = -0.2f;
		lineVertices[1] = -0.7f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -0.2f;
		lineVertices[4] = 0.7f;
		lineVertices[5] = 0.0f;


		linecolor[0] = dgreen_translate[0];
		linecolor[1] = dgreen_translate[1];
		linecolor[2] = dgreen_translate[2];
		linecolor[3] = dsaffron_translate[0];
		linecolor[4] = dsaffron_translate[1];
		linecolor[5] = dsaffron_translate[2];

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);



		lineVertices[0] = -0.2f;
		lineVertices[1] = 0.688f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.1f;
		lineVertices[4] = 0.688f;
		lineVertices[5] = 0.0f;


		linecolor[0] = dsaffron_translate[0];
		linecolor[1] = dsaffron_translate[1];
		linecolor[2] = dsaffron_translate[2];
		linecolor[3] = dsaffron_translate[0];
		linecolor[4] = dsaffron_translate[1];
		linecolor[5] = dsaffron_translate[2];

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);


		lineVertices[0] = 0.1f;
		lineVertices[1] = 0.7f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.1f;
		lineVertices[4] = -0.7f;
		lineVertices[5] = 0.0f;


		linecolor[0] = dsaffron_translate[0];
		linecolor[1] = dsaffron_translate[1];
		linecolor[2] = dsaffron_translate[2];
		linecolor[3] = dgreen_translate[0];
		linecolor[4] = dgreen_translate[1];
		linecolor[5] = dgreen_translate[2];

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);


		lineVertices[0] = 0.1f;
		lineVertices[1] = -0.688f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -0.2f;
		lineVertices[4] = -0.688f;
		lineVertices[5] = 0.0f;


		linecolor[0] = dgreen_translate[0];
		linecolor[1] = dgreen_translate[1];
		linecolor[2] = dgreen_translate[2];
		linecolor[3] = dgreen_translate[0];
		linecolor[4] = dgreen_translate[1];
		linecolor[5] = dgreen_translate[2];

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}

	//DRAW   I
	
	if (nDone) {
		if (i2translate < 0)
			i2translate = i2translate + 0.006;
		else
			i2Done = true;

		modelViewMatrix = translate(0.0f, i2translate, -2.5f);
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		lineVertices[0] = 0.35f;
		lineVertices[1] = 0.7;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.35f;
		lineVertices[4] = -0.7f;
		lineVertices[5] = 0.0f;


		linecolor[0] = 255.0 / 255.0;
		linecolor[1] = 153.0 / 255.0;
		linecolor[2] = 51.0 / 255.0;
		linecolor[3] = 18.0 / 255.0;
		linecolor[4] = 136.0 / 255.0;
		linecolor[5] = 7.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);

	}

	
	//DRAW FLAG
	/*lineVertices[0] = 0.67f;
	lineVertices[1] = 0.02;
	lineVertices[2] = 0.0f;
	lineVertices[3] = 0.83f;
	lineVertices[4] = 0.02f;
	lineVertices[5] = 0.0f;


	linecolor[0] = 255.0 / 255.0;
	linecolor[1] = 153.0 / 255.0;
	linecolor[2] = 51.0 / 255.0;
	linecolor[3] = 255.0 / 255.0;
	linecolor[4] = 153.0 / 255.0;
	linecolor[5] = 51.0 / 255.0;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINES, 0, 2);



	lineVertices[0] = 0.67f;
	lineVertices[1] = 0.00;
	lineVertices[2] = 0.0f;
	lineVertices[3] = 0.83f;
	lineVertices[4] = 0.00f;
	lineVertices[5] = 0.0f;


	linecolor[0] = 255.0 / 255.0;
	linecolor[1] = 255.0 / 255.0;
	linecolor[2] = 255.0 / 255.0;
	linecolor[3] = 255.0 / 255.0;
	linecolor[4] = 255.0 / 255.0;
	linecolor[5] = 255.0 / 255.0;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINES, 0, 2);



	lineVertices[0] = 0.67f;
	lineVertices[1] = -0.02f;
	lineVertices[2] = 0.0f;
	lineVertices[3] = 0.83f;
	lineVertices[4] = -0.02f;
	lineVertices[5] = 0.0f;


	linecolor[0] = 18.0 / 255.0;
	linecolor[1] = 136.0 / 255.0;
	linecolor[2] = 7.0 / 255.0;
	linecolor[3] = 18.0 / 255.0;
	linecolor[4] = 136.0 / 255.0;
	linecolor[5] = 7.0 / 255.0;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINES, 0, 2);*/




	//DRAW A
	
	if (iDone) {
		if (atranslate > 0)
			atranslate = atranslate - 0.0025;
		else
			aDone = true;

		modelViewMatrix = translate(atranslate, 0.0f, -2.5f);
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);



		lineVertices[0] = 0.75f;
		lineVertices[1] = 0.7;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.9f;
		lineVertices[4] = -0.7f;
		lineVertices[5] = 0.0f;


		linecolor[0] = 255.0 / 255.0;
		linecolor[1] = 153.0 / 255.0;
		linecolor[2] = 51.0 / 255.0;
		linecolor[3] = 18.0 / 255.0;
		linecolor[4] = 136.0 / 255.0;
		linecolor[5] = 7.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);



		lineVertices[0] = 0.75f;
		lineVertices[1] = 0.7;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.6f;
		lineVertices[4] = -0.7f;
		lineVertices[5] = 0.0f;


		linecolor[0] = 255.0 / 255.0;
		linecolor[1] = 153.0 / 255.0;
		linecolor[2] = 51.0 / 255.0;
		linecolor[3] = 18.0 / 255.0;
		linecolor[4] = 136.0 / 255.0;
		linecolor[5] = 7.0 / 255.0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);
	}
















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

	if (vbo_position) {
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vao) {
		glDeleteBuffers(1, &vao);
		vao = 0;
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
