#include<windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include<gl/glu.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include<time.h>
//#include "Texture.h"
#include<strsafe.h>
#include "linklist.h"
#include <mmreg.h>
//#include <sys\timeb.h> 

//******** Linker to link the OpenGl Libraray **********//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

//#include "irrKlang/include/irrKlang.h"
//using namespace irrklang;
//******** Linker to link the OpenGl Libraray **********//
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "irrKlang.lib")
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

//ISoundEngine *SoundEngine = createIrrKlangDevice();
list_t newlist;
const char *ptr4;
node_t *newtrav = NULL;
bool soundflag = false;
const volatile char *ptr = NULL;
const volatile char *ptr1 = NULL;
GLuint texture_images[250];
//********* Global function declaration ***********//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define FOURCC_WAVE mmioFOURCC('W', 'A', 'V', 'E')
#define FOURCC_FMT mmioFOURCC('f', 'm', 't', ' ')
#define FOURCC_DATA mmioFOURCC('d', 'a', 't', 'a')

DWORD CalculateWaveLength(LPTSTR FileName)
{
	MMIOINFO mmioinfo = { 0 };
	MMCKINFO mmckinfoRIFF = { 0 };
	MMCKINFO mmckinfoFMT = { 0 };
	MMCKINFO mmckinfoDATA = { 0 };
	MMRESULT mmr;
	WAVEFORMATEXTENSIBLE waveFormat = { 0 };
	HMMIO mmh = mmioOpen(FileName, &mmioinfo, MMIO_DENYNONE | MMIO_READ);
	if (mmh == NULL)
	{
		printf("Unable to open %s: %x\n", FileName, mmioinfo.wErrorRet);
		exit(1);
	}

	mmr = mmioDescend(mmh, &mmckinfoRIFF, NULL, 0);
	if (mmr != MMSYSERR_NOERROR && mmckinfoRIFF.ckid != FOURCC_RIFF)
	{
		printf("Unable to find RIFF section in .WAV file, possible file format error: %x\n", mmr);
		exit(1);
	}
	if (mmckinfoRIFF.fccType != FOURCC_WAVE)
	{
		printf("RIFF file %s is not a WAVE file, possible file format error\n", FileName);
		exit(1);
	}

	// It's a wave file, read the format tag.
	mmckinfoFMT.ckid = FOURCC_FMT;
	mmr = mmioDescend(mmh, &mmckinfoFMT, &mmckinfoRIFF, MMIO_FINDCHUNK);
	if (mmr != MMSYSERR_NOERROR)
	{
		printf("Unable to find FMT section in RIFF file, possible file format error: %x\n", mmr);
		exit(1);
	}
	// The format tag fits into a WAVEFORMAT, so read it in.
	if (mmckinfoFMT.cksize >= sizeof(WAVEFORMAT))
	{
		// Read the requested size (limit the read to the existing buffer though).
		LONG readLength = mmckinfoFMT.cksize;
		if (mmckinfoFMT.cksize >= sizeof(waveFormat))
		{
			readLength = sizeof(waveFormat);
		}
		if (readLength != mmioRead(mmh, (char *)&waveFormat, readLength))
		{
			printf("Read error reading WAVE format from file\n");
			exit(1);
		}
	}
	if (waveFormat.Format.wFormatTag != WAVE_FORMAT_PCM)
	{
		printf("WAVE file %s is not a PCM format file, it's a %d format file\n", FileName, waveFormat.Format.wFormatTag);
		exit(1);
	}
	// Pop back up a level
	mmr = mmioAscend(mmh, &mmckinfoFMT, 0);
	if (mmr != MMSYSERR_NOERROR)
	{
		printf("Unable to pop up in RIFF file, possible file format error: %x\n", mmr);
		exit(1);
	}

	// Now read the data section.
	mmckinfoDATA.ckid = FOURCC_DATA;
	mmr = mmioDescend(mmh, &mmckinfoDATA, &mmckinfoRIFF, MMIO_FINDCHUNK);
	if (mmr != MMSYSERR_NOERROR)
	{
		printf("Unable to find FMT section in RIFF file, possible file format error: %x\n", mmr);
		exit(1);
	}
	// Close the handle, we're done.
	mmr = mmioClose(mmh, 0);
	//
	// We now have all the info we need to calculate the file size. Use 64bit math 
	// to avoid potential rounding issues.
	//
	LONGLONG fileLengthinMS = mmckinfoDATA.cksize * 1000;
	fileLengthinMS /= waveFormat.Format.nAvgBytesPerSec;
	return fileLengthinMS;
}









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
static WCHAR *song = (WCHAR*)malloc(256);
static WCHAR *displaysong = (WCHAR*)malloc(256);
DWORD  filelength = 1;
//struct timeb start_time , current_time;
clock_t start_time, current_time;
static float xpos = 0.0f;
bool progress_bar = false;
bool day_night_effect = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lParam)
{
	//******** Function Declaration ****************//
	void ToggelFullScreen(void);
	void resize(int, int);

	void unintialize(void);
	
	ptr = "Bharat-Humko-Jaan-Se-Pyara-Hai-Instrumental.wav";
	
	size_t var;
	static TCHAR str[100];
	static int song_number = 1;



	//code

	switch (iMsg)
	{
	case WM_CREATE:

		init(&newlist);
		ptr1 = "Anoushka_Shankar_Instrumental.wav";
		add_last(&newlist, (const char*)ptr1);
		ptr1 = "Rakhumai_Rakhumai.wav";
		add_last(&newlist, (const char*)ptr1);
		add_last(&newlist, (const char*)ptr);
		ptr1 = "krishna_flute-theme.wav";
		add_last(&newlist, (const char*)ptr1);
		ptr1 = "Yeh-Haseen-Vadiyan_Instrumental.wav";
		add_last(&newlist, (const char*)ptr1);
		ptr1 = "Shape_of_You.wav";
		add_last(&newlist, (const char*)ptr1);
		newtrav = newlist.head;
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
		case 'N':
		case 'n':
			//MessageBox(hwnd, TEXT("Inside N n."), TEXT("Bhushan Midia msg"), MB_OK);
			if (newtrav != NULL  && newtrav->next != NULL)
			{
				if(song_number != 1)
					newtrav = newtrav->next;
				
				ptr4 = newtrav->filename;
				mbstowcs_s(&var, song, 256, ptr4, _TRUNCATE);

				filelength = CalculateWaveLength((LPTSTR)song);
				//mbstowcs_s(ret, )
				//wsprintf(str, TEXT("Filename = %s"), trav->filename);
				//MessageBox(hwnd, str, L"Filename", MB_OK);
				//wsprintf(str, TEXT("Filename = %d"), filelength);
				//MessageBox(hwnd, str, L"Filename", MB_OK);

				
				//wsprintf(str, TEXT("Filename = %d"), filelength);
				//MessageBox(hwnd, str, L"Filename", MB_OK);
				//soundflag = true;
				//SoundEngine->play2D(ptr4, GL_TRUE);
				//SoundEngine->play2D(ptr4, false,false,false,ESM_AUTO_DETECT,false);
				//MessageBox(NULL, song, L"Song", MB_OK);
				PlaySound(
					(LPCWSTR)song,
					NULL,
					SND_ASYNC
				); 
				
				song_number++;
				//ftime(&start_time);
				start_time = clock();
				xpos = 0.0f;
				progress_bar = true;
			}
			else {
				MessageBox(hwnd, TEXT("This is last song in the list."), TEXT("Matrix Media Player !!!"), MB_OK);
			}
			
			
			break;
		case 'P':
		case 'p':
			//MessageBox(hwnd, TEXT("Inside N n."), TEXT("Bhushan Midia msg"), MB_OK);
			if (newtrav->prev != NULL)
			{

				newtrav = newtrav->prev;
				ptr4 = newtrav->filename;
				mbstowcs_s(&var, song, 256, ptr4, _TRUNCATE);
				
				PlaySound(
					(LPCWSTR)song,
					NULL,
					SND_ASYNC
				);
				
				start_time = clock();
				song_number--;
				xpos = 0.0f;
			}
			else {
				MessageBox(hwnd, TEXT("This is First song in the list."), TEXT("Matrix Media Player !!!"), MB_OK);
			}


			break;
		case 'D':
		case 'd':
			if (day_night_effect == false)
				day_night_effect = true;
			else
				day_night_effect = false;

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

void intialize_all_sea_frames()
{
	BOOL loadTexture(GLuint*, TCHAR*);

	loadTexture(&texture_images[0], (TCHAR*)L"./Sea_frames/frame0001.bmp");
	loadTexture(&texture_images[1], (TCHAR*)L"./Sea_frames/frame0002.bmp");
	loadTexture(&texture_images[2], (TCHAR*)L"./Sea_frames/frame0003.bmp");
	loadTexture(&texture_images[3], (TCHAR*)L"./Sea_frames/frame0004.bmp");
	loadTexture(&texture_images[4], (TCHAR*)L"./Sea_frames/frame0005.bmp");
	loadTexture(&texture_images[5], (TCHAR*)L"./Sea_frames/frame0006.bmp");
	loadTexture(&texture_images[6], (TCHAR*)L"./Sea_frames/frame0007.bmp");
	loadTexture(&texture_images[7], (TCHAR*)L"./Sea_frames/frame0008.bmp");
	loadTexture(&texture_images[8], (TCHAR*)L"./Sea_frames/frame0009.bmp");
	loadTexture(&texture_images[9], (TCHAR*)L"./Sea_frames/frame0010.bmp");
	loadTexture(&texture_images[10], (TCHAR*)L"./Sea_frames/frame0011.bmp");
	loadTexture(&texture_images[11], (TCHAR*)L"./Sea_frames/frame0012.bmp");
	loadTexture(&texture_images[12], (TCHAR*)L"./Sea_frames/frame0013.bmp");
	loadTexture(&texture_images[13], (TCHAR*)L"./Sea_frames/frame0014.bmp");
	loadTexture(&texture_images[14], (TCHAR*)L"./Sea_frames/frame0015.bmp");
	loadTexture(&texture_images[15], (TCHAR*)L"./Sea_frames/frame0016.bmp");
	loadTexture(&texture_images[16], (TCHAR*)L"./Sea_frames/frame0017.bmp");
	loadTexture(&texture_images[17], (TCHAR*)L"./Sea_frames/frame0018.bmp");
	loadTexture(&texture_images[18], (TCHAR*)L"./Sea_frames/frame0019.bmp");
	loadTexture(&texture_images[19], (TCHAR*)L"./Sea_frames/frame0020.bmp");
	loadTexture(&texture_images[20], (TCHAR*)L"./Sea_frames/frame0021.bmp");
	loadTexture(&texture_images[21], (TCHAR*)L"./Sea_frames/frame0022.bmp");
	loadTexture(&texture_images[22], (TCHAR*)L"./Sea_frames/frame0023.bmp");
	loadTexture(&texture_images[23], (TCHAR*)L"./Sea_frames/frame0024.bmp");
	loadTexture(&texture_images[24], (TCHAR*)L"./Sea_frames/frame0025.bmp");
	loadTexture(&texture_images[25], (TCHAR*)L"./Sea_frames/frame0026.bmp");
	loadTexture(&texture_images[26], (TCHAR*)L"./Sea_frames/frame0027.bmp");
	loadTexture(&texture_images[27], (TCHAR*)L"./Sea_frames/frame0028.bmp");
	loadTexture(&texture_images[28], (TCHAR*)L"./Sea_frames/frame0029.bmp");
	loadTexture(&texture_images[29], (TCHAR*)L"./Sea_frames/frame0030.bmp");
	loadTexture(&texture_images[30], (TCHAR*)L"./Sea_frames/frame0031.bmp");
	loadTexture(&texture_images[31], (TCHAR*)L"./Sea_frames/frame0032.bmp");
	loadTexture(&texture_images[32], (TCHAR*)L"./Sea_frames/frame0033.bmp");
	loadTexture(&texture_images[33], (TCHAR*)L"./Sea_frames/frame0034.bmp");
	loadTexture(&texture_images[34], (TCHAR*)L"./Sea_frames/frame0035.bmp");
	loadTexture(&texture_images[35], (TCHAR*)L"./Sea_frames/frame0036.bmp");
	loadTexture(&texture_images[36], (TCHAR*)L"./Sea_frames/frame0037.bmp");
	loadTexture(&texture_images[37], (TCHAR*)L"./Sea_frames/frame0038.bmp");
	loadTexture(&texture_images[38], (TCHAR*)L"./Sea_frames/frame0039.bmp");
	loadTexture(&texture_images[39], (TCHAR*)L"./Sea_frames/frame0040.bmp");
	loadTexture(&texture_images[40], (TCHAR*)L"./Sea_frames/frame0041.bmp");
	loadTexture(&texture_images[41], (TCHAR*)L"./Sea_frames/frame0042.bmp");
	loadTexture(&texture_images[42], (TCHAR*)L"./Sea_frames/frame0043.bmp");
	loadTexture(&texture_images[43], (TCHAR*)L"./Sea_frames/frame0044.bmp");
	loadTexture(&texture_images[44], (TCHAR*)L"./Sea_frames/frame0045.bmp");
	loadTexture(&texture_images[45], (TCHAR*)L"./Sea_frames/frame0046.bmp");
	loadTexture(&texture_images[46], (TCHAR*)L"./Sea_frames/frame0047.bmp");
	loadTexture(&texture_images[47], (TCHAR*)L"./Sea_frames/frame0048.bmp");
	loadTexture(&texture_images[48], (TCHAR*)L"./Sea_frames/frame0049.bmp");
	loadTexture(&texture_images[49], (TCHAR*)L"./Sea_frames/frame0050.bmp");
	loadTexture(&texture_images[50], (TCHAR*)L"./Sea_frames/frame0051.bmp");
	loadTexture(&texture_images[51], (TCHAR*)L"./Sea_frames/frame0052.bmp");
	loadTexture(&texture_images[52], (TCHAR*)L"./Sea_frames/frame0053.bmp");
	loadTexture(&texture_images[53], (TCHAR*)L"./Sea_frames/frame0054.bmp");
	loadTexture(&texture_images[54], (TCHAR*)L"./Sea_frames/frame0055.bmp");
	loadTexture(&texture_images[55], (TCHAR*)L"./Sea_frames/frame0056.bmp");
	loadTexture(&texture_images[56], (TCHAR*)L"./Sea_frames/frame0057.bmp");
	loadTexture(&texture_images[57], (TCHAR*)L"./Sea_frames/frame0058.bmp");
	loadTexture(&texture_images[58], (TCHAR*)L"./Sea_frames/frame0059.bmp");
	loadTexture(&texture_images[59], (TCHAR*)L"./Sea_frames/frame0060.bmp");
	loadTexture(&texture_images[60], (TCHAR*)L"./Sea_frames/frame0061.bmp");
	loadTexture(&texture_images[61], (TCHAR*)L"./Sea_frames/frame0062.bmp");
	loadTexture(&texture_images[62], (TCHAR*)L"./Sea_frames/frame0063.bmp");
	loadTexture(&texture_images[63], (TCHAR*)L"./Sea_frames/frame0064.bmp");
	loadTexture(&texture_images[64], (TCHAR*)L"./Sea_frames/frame0065.bmp");
	loadTexture(&texture_images[65], (TCHAR*)L"./Sea_frames/frame0066.bmp");
	loadTexture(&texture_images[66], (TCHAR*)L"./Sea_frames/frame0067.bmp");
	loadTexture(&texture_images[67], (TCHAR*)L"./Sea_frames/frame0068.bmp");
	loadTexture(&texture_images[68], (TCHAR*)L"./Sea_frames/frame0069.bmp");
	loadTexture(&texture_images[69], (TCHAR*)L"./Sea_frames/frame0070.bmp");
	loadTexture(&texture_images[70], (TCHAR*)L"./Sea_frames/frame0071.bmp");
	loadTexture(&texture_images[71], (TCHAR*)L"./Sea_frames/frame0072.bmp");
	loadTexture(&texture_images[72], (TCHAR*)L"./Sea_frames/frame0073.bmp");
	loadTexture(&texture_images[73], (TCHAR*)L"./Sea_frames/frame0074.bmp");
	loadTexture(&texture_images[74], (TCHAR*)L"./Sea_frames/frame0075.bmp");
	loadTexture(&texture_images[75], (TCHAR*)L"./Sea_frames/frame0076.bmp");
	loadTexture(&texture_images[76], (TCHAR*)L"./Sea_frames/frame0077.bmp");
	loadTexture(&texture_images[77], (TCHAR*)L"./Sea_frames/frame0078.bmp");
	loadTexture(&texture_images[78], (TCHAR*)L"./Sea_frames/frame0079.bmp");
	loadTexture(&texture_images[79], (TCHAR*)L"./Sea_frames/frame0080.bmp");
	loadTexture(&texture_images[80], (TCHAR*)L"./Sea_frames/frame0081.bmp");
	loadTexture(&texture_images[81], (TCHAR*)L"./Sea_frames/frame0082.bmp");
	loadTexture(&texture_images[82], (TCHAR*)L"./Sea_frames/frame0083.bmp");
	loadTexture(&texture_images[83], (TCHAR*)L"./Sea_frames/frame0084.bmp");
	loadTexture(&texture_images[84], (TCHAR*)L"./Sea_frames/frame0085.bmp");
	loadTexture(&texture_images[85], (TCHAR*)L"./Sea_frames/frame0086.bmp");
	loadTexture(&texture_images[86], (TCHAR*)L"./Sea_frames/frame0087.bmp");
	loadTexture(&texture_images[87], (TCHAR*)L"./Sea_frames/frame0088.bmp");
	loadTexture(&texture_images[88], (TCHAR*)L"./Sea_frames/frame0089.bmp");
	loadTexture(&texture_images[89], (TCHAR*)L"./Sea_frames/frame0090.bmp");
	loadTexture(&texture_images[90], (TCHAR*)L"./Sea_frames/frame0091.bmp");
	loadTexture(&texture_images[91], (TCHAR*)L"./Sea_frames/frame0092.bmp");
	loadTexture(&texture_images[92], (TCHAR*)L"./Sea_frames/frame0093.bmp");
	loadTexture(&texture_images[93], (TCHAR*)L"./Sea_frames/frame0094.bmp");
	loadTexture(&texture_images[94], (TCHAR*)L"./Sea_frames/frame0095.bmp");
	loadTexture(&texture_images[95], (TCHAR*)L"./Sea_frames/frame0096.bmp");
	loadTexture(&texture_images[96], (TCHAR*)L"./Sea_frames/frame0097.bmp");
	loadTexture(&texture_images[97], (TCHAR*)L"./Sea_frames/frame0098.bmp");
	loadTexture(&texture_images[98], (TCHAR*)L"./Sea_frames/frame0099.bmp");
	loadTexture(&texture_images[99], (TCHAR*)L"./Sea_frames/frame0100.bmp");


	loadTexture(&texture_images[100], (TCHAR*)L"./Sea_frames/frame0101.bmp");
	loadTexture(&texture_images[101], (TCHAR*)L"./Sea_frames/frame0102.bmp");
	loadTexture(&texture_images[102], (TCHAR*)L"./Sea_frames/frame0103.bmp");
	loadTexture(&texture_images[103], (TCHAR*)L"./Sea_frames/frame0104.bmp");
	loadTexture(&texture_images[104], (TCHAR*)L"./Sea_frames/frame0105.bmp");
	loadTexture(&texture_images[105], (TCHAR*)L"./Sea_frames/frame0106.bmp");
	loadTexture(&texture_images[106], (TCHAR*)L"./Sea_frames/frame0107.bmp");
	loadTexture(&texture_images[107], (TCHAR*)L"./Sea_frames/frame0108.bmp");
	loadTexture(&texture_images[108], (TCHAR*)L"./Sea_frames/frame0109.bmp");
	loadTexture(&texture_images[109], (TCHAR*)L"./Sea_frames/frame0110.bmp");
	loadTexture(&texture_images[110], (TCHAR*)L"./Sea_frames/frame0111.bmp");
	loadTexture(&texture_images[111], (TCHAR*)L"./Sea_frames/frame0112.bmp");
	loadTexture(&texture_images[112], (TCHAR*)L"./Sea_frames/frame0113.bmp");
	loadTexture(&texture_images[113], (TCHAR*)L"./Sea_frames/frame0114.bmp");
	loadTexture(&texture_images[114], (TCHAR*)L"./Sea_frames/frame0115.bmp");
	loadTexture(&texture_images[115], (TCHAR*)L"./Sea_frames/frame0116.bmp");
	loadTexture(&texture_images[116], (TCHAR*)L"./Sea_frames/frame0117.bmp");
	loadTexture(&texture_images[117], (TCHAR*)L"./Sea_frames/frame0118.bmp");
	loadTexture(&texture_images[118], (TCHAR*)L"./Sea_frames/frame0119.bmp");
	loadTexture(&texture_images[119], (TCHAR*)L"./Sea_frames/frame0120.bmp");
	loadTexture(&texture_images[120], (TCHAR*)L"./Sea_frames/frame0121.bmp");
	loadTexture(&texture_images[121], (TCHAR*)L"./Sea_frames/frame0122.bmp");
	loadTexture(&texture_images[122], (TCHAR*)L"./Sea_frames/frame0123.bmp");
	loadTexture(&texture_images[123], (TCHAR*)L"./Sea_frames/frame0124.bmp");
	loadTexture(&texture_images[124], (TCHAR*)L"./Sea_frames/frame0125.bmp");
	loadTexture(&texture_images[125], (TCHAR*)L"./Sea_frames/frame0126.bmp");
	loadTexture(&texture_images[126], (TCHAR*)L"./Sea_frames/frame0127.bmp");
	loadTexture(&texture_images[127], (TCHAR*)L"./Sea_frames/frame0128.bmp");
	loadTexture(&texture_images[128], (TCHAR*)L"./Sea_frames/frame0129.bmp");
	loadTexture(&texture_images[129], (TCHAR*)L"./Sea_frames/frame0130.bmp");
	loadTexture(&texture_images[130], (TCHAR*)L"./Sea_frames/frame0131.bmp");
	loadTexture(&texture_images[131], (TCHAR*)L"./Sea_frames/frame0132.bmp");
	loadTexture(&texture_images[132], (TCHAR*)L"./Sea_frames/frame0133.bmp");
	loadTexture(&texture_images[133], (TCHAR*)L"./Sea_frames/frame0134.bmp");
	loadTexture(&texture_images[134], (TCHAR*)L"./Sea_frames/frame0135.bmp");
	loadTexture(&texture_images[135], (TCHAR*)L"./Sea_frames/frame0136.bmp");
	loadTexture(&texture_images[136], (TCHAR*)L"./Sea_frames/frame0137.bmp");
	loadTexture(&texture_images[137], (TCHAR*)L"./Sea_frames/frame0138.bmp");
	loadTexture(&texture_images[138], (TCHAR*)L"./Sea_frames/frame0139.bmp");
	loadTexture(&texture_images[139], (TCHAR*)L"./Sea_frames/frame0140.bmp");
	loadTexture(&texture_images[140], (TCHAR*)L"./Sea_frames/frame0141.bmp");
	loadTexture(&texture_images[141], (TCHAR*)L"./Sea_frames/frame0142.bmp");
	loadTexture(&texture_images[142], (TCHAR*)L"./Sea_frames/frame0143.bmp");
	loadTexture(&texture_images[143], (TCHAR*)L"./Sea_frames/frame0144.bmp");
	loadTexture(&texture_images[144], (TCHAR*)L"./Sea_frames/frame0145.bmp");
	loadTexture(&texture_images[145], (TCHAR*)L"./Sea_frames/frame0146.bmp");
	loadTexture(&texture_images[146], (TCHAR*)L"./Sea_frames/frame0147.bmp");
	loadTexture(&texture_images[147], (TCHAR*)L"./Sea_frames/frame0148.bmp");
	loadTexture(&texture_images[148], (TCHAR*)L"./Sea_frames/frame0149.bmp");
	loadTexture(&texture_images[149], (TCHAR*)L"./Sea_frames/frame0150.bmp");
	loadTexture(&texture_images[150], (TCHAR*)L"./Sea_frames/frame0151.bmp");
	loadTexture(&texture_images[151], (TCHAR*)L"./Sea_frames/frame0152.bmp");
	loadTexture(&texture_images[152], (TCHAR*)L"./Sea_frames/frame0153.bmp");
	loadTexture(&texture_images[153], (TCHAR*)L"./Sea_frames/frame0154.bmp");
	loadTexture(&texture_images[154], (TCHAR*)L"./Sea_frames/frame0155.bmp");
	loadTexture(&texture_images[155], (TCHAR*)L"./Sea_frames/frame0156.bmp");
	loadTexture(&texture_images[156], (TCHAR*)L"./Sea_frames/frame0157.bmp");
	loadTexture(&texture_images[157], (TCHAR*)L"./Sea_frames/frame0158.bmp");
	loadTexture(&texture_images[158], (TCHAR*)L"./Sea_frames/frame0159.bmp");
	loadTexture(&texture_images[159], (TCHAR*)L"./Sea_frames/frame0160.bmp");
	loadTexture(&texture_images[160], (TCHAR*)L"./Sea_frames/frame0161.bmp");
	loadTexture(&texture_images[161], (TCHAR*)L"./Sea_frames/frame0162.bmp");
	loadTexture(&texture_images[162], (TCHAR*)L"./Sea_frames/frame0163.bmp");
	loadTexture(&texture_images[163], (TCHAR*)L"./Sea_frames/frame0164.bmp");
	loadTexture(&texture_images[164], (TCHAR*)L"./Sea_frames/frame0165.bmp");
	loadTexture(&texture_images[165], (TCHAR*)L"./Sea_frames/frame0166.bmp");
	loadTexture(&texture_images[166], (TCHAR*)L"./Sea_frames/frame0167.bmp");
	loadTexture(&texture_images[167], (TCHAR*)L"./Sea_frames/frame0168.bmp");
	loadTexture(&texture_images[168], (TCHAR*)L"./Sea_frames/frame0169.bmp");
	loadTexture(&texture_images[169], (TCHAR*)L"./Sea_frames/frame0170.bmp");
	loadTexture(&texture_images[170], (TCHAR*)L"./Sea_frames/frame0171.bmp");
	loadTexture(&texture_images[171], (TCHAR*)L"./Sea_frames/frame0172.bmp");
	loadTexture(&texture_images[172], (TCHAR*)L"./Sea_frames/frame0173.bmp");
	loadTexture(&texture_images[173], (TCHAR*)L"./Sea_frames/frame0174.bmp");
	loadTexture(&texture_images[174], (TCHAR*)L"./Sea_frames/frame0175.bmp");
	loadTexture(&texture_images[175], (TCHAR*)L"./Sea_frames/frame0176.bmp");
	loadTexture(&texture_images[176], (TCHAR*)L"./Sea_frames/frame0177.bmp");
	loadTexture(&texture_images[177], (TCHAR*)L"./Sea_frames/frame0178.bmp");
	loadTexture(&texture_images[178], (TCHAR*)L"./Sea_frames/frame0179.bmp");
	loadTexture(&texture_images[179], (TCHAR*)L"./Sea_frames/frame0180.bmp");
	loadTexture(&texture_images[180], (TCHAR*)L"./Sea_frames/frame0181.bmp");
	loadTexture(&texture_images[181], (TCHAR*)L"./Sea_frames/frame0182.bmp");
	loadTexture(&texture_images[182], (TCHAR*)L"./Sea_frames/frame0183.bmp");
	loadTexture(&texture_images[183], (TCHAR*)L"./Sea_frames/frame0184.bmp");
	loadTexture(&texture_images[184], (TCHAR*)L"./Sea_frames/frame0185.bmp");
	loadTexture(&texture_images[185], (TCHAR*)L"./Sea_frames/frame0186.bmp");
	loadTexture(&texture_images[186], (TCHAR*)L"./Sea_frames/frame0187.bmp");
	loadTexture(&texture_images[187], (TCHAR*)L"./Sea_frames/frame0188.bmp");
	loadTexture(&texture_images[188], (TCHAR*)L"./Sea_frames/frame0189.bmp");
	loadTexture(&texture_images[189], (TCHAR*)L"./Sea_frames/frame0190.bmp");
	loadTexture(&texture_images[190], (TCHAR*)L"./Sea_frames/frame0191.bmp");
	loadTexture(&texture_images[191], (TCHAR*)L"./Sea_frames/frame0192.bmp");
	loadTexture(&texture_images[192], (TCHAR*)L"./Sea_frames/frame0193.bmp");
	loadTexture(&texture_images[193], (TCHAR*)L"./Sea_frames/frame0194.bmp");
	loadTexture(&texture_images[194], (TCHAR*)L"./Sea_frames/frame0195.bmp");
	loadTexture(&texture_images[195], (TCHAR*)L"./Sea_frames/frame0196.bmp");
	loadTexture(&texture_images[196], (TCHAR*)L"./Sea_frames/frame0197.bmp");
	loadTexture(&texture_images[197], (TCHAR*)L"./Sea_frames/frame0198.bmp");
	loadTexture(&texture_images[198], (TCHAR*)L"./Sea_frames/frame0199.bmp");
	loadTexture(&texture_images[199], (TCHAR*)L"./Sea_frames/frame0200.bmp");

	loadTexture(&texture_images[200], (TCHAR*)L"./Sea_frames/frame0201.bmp");
	loadTexture(&texture_images[201], (TCHAR*)L"./Sea_frames/frame0202.bmp");
	loadTexture(&texture_images[202], (TCHAR*)L"./Sea_frames/frame0203.bmp");
	loadTexture(&texture_images[203], (TCHAR*)L"./Sea_frames/frame0204.bmp");
	loadTexture(&texture_images[204], (TCHAR*)L"./Sea_frames/frame0205.bmp");
	loadTexture(&texture_images[205], (TCHAR*)L"./Sea_frames/frame0206.bmp");
	loadTexture(&texture_images[206], (TCHAR*)L"./Sea_frames/frame0207.bmp");
	loadTexture(&texture_images[207], (TCHAR*)L"./Sea_frames/frame0208.bmp");
	loadTexture(&texture_images[208], (TCHAR*)L"./Sea_frames/frame0209.bmp");
	loadTexture(&texture_images[209], (TCHAR*)L"./Sea_frames/frame0210.bmp");
	loadTexture(&texture_images[210], (TCHAR*)L"./Sea_frames/frame0211.bmp");
	loadTexture(&texture_images[211], (TCHAR*)L"./Sea_frames/frame0212.bmp");
	loadTexture(&texture_images[212], (TCHAR*)L"./Sea_frames/frame0213.bmp");
	loadTexture(&texture_images[213], (TCHAR*)L"./Sea_frames/frame0214.bmp");
	loadTexture(&texture_images[214], (TCHAR*)L"./Sea_frames/frame0215.bmp");
	loadTexture(&texture_images[215], (TCHAR*)L"./Sea_frames/frame0216.bmp");
	loadTexture(&texture_images[216], (TCHAR*)L"./Sea_frames/frame0217.bmp");
	loadTexture(&texture_images[217], (TCHAR*)L"./Sea_frames/frame0218.bmp");
	loadTexture(&texture_images[218], (TCHAR*)L"./Sea_frames/frame0219.bmp");
	loadTexture(&texture_images[219], (TCHAR*)L"./Sea_frames/frame0220.bmp");
	loadTexture(&texture_images[220], (TCHAR*)L"./Sea_frames/frame0221.bmp");
	loadTexture(&texture_images[221], (TCHAR*)L"./Sea_frames/frame0222.bmp");
	loadTexture(&texture_images[222], (TCHAR*)L"./Sea_frames/frame0223.bmp");
	loadTexture(&texture_images[223], (TCHAR*)L"./Sea_frames/frame0224.bmp");
	loadTexture(&texture_images[224], (TCHAR*)L"./Sea_frames/frame0225.bmp");
	loadTexture(&texture_images[225], (TCHAR*)L"./Sea_frames/frame0226.bmp");
	loadTexture(&texture_images[226], (TCHAR*)L"./Sea_frames/frame0227.bmp");
	loadTexture(&texture_images[227], (TCHAR*)L"./Sea_frames/frame0228.bmp");
	loadTexture(&texture_images[228], (TCHAR*)L"./Sea_frames/frame0229.bmp");
	loadTexture(&texture_images[229], (TCHAR*)L"./Sea_frames/frame0230.bmp");
	loadTexture(&texture_images[230], (TCHAR*)L"./Sea_frames/frame0231.bmp");
	loadTexture(&texture_images[231], (TCHAR*)L"./Sea_frames/frame0232.bmp");
	loadTexture(&texture_images[232], (TCHAR*)L"./Sea_frames/frame0233.bmp");
	loadTexture(&texture_images[233], (TCHAR*)L"./Sea_frames/frame0234.bmp");
	loadTexture(&texture_images[234], (TCHAR*)L"./Sea_frames/frame0235.bmp");
	loadTexture(&texture_images[235], (TCHAR*)L"./Sea_frames/frame0236.bmp");
	loadTexture(&texture_images[236], (TCHAR*)L"./Sea_frames/frame0237.bmp");
	loadTexture(&texture_images[237], (TCHAR*)L"./Sea_frames/frame0238.bmp");
	loadTexture(&texture_images[238], (TCHAR*)L"./Sea_frames/frame0239.bmp");
	loadTexture(&texture_images[239], (TCHAR*)L"./Sea_frames/frame0240.bmp");
	loadTexture(&texture_images[240], (TCHAR*)L"./Sea_frames/frame0241.bmp");
	loadTexture(&texture_images[241], (TCHAR*)L"./Sea_frames/frame0242.bmp");
	loadTexture(&texture_images[242], (TCHAR*)L"./Sea_frames/frame0243.bmp");
	loadTexture(&texture_images[243], (TCHAR*)L"./Sea_frames/frame0244.bmp");
	loadTexture(&texture_images[244], (TCHAR*)L"./Sea_frames/frame0245.bmp");
	loadTexture(&texture_images[245], (TCHAR*)L"./Sea_frames/frame0246.bmp");
	loadTexture(&texture_images[246], (TCHAR*)L"./Sea_frames/frame0247.bmp");
	loadTexture(&texture_images[247], (TCHAR*)L"./Sea_frames/frame0248.bmp");
	loadTexture(&texture_images[248], (TCHAR*)L"./Sea_frames/frame0249.bmp");
	loadTexture(&texture_images[249], (TCHAR*)L"./Sea_frames/frame0250.bmp");


}


int intialize(void)
{
	fprintf(gpFile, "Inside Intialization.!!!\n");
	// ******* Function declaration **********//
	void resize(int, int);
	
	void ToggelFullScreen(void);
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

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	
	PlaySound(
		TEXT("ocean-waves.wav"),
		NULL,
		SND_ASYNC | SND_LOOP
	);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);

	intialize_all_sea_frames();

	resize(WIN_WIDTH, WIN_HEIGHT);
	
	ToggelFullScreen();
	
	return(0);
}



BOOL loadTexture(GLuint *texture, TCHAR* imageResouceID)
{
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), (LPCWSTR)imageResouceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		DeleteObject(hBitmap);
	}

	return (bStatus);
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
		glOrtho(-1.0f,
			1.0f,
			(-1.0f * ((GLfloat)height / (GLfloat)width)),
			(1.0f * ((GLfloat)height / (GLfloat)width)),
			-1.0f,
			1.0f
		);
	}
	else {
		glOrtho((-1.0f * ((GLfloat)width / (GLfloat)height)),
			(1.0f * ((GLfloat)width / (GLfloat)height)),
			-1.0f,
			1.0f,
			-1.0f,
			1.0f);
	}
}

