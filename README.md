# RoundButton MFC

C++ MFC port of the [DevInDeep/RoundButton](https://github.com/DevInDeep/RoundButton) C# WinForms custom control.

## Features

- Rounded rectangle button with configurable corner radius
- Custom background color, border color and border width
- Optional mouse-over background color change
- Owner-draw using GDI+ for anti-aliased rendering
- Click handling and text display with automatic contrast (black/white)

## Quick Start

1. Clone the repository:
   ```bash
   git clone https://github.com/trinityplay2017/CSharpPortToCPlusplus.git
   cd CSharpPortToCPlusplus
   ```
2. Open **`RoundButton.sln`** in Visual Studio 2019 or 2022.
3. Select **Debug | x64** (or Win32) and press **F5**.

Requirements:
- Visual Studio with **Desktop development with C++** workload
- **MFC and ATL** support installed (Individual components → “MFC and ATL”)
- Windows 10 SDK

## Project Structure

```
├── RoundButton.sln              # Visual Studio solution
├── RoundButton.vcxproj          # Project file
├── RoundButton.vcxproj.filters
├── RoundButton.h / .cpp         # CRoundButton custom control
├── RoundButtonDlg.h / .cpp      # Demo dialog
├── RoundButtonApp.h / .cpp      # Application entry point
├── RoundButton.rc               # Dialog resource
├── resource.h
├── stdafx.h / .cpp              # Precompiled header
├── targetver.h
├── res/
│   └── RoundButton.rc2
└── README.md
```

## Usage Example

```cpp
// In your dialog header
CRoundButton m_btn;

// In DoDataExchange
DDX_Control(pDX, IDC_MYBUTTON, m_btn);

// In OnInitDialog
m_btn.SetRadius(20);
m_btn.SetBackgroundColor(RGB(11, 205, 255));
m_btn.SetBorderColor(RGB(255, 255, 255));
m_btn.SetBorderWidth(7.0f);
m_btn.SetUseMouseOverBackColor(true);
m_btn.SetMouseOverBackColor(RGB(180, 180, 180));
m_btn.SetWindowText(_T("Say Hi"));
```

## Public API

| Method | Description |
|--------|-------------|
| `SetRadius(int)` | Corner radius in pixels |
| `SetBackgroundColor(COLORREF)` | Fill color |
| `SetBorderColor(COLORREF)` | Border color |
| `SetBorderWidth(float)` | Border thickness |
| `SetUseMouseOverBackColor(bool)` | Enable hover color change |
| `SetMouseOverBackColor(COLORREF)` | Hover fill color |
| `SetWindowText` / `GetWindowText` | Button caption |

## Notes

- Uses pure owner-draw `CButton` + GDI+ (no embedded child controls).
- Mouse enter/leave via `TrackMouseEvent`.
- Text color is chosen automatically from background luminance.
- Toolset is set to **v143** (VS 2022). For VS 2019 change Platform Toolset to **v142** in project properties.

## Original Project

Ported from: https://github.com/DevInDeep/RoundButton
