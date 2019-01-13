//Header
#include<Windows.h>

//global variable declarartion 
bool bFullScreen = false;
HWND ghWnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
//global function declaration 
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//varible declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("BhushanApp");

	//code
	//initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
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

	//Register above class
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindow(szAppName,
		TEXT("Bhushan's Window"),
		WS_OVERLAPPED,
		100,
		100,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghWnd = hwnd;
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	//message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//declaration of variable
	HDC hdc;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");
	PAINTSTRUCT ps;
	//declaration of function
	void ToggelFullScreen(void);
	//code
	switch (iMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd, TEXT("This is WM_CREATE."), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			MessageBox(hwnd, TEXT("This is VK_ESCAPE"), TEXT("Bhushan msg"), MB_OK);
			DestroyWindow(hwnd);
			break;

		case 0x46:
			ToggelFullScreen();
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("This is WM_LBUTTONDOWN"), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_RBUTTONDOWN:
		MessageBox(hwnd, TEXT("This is WM_RBUTTONDOWN"), TEXT("Bhushan msg"), MB_OK);
		break;

	case WM_DESTROY:
		MessageBox(hwnd, TEXT("This is WM_DESTROY"), TEXT("Bhushan msg"), MB_OK);
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		GetClientRect(hwnd, &rc);
		hdc = BeginPaint(hwnd, &ps);
		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 255, 0));
		DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;


	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
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