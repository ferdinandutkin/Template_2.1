#define USE_MATH_DEFINES

#include "Template.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <dshow.h>


namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    const int speed = 100;
    const int radius = 10;
    const int min_radius = 10;
    const int max_radius = 100;
    const int max_speed = 500;
    const int min_speed = 10;

}

enum id {
    id_timer = 1001, id_dialog_edit_width, id_dialog_edit_height, id_dialog_button_ok, id_dialog_button_cancel
};
using namespace WinApi;


class Program {
    static MessageMap message_map;
    static DialogMessageMap dialog_message_map;
    static HINSTANCE hinst;
    static MenuClass menu_class;
    static HWND _hdialog;
    static POINT min_window_size, max_window_size, cur_size;
    static int count;

    class Round {
        struct Point {
            double x;
            double y;

            Point(double x, double y) : x(x), y(y) {}

            Point() : x(0), y(0) {}
        };

        Point cur_pos;
        int V;
        int radius;
        Point ort;
        double alpha;
        COLORREF color;
        time_t cur_time;

        bool reflection(RECT rect) {
            //левая граница
            bool if_reflected = false;
            if (cur_pos.x <= rect.left) {
                if (ort.x < 0) {
                    if_reflected = true;
                    ort.x *= -1;
                    color = GetRandColor();
                }
            }

            if (cur_pos.x + 2 * radius >= rect.right) {
                if (ort.x > 0) {
                    if_reflected = true;
                    ort.x *= -1;
                    color = GetRandColor();
                }

            }
            if (cur_pos.y <= rect.top) {
                if (ort.y < 0) {
                    if_reflected = true;
                    color = GetRandColor();
                    ort.y *= -1;
                }
            }

            if (cur_pos.y + 2 * radius >= rect.bottom) {
                if (ort.y > 0) {
                    if_reflected = true;
                    color = GetRandColor();
                    ort.y *= -1;
                }
            }
            return if_reflected;
        }

        int rand_in_diaposon(int from, int to) {
            return rand() % (to - from + 1) + from;

        }


    public:
        void print_round(HDC hdc) {
            auto hbrush = CreateSolidBrush(color);
            hbrush = (HBRUSH) SelectObject(hdc, hbrush);
            Ellipse(hdc, cur_pos.x, cur_pos.y, cur_pos.x + 2 * radius, cur_pos.y + 2 * radius);
            DeleteObject(SelectObject(hdc, hbrush));
        }

        void move(RECT rect) {
            time_t next_time = clock();
            time_t delta_time = next_time - cur_time;
            cur_pos.x += ort.x * V * delta_time / CLOCKS_PER_SEC;
            cur_pos.y += ort.y * V * delta_time / CLOCKS_PER_SEC;

            cur_time = next_time;

            cur_pos.x = std::min(cur_pos.x, 1.0 * rect.right - 2 * radius);
            cur_pos.x = std::max(cur_pos.x, 1.0 * rect.left);

            cur_pos.y = std::min(cur_pos.y, 1.0 * rect.bottom - 2 * radius);
            cur_pos.y = std::max(cur_pos.y, 1.0 * rect.top);

            reflection(rect);
        }

        void init_round(RECT rect, int _radius = consts::radius, int _speed = consts::speed) {
            static bool inited = false;
            if (inited)
                return;
            inited = true;
            radius = _radius;
            cur_pos = Point(rand() % (rect.right - radius * 2 + 1), rand() % (rect.bottom - 2 * radius + 1));
            V = _speed;
            color = GetRandColor();
            alpha = (rand() % 360) * M_PI / 180;
            cur_time = clock();
            ort = Point(cos(alpha), sin(alpha));
        }

        std::pair<POINT, int> get_centre_and_radius() {
            POINT center = {cur_pos.x + radius, cur_pos.y + radius};
            return std::make_pair(center, radius);
        }


        void randomize_position(RECT rect) {
            radius = rand_in_diaposon(consts::min_radius, std::min(consts::max_radius,
                                                                   std::min((GetRectHeigth(rect) - 20) / 2,
                                                                            (GetRectWidth(rect) - 20) / 2)));
            cur_pos.x = rand_in_diaposon(rect.left, rect.right - 2 * radius);
            cur_pos.y = rand_in_diaposon(rect.top, rect.bottom - 2 * radius);

            alpha = rand_in_diaposon(0, 359) * M_PI / 180;
            ort = Point(cos(alpha), sin(alpha));

            V = rand_in_diaposon(consts::min_speed, consts::max_speed);
            color = GetRandColor();
        }

    };

    static Round round;

    static double get_distance(POINT point1, POINT point2) {
        return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
    }

