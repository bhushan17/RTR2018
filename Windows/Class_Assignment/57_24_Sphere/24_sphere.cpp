
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
float angleOfXrotation = 0.0f;
float angleOfYrotation = 0.0f;
float angleOfZrotation = 0.0f;

GLint keyPress = 0;

//light configuration parameter
GLfloat LightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
//GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f,0.0f,0.0f,1.0f };


GLfloat Light_Model_Ambient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat Light_Model_Local_Viewer[] = { 0.0f };



GLUquadric *quadric[24] = { NULL };
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
		case 'X':
		case 'x':
			keyPress = 1;
			angleOfXrotation = 0.0f;
			break;
		case 'Y':
		case 'y':
			keyPress = 2;
			angleOfYrotation = 0.0f;
			break;
		case 'Z':
		case 'z':
			keyPress = 3;
			angleOfZrotation = 0.0f;
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
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);



	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++) {
		quadric[i] = gluNewQuadric();
	}
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


	if (width <= height)
	{
		glOrtho(0.0f, //left
			15.5f, //right
			0.0f, //bottom
			(15.5f * ((GLfloat)height / (GLfloat)width)), //top
			-10.0f,//near
			10.0f//far
		);
	}
	else {
		glOrtho(0.0f,
			(15.5f * ((GLfloat)width / (GLfloat)height)),
			0.0f,
			15.5f,
			-10.0f,
			10.0f);
	}

}

