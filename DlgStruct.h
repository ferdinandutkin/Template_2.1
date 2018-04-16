//
// Created by serko on 15.04.2018.
//

#ifndef UNTITLED7_DLGSTRUCT_H
#define UNTITLED7_DLGSTRUCT_H


#include <windows.h>

struct MyData {
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

};

#endif //UNTITLED7_DLGSTRUCT_H
