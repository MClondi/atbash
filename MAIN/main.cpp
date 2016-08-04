#include <windows.h>
#include "MainModel.h"

#define WINDOW L"atbash"
#define EDIT_THREADS 0
#define RADIO_CPP 1
#define RADIO_ASM 2
#define LABEL_FILEPATH 3
#define BUTTON_CHOOSE 4
#define BUTTON_START 5
#define BUTTON_OPEN 6
#define LABEL_THREADS 7
#define LABEL_TIME 8

LRESULT CALLBACK WndProc(HWND mainWindow, UINT msg, WPARAM wParam, LPARAM lParam);

HWND mainWindow;
HWND filepath;
HWND timeLabel;
MainModel mainModel;

// Generic WinApi window, with placed widgets
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Error", MB_ICONERROR | MB_OK);
		return EXIT_FAILURE;
	}

	mainWindow = CreateWindowEx(WS_EX_CLIENTEDGE, WINDOW, L"Atbash", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 540, 260, NULL, NULL, hInstance, NULL);
	if (!mainWindow)
	{
		MessageBox(NULL, L"Call to CreateWindow failed!", L"Error", MB_ICONERROR | MB_OK);
		return EXIT_FAILURE;
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	WCHAR cpuThreads[2];
	swprintf_s(cpuThreads, 2, L"%d", sysinfo.dwNumberOfProcessors);

	HWND threadsLabel = CreateWindowEx(0, L"EDIT", L"Threads (1-64):", ES_READONLY | WS_CHILD | WS_VISIBLE, 20, 20, 150, 30, mainWindow, (HMENU)LABEL_THREADS, hInstance, NULL);
	HWND threads = CreateWindowEx(0, L"EDIT", cpuThreads, WS_BORDER | ES_NUMBER | WS_CHILD | WS_VISIBLE, 180, 20, 30, 20, mainWindow, (HMENU)EDIT_THREADS, hInstance, NULL);
	
	HWND cppRadio = CreateWindowEx(0, L"BUTTON", L"Use C++", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 60, 150, 30, mainWindow, (HMENU)RADIO_CPP, hInstance, NULL);
	HWND asmRadio = CreateWindowEx(0, L"BUTTON", L"Use ASM", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 180, 60, 150, 30, mainWindow, (HMENU)RADIO_ASM, hInstance, NULL);
	SendMessage(cppRadio, BM_SETCHECK, (WPARAM)"", TRUE);

	filepath = CreateWindowEx(0, L"EDIT", L"", ES_READONLY | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE, 20, 100, 480, 30, mainWindow, (HMENU)LABEL_FILEPATH, hInstance, NULL);
	
	HWND chooseButton = CreateWindowEx(0, L"BUTTON", L"Choose file", WS_CHILD | WS_VISIBLE, 20, 140, 150, 30, mainWindow, (HMENU)BUTTON_CHOOSE, hInstance, NULL);
	HWND startButton = CreateWindowEx(0, L"BUTTON", L"Start", WS_CHILD | WS_VISIBLE, 180, 140, 150, 30, mainWindow, (HMENU)BUTTON_START, hInstance, NULL);
	HWND openButton = CreateWindowEx(0, L"BUTTON", L"Open file", WS_CHILD | WS_VISIBLE, 340, 140, 150, 30, mainWindow, (HMENU)BUTTON_OPEN, hInstance, NULL);

	HWND staticTimeLabel = CreateWindowEx(0, L"EDIT", L"Time (ms):", ES_READONLY | WS_CHILD | WS_VISIBLE, 20, 180, 150, 30, mainWindow, (HMENU)LABEL_THREADS, hInstance, NULL);
	timeLabel = CreateWindowEx(0, L"EDIT", L"", ES_READONLY | WS_CHILD | WS_VISIBLE, 180, 180, 300, 30, mainWindow, (HMENU)LABEL_TIME, hInstance, NULL);


	ShowWindow(mainWindow, nCmdShow);
	UpdateWindow(mainWindow);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return message.wParam;
}

// Callbacks handled here
LRESULT CALLBACK WndProc(HWND mainWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case RADIO_ASM: // The user checked "Use ASM"
			mainModel.useAsmDll();
			break;
		case RADIO_CPP: // The user checked "Use C++" 
			mainModel.useCppDll();
			break;
		case BUTTON_CHOOSE: // Opening another windows where the user chooses the .txt file to cipher
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = mainWindow;
			ofn.lpstrFile = new TCHAR[MAX_PATH];
			ofn.lpstrFile[0] = '\0';
			
			WCHAR initPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, initPath);
			ofn.lpstrInitialDir = (LPCWSTR)initPath; 

			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("txt Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
			ofn.nFilterIndex = 0;

			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if(GetOpenFileName(&ofn)){
				SetDlgItemText(mainWindow, LABEL_FILEPATH, ofn.lpstrFile);
				mainModel.setFile(ofn.lpstrFile);
			};
			break;
		case BUTTON_START:	// Checking the thread amout, and if its valid, starting the calculations
			{
				UINT threads = GetDlgItemInt(mainWindow, EDIT_THREADS, NULL, TRUE);
				if (threads > 64 || threads < 1){
					MessageBox(NULL, L"Thread number invalid", L"Error", MB_ICONERROR | MB_OK);
					break;
				}
				if (mainModel.getFile() == L""){
					MessageBox(NULL, L"Filepath cannot be empty", L"Error", MB_ICONERROR | MB_OK);
					break;
				}
				mainModel.setThreads(threads);
				long long time = mainModel.start();
				SetDlgItemText(mainWindow, LABEL_TIME, std::to_wstring(time).c_str());
			}
			break;
		case BUTTON_OPEN:  // Opening the file, before of after ciphering, with Windows Notepad
			if (mainModel.getFile() != L"")
				HINSTANCE hInst = ShellExecute(0, L"open", L"c:\\windows\\notepad.exe", mainModel.getFile(), 0, SW_SHOW);
			else
				MessageBox(NULL, L"Filepath cannot be empty", L"Error", MB_ICONERROR | MB_OK);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(mainWindow);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(mainWindow, msg, wParam, lParam);
	}
	return 0;
}