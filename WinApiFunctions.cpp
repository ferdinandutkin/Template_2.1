//
// Created by Сергей on 19.04.2018.
//

#include <random>
#include <ctime>
#include "WinApiFunctions.h"

namespace WinApi {
    int GetRectWidth(RECT rect) {
        return rect.right - rect.left;
    }

    int GetRectHeigth(RECT rect) {
        return rect.bottom - rect.top;
    }


    void ClearBackgroud(HDC deviceContext, RECT clientRect) {
        auto backgroudBrush = CreateSolidBrush(GetSysColor(CTLCOLOR_DLG));
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
        return RGB(random(0, 255), random(0, 255), random(0, 255));
    }

    int random(int a, int b) {
        static std::mt19937 gen(static_cast<unsigned int>(time(nullptr)));
        std::uniform_int_distribution<> uid(a, b);
        return uid(gen);
    }
}