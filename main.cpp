#define USE_MATH_DEFINES

#include "Template.h"
#include <iostream>
#include <cmath>


namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
}

using namespace WinApi;

class Program {
    static MessageMap message_map;
    static DialogMessageMap dialog_message_map;
    static HINSTANCE hinst;
    static MenuClass menu_class;
    static HWND _hdialog;

    static void init_message_map() {
        message_map
                .AddHandler(WM_CREATE, on_create)
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
        menu_class
                .CreateMenuClass(hwnd, message_map)

                .SetMenu();
    }


    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        auto hdc = DoubleBuffering(hwnd);
        auto rect = GetClientRect(hwnd);
        ClearBackgroud(hdc, rect);
        call_dialog(hwnd);

        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }


    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }



    ////////////////////////////////////////  next functions for dialog  ///////////////////////////////////////////////

    static void init_dialog_message_map() {
        dialog_message_map
                .AddHandler(WM_INITDIALOG, on_dialog_initialise)
                .AddHandler(WM_DISPLAYCHANGE, on_dialog_update)
                .AddHandler(WM_CLOSE, on_dialog_close)
                .AddHandler(WM_DESTROY, on_dialog_destroy);
    }

    static void dialog_loop() {
        MSG message;
        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_hdialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }

    }

    static void call_dialog(HWND hwnd) {
        init_dialog_message_map();
        HCreateDialog(hinst, hwnd, dialog_proc);
        dialog_loop();
    }

    // "on_dialog" functions
    static void on_dialog_initialise(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _hdialog = hwnd;
        InitialiseDialogAndControlls(hinst, hwnd)
                .init_dialog()
                .CreateEditBox({1, 2}, 123, 100, 20);
    }

    static void on_dialog_update(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_dialog_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void on_dialog_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }



/*
    static void on_dialog_(HWND hwnd,WPARAM wparam,LPARAM lparam) {}
*/
    // end of "on_dialog" functions

    static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        dialog_message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

    ////////////////////////////////////////////////  the end  /////////////////////////////////////////////////////////
};

MessageMap Program::message_map;
DialogMessageMap Program::dialog_message_map;
HINSTANCE Program::hinst;
MenuClass Program::menu_class;
HWND Program::_hdialog;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}