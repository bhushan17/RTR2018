
#include<windows.h>
#include<stdio.h> // for file I/O

#include<d3d11.h>
#include<d3dcompiler.h>
//#include"WICTextureLoader.h" // For DirectX::CreateWICTextureFromFile

#pragma warning(disable: 4838)
#include"XNAMath\xnamath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")
//#pragma comment(lib,"DirectXTK.lib")  //For DirectX::CreateWICTextureFromFile

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
ID3D11PixelShader *gpID3D11PixelShader_BRK = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position_Cube_BRK = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Texture_Cube_BRK = NULL;
ID3D11InputLayout *gpID3D11InputLayout_BRK = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_BRK = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;



ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_Cube = NULL;
ID3D11SamplerState *gpID3D11SamplerState_Texture_Cube = NULL;
ID3D11Texture2D *pTexture = NULL;
//D3D11_BOX destRegion;



//UINT rowPitch = 32;


#define CHECKIMAGEWIDTH   64
#define CHECKIMAGEHEIGHT  64

UCHAR checkimage[CHECKIMAGEWIDTH][CHECKIMAGEHEIGHT][4];

float gAngleCube = 0.0f; //Angle of rotation on Cube

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

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
	HRESULT LoadD3DTexture(const wchar_t *, ID3D11ShaderResourceView **);

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
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"struct vertex_output" \
		"{" \
		"float4 position : SV_POSITION;" \
		"float2 texcoord : TEXCOORD;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float2 texcoord : TEXCOORD )" \
		"{" \
		"vertex_output output;" \
		"output.position = mul(worldViewProjectionMatrix , pos);" \
		"output.texcoord = texcoord;"
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

	const char *pixelShaderSourceCode =
		"Texture2D myTexture2D;" \
		"SamplerState mySamplerState;"
		"float4 main(float4 pos:SV_POSITION,float2 texcoord:TEXCOORD) : SV_TARGET" \
		"{" \
		"float4 color = myTexture2D.Sample(mySamplerState, texcoord);" \
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
	
	
	   
	float cube_vertices[] =
	{
		

		//Side 3 (Front)
		//triangle 1
		-2.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		-2.0f,-1.0f,0.0f,
		//triangle 2
		-2.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f,

		

	};

	float cube_texcoords[] =
	{
		

		
		0.0f,0.0f,
		1.0f,0.0f,
		0.0f,1.0f,
		0.0f,1.0f,
		1.0f,0.0f,
		1.0f,1.0f
		
	};

	//create vertex buffer
	


	D3D11_BUFFER_DESC bufferDesc_CubePosition;
	ZeroMemory(&bufferDesc_CubePosition, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_CubePosition.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_CubePosition.ByteWidth = sizeof(float) * ARRAYSIZE(cube_vertices);
	bufferDesc_CubePosition.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_CubePosition.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_CubePosition, NULL, &gpID3D11Buffer_VertexBuffer_Position_Cube_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Rectangle Vertex Buffer. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBufferShader() Succeeded For Rectangle Vertex Buffer.  \n");
		fclose(gpFile_BRK);
	}

	D3D11_BUFFER_DESC bufferDesc_CubeTexture;
	ZeroMemory(&bufferDesc_CubeTexture, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_CubeTexture.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_CubeTexture.ByteWidth = sizeof(float) * ARRAYSIZE(cube_texcoords);
	bufferDesc_CubeTexture.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_CubeTexture.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_CubeTexture, NULL, &gpID3D11Buffer_VertexBuffer_Texture_Cube_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Rectangle Color Buffer. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBufferShader() Succeeded For Color Buffer.  \n");
		fclose(gpFile_BRK);
	}

	//copy Trianglevertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	gpID3D11DeviceContext_BRK->Map(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, cube_vertices, sizeof(cube_vertices));
	gpID3D11DeviceContext_BRK->Unmap(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, NULL);

	gpID3D11DeviceContext_BRK->Map(gpID3D11Buffer_VertexBuffer_Texture_Cube_BRK, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, cube_texcoords, sizeof(cube_texcoords));
	gpID3D11DeviceContext_BRK->Unmap(gpID3D11Buffer_VertexBuffer_Texture_Cube_BRK, NULL);
	//create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device_BRK->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc), pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout_BRK);

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

	//define and set the constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device_BRK->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr, &gpID3D11Buffer_ConstantBuffer_BRK);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Failed for Constant Buffer. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateBuffer() Succeeded for Constant Buffer. \n");
		fclose(gpFile_BRK);
	}

	gpID3D11DeviceContext_BRK->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_BRK);


	//Set Rasterization state
	//In D3D, backface culling is by default 'ON'
	//Means backface of geometry will not be visible
	//this causes our 2D triangles backface to vanish on rotation.
	//so set culling 'OFF'
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void *)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //this allows both sides of 2D geometry visible on rotation
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device_BRK->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRasterizerState() Failed for Culling. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateRasterizerState() Succeeded for Culling. \n");
		fclose(gpFile_BRK);
	}
	
	gpID3D11DeviceContext_BRK->RSSetState(gpID3D11RasterizerState);

	//PYRAMID
	//create texture resource and texture view
	
	//CUBE
	//create texture resource and texture view
	//hr = LoadD3DTexture(L"Vijay_Kundali.bmp", &gpID3D11ShaderResourceView_Texture_Cube);
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "LoadD3DTexture() Failed for Cube Texture. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "LoadD3DTexture() Succeeded for Cube Texture. \n");
		fclose(gpFile_BRK);
	}

	//Create the sample state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	hr = gpID3D11Device_BRK->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture_Cube);
	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateSamplerState() Failed for Cube. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateSamplerState() Succeeded for Cube. \n");
		fclose(gpFile_BRK);
	}
		   	  
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


	/*destRegion.left = 120;
	destRegion.right = 200;
	destRegion.top = 100;
	destRegion.bottom = 220;
	destRegion.front = 0;
	destRegion.back = 1;*/




	void makecheckImage(void);

	makecheckImage();

	D3D11_TEXTURE2D_DESC desc;

	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Width = CHECKIMAGEWIDTH;
	desc.Height = CHECKIMAGEHEIGHT;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	hr =  gpID3D11Device_BRK->CreateTexture2D(&desc, NULL, &pTexture);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateTexture2D() Failed for Cube. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateTexture2D() Succeeded for Cube. \n");
		fclose(gpFile_BRK);
	}



	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext_BRK->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, checkimage, sizeof(UCHAR) * CHECKIMAGEWIDTH * CHECKIMAGEHEIGHT * 4);
	gpID3D11DeviceContext_BRK->Unmap(pTexture, NULL);

	D3D11_SHADER_RESOURCE_VIEW_DESC gpID3D11ShaderResourceViewDesc;
	//memset((void*)&gpID3D11ShaderResourceViewDesc, 0, sizeof(gpID3D11ShaderResourceViewDesc));
	ZeroMemory((void*)&gpID3D11ShaderResourceViewDesc, sizeof(gpID3D11ShaderResourceViewDesc));

	gpID3D11ShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpID3D11ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device_BRK->CreateShaderResourceView(pTexture, &gpID3D11ShaderResourceViewDesc, &gpID3D11ShaderResourceView_Texture_Cube);

	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateShaderResourceView() Failed for Cube. \n");
		fclose(gpFile_BRK);
		return(hr);

	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "ID3D11Device :: CreateShaderResourceView() Succeeded for Cube. \n");
		fclose(gpFile_BRK);
	}

	return(S_OK);
}



