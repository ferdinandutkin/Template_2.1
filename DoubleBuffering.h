//
// Created by serko on 19.04.2018.
//

#ifndef UNTITLED7_DOUBLEBUFFERING_H
#define UNTITLED7_DOUBLEBUFFERING_H

#include <windows.h>
#include <utility>

namespace WinApi {
    class DoubleBuffering final {
        HDC hdc, memory_context;
        HWND hwnd;
        PAINTSTRUCT ps;
        bool need_to_end_paint = false;
        HGDIOBJ old_bitmap;

        static void CopyContextBits(HDC destination, HDC source, RECT clientRect);

        static HGDIOBJ SelectBufferBitmap(HDC deviceContext, HDC memoryContext, RECT clientRect);

        DoubleBuffering(HWND hwnd, std::pair<HDC, PAINTSTRUCT> paint);

    public:

//!     WM_ERASEBKGND
        static void on_clear_background(HWND hwnd, WPARAM wparam, LPARAM lparam);

        DoubleBuffering(HWND hwnd, HDC hdc);

        explicit DoubleBuffering(HWND hwnd);

        operator HDC();

        ~DoubleBuffering();
    };

}

#endif //UNTITLED7_DOUBLEBUFFERING_H
