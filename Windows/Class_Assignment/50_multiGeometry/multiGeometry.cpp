#include<windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>

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
int status = NULL;

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

	// ****** Register above class *************//
	fopen_s(&gpFile, "Log.txt", "a+");
	fprintf(gpFile, "Register class start !!!\n");
	fclose(gpFile);

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
	ShowWindow(hwnd, iCmdShow);
	//UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);



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
	static int width, hight;
	//code

	switch (iMsg)
	{
	case WM_CREATE:
		//MessageBox(hwnd, TEXT("This is WM_CREATE."), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_CHAR:
		switch (wparam)
		{
		case VK_ESCAPE:
			//MessageBox(hwnd, TEXT("This is VK_ESCAPE"), TEXT("Bhushan msg"), MB_OK);
			DestroyWindow(hwnd);
			break;

		case 'f':
		case 'F':
			ToggelFullScreen();
			break;

		case '0':
			status = 0;
			glViewport(0, 0, (GLsizei)width, (GLsizei)hight);
			break;

		case '1':
			status = 1;
			glViewport(0, 0, (GLsizei)width / 2, (GLsizei)hight / 2);

			break;

		case '2':
			status = 2;
			glViewport(width / 2, 0, (GLsizei)width / 2, (GLsizei)hight / 2);
			break;

		case '3':
			status = 3;
			glViewport(0, hight / 2, (GLsizei)width / 2, (GLsizei)hight / 2);
			break;

		case '4':
			status = 4;
			glViewport(width / 2, hight / 2, (GLsizei)width / 2, (GLsizei)hight / 2);
			break;

		case '5':
			status = 5;
			glViewport(0, 0, (GLsizei)width / 2, (GLsizei)hight);
			break;

		case '6':
			status = 6;
			glViewport(width / 2, 0, (GLsizei)width / 2, (GLsizei)hight);
			break;

		case '7':
			status = 7;
			glViewport(0, hight / 2, (GLsizei)width, (GLsizei)hight / 2);
			break;

		case '8':
			status = 8;
			glViewport(0, 0, (GLsizei)width, (GLsizei)hight / 2);
			break;

		case '9':
			status = 9;
			glViewport(width / 4, hight / 4, (GLsizei)width / 2, (GLsizei)hight / 2);
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
		width = LOWORD(lParam);
		hight = HIWORD(lParam);
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
int gwidth, gheight;
void resize(int width, int height)
{
	gwidth = width;
	gheight = height;
	if (status == 0)
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	else if (status == 1)
		glViewport(0, 0, (GLsizei)width / 2, (GLsizei)height / 2);
	else if (status == 2)
		glViewport(width / 2, 0, (GLsizei)width / 2, (GLsizei)height / 2);
	else if (status == 3)
		glViewport(0, height / 2, (GLsizei)width / 2, (GLsizei)height / 2);
	else if (status == 4)
		glViewport(width / 2, height / 2, (GLsizei)width / 2, (GLsizei)height / 2);
	else if (status == 5)
		glViewport(0, 0, (GLsizei)width / 2, (GLsizei)height);
	else if (status == 6)
		glViewport(width / 2, 0, (GLsizei)width / 2, (GLsizei)height);
	else if (status == 7)
		glViewport(0, height / 2, (GLsizei)width, (GLsizei)height / 2);
	else if (status == 8)
		glViewport(0, 0, (GLsizei)width, (GLsizei)height / 2);
	else if (status == 9)
		glViewport(width / 4, height / 4, (GLsizei)width / 2, (GLsizei)height / 2);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display()
{

	//code
	glClear(GL_COLOR_BUFFER_BIT);


	glViewport(0, gheight / 2, (GLsizei)gwidth / 3, (GLsizei)gheight / 2);
	//glViewport(0, 0, (GLsizei)gwidth / 2, (GLsizei)gheight / 2);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);
	glPointSize(5);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);

	
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(0.34f, 1.0f);
	glVertex2f(1.0f, 1.0f);

	glVertex2f(-1.0f, 0.33f);
	glVertex2f(-0.33f, 0.33f);
	glVertex2f(0.34f, 0.33f);
	glVertex2f(1.0f, 0.33f);

	glVertex2f(-1.0f, -0.34f);
	glVertex2f(-0.33f, -0.34f);
	glVertex2f(0.34f, -0.34f);
	glVertex2f(1.0f, -0.34f);


	glVertex2f(-1.0f, -1.0f);
	glVertex2f(-0.33f, -1.0f);
	glVertex2f(0.34f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();



	
	glViewport(gwidth / 3, gheight / 2, (GLsizei)gwidth / 3, (GLsizei)gheight / 2);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-1.0f, 0.33f);
	glVertex2f(1.0f, 0.33f);
	glVertex2f(-0.33f, -1.00f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(0.34f, 1.0f);
	glVertex2f(-1.0f, -0.34f);
	glVertex2f(1.0f, -0.34f);
	glVertex2f(0.34f, -1.00f);
	glEnd();

	
	glViewport(gwidth * 1.7 / 3, gheight / 2, (GLsizei)gwidth * 1.7 / 3, (GLsizei)gheight / 2);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();


	glBegin(GL_LINES);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-0.33f, -1.0f);

	glVertex2f(0.34f, 1.0f);
	glVertex2f(0.34f, -1.0f);

	glVertex2f(-1.0f, 0.33f);
	glVertex2f(1.0f, 0.33f);

	glVertex2f(-1.00f, -0.34f);
	glVertex2f(1.0f, -0.34f);
	glEnd();



	
	glViewport(0, 0, (GLsizei)gwidth / 3, (GLsizei)gheight / 2);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-1.0f, 0.33f);
	glVertex2f(1.0f, 0.33f);
	glVertex2f(-0.33f, -1.00f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(0.34f, 1.0f);
	glVertex2f(-1.0f, -0.34f);
	glVertex2f(1.0f, -0.34f);
	glVertex2f(0.34f, -1.00f);
	glEnd();
	

	glViewport(gwidth / 3, 0, (GLsizei)gwidth / 3, (GLsizei)gheight / 2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(1.0f, 0.33f);

	glVertex2f(-1.0f, 1.0f);
	glVertex2f(1.0f, -0.34f);

	glVertex2f(-1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glVertex2f(-1.0f, 1.0f);
	glVertex2f(0.34f, -1.0f);

	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-0.33f, -1.0f);
	glEnd();

	glViewport(gwidth * 1.7 / 3, 0, (GLsizei)gwidth * 1.7 / 3, (GLsizei)gheight / 2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(-0.33f, -1.0f);
	glEnd();


	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);

	glVertex2f(0.34f, 1.0f);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-0.33f, -1.0f);
	glVertex2f(0.34f, -1.0f);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);

	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.34f, 1.0f);
	glVertex2f(0.34f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();



	glViewport(gwidth * 1.7 / 3, 0, (GLsizei)gwidth * 1.7 / 3, (GLsizei)gheight / 2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.0f);

	
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();


	glBegin(GL_LINES);
	glVertex2f(-0.33f, 1.0f);
	glVertex2f(-0.33f, -1.0f);

	glVertex2f(0.34f, 1.0f);
	glVertex2f(0.34f, -1.0f);

	glVertex2f(-1.0f, 0.33f);
	glVertex2f(1.0f, 0.33f);

	glVertex2f(-1.00f, -0.34f);
	glVertex2f(1.0f, -0.34f);
	glEnd();


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
