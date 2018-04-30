//
// Created by serko on 19.04.2018.
//

#include "DialogClass.h"

namespace WinApi {

    HCreateDialog::HCreateDialog(HINSTANCE hinst, HWND parent, DLGPROC proc) {
        _hdialog = CreateDialogIndirect(hinst, &md.mHeader, parent, proc);
        ShowWindow(_hdialog, SW_NORMAL);
    }

    HCreateDialog::operator HWND() const {
        return _hdialog;
    }

    const unsigned long InitialiseDialogAndControlls::default_dialog_style = DS_SETFONT |
                                                                             DS_MODALFRAME |
                                                                             DS_FIXEDSYS |
                                                                             WS_POPUP |
                                                                             WS_CAPTION |
                                                                             WS_SYSMENU;

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::CreateControl(const wchar_t *className, unsigned long style, int x, int y, int width,
                                                int height, int id, LPCWSTR name) {
        controls.push_back(
                CreateWindow(className, name, WS_CHILD | WS_VISIBLE | style, x, y, width, height, _hdialog, (HMENU) id,
                             hinst,
                             nullptr));
        return *this;
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::CreateEditBox(POINT pos, int id, int width, int height, unsigned long style,
                                                LPCWSTR str) {
        return CreateControl(WC_EDIT, WS_TABSTOP | WS_BORDER | ES_CENTER | style,
                             pos.x, pos.y, width, height, id, str);
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::CreateComboBox(POINT pos, int id, int width, int height, unsigned long style) {
        return CreateControl(WC_COMBOBOX, CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP | style,
                             pos.x, pos.y, width, height, id);
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::CreateSlider(POINT pos, int id, int width, int height, int min_val, int max_val,
                                               unsigned long style) {
        CreateControl(TRACKBAR_CLASS, WS_TABSTOP | TBS_AUTOTICKS | TBS_HORZ | style, pos.x, pos.y, width, height, id);
        auto slider = GetDlgItem(_hdialog, id);
        SendMessage(slider, TBM_SETRANGE, TRUE, MAKELONG(min_val, max_val));
        SendMessage(slider, TBM_SETPOS, TRUE, 0);

        return *this;
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::init_dialog(POINT pos, int width, int height, const std::wstring &title,
                                              unsigned long style) {
        SetWindowLong(_hdialog, GWL_STYLE, style);
        SetWindowPos(_hdialog, nullptr, pos.x, pos.y, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowText(_hdialog, title.c_str());
        return *this;
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::_CreateButton(POINT pos, int id, int width, int height, unsigned long style,
                                                const std::wstring &name) {
        return CreateControl(WC_BUTTON, style, pos.x, pos.y, width, height, id, name.c_str());
    }

    InitialiseDialogAndControlls &
    InitialiseDialogAndControlls::CreatePushButton(POINT pos, int id, int width, int height, unsigned long style,
                                                   const std::wstring &name) {
        return _CreateButton(pos, id, width, height, BS_PUSHBUTTON | style, name);
    }
}