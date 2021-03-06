//
// Created by Сергей on 19.04.2018.
//

#include <random>
#include "WinApiFunctions.h"

namespace WinApi {
    int GetRectWidth(RECT rect) {
        return rect.right - rect.left;
    }

    int GetRectHeigth(RECT rect) {
        return rect.bottom - rect.top;
    }


    void ClearBackgroud(HDC deviceContext, RECT clientRect) {
        auto backgroudBrush = GetSysColorBrush(COLOR_WINDOW); // CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(deviceContext, &clientRect, backgroudBrush);
        DeleteObject(backgroudBrush);
    }


    void ReleaseOldObj(HDC hdc, HGDIOBJ obj) {
        auto buf_obj = SelectObject(hdc, obj);
        DeleteObject(buf_obj);
    }

    RECT GetClientRect(HWND hwnd) {
        RECT r;
        ::GetClientRect(hwnd, &r);
        return r;
    }

    std::pair<HDC, PAINTSTRUCT> BeginPaint(HWND hwnd) {
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

    COLORREF GetRandColor() {
        std::linear_congruential_engine<unsigned char, 123, 456, 256> rand;
        return RGB(rand() % 256, rand() % 256, rand() % 256);
    }
}