
#include<windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
#include "song.h"


//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "Winmm.lib")


//******* Macros **********//
#define WIN_WIDTH          800
#define WIN_HEIGHT         600

//**** Global variable declaration ************//
bool bFullScreen = false;
DWORD dwStyle;
HWND ghWnd = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;

int gllookat_status = 0;

//********* Global function declaration ***********//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//****** Function Declaration ************//

	int intialize(void);
	void display(void);

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

		fprintf(gpFile, "Intialization Successfully.!!!\n");

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
			//ToggelFullScreen();
			break;
		case 0x5A:
			gllookat_status++;
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
	void ToggelFullScreen(void);
	fprintf(gpFile, "Inside Intialization.!!!\n");
	// ******* Function declaration **********//
	void resize(int, int);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	resize(WIN_WIDTH, WIN_HEIGHT);
	ToggelFullScreen();
	PlaySound(
		MAKEINTRESOURCE(ID_MYSONG),
		NULL, //GetModuleHandle(NULL),
		 SND_RESOURCE | SND_ASYNC 
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}


void drawplane()
{
	glBegin(GL_POLYGON);
	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	glVertex3f(0, 0.05, 0);
	glVertex3f(0.2, 0.05, 0);
	glVertex3f(0.25, 0, 0);
	glVertex3f(0.2, -0.05, 0);
	glVertex3f(0, -0.05, 0);

	glEnd();

	// Write IAF
	// I

	glColor3d(1, 1, 1);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	// I
	glVertex3f(0.05, 0.02, 0);
	glVertex3f(0.05, -0.02, 0);
	glVertex3f(0.03, 0.02, 0);
	glVertex3f(0.07, 0.02, 0);
	glVertex3f(0.03, -0.02, 0);
	glVertex3f(0.07, -0.02, 0);

	//A
	glVertex3f(0.095, -0.003, 0);
	glVertex3f(0.123, -0.003, 0);
	glVertex3f(0.085, -0.025, 0);
	glVertex3f(0.105, 0.025, 0);
	glVertex3f(0.105, 0.025, 0);
	glVertex3f(0.13, -0.025, 0);

	//F
	glVertex3f(0.15, 0.023, 0);
	glVertex3f(0.175, 0.023, 0);
	glVertex3f(0.15, 0.0, 0);
	glVertex3f(0.175, 0.0, 0);
	glVertex3f(0.15, 0.025, 0);
	glVertex3f(0.15, -0.025, 0);

	glEnd();


	//draw wing1

	float x, y;

	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);

	for (double i = 0; i <= 1.5;) {
		glBegin(GL_TRIANGLES);
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, 0.06 + y);

		i = i + 0.3;
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);
		glVertex2d(x, 0.06 + y);

		glVertex2d(0.03, 0.06);
		glEnd();

	}

	//draw wing2

	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	for (double i = 0; i <= 1.5;) {
		glBegin(GL_TRIANGLES);
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, -0.06 - y);
		i = i + 0.3;
		x = 0.13 * cos(i);
		y = 0.03 * sin(i);

		glVertex2d(x, -0.06 - y);

		glVertex2d(0.03, -0.06);
		glEnd();

	}

	//draw exaust part

	glBegin(GL_QUADS);
	glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
	glVertex3f(-0.02f, 0.05, 0);
	glVertex3f(-0.01, 0.05, 0);

	glVertex3f(-0.01, -0.05, 0);
	glVertex3f(-0.02f, -0.05, 0);
	glEnd();

	//draw Flag

	glBegin(GL_QUADS);
	glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
	glVertex3f(-0.029, 0.0375, 0);
	glVertex3f(-0.029, 0.0125, 0);

	glVertex3f(-0.12, 0.0125, 0);
	glVertex3f(-0.12, 0.0375, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(-0.029, 0.0125, 0);
	glVertex3f(-0.029, -0.0125, 0);
	glVertex3f(-0.12, -0.0125, 0);
	glVertex3f(-0.12, 0.0125, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.029, -0.0125, 0);
	glVertex3f(-0.029, -0.0375, 0);

	glVertex3f(-0.12, -0.0375, 0);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.12, -0.0125, 0);
	glEnd();
}

void display()
{
	

	static float itranslate = -0.65f;
	static float ntranslate = 1.4f;
	static float i2translate = -1.3f;
	static float atranslate = 0.7f;

	static float planetranslate = -1.7f;

	static float dsaffron_translate[3] = { 0,0,0 };
	static float dgreen_translate[3] = { 0,0,0 };

	static bool iDone = false;
	static bool nDone = false;
	static bool dDone = false;
	static bool i2Done = false;
	static bool aDone = false;
	static bool flag = false;
	
	static bool downplanedone = false;

	bool x1done, x2done, x2i2cross;
	x1done = x2done = x2i2cross = false;


	static float Upper_plane_xtranslation = -1.7f;
	static float Upper_plane_ytranslation = 1.1f;

	static float Upper_plane_rotation_angle = -90.0f;

	static float down_plane_xtranslation = -1.7f;
	static float down_plane_ytranslation = -1.1f;
	//static float down_plane_translation_angle = 180.0f;
	static float down_plane_rotation_angle = 90.0f;
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (downplanedone == false)
	{
		if (dDone) {

			// Draw Down Plane

			

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//glTranslatef(down_plane_xtranslation + (0.56 * sin(down_plane_translation_angle)), down_plane_ytranslation + (1.1 * sin(down_plane_translation_angle)), -2.0f);
			glTranslatef(down_plane_xtranslation, down_plane_ytranslation, -2.0f);
			glRotatef(down_plane_rotation_angle, 0, 0, 1);
			//*****************************************************************************************************
			// Draw Plane
			
			drawplane();

			
		

	// Draw Upper Plane


			

			// Draw Plane
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(Upper_plane_xtranslation, Upper_plane_ytranslation, -2.0f);
			glRotatef(Upper_plane_rotation_angle, 0, 0, 1);

			drawplane();
		
			
		}

	}
		
	if (dDone) {

		if (down_plane_xtranslation < -0.91) {


			down_plane_ytranslation = down_plane_ytranslation + 0.0035;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.3f;

			
		}
		else {
			downplanedone = true;
		}

		if (down_plane_xtranslation > 0.73) {
			downplanedone = false;
			down_plane_ytranslation = down_plane_ytranslation - 0.0035;
			down_plane_rotation_angle = down_plane_rotation_angle - 0.3f;

		}
				
		if (Upper_plane_xtranslation < -0.91) {


			Upper_plane_ytranslation = Upper_plane_ytranslation - 0.0035;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.3f;
		}

		if (Upper_plane_xtranslation > 0.73) {

			Upper_plane_ytranslation = Upper_plane_ytranslation + 0.0035;
			Upper_plane_rotation_angle = Upper_plane_rotation_angle + 0.3f;

		}

		if (planetranslate < -0.74) {
			down_plane_xtranslation = down_plane_xtranslation + 0.0025f;
			Upper_plane_xtranslation = Upper_plane_xtranslation + 0.0025f;
		}
		else
		{
			down_plane_xtranslation = down_plane_xtranslation + 0.0038f;
			Upper_plane_xtranslation = Upper_plane_xtranslation + 0.0038f;
		}
		

	}
	










	//Draw I


	if (itranslate < 0.001)
		itranslate = itranslate + 0.0025;
	else
		iDone = true;


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(itranslate, 0.0f, -2.0f);
	glLineWidth(10.0f);
	glBegin(GL_LINES);
	glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
	glVertex3f(-0.85, 0.6, 0);
	glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
	glVertex3f(-0.85, -0.6, 0);
	glEnd();

	//Draw N

	if (aDone) {
		if (ntranslate > 0)
			ntranslate = ntranslate - 0.0072;
		else
			nDone = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, ntranslate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.6, -0.6, 0);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.6, 0.6, 0);

		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.6, 0.6, 0);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.3, -0.6, 0);

		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(-0.3, -0.6, 0);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(-0.3, 0.6, 0);
		glEnd();
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

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(-0.15, -0.6, 0);
		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(-0.15, 0.6, 0);

		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(-0.2, 0.588, 0);
		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(0.1, 0.588, 0);


		glColor3d(dsaffron_translate[0], dsaffron_translate[1], dsaffron_translate[2]);
		glVertex3f(0.1, 0.6, 0);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(0.1, -0.6, 0);

		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(0.1, -0.588, 0);
		glColor3d(dgreen_translate[0], dgreen_translate[1], dgreen_translate[2]);
		glVertex3f(-0.2, -0.588, 0);
		glEnd();
	}



	//Draw I2

	if (nDone) {
		if (i2translate < 0)
			i2translate = i2translate + 0.008;
		else
			i2Done = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, i2translate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
		glVertex3f(0.35, 0.6, 0);
		glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
		glVertex3f(0.35, -0.6, 0);
		glEnd();
	}


	


	//Draw A
	if (iDone) {
		if (atranslate > 0)
			atranslate = atranslate - 0.0025;
		else
			aDone = true;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(atranslate, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);

	
		if (x2done == false) {
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.6, -0.6, 0);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
		}

		
		if (x1done == false) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.9, -0.6, 0);
		}
		

		glEnd();
	}

	//Middle Plane start
	if (dDone) {
		if (planetranslate < -0.74) {
			planetranslate = planetranslate + 0.0025;
			
		    
		}
		else if (planetranslate < 1.5) {
			planetranslate = planetranslate + 0.0038;
			flag = true;
		}
		

			


		// Draw Plane
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(planetranslate, 0.0f, -2.0f);

		glBegin(GL_POLYGON);
		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		glVertex3f(0, 0.05, 0);
		glVertex3f(0.2, 0.05, 0);
		glVertex3f(0.25, 0, 0);
		glVertex3f(0.2, -0.05, 0);
		glVertex3f(0, -0.05, 0);

		glEnd();

		// Write IAF
		// I
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//glTranslatef(planetranslate, 0.0, -2.0f);
		glColor3d(1, 1, 1);
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		// I
		glVertex3f(0.05, 0.02, 0);
		glVertex3f(0.05, -0.02, 0);
		glVertex3f(0.03, 0.02, 0);
		glVertex3f(0.07, 0.02, 0);
		glVertex3f(0.03, -0.02, 0);
		glVertex3f(0.07, -0.02, 0);

		
		//A
		glVertex3f(0.095, -0.003, 0);
		glVertex3f(0.123, -0.003, 0);
		glVertex3f(0.085, -0.025, 0);
		glVertex3f(0.105, 0.025, 0);
		glVertex3f(0.105, 0.025, 0);
		glVertex3f(0.13, -0.025, 0);

		//F
		glVertex3f(0.15, 0.023, 0);
		glVertex3f(0.175, 0.023, 0);
		glVertex3f(0.15, 0.0, 0);
		glVertex3f(0.175, 0.0, 0);
		glVertex3f(0.15, 0.025, 0);
		glVertex3f(0.15, -0.025, 0);
		
		glEnd();
		

		//draw wing1

		float x, y;

		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);

		for (double i = 0; i <= 1.5;) {
			glBegin(GL_TRIANGLES);
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, 0.06 + y);

			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);
			glVertex2d(x, 0.06 + y);

			glVertex2d(0.03, 0.06);
			glEnd();

		}

		//draw wing2

		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		for (double i = 0; i <= 1.5;) {
			glBegin(GL_TRIANGLES);
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, -0.06 - y);
			i = i + 0.3;
			x = 0.13 * cos(i);
			y = 0.03 * sin(i);

			glVertex2d(x, -0.06 - y);

			glVertex2d(0.03, -0.06);
			glEnd();

		}

		//draw exaust part
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(planetranslate -0.02f, 0.0f, -2.0f);

		glBegin(GL_QUADS);
		glColor3f(186.0 / 255.0, 226.0 / 255.0, 238.0 / 255.0);
		glVertex3f(0, 0.05, 0);
		glVertex3f(0.01, 0.05, 0);

		glVertex3f(0.01, -0.05, 0);
		glVertex3f(0, -0.05, 0);
		glEnd();

		//draw Flag
		if (flag)
		{
			
			static float x = -0.03f;
			static float x1, x2 = -0.855f;
			static bool flag_blur = false;

			if (planetranslate < 1.5) {
				x1 = x + planetranslate;
			}
			else {
				flag_blur = true;
			}
				

			if (planetranslate > 1.5 && x2 < 0.667) {
				x2 = x2 + 0.00247;
				
			}

			if (x2 > 0.667 ) {
				x2done = true;
				
			}
			if (x2 > 0.1)
			{
				x2i2cross = true;
			}
			if (x2 > 0.35)
			{
				x2done = true;
			}

			if (planetranslate > 1.5  && x1 > 0.82) {
				
				x1 = x1 - 0.0011;
			}

			if (planetranslate > 1.5 && x1 < 0.82) {
				
				x1done = true;
			}
				
				
			if (flag_blur) {


				// safron
				static float flagsafron[3] = { 255.0 / 255.0 , 153.0 / 255.0, 51.0 / 255.0 };

				flagsafron[0] = flagsafron[0] - ((255.0 / 255.0) / 650);
				flagsafron[1] = flagsafron[1] - ((153.0 / 255.0) / 650);
				flagsafron[2] = flagsafron[2] - ((51.0 / 255.0) / 650);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glTranslatef(0.0, 0.0f, -2.0f);

				
				glBegin(GL_QUADS);
				glColor3f(flagsafron[0], flagsafron[1], flagsafron[2]);
				glVertex3f(0.667, 0.0375, 0);
				glVertex3f(0.667, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(x2, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, 0.0375, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
				glVertex3f(0.667, 0.0375, 0);
				glVertex3f(0.667, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(0.82, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, 0.0375, 0);
				glEnd();




				glBegin(GL_QUADS);
				glColor3f(flagsafron[0], flagsafron[1], flagsafron[2]);
				glVertex3f(x1, 0.0375, 0);
				glVertex3f(x1, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(0.82, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, 0.0375, 0);
				glEnd();


				// white

				static float flagwhite = 255.0 / 255.0;

				flagwhite = flagwhite - ((255.0 / 255.0) / 650);
				

				glBegin(GL_QUADS);
				glColor3f(flagwhite, flagwhite, flagwhite);
				glVertex3f(0.667, 0.0125, 0);
				glVertex3f(0.667, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0125, 0);
				glVertex3f(x2, 0.0125, 0);
				glEnd();



				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
				glVertex3f(0.667, 0.0125, 0);
				glVertex3f(0.667, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0125, 0);
				glVertex3f(0.82, 0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(flagwhite, flagwhite, flagwhite);
				glVertex3f(x1, 0.0125, 0);
				glVertex3f(x1, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0125, 0);
				glVertex3f(0.82, 0.0125, 0);
				glEnd();



				// green

				static float flaggreen[3] = { 18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0 };

				flaggreen[0] = flaggreen[0] - ((18.0 / 255.0) / 650);
				flaggreen[1] = flaggreen[1] - ((136.0 / 255.0) / 650);
				flaggreen[2] = flaggreen[2] - ((7.0 / 255.0) / 650);



				glBegin(GL_QUADS);
				glColor3f(flaggreen[0], flaggreen[1], flaggreen[2]);
				glVertex3f(0.667, -0.0125, 0);
				glVertex3f(0.667, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0375, 0);
				glVertex3f(x2, -0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(0.667, -0.0125, 0);
				glVertex3f(0.667, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0375, 0);
				glVertex3f(0.82, -0.0125, 0);
				glEnd();


				glBegin(GL_QUADS);
				glColor3f(flaggreen[0], flaggreen[1], flaggreen[2]);
				glVertex3f(x1, -0.0125, 0);
				glVertex3f(x1, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(0.82, -0.0375, 0);
				glVertex3f(0.82, -0.0125, 0);
				glEnd();

			}
			else {


				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glTranslatef(0.0, 0.0f, -2.0f);

				glBegin(GL_QUADS);
				glColor3f(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
				glVertex3f(x1, 0.0375, 0);
				glVertex3f(x1, 0.0125, 0);
				//x = x + planetranslate;
				glVertex3f(x2, 0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, 0.0375, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(1, 1, 1);
				glVertex3f(x1, 0.0125, 0);
				glVertex3f(x1, -0.0125, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0125, 0);
				glVertex3f(x2, 0.0125, 0);
				glEnd();

				glBegin(GL_QUADS);
				glColor3f(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(x1, -0.0125, 0);
				glVertex3f(x1, -0.0375, 0);
				//glColor3f(1, 1, 1);
				glVertex3f(x2, -0.0375, 0);
				glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
				glVertex3f(x2, -0.0125, 0);
				glEnd();
			}
		}
		else {
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(planetranslate - 0.02f, 0.0f, -2.0f);

			glBegin(GL_QUADS);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(-0.009, 0.0375, 0);
			glVertex3f(-0.009, 0.0125, 0);

			glVertex3f(-0.1, 0.0125, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, 0.0375, 0);
			glEnd();

			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			glVertex3f(-0.009, 0.0125, 0);
			glVertex3f(-0.009, -0.0125, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, -0.0125, 0);
			glVertex3f(-0.1, 0.0125, 0);
			glEnd();

			glBegin(GL_QUADS);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(-0.009, -0.0125, 0);
			glVertex3f(-0.009, -0.0375, 0);
			//glColor3f(1, 1, 1);
			glVertex3f(-0.1, -0.0375, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(-0.1, -0.0125, 0);
			glEnd();
		}
		

	}

	




	//Draw I2

	if (nDone) {
		/*if (i2translate < 0)
			i2translate = i2translate + 0.008;
		else
			i2Done = true;*/

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, i2translate, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);
		if (x2i2cross) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.35, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.35, -0.6, 0);

		}
		
		glEnd();
	}


	
	//Draw A
	if (iDone) {
		/*if (atranslate > 0)
			atranslate = atranslate - 0.0025;
		else
			aDone = true;*/

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(atranslate, 0.0f, -2.0f);
		glLineWidth(10.0f);
		glBegin(GL_LINES);

		
		if (x2done == true) {
		//if (x2done) {
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.6, -0.6, 0);
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
		}


		if (x1done == true) {
		//if (x2done) {
			glColor3d(255.0 / 255.0, 153.0 / 255.0, 51.0 / 255.0);
			glVertex3f(0.75, 0.6, 0);
			glColor3d(18.0 / 255.0, 136.0 / 255.0, 7.0 / 255.0);
			glVertex3f(0.9, -0.6, 0);
		}


		glEnd();
	}

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