void makecheckImage(void)
{
	int i, j, c;

	for (i = 0; i < CHECKIMAGEHEIGHT; i++)
	{
		for (j = 0; j < CHECKIMAGEWIDTH; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkimage[i][j][0] = (UCHAR)c;
			checkimage[i][j][1] = (UCHAR)c;
			checkimage[i][j][2] = (UCHAR)c;
			checkimage[i][j][3] = (UCHAR)255;
		}
	}
}
/*
HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	//code
	HRESULT hr;
	void makecheckImage(void);

	makecheckImage();
	//create texture
	//hr = DirectX::CreateWICTextureFromFile(gpID3D11Device_BRK, gpID3D11DeviceContext_BRK, textureFileName, NULL, ppID3D11ShaderResourceView);

	hr = DirectX::CreateWICTextureFromMemory(gpID3D11Device_BRK, gpID3D11DeviceContext_BRK,(uint8_t*) checkimage,(size_t) sizeof(checkimage), NULL, ppID3D11ShaderResourceView);


	if (FAILED(hr))
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "CreateWICTextureFromMemory() Failed for texture Resource. \n");
		fclose(gpFile_BRK);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile_BRK, gszLogFileName, "a+");
		fprintf_s(gpFile_BRK, "CreateWICTextureFromMemory() succeeded for texture resource. \n");
		fclose(gpFile_BRK);

	}

	return(hr);
}
*/


