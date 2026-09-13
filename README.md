# RoundButton MFC

C++ MFC port of the [DevInDeep/RoundButton](https://github.com/DevInDeep/RoundButton) C# WinForms custom control.

## Features

- Rounded rectangle button with configurable corner radius
- Custom background color, border color and border width
- Optional mouse-over background color change
- Owner-draw using GDI+ for anti-aliased rendering
- Click handling and text display with automatic contrast

## Project Structure

```
RoundButtonMFC/
├── RoundButton.h / .cpp     # The custom CRoundButton control
├── RoundButtonDlg.h / .cpp  # Demo dialog
├── RoundButtonApp.h / .cpp  # Application entry
├── stdafx.h / .cpp          # Precompiled header
├── targetver.h
├── resource.h
├── RoundButton.rc
└── README.md
```

## How to Build (Visual Studio)

1. Open Visual Studio 2019/2022.
2. Create a new project: **MFC App** → Application type: **Dialog based** → uncheck "Use Unicode libraries" if you prefer ANSI (or keep Unicode).
3. Replace the generated dialog/app files with the ones from this repository (or add the source files to your project).
4. Add `RoundButton.h` and `RoundButton.cpp` to the project.
5. Make sure **GDI+** is linked (`gdiplus.lib` – already pragma'd in the header).
6. In the dialog resource, place an owner-draw Button control with ID `IDC_ROUNDBUTTON` and a Static control with ID `IDC_STATUS`.
7. In `DoDataExchange` subclass the button as shown.
8. Build and run.

Alternatively you can start from an empty MFC dialog project and drop these files in.

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

## Notes

- Requires Windows SDK with GDI+ (standard on modern VS).
- The original C# control embedded a flat Button inside a UserControl; this port uses a pure owner-draw `CButton` for simplicity and better performance.
- Mouse tracking uses `TrackMouseEvent` for enter/leave.
- Text color automatically chooses black or white based on background luminance.

## Original Project

Ported from: https://github.com/DevInDeep/RoundButton
