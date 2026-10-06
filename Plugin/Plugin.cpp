/*
  Copyright (C) 2020-2022 oZone10
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

extern FuncItem funcItem[nbFunc];
extern NppData nppData;

//BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID /*lpReserved*/)
//{
//    switch (reasonForCall)
//    {
//    case DLL_PROCESS_ATTACH:
//        pluginInit(hModule);
//        break;
//
//    case DLL_PROCESS_DETACH:
//        pluginCleanUp();
//        break;
//
//    case DLL_THREAD_ATTACH:
//        break;
//
//    case DLL_THREAD_DETACH:
//        break;
//    }
//    return TRUE;
//}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
    nppData = notpadPlusData;
    PluginInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
    return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int* nbF)
{
    *nbF = nbFunc;
    return funcItem;
}


extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode)
{
    if (notifyCode == nullptr)
        return;

    switch (notifyCode->nmhdr.code)
    {
    case NPPN_READY:
    {
        ConfigureAllScintillaViews();
        SetMicaNpp();
    }
    break;

    case SCN_UPDATEUI:
    {
        // Only invalidate on scroll events when a translucency effect is active (micaType 2 to 5).
        // When micaType is Auto (0) or None (1), effects are disabled so zero extra work is done.
        // Memory and CPU overhead is zero on large files as Scintilla double-buffering only buffers the viewport.
        if (micaType >= 2 && micaType <= 5)
        {
            if (notifyCode->updated & (SC_UPDATE_V_SCROLL | SC_UPDATE_H_SCROLL))
            {
                const auto hWndFrom = static_cast<HWND>(notifyCode->nmhdr.hwndFrom);
                if (hWndFrom != nullptr && (hWndFrom == nppData._scintillaMainHandle || hWndFrom == nppData._scintillaSecondHandle))
                {
                    ::InvalidateRect(hWndFrom, nullptr, FALSE);
                }
            }
        }
    }
    break;

    default:
        break;
    }
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if (Message == WM_DWMCOMPOSITIONCHANGED)
    {
        SetMicaNpp();
    }
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
