
#include<windows.h>
#include<stdio.h> // for file I/O

#include<d3d11.h>
#include<d3dcompiler.h>

//to suppress the warning "C4838: conversion from 'unsign int' to 'INT'
//requires a narrowing conversion"
//due to '.inl' files include in xnamath.h file
#pragma warning(disable: 4838)
#include"XNAMath\xnamath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global variable declarartion

FILE *gpFile_BRK = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd_BRK = NULL;

DWORD dwStyle_BRK;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow_BRK = false;
bool gbEscapeKeyIsPressed_BRK = false;
bool gbFullscreen_BRK = false;

float gClearColor_BRK[4]; // RGBA
IDXGISwapChain *gpIDXGISwapChain_BRK = NULL;
ID3D11Device *gpID3D11Device_BRK = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext_BRK = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView_BRK = NULL;


ID3D11VertexShader *gpID3D11VertexShader_BRK = NULL;
ID3D11HullShader *gpID3D11HullShader_BRK = NULL;
ID3D11DomainShader *gpID3D11DomainShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader_BRK = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_BRK = NULL;
ID3D11InputLayout *gpID3D11InputLayout_BRK = NULL;

ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_HullShader_BRK = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_DomainShader_BRK = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_PixelShader_BRK = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;


struct CBUFFER_HULL_SHADER
{
	XMVECTOR Hull_Constant_Function_params;
};


struct CBUFFER_DOMAIN_SHADER
{
	XMMATRIX WorldViewProjectionMatrix;
};

struct CBUFFER_PIXEL_SHADER
{
	XMVECTOR LineColor;
};

unsigned int guiNumberOfLineSegments = 1;

