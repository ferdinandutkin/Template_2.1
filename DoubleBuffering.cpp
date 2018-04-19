//
// Created by serko on 19.04.2018.
//

#include "DoubleBuffering.h"
#include "WinApiFunctions.h"

namespace WinApi {

    void DoubleBuffering::CopyContextBits(HDC destination, HDC source, RECT clientRect) {
        BitBlt(
                destination,
                clientRect.left, clientRect.top,
                GetRectWidth(clientRect), GetRectHeigth(clientRect),
                source,
                0, 0,
                SRCCOPY);
    }

    DoubleBuffering::DoubleBuffering(HWND hwnd, HDC hdc) : DoubleBuffering(hwnd, std::make_pair(hdc, PAINTSTRUCT({}))) {
    }

    DoubleBuffering::DoubleBuffering(HWND hwnd, std::pair<HDC, PAINTSTRUCT> paint) : hdc(paint.first), hwnd(hwnd),
                                                                                     ps(paint.second) {
        auto rect = GetClientRect(hwnd);
        memory_context = CreateCompatibleDC(hdc);
        old_bitmap = SelectBufferBitmap(hdc, memory_context, rect);
    }

    DoubleBuffering::DoubleBuffering(HWND hwnd) : DoubleBuffering(hwnd, BeginPaint(hwnd)) {
        need_to_end_paint = true;
    }

    DoubleBuffering::operator HDC() {
        return memory_context;
    }

    DoubleBuffering::~DoubleBuffering() {
        auto rect = GetClientRect(hwnd);
        CopyContextBits(hdc, memory_context, rect);
        ReleaseOldObj(memory_context, old_bitmap);
        DeleteDC(memory_context);
        if (need_to_end_paint)
            EndPaint(hwnd, &ps);
        else
            ReleaseDC(hwnd, hdc);
    }

    void DoubleBuffering::on_clear_background(HWND hwnd, WPARAM wparam, LPARAM lparam) {

    }

    HGDIOBJ DoubleBuffering::SelectBufferBitmap(HDC deviceContext, HDC memoryContext, RECT clientRect) {
        auto bufferBitmap = CreateCompatibleBitmap(deviceContext, GetRectWidth(clientRect), GetRectHeigth(clientRect));
        auto oldBitmap = SelectObject(memoryContext, bufferBitmap);
        CopyContextBits(memoryContext, deviceContext, clientRect);
//        ClearBackgroud(memoryContext, clientRect);

        return oldBitmap;
    }

}