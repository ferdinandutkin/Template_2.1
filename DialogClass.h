//
// Created by serko on 19.04.2018.
//

#ifndef UNTITLED7_DIALOGCLASS_H
#define UNTITLED7_DIALOGCLASS_H

#include "Template.h"
#include <windows.h>
#include <functional>

namespace WinApi {

    class HCreateDialog {
        const struct MyData {
            DLGTEMPLATE mHeader;

#pragma pack(2)
            WORD mNoMenu; // 0x0000 -- no menu
            WORD mStdClass; // 0x0000 -- standard dialog class
            wchar_t mTitle[5]; // title: "Test"

#pragma pack(4)
            DLGITEMTEMPLATE mItem;

#pragma pack(2)
            WORD mFfff; // 0xFFFF -- next is standard class ID
            WORD mListBoxClassId; // 0x0083 -- class ID for listbox
            wchar_t mText[5]; // text (not used for listbox)
            WORD mNoData; // 0

#pragma pack(4)

        } md = {{WS_CAPTION | WS_VISIBLE | DS_CENTER, 0, 1, 10, 10, 100, 100}, 0, 0, L"Test",
                {WS_CHILD | WS_BORDER, 0, 1, 1, 50, 50, 1234}, 0xFFFF, 0x0083, L"Test", 0};

        HWND _hdialog;
    public:
        HCreateDialog(HINSTANCE hinst, HWND parent, DLGPROC proc);

        operator HWND() const;
    };


    class InitialiseDialogAndControlls {
        HWND _hdialog;
        HINSTANCE hinst;
        std::vector<HWND> controls;
    public:
        const static unsigned long default_dialog_style;

        InitialiseDialogAndControlls(HINSTANCE hinst, HWND h_dialog) : _hdialog(h_dialog), hinst(hinst) {}

        InitialiseDialogAndControlls &
        CreateControl(const wchar_t *className, unsigned long style, int x, int y, int width, int height, int id,
                      LPCWSTR name = L"");    //!style|WS_VISIBLE|WS_CHILD

        void DeleteControls() {
            while (!controls.empty()) {
                DestroyWindow(controls.back());
                controls.pop_back();
            }
        }

//!     prefix: E
        InitialiseDialogAndControlls &
        CreateEditBox(POINT pos, int id, int width, int height, unsigned long style = 0, LPCWSTR str = L"");

//!     prefix: CB
        InitialiseDialogAndControlls &CreateComboBox(POINT pos, int id, int width, int height, unsigned long style = 0);

//!     prefix: TB
        InitialiseDialogAndControlls &
        CreateSlider(POINT pos, int id, int width, int height, int min_val = 0, int max_val = 10,
                     unsigned long style = 0);

        InitialiseDialogAndControlls &
        _CreateButton(POINT pos, int id, int width, int height, unsigned long style = 0,
                      const std::wstring &name = L"");

        InitialiseDialogAndControlls &
        CreatePushButton(POINT pos, int id, int width, int height, unsigned long style = 0,
                         const std::wstring &name = L"");

        InitialiseDialogAndControlls &
        CreateCheckBox(POINT pos, int id, int width, int height, unsigned long style = 0,
                       const std::wstring &name = L"") {
            return _CreateButton(pos, id, width, height, BS_AUTOCHECKBOX | style, name);
        }

        InitialiseDialogAndControlls &
        CreateGroupBox(POINT pos, int id, int width, int height, unsigned long style = 0,
                       const std::wstring &name = L"") {
            return _CreateButton(pos, id, width, height, BS_GROUPBOX | style, name);
        }

        InitialiseDialogAndControlls &
        init_dialog(POINT pos = {0, 0}, int width = 500, int height = 400, const std::wstring &title = L"Dialog",
                    unsigned long style = default_dialog_style);
    };
}

#endif //UNTITLED7_DIALOGCLASS_H
