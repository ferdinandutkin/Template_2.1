#define USE_MATH_DEFINES

#include "Template.h"
#include "Vector.h"
#include <iostream>
#include <dshow.h>


namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";

    const int min_radius = 10;
    const int max_radius = 30;
    const double def_speed = 3;
    const double shift_accel = 2;
    const double ctrl_shift_accel = 1.0 / 1.5;
}

namespace colors {
    const COLORREF red = RGB(255, 0, 0);
    const COLORREF green = RGB(0, 255, 0);
    const COLORREF blue = RGB(0, 0, 255);
    const COLORREF yellow = RGB(0, 255, 255);
    const COLORREF black = RGB(0, 0, 0);

}

namespace keys {
    const auto W = 0x57;
    const auto A = 0x41;
    const auto S = 0x53;
    const auto D = 0x44;
    const auto P = 0x50;
}

using namespace WinApi;

class Program {
    class Ball {
        int radius;
        Vector<int> position;
        COLORREF color;
    public:
        COLORREF get_color() const {
            return color;
        }

    private:

        static void return_to_rect(Vector<int> &point, RECT rect, int radius) {
            point.x = std::min(point.x, static_cast<int>(rect.right - radius));
            point.x = std::max(point.x, static_cast<int>(rect.left + radius));
            point.y = std::min(point.y, static_cast<int>(rect.bottom - radius));
            point.y = std::max(point.y, static_cast<int>(rect.top + radius));
        }

    public:
        Ball() : radius(0), position(), color(colors::black) {}

        void init_ball(RECT rect) {
            color = colors::red;
            position = Vector<int>(random(rect.left + radius, rect.right - radius),
                                   random(rect.top + radius, rect.bottom - radius));
            radius = random(consts::min_radius, consts::max_radius);
        }

        void move_ball(Vector<int> direction, RECT rect) {
            position = position + direction;
            return_to_rect(position, rect, radius);
        }

        void set_color(COLORREF _color) {
            color = _color;
        }

        std::pair<Vector<int>, int> get_position_and_radius() {
            return std::make_pair(position, radius);
        }

        void draw_ball(HDC hdc, RECT rect) {
            return_to_rect(position, rect, radius);
            auto brush = CreateSolidBrush(color);
            brush = (HBRUSH) SelectObject(hdc, brush);
            Ellipse(hdc, position.x - radius, position.y - radius, position.x + radius, position.y + radius);
            DeleteObject(SelectObject(hdc, brush));
        }
    };


    static MessageMap message_map;
    static DialogMessageMap dialog_message_map;
    static HINSTANCE hinst;
    static MenuClass menu_class;
    static HWND _hdialog;
    static Ball ball;
    static bool pause;
    static bool need_to_change_color_to_yellow;

    static void init_message_map() {
        message_map
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_KEYDOWN, on_key_down)
                .AddHandler(WM_MOUSEMOVE, on_mouse_move)
                .AddHandler(WM_LBUTTONDOWN, on_mouse_left_button_down)
                .AddHandler(WM_LBUTTONUP, on_mouse_left_button_up)
                .AddHandler(WM_GETMINMAXINFO, on_min_max_info);
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

    static bool check_pause(HDC hdc, RECT rect);

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

        ball.init_ball(GetClientRect(hwnd));
    }

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        auto hdc = DoubleBuffering(hwnd);
        auto rect = GetClientRect(hwnd);
        ClearBackgroud(hdc, rect);

        check_pause(hdc, rect);

        ball.draw_ball(hdc, rect);
        return 0;
    }

    static void on_key_down(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (pause && wparam != keys::P)
            return;
        auto rect = GetClientRect(hwnd);
        auto shift = static_cast<bool>(GetKeyState(VK_SHIFT) & (0x8000));
        auto ctrl = static_cast<bool>(GetKeyState(VK_CONTROL) & (0x8000));

        auto speed = consts::def_speed * ((!shift) + (shift * (!ctrl) * consts::shift_accel) +
                                          (shift * ctrl * consts::ctrl_shift_accel));

        switch (wparam) {
            case (keys::W):
            case VK_UP: {
                ball.move_ball(Vector<int>(0, static_cast<int>(-1 * speed)), rect);
                break;
            }

            case (keys::A):
            case VK_LEFT: {
                ball.move_ball(Vector<int>(static_cast<int>(-1 * speed), 0), rect);
                break;
            }

            case (keys::S):
            case VK_DOWN: {
                ball.move_ball(Vector<int>(0, static_cast<int>(speed)), rect);
                break;
            }


            case (keys::D):
            case VK_RIGHT: {
                ball.move_ball(Vector<int>(static_cast<int>(speed), 0), rect);
                break;
            }

            case (keys::P): {
                pause = !pause;
                POINT cursor_position;
                GetCursorPos(&cursor_position);
                ScreenToClient(hwnd, &cursor_position);
                on_mouse_move(hwnd, 0, MAKELONG(cursor_position.x, cursor_position.y));
                break;
            }
            default:
                break;
        }
        InvalidateRect(hwnd, nullptr, false);
    }

    static bool point_in_round(Vector<int> point, Vector<int> round_center, int radius);

    static void on_mouse_move(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (pause)
            return;
        Vector<int> cursor_pos(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        static auto cursor = LoadCursor(nullptr, IDC_HAND);
        static bool was_in_ball = false;
        if (auto[position, radius]=ball.get_position_and_radius(); point_in_round(cursor_pos, position, radius)) {
            SetCursor(cursor);
            if (need_to_change_color_to_yellow && !(GetKeyState(VK_LBUTTON) & 0x8000))
                ball.set_color(colors::yellow);
            was_in_ball = true;
        } else {
            if (need_to_change_color_to_yellow && !(GetKeyState(VK_LBUTTON) & 0x8000) && was_in_ball)
                ball.set_color(colors::red);
            need_to_change_color_to_yellow = true;
            was_in_ball = false;
        }
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_mouse_left_button_down(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (pause)
            return;
        static auto cursor = LoadCursor(nullptr, IDC_HAND);

        Vector<int> cursor_pos(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        if (auto[position, radius]=ball.get_position_and_radius(); point_in_round(cursor_pos, position, radius)) {
            SetCursor(cursor);

            ball.set_color(colors::black);
            need_to_change_color_to_yellow = false;
        }
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_mouse_left_button_up(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        static auto cursor = LoadCursor(nullptr, IDC_HAND);
        if (pause)
            return;
        Vector<int> cursor_pos(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
        if (auto[position, radius]=ball.get_position_and_radius(); point_in_round(cursor_pos, position, radius)) {
            SetCursor(cursor);
            if (ball.get_color() != colors::red)
                ball.set_color(colors::red);
            else if (wparam & MK_SHIFT)
                ball.set_color(colors::blue);
            else
                ball.set_color(colors::green);
            need_to_change_color_to_yellow = false;
        }
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_min_max_info(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto min_max_info = (MINMAXINFO *) lparam;
        RECT r;
        GetWindowRect(hwnd, &r);
        min_max_info->ptMinTrackSize.x = min_max_info->ptMinTrackSize.y =
                consts::max_radius * 2 + (GetRectHeigth(r) - GetRectHeigth(GetClientRect(hwnd)));
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
Program::Ball Program::ball;
bool Program::pause = false;
bool Program::need_to_change_color_to_yellow = true;

bool Program::check_pause(HDC hdc, RECT rect) {
    if (pause) {
        DrawText(hdc, L"Пауза", -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        return true;
    }
    return false;
}

bool Program::point_in_round(Vector<int> point, Vector<int> round_center, int radius) {
    return (point - round_center).length() < radius;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}