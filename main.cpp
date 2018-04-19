#define USE_MATH_DEFINES

#include "Template.h"
#include <iostream>
#include <cmath>


namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
}


class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static WinApi::MenuClass menu_class;
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

        auto hdc = WinApi::DoubleBuffering(hwnd);
        auto rect = WinApi::GetClientRect(hwnd);
        WinApi::ClearBackgroud(hdc, rect);

        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }

    static HWND CreateControl(const wchar_t *className, long style, int x, int y, int width, int height, int id,
                              LPCWSTR name = L"") {
        return CreateWindow(
                className, name,
                WS_CHILD | WS_VISIBLE | style,
                x, y,
                width, height,
                _hdialog,
                (HMENU) id,
                hinst,
                nullptr);
    }    //!style|WS_VISIBLE|WS_CHILD

    static HWND
    CreateEditBox(int x, int y, int id, int width = 100, int height = 20, long style = 0, LPCWSTR str = L"") {
        auto new_edit_box = CreateControl(WC_EDIT, WS_TABSTOP | WS_BORDER | ES_CENTER | style, x, y, width, height,
                                          id, str);
        return new_edit_box;
    }


    static HWND CreateComboBox(int x, int y, int id, int width = 100, int height = 100, long style = 0) {

        auto combo_box = CreateControl(WC_COMBOBOX, CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP | style,
                                       x, y, width, height, id);
        return combo_box;
    }

    static HWND CreateSlider(int min_val, int max_val, int id, int style = 0) {
        auto slider = CreateControl(TRACKBAR_CLASS, WS_TABSTOP | TBS_AUTOTICKS | TBS_HORZ, 240, 160, 150, 32, id);

        SendMessage(slider, TBM_SETRANGE, TRUE, MAKELONG(min_val, max_val));
        SendMessage(slider, TBM_SETPOS, TRUE, 0);

        return slider;
    }

    static void init_dialog(HWND hwnd) {
        SetWindowLong(hwnd, GWL_STYLE, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU);
        SetWindowPos(_hdialog, nullptr, 0, 0, 500, 250, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowText(_hdialog, consts::title);
    }

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
WinApi::MenuClass Program::menu_class;
HWND Program::_hdialog;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}