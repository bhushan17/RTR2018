
#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include"vmath.h"

#include "cuda.h" // for CUDA


#include <cuda_gl_interop.h>
#include <cuda_runtime.h>
#include "helper_cuda.h"
#include "helper_timer.h"
using namespace vmath;

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "cudart.lib")

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
GLuint vbo_position, vbo_gpu;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;  //this is from Vmath

const int glMesh_width = 2048;
const int glMesh_height = 2048;
static int flag =0;
int glMesh_width_temp = 2048;
int glMesh_height_temp = 2048;
#define MYARRAYSIZE  glMesh_width*glMesh_height*4
float pos[glMesh_width][glMesh_height][4];
//float ***pos = NULL;

struct cudaGraphicsResource *graphicResource = NULL;

cudaError_t error;
bool bOnGPU = false;

//********* Global function declaration ***********//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void launchCudaKernel(float4 *pPos, int meshWidth, int meshHeight, float type);

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
	void launchCPUKernel(unsigned int Meshwidth, unsigned int Meshheight,  float type);
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
			
		case 'G':
		case 'g':
			if(true == bOnGPU)
				bOnGPU = false;
			else
				bOnGPU = true;
			
			break;
		case VK_UP:
			if(flag == 0)
			{
				glMesh_width_temp = 128;
				glMesh_height_temp = 128;
				flag = 1;
			}
			else if(flag == 1)
			{
				glMesh_width_temp = 256;
				glMesh_height_temp = 256;
				flag = 2;
			}
			else if(flag == 2)
			{
				glMesh_width_temp = 512;
				glMesh_height_temp = 512;
				flag = 3;
			}
			else if(flag == 3)
			{
				glMesh_width_temp = 1024;
				glMesh_height_temp = 1024;
				flag = 4;
			}
			else if(flag == 4)
			{
				glMesh_width_temp = 2048;
				glMesh_height_temp = 2048;
				flag = 4;
			}
			//launchCPUKernel(glMesh_width, glMesh_height, 0.0f);
			break;
			
		case VK_DOWN:
			if(flag == 5)
			{
				glMesh_width_temp = 1024;
				glMesh_height_temp = 1024;
				flag = 4;
			}
			else if(flag == 4)
			{
				glMesh_width_temp = 512;
				glMesh_height_temp = 512;
				flag = 3;
			}
			else if(flag == 3)
			{
				glMesh_width_temp = 256;
				glMesh_height_temp = 256;
				flag = 2;
			}
			else if(flag == 2)
			{
				glMesh_width_temp = 128;
				glMesh_height_temp = 128;
				flag = 1;
			}
			else if(flag == 1)
			{
				glMesh_width_temp = 0;
				glMesh_height_temp = 0;
				flag = 0;
			}
			//launchCPUKernel(glMesh_width, glMesh_height, 0.0f);
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
	//void launchCPUKernel(unsigned int Meshwidth, unsigned int Meshheight,  float type);
	int devCount;
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


	error = cudaGetDeviceCount(&devCount);
	if(error !=cudaSuccess)
	{
		fprintf(gpFile, "CudaGetDevice Count failed !!!");
		fclose(gpFile);
		unintialize();
		exit(0);
		
	}
	else if(devCount == 0)
	{
		fprintf(gpFile, "No Cuda Device !!!");
		fclose(gpFile);
		unintialize();
		exit(0);
	}
	else{
		cudaSetDevice(0);
	}


	//Define VertexShader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Specify/Write vertex shader code

	const GLchar *VertexShaderSourseCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		
		"uniform mat4 u_mvp_matrix;" \
		
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		
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
		
		"void main(void)" \
		"{" \
		"fragColor = vec4(0.9,0.1,0.2,1.0);" \
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
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	
	
	glGenBuffers(1, &vbo_gpu);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	
	
	error = cudaGraphicsGLRegisterBuffer(&graphicResource,vbo_gpu,cudaGraphicsMapFlagsWriteDiscard);
	
	if(error !=cudaSuccess)
	{
		fprintf(gpFile, "cudagraphicsRegister failed !!!");
		fclose(gpFile);
		unintialize();
		DestroyWindow(ghWnd);
		exit(0);
		
	}
		
	glBindVertexArray(0);	
	
	/*pos = (float***)malloc(glMesh_width * sizeof(float**));
	
	for(int i=0; i< glMesh_width; i++)
	{
		pos[i] = (float**)malloc(glMesh_height * sizeof(float*));
	}
	
	
	for(int i=0; i< glMesh_width; i++)
	{
		for(int j=0; j< glMesh_height; j++)
		{
			pos[i][j] = (float*)malloc(4 * sizeof(float));
		}
	}*/
	

	/*for(int i =0; i<glMesh_width; i++)
	{
		for(int j =0; j<glMesh_height; j++)
		{
			for(int k=0; k<4; k++)
			{
				pos[i][j][k]=0.0f;
			}
		}
	}*/
	
	//launchCPUKernel(glMesh_width, glMesh_height, 0.0f);	

		

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);


	//glEnable(GL_TEXTURE_2D);

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


