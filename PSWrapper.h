#pragma once

#include "resource.h"
#include <windows.h>

extern HINSTANCE hInst;
extern WCHAR szTitle[];
extern WCHAR szWindowClass[];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
