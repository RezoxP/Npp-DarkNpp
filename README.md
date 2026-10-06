# Npp-Mica

[![Build status](https://img.shields.io/github/actions/workflow/status/RezoxP/Npp-Mica/build.yml?branch=master&logo=Github)](https://github.com/RezoxP/Npp-Mica)
[![Latest release](https://img.shields.io/github/v/release/RezoxP/Npp-Mica?include_prereleases)](https://github.com/RezoxP/Npp-Mica/releases/latest)
[![Total downloads](https://img.shields.io/github/downloads/RezoxP/Npp-Mica/total.svg)](https://github.com/RezoxP/Npp-Mica/releases)
[![License](https://img.shields.io/github/license/RezoxP/Npp-Mica?color=9cf)](https://www.gnu.org/licenses/gpl-3.0.en.html)

**Npp-Mica** is a modern [Notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus) plugin that brings native Windows 11 backdrop materials (**Mica**, **Mica Acrylic**, **Mica Alternative**, and **Fluent Acrylic**) to Notepad++, along with immersive dark mode title bar styling, context menus, and scrollbars.

> [!NOTE]
> **Fork Information & Attribution**:
> This project is a modernized fork of [ozone10/Npp-DarkNpp](https://github.com/ozone10/Npp-DarkNpp). While the original DarkNpp plugin introduced dark styling before Notepad++ v8.0, this fork focuses on bringing native **Windows 11 Fluent Design backdrops (Mica & Acrylic)**, comprehensive bug fixes for dynamic DWM backdrop switching, and optimized Scintilla client-area rendering (eliminating emoji rendering artifacts, text selection glitches, and high CPU/RAM overhead on large documents).

---

## ⚠️ Beta Notice & Reporting Issues

> [!WARNING]
> **Npp-Mica is currently in beta.**
> Because Desktop Window Manager (DWM) composition and backdrop effects interact directly with Windows rendering pipelines, you may encounter visual glitches or edge cases depending on your graphics hardware, Windows build, or display configuration.

If you encounter any bugs, crashes, or rendering artifacts, please [**Open a GitHub Issue**](https://github.com/RezoxP/Npp-Mica/issues)!

### What to include when reporting an issue:
To help us diagnose and fix problems quickly, please include:
1. **Windows version & build number** (Press `Win + R`, run `winver`, e.g. *Windows 11 23H2 Build 22631*).
2. **Notepad++ Debug Info** (In Notepad++, click `?` -> `Debug Info...` and copy the text).
3. **Architecture** (64-bit `x64` or 32-bit `x86`).
4. **Active Backdrop Effect** (`Auto`, `None`, `Mica`, `Mica Acrylic`, `Mica Alternative`, or `Acrylic`).
5. **Display Settings**: Whether **HDR** or **Auto Color Management (ACM)** is enabled in *Windows Settings -> Display*.
6. **Screenshots or screen recordings** showing the visual anomaly.

---

## 📦 Installation Guide

Npp-Mica is available for both **64-bit (x64)** and **32-bit (x86)** Notepad++.

### Step 1: Check your Notepad++ Architecture
1. Launch Notepad++.
2. Click **`?`** in the top menu -> **`About Notepad++`** (or press `F1`).
3. Note whether it says **64-bit** or **32-bit**.

---

### Step 2: Download and Install

#### Option A: ZIP Archive (Recommended)
1. Head to the [**Releases Page**](https://github.com/RezoxP/Npp-Mica/releases) and download the matching archive:
   - For 64-bit Notepad++: `DarkNpp-x64.zip`
   - For 32-bit Notepad++: `DarkNpp-Win32.zip`
2. In Notepad++, click **`Plugins`** -> **`Open Plugins Folder...`**.
   - Default 64-bit path: `C:\Program Files\Notepad++\plugins\`
   - Default 32-bit path: `C:\Program Files (x86)\Notepad++\plugins\`
3. Extract the contents of the ZIP archive directly into your `plugins\` folder.
   - It will create a folder named `DarkNpp` containing `DarkNpp.dll`.
   - Your final folder structure should look like:
     ```text
     Notepad++\
     └── plugins\
         └── DarkNpp\
             └── DarkNpp.dll
     ```
4. Restart Notepad++.

#### Option B: Direct DLL Download
1. Download `DarkNpp-x64.dll` (for 64-bit) or `DarkNpp-Win32.dll` (for 32-bit) from [Releases](https://github.com/RezoxP/Npp-Mica/releases).
2. Rename the downloaded file to **`DarkNpp.dll`**.
3. Open your Notepad++ `plugins\` folder (`Plugins` -> `Open Plugins Folder...`).
4. Create a new folder named **`DarkNpp`**.
5. Move `DarkNpp.dll` into `plugins\DarkNpp\`.
6. Restart Notepad++.

---

## 🎨 Features & Effect Modes

You can switch effects directly from the top menu via **`Plugins` -> `DarkNpp`**, or configure them in `%APPDATA%\Notepad++\plugins\config\DarkNpp.ini`.

| Option | Setting | Description |
| :--- | :---: | :--- |
| **Auto** | `micaType=0` | System default — applies standard Mica material to the title bar only. |
| **None** | `micaType=1` | Disables all backdrop materials on the main window. |
| **Mica** | `micaType=2` | Full-window Windows 11 Mica material matching your desktop wallpaper. |
| **Mica Acrylic** | `micaType=3` | Windows 11 DWM transient backdrop (`DWMSBT_TRANSIENTWINDOW`). |
| **Mica Alternative** | `micaType=4` | Windows 11 Mica Alt material (`DWMSBT_TABBEDWINDOW`), subtle variation designed for tabbed apps. |
| **Acrylic** | `micaType=5` | Fluent Acrylic blur-behind effect with noise texture and tint (`SetWindowCompositionAttribute`). |

### Additional Settings
- **`useDark`**:
  - `1` - Enable dark mode styling (recommended for Mica and Acrylic effects).
  - `0` - Light mode styling.

---

## ⚡ Performance & Stability

- **Optimized Scintilla Rendering**: Double-buffered drawing (`SCI_SETBUFFEREDDRAW`) and Direct2D retention are enabled to eliminate emoji rendering glitches, text artifacts, and high CPU/RAM usage when handling large files.
- **Zero Overhead**: Backdrop effects are handled directly by the Windows Desktop Window Manager (DWM) composition engine with hardware acceleration.

> [!IMPORTANT]
> - With `micaType` set to values other than `0`, avoid enabling Windows HDR and Auto Color Management (ACM) on older Windows 11 builds, as DWM may produce transparency composition artifacts.
> - For the cleanest look, navigate to Windows *Settings -> Personalization -> Colors* and disable *"Show accent color on title bars and window borders"*.

---

## 🛠️ Building from Source

Npp-Mica is compiled using Visual Studio 2022 (MSVC v143) with the Windows 11 SDK. Continuous integration and automated binary releases can be dispatched via GitHub Actions (`.github/workflows/build.yml`).

To build locally:
```powershell
# Open DarkNpp.sln in Visual Studio 2022
# Select Release | x64 or Release | Win32
msbuild DarkNpp.sln /p:Configuration=Release /p:Platform=x64
msbuild DarkNpp.sln /p:Configuration=Release /p:Platform=Win32
```

---

## 📄 License & Credits

- Original DarkNpp plugin developed by [ozone10](https://github.com/ozone10/Npp-DarkNpp).
- Npp-Mica enhancements, Windows 11 backdrop extensions, and Scintilla rendering optimizations maintained by [RezoxP](https://github.com/RezoxP/Npp-Mica).
- Licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
