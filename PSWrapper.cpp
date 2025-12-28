#include "framework.h"
#include "PSWrapper.h"
#include <commctrl.h>
#include <string>
#include <vector>
#include <shellapi.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
std::wstring szTitle = L"Keolis Script Launcher v1.0";
std::wstring szWindowClass;

// Script configuration
const std::vector<ScriptInfo> g_scripts = {
    {L"Create User", IDR_SCRIPT1},
    {L"Delete Profile", IDR_SCRIPT2},
    {L"Find Employee ID", IDR_SCRIPT3}
};

bool ExtractResourceToFile(WORD resourceID, const wchar_t* outPath);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wchar_t szWindowClassBuffer[MAX_LOADSTRING] = {};
    LoadStringW(hInstance, IDC_PSWRAPPER, szWindowClassBuffer, MAX_LOADSTRING);
    szWindowClass = szWindowClassBuffer;
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
    wcex.lpszClassName = szWindowClass.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    int winWidth = 260, winHeight = 230;
    HWND hWnd = CreateWindowW(szWindowClass.c_str(), szTitle.c_str(), style,
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
    static HWND hCheckAdmin = nullptr;
    static HWND hStatusText = nullptr;

    switch (message)
    {
    case WM_CREATE:
        hComboBox = CreateWindowEx(
            0, WC_COMBOBOX, nullptr,
            CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
            10, 10, 220, 100,
            hWnd, (HMENU)IDC_COMBO_SCRIPTS, hInst, nullptr);

        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Select a function...");
        for (const auto& script : g_scripts) {
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)script.displayName.c_str());
        }
        SendMessage(hComboBox, CB_SETCURSEL, 0, 0);

        hCheckAdmin = CreateWindowEx(
            0, L"BUTTON", L"Run as Administrator",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
            10, 50, 220, 20,
            hWnd, (HMENU)IDC_CHECK_ADMIN, hInst, nullptr);

        hRunButton = CreateWindowEx(
            0, L"BUTTON", L"Click to run!",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 80, 220, 35,
            hWnd, (HMENU)IDC_BUTTON_RUN, hInst, nullptr);

        hStatusText = CreateWindowEx(
            0, L"STATIC", L"Ready",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 125, 220, 20,
            hWnd, (HMENU)IDC_STATIC_STATUS, hInst, nullptr);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON_RUN:
            ExecuteScript(hWnd, hComboBox, hCheckAdmin);
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

void SetExecutionState(HWND hWnd, bool isExecuting)
{
    HWND hButton = GetDlgItem(hWnd, IDC_BUTTON_RUN);
    HWND hCombo = GetDlgItem(hWnd, IDC_COMBO_SCRIPTS);
    HWND hCheck = GetDlgItem(hWnd, IDC_CHECK_ADMIN);
    HWND hStatus = GetDlgItem(hWnd, IDC_STATIC_STATUS);

    EnableWindow(hButton, !isExecuting);
    EnableWindow(hCombo, !isExecuting);
    EnableWindow(hCheck, !isExecuting);

    if (isExecuting) {
        SetWindowText(hStatus, L"Running script...");
        SetWindowText(hWnd, (szTitle + L" - Running...").c_str());
    }
    else {
        SetWindowText(hStatus, L"Ready");
        SetWindowText(hWnd, szTitle.c_str());
    }
}

void ExecuteScript(HWND hWnd, HWND hComboBox, HWND hCheckAdmin)
{
    SetExecutionState(hWnd, true);

    int sel = static_cast<int>(SendMessage(hComboBox, CB_GETCURSEL, 0, 0));

    // sel=0 is "Select a function...", actual scripts start at index 1
    if (sel == 0 || sel == CB_ERR || sel > static_cast<int>(g_scripts.size())) {
        MessageBox(hWnd, L"Please select a function from the dropdown.", L"Error", MB_OK | MB_ICONERROR);
        SetExecutionState(hWnd, false);
        return;
    }

    // Get the script info (index is 1-based in combobox, 0-based in vector)
    const ScriptInfo& scriptInfo = g_scripts[sel - 1];

    // Get temp path using a mutable buffer
    std::vector<wchar_t> tempPathBuffer(MAX_PATH);
    GetTempPathW(MAX_PATH, tempPathBuffer.data());
    std::wstring tempPath(tempPathBuffer.data());

    // Build temp file path
    std::wstring tempFile = tempPath + L"PSWScript" + std::to_wstring(scriptInfo.resourceId) + L".ps1";

    if (!ExtractResourceToFile(scriptInfo.resourceId, tempFile.c_str())) {
        MessageBox(hWnd, L"Failed to extract script.", L"Error", MB_OK | MB_ICONERROR);
        SetExecutionState(hWnd, false);
        return;
    }

    // Check if "Run as Administrator" is checked
    LRESULT checkState = SendMessage(hCheckAdmin, BM_GETCHECK, 0, 0);
    bool runAsAdmin = (checkState == BST_CHECKED);

    if (runAsAdmin) {
        // Use ShellExecuteEx for elevation
        std::wstring params = L"-ExecutionPolicy Bypass -File \"" + tempFile + L"\"";

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.lpVerb = L"runas";
        sei.lpFile = L"powershell.exe";
        sei.lpParameters = params.c_str();
        sei.nShow = SW_SHOW;

        if (ShellExecuteExW(&sei)) {
            if (sei.hProcess) {
                CloseHandle(sei.hProcess);
            }
            SetExecutionState(hWnd, false);
        }
        else {
            DWORD error = GetLastError();
            if (error != ERROR_CANCELLED) { // User cancelled UAC prompt
                MessageBox(hWnd, L"Failed to launch PowerShell with elevation.", L"Error", MB_OK | MB_ICONERROR);
            }
            SetExecutionState(hWnd, false);
        }
    }
    else {
        // Use CreateProcess for normal execution
        std::wstring cmdLine = L"powershell.exe -ExecutionPolicy Bypass -File \"" + tempFile + L"\"";

        // CreateProcessW requires a non-const buffer
        std::vector<wchar_t> cmdLineBuffer(cmdLine.begin(), cmdLine.end());
        cmdLineBuffer.push_back(L'\0');

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};

        BOOL success = CreateProcessW(
            nullptr, cmdLineBuffer.data(), nullptr, nullptr, FALSE,
            0, nullptr, nullptr, &si, &pi);

        if (success) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            SetExecutionState(hWnd, false);
        }
        else {
            MessageBox(hWnd, L"Failed to launch PowerShell script.", L"Error", MB_OK | MB_ICONERROR);
            SetExecutionState(hWnd, false);
        }
    }
}

bool ExtractResourceToFile(WORD resourceID, const wchar_t* outPath) {
    HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (!hRes) return false;
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) return false;
    DWORD dataSize = SizeofResource(NULL, hRes);
    void* pData = LockResource(hData);
    if (!pData) return false;

    // Use Windows API for file creation (C++14 compatible)
    HANDLE hFile = CreateFileW(outPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD bytesWritten = 0;
    BOOL writeSuccess = WriteFile(hFile, pData, dataSize, &bytesWritten, NULL);
    CloseHandle(hFile);

    return writeSuccess && (bytesWritten == dataSize);
}