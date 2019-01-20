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
			/*if (bFullScreen == false)
			{
				DEVMODE DisplayMode;
				memset(&DisplayMode, 0, sizeof(DEVMODE));
				DisplayMode.dmSize = sizeof(DEVMODE);

				EnumDisplaySettings(NULL, 0, &DisplayMode);
				ChangeDisplaySettings((DEVMODE*)&DisplayMode, CDS_FULLSCREEN);
				bFullScreen = true;
			}
			else {
				ChangeDisplaySettings(NULL, 0);
				bFullScreen = false;
			}*/
			
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

	case WM_DISPLAYCHANGE:
		MessageBox(hwnd, TEXT("This is WM_DISPLAYCHANGE"), TEXT("Bhushan msg"), MB_OK);
		/*MONITORINFO mI;
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
		break;*/

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggelFullScreen(void)
{
	MONITORINFO mI;
	DEVMODE DisplayMode;
	int ret_val;
	memset(&DisplayMode, 0, sizeof(DEVMODE));
	DisplayMode.dmSize = sizeof(DEVMODE);
	TCHAR str[255];
	DISPLAY_DEVICE device;
	device.cb = sizeof(DISPLAY_DEVICE);
	device.StateFlags = DISPLAY_DEVICE_PRIMARY_DEVICE;
	EnumDisplayDevices(NULL, 0, &device, EDD_GET_DEVICE_INTERFACE_NAME);
	HWND hwndDekstop;
	RECT rc;
	HDC hdc;
	
	
	if (bFullScreen == false)
	{
		hwndDekstop = GetDesktopWindow();
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		//DisplayMode.dmPosition.x = (LONG )rc.left;
		


		//DisplayMode.dmPosition.y =(LONG ) rc.top;
		DisplayMode.dmPelsWidth = (DWORD)(rc.right - rc.left);
		DisplayMode.dmPelsHeight = (DWORD)(rc.bottom - rc.top);
		//DisplayMode.dmBitsPerPel =(DWORD) 32;
		//DisplayMode.dmDisplayFlags = DM_DISPLAYFLAGS;
		//DisplayMode.dmDisplayFrequency = DM_DISPLAYFREQUENCY;

		
		//DisplayMode.DUMMYUNIONNAME.dmPosition.x = (LONG)rc.left;
		//DisplayMode.DUMMYUNIONNAME.dmPosition.y = (LONG)rc.top;
		//DisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_POSITION;
		//EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &DisplayMode);
		ret_val=ChangeDisplaySettings( (DEVMODE*)&DisplayMode, CDS_FULLSCREEN);
		UpdateWindow(ghWnd);
		switch (ret_val)
		{
		case DISP_CHANGE_SUCCESSFUL:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_SUCCESSFUL."));
			break;

		case DISP_CHANGE_BADDUALVIEW:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_BADDUALVIEW."));
			break;
		case DISP_CHANGE_BADFLAGS:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_BADFLAGS."));
			break;
		case DISP_CHANGE_BADMODE:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_BADMODE."));
			break;
		case DISP_CHANGE_BADPARAM:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_BADPARAM."));
			break;
		case DISP_CHANGE_FAILED:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_FAILED."));
			break;
		case DISP_CHANGE_NOTUPDATED:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_NOTUPDATED."));
			break;
		case DISP_CHANGE_RESTART:
			wsprintf(str, TEXT("Ret is DISP_CHANGE_RESTART."));
			break;
		}

		MessageBox(ghWnd,str, TEXT("Bhushan msg"), MB_OK);

		//dwStyle = GetWindowLong(ghWnd, GWL_STYLE);
		//mI = { sizeof(MONITORINFO) };
		//if (dwStyle & WS_OVERLAPPEDWINDOW)
		//{
			//if (GetWindowPlacement(ghWnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &mI))
			//{
				//hwndDekstop = GetDesktopWindow();
				//GetWindowRect(hwndDekstop, &rc);
				//hdc = GetDC(hwndDekstop);
				
				//DisplayMode.dmPosition.x = rc.left;
			    
				
				//DisplayMode.dmPosition.y = rc.top;
				//wsprintf(str, TEXT("dmPelsWidth = %d", DisplayMode.dmPelsWidth));
				//MessageBox(ghWnd, str, TEXT("Bhushan msg"), MB_OK);
				
				//wsprintf(str, TEXT("dmPelsHeight = %d", DisplayMode.dmPelsHeight));
				//MessageBox(ghWnd, str, TEXT("Bhushan msg"), MB_OK);

				//DisplayMode.dmPelsWidth = GetDeviceCaps(hdc, HORZRES);
				//DisplayMode.dmPelsHeight = GetDeviceCaps(hdc, VERTRES);
				//DisplayMode.dmBitsPerPel = Bits;
				//DisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				//ChangeDisplaySettingsEx(NULL, (DEVMODE*)&DisplayMode,ghWnd, CDS_FULLSCREEN,NULL);
				//wsprintf(str, TEXT("dmPelsWidth = %d", DisplayMode.dmPelsWidth));
				//MessageBox(ghWnd, str, TEXT("Bhushan msg"), MB_OK);

				//wsprintf(str, TEXT("dmPelsHeight = %d", DisplayMode.dmPelsHeight));
				//MessageBox(ghWnd, str, TEXT("Bhushan msg"), MB_OK);
				/*SetWindowLong(ghWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghWnd,
					HWND_TOP,
					mI.rcMonitor.left,
					mI.rcMonitor.top,
					mI.rcMonitor.right - mI.rcMonitor.left,
					mI.rcMonitor.bottom - mI.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
					*/
			//}
		//}
		ShowCursor(FALSE);
		bFullScreen = true;
	}
	else
	{
		mI = { sizeof(MONITORINFO) };
		
		memset(&DisplayMode, 0, sizeof(DisplayMode));
		GetMonitorInfo(MonitorFromWindow(ghWnd, MONITORINFOF_PRIMARY), &mI);
		int iCenteringWindowX = ((mI.rcMonitor.right - mI.rcMonitor.left) / 2) - ((wpPrev.rcNormalPosition.right - wpPrev.rcNormalPosition.left) / 2);
		int iCenteringWindowY = ((mI.rcMonitor.bottom - mI.rcMonitor.top) / 2) - ((wpPrev.rcNormalPosition.bottom - wpPrev.rcNormalPosition.top) / 2);
		
		DisplayMode.dmSize = sizeof(DisplayMode);
		DisplayMode.dmPosition.x = iCenteringWindowX;
		DisplayMode.dmPosition.y = iCenteringWindowY;
		DisplayMode.dmPelsWidth = wpPrev.rcNormalPosition.right - wpPrev.rcNormalPosition.left;
		DisplayMode.dmPelsHeight = wpPrev.rcNormalPosition.bottom - wpPrev.rcNormalPosition.top;

		ChangeDisplaySettings((DEVMODE*)&DisplayMode, CDS_UPDATEREGISTRY);
		/*SetWindowLong(ghWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghWnd, &wpPrev);
		SetWindowPos(ghWnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		*/
		ShowCursor(TRUE);
		bFullScreen = false;
	}
}