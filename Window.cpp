//
// Created by serko on 31.03.2018.
//

#include <iostream>
#include "Window.h"

bool
WinApi::Window::create_and_show(HINSTANCE hInstance, std::wstring window_class, std::wstring title, int nCmdShow,
                                DWORD style) {
    auto hwnd = CreateWindowW(window_class.c_str(),
                              title.c_str(),
                              style,
                              CW_USEDEFAULT, 0,
                              CW_USEDEFAULT, 0,
                              nullptr,
                              nullptr,
                              hInstance,
                              nullptr);
//std::wcout<<title<<' '<<window_class;
    if (!hwnd)
        return false;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return true;
}