XMMATRIX gPerspectiveProjectionMatrix;

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declaration
	HRESULT initialize(void);
	void unintialize(void);
	void display(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("Direct3D11");
	bool bDone = false;

	//code
	// create log file
	if (fopen_s(&gpFile_BRK, gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not be Ceated \n Exitting ..."), TEXT("ERROR"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile_BRK, "Log File Is Successfully Opened. \n");
		fclose(gpFile_BRK);
	}

	//intialize WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register WNDCLASSEX structure
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindow(szClassName,
		TEXT("Direct3D11 Bhushan's Window "),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd_BRK = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//intialize D3D
	HRESULT hr = S_OK;
	hr = initialize();

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "intialize() Failed. Exitting Now ... \n");
		fclose(gpFile_BRK);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "intialize() Succeeded.\n");
		fclose(gpFile_BRK);
	}

	//message loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//render
			display();
			if (gbActiveWindow_BRK == true)
			{
				if (gbEscapeKeyIsPressed_BRK == true)
					bDone = true;
			}
		}
	}

	//cleanUp
	unintialize();

	return((int)msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	HRESULT resize(int, int);
	void ToggleFullscreen(void);
	void unintialize(void);

	//variable declarartion
	HRESULT hr;

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) //if 0, window is active
			gbActiveWindow_BRK = true;
		else  //if non-zero , window is not active
			gbActiveWindow_BRK = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		if (gpID3D11DeviceContext_BRK)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile_BRK, gszLogFileName, "a+");
				fprintf_s(gpFile_BRK, "resize() Failed. \n");
				fclose(gpFile_BRK);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile_BRK, gszLogFileName, "a+");
				fprintf_s(gpFile_BRK, "resize() Succeeded. \n");
				fclose(gpFile_BRK);
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: // case 27
			if (gbEscapeKeyIsPressed_BRK == false)
				gbEscapeKeyIsPressed_BRK = true;
			break;
		case 0x46: //F 0r f
			if (gbFullscreen_BRK == false)
			{
				ToggleFullscreen();
				gbFullscreen_BRK = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen_BRK = false;
			}
			break;
		case VK_UP:
			guiNumberOfLineSegments++;
			if(guiNumberOfLineSegments >= 50)
				guiNumberOfLineSegments = 50;
			break;
		case VK_DOWN:
			guiNumberOfLineSegments--;
			if(guiNumberOfLineSegments <= 0)
				guiNumberOfLineSegments = 1;
			break;
		default:
			break;

		}
		break;
	case WM_NCLBUTTONDOWN:
		break;
	case WM_CLOSE:
		unintialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	MONITORINFO mI;
	if (gbFullscreen_BRK == false)
	{
		dwStyle_BRK = GetWindowLong(ghwnd_BRK, GWL_STYLE);
		mI = { sizeof(MONITORINFO) };
		if (dwStyle_BRK & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd_BRK, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd_BRK, MONITORINFOF_PRIMARY), &mI))
			{
				SetWindowLong(ghwnd_BRK, GWL_STYLE, dwStyle_BRK & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd_BRK,
					HWND_TOP,
					mI.rcMonitor.left,
					mI.rcMonitor.top,
					mI.rcMonitor.right - mI.rcMonitor.left,
					mI.rcMonitor.bottom - mI.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen_BRK = true;
	}
	else
	{
		SetWindowLong(ghwnd_BRK, GWL_STYLE, dwStyle_BRK | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd_BRK, &wpPrev);
		SetWindowPos(ghwnd_BRK,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullscreen_BRK = false;
	}
}

HRESULT initialize(void)
{
	//function declarations
	void uninitialize(void);
	HRESULT resize(int, int);

	//variable declarations
	HRESULT hr = S_OK;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0; //default , lowest

	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1; //based upon d3dFeatureLevel_required

	//code
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]); // calculation size of array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd_BRK;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,  //Adapter
			d3dDriverType, //Driver Type
			NULL,  // SoftWare
			createDeviceFlags, //Flags
			&d3dFeatureLevel_required,  //Feature Levels
			numFeatureLevels,  //Num Feaure Levels
			D3D11_SDK_VERSION,  // SDK_VERSION
			&dxgiSwapChainDesc,  // Swap chain Desc
			&gpIDXGISwapChain_BRK,   //swap chain
			&gpID3D11Device_BRK,  // Device
			&d3dFeatureLevel_acquired,  // Feature Level
			&gpID3D11DeviceContext_BRK);   // Device Context

		if (SUCCEEDED(hr))
		{
			break;
		}


	}

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3D11CreateDeviceAndSwapChain() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3D11CreateDeviceAndSwapChain() Succeeded. \n");
		fprintf_s(gpFile_BRK, "The Chosen Driver Is of. \n");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile_BRK, "HArdware Type. \n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile_BRK, "WARP Type. \n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile_BRK, "Reference Type. \n");
		}
		else
		{
			fprintf_s(gpFile_BRK, "Unknow Type. \n");
		}

		fprintf_s(gpFile_BRK, "The Supported Heighest Feature Level Is");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile_BRK, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile_BRK, "10.1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile_BRK, "10.0\n");
		}
		else
		{
			fprintf_s(gpFile_BRK, "Unknown.\n");
		}

		fclose(gpFile_BRK);
	}

	//intialize shader ,input layout, constant buffer etc.
	const char *vertexShaderSourceCode =
		"struct vertex_output" \
		"{" \
		"float4 position : POSITION;" \
		"};" \
		"vertex_output main(float2 pos : POSITION)" \
		"{" \
		"vertex_output output;" \
		"output.position = float4(pos,0.0f,1.0f);" \
		"return(output);" \
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile_BRK, gszLogFileName, "a+");
			fprintf_s(gpFile_BRK, "D3DCompile() Failed For Vertex Shader : %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile_BRK);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3DCompile() Succeeded For Vertex Shader. \n");
		fclose(gpFile_BRK);
	}

	hr = gpID3D11Device_BRK->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateVertexShader() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateVertexShader() Succeeded.  \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->VSSetShader(gpID3D11VertexShader_BRK, 0, 0);



	//Hull Shader
	const char* hullShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4 hull_constant_function_params;" \
		"}" \
		"struct vertex_output" \
		"{"
		"float4 position : POSITION;" \
		"};" \
		"struct hull_constant_output" \
		"{" \
		"float edges[2] : SV_TESSFACTOR;" \
		"};" \
		"hull_constant_output hull_constant_function(void)" \
		"{" \
		"hull_constant_output output;" \
		"float numberOfStrips = hull_constant_function_params[0];" \
		"float numberOfSegments = hull_constant_function_params[1];" \
		"output.edges[0] = numberOfStrips;" \
		"output.edges[1] = numberOfSegments;" \
		"return(output);" \
		"}" \
		"struct hull_output" \
		"{" \
		"float4 position : POSITION;" \
		"};" \
		"[domain(\"isoline\")]" \
		"[partitioning(\"integer\")]" \
		"[outputtopology(\"line\")]" \
		"[outputcontrolpoints(4)]" \
		"[patchconstantfunc(\"hull_constant_function\")]" \
		"hull_output main(InputPatch<vertex_output,4>input_patch,uint i:SV_OUTPUTCONTROLPOINTID)" \
		"{" \
		"hull_output output;" \
		"output.position = input_patch[i].position;" \
		"return(output);" \
		"}";
		
	ID3DBlob *pID3DBlob_HullShaderCode = NULL;
	pID3DBlob_Error = NULL;
	hr = D3DCompile(hullShaderSourceCode,
		 lstrlenA(hullShaderSourceCode) + 1,
		 "HS",
		 NULL,
		 D3D_COMPILE_STANDARD_FILE_INCLUDE,
		 "main",
		 "hs_5_0",
		 0,
		 0,
		 &pID3DBlob_HullShaderCode,
		 &pID3DBlob_Error);
		 
	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile_BRK, gszLogFileName, "a+");
			fprintf_s(gpFile_BRK, "D3DCompile() Failed For HULL Shader : %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile_BRK);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3DCompile() Succeeded For HULL Shader. \n");
		fclose(gpFile_BRK);
	}

	hr = gpID3D11Device_BRK->CreateHullShader(pID3DBlob_HullShaderCode->GetBufferPointer(), pID3DBlob_HullShaderCode->GetBufferSize(), NULL, &gpID3D11HullShader_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateHullShader() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateHullShader() Succeeded.  \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->HSSetShader(gpID3D11HullShader_BRK, 0, 0);
	pID3DBlob_HullShaderCode->Release();
	pID3DBlob_HullShaderCode = NULL;
	
	
	//Domain Shader
	const char *domainShaderSourceCode = 
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"struct hull_constant_output" \
		"{" \
		"float edges[2] : SV_TESSFACTOR;" \
		"};" \
		"struct hull_output" \
		"{" \
		"float4 position : POSITION;" \
		"};" \
		"struct domain_output" \
		"{" \
		"float4 position : SV_POSITION;" \
		"};" \
		"[domain(\"isoline\")]" \
		"domain_output main(hull_constant_output input, OutputPatch<hull_output,4>output_patch, float2 tessCoord : SV_DOMAINLOCATION)" \
		"{" \
		"domain_output output;" \
		"float u = tessCoord.x;" \
		"float3 p0 = output_patch[0].position.xyz;" \
		"float3 p1 = output_patch[1].position.xyz;" \
		"float3 p2 = output_patch[2].position.xyz;" \
		"float3 p3 = output_patch[3].position.xyz;" \
		"float u1 = (1.0f - u);" \
		"float u2 = u * u;" \
		"float b3 = u2 * u;" \
		"float b2 = 3.0f * u2 * u1;" \
		"float b1 = 3.0f * u * u1 * u1 ;" \
		"float b0 = u1 * u1 * u1;" \
		"float3 p = p0*b0 + p1*b1 + p2*b2 + p3*b3;" \
		"output.position = mul(worldViewProjectionMatrix, float4(p, 1.0f));" \
		"return(output);" \
		"}";
		
		
		ID3DBlob* pID3DBlob_DomainShaderCode = NULL;
		pID3DBlob_Error = NULL;


		hr = D3DCompile(domainShaderSourceCode,
			lstrlenA(domainShaderSourceCode) + 1,
			"DS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ds_5_0",
			0,
			0,
			&pID3DBlob_DomainShaderCode,
			&pID3DBlob_Error);

		
						
	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile_BRK, gszLogFileName, "a+");
			fprintf_s(gpFile_BRK, "D3DCompile() Failed For Domain Shader : %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile_BRK);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3DCompile() Succeeded For Domain Shader. \n");
		fclose(gpFile_BRK);
	}

	hr = gpID3D11Device_BRK->CreateDomainShader(pID3DBlob_DomainShaderCode->GetBufferPointer(), pID3DBlob_DomainShaderCode->GetBufferSize(), NULL, &gpID3D11DomainShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateDomainShader() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateDomainShader() Succeeded.  \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->DSSetShader(gpID3D11DomainShader, 0, 0);
	pID3DBlob_DomainShaderCode->Release();
	pID3DBlob_DomainShaderCode = NULL;
			
		 
		
	//Pixel Shader
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4 lineColor;" \
		"}" \
		"float4 main(void) : SV_TARGET" \
		"{" \
		"float4 color;"
		"color = lineColor;" \
		"return(color);" \
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile_BRK, gszLogFileName, "a+");
			fprintf_s(gpFile_BRK, "D3DCompile() Failed For Pixel Shader : %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile_BRK);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "D3DCompile() Succeeded For Pixel Shader. \n");
		fclose(gpFile_BRK);
	}


	hr = gpID3D11Device_BRK->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(), pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreatePixelShader() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreatePixelShader() Succeeded.  \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->PSSetShader(gpID3D11PixelShader_BRK, 0, 0);
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	float vertices[]={-1.0f,-1.0f,-0.5f,1.0f,0.5f,-1.0f,1.0f,1.0f};

	//create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Vertex Buffer. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBufferShader() Succeeded For Vertex Buffer.  \n");
		fclose(gpFile_BRK);
	}

	//copy vertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext_BRK->Map(gpID3D11Buffer_VertexBuffer_BRK, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	gpID3D11DeviceContext_BRK->Unmap(gpID3D11Buffer_VertexBuffer_BRK, NULL);

	//create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc;
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc.InputSlot = 0;
	inputElementDesc.AlignedByteOffset = 0;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;

	hr = gpID3D11Device_BRK->CreateInputLayout(&inputElementDesc, 1, pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateInputLayout() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateInputLayout() Succeeded. \n");
		fclose(gpFile_BRK);
	}


	gpID3D11DeviceContext_BRK->IASetInputLayout(gpID3D11InputLayout_BRK);
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	//define and set the constant buffer for hull shader
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_HULL_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer_HullShader_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Constant Buffer for Hull Shader. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Succeeded for Constant Buffer for Hull Shader. \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->HSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer_HullShader_BRK);
	
	
	
	//define and set the constant buffer for domain shader
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth =  sizeof(CBUFFER_DOMAIN_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	
	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer_DomainShader_BRK);
	
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Constant Buffer for Domain Shader. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Succeeded for Constant Buffer for Domain Shader. \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->DSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer_DomainShader_BRK);
	
	
	
	//define and set the constant buffer for pixel shader
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth =  sizeof(CBUFFER_PIXEL_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	
	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer_PixelShader_BRK);
	
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Constant Buffer for Pixel Shader. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Succeeded for Constant Buffer for Pixel Shader. \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->PSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer_PixelShader_BRK);
	
	
	
	
	
	//set rasterization state
	//In D3D, backface culling is by default 'ON'
	//Means backface of geometry will not be visible
	//this causes our 2D triangles backface to vanish on rotation.
	//so set culling 'OFF'
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void*)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	
	hr = gpID3D11Device_BRK->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	
	if(FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRasterizerState() Failed for culling.\n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRasterizerState() Succeeded for culling.\n");
		fclose(gpFile_BRK);
	}
	
	gpID3D11DeviceContext_BRK->RSSetState(gpID3D11RasterizerState);
	
	
	
	//d3d clear color
	//blue
	gClearColor_BRK[0] = 0.0f;
	gClearColor_BRK[1] = 0.0f;
	gClearColor_BRK[2] = 0.0f;
	gClearColor_BRK[3] = 1.0f;


	//Set projection matrix to identity matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	//call resize for first time
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "resize() Failed. \n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "resize() succeeded. \n");
		fclose(gpFile_BRK);

	}

	return(S_OK);
}