    static void init_message_map() {
        message_map
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_GETMINMAXINFO, on_get_min_max_info)
                .AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_TIMER, on_timer)
                .AddHandler(WM_ERASEBKGND, DoubleBuffering::on_clear_background)
                .AddHandler(WM_LBUTTONDOWN, on_lbuttondown);
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

        SetTimer(hwnd, id_timer, 10, nullptr);
        menu_class
                .CreateMenuClass(hwnd, message_map)
                .AddMenuItem(0, settings_menu_handler, L"&Меню")
                .SetMenu();
        auto r = GetWindowRect(hwnd);
        cur_size.x = GetRectWidth(r);
        cur_size.y = GetRectHeigth(r);
    }


    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        auto hdc = DoubleBuffering(hwnd);
        auto rect = GetClientRect(hwnd);
        ClearBackgroud(hdc, rect);

        round.init_round(rect);

        round.move(rect);
        round.print_round(hdc);


        return 0;
    }

    static LRESULT on_get_min_max_info(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto minmaxinfo = ((MINMAXINFO *) lparam);
        minmaxinfo->ptMinTrackSize.x = minmaxinfo->ptMinTrackSize.y =
                GetRectHeigth(GetWindowRect(hwnd)) - GetRectHeigth(GetClientRect(hwnd)) + 2 * consts::max_radius + 10;
        min_window_size = minmaxinfo->ptMinTrackSize;
        max_window_size = minmaxinfo->ptMaxTrackSize;
        return 0;
    }

    static void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (wparam == id_timer)
            InvalidateRect(hwnd, nullptr, false);
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }


    static void on_lbuttondown(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto[center, radius] = round.get_centre_and_radius();
        if (get_distance(center, {LOWORD(lparam), HIWORD(lparam)}) <= radius) {
            round.randomize_position(GetClientRect(hwnd));
            count++;
        } else {
            std::wstringstream ss;
            ss << count;
            std::wstring str;
            ss >> str;
            if (MessageBox(hwnd, L"GAME OVER", str.c_str(), MB_ICONHAND | MB_RETRYCANCEL) == IDRETRY)
                count = 0;
            else
                DestroyWindow(hwnd);
        }

    }


    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
    //////////////////////////////////////////  menu handlers  /////////////////////////////////////////////////////////

    static void settings_menu_handler(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto r = GetWindowRect(hwnd);
        cur_size.x = GetRectWidth(r);
        cur_size.y = GetRectHeigth(r);
        call_dialog(hwnd);
        SetWindowPos(hwnd, nullptr, 0, 0, cur_size.x, cur_size.y, SWP_NOMOVE | SWP_SHOWWINDOW);
    }

    /////////////////////////////////////  end of menu handlers  ///////////////////////////////////////////////////////


    ////////////////////////////////////////  next functions for dialog  ///////////////////////////////////////////////

    static void init_dialog_message_map() {

        dialog_message_map
                .AddHandler(WM_INITDIALOG, on_dialog_initialise)
                .AddHandler(WM_DISPLAYCHANGE, on_dialog_update)
                .AddHandler(WM_CLOSE, on_dialog_close)
                .AddCommandHandler(id_dialog_button_cancel, on_dialog_close)
                .AddCommandHandler(id_dialog_button_ok, on_dialog_ok)
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

        std::wstring edit_width, edit_height;
        std::wstringstream ss;
        ss << cur_size.x << L" " << cur_size.y;
        ss >> edit_width >> edit_height;

        InitialiseDialogAndControlls(hinst, hwnd)
                .init_dialog()
                .CreateStaticBox({20, 20}, 100, 20, 0, L"Ширина")
                .CreateEditBox({140, 20}, id_dialog_edit_width, 100, 20, ES_NUMBER, edit_width.c_str())

                .CreateStaticBox({20, 60}, 100, 20, 0, L"Высота")
                .CreateEditBox({140, 60}, id_dialog_edit_height, 100, 20, ES_NUMBER, edit_height.c_str())
                .CreatePushButton({360, 180}, id_dialog_button_ok, 60, 20, 0, L"Ok")
                .CreatePushButton({430, 180}, id_dialog_button_cancel, 60, 20, 0, L"Отмена");


    }

    static void on_dialog_update(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_dialog_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void on_dialog_ok(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        wchar_t inf_edit_width[100];
        wchar_t inf_edit_height[100];

        Edit_GetText(GetDlgItem(hwnd, id_dialog_edit_width), inf_edit_width, 8);
        Edit_GetText(GetDlgItem(hwnd, id_dialog_edit_height), inf_edit_height, 8);

        std::wstringstream ss;
        ss << inf_edit_width << L" " << inf_edit_height;

        POINT window_size;
        ss >> window_size.x >> window_size.y;

        if (window_size.x >= min_window_size.x && window_size.x <= max_window_size.x &&
            window_size.y >= min_window_size.y && window_size.y <= max_window_size.y) {
            cur_size = window_size;
        } else
            MessageBox(hwnd, L"Invalid data", L"Ошибка", MB_ICONERROR | MB_OK);

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
POINT Program::min_window_size, Program::max_window_size, Program::cur_size;
Program::Round Program::round;
int Program::count = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}