#pragma once
#include <fstream>

//simple mode 1 - clicks on the taskbar icon and toggles the modes manually
//simple mode 0 - uses the API to toggle modes (displaycal API is broken currently)
#define SIMPLE_MODE 1

//simple logging of when modes change
#define LOGGING_ENABLED 1

#if (SIMPLE_MODE == 1)
#define ENABLE_DISPCAL true
#define DISABLE_DISPCAL false
#define SLEEP_INTERVAL_STEP4 500 //in ms
#define SLEEP_INTERVAL_STEP5 50 //in ms

//these are screen coordinates, for the displaycal taskbar icons
//customize for each use case
#define DISPCAL_ICON_LOC 4915, 1415
#define DISPCAL_LOAD_CAL_LOC 4915, 1080
#define DISPCAL_RESET_TABLE_LOC 4915, 1110
#else
#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

#define ENABLE_DISPCAL _T("apply-profiles")
#define DISABLE_DISPCAL _T("reset-vcgt")
#define SLEEP_INTERVAL_STEP1 500 //in ms
#define SLEEP_INTERVAL_STEP2 750 //in ms
#define SLEEP_INTERVAL_STEP3 50 //in ms

#define SCRIPT_PATH ("C:\\Program Files (x86)\\DisplayCAL\\DisplayCAL-scripting-client.exe")
#define SCRIPT2_PATH ("C:\\Program Files (x86)\\DisplayCAL\\DisplayCAL-apply-profiles.exe")
#define APP_NAME _T("DisplayCAL Scripting Client")
#define APP_NAME_EXE _T("DisplayCAL-scripting-client.exe")
#define APP2_NAME_EXE _T("DisplayCAL-apply-profiles.exe")

extern std::string path_to_lock_file;
#endif

#define SLEEP_INTERVAL 2000 //in ms
#define MAX_WAIT_TIMER (10 * 1000) //in ms

enum current_mode
{
	INVALID = 0,
	IN_SDR,
	IN_HDR
};