HRESULT resize(int width, int height)
{
	//code
	HRESULT hr = S_OK;

	//free any size-dependant resources
	if (gpID3D11RenderTargetView_BRK)
	{
		gpID3D11RenderTargetView_BRK->Release();
		gpID3D11RenderTargetView_BRK = NULL;
	}

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
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

	//Create depth stencil buffer (or zbuffer)
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
	gpID3D11DeviceContext_BRK->OMSetRenderTargets(1, &gpID3D11RenderTargetView_BRK, gpID3D11DepthStencilView);


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

	//Clear the depth/stencil view to 1.0
	gpID3D11DeviceContext_BRK->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//select which vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;



	//transition is concerned with world matrix transformation
	XMMATRIX translationMatrix = XMMatrixTranslation(-1.5f, 0.0f, 6.0f);
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	//final WorldViewProjection matrix
	XMMATRIX wvpMatrix = XMMatrixIdentity();

	//load the data inro the constant buffer
	CBUFFER constantBuffer;
	
	stride = sizeof(float) * 3;
	offset = 0;
	
	gpID3D11DeviceContext_BRK->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, &stride, &offset);

	stride = sizeof(float) * 2;
	offset = 0;
	gpID3D11DeviceContext_BRK->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Texture_Cube_BRK, &stride, &offset);

	//bind texture and sampler as pixel shader resorce
	gpID3D11DeviceContext_BRK->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_Cube);
	gpID3D11DeviceContext_BRK->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_Cube);
	//select geometry primtive
	gpID3D11DeviceContext_BRK->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	//select geometry primtive
	gpID3D11DeviceContext_BRK->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//transition is concerned with world matrix transformation
	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixRotationY(-gAngleCube);
	XMMATRIX rotationMatriX = XMMatrixRotationX(-gAngleCube);
	XMMATRIX rotationMatriZ = XMMatrixRotationZ(-gAngleCube);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	//final WorldViewProjection matrix
	wvpMatrix = scaleMatrix * /*rotationMatrix * rotationMatriX*rotationMatriZ**/ translationMatrix * worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	//load the data inro the constant buffer
	constantBuffer;
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext_BRK->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_BRK, 0, NULL, &constantBuffer, 0, 0);
	//gpID3D11DeviceContext_BRK->UpdateSubresource(gpID3D11ShaderResourceView_Texture_Cube, 0, &destRegion, checkimage	, rowPitch, 0);

	float cube_vertices[] =
	{


		//Side 3 (Front)
		//triangle 1
		-2.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		-2.0f,-1.0f,0.0f,
		//triangle 2
		-2.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,-1.0f,0.0f,



	};

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext_BRK->Map(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, cube_vertices, sizeof(cube_vertices));
	gpID3D11DeviceContext_BRK->Unmap(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, NULL);

	//draw vertex buffer to render target
	gpID3D11DeviceContext_BRK->Draw(6, 0);
	
	float cube2_vertices[] =
	{


		//Side 3 (Front)
		//triangle 1
		1.0f,0.8f,-1.41421f,
		2.41421,0.8f,0,
		1.0f,-0.8f,-1.41421f,
		//triangle 2
		1.0f,-0.8f,-1.41421f,
		2.41421,0.8f,0,
		2.41421,-0.8f,0,



	};

	
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gpID3D11DeviceContext_BRK->Map(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, cube2_vertices, sizeof(cube2_vertices));
	gpID3D11DeviceContext_BRK->Unmap(gpID3D11Buffer_VertexBuffer_Position_Cube_BRK, NULL);

	//draw vertex buffer to render target
	gpID3D11DeviceContext_BRK->Draw(6, 0);
		

	//switch between front and back buffer
	gpIDXGISwapChain_BRK->Present(0, 0);
}

void unintialize(void)
{
	//code
	if (gpID3D11SamplerState_Texture_Cube)
	{
		gpID3D11SamplerState_Texture_Cube->Release();
		gpID3D11SamplerState_Texture_Cube = NULL;
	}

	if (gpID3D11ShaderResourceView_Texture_Cube)
	{
		gpID3D11ShaderResourceView_Texture_Cube->Release();
		gpID3D11ShaderResourceView_Texture_Cube = NULL;
	}

	

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBuffer_BRK)
	{
		gpID3D11Buffer_ConstantBuffer_BRK->Release();
		gpID3D11Buffer_ConstantBuffer_BRK = NULL;
	}

	if (gpID3D11InputLayout_BRK)
	{
		gpID3D11InputLayout_BRK->Release();
		gpID3D11InputLayout_BRK = NULL;
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