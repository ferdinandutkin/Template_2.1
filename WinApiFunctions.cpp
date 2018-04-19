//
// Created by Сергей on 19.04.2018.
//

#include "WinApiFunctions.h"

int DoubleBuffering::GetRectWidth(RECT rect) {
    return rect.right - rect.left;
}

int DoubleBuffering::GetRectHeigth(RECT rect) {
    return rect.bottom - rect.top;
}

HGDIOBJ DoubleBuffering::SelectBufferBitmap(HDC deviceContext, HDC memoryContext, RECT clientRect) {
    auto bufferBitmap = CreateCompatibleBitmap(deviceContext, GetRectWidth(clientRect), GetRectHeigth(clientRect));
    auto oldBitmap = SelectObject(memoryContext, bufferBitmap);
    CopyContextBits(memoryContext, deviceContext, clientRect);
//        ClearBackgroud(memoryContext, clientRect);

    return oldBitmap;
}

void DoubleBuffering::ClearBackgroud(HDC deviceContext, RECT clientRect) {
    auto backgroudBrush = GetSysColorBrush(COLOR_WINDOW); // CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(deviceContext, &clientRect, backgroudBrush);
}

void DoubleBuffering::CopyContextBits(HDC destination, HDC source, RECT clientRect) {
    BitBlt(
            destination,
            clientRect.left, clientRect.top,
            GetRectWidth(clientRect), GetRectHeigth(clientRect),
            source,
            0, 0,
            SRCCOPY);
}

void DoubleBuffering::ReleaseOldObj(HDC hdc, HGDIOBJ obj) {
    auto buf_obj = SelectObject(hdc, obj);
    DeleteObject(buf_obj);
}

RECT DoubleBuffering::GetClientRect(HWND hwnd) {
    RECT r;
    ::GetClientRect(hwnd, &r);
    return r;
}

DoubleBuffering::DoubleBuffering(HWND hwnd, HDC hdc) : DoubleBuffering(hwnd, std::make_pair(hdc, PAINTSTRUCT({}))) {
}

DoubleBuffering::DoubleBuffering(HWND hwnd, std::pair<HDC, PAINTSTRUCT> paint) : hdc(paint.first), hwnd(hwnd),
                                                                                 ps(paint.second) {
    auto rect = GetClientRect(hwnd);
    memory_context = CreateCompatibleDC(hdc);
    old_bitmap = SelectBufferBitmap(hdc, memory_context, rect);
}

DoubleBuffering::DoubleBuffering(HWND hwnd) : DoubleBuffering(hwnd, Begin_Paint(hwnd)) {
    need_to_end_paint = true;
}

DoubleBuffering::operator HDC() {
    return memory_context;
}

DoubleBuffering::~DoubleBuffering() {
    auto rect = GetClientRect(hwnd);
    CopyContextBits(hdc, memory_context, rect);
    ReleaseOldObj(memory_context, old_bitmap);
    ReleaseDC(hwnd, hdc);
    if (need_to_end_paint)
        EndPaint(hwnd, &ps);
    else
        ReleaseDC(hwnd, hdc);
}

void DoubleBuffering::on_clear_background(HWND hwnd, WPARAM wparam, LPARAM lparam) {

}

std::pair<HDC, PAINTSTRUCT> Begin_Paint(HWND hwnd) {
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    return std::make_pair(ps.hdc, ps);
}

RECT rect_to_screen(HWND hwnd, RECT rect) {
    POINT positionLeftTop = {rect.left, rect.top};
    ClientToScreen(hwnd, &positionLeftTop);
    POINT positionRightBottom = {rect.right, rect.bottom};
    ClientToScreen(hwnd, &positionRightBottom);
    SetRect(&rect, positionLeftTop.x, positionLeftTop.y, positionRightBottom.x, positionRightBottom.y);
    return rect;
}
