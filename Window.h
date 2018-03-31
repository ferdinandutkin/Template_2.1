//
// Created by serko on 31.03.2018.
//

#ifndef UNTITLED7_WINDOW_H
#define UNTITLED7_WINDOW_H

#include <windows.h>

namespace WinApi {
    class Window {
    public:
        bool create_and_show(HINSTANCE hInstance, LPCWSTR windowClass, LPCWSTR title, int nCmdShow,
                             DWORD style = WS_OVERLAPPEDWINDOW);
    };
}

#endif //UNTITLED7_WINDOW_H
