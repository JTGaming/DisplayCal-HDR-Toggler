// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "definitions.h"
#include "Functions.h"
#include <ShlObj_core.h>

INT __stdcall WinMain(HINSTANCE, HINSTANCE,
	PSTR, INT)
{
	SetProcessDPIAware();
	WriteErrorMessage("App launched");

#if (SIMPLE_MODE != 1)
	char path[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		WriteErrorMessage("Failed to get appdata folder path");
		return 1;
	}

	KillProcess(APP2_NAME_EXE);
	Sleep(100);
	startup(SCRIPT2_PATH);
	Sleep(100);

	path_to_lock_file = path;
	path_to_lock_file += "\\DisplayCAL\\DisplayCAL-scripting-client.lock";
#endif

	current_mode previous_mode, mode;
	RECT desktop_size{};
	previous_mode = GetCurrentMonitorMode(desktop_size);

	//sendinput absolute position is normalized from 0 to 65535
	//so we have to do the same if we want to click on a specific pixel coord
	// 
	//doing this outside the loop since if monitor configuration changes,
	//the displaycal button coordinates would fail anyways
	std::pair<float, float> scale_desktop;
	int vScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int vScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	scale_desktop.first = 65535.f / vScreenWidth;
	scale_desktop.second = 65535.f / vScreenHeight;

	while(true)
	{
		//win+alt+b HDR toggle resets the cursor to the middle of the screen
		//this way, we can save the position of the mouse cursor prior to the reset
		static POINT saved_mousePos{};
		POINT mousePos;
		GetCursorPos(&mousePos);
		if (mousePos.x != (desktop_size.right + desktop_size.left) / 2 && mousePos.y != (desktop_size.bottom + desktop_size.top) / 2)
			saved_mousePos = mousePos;

		mode = GetCurrentMonitorMode(desktop_size);

		if (mode == INVALID)
			WriteErrorMessage("Failed color space check");

		if (previous_mode != IN_SDR && mode == IN_SDR)
			DoProcessWork(ENABLE_DISPCAL, scale_desktop, saved_mousePos);
		else if (previous_mode != IN_HDR && mode == IN_HDR)
			DoProcessWork(DISABLE_DISPCAL, scale_desktop, saved_mousePos);

		if (mode != INVALID)
			previous_mode = mode;
		Sleep(SLEEP_INTERVAL);
	}
}