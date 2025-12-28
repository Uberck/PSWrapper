# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PSWrapper (Keolis Script Launcher) is a lightweight Windows desktop application written in C++ using the Win32 API. It provides a GUI for launching PowerShell scripts embedded as resources within the executable.

## Build System

This project uses Visual Studio 2022+ with MSBuild. The project is configured with:
- Platform Toolset: v145 (Visual Studio 2026 target)
- Windows SDK: 10.0
- Character Set: Unicode
- Runtime Library (Release x64): MultiThreaded (static linking)

### Building

Open `PSWrapper.sln` in Visual Studio and build using the IDE, or use MSBuild from command line:

```cmd
msbuild PSWrapper.sln /p:Configuration=Release /p:Platform=x64
```

**Important:** Release builds use static runtime linking (`RuntimeLibrary=MultiThreaded`) to produce standalone executables without CRT dependencies.

## Architecture

### Resource-Based Script Deployment

Scripts are **not** deployed alongside the executable. Instead, they are embedded directly into the executable as RCDATA resources at build time:

1. PowerShell scripts (script1.ps1, script2.ps1, script3.ps1) are defined in `PSWrapper.rc` with resource IDs (IDR_SCRIPT1, IDR_SCRIPT2, IDR_SCRIPT3)
2. At runtime, `ExtractResourceToFile()` extracts the selected script to a temporary file
3. The script is executed via `CreateProcessW()` with PowerShell using `-ExecutionPolicy Bypass`
4. Temporary script files are placed in the system temp directory with names like `PSWScript101.ps1`

This architecture ensures scripts cannot be modified by end users and are bundled with the executable.

### Adding New Scripts

To add or modify scripts:

1. Add the script resource ID to `Resource.h` (e.g., `#define IDR_SCRIPT4 104`)
2. Add the script file to `PSWrapper.rc` (e.g., `IDR_SCRIPT4 RCDATA "script4.ps1"`)
3. Update `WndProc()` in `PSWrapper.cpp`:
   - Add ComboBox item: `SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"New Function Name");`
   - Add case in the switch statement to map selection index to resource ID
4. Ensure the script file exists in the root directory at build time

### Window Architecture

The application uses a standard Win32 message loop with:
- Fixed-size window (240x200) with non-resizable style (`WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU`)
- ComboBox control (ID 1001) for script selection
- Button control (ID 1002) for execution
- Menu with About dialog (accessible via Alt+? or Alt+/)

All UI creation happens in `WM_CREATE` message handler within `WndProc()`.

## Key Files

- `PSWrapper.cpp` - Main application logic, window procedure, script extraction and execution
- `PSWrapper.h` - Function declarations and global variable exports
- `PSWrapper.rc` - Win32 resources (menu, dialog, icons, embedded scripts)
- `Resource.h` - Resource ID definitions (controls, dialogs, script resource IDs)
- `framework.h` - Precompiled header for Windows headers
- `targetver.h` - Windows SDK version targeting

## Code Patterns

### Script Execution Flow

```
User selects item → WM_COMMAND with ID 1002 →
Get ComboBox selection → Map to resource ID →
ExtractResourceToFile() → Build PowerShell command line →
CreateProcessW() → Launch script
```

### Resource Extraction

The `ExtractResourceToFile()` function uses:
- `FindResource()` to locate RCDATA resource
- `LoadResource()` and `LockResource()` to access data
- `std::ofstream` to write binary data to temp file

### Error Handling

User-facing errors are shown via `MessageBox()` with MB_ICONERROR. Common error scenarios:
- No function selected from dropdown
- Resource extraction failure
- Process creation failure

## Development Notes

- The application title includes version number: "Keolis Script Launcher v1.0" (PSWrapper.cpp:10)
- All strings use wide character format (WCHAR, std::wstring) due to Unicode character set
- The project includes both old Visual Studio version artifacts (PSWrapper_old directory) which can be ignored
