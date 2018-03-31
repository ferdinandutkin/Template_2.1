//
// Created by serko on 31.03.2018.
//

#ifndef UNTITLED7_WINDOWCLASS_H
#define UNTITLED7_WINDOWCLASS_H


#include <windows.h>

namespace WinApi {
    class WindowClass {
        WNDCLASSEXW wcex{};
    public:
        WindowClass(HINSTANCE hInstance, LPCWSTR className, WNDPROC windowProcedure);

        ATOM Register();
    };


}
#endif //UNTITLED7_WINDOWCLASS_H
