#pragma once
#include "definitions.h"

current_mode GetCurrentMonitorMode();
void WriteErrorMessage(const std::string& err);

#if (SIMPLE_MODE == 1)
void DoProcessWork(bool toggle);
#else
VOID startup(const char* lpApplicationName);
DWORD GetProcessIdFromName(LPCTSTR szProcessName);
void SendKeystrokesToNotepad(const TCHAR* const text);
void KillProcess(LPCTSTR procname);
void DoProcessWork(const wchar_t* str);
#endif
