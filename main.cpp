#define UNICODE
#define USE_MATH_DEFINES

#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
}

class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;

    static void init_message_map() {
        message_map.AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy);
    }

    static void DeleteGdiObjects() {
    }

    static int main_loop() {
        MSG message;
        BOOL result;
        while ((result = GetMessage(&message, nullptr, 0, 0))) {
            if (result == -1) throw std::runtime_error("Critical error");
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        DeleteGdiObjects();

        return (int) message.wParam;
    }

public:
    static int main(HINSTANCE hInstance, int nCmdShow) {
        hinst = hInstance;
        init_message_map();

        WinApi::WindowClass(hinst, consts::main_class_name, wnd_proc).Register();
        WinApi::Window().create_and_show(hinst, consts::main_class_name, consts::title, nCmdShow);
        return main_loop();
    }


    static LRESULT on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
    }

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}