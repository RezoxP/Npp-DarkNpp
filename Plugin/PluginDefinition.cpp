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

constexpr int menuItemEnableDark = 0;
constexpr int menuItemMica = menuItemEnableDark + 3;
constexpr int menuItemAbout = menuItemMica + 7;

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
    funcItem[menuItemAbout] = { L"&About...", About, 0, false, nullptr };
}

void LoadSettings()
{
    ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(iniFilePath));
    ::PathAppend(iniFilePath, L"\\DarkNpp.ini");

    enableDark = ::GetPrivateProfileInt(sectionName, L"useDark", 1, iniFilePath) != 0;

    micaType = ::GetPrivateProfileInt(sectionName, L"micaType", 0, iniFilePath);
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

void SetLegacyAccentPolicy(HWND hWnd, bool enableAcrylic)
{
    // SWCA accent policy is strictly a legacy fallback for Windows 10.
    // On Windows 11, calling SWCA corrupts DWM composition and breaks DWMWA_SYSTEMBACKDROP_TYPE.
    if (IsAtLeastWin10Build(BUILD_WIN11))
    {
        return;
    }

    if (IsAtLeastWin10Build(VER_1809))
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
                    policy.nColor = 0x01101010;
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
}

void SetMica(HWND hWnd)
{
    // On Windows 11, ensure no legacy accent policy is active so DWM system backdrops function properly.
    if (IsAtLeastWin10Build(BUILD_WIN11))
    {
        ClearLegacyAccentPolicy(hWnd);
    }
    else
    {
        // On Windows 10, acrylic blur behind is only available via undocumented SWCA.
        SetLegacyAccentPolicy(hWnd, micaType == 5);
    }

    constexpr MARGINS marginsExtended = { -1, -1, -1, -1 };
    constexpr MARGINS marginsReset{};

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
                mica = DWMSBT_MAINWINDOW;
            }
            break;

            case 3:
            case 5:
            {
                // DWMSBT_TRANSIENTWINDOW is the native Windows 11 Acrylic backdrop
                mica = DWMSBT_TRANSIENTWINDOW;
            }
            break;

            case 4:
            {
                mica = DWMSBT_TABBEDWINDOW;
            }
            break;

            default:
            {
                mica = DWMSBT_AUTO;
            }
            break;
        }

        const bool isExtended = (micaType >= 2 && micaType <= 5);
        ::DwmExtendFrameIntoClientArea(hWnd, isExtended ? &marginsExtended : &marginsReset);
        ::DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &mica, sizeof(mica));
    }
    else if (IsAtLeastWin10Build(BUILD_WIN11))
    {
        const BOOL useMica = (micaType != 1 && micaType != 0);
        const bool isExtended = (micaType >= 2 && micaType <= 5);
        ::DwmExtendFrameIntoClientArea(hWnd, isExtended ? &marginsExtended : &marginsReset);
        ::DwmSetWindowAttribute(hWnd, DWMWA_MICA_EFFECT, &useMica, sizeof(useMica));
    }
    else
    {
        const bool isExtended = (micaType == 5);
        ::DwmExtendFrameIntoClientArea(hWnd, isExtended ? &marginsExtended : &marginsReset);
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
