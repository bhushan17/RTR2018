
#include<windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include <vector>
#include "vmath.h"

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
GLuint vbo,vbo_normal;
GLuint modelMatrix_Uniform, viewMatrix_Uniform, projection_Uniform;
GLuint ld_Uniform, la_Uniform, ls_Uniform, lightPosition_Uniform;
GLuint ka_Uniform, kd_Uniform, ks_Uniform;
GLuint LKeyPress_Uniform, materialShinyness_Uniform;

float lightAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float lightDiffuse[4] = { 0.6f,0.0f,0.0f,1.0f };
float lightSpecular[4] = { 0.7f,0.5f,0.2f,1.0f };
float lightPosition[4] = { 100.0f,100.0f,100.0f,1.0f };


float materialAmbient[4] = { 0.0f,0.0f,0.0f,0.0f };
float materialDiffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float materialShinyness = 10.0f;

mat4 perspectiveProjectionMatrix;  //this is from Vmath
bool bLight = false;



int sizeOfPosArray = 0;

float length(float* v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float* create_paticleVectore(void)
{
	fprintf(gpFile, "\n InSide Cretae Particles.");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");
	float* localptr = NULL;
    localptr = (float*)malloc(sizeof(float));
	
	/*for(int i =0;i<3;i++)
		localptr[i] = (float*)malloc(3*sizeof(float));*/
	
	//memset(localptr,0,sizeof(float));
	//sizeOfPosArray++;
	return localptr;
	
}

void Push_back_paticle(float* pos_vec,float* old_pos_vec, float coordinate)
{
	/*fprintf(gpFile, "\n Enter in Push_back_paticle.");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");*/
	
	pos_vec = (float*)realloc(pos_vec, (sizeOfPosArray+1)*sizeof(float));
	old_pos_vec = (float*)realloc(old_pos_vec, (sizeOfPosArray+1)*sizeof(float));
	
	/*for(int i =0;i<3;i++)
		pos_vec[i] = (float*)realloc(pos_vec[i], 3*sizeof(float));
	
	for(int i =0;i<3;i++)
		old_pos_vec[i] = (float*)realloc(old_pos_vec[i], 3*sizeof(float*));*/
	
	/*pos_vec[sizeOfPosArray -1][0] = coordinate[0];
	pos_vec[sizeOfPosArray -1][1] = coordinate[1];
	pos_vec[sizeOfPosArray -1][2] = coordinate[2];
	
	old_pos_vec[sizeOfPosArray -1][0] = coordinate[0];
	old_pos_vec[sizeOfPosArray -1][1] = coordinate[1];
	old_pos_vec[sizeOfPosArray -1][2] = coordinate[2];*/
	
	//pos_vec++;
	//old_pos_vec++;
	sizeOfPosArray++;
	pos_vec[sizeOfPosArray -1]= coordinate;
	old_pos_vec[sizeOfPosArray -1] = coordinate;
	
	
	
	
	fprintf(gpFile, "\n Exit from Push_back_paticle  pos_vec[%d] = %f", sizeOfPosArray-1 , pos_vec[sizeOfPosArray-1]);
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");
	
}

#define DAMPING 0.0 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.3  // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 50 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)






#define CLOTHWIDTH   10.0
#define CLOTHHEIGHT  25.0
#define CLOTHWIDTH_NOOFPARTICLES  120
#define CLOTHHEIGHT_NOOFPARTICLES  80
#define NOOFPARTICLE CLOTHWIDTH_NOOFPARTICLES*CLOTHHEIGHT_NOOFPARTICLES
#define NEIGHBOUR_PARTICLES_ARRAY_INDEX CLOTHWIDTH_NOOFPARTICLES*CLOTHHEIGHT_NOOFPARTICLES*4


float Particles_pos_array[NOOFPARTICLE][3];
float Particles_old_pos_array[NOOFPARTICLE][3];

int NeighbourParticlesInddex[NEIGHBOUR_PARTICLES_ARRAY_INDEX][2];
bool MovableStatus[NOOFPARTICLE];
float RestDistance[NEIGHBOUR_PARTICLES_ARRAY_INDEX];
float Particles_Normals[NOOFPARTICLE][3];
float Particles_Accelaration[NOOFPARTICLE][3];
float *GPUParticles_Accelaration = NULL;
size_t tPitch = 0;



int particle_width =CLOTHWIDTH_NOOFPARTICLES, particle_height =CLOTHHEIGHT_NOOFPARTICLES;
float mass = 1.0;
float accelaration[3]={0.0 * TIME_STEPSIZE2, -0.5 * TIME_STEPSIZE2, 0.2 * TIME_STEPSIZE2};
float windForce[3] = {0.0 * TIME_STEPSIZE2, -5.0 * TIME_STEPSIZE2, 2.5 * TIME_STEPSIZE2};
//float**Particles_pos_array; //= create_paticleVectore();
//float* Particles_old_pos_array; //= create_paticleVectore();
int widthaddjustment = 0;
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
	void Movable();
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
			
		case VK_DOWN:
			//MessageBox(hwnd, TEXT("This is VK_ESCAPE"), TEXT("Bhushan msg"), MB_OK);
			//if(widthaddjustment<10)
				widthaddjustment++;
			
			Movable();
			break;	
		case VK_UP:
			//MessageBox(hwnd, TEXT("This is VK_ESCAPE"), TEXT("Bhushan msg"), MB_OK);
			//if(widthaddjustment>0)
				widthaddjustment--;
			
			Movable();
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


GLfloat triangleVertices[] = { 0.0f,1.0f,0.0f,
										-1.0f,-1.0f,0.0f,
										1.0f,-1.0f,0.0f };

GLfloat triangleVertices_normal[] = {0.0f,0.0f,1.0f,
									0.0f,0.0f,1.0f, 
									0.0f,0.0f,1.0f};





float* getParticle(int x, int y) { return Particles_pos_array[y*particle_width + x]; }
float* getParticleNormals(int x, int y) { return Particles_Normals[y*particle_width + x]; }

int getParticles_pos_array_index(int x, int y) { return y*particle_width + x; }



/*void Movable()
{
	for (int i = 0; i < (particle_width - widthaddjustment)/2; i++)
		{
			//getParticle(0+i ,0)->offsetPos(vec3(0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//getParticle(0 + i, 0)->makeUnmovable();
			MovableStatus[getParticles_pos_array_index(0 + i,0)] = false;

			//getParticle(0+i ,0)->offsetPos(vec3(-0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//getParticle(0 + i, 0)->offsetPos(vec3(0.5, -0.5, 0.0));
			//getParticle(num_particles_width-1-i ,0)->makeUnmovable();
			MovableStatus[getParticles_pos_array_index(particle_width-1-i,0)] = false;
			//getParticle(0 + i, num_particles_height - 1)->makeUnmovable();
		}
}*/


void Movable()
{
	for (int i = 0; i < (particle_width - widthaddjustment)/2; i = i+24)
		{
			//getParticle(0+i ,0)->offsetPos(vec3(0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//getParticle(0 + i, 0)->makeUnmovable();
			MovableStatus[getParticles_pos_array_index(0 + i,0)] = false;
			MovableStatus[getParticles_pos_array_index(0 + i+1,0)] = false;
			MovableStatus[getParticles_pos_array_index(0 + i+2,0)] = false;
			MovableStatus[getParticles_pos_array_index(0 + i+3,0)] = false;
			MovableStatus[getParticles_pos_array_index(0 + i+4,0)] = false;

			//getParticle(0+i ,0)->offsetPos(vec3(-0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//getParticle(0 + i, 0)->offsetPos(vec3(0.5, -0.5, 0.0));
			//getParticle(num_particles_width-1-i ,0)->makeUnmovable();
			MovableStatus[getParticles_pos_array_index(particle_width-1-i,0)] = false;
			MovableStatus[getParticles_pos_array_index(particle_width-1-i+1,0)] = false;
			MovableStatus[getParticles_pos_array_index(particle_width-1-i+2,0)] = false;
			MovableStatus[getParticles_pos_array_index(particle_width-1-i+3,0)] = false;
			MovableStatus[getParticles_pos_array_index(particle_width-1-i+4,0)] = false;
			
			//getParticle(0 + i, num_particles_height - 1)->makeUnmovable();
		}
		
		
}



void Cloth(float width, float height, int num_particles_width, int num_particles_height) 
{
		//particles[num_particles_width*num_particles_height];
		// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
		for (int x = 0; x < num_particles_width; x++)
		{
			for (int y = 0; y < num_particles_height; y++)
			{
				float pos[3] = {width * (x / (float)num_particles_width),
					-height * (y / (float)num_particles_height),
				0};
				//particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
				/*Push_back_paticle(Particles_pos_array, Particles_old_pos_array, pos[0]);
				Push_back_paticle(Particles_pos_array, Particles_old_pos_array, pos[1]);
				Push_back_paticle(Particles_pos_array, Particles_old_pos_array, pos[2]);*/
				Particles_pos_array[y*num_particles_width + x][0] = pos[0];
				Particles_pos_array[y*num_particles_width + x][1] = pos[1];
				Particles_pos_array[y*num_particles_width + x][2] = pos[2];
				
				Particles_old_pos_array[y*num_particles_width + x][0] = pos[0];
				Particles_old_pos_array[y*num_particles_width + x][1] = pos[1];
				Particles_old_pos_array[y*num_particles_width + x][2] = pos[2];
			}
		}
		
		int index = 0;
		float distance[3];
		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
		for (int x = 0; x < num_particles_width; x++)
		{
			for (int y = 0; y < num_particles_height; y++)
			{
				if (x < num_particles_width - 1) 
				{
					NeighbourParticlesInddex[index][0] = getParticles_pos_array_index(x,y);
					NeighbourParticlesInddex[index][1] = getParticles_pos_array_index(x+1,y);
					
					
					// the length between particle p1 and p2 in rest configuration
				
					distance[0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] - Particles_pos_array[NeighbourParticlesInddex[index][1]][0];
					distance[1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] - Particles_pos_array[NeighbourParticlesInddex[index][1]][1];
					distance[2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] - Particles_pos_array[NeighbourParticlesInddex[index][1]][2];
					RestDistance[index] = length(distance);
					distance[0] = 0.0f;
					distance[1] = 0.0f;
					distance[2] = 0.0f;
					
					index++;
				}//makeConstraint(getParticle(x, y), getParticle(x + 1, y));
				if (y < num_particles_height - 1) 
				{
					NeighbourParticlesInddex[index][0] = getParticles_pos_array_index(x,y);
					NeighbourParticlesInddex[index][1] = getParticles_pos_array_index(x,y+1);
					
					distance[0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] - Particles_pos_array[NeighbourParticlesInddex[index][1]][0];
					distance[1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] - Particles_pos_array[NeighbourParticlesInddex[index][1]][1];
					distance[2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] - Particles_pos_array[NeighbourParticlesInddex[index][1]][2];
					RestDistance[index] = length(distance);
					distance[0] = 0.0f;
					distance[1] = 0.0f;
					distance[2] = 0.0f;
					
					index++;
				}//makeConstraint(getParticle(x, y), getParticle(x, y + 1));
				if (x < num_particles_width - 1 && y < num_particles_height - 1)
				{
					NeighbourParticlesInddex[index][0] = getParticles_pos_array_index(x,y);
					NeighbourParticlesInddex[index][1] = getParticles_pos_array_index(x+1,y+1);
					
					distance[0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] - Particles_pos_array[NeighbourParticlesInddex[index][1]][0];
					distance[1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] - Particles_pos_array[NeighbourParticlesInddex[index][1]][1];
					distance[2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] - Particles_pos_array[NeighbourParticlesInddex[index][1]][2];
					RestDistance[index] = length(distance);
					distance[0] = 0.0f;
					distance[1] = 0.0f;
					distance[2] = 0.0f;
					
					index++;
				}//makeConstraint(getParticle(x, y), getParticle(x + 1, y + 1));
				if (x < num_particles_width - 1 && y < num_particles_height - 1)
				{
					NeighbourParticlesInddex[index][0] = getParticles_pos_array_index(x+1,y);
					NeighbourParticlesInddex[index][1] = getParticles_pos_array_index(x,y+1);
					
					distance[0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] - Particles_pos_array[NeighbourParticlesInddex[index][1]][0];
					distance[1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] - Particles_pos_array[NeighbourParticlesInddex[index][1]][1];
					distance[2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] - Particles_pos_array[NeighbourParticlesInddex[index][1]][2];
					RestDistance[index] = length(distance);
					distance[0] = 0.0f;
					distance[1] = 0.0f;
					distance[2] = 0.0f;
					
					index++;
					
				}//makeConstraint(getParticle(x + 1, y), getParticle(x, y + 1));
			}
		}
		
		
		Movable();
		
		
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
	

	//Create voa/,
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices_normal), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//Cloth cloth1(20, 10, 30, 30);
	//cloth1.Cloth(20, 10, 30, 30);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);


	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();
	
	
	fprintf(gpFile, "\n Before Cloth");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");
	//Particles_pos_array = create_paticleVectore();
    //Particles_old_pos_array = create_paticleVectore();
	/*Particles_pos_array = (float**)malloc(particle_width * particle_height * sizeof(float*));
	
	for(int i =0; i<3;i++)
	{
		Particles_pos_array[i] = (float*)malloc(3* sizeof(float));
	}*/
	
	for(int index = 0; index < NOOFPARTICLE; index++)
		MovableStatus[index] = true;
	
	for(int x = 0; x < NOOFPARTICLE; x++)
	{
		for(int y = 0; y < 3; y++)
		{
			Particles_Accelaration[x][y] = 0.0f;
		}
	}
	
	 
	/*cudaMalloc((void ***)&GPUParticles_Accelaration, NOOFPARTICLE * sizeof(float*));
	for(int y = 0; y < NOOFPARTICLE; y++)
	{
			cudaMalloc((void **)&GPUParticles_Accelaration[y], 3 * sizeof(float));
	}*/
	
	//cudaMallocPitch((void **)&GPUParticles_Accelaration, &tPitch, 3, sizeof(float3)*NOOFPARTICLE);
	cudaMalloc((void**)&GPUParticles_Accelaration, sizeof(float) * NOOFPARTICLE * 3);
    Cloth(CLOTHWIDTH,CLOTHHEIGHT,CLOTHWIDTH_NOOFPARTICLES,CLOTHHEIGHT_NOOFPARTICLES);
	
	
	fprintf(gpFile, "\n After Cloth");
	fclose(gpFile);
	fopen_s(&gpFile, "Log.txt", "a+");
	
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

void timeStep()
	{
		
		for (int i = 0; i < CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
		{
			//index = 0;
			for (int index = 0; index < NEIGHBOUR_PARTICLES_ARRAY_INDEX; index++)
			{
				//vec3 p1_to_p2 = p2->getPos() - p1->getPos(); // vector from p1 to p2
				float p1_to_p2[3];
				p1_to_p2[0] = Particles_pos_array[NeighbourParticlesInddex[index][1]][0] - Particles_pos_array[NeighbourParticlesInddex[index][0]][0];
				p1_to_p2[1] = Particles_pos_array[NeighbourParticlesInddex[index][1]][1] - Particles_pos_array[NeighbourParticlesInddex[index][0]][1];
				p1_to_p2[2] = Particles_pos_array[NeighbourParticlesInddex[index][1]][2] - Particles_pos_array[NeighbourParticlesInddex[index][0]][2];
												
				float current_distance = length(p1_to_p2); // current distance between p1 and p2
				
				// the length between particle p1 and p2 in rest configuration
				/*float distance[3];
				distance[0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] - Particles_pos_array[NeighbourParticlesInddex[index][1]][0];
				distance[1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] - Particles_pos_array[NeighbourParticlesInddex[index][1]][1];
				distance[2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] - Particles_pos_array[NeighbourParticlesInddex[index][1]][2];
				float rest_distance = length(distance); */
				
				// The offset vector that could moves p1 into a distance of rest_distance to p2
				//vec3 correctionVector = p1_to_p2 * (1 - rest_distance / current_distance); 
				float correctionVector[3];
				correctionVector[0] = p1_to_p2[0] * (1 - RestDistance[index] / current_distance);
				correctionVector[1] = p1_to_p2[1] * (1 - RestDistance[index] / current_distance);
				correctionVector[2] = p1_to_p2[2] * (1 - RestDistance[index] / current_distance);
				
				// Lets make it half that length, so that we can move BOTH p1 and p2.
				//vec3 correctionVectorHalf = correctionVector * 0.5; 
				float correctionVectorHalf[3];
				correctionVectorHalf[0] = correctionVector[0] * 0.5;
				correctionVectorHalf[1] = correctionVector[1] * 0.5;
				correctionVectorHalf[2] = correctionVector[2] * 0.5;
				
				
				// correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
				if(MovableStatus[NeighbourParticlesInddex[index][0]])
				{
					//p1->offsetPos(correctionVectorHalf); 
					Particles_pos_array[NeighbourParticlesInddex[index][0]][0] = Particles_pos_array[NeighbourParticlesInddex[index][0]][0] + correctionVectorHalf[0];
					Particles_pos_array[NeighbourParticlesInddex[index][0]][1] = Particles_pos_array[NeighbourParticlesInddex[index][0]][1] + correctionVectorHalf[1];
					Particles_pos_array[NeighbourParticlesInddex[index][0]][2] = Particles_pos_array[NeighbourParticlesInddex[index][0]][2] + correctionVectorHalf[2];
				}
				
				if(MovableStatus[NeighbourParticlesInddex[index][1]])
				{
					//p2->offsetPos(-correctionVectorHalf); 
					Particles_pos_array[NeighbourParticlesInddex[index][1]][0] = Particles_pos_array[NeighbourParticlesInddex[index][1]][0] - correctionVectorHalf[0];
					Particles_pos_array[NeighbourParticlesInddex[index][1]][1] = Particles_pos_array[NeighbourParticlesInddex[index][1]][1] - correctionVectorHalf[1];
					Particles_pos_array[NeighbourParticlesInddex[index][1]][2] = Particles_pos_array[NeighbourParticlesInddex[index][1]][2] - correctionVectorHalf[2];
				}
			}
		}
		
		for (int index = 0; index < NOOFPARTICLE; index++)
		{
			//(*particle).timeStep(); // calculate the position of each particle at the next time step.
			if(MovableStatus[index])
			{
				float temp[3];
				temp[0] = Particles_pos_array[index][0];
				temp[1] = Particles_pos_array[index][1];
				temp[2] = Particles_pos_array[index][2];
				
				Particles_pos_array[index][0] = Particles_pos_array[index][0] + (Particles_pos_array[index][0] - Particles_old_pos_array[index][0])*(1.0 - DAMPING) + Particles_Accelaration[index][0] * TIME_STEPSIZE2;
				Particles_pos_array[index][1] = Particles_pos_array[index][1] + (Particles_pos_array[index][1] - Particles_old_pos_array[index][1])*(1.0 - DAMPING) + Particles_Accelaration[index][1] * TIME_STEPSIZE2;
				Particles_pos_array[index][2] = Particles_pos_array[index][2] + (Particles_pos_array[index][2] - Particles_old_pos_array[index][2])*(1.0 - DAMPING) + Particles_Accelaration[index][2] * TIME_STEPSIZE2;
				
				Particles_old_pos_array[index][0] = temp[0];
				Particles_old_pos_array[index][1] = temp[1];
				Particles_old_pos_array[index][2] = temp[2];
				
				Particles_Accelaration[index][0] = 0.0f;
				Particles_Accelaration[index][1] = 0.0f;
				Particles_Accelaration[index][2] = 0.0f;
			}
			
			
		}
		
	}



GLfloat angle_cubeX = 0.0f;
GLfloat angle_cubeY = 0.0f;
GLfloat angle_cubeZ = 0.0f;

float* cross(const float* v1, const float* v2)
{
	float crossProduct[3] = {v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]};
	/*crossProduct[0] = (v1[1] * v2[2] - v1[2] * v2[1]) ;
	crossProduct[1] = (v1[2] * v2[0] - v1[0] * v2[2]);
	crossProduct[2] = (v1[0] * v2[1] - v1[1] * v2[0]);*/
	return &crossProduct[0];
}

void addforce()
{
	for(int x = 0; x < NOOFPARTICLE; x++)
	{
		Particles_Accelaration[x][0] = accelaration[0];
		Particles_Accelaration[x][1] = accelaration[1];
		Particles_Accelaration[x][2] = accelaration[2];
	
	}
	
	/*void addforceCudaKernel(float* partcleArray, float* accelaration, int numOfParticles);
	
	addforceCudaKernel((float(*) [120*80*3]) GPUParticles_Accelaration, (float(*) [3])accelaration, NOOFPARTICLE);
	
    cudaError_t error; 
	error = cudaMemcpy(Particles_Accelaration, GPUParticles_Accelaration, NOOFPARTICLE *3*sizeof(float) , cudaMemcpyDeviceToHost);*/
	
	/*fprintf(gpFile, "Value of GPUParticles_Accelaration = %f  %f   %f\n ", GPUParticles_Accelaration[0].x,GPUParticles_Accelaration[0].y,GPUParticles_Accelaration[0].z);
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");
	
	if(cudaSuccess == error)
	{
		fprintf(gpFile, "cudaMemCpy  cudaSuccess\n ");
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");
	}
	
	
	if(cudaErrorInvalidValue == error)
	{
		fprintf(gpFile, "cudaMemCpy  cudaErrorInvalidValue\n ");
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");
	}
	
	if(cudaErrorInvalidDevicePointer == error)
	{
		fprintf(gpFile, "cudaMemCpy  cudaErrorInvalidDevicePointer\n ");
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");
	}
	
	if(cudaErrorInvalidMemcpyDirection == error)
	{
		fprintf(gpFile, "cudaMemCpy  cudaErrorInvalidMemcpyDirection\n ");
		fclose(gpFile);
		fopen_s(&gpFile, "Log.txt", "a+");
	}*/
	
	
	//float Particles_Accelaration[NOOFPARTICLE][3];
}

void applyWindForce()
{
	for (int x = 0; x < particle_width - 1; x++)
	{
		for (int y = 0; y < particle_height - 1; y++)
		{
			//addWindForcesForTriangle(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1), direction);
			//vec3 normal = calcTriangleNormal(p1, p2, p3);
			float *normal;
			float *TrianglePoint1, *TrianglePoint2, *TrianglePoint3;
			float v1[3], v2[3];
			
			TrianglePoint1 = getParticle(x+1,y);
			TrianglePoint2 = getParticle(x,y);
			TrianglePoint3 = getParticle(x,y+1);
			
			
			v1[0] = TrianglePoint2[0] - TrianglePoint1[0];
			v1[1] = TrianglePoint2[1] - TrianglePoint1[1];
			v1[2] = TrianglePoint2[2] - TrianglePoint1[2];
			
			v2[0] = TrianglePoint3[0] - TrianglePoint1[0];
			v2[1] = TrianglePoint3[1] - TrianglePoint1[1];
			v2[2] = TrianglePoint3[2] - TrianglePoint1[2];
			
			normal = cross(&v1[0], &v2[0]);
			
			//vec3 d = normalized(normal);
			float d[3];
			float l = length(&normal[0]);
			d[0] = normal[0]/l;
			d[1] = normal[1]/l;
			d[2] = normal[2]/l;			
			
			float dotproduct = d[0] * windForce[0] + d[1] * windForce[1] + d[2] * windForce[2];
			float force[3] = {normal[0] * dotproduct,normal[1] * dotproduct,normal[1] * dotproduct} ;
			
			//p1->addForce(force);
			int particalIndex = 0;
			particalIndex = getParticles_pos_array_index(x+1,y);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
			//p2->addForce(force);
			particalIndex = getParticles_pos_array_index(x,y);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
			//p3->addForce(force);
			particalIndex = getParticles_pos_array_index(x,y+1);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
				
			
			//addWindForcesForTriangle(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1), direction);
			//normal=NULL;
			//TrianglePoint1=NULL; TrianglePoint2=NULL; TrianglePoint3=NULL;
			//float v1[3], v2[3];
			
			TrianglePoint1 = getParticle(x+1,y+1);
			TrianglePoint2 = getParticle(x+1,y);
			TrianglePoint3 = getParticle(x,y+1);
			
			
			v1[0] = TrianglePoint2[0] - TrianglePoint1[0];
			v1[1] = TrianglePoint2[1] - TrianglePoint1[1];
			v1[2] = TrianglePoint2[2] - TrianglePoint1[2];
			
			v2[0] = TrianglePoint3[0] - TrianglePoint1[0];
			v2[1] = TrianglePoint3[1] - TrianglePoint1[1];
			v2[2] = TrianglePoint3[2] - TrianglePoint1[2];
			
			normal = cross(&v1[0], &v2[0]);
			
			//vec3 d = normalized(normal);
			//float d[3];
			l = length(&normal[0]);
			d[0] = normal[0]/l;
			d[1] = normal[1]/l;
			d[2] = normal[2]/l;			
			
			dotproduct = d[0] * windForce[0] + d[1] * windForce[1] + d[2] * windForce[2];
			force[0] = normal[0] * dotproduct; force[1] = normal[1] * dotproduct; force[2] = normal[1] * dotproduct;
			
			//p1->addForce(force);
			
			particalIndex = getParticles_pos_array_index(x+1,y+1);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
			//p2->addForce(force);
			particalIndex = getParticles_pos_array_index(x+1,y);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
			//p3->addForce(force);
			particalIndex = getParticles_pos_array_index(x,y+1);
			Particles_Accelaration[particalIndex][0] = Particles_Accelaration[particalIndex][0] + force[0] / mass;
			Particles_Accelaration[particalIndex][1] = Particles_Accelaration[particalIndex][1] + force[1] / mass;
			Particles_Accelaration[particalIndex][2] = Particles_Accelaration[particalIndex][2] + force[2] / mass;
			
		}
	}
}

void display()
{
	//code
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(glShaderProgramObject);

	//Declaration of Matrix
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 translateMatrix;
	mat4 rotationMatrixX;
	mat4 rotationMatrixY;
	mat4 rotationMatrixZ;
	mat4 modelViewProjectionMatrix;

	//Intialize above matrix to Identity;
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotationMatrixX = mat4::identity();
	rotationMatrixY = mat4::identity();
	rotationMatrixZ = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary transformation

	//Do necessary Matrix multiplication
	translateMatrix = translate(-5.0f, 15.0f, -40.0f);
	//translateMatrix = translate(0.0f, 0.0f, -6.0f);
	rotationMatrixX = rotate(angle_cubeX, 1.0f, 0.0f, 0.0f);
	rotationMatrixY = rotate(angle_cubeY, 0.0f, 1.0f, 0.0f);
	rotationMatrixZ = rotate(angle_cubeZ, 0.0f, 0.0f, 1.0f);
	translateMatrix = translateMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ;
	modelMatrix = modelMatrix * translateMatrix;

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

	//float accelaration[3]={0.0,-1.0,0.2};
	//cloth1.addForce(vec3(0.5, -0.1, -1.4)*TIME_STEPSIZE2);
	//cloth1.windForce(vec3(5.0, 1.0, 1.7)*TIME_STEPSIZE2);
    //float force[3] = {0.0f, -1.5f, 5.2f};
	//cloth1.gpuaddForce(&force[0]);
	//cloth1.gpuaddForce(vec3(0.0, 0.0,0.0)*TIME_STEPSIZE2);
	addforce();
	//cloth1.windForce(vec3(0.0, -0.5, 0.01)*TIME_STEPSIZE2);
	applyWindForce();

	//cloth1.timeStep(); // calculate the particle positions of the next frame
	timeStep();
	//bind wit vao (This will avoid many repeated binding with vbo)
	glBindVertexArray(vao);

	//Similarly Bind with any texture
	//cloth1.drawShaded();
	//Draw necessary seen
	
	for (int x = 0; x < particle_width -1; x++)
	{
		for (int y = 0; y < particle_height-1 ; y++)
		{
			//vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			float *normal;
			float *TrianglePoint1, *TrianglePoint2, *TrianglePoint3;
			float v1[3], v2[3];
			
			TrianglePoint1 = getParticle(x+1,y);
			TrianglePoint2 = getParticle(x,y);
			TrianglePoint3 = getParticle(x,y+1);
			
			
			v1[0] = TrianglePoint2[0] - TrianglePoint1[0];
			v1[1] = TrianglePoint2[1] - TrianglePoint1[1];
			v1[2] = TrianglePoint2[2] - TrianglePoint1[2];
			
			v2[0] = TrianglePoint3[0] - TrianglePoint1[0];
			v2[1] = TrianglePoint3[1] - TrianglePoint1[1];
			v2[2] = TrianglePoint3[2] - TrianglePoint1[2];
			
			normal = cross(&v1[0], &v2[0]);
			
			/*getParticle(x+1,y)->addToNormal(normal);
			getParticle(x, y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);*/
			
			Particles_Normals[y*particle_width + (x+1)][0] = normal[0];
			Particles_Normals[y*particle_width + (x+1)][1] = normal[1];
			Particles_Normals[y*particle_width + (x+1)][2] = normal[2];
				
			Particles_Normals[y*particle_width + x][0] = normal[0];
			Particles_Normals[y*particle_width + x][1] = normal[1];
			Particles_Normals[y*particle_width + x][2] = normal[2];
			
			Particles_Normals[(y+1)*particle_width + x][0] = normal[0];
			Particles_Normals[(y+1)*particle_width + x][1] = normal[1];
			Particles_Normals[(y+1)*particle_width + x][2] = normal[2];
			
			
			

			

			/*normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
			getParticle(x+1,y+1)->addToNormal(normal);
			getParticle(x + 1, y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);*/
			
			normal = NULL;
			TrianglePoint1 = NULL; TrianglePoint2 = NULL; TrianglePoint3=NULL;
			v1[0] = 0.0f; v1[1] = 0.0f; v1[2] = 0.0f;
			v2[0] = 0.0f; v2[1] = 0.0f; v2[2] = 0.0f;
			
			TrianglePoint1 = getParticle(x+1,y+1);
			TrianglePoint2 = getParticle(x+1,y);
			TrianglePoint3 = getParticle(x,y+1);
			
			
			v1[0] = TrianglePoint2[0] - TrianglePoint1[0];
			v1[1] = TrianglePoint2[1] - TrianglePoint1[1];
			v1[2] = TrianglePoint2[2] - TrianglePoint1[2];
			
			v2[0] = TrianglePoint3[0] - TrianglePoint1[0];
			v2[1] = TrianglePoint3[1] - TrianglePoint1[1];
			v2[2] = TrianglePoint3[2] - TrianglePoint1[2];
			
			normal = cross(&v1[0], &v2[0]);
			
			/*getParticle(x+1,y)->addToNormal(normal);
			getParticle(x, y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);*/
			
			Particles_Normals[(y+1)*particle_width + (x+1)][0] = normal[0];
			Particles_Normals[(y+1)*particle_width + (x+1)][1] = normal[1];
			Particles_Normals[(y+1)*particle_width + (x+1)][2] = normal[2];
				
			Particles_Normals[y*particle_width + (x+1)][0] = normal[0];
			Particles_Normals[y*particle_width + (x+1)][1] = normal[1];
			Particles_Normals[y*particle_width + (x+1)][2] = normal[2];
			
			Particles_Normals[(y+1)*particle_width + x][0] = normal[0];
			Particles_Normals[(y+1)*particle_width + x][1] = normal[1];
			Particles_Normals[(y+1)*particle_width + x][2] = normal[2];
				
	
		}
	}
	
	for (int x = 0; x < particle_width - 1; x++)
	{
		for (int y = 0; y < particle_height - 1; y++)
		{
			float* point = getParticle(x, y + 1);
			triangleVertices[0] = point[0];
			triangleVertices[1] = point[1];
			triangleVertices[2] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x, y+1);
			
			
			triangleVertices_normal[0] = point[0];
			triangleVertices_normal[1] = point[1];
			triangleVertices_normal[2] = point[2];
			
			point =  NULL;
			point = getParticle(x, y);
			triangleVertices[3] = point[0];
			triangleVertices[4] = point[1];
			triangleVertices[5] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x, y);
			triangleVertices_normal[3] = point[0];
			triangleVertices_normal[4] = point[1];
			triangleVertices_normal[5] = point[2];
			
			point =  NULL;
			point = getParticle(x + 1, y);
			triangleVertices[6] = point[0];
			triangleVertices[7] = point[1];
			triangleVertices[8] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x+1, y);
			triangleVertices_normal[6] = point[0];
			triangleVertices_normal[7] = point[1];
			triangleVertices_normal[8] = point[2];
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices_normal), triangleVertices_normal, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
			point =  NULL;
			point = getParticle(x, y + 1);
			triangleVertices[0] = point[0];
			triangleVertices[1] = point[1];
			triangleVertices[2] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x, y+1);
			triangleVertices_normal[0] = point[0];
			triangleVertices_normal[1] = point[1];
			triangleVertices_normal[2] = point[2];
			
			point =  NULL;
			point = getParticle(x+1, y);
			triangleVertices[3] = point[0];
			triangleVertices[4] = point[1];
			triangleVertices[5] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x+1, y);
			triangleVertices_normal[3] = point[0];
			triangleVertices_normal[4] = point[1];
			triangleVertices_normal[5] = point[2];
			
			point =  NULL;
			point = getParticle(x + 1, y+1);
			triangleVertices[6] = point[0];
			triangleVertices[7] = point[1];
			triangleVertices[8] = point[2];
			
			point =  NULL;
			point = getParticleNormals(x+1, y+1);
			triangleVertices_normal[6] = point[0];
			triangleVertices_normal[7] = point[1];
			triangleVertices_normal[8] = point[2];
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),triangleVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices_normal), triangleVertices_normal, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}
	
	//unbind vao
	glBindVertexArray(0);

	//unusal program

	glUseProgram(0);

	/*if (clothHeight < 15)
	{
		clothHeight = clothHeight + 1;
		
		cloth1.ClothReintialization(5, clothHeight, 30, 30);
	}

	if (clothparticleHeight < 25)
	{
		clothparticlewidth = clothparticlewidth + 5;
		clothparticleHeight = clothparticleHeight + 5;
	}*/
   //Sleep(200);
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

	if (vbo) {
		glDeleteBuffers(1, &vbo);
		vbo = 0;
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




