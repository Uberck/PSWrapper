#pragma once

#include "resource.h"
#include <windows.h>
#include <string>
#include <vector>

// Script information structure
struct ScriptInfo {
    std::wstring displayName;
    WORD resourceId;
};

extern HINSTANCE hInst;
extern std::wstring szTitle;
extern std::wstring szWindowClass;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool                ExtractResourceToFile(WORD resourceID, const wchar_t* outPath);
void                ExecuteScript(HWND hWnd, HWND hComboBox, HWND hCheckAdmin);
void                SetExecutionState(HWND hWnd, bool isExecuting);