void update()
{
	/*if (angleOfXrotation > 360)
	{
	    angleOfXrotation = 0.0f;
	}
	else {
	    angleOfXrotation = angleOfXrotation + 2.0f;
	}
	if (angleOfYrotation > 360)
	{
		angleOfYrotation = 0.0f;
	}
	else {
		angleOfYrotation = angleOfYrotation + 2.0f;
	}
	if (angleOfZrotation > 360)
	{		
		angleOfZrotation = 0.0f;
	}
	else {		
		angleOfZrotation = angleOfZrotation + 2.0f;
	}*/
	angleOfXrotation = angleOfXrotation + 2.0f;
	angleOfYrotation = angleOfYrotation + 2.0f;
	angleOfZrotation = angleOfZrotation + 2.0f;

}
void display()
{
	//code
	void draw24Sphere(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Cuboid

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPress == 1) {
		//angleOfYrotation = 0.0f;
		//angleOfZrotation = 0.0f;
		LightPosition[0] = 0.0f;
		LightPosition[1] = angleOfXrotation;
		LightPosition[2] = 0.0f;
		glRotatef(angleOfXrotation, 1.0f, 0.0f, 0.0f);
		//angleOfXrotation = angleOfXrotation + 2.0f;
	}
	else if (keyPress == 2) {
		//angleOfXrotation = 0.0f;
		//angleOfZrotation = 0.0f;
		LightPosition[0] = 0.0f;
		LightPosition[1] = 0.0f;
		LightPosition[2] = angleOfYrotation;
		glRotatef(angleOfYrotation, 0.0f, 1.0f, 0.0f);
		//angleOfYrotation = angleOfYrotation + 2.0f;
	}
	else if (keyPress == 3) {
		//angleOfYrotation = 0.0f;
		//angleOfXrotation = 0.0f;
		LightPosition[0] = angleOfZrotation;
		LightPosition[1] = 0.0f;
		LightPosition[2] = 0.0f;
		glRotatef(angleOfZrotation, 0.0f, 0.0f, 1.0f);
		//angleOfZrotation = angleOfZrotation + 2.0f;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	draw24Sphere();

	SwapBuffers(ghdc);
}

void draw24Sphere() {

	// Material Configuration
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShianyness[1];


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//************* 1st Sphere on 1st column, emerald **************//
	MaterialAmbient[0] = 0.0215;
	MaterialAmbient[1] = 0.1745; 
	MaterialAmbient[2] = 0.0215;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.07568;
	MaterialDiffuse[1] = 0.61424;
	MaterialDiffuse[2] = 0.07568;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT,GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.633;
	MaterialSpecular[1] = 0.727811;
	MaterialSpecular[2] = 0.633;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] =  0.6 * 128 ;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 14.0f, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);

	//************* 2nd Sphere on 1st column, jade **************//
	MaterialAmbient[0] = 0.135;
	MaterialAmbient[1] = 0.2225;
	MaterialAmbient[2] = 0.1575;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.54;
	MaterialDiffuse[1] = 0.89;
	MaterialDiffuse[2] = 0.63;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.316228;
	MaterialSpecular[1] = 0.316228;
	MaterialSpecular[2] = 0.316228;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 11.4f, 0.0f);
	gluSphere(quadric[1], 1.0f, 30, 30);


	//************* 3rd Sphere on 1st column, obsidian **************//
	MaterialAmbient[0] = 0.05375;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.06625;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.18275;
	MaterialDiffuse[1] = 0.17;
	MaterialDiffuse[2] = 0.22525;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.332741;
	MaterialSpecular[1] = 0.328634;
	MaterialSpecular[2] = 0.346435;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.3* 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 8.9f, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);


	//************* 4th Sphere on 1st column, pearl **************//
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.20725;
	MaterialAmbient[2] = 0.20725;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 1.0;
	MaterialDiffuse[1] = 0.829;
	MaterialDiffuse[2] = 0.829;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.296648;
	MaterialSpecular[1] = 0.296648;
	MaterialSpecular[2] = 0.296648;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.088 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 6.4f, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);


	//************* 5th Sphere on 1st column, ruby **************//
	MaterialAmbient[0] = 0.1745;
	MaterialAmbient[1] = 0.01175;
	MaterialAmbient[2] = 0.01175;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.61424;
	MaterialDiffuse[1] = 0.04136;
	MaterialDiffuse[2] = 0.04136;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.727811;
	MaterialSpecular[1] = 0.626959;
	MaterialSpecular[2] = 0.626959;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 3.9f, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);

	//************* 6th Sphere on 1st column, turquoise **************//
	MaterialAmbient[0] = 0.1;
	MaterialAmbient[1] = 0.18725;
	MaterialAmbient[2] = 0.1745;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.396;
	MaterialDiffuse[1] = 0.74151;
	MaterialDiffuse[2] = 0.69102;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.297254;
	MaterialSpecular[1] = 0.30829;
	MaterialSpecular[2] = 0.306678;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(4.0f, 1.4f, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);









	//************* 1st Sphere on 2nd column, brass **************//
	MaterialAmbient[0] = 0.329412;
	MaterialAmbient[1] = 0.223529;
	MaterialAmbient[2] = 0.027451;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.780392;
	MaterialDiffuse[1] = 0.568627;
	MaterialDiffuse[2] = 0.113725;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.992157;
	MaterialSpecular[1] = 0.941176;
	MaterialSpecular[2] = 0.807843;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.21794872 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 14.0f, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);

	//************* 2nd Sphere on 2nd column, bronze **************//
	MaterialAmbient[0] = 0.2125;
	MaterialAmbient[1] = 0.1275;
	MaterialAmbient[2] = 0.054;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.714;
	MaterialDiffuse[1] = 0.4284;
	MaterialDiffuse[2] = 0.18144;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.393548;
	MaterialSpecular[1] = 0.271906;
	MaterialSpecular[2] = 0.166721;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.2 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 11.4f, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);


	//************* 3rd Sphere on 2nd column, chrome **************//
	MaterialAmbient[0] = 0.25;
	MaterialAmbient[1] = 0.25;
	MaterialAmbient[2] = 0.25;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.774597;
	MaterialSpecular[1] = 0.774597;
	MaterialSpecular[2] = 0.774597;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 8.9f, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);


	//************* 4th Sphere on 2nd column, pearl **************//
	MaterialAmbient[0] = 0.19125;
	MaterialAmbient[1] = 0.0735;
	MaterialAmbient[2] = 0.0225;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.7038;
	MaterialDiffuse[1] = 0.27048;
	MaterialDiffuse[2] = 0.0828;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.256777;
	MaterialSpecular[1] = 0.137622;
	MaterialSpecular[2] = 0.086014;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 6.4f, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);


	//************* 5th Sphere on 2nd column, ruby **************//
	MaterialAmbient[0] = 0.24725;
	MaterialAmbient[1] = 0.1995;
	MaterialAmbient[2] = 0.0745;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.75164;
	MaterialDiffuse[1] = 0.60648;
	MaterialDiffuse[2] = 0.22648;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.628281;
	MaterialSpecular[1] = 0.555802;
	MaterialSpecular[2] = 0.366065;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 3.9f, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);

	//************* 6th Sphere on 2nd column, silver **************//
	MaterialAmbient[0] = 0.19225;
	MaterialAmbient[1] = 0.19225;
	MaterialAmbient[2] = 0.19225;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.50754;
	MaterialDiffuse[1] = 0.50754;
	MaterialDiffuse[2] = 0.50754;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.508273;
	MaterialSpecular[1] = 0.508273;
	MaterialSpecular[2] = 0.508273;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(10.0f, 1.4f, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);









	//************* 1st Sphere on 3nd column, black **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.5;
	MaterialSpecular[1] = 0.5;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 14.0f, 0.0f);
	gluSphere(quadric[12], 1.0f, 30, 30);

	//************* 2nd Sphere on 3nd column, cyan **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.1;
	MaterialAmbient[2] = 0.06;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.0;
	MaterialDiffuse[1] = 0.50980392;
	MaterialDiffuse[2] = 0.50980392;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.50196078;
	MaterialSpecular[1] = 0.50196078;
	MaterialSpecular[2] = 0.50196078;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 11.4f, 0.0f);
	gluSphere(quadric[13], 1.0f, 30, 30);


	//************* 3rd Sphere on 3nd column, green **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.1;
	MaterialDiffuse[1] = 0.35;
	MaterialDiffuse[2] = 0.1;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.45;
	MaterialSpecular[1] = 0.55;
	MaterialSpecular[2] = 0.45;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 8.9f, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);


	//************* 4th Sphere on 3nd column, red **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.0;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.6;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 6.4f, 0.0f);
	gluSphere(quadric[15], 1.0f, 30, 30);


	//************* 5th Sphere on 3nd column, white **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.55;
	MaterialDiffuse[1] = 0.55;
	MaterialDiffuse[2] = 0.55;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.70;
	MaterialSpecular[1] = 0.70;
	MaterialSpecular[2] = 0.70;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 3.9f, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);

	//************* 6th Sphere on 3nd column, plastic **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.0;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.6;
	MaterialSpecular[1] = 0.6;
	MaterialSpecular[2] = 0.5;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(16.0f, 1.4f, 0.0f);
	gluSphere(quadric[17], 1.0f, 30, 30);






	//************* 1st Sphere on 4th column, black **************//
	MaterialAmbient[0] = 0.02;
	MaterialAmbient[1] = 0.02;
	MaterialAmbient[2] = 0.02;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.01;
	MaterialDiffuse[1] = 0.01;
	MaterialDiffuse[2] = 0.01;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.4;
	MaterialSpecular[1] = 0.4;
	MaterialSpecular[2] = 0.4;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 14.0f, 0.0f);
	gluSphere(quadric[18], 1.0f, 30, 30);

	//************* 2nd Sphere on 4th column, cyan **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 11.4f, 0.0f);
	gluSphere(quadric[19], 1.0f, 30, 30);


	//************* 3rd Sphere on 4th column, green **************//
	MaterialAmbient[0] = 0.0;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.4;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.04;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 8.9f, 0.0f);
	gluSphere(quadric[20], 1.0f, 30, 30);


	//************* 4th Sphere on 4th column, red **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.0;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.4;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.04;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 6.4f, 0.0f);
	gluSphere(quadric[21], 1.0f, 30, 30);


	//************* 5th Sphere on 4th column, white **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.05;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.5;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.7;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 3.9f, 0.0f);
	gluSphere(quadric[22], 1.0f, 30, 30);

	//************* 6th Sphere on 4th column, plastic **************//
	MaterialAmbient[0] = 0.05;
	MaterialAmbient[1] = 0.05;
	MaterialAmbient[2] = 0.0;
	MaterialAmbient[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	MaterialDiffuse[0] = 0.5;
	MaterialDiffuse[1] = 0.5;
	MaterialDiffuse[2] = 0.4;
	MaterialDiffuse[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	MaterialSpecular[0] = 0.7;
	MaterialSpecular[1] = 0.7;
	MaterialSpecular[2] = 0.04;
	MaterialSpecular[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	MaterialShianyness[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShianyness);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(22.0f, 1.4f, 0.0f);
	gluSphere(quadric[23], 1.0f, 30, 30);








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