HRESULT resize(int width, int height)
{
	//code
	HRESULT hr = S_OK;
	if(gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}
	
	//free any size-dependant resources
	if (gpID3D11RenderTargetView_BRK)
	{
		gpID3D11RenderTargetView_BRK->Release();
		gpID3D11RenderTargetView_BRK = NULL;
	}

	//resize swap chain buffers accordingly
	gpIDXGISwapChain_BRK->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//again get back buffer from swap chain
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain_BRK->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	//again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device_BRK->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRenderTargetView()  Failed. \n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRenderTargetView()  Succeeded. \n");
		fclose(gpFile_BRK);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;


	//create depth stencil buffer (or zbuffer)
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	gpID3D11Device_BRK->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);
	
	//create depth stencil view from above depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = gpID3D11Device_BRK->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc
		, &gpID3D11DepthStencilView);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateDepthStencilView()  Failed. \n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateDepthStencilView()  Succeeded. \n");
		fclose(gpFile_BRK);
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;


	//Set render target view as render target
	gpID3D11DeviceContext_BRK->OMSetRenderTargets(1, &gpID3D11RenderTargetView_BRK, NULL);


	//Set ViewPort
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext_BRK->RSSetViewports(1, &d3dViewPort);


	//set Perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	

	return(hr);
}

