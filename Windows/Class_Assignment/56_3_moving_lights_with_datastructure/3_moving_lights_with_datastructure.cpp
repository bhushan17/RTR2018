
#include<windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

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

bool bLight = false;
float cuboid_rotation_angle = 0.0f;
//light configuration parameter

struct Light {
	GLfloat Ambient[4];
	GLfloat Diffuse[4];
	GLfloat Specular[4];
	GLfloat Position[4];
	GLfloat LightAngle;
};

Light lights[3] = { { { 0.0f,0.0f,0.0f,1.0 },
					  { 1.0,0.0f,0.0f,1.0 },
					  { 1.0f, 0.0f, 0.0f, 1.0f },
					  { 0.0f,0.0f,0.0f,1.0 },0.0f},
                    {{ 0.0f,0.0f,0.0f,1.0 },
					  { 0.0f,1.0f,0.0,1.0 },
					   { 0.0f, 1.0f, 0.0f, 1.0f },
					   { 0.0,0.0f,0.0f,1.0 },0.0f},
                    {{ 0.0f,0.0f,0.0f,1.0 },
					  { 0.0f,0.0f,1.0,1.0 },
					  { 0.0f, 0.0f, 1.0f, 1.0f },
					  { 0.0,0.0f,0.0f,1.0 },0.0f}
};

/*
GLfloat LightAmbientzero[] = { 0.0f,0.0f,0.0f,1.0 };
GLfloat LightDiffusezero[] = { 1.0,0.0f,0.0f,1.0 };
GLfloat LightSpecularzero[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightPositionzero[] = { 0.0f,0.0f,0.0f,1.0 };


GLfloat LightAmbientOne[] = { 0.0f,0.0f,0.0f,1.0 };
GLfloat LightDiffuseOne[] = { 0.0f,1.0f,0.0,1.0 };
GLfloat LightSpecularOne[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat LightPositionOne[] = { 0.0,0.0f,0.0f,1.0 };

GLfloat LightAmbientTwo[] = { 0.0f,0.0f,0.0f,1.0 };
GLfloat LightDiffuseTwo[] = { 0.0f,0.0f,1.0,1.0 };
GLfloat LightSpecularTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LightPositionTwo[] = { 0.0,0.0f,0.0f,1.0 };

GLfloat LightAnglezero = 0.0f;
GLfloat LightAngleone = 0.0f;
GLfloat LightAngletwo = 0.0f;
*/

// Material Configuration
GLfloat MaterialAmbient[] = { 0.0,0.0,0.0,1.0 };
GLfloat MaterialDiffuse[] = { 1.0,1.0,1.0,1.0 };
GLfloat MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialShianyness[] = { 128.0f };

GLUquadric *quadric = NULL;
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
				glEnable(GL_LIGHTING);
			}
			else {
				bLight = false;
				glDisable(GL_LIGHTING);
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

int intialize(void)
{
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


	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	/*glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientzero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffusezero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularzero);
	//glLightfv(GL_LIGHT0, GL_POSITION, LightPositionzero);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularOne);
	//glLightfv(GL_LIGHT1, GL_POSITION, LightPositionOne);

	glLightfv(GL_LIGHT2, GL_AMBIENT, LightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuseTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, LightSpecularTwo);
	//glLightfv(GL_LIGHT2, GL_POSITION, LightPositionTwo);*/

	glLightfv(GL_LIGHT0, GL_AMBIENT, lights[0].Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[0].Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lights[0].Specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, lights[0].Position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lights[1].Ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[1].Diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lights[1].Specular);
	//glLightfv(GL_LIGHT1, GL_POSITION, lights[1].Position);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lights[2].Ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lights[2].Diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lights[2].Specular);
	//glLightfv(GL_LIGHT2, GL_POSITION, lights[2].Position);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void update()
{
	if (lights[0].LightAngle > 360.0f)
	{
		lights[0].LightAngle = 0.0f;
	}
	else {
		lights[0].LightAngle = lights[0].LightAngle + 1.0f;
	}

	if (lights[1].LightAngle> 360.0f)
	{
		lights[1].LightAngle = 0.0f;
	}
	else {
		lights[1].LightAngle = lights[1].LightAngle + 1.0f;
	}

	if (lights[2].LightAngle > 360.0f)
	{
		lights[2].LightAngle = 0.0f;
	}
	else {
		lights[2].LightAngle = lights[2].LightAngle + 1.0f;
	}

}
void display()
{
	//code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Cuboid

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Light Zero
	glPushMatrix();
	glRotatef(lights[0].LightAngle, 1.0f, 0.0f, 0.0f);
	lights[0].Position[1] = lights[0].LightAngle;
	glLightfv(GL_LIGHT0, GL_POSITION, lights[0].Position);
	glPopMatrix();

	//Light One
	glPushMatrix();
	glRotatef(lights[1].LightAngle, 0.0f, 1.0f, 0.0f);
	lights[1].Position[0] = lights[1].LightAngle;
	glLightfv(GL_LIGHT1, GL_POSITION, lights[1].Position);
	glPopMatrix();

	//Light TWO
	glPushMatrix();
	glRotatef(lights[2].LightAngle, 0.0f, 0.0f, 1.0f);
	lights[2].Position[0] = lights[2].LightAngle;
	glLightfv(GL_LIGHT2, GL_POSITION, lights[2].Position);
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.2f, 60, 60);
	glPopMatrix();

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
