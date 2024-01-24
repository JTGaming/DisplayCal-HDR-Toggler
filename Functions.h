#pragma once
#include "definitions.h"
#include <Windows.h>

current_mode GetCurrentMonitorMode(RECT&);
void WriteErrorMessage(const std::string& err);

#if (SIMPLE_MODE == 1)
void DoProcessWork(bool toggle, const std::pair<float, float>&, const POINT&);
#else
VOID startup(const char* lpApplicationName);
DWORD GetProcessIdFromName(LPCTSTR szProcessName);
void SendKeystrokesToNotepad(const TCHAR* const text);
void KillProcess(LPCTSTR procname);
void DoProcessWork(const wchar_t* str, std::pair<float, float>, const POINT&);
#endif
