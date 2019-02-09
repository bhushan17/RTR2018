#include<windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <GL/glut.h>

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")

//******* Macros **********//
#define WIN_WIDTH          800
#define WIN_HEIGHT         600

#define MAX_PARTICLES 1000
#define WCX		640
#define WCY		480
#define RAIN	0
#define SNOW	1
#define	HAIL	2

#define KEY_UP  0x0065
#define KEY_DOWN 0x0067
#define KEY_LEFT 0x0064
#define KEY_RIGHT 0x0066
#define KEY_PAGE_UP 0x0068 
#define KEY_PAGE_DOWN 0x0069

//**** Global variable declaration ************//
bool bFullScreen = false;
DWORD dwStyle;
HWND ghWnd = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;


float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall;

//floor colors
float r = 0.0;
float g = 1.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;


typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
	// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;


// Paticle System
particles par_sys[MAX_PARTICLES];


//********* Global function declaration ***********//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void normal_keys(unsigned char key);
void special_keys(int key);
void initParticles(int i);
void init();
void drawRain();
void drawHail();
void drawSnow();
void drawScene();


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

		case 'F':
		case 'f':
			ToggelFullScreen();
			break;
		}
		normal_keys(wparam);
		special_keys(wparam);
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

	init();
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;

	//fH = tan( (fovY / 2) / 180 * pi ) * zNear;
	fH = tan(fovY / 360 * pi) * zNear;
	fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}


void resize(int width, int height)
{
	if (height == 0) height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluPerspective(45, (float)width / (float)height, .1, 200);
	perspectiveGL(45, (float)width / (float)height, .1, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void display()
{
	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	drawScene();
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

void normal_keys(unsigned char key) {
	if (key == 'r') { // Rain
		fall = RAIN;
		//glutPostRedisplay();
		//display();
	}
	if (key == 'h') { // Hail
		fall = HAIL;
		//glutPostRedisplay();
		//display();
	}
	if (key == 's') { // Snow
		fall = SNOW;
		//glutPostRedisplay();
		//display();
	}
	if (key == '=') { //really '+' - make hail bigger
		hailsize += 0.01;
	}
	if (key == '-') { // make hail smaller
		if (hailsize > 0.1) hailsize -= 0.01;
	}
	if (key == ',') { // really '<' slow down
		if (slowdown > 4.0) slowdown += 0.01;
	}
	if (key == '.') { // really '>' speed up
		if (slowdown > 1.0) slowdown -= 0.01;
	}
	if (key == 'q') { // QUIT
		exit(0);
	}
}

void special_keys(int key) {
	if (key == 'a') {
		zoom += 1.0;
	}
	if (key == 'b') {
		zoom -= 1.0;
	}
	if (key == 'c') {
		pan += 1.0;
	}
	if (key == 'd') {
		pan -= 1.0;
	}
	if (key == 'e') {
		tilt -= 1.0;
	}
	if (key == 'z') {
		tilt += 1.0;
	}
}

// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand()%60)-25;
	par_sys[i].ypos = 15;//10.0;
	par_sys[i].zpos = (float)(rand() % 21) - 10;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}

void init() {
	int x, z;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	// Ground Verticies
	  // Ground Colors
	for (z = 0; z < 21; z++) {
		for (x = 0; x < 21; x++) {
			ground_points[x][z][0] = x - 10.0;
			ground_points[x][z][1] = accum;
			ground_points[x][z][2] = z - 10.0;

			ground_colors[z][x][0] = r; // red value
			ground_colors[z][x][1] = g; // green value
			ground_colors[z][x][2] = b; // blue value
			ground_colors[z][x][3] = 0.0; // acummulation factor
		}
	}

	// Initialize particles
	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}
}

// For Rain
void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

// For Hail
void drawHail() {
	float x, y, z;

	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.8, 0.8, 0.9);
			glBegin(GL_QUADS);
			// Front
			glVertex3f(x - hailsize, y - hailsize, z + hailsize); // lower left
			glVertex3f(x - hailsize, y + hailsize, z + hailsize); // upper left
			glVertex3f(x + hailsize, y + hailsize, z + hailsize); // upper right
			glVertex3f(x + hailsize, y - hailsize, z + hailsize); // lower left
			//Left
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			// Back
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			//Right
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			//Top
			glVertex3f(x - hailsize, y + hailsize, z + hailsize);
			glVertex3f(x - hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z - hailsize);
			glVertex3f(x + hailsize, y + hailsize, z + hailsize);
			//Bottom
			glVertex3f(x - hailsize, y - hailsize, z + hailsize);
			glVertex3f(x - hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z - hailsize);
			glVertex3f(x + hailsize, y - hailsize, z + hailsize);
			glEnd();

			// Update values
			//Move
			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].vel = par_sys[loop].vel * -1.0;
			}
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000); // * 1000
			par_sys[loop].vel += par_sys[loop].gravity;

			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

// For Snow
void drawSnow() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			//glutSolidSphere(0.2, 16, 16);
			glPopMatrix();

			// Update values
			//Move
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				int zi = z - zoom + 10;
				int xi = x + 10;
				ground_colors[zi][xi][0] = 1.0;
				ground_colors[zi][xi][2] = 1.0;
				ground_colors[zi][xi][3] += 1.0;
				if (ground_colors[zi][xi][3] > 1.0) {
					ground_points[xi][zi][1] += 0.1;
				}
				par_sys[loop].life = -1.0;
			}

			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

// Draw Particles
void drawScene() {
	int i, j;
	float x, y, z;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();


	glRotatef(pan, 0.0, 1.0, 0.0);
	glRotatef(tilt, 1.0, 0.0, 0.0);

	// GROUND?!
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	// along z - y const
	for (i = -10; i + 1 < 11; i++) {
		// along x - y const
		for (j = -10; j + 1 < 11; j++) {
			glColor3fv(ground_colors[i + 10][j + 10]);
			glVertex3f(ground_points[j + 10][i + 10][0],
				ground_points[j + 10][i + 10][1],
				ground_points[j + 10][i + 10][2] + zoom);
			glColor3fv(ground_colors[i + 10][j + 1 + 10]);
			glVertex3f(ground_points[j + 1 + 10][i + 10][0],
				ground_points[j + 1 + 10][i + 10][1],
				ground_points[j + 1 + 10][i + 10][2] + zoom);
			glColor3fv(ground_colors[i + 1 + 10][j + 1 + 10]);
			glVertex3f(ground_points[j + 1 + 10][i + 1 + 10][0],
				ground_points[j + 1 + 10][i + 1 + 10][1],
				ground_points[j + 1 + 10][i + 1 + 10][2] + zoom);
			glColor3fv(ground_colors[i + 1 + 10][j + 10]);
			glVertex3f(ground_points[j + 10][i + 1 + 10][0],
				ground_points[j + 10][i + 1 + 10][1],
				ground_points[j + 10][i + 1 + 10][2] + zoom);
		}

	}
	glEnd();
	// Which Particles
	if (fall == RAIN) {
		drawRain();
	}
	else if (fall == HAIL) {
		drawHail();
	}
	else if (fall == SNOW) {
		drawSnow();
	}

	//SwapBuffers(ghdc);
}









