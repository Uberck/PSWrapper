# Keolis Script Launcher (PSWrapper)

**Keolis Script Launcher (PSWrapper)** is a lightweight Windows desktop application for launching PowerShell scripts via a simple GUI. It is written in C++ using the Win32 API.

## Features

- Dropdown menu to select from multiple PowerShell scripts (e.g., Create User, Delete Profile, Find Employee ID)
- "Run!" button to execute the selected script using PowerShell
- Minimal, fixed-size window
- Scripts are expected to be deployed alongside the executable

## Usage

1. Build the project in Visual Studio 2022 (C++14).
2. Place your PowerShell scripts (`script1.ps1`, `script2.ps1`, `script3.ps1`) in the same directory as the executable.
3. Run the application.
4. Select a function from the dropdown and click "Run!" to execute the corresponding script.

## Building

- Open the solution in Visual Studio 2022.
- Build the project (no special dependencies beyond the Windows SDK).

## Customization

- To change the script names or add more scripts, edit the ComboBox items and script file mappings in `WndProc` in `PSWrapper.cpp`.

## License

MIT License
