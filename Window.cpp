//
// Created by serko on 31.03.2018.
//

#include "Window.h"

bool
WinApi::Window::create_and_show(HINSTANCE hInstance, LPCWSTR windowClass, LPCWSTR title, int nCmdShow, LONG style) {
    auto hwnd = CreateWindowW(windowClass,
                              title,
                              style,
                              CW_USEDEFAULT, 0,
                              CW_USEDEFAULT, 0,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);

    if (!hwnd)
        return false;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return true;
}
