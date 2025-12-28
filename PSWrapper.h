#pragma once

#include "resource.h"
#include <windows.h>
#include <string>

extern HINSTANCE hInst;
extern std::wstring szTitle;
extern std::wstring szWindowClass;
extern HBRUSH hBrushBlack;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool                ExtractResourceToFile(WORD resourceID, const wchar_t* outPath);