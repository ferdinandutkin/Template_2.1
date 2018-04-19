//
// Created by serko on 19.04.2018.
//

#ifndef UNTITLED7_DIALOGCLASS_H
#define UNTITLED7_DIALOGCLASS_H

#include <windows.h>
#include <functional>
#include "Template.h"

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

        operator HWND();
    };
}

#endif //UNTITLED7_DIALOGCLASS_H
