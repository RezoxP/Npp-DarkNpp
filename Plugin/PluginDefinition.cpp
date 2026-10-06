/*
  Copyright (C) 2020-2025 oZone10
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "PluginDefinition.h"

FuncItem funcItem[nbFunc];
NppData nppData;

wchar_t iniFilePath[MAX_PATH] = { '\0' };
const wchar_t sectionName[] = L"DarkNpp";

static bool enableDark = false;

int micaType = 0;
int effectIntensity = 75;

constexpr int menuItemEnableDark = 0;
constexpr int menuItemMica = menuItemEnableDark + 3;
constexpr int menuItemIntensity = menuItemMica + 7;
constexpr int menuItemAbout = menuItemIntensity + 6;

constexpr size_t classNameLenght = 64;

void PluginInit()
{
    LoadSettings();
    CommandMenuInit();
    SetDarkNpp();
    SetMicaNpp();
    ConfigureAllScintillaViews();
}

void CommandMenuInit()
{
    funcItem[menuItemEnableDark + 0] = { L"Enable Dark Mode", DarkCheckTag, 0, enableDark, nullptr };
    funcItem[menuItemEnableDark + 1] = { L"Refresh Dark Mode", SetDarkNpp, 0, false, nullptr };
    funcItem[menuItemEnableDark + 2] = { L"---", nullptr, 0, false, nullptr };
    funcItem[menuItemMica + 0] = { L"Auto", SetMicaTagAuto, 0, micaType == 0, nullptr };
    funcItem[menuItemMica + 1] = { L"None", SetMicaTagNone, 0, micaType == 1, nullptr };
    funcItem[menuItemMica + 2] = { L"Mica", SetMicaTagMica, 0, micaType == 2, nullptr };
    funcItem[menuItemMica + 3] = { L"Mica Acrylic", SetMicaTagAcrylic, 0, micaType == 3, nullptr };
    funcItem[menuItemMica + 4] = { L"Mica Alternative", SetMicaTagTabbed, 0, micaType == 4, nullptr };
    funcItem[menuItemMica + 5] = { L"Acrylic", SetTagAcrylic, 0, micaType == 5, nullptr };
    funcItem[menuItemMica + 6] = { L"---", nullptr, 0, false, nullptr };
    funcItem[menuItemIntensity + 0] = { L"Intensity: 100% (High)", SetIntensity100, 0, effectIntensity >= 90, nullptr };
    funcItem[menuItemIntensity + 1] = { L"Intensity: 75% (Medium)", SetIntensity75, 0, effectIntensity >= 65 && effectIntensity < 90, nullptr };
    funcItem[menuItemIntensity + 2] = { L"Intensity: 50% (Low)", SetIntensity50, 0, effectIntensity >= 40 && effectIntensity < 65, nullptr };
    funcItem[menuItemIntensity + 3] = { L"Intensity: 25% (Subtle)", SetIntensity25, 0, effectIntensity < 40, nullptr };
    funcItem[menuItemIntensity + 4] = { L"Customize Intensity...", ShowIntensityDialog, 0, false, nullptr };
    funcItem[menuItemIntensity + 5] = { L"---", nullptr, 0, false, nullptr };
    funcItem[menuItemAbout] = { L"&About...", About, 0, false, nullptr };
}

void LoadSettings()
{
    ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(iniFilePath));
    ::PathAppend(iniFilePath, L"\\DarkNpp.ini");

    enableDark = ::GetPrivateProfileInt(sectionName, L"useDark", 1, iniFilePath) != 0;

    micaType = ::GetPrivateProfileInt(sectionName, L"micaType", 0, iniFilePath);

    effectIntensity = ::GetPrivateProfileInt(sectionName, L"effectIntensity", 75, iniFilePath);
    if (effectIntensity < 10) effectIntensity = 10;
    if (effectIntensity > 100) effectIntensity = 100;
}

void SavePluginParams()
{
    funcItem[menuItemEnableDark]._init2Check = enableDark;
    ::WritePrivateProfileString(sectionName, L"useDark", enableDark ? L"1" : L"0", iniFilePath);
    for (int i = 0; i < 6; i++)
    {
        funcItem[menuItemMica + i]._init2Check = (micaType == i);
    }
    ::WritePrivateProfileString(sectionName, L"micaType", std::to_wstring(micaType).c_str(), iniFilePath);

    funcItem[menuItemIntensity + 0]._init2Check = (effectIntensity >= 90);
    funcItem[menuItemIntensity + 1]._init2Check = (effectIntensity >= 65 && effectIntensity < 90);
    funcItem[menuItemIntensity + 2]._init2Check = (effectIntensity >= 40 && effectIntensity < 65);
    funcItem[menuItemIntensity + 3]._init2Check = (effectIntensity < 40);
    ::WritePrivateProfileString(sectionName, L"effectIntensity", std::to_wstring(effectIntensity).c_str(), iniFilePath);
}

void DarkCheckTag()
{
    enableDark = !enableDark;
    ::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[menuItemEnableDark]._cmdID, MF_BYCOMMAND | (enableDark ? MF_CHECKED : MF_UNCHECKED));
    SetDarkNpp();
    SavePluginParams();
}

void SetMicaTagAuto()
{
    micaType = 0;
    MicaCheckTag();
}

void SetMicaTagNone()
{
    micaType = 1;
    MicaCheckTag();
}

void SetMicaTagMica()
{
    micaType = 2;
    MicaCheckTag();
}

void SetMicaTagAcrylic()
{
    micaType = 3;
    MicaCheckTag();
}

void SetMicaTagTabbed()
{
    micaType = 4;
    MicaCheckTag();
}

void SetTagAcrylic()
{
    micaType = 5;
    MicaCheckTag();
}

void MicaCheckTag()
{
    const auto hMenu = ::GetMenu(nppData._nppHandle);

    for (int i = 0; i < 6; i++)
    {
        ::CheckMenuItem(hMenu, funcItem[menuItemMica + i]._cmdID, MF_BYCOMMAND | (micaType == i ? MF_CHECKED : MF_UNCHECKED));
    }

    SetMicaNpp();
    SavePluginParams();
}

void SetIntensity100()
{
    effectIntensity = 100;
    IntensityCheckTag();
}

void SetIntensity75()
{
    effectIntensity = 75;
    IntensityCheckTag();
}

void SetIntensity50()
{
    effectIntensity = 50;
    IntensityCheckTag();
}

void SetIntensity25()
{
    effectIntensity = 25;
    IntensityCheckTag();
}

void IntensityCheckTag()
{
    const auto hMenu = ::GetMenu(nppData._nppHandle);
    if (hMenu)
    {
        ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 0]._cmdID, MF_BYCOMMAND | (effectIntensity >= 90 ? MF_CHECKED : MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 1]._cmdID, MF_BYCOMMAND | (effectIntensity >= 65 && effectIntensity < 90 ? MF_CHECKED : MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 2]._cmdID, MF_BYCOMMAND | (effectIntensity >= 40 && effectIntensity < 65 ? MF_CHECKED : MF_UNCHECKED));
        ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 3]._cmdID, MF_BYCOMMAND | (effectIntensity < 40 ? MF_CHECKED : MF_UNCHECKED));
    }

    SetMicaNpp();
    SavePluginParams();
}

void About()
{
    ::MessageBox(
        NULL,
        L"This is Dark mode & Mica effects Notepad++ test.\n"
        L"Plugin is using undocumented WINAPI.\n"
        L"@2020-2025 by oZone10",
        L"About",
        MB_OK);
}

bool IsAtLeastWin10Build(DWORD buildNumber)
{
    if (!::IsWindows10OrGreater())
    {
        return false;
    }

    const auto mask = ::VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL);

    OSVERSIONINFOEXW osvi{};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwBuildNumber = buildNumber;
    return VerifyVersionInfo(&osvi, VER_BUILDNUMBER, mask) != FALSE;
}

void SetMode(HMODULE hUxtheme)
{
    const auto ord135 = ::GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));

    if (IsAtLeastWin10Build(VER_1903))
    {
        using SPAM = PreferredAppMode(WINAPI*)(PreferredAppMode appMode);
        const auto _SetPreferredAppMode = reinterpret_cast<SPAM>(ord135);

        auto appMode = enableDark ? PreferredAppMode::ForceDark : PreferredAppMode::ForceLight;

        if (_SetPreferredAppMode != nullptr)
        {
            _SetPreferredAppMode(appMode);
        }
    }
    else
    {
        using ADMFA = bool (WINAPI*)(bool allow);
        const auto _AllowDarkModeForApp = reinterpret_cast<ADMFA>(ord135);

        if (_AllowDarkModeForApp != nullptr)
        {
            _AllowDarkModeForApp(true);
        }
    }
}

void SetTheme(HWND hWnd)
{
    const auto hUxtheme = LoadLibraryEx(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (hUxtheme == nullptr)
    {
        return;
    }

    using ADMFW = bool (WINAPI*)(HWND, bool);
    using FMT = void (WINAPI*)();

    const auto _AllowDarkModeForWindow = reinterpret_cast<ADMFW>(::GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
    const auto _FlushMenuThemes = reinterpret_cast<FMT>(::GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));

    if (_AllowDarkModeForWindow != nullptr && _FlushMenuThemes != nullptr)
    {
        _AllowDarkModeForWindow(hWnd, enableDark);
        SetMode(hUxtheme);
        _FlushMenuThemes();
    }

    ::FreeLibrary(hUxtheme);
}

void SetTitleBar(HWND hWnd)
{
    BOOL dark = enableDark ? TRUE : FALSE;

    if (IsAtLeastWin10Build(BUILD_WIN11))
    {
        ::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    }
    else if (IsAtLeastWin10Build(VER_1903))
    {
        const auto hUser32 = ::LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (hUser32)
        {
            const auto _SetWindowCompositionAttribute = reinterpret_cast<SWCA>(::GetProcAddress(hUser32, "SetWindowCompositionAttribute"));

            if (_SetWindowCompositionAttribute != nullptr)
            {
                WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };

                if (_SetWindowCompositionAttribute(hWnd, &data))
                {
                    ::FreeLibrary(hUser32);
                    return;
                }
            }

            ::FreeLibrary(hUser32);
        }
    }
    else if (IsAtLeastWin10Build(VER_1809))
    {
        ::SetProp(hWnd, L"UseImmersiveDarkModeColors", reinterpret_cast<HANDLE>(static_cast<INT_PTR>(dark)));
    }
}

void SetTooltips(HWND hWnd)
{
    DWORD processID = 0;
    ::GetWindowThreadProcessId(hWnd, &processID);
    HWND hTooltip = nullptr;
    LPCWSTR themeName = enableDark ? L"DarkMode_Explorer" : nullptr;
    do {
        hTooltip = ::FindWindowEx(nullptr, hTooltip, nullptr, nullptr);
        DWORD checkProcessID = 0;
        ::GetWindowThreadProcessId(hTooltip, &checkProcessID);

        if (checkProcessID == processID)
        {
            WCHAR className[classNameLenght] = { '\0' };

            if (GetClassName(hTooltip, className, classNameLenght) > 0)
            {
                if (wcscmp(className, TOOLTIPS_CLASS) == 0)
                {
                    ::SetWindowTheme(hTooltip, themeName, nullptr);
                }
                else if (wcscmp(className, TOOLBARCLASSNAME) == 0)
                {
                    const auto hTip = reinterpret_cast<HWND>(::SendMessage(hTooltip, TB_GETTOOLTIPS, 0, 0));
                    if (hTip != nullptr)
                    {
                        ::SetWindowTheme(hTip, themeName, nullptr);
                    }
                }
                else if (wcscmp(className, WC_TREEVIEW) == 0)
                {
                    const auto hTip = TreeView_GetToolTips(hTooltip);
                    if (hTip != nullptr)
                    {
                        ::SetWindowTheme(hTip, themeName, nullptr);
                    }
                }
                else if (wcscmp(className, WC_LISTVIEW) == 0)
                {
                    const auto hTip = ListView_GetToolTips(hTooltip);
                    if (hTip != nullptr)
                    {
                        ::SetWindowTheme(hTip, themeName, nullptr);
                    }
                }
                else if (wcscmp(className, WC_TABCONTROL) == 0)
                {
                    const auto hTip = TabCtrl_GetToolTips(hTooltip);
                    if (hTip != nullptr)
                    {
                        ::SetWindowTheme(hTip, themeName, nullptr);
                    }
                }
            }
        }
    } while (hTooltip != nullptr);
}

BOOL CALLBACK ScrollBarChildProc(HWND hWnd, LPARAM lparam)
{
    const auto dwStyle = ::GetWindowLongPtr(hWnd, GWL_STYLE);
    if ((dwStyle & WS_CHILD) && (dwStyle & (WS_VSCROLL | WS_HSCROLL)))
    {
        wchar_t className[classNameLenght] = { '\0' };
        if (GetClassName(hWnd, className, classNameLenght) > 0)
        {
            if ((wcscmp(className, WC_TREEVIEW) == 0) ||
                (wcscmp(className, WC_LISTVIEW) == 0) ||
                (wcscmp(className, WC_HEADER) == 0) ||
                (wcscmp(className, L"Scintilla") == 0) ||
                (wcscmp(className, WC_TABCONTROL) == 0) ||
                (wcscmp(className, TOOLBARCLASSNAME) == 0) ||
                (wcscmp(className, REBARCLASSNAME) == 0) ||
                (wcscmp(className, STATUSCLASSNAME) == 0))
            {
                return TRUE;
            }
        }
        ::SetWindowTheme(hWnd, reinterpret_cast<LPCWSTR>(lparam), nullptr);
    }

    return TRUE;
}

void ConfigureScintillaForEffects(HWND hSci)
{
    if (hSci != nullptr && ::IsWindow(hSci))
    {
        // Double-buffered drawing: ensures Scintilla renders to an offscreen surface
        // before presentation, eliminating alpha punch-through holes and flicker.
        // Memory overhead is strictly viewport size (~8MB for 1080p), constant for any file size.
        const auto buffered = ::SendMessage(hSci, SCI_GETBUFFEREDDRAW, 0, 0);
        if (!buffered)
        {
            ::SendMessage(hSci, SCI_SETBUFFEREDDRAW, TRUE, 0);
        }

        // If DirectWrite (1) is active, promote to DirectWrite Retain (2) so that Direct2D
        // retains glyph surfaces across redraws, avoiding cache invalidation and saving CPU.
        const auto tech = ::SendMessage(hSci, SCI_GETTECHNOLOGY, 0, 0);
        if (tech == SC_TECHNOLOGY_DIRECTWRITE)
        {
            ::SendMessage(hSci, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITERETAIN, 0);
        }

        if (micaType >= 2 && micaType <= 5)
        {
            // Set Scintilla background tint according to effectIntensity (10% to 100%):
            // 100% intensity -> tint = 0 (pure black), DWM backdrop shines through at full strength!
            // 10% intensity -> tint = 45 (dark overlay), subtle background.
            int tint = static_cast<int>((100 - effectIntensity) * 0.45f);
            if (tint < 0) tint = 0;
            if (tint > 50) tint = 50;
            ::SendMessage(hSci, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(tint, tint, tint));
        }
    }
}

void ConfigureAllScintillaViews()
{
    ConfigureScintillaForEffects(nppData._scintillaMainHandle);
    ConfigureScintillaForEffects(nppData._scintillaSecondHandle);
}

void ClearLegacyAccentPolicy(HWND hWnd)
{
    HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
    if (hUser32 != nullptr)
    {
        const auto _SetWindowCompositionAttribute = reinterpret_cast<SWCA>(::GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
        if (_SetWindowCompositionAttribute != nullptr)
        {
            ACCENTPOLICY policy{};
            policy.nAccentState = ACCENT_DISABLED;
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY) };
            _SetWindowCompositionAttribute(hWnd, &data);
        }
    }
}

void SetLegacyAccentPolicy(HWND hWnd, bool enableAcrylic, uint32_t nColor)
{
    HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
    if (hUser32 == nullptr)
    {
        hUser32 = ::LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    if (hUser32 != nullptr)
    {
        const auto _SetWindowCompositionAttribute = reinterpret_cast<SWCA>(::GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
        if (_SetWindowCompositionAttribute != nullptr)
        {
            ACCENTPOLICY policy{};
            if (enableAcrylic)
            {
                policy.nAccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
                policy.nFlags = 2;
                policy.nColor = (nColor != 0) ? nColor : 0x80181818;
            }
            else
            {
                policy.nAccentState = ACCENT_DISABLED;
            }
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY) };
            _SetWindowCompositionAttribute(hWnd, &data);
        }
    }
}

void SetMica(HWND hWnd)
{
    constexpr MARGINS marginsExtended = { -1, -1, -1, -1 };
    constexpr MARGINS marginsReset{};

    if (micaType == 5) // Acrylic (Fluent Acrylic blur behind)
    {
        // For Fluent Acrylic blur, disable DWM system backdrop so SWCA blur is visible
        if (IsAtLeastWin10Build(BUILD_22H2))
        {
            auto none = DWMSBT_NONE;
            ::DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &none, sizeof(none));
        }

        // Calculate tint alpha from effectIntensity (10 to 100):
        // Higher intensity -> lower alpha tint (more translucent blur!)
        // Lower intensity -> higher alpha tint (darker subtle blur!)
        int alpha = 255 - static_cast<int>(effectIntensity * 2.15f);
        if (alpha < 0x18) alpha = 0x18;
        if (alpha > 0xF0) alpha = 0xF0;

        uint32_t tintColor = 0x00141414;
        uint32_t nColor = (static_cast<uint32_t>(alpha) << 24) | tintColor;

        SetLegacyAccentPolicy(hWnd, true, nColor);
        ::DwmExtendFrameIntoClientArea(hWnd, &marginsExtended);
    }
    else
    {
        // For non-Acrylic modes (Mica, Mica Acrylic, Tabbed, Auto, None):
        // Always reset SWCA so DWM system backdrops are unhindered!
        ClearLegacyAccentPolicy(hWnd);

        if (IsAtLeastWin10Build(BUILD_22H2))
        {
            auto mica = DWMSBT_AUTO;
            switch (micaType)
            {
                case 1:
                {
                    mica = DWMSBT_NONE;
                }
                break;

                case 2:
                {
                    mica = DWMSBT_MAINWINDOW; // Mica
                }
                break;

                case 3:
                {
                    mica = DWMSBT_TRANSIENTWINDOW; // Mica Acrylic
                }
                break;

                case 4:
                {
                    mica = DWMSBT_TABBEDWINDOW; // Mica Alternative (Tabbed)
                }
                break;

                default:
                {
                    mica = DWMSBT_AUTO;
                }
                break;
            }

            const bool isExtended = (micaType >= 2 && micaType <= 4);
            ::DwmExtendFrameIntoClientArea(hWnd, isExtended ? &marginsExtended : &marginsReset);
            ::DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &mica, sizeof(mica));
        }
        else if (IsAtLeastWin10Build(BUILD_WIN11))
        {
            const BOOL useMica = (micaType != 1 && micaType != 0);
            const bool isExtended = (micaType >= 2 && micaType <= 4);
            ::DwmExtendFrameIntoClientArea(hWnd, isExtended ? &marginsExtended : &marginsReset);
            ::DwmSetWindowAttribute(hWnd, DWMWA_MICA_EFFECT, &useMica, sizeof(useMica));
        }
        else
        {
            ::DwmExtendFrameIntoClientArea(hWnd, &marginsReset);
        }
    }
}

void SetDarkNpp()
{
    HWND hwnd = nppData._nppHandle;
    SetTheme(hwnd);
    SetTitleBar(hwnd);
    SetTooltips(hwnd);

    ::EnumChildWindows(hwnd, &ScrollBarChildProc, reinterpret_cast<LPARAM>(enableDark ? L"DarkMode_Explorer" : nullptr));

    SetMica(hwnd);
}

void SetMicaNpp()
{
    HWND hwnd = nppData._nppHandle;
    SetMica(hwnd);
    ConfigureAllScintillaViews();
    if (hwnd != nullptr && ::IsWindow(hwnd))
    {
        ::RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
    }
}

static HWND hIntensityDlg = nullptr;
static HWND hTrackbar = nullptr;
static HWND hValueLabel = nullptr;
static HBRUSH hDarkBrush = nullptr;

LRESULT CALLBACK IntensityDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        BOOL dark = TRUE;
        ::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

        ::CreateWindowExW(0, L"STATIC", L"Adjust Effect Intensity / Translucency:",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 15, 290, 20, hWnd, nullptr, nullptr, nullptr);

        hTrackbar = ::CreateWindowExW(0, TRACKBAR_CLASS, L"",
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_NOTICKS,
            20, 42, 290, 30, hWnd, reinterpret_cast<HMENU>(101), nullptr, nullptr);

        ::SendMessage(hTrackbar, TBM_SETRANGE, TRUE, MAKELPARAM(10, 100));
        ::SendMessage(hTrackbar, TBM_SETPOS, TRUE, effectIntensity);

        const std::wstring valText = std::wstring(L"Intensity: ") + std::to_wstring(effectIntensity) + L"%";
        hValueLabel = ::CreateWindowExW(0, L"STATIC", valText.c_str(),
            WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 78, 290, 20, hWnd, reinterpret_cast<HMENU>(102), nullptr, nullptr);

        ::CreateWindowExW(0, L"BUTTON", L"OK",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            125, 110, 80, 28, hWnd, reinterpret_cast<HMENU>(IDOK), nullptr, nullptr);
    }
    return 0;

    case WM_HSCROLL:
    {
        if (reinterpret_cast<HWND>(lParam) == hTrackbar)
        {
            effectIntensity = static_cast<int>(::SendMessage(hTrackbar, TBM_GETPOS, 0, 0));
            if (effectIntensity < 10) effectIntensity = 10;
            if (effectIntensity > 100) effectIntensity = 100;

            const std::wstring valText = std::wstring(L"Intensity: ") + std::to_wstring(effectIntensity) + L"%";
            ::SetWindowTextW(hValueLabel, valText.c_str());

            SetMicaNpp();

            const auto hMenu = ::GetMenu(nppData._nppHandle);
            if (hMenu)
            {
                ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 0]._cmdID, MF_BYCOMMAND | (effectIntensity >= 90 ? MF_CHECKED : MF_UNCHECKED));
                ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 1]._cmdID, MF_BYCOMMAND | (effectIntensity >= 65 && effectIntensity < 90 ? MF_CHECKED : MF_UNCHECKED));
                ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 2]._cmdID, MF_BYCOMMAND | (effectIntensity >= 40 && effectIntensity < 65 ? MF_CHECKED : MF_UNCHECKED));
                ::CheckMenuItem(hMenu, funcItem[menuItemIntensity + 3]._cmdID, MF_BYCOMMAND | (effectIntensity < 40 ? MF_CHECKED : MF_UNCHECKED));
            }
        }
    }
    return 0;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            SavePluginParams();
            ::DestroyWindow(hWnd);
        }
    }
    return 0;

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORDLG:
    {
        const HDC hdc = reinterpret_cast<HDC>(wParam);
        ::SetTextColor(hdc, RGB(225, 225, 225));
        ::SetBkColor(hdc, RGB(32, 32, 32));
        if (hDarkBrush == nullptr)
        {
            hDarkBrush = ::CreateSolidBrush(RGB(32, 32, 32));
        }
        return reinterpret_cast<INT_PTR>(hDarkBrush);
    }

    case WM_DESTROY:
    {
        if (hDarkBrush != nullptr)
        {
            ::DeleteObject(hDarkBrush);
            hDarkBrush = nullptr;
        }
        hIntensityDlg = nullptr;
    }
    return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ShowIntensityDialog()
{
    if (hIntensityDlg != nullptr && ::IsWindow(hIntensityDlg))
    {
        ::SetForegroundWindow(hIntensityDlg);
        return;
    }

    static bool registered = false;
    const wchar_t* className = L"DarkNppIntensityDlg";
    if (!registered)
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = IntensityDlgProc;
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = ::CreateSolidBrush(RGB(32, 32, 32));
        wc.lpszClassName = className;
        ::RegisterClassExW(&wc);
        registered = true;
    }

    RECT rcParent{};
    ::GetWindowRect(nppData._nppHandle, &rcParent);
    const int dlgW = 345;
    const int dlgH = 195;
    const int x = rcParent.left + ((rcParent.right - rcParent.left) - dlgW) / 2;
    const int y = rcParent.top + ((rcParent.bottom - rcParent.top) - dlgH) / 2;

    INITCOMMONCONTROLSEX icex{};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    ::InitCommonControlsEx(&icex);

    hIntensityDlg = ::CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        className,
        L"DarkNpp - Effect Intensity",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, dlgW, dlgH,
        nppData._nppHandle,
        nullptr,
        ::GetModuleHandleW(nullptr),
        nullptr
    );

    if (hIntensityDlg != nullptr)
    {
        ::ShowWindow(hIntensityDlg, SW_SHOW);
        ::UpdateWindow(hIntensityDlg);
    }
}
