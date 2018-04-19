//
// Created by Сергей on 19.04.2018.
//

#ifndef UNTITLED7_WINAPIFUNCTIONS_H
#define UNTITLED7_WINAPIFUNCTIONS_H

#include <windows.h>
#include <utility>

std::pair<HDC, PAINTSTRUCT> Begin_Paint(HWND hwnd);

RECT rect_to_screen(HWND hwnd, RECT rect);

class DoubleBuffering {
    HDC hdc, memory_context;
    HWND hwnd;
    PAINTSTRUCT ps;
    bool need_to_end_paint = false;
    HGDIOBJ old_bitmap;

public:
    static int GetRectWidth(RECT rect);

    static int GetRectHeigth(RECT rect);

    static HGDIOBJ SelectBufferBitmap(HDC deviceContext, HDC memoryContext, RECT clientRect);

    static void ClearBackgroud(HDC deviceContext, RECT clientRect);

    static void on_clear_background(HWND hwnd, WPARAM wparam, LPARAM lparam);

    static void CopyContextBits(HDC destination, HDC source, RECT clientRect);

    static void ReleaseOldObj(HDC hdc, HGDIOBJ obj);

    static RECT GetClientRect(HWND hwnd);

    DoubleBuffering(HWND hwnd, HDC hdc);

    DoubleBuffering(HWND hwnd);

    DoubleBuffering(HWND hwnd, std::pair<HDC, PAINTSTRUCT> paint);

    operator HDC();

    ~DoubleBuffering();
};


#endif //UNTITLED7_WINAPIFUNCTIONS_H
