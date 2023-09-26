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
	previous_mode = GetCurrentMonitorMode();

	while(true)
	{
		mode = GetCurrentMonitorMode();

		if (mode == INVALID)
			WriteErrorMessage("Failed color space check");

		if (previous_mode != IN_SDR && mode == IN_SDR)
			DoProcessWork(ENABLE_DISPCAL);
		else if (previous_mode != IN_HDR && mode == IN_HDR)
			DoProcessWork(DISABLE_DISPCAL);

		if (mode != INVALID)
			previous_mode = mode;
		Sleep(SLEEP_INTERVAL);
	}
}