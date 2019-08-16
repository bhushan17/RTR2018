
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary transformation

	//Do necessary Matrix multiplication
	modelViewMatrix = translate(0.0f, 0.0f, -2.5f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	//This waas done internally by gluaurtho in FFP

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao);

	//Similarly Bind with any texture
	GLfloat triangleVertices[] = { 0.0f,1.0f,0.0f,
										0.0f,-1.0f,0.0f };

	GLfloat linecolor[6] = { 0.0f,1.0f,0.0f,
										0.0f,1.0f,0.0f };

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 2);

	triangleVertices[0] = -1.0f;
	triangleVertices[1] = 0.0f;
	triangleVertices[2] = 0.0f;
	triangleVertices[3] = 1.0f;
	triangleVertices[4] = 0.0f;
	triangleVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, 2);
	

	linecolor[0] = 0.0f;
	linecolor[1] = 0.3f;
	linecolor[2] = 0.0f;
	linecolor[3] = 0.0f;
	linecolor[4] = 0.3f;
	linecolor[5] = 0.0f;

	for (index = 0; index <= 40; index++)
	{
		x_coordinate = x_coordinate + 0.05f;
		y_coordinate = y_coordinate + 0.05f;

		if (int(x_coordinate * 100) == 0)
		{

			continue;
		}
		else {
			triangleVertices[0] = x_coordinate;
			triangleVertices[1] = -1.0f;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = x_coordinate;
			triangleVertices[4] = 1.0f;
			triangleVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, 0, 2);

		

			triangleVertices[0] = -1.0f;
			triangleVertices[1] = y_coordinate;
			triangleVertices[2] = 0.0f;
			triangleVertices[3] = 1.0f;
			triangleVertices[4] = y_coordinate;
			triangleVertices[5] = 0.0f;

			glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glLineWidth(1.0f);
			glDrawArrays(GL_LINES, 0, 2);


			
		}

	}
	//Draw triangle by line
	GLfloat linec[9];
	linec[0] = 1.0f;
	linec[1] = 1.0f;
	linec[2] = 1.0f;
	linec[3] = 1.0f;
	linec[4] = 1.0f;
	linec[5] = 1.0f;
	linec[6] = 1.0f;
	linec[7] = 1.0f;
	linec[8] = 1.0f;

	GLfloat lineVertices[9]; 
	lineVertices[0] = 0.0f;
	lineVertices[1] = 1.0;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -sqrt(0.75f);
	lineVertices[4] = -0.5f;
	lineVertices[5] = 0.0f;
	lineVertices[6] = sqrt(0.75f);
	lineVertices[7] = -0.5f;
	lineVertices[8] = 0.0f;
	

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linec), linec, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 3);
	
	int  numpoints = 1000;

	GLfloat circlevertex[6];
	GLfloat circlcolor[6] = { 1,1,1,1,1,1 };
	float radius = 0.5f;
	float angle = 0.0f;
	int i = 0, j = 1, k = 2;
	//Inner circle
	for (index = 0; index < numpoints; index++)
	{
		angle = (2 * M_PI *index) / numpoints;
		circlevertex[0] = (radius * cos(angle));
		circlevertex[1] = (radius * sin(angle));
		circlevertex[2] = 0;


		angle = (2 * M_PI * (index+1)) / numpoints;
		circlevertex[3] = (radius * cos(angle));
		circlevertex[4] = (radius * sin(angle));
		circlevertex[5] = 0;
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlevertex), circlevertex, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlcolor), circlcolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);

	}
	
	//Outer Circle
	for (index = 0; index < numpoints; index++)
	{
		angle = (2 * M_PI *index) / numpoints;
		circlevertex[0] =  cos(angle);
		circlevertex[1] =  sin(angle);
		circlevertex[2] = 0;


		angle = (2 * M_PI * (index + 1)) / numpoints;
		circlevertex[3] =  cos(angle);
		circlevertex[4] =  sin(angle);
		circlevertex[5] = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlevertex), circlevertex, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(circlcolor), circlcolor, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 2);

	}

	//Square by line
	
	GLfloat reccolor[12] = { 1,1,1,1,1,1,1,1,1,1,1,1 };
	

	GLfloat rectvertex[12];
	rectvertex[0] = -sqrt(0.75f);
	rectvertex[1] = 0.5;
	rectvertex[2] = 0.0f;
	rectvertex[3] = -sqrt(0.75f);
	rectvertex[4] = -0.5f;
	rectvertex[5] = 0.0f;
	rectvertex[6] = sqrt(0.75f);
	rectvertex[7] = -0.5f;
	rectvertex[8] = 0.0f;
	rectvertex[9] = sqrt(0.75f);
	rectvertex[10] = 0.5;
	rectvertex[11] = 0.0f;


	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectvertex), rectvertex, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(reccolor), reccolor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_LOOP, 0, 4);



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