void launchCPUKernel(unsigned int Meshwidth, unsigned int Meshheight,  float type)
{
	float u,v,w,frequency = 2.0f;
	
	for(int i =0; i < Meshwidth; i++)
	{
		for(int j =0; j < Meshheight; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				u = i / float(Meshwidth);
				v = j / float(Meshheight);
				
				u = (u*5.0) - 3.0;
				v = (v*5.0)-3.0;
				
				w = sinf(frequency * u + type) * cosf(frequency * v + type);
				
				if(k == 0)
					pos[i][j][k]=u;
				
				if(k == 1)
					pos[i][j][k]=w;
				
				if(k == 2)
					pos[i][j][k]=v;
				
				if(k == 3)
					pos[i][j][k]=1.0;
				
			}
		}
	}
	
}
void display()
{
	// function declarations
	void unintialize(void);
	
	//code
	static float animation_type= 0.0f; 
	static int x=0,y=0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 translateMatrix;
	mat4 rotationMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary transformation

	//Do necessary Matrix multiplication
	translateMatrix = translate(0.0f, 1.0f, -5.5f);
	rotationMatrix = rotate(GLfloat(90.0),0.0f,1.0f,0.0f); 
	translateMatrix = translateMatrix * rotationMatrix;
	modelViewMatrix = modelViewMatrix * translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	//This waas done internally by gluaurtho in FFP

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	
	/*for(int i =0; i<2048; i++)
	{
		for(int j =0; j<2048; j++)
		{
			for(int k=0; k<4; k++)
			{
				pos[i][j][k]=0.0f;
			}
		}
	}*/
	
	memset(pos,0,MYARRAYSIZE *4);
	
	
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao);

	float4 *pPos=NULL;
	size_t byteCount;
	//Similarly Bind with any texture
	if(bOnGPU == true)
	{
		//Step1  Map the resource
		error = cudaGraphicsMapResources(1,&graphicResource,0);
		if (error != cudaSuccess) {
			fprintf(gpFile, "cudaGraphicsMapResources() failed.\n");
			unintialize();
			DestroyWindow(ghWnd);
			exit(0);
		}
		
		//Step 2 
		error = cudaGraphicsResourceGetMappedPointer((void**)&pPos, &byteCount, graphicResource);
		if (error != cudaSuccess) {
			fprintf(gpFile, "cudaGraphicsResourcesGetMappedPointer() failed.\n");
			unintialize();
			DestroyWindow(ghWnd);
			exit(0);
		}
		
		//Step 3 launch cuda kernel
		launchCudaKernel(pPos, glMesh_width_temp, glMesh_height_temp, animation_type);
		
		// Step 4 unmap resorce
		error = cudaGraphicsUnmapResources(1,&graphicResource,0);
		if (error != cudaSuccess) {
			fprintf(gpFile, "cudaGraphicsUnMapResources() failed.\n");
			unintialize();
			DestroyWindow(ghWnd);
			exit(0);
		}
	}
	else
	{
		launchCPUKernel(glMesh_width_temp, glMesh_height_temp, animation_type);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER,glMesh_width_temp * glMesh_height_temp  * sizeof(float), pos, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
		

	if(bOnGPU)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	}
	else{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	}
	
	
	
	



	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, glMesh_width_temp*glMesh_height_temp);

	translateMatrix = translate(0.0f, -1.5f, 0.0f);
	modelViewMatrix = modelViewMatrix * translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	//This waas done internally by gluaurtho in FFP

	//Send necessary Matrix to shader in respective uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glDrawArrays(GL_POINTS, 0, glMesh_width_temp*glMesh_height_temp);
	//unbind vao
	glBindVertexArray(0);

	//unusal program

	glUseProgram(0);

    if(x <glMesh_width_temp)
	{
		x++;
		y++;
	}
	else{
		x=0;
		y=0;
	}
	animation_type = animation_type +0.01;
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
