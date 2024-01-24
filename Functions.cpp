// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Functions.h"
#include <assert.h>
#include <dxgi1_6.h>
#pragma comment (lib, "dxgi.lib")

#if (SIMPLE_MODE == 1)
//do it via right-click toggling the option
void DoProcessWork(bool toggle, const std::pair<float, float>& scale_desktop, const POINT& saved_mousePos)
{
	std::pair<LONG, LONG> idxs;
	HWND focus_wnd = GetForegroundWindow();
	INPUT Inputs[3] = { 0 };

	Sleep(SLEEP_INTERVAL_STEP4);

	//right-click at pos [x,y]
	idxs = std::pair<LONG, LONG>(DISPCAL_ICON_LOC);
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = LONG(scale_desktop.first * idxs.first); // desired X coordinate
	Inputs[0].mi.dy = LONG(scale_desktop.second * idxs.second); // desired Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(3, Inputs, sizeof(INPUT));

	Sleep(SLEEP_INTERVAL_STEP4);

	if (toggle)
	{
		WriteErrorMessage("DisplayCal Enabled");
		//click at top option
		idxs = std::pair<LONG, LONG>(DISPCAL_LOAD_CAL_LOC);
		Inputs[0].type = INPUT_MOUSE;
		Inputs[0].mi.dx = LONG(scale_desktop.first * idxs.first); // desired X coordinate
		Inputs[0].mi.dy = LONG(scale_desktop.second * idxs.second); // desired Y coordinate
		Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_MOVE;

		Inputs[1].type = INPUT_MOUSE;
		Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		Inputs[2].type = INPUT_MOUSE;
		Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(3, Inputs, sizeof(INPUT));
	}
	else
	{
		WriteErrorMessage("DisplayCal Disabled");
		//click at second option
		idxs = std::pair<LONG, LONG>(DISPCAL_RESET_TABLE_LOC);
		Inputs[0].type = INPUT_MOUSE;
		Inputs[0].mi.dx = LONG(scale_desktop.first * idxs.first); // desired X coordinate
		Inputs[0].mi.dy = LONG(scale_desktop.second * idxs.second); // desired Y coordinate
		Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_MOVE;

		Inputs[1].type = INPUT_MOUSE;
		Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		Inputs[2].type = INPUT_MOUSE;
		Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(3, Inputs, sizeof(INPUT));
	}

	//reset back to where user was before we interfered
	SetCursorPos(saved_mousePos.x, saved_mousePos.y);
	SetForegroundWindow(focus_wnd);
}
#else
#include <TlHelp32.h>
#include <algorithm>

//do it via scripting
void DoProcessWork(const wchar_t* str, const std::pair<float, float>&, const POINT&)
{
	std::wstring wide(str);
	std::string str1;
	std::transform(wide.begin(), wide.end(), std::back_inserter(str1), [](wchar_t c) {
		return (char)c;
		});
	WriteErrorMessage(str1);

	KillProcess(APP_NAME_EXE);
	Sleep(10);

	remove(path_to_lock_file.c_str());

	startup(SCRIPT_PATH);
	Sleep(SLEEP_INTERVAL_STEP1);

	SendKeystrokesToNotepad(str);
	Sleep(SLEEP_INTERVAL_STEP2);

	KillProcess(APP_NAME_EXE);
}

VOID startup(const char* lpApplicationName)
{
	// additional information
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcessA(lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

DWORD GetProcessIdFromName(LPCTSTR szProcessName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) != FALSE)
		while (Process32Next(snapshot, &entry) != FALSE)
			if (!_wcsicmp(entry.szExeFile, szProcessName))
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
	CloseHandle(snapshot);
	return 0;
}

HWND GetWindowHandle()
{
	HWND handle = FindWindow(NULL, APP_NAME);

	uint32_t waited_ms = 0;
	while (handle == NULL && waited_ms < MAX_WAIT_TIMER)
	{
		Sleep(SLEEP_INTERVAL_STEP3);
		waited_ms += SLEEP_INTERVAL_STEP3;
		handle = FindWindow(NULL, APP_NAME);
	}

	return handle;
}

