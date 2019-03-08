
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


void display()
{
	//code

	float x, y;

	static float x_Translate = 0.0f, z_Translate = -10.0f, y_aTranslate = 0.0f;
	static float x_Translate_moon = 0.0f, y_aTranslate_moon = 0.0f;
	
	static float angle = 0.0f;
	
	static float moon_angle = 0.0f;
	static float z_Translate_moon = -20.0f;

	int status;
	int eart_status;
	static int star_status;
			
	static float sun_roatate = 0.0f;

	static float star_ztranslation = -5.0f;
	

	float starArray[30][2] = { {-3.0f,4.2f},
							{6.5f,-3.7f},
							{-5.0f,1.0f},
							{-4.0f,-5.0f},
							{6.0f,3.5f},
							{3.5f,-3.7f},
							{5.0f,1.0f},
							{6.0f,5.0f},
							{5.0f,-4.0f},
							{-2.0f,-4.0f},
							{-6.0f,4.5f}, 
							{4.0f,-3.0f},
							{-5.0f,4.2f},
							{-3.0f,1.0f},
							{-6.0f,-7.0f},
							{4.0f,-3.0f},
							{3.0f,3.5f},
							{-7.0f,-5.0f},
							{6.0f,3.0f},
							{2.0f,5.0f},
							{-7.0f,6.0f},
							{-2.5f,-6.0f},
							{-6.0f,6.5f}, 
							{-6.0f,-3.0f},
							{-6.0f,-3.0f},
							{-4.0f,4.0f}, 
							{-7.52f,-3.0f}, 
							{-7.0f,8.0f},
							{7.0f,8.0f},
							{7.0f,-8.0f}, };

	   	  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	if (z_Translate < -10.0f)
	{
		status = 0;
	}
	else {

		status = 1;
	}

	if (z_Translate_moon < -10.0f)
	{
		eart_status = 0;
	}
	else {

		eart_status = 1;
	}



	//draw star
	if (star_status < 2) {

		for (int z = 0;z < 31; z = z + 2) {

			glLoadIdentity();

			glTranslatef(starArray[z][0], starArray[z][1], -13.0f);

			glColor3f(1, 1, 1);
			for (double i = 0; i <= 360;) {
				glBegin(GL_TRIANGLES);
				x = 0.02 * cos(i);
				y = 0.02 * sin(i);

				glVertex2d(x, y);
				i = i + .5;
				x = 0.02 * cos(i);
				y = 0.02 * sin(i);

				glVertex2d(x, y);


				glVertex2d(0, 0);
				glEnd();
				i = i + .5;
			}
		}
	}
	else if (star_status < 4)
	{
		for (int z = 1;z < 31;z = z + 2) {

			glLoadIdentity();

			glTranslatef(starArray[z][0], starArray[z][1], -13.0f);

			glColor3f(1, 1, 1);
			for (double i = 0; i <= 360;) {
				glBegin(GL_TRIANGLES);
				x = 0.02 * cos(i);
				y = 0.02 * sin(i);

				glVertex2d(x, y);
				i = i + .5;
				x = 0.02 * cos(i);
				y = 0.02 * sin(i);

				glVertex2d(x, y);


				glVertex2d(0, 0);
				glEnd();
				i = i + .5;
			}
		}
	}
	else
		star_status = 0;




	star_status++;











	// Draw sun if earth is rotate infront of sun
	if (status == 1) {
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -9.0f);
		glRotatef(sun_roatate, 0, 0, 1);

		
		for (double i = 0; i <= 360;) {
			glBegin(GL_TRIANGLES);
			x = 0.3 * cos(i);
			y = 0.3 * sin(i);
			glColor3f(1, 0, 0);
			
			glVertex2d(x, y);
			i = i + .5;
			x = 1.3 * cos(i);
			y = 1.3 * sin(i);
			glColor3f(1, 0.7, 0);
			glVertex2d(x, y);
			glColor3f(1, 0, 0);
			glVertex2d(0, 0);
			glEnd();
			i = i + .5;
		}
		

	}

	




	// draw Earth
	if (eart_status == 0) {
		glLoadIdentity();
		glTranslatef(x_Translate, y_aTranslate, z_Translate);

		glColor3f(0, 0, 1);

		for (double i = 0; i <= 360;) {
			glBegin(GL_TRIANGLES);
			x = 0.5 * cos(i);
			y = 0.5 * sin(i);

			glVertex2d(x, y);
			i = i + .5;
			x = 0.5 * cos(i);
			y = 0.5 * sin(i);

			glVertex2d(x, y);

			glVertex2d(0, 0);
			glEnd();
			i = i + .5;
		}
	}

	// Earth rotation translation

	if (angle <= 360) {
		x_Translate = 4 * cos(angle);
		y_aTranslate = 0.7 * sin(angle);

		if (x_Translate > 0) {
			z_Translate = z_Translate - 0.04;


		}
		else {
			z_Translate = z_Translate + 0.04;


		}
		angle = angle + 0.05f;

	}
	else {
		angle = 0.0f;


	}


	//Draw Moon

	glLoadIdentity();
	glTranslatef(x_Translate_moon, y_aTranslate_moon, z_Translate);

	glColor3f(1, 1, 1);

	for (double i = 0; i <= 360;) {
		glBegin(GL_TRIANGLES);
		x = 0.19 * cos(i);
		y = 0.19 * sin(i);
		glVertex2d(x, y);
		i = i + .5;
		x = 0.19 * cos(i);
		y = 0.19 * sin(i);
		glVertex2d(x, y);
		glVertex2d(0, 0);
		glEnd();
		i = i + .5;
	}

	x_Translate_moon = x_Translate - (1.1 * cos(moon_angle));
	y_aTranslate_moon = y_aTranslate - (0.1 * sin(moon_angle));
	moon_angle = moon_angle + 0.5f;

	// Moon translation path
	if (moon_angle <= 2.5f) {
		z_Translate_moon = -14.0f;
	}
	else if (moon_angle <= 5.0f) {
		z_Translate_moon = 14.0f;
	}
	else {
		moon_angle = 0.0f;
	}

	// draw Earth
	if (eart_status == 1) {
		glLoadIdentity();
		glTranslatef(x_Translate, y_aTranslate, z_Translate);
		glColor3f(0, 0, 1);

		for (double i = 0; i <= 360;) {
			glBegin(GL_TRIANGLES);
			x = 0.5 * cos(i);
			y = 0.5 * sin(i);
			
			glVertex2d(x, y);
			i = i + .5;
			x = 0.5 * cos(i);
			y = 0.5 * sin(i);
			
			glVertex2d(x, y);
			
			glVertex2d(0, 0);
			glEnd();
			i = i + .5;
		}
		
	}







	// Draw sun if earth is rotate backside of sun
	if (status == 0) {
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -9.0f);
		glRotatef(sun_roatate, 0, 0, 1);

		glColor3f(1, 0.2, 0);
		for (double i = 0; i <= 360;) {
			glBegin(GL_TRIANGLES);
			x = 0.3 * cos(i);
			y = 0.3 * sin(i);
			glColor3f(1, 0, 0);

			glVertex2d(x, y);
			i = i + .5;
			x = 1.3 * cos(i);
			y = 1.3 * sin(i);
			glColor3f(1, 0.7, 0);
			glVertex2d(x, y);
			glColor3f(1, 0, 0);
			glVertex2d(0, 0);
			glEnd();
			i = i + .5;
		}
	}


	if (sun_roatate > 360)
		sun_roatate = 0.0f;
	else
		sun_roatate = sun_roatate + 1.0f;

	//glEnd();
	Sleep(150);




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