void display(void)
{
	//code
	//clear render target view to a chosen color
	gpID3D11DeviceContext_BRK->ClearRenderTargetView(gpID3D11RenderTargetView_BRK, gClearColor_BRK);

	gpID3D11DeviceContext_BRK->ClearDepthStencilView(gpID3D11DepthStencilView,D3D11_CLEAR_DEPTH, 1.0f, 0);

	//select which vertex buffer to display
	UINT stride = sizeof(float) * 2;
	UINT offset = 0;

	gpID3D11DeviceContext_BRK->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_BRK, &stride, &offset);

	//select geometry primtive
	gpID3D11DeviceContext_BRK->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//transition is concerned with world matrix transformation
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	//final WorldViewProjection matrix
	XMMATRIX wvpMatrix = translationMatrix * worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;


	//load the data into the constant buffer for domain shader
	CBUFFER_DOMAIN_SHADER constantBuffer_dominShader;
	constantBuffer_dominShader.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext_BRK->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_DomainShader_BRK,0,NULL,&constantBuffer_dominShader,0,0);

	//load the data into the constant buffer for hull shader
	CBUFFER_HULL_SHADER constantBuffer_hullShader;
	constantBuffer_hullShader.Hull_Constant_Function_params = XMVectorSet(1.0f,(FLOAT)guiNumberOfLineSegments,0.0f,0.0f);
	gpID3D11DeviceContext_BRK->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_HullShader_BRK,0,NULL,&constantBuffer_hullShader,0,0);
	
	TCHAR str[255];
	wsprintf(str, TEXT("Direct3D11 Window [ Segments = %2d ] "),guiNumberOfLineSegments);
	SetWindowText(ghwnd_BRK,str);
	
	
	//load the data into the constant buffer for pixel shader 
	CBUFFER_PIXEL_SHADER constantBuffer_pixelShader;
	constantBuffer_pixelShader.LineColor = XMVectorSet(1.0f,1.0f,0.0f,1.0f); //yellow
	gpID3D11DeviceContext_BRK->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_PixelShader_BRK,0,NULL,&constantBuffer_pixelShader,0,0);
	
	
	
	
	
	//draw vertex buffer to render target
	gpID3D11DeviceContext_BRK->Draw(4, 0);



	//switch between front and back buffer
	gpIDXGISwapChain_BRK->Present(0, 0);
}