void display()
{
	//code
	static int timer = 1;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	static float red = 1.0f, green = 0.9f, blue = 0.9f;
	static bool gflag = true, bflag = true;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glColor3f(red, green, blue);
	if (day_night_effect == true) {
		if (green < 1.0f && gflag == true) {
			green = green + 0.001;
		}



		if (blue < 1.0f && bflag == true) {
			blue = blue + 0.001;
		}

		if (blue > 1.0f) {
			bflag = false;
			gflag = false;
			//blue = 1.0f;
			//green = 1.0f;
		}

		if (bflag == false && blue > 0.0f)
		{
			blue = blue - 0.001;
			if (green > 0.5f) {
				green = green - 0.001;
			}
		}

		if (blue < 0.0f) {
			bflag = true;
			gflag = true;
		}
	}
	else {
		red = 1.0f; green = 0.9f; blue = 0.9f;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, 0.0f);
	//glRotatef(triangle_rotation_angle, 0.0f, 1.0f, 0.0f);
	glScalef(2.0f, 1.0f, 1.0);


	// Bind texture
	static int count = 0;

	if (timer % 5 == 0)
	{
		count++;
	}
	else {
		glBindTexture(GL_TEXTURE_2D, texture_images[count]);
	}

	if (count > 249)
	{
		count = 0;
		timer = 0;
	}
	   	  
	// Draw Quard
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0, 1);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0, 0);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1, 0);
	glVertex2f(1.0f, -1.0f);
	glEnd();

	timer++;


	// Text code
	GLuint nFontList = glGenLists(128);
	HDC hDC;
	hDC = wglGetCurrentDC();
	HFONT hFont;
	LOGFONT logfont;
	logfont.lfHeight = -20;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = TRUE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	//lstrcpyW(logfont.lfFaceName, TEXT("Arial"));
	strcpy_s((char*)logfont.lfFaceName, 27, "Arial");
	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(hDC, hFont);
	GLYPHMETRICSFLOAT agmf[128] ;
	// Create display lists for glyphs 0 through 128
	nFontList = glGenLists(128);
	//wglUseFontOutlinesW(hDC, 0, 128, nFontList, 0.0f, 0.2f, WGL_FONT_POLYGONS, agmf);
	wglUseFontBitmaps(hDC, 0, 128, nFontList);
	DeleteObject(hFont); // Don’t need original font anymore
	// Black Background
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 0.0f);

	glListBase(nFontList);
	size_t var;
	node* display;
	display = newlist.head;
	float ypos = 0.8f;
	
	
	glColor3f(0.0f, 0.0f, 0.0f);
	ptr4 = "Play List";
	mbstowcs_s(&var, displaysong, 256, ptr4, _TRUNCATE);
	glRasterPos3f(0.0, ypos, 0.0);
	//glScalef(0.1f, 0.1f, 0.1f);
	glCallLists(40, GL_UNSIGNED_BYTE, (LPCWSTR)displaysong);

	//ftime(&current_time);
	current_time = clock();
	
	//DWORD song_time = (DWORD)(1000.0 * (current_time.time - start_time.time)
	//	+ (current_time.millitm - start_time.millitm));

	double song_time = ((double)(current_time - start_time) )/ CLOCKS_PER_SEC * 10;

	if (song_time < filelength)
	{
		xpos = xpos + (0.8f/ filelength);
	}
	//else
		//xpos = 0.0f;

	while (display != NULL)
	{
		//glRasterPos2i(0, ypos);
		ypos = ypos - 0.1f;
		
		ptr4 = display->filename;
		mbstowcs_s(&var, displaysong, 256, ptr4, _TRUNCATE);
		//glColor3f(0.0f, 1.0f, 0.0f);
		if (display->filename == newtrav->filename)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glRasterPos3f(0.0, ypos, 0.0);
			//glScalef(0.1f, 0.1f, 0.1f);
			glCallLists(60, GL_UNSIGNED_BYTE, (LPCWSTR)displaysong);
			if (progress_bar == true) {
				glLineWidth(5);
				ypos = ypos - 0.1f;

				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(0, ypos, 0);
				glVertex3f(0.8f, ypos, 0);
				glEnd();

				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(0, ypos, 0);
				glVertex3f(xpos, ypos, 0);
				glEnd();
			}
		}
		else {
			glColor3f(1.0f, 1.0f, 0.0f);
			glRasterPos3f(0.0, ypos, 0.0);
			//glScalef(0.1f, 0.1f, 0.1f);
			glCallLists(60, GL_UNSIGNED_BYTE, (LPCWSTR)displaysong);
		}

		
		display = display->next;
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


