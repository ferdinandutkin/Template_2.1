//
// Created by serko on 31.03.2018.
//

#ifndef UNTITLED7_WINDOW_H
#define UNTITLED7_WINDOW_H

#include <windows.h>
#include <string>
namespace WinApi {
    class Window {
    public:
        bool create_and_show(HINSTANCE hInstance, std::wstring window_class, std::wstring title, int nCmdShow,
                             DWORD style = WS_OVERLAPPEDWINDOW);
    };
}

#endif //UNTITLED7_WINDOW_H