bool SetHWNDasForeground(HWND wnd)
{
	bool has_been_set = SetForegroundWindow(wnd);

	uint32_t waited_ms = 0;
	while (has_been_set == false && waited_ms < MAX_WAIT_TIMER)
	{
		Sleep(SLEEP_INTERVAL_STEP3);
		waited_ms += SLEEP_INTERVAL_STEP3;
		has_been_set = SetForegroundWindow(wnd);
	}

	return has_been_set;
}

void SendKeystrokesToNotepad(const TCHAR* const text)
{
	INPUT* keystroke;
	size_t i, character_count, keystrokes_to_send;
	HWND notepad;

	assert(text != NULL);

	//Fill in the array of keystrokes to send.
	character_count = _tcslen(text);
	keystrokes_to_send = (character_count + 1) * 2 + 1;
	keystroke = new INPUT[keystrokes_to_send]{};
	for (i = 0; i < character_count; ++i)
	{
		keystroke[i * 2].type = INPUT_KEYBOARD;
		keystroke[i * 2].ki.wVk = 0;
		keystroke[i * 2].ki.wScan = text[i];
		keystroke[i * 2].ki.dwFlags = KEYEVENTF_UNICODE;
		keystroke[i * 2].ki.time = 0;
		keystroke[i * 2].ki.dwExtraInfo = GetMessageExtraInfo();

		keystroke[i * 2 + 1].type = INPUT_KEYBOARD;
		keystroke[i * 2 + 1].ki.wVk = 0;
		keystroke[i * 2 + 1].ki.wScan = text[i];
		keystroke[i * 2 + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
		keystroke[i * 2 + 1].ki.time = 0;
		keystroke[i * 2 + 1].ki.dwExtraInfo = GetMessageExtraInfo();
	}

	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wScan = 0;
	input.ki.dwFlags = 0;

	//Get the handle of the Notepad window.
	notepad = GetWindowHandle();
	if (notepad == NULL)
	{
		WriteErrorMessage("Couldn't find script window");
		return;
	}
	//Bring the Notepad window to the front.
	if (SetHWNDasForeground(notepad))
	{
		//Send the keystrokes.
		SendInput((UINT)keystrokes_to_send, keystroke, sizeof(*keystroke));

		//send enter key
		{
			// Tab Down 
			input.ki.wVk = 0x000D;
			SendInput(1, &input, sizeof(INPUT));
			// Tab Up
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &input, sizeof(INPUT));
		}
	}
	else
		WriteErrorMessage("Couldn't set script window as foreground");

	delete[] keystroke;
}

void KillProcess(LPCTSTR procname)
{
	const auto explorer = OpenProcess(PROCESS_TERMINATE, false, GetProcessIdFromName(procname));
	TerminateProcess(explorer, 1);
	CloseHandle(explorer);
}

std::string path_to_lock_file;
#endif

void WriteErrorMessage(const std::string& err)
{
#if (LOGGING_ENABLED == 1)
	std::fstream my_file;
	my_file.open("errlog.txt", std::ios::app);
	if (!my_file) {
		assert(0);
	}
	else {
		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		char date_string[100];
		strftime(date_string, 50, "%B %d, %Y - %T | ", &newtime);

		my_file << date_string;
		my_file << err.c_str() << "\n";
		my_file.close();
	}
#endif
}

// To detect HDR support, we will need to check the color space in the primary DXGI output associated with the app at
// this point in time (using window/display intersection). 
current_mode GetCurrentMonitorMode(RECT& desktop_size)
{
	HRESULT result;
	DXGI_OUTPUT_DESC1 desc1;

	IDXGIOutput6* output6;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return INVALID;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		factory->Release();
		return INVALID;
	}
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		factory->Release();
		adapter->Release();
		return INVALID;
	}
	result = adapterOutput->QueryInterface(__uuidof(IDXGIOutput6), (void**)&output6);
	if (FAILED(result))
	{
		factory->Release();
		adapter->Release();
		adapterOutput->Release();
		return INVALID;
	}
	result = output6->GetDesc1(&desc1);

	factory->Release();
	adapter->Release();
	adapterOutput->Release();
	output6->Release();

	if (FAILED(result))
		return INVALID;

	desktop_size = desc1.DesktopCoordinates;
	return desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? IN_HDR : IN_SDR;
}