void unintialize(void)
{
	//code
	if (gpID3D11Buffer_ConstantBuffer_HullShader_BRK)
	{
		gpID3D11Buffer_ConstantBuffer_HullShader_BRK->Release();
		gpID3D11Buffer_ConstantBuffer_HullShader_BRK = NULL;
	}

	if (gpID3D11InputLayout_BRK)
	{
		gpID3D11InputLayout_BRK->Release();
		gpID3D11InputLayout_BRK = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_BRK)
	{
		gpID3D11Buffer_VertexBuffer_BRK->Release();
		gpID3D11Buffer_VertexBuffer_BRK = NULL;
	}

	if (gpID3D11PixelShader_BRK)
	{
		gpID3D11PixelShader_BRK->Release();
		gpID3D11PixelShader_BRK = NULL;
	}

	if (gpID3D11VertexShader_BRK)
	{
		gpID3D11VertexShader_BRK->Release();
		gpID3D11VertexShader_BRK = NULL;
	}

	if (gpID3D11RenderTargetView_BRK)
	{
		gpID3D11RenderTargetView_BRK->Release();
		gpID3D11RenderTargetView_BRK = NULL;
	}

	if (gpIDXGISwapChain_BRK)
	{
		gpIDXGISwapChain_BRK->Release();
		gpIDXGISwapChain_BRK = NULL;
	}

	if (gpID3D11DeviceContext_BRK)
	{
		gpID3D11DeviceContext_BRK->Release();
		gpID3D11DeviceContext_BRK = NULL;
	}

	if (gpID3D11Device_BRK)
	{
		gpID3D11Device_BRK->Release();
		gpID3D11Device_BRK = NULL;
	}

	if (gpFile_BRK)
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "unintialize() Succeeded\n");
		fprintf_s(gpFile_BRK, "Log File Is Successfully Closed.\n");
		fclose(gpFile_BRK);
	}
}