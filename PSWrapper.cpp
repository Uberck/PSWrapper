#include "framework.h"
#include "PSWrapper.h"
#include <commctrl.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Keolis Script Launcher v1.0";
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDC_PSWRAPPER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PSWRAPPER));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PSWRAPPER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PSWRAPPER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    int winWidth = 240, winHeight = 200;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, style,
        CW_USEDEFAULT, 0, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hComboBox = nullptr;
    static HWND hRunButton = nullptr;
    switch (message)
    {
    case WM_CREATE:
        hComboBox = CreateWindowEx(
            0, WC_COMBOBOX, nullptr,
            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
            10, 10, 200, 100,
            hWnd, (HMENU)1001, hInst, nullptr);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Create User");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Delete Profile");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Find Employee ID");
        hRunButton = CreateWindowEx(
            0, L"BUTTON", L"Run!",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 70, 200, 30,
            hWnd, (HMENU)1002, hInst, nullptr);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case 1002:
        {
            int sel = (int)SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            LPCWSTR scriptName = nullptr;
            switch (sel)
            {
            case 0: scriptName = L"script1.ps1"; break;
            case 1: scriptName = L"script2.ps1"; break;
            case 2: scriptName = L"script3.ps1"; break;
            default:
                MessageBox(hWnd, L"Please select a function from the dropdown.", L"Error", MB_OK | MB_ICONERROR);
                return 0;
            }
            WCHAR cmdLine[512];
            wsprintf(cmdLine, L"powershell.exe -ExecutionPolicy Bypass -File \"%s\"", scriptName);
            STARTUPINFOW si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            BOOL success = CreateProcessW(
                nullptr, cmdLine, nullptr, nullptr, FALSE,
                0, nullptr, nullptr, &si, &pi);
            if (success) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                MessageBox(hWnd, L"Failed to launch PowerShell script.", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}