//
// Created by serko on 31.03.2018.
//

#include "WindowClass.h"

WinApi::WindowClass::WindowClass(HINSTANCE hInstance, LPCWSTR className, WNDPROC windowProcedure) {
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = windowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = className;
    wcex.hIconSm = nullptr;
}

ATOM WinApi::WindowClass::Register() {
    return RegisterClassExW(&wcex);
}
