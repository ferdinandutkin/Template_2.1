#define USE_MATH_DEFINES

#include "Template.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <dshow.h>

const int VK_P = 0x50;
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
    id_timer = 1001,
    id_dialog_edit_width,
    id_dialog_edit_height,
    id_dialog_button_ok,
    id_dialog_button_cancel,
    id_stat_ok
};
using namespace WinApi;


class Program {
    static MessageMap message_map;
    static DialogMessageMap dialog_message_map;
    static DialogMessageMap stat_dialog_message_map;
    static HINSTANCE hinst;
    static MenuClass menu_class;
    static HWND _hdialog;
    static HWND _h_stat_dialog;

    static POINT min_window_size, max_window_size, cur_size;
    static int count;
    static bool rects[21][21];
    static RECT field;
    static int N, M;
    static bool pause, game_over;

    struct Statistics {
        unsigned int time;
        unsigned int count;
    };

    static std::vector<Statistics> statistics;
    static time_t start_time, pause_time, start_pause;

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
        COLORREF color;
        time_t cur_time;

        bool reflection(RECT rect) {
            //левая граница
            bool if_reflected = false;
            int W = GetRectWidth(rect), H = GetRectHeigth(rect);
            int size_of_kletka = std::min(W / N, H / M);
            int kletka_number_x = (int) (((cur_pos.x + radius - rect.left) / size_of_kletka));
            int kletka_number_y = (int) (((-1 * cur_pos.y + radius + rect.bottom) / size_of_kletka));
            radius = size_of_kletka / 2;

            int kletka1_number_x = (int) (((cur_pos.x - rect.left) / size_of_kletka));
            int kletka1_number_y = (int) (((-1 * cur_pos.y + rect.bottom) / size_of_kletka));


            int kletka2_number_x = (int) (((cur_pos.x - rect.left) / size_of_kletka));
            int kletka2_number_y = (int) (((-1 * cur_pos.y + rect.bottom + size_of_kletka) / size_of_kletka));

            int kletka3_number_x = (int) (((cur_pos.x - rect.left + size_of_kletka) / size_of_kletka));
            int kletka3_number_y = (int) (((-1 * cur_pos.y + rect.bottom) / size_of_kletka));

            int kletka4_number_x = (int) (((cur_pos.x - rect.left + size_of_kletka) / size_of_kletka));
            int kletka4_number_y = (int) (((-1 * cur_pos.y + rect.bottom + size_of_kletka) / size_of_kletka));

            if (rects[kletka1_number_x][kletka1_number_y] || rects[kletka2_number_x][kletka2_number_y] ||
                rects[kletka3_number_x][kletka3_number_y] || rects[kletka4_number_x][kletka4_number_y]) {
                cur_pos.x = kletka_number_x * size_of_kletka + rect.left;
                cur_pos.y = rect.bottom - kletka_number_y * size_of_kletka;
                V = 0;
                rects[kletka_number_x][kletka_number_y] = true;
            }

            if (cur_pos.x <= rect.left && !rects[kletka_number_x][kletka_number_y]) {
                if_reflected = true;
                ort.x *= -1;
                color = GetRandColor();

            }

            if (cur_pos.x + 2 * radius >= rect.right) {
                if_reflected = true;
                ort.x *= -1;
                color = GetRandColor();

            }

            if (cur_pos.y <= rect.top) {
                V = 0;
                rects[kletka_number_x][M] = true;
                cur_pos.x = rect.left + kletka_number_x * size_of_kletka;
                return 0;
            }
//            if ((ort.x < 0 && rects[kletka_number_x + 1][kletka_number_y]) ||
//                (ort.x > 0 && kletka_number_x > 0 && rects[kletka_number_x - 1][kletka_number_y]) ||
//                (rects[kletka_number_x][kletka_number_y + 1])) {
//                V = 0;
//                rects[kletka_number_x][kletka_number_y] = true;
//                cur_pos.x = rect.left + kletka_number_x * size_of_kletka;
//                cur_pos.y = rect.bottom - kletka_number_y * size_of_kletka;
//
//                return 0;
//            }
//            if((ort.x < 0 && rects[kletka_number_x + 1][kletka_number_y+1]) ||
//            (ort.x > 0 && kletka_number_x > 0 && rects[kletka_number_x - 1][kletka_number_y+1])){
//                V = 0;
//                rects[kletka_number_x][kletka_number_y+1] = true;
//                cur_pos.x = rect.left + kletka_number_x * size_of_kletka;
//                cur_pos.y = rect.bottom - (kletka_number_y+1) * size_of_kletka;
//                return 0;
//            }
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
            cur_pos.x -= ort.x * V * delta_time / CLOCKS_PER_SEC;
            cur_pos.y -= ort.y * V * delta_time / CLOCKS_PER_SEC;

            cur_time = next_time;

            cur_pos.x = std::min(cur_pos.x, 1.0 * rect.right - 2 * radius);
            cur_pos.x = std::max(cur_pos.x, 1.0 * rect.left);

            cur_pos.y = std::min(cur_pos.y, 1.0 * rect.bottom - 2 * radius);
            cur_pos.y = std::max(cur_pos.y, 1.0 * rect.top);

            reflection(rect);
        }

        void init_round(RECT rect, int _radius, std::pair<double, double> _ort = std::make_pair(1, 0),
                        int _speed = consts::speed) {
//            bool inited = false;
//            if (inited)
//                return;
//            inited = true;
            radius = _radius;
            cur_pos = Point(GetRectWidth(field) / 2 - radius + rect.left, rect.bottom - 2 * radius);
            V = _speed;
            color = GetRandColor();
            cur_time = clock();
            ort = Point(_ort.first, _ort.second);
        }


        void set_time(time_t time) {
            cur_time = time;
        }

    };

    static void make_pause() {
        if (game_over)
            return;
        start_pause = clock();
        pause = true;
    }

    static void resume() {
        if (game_over)
            return;
        time_t time = clock();
        for (auto &i:rounds) {
            i.set_time(time);
        }
        pause_time += clock() - start_pause;
        pause = false;
    }

    static std::vector<Round> rounds;

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
                .AddHandler(WM_KEYDOWN, on_key_down)
                .AddHandler(WM_ERASEBKGND, DoubleBuffering::on_clear_background);
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
                .AddPopupMenuItem(123, L"&Файл")
                .AddMenuItem(0, settings_menu_handler, L"&Настройки", 123)
                .AddMenuItem(0, statisticks_menu_handler, L"&Статистика")
                .SetMenu();
        auto r = GetWindowRect(hwnd);
        cur_size.x = GetRectWidth(r);
        cur_size.y = GetRectHeigth(r);
        start_time = clock();
    }


    static LRESULT on_key_down(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        switch (wparam) {
            case VK_P: {
                if (pause)
                    resume();
                else {
                    if (game_over)
                        break;
                    make_pause();
                    auto hdc = GetDC(hwnd);
                    auto rect = GetClientRect(hwnd);
                    DrawText(hdc, L"Pause", -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                    ReleaseDC(hwnd, hdc);
                }
                break;
            }
            case VK_F5: {
                restart_game();
                break;
            }
            case VK_F2:
                make_pause();
                call_stat_dialog(hwnd);
                resume();
            default:
                break;
        }
    }

    static void print_kletki(HDC hdc, RECT rect, int kletka_size);

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (pause || game_over)
            return 0;

        static time_t last_call_time = clock();

        auto hdc = DoubleBuffering(hwnd);
        auto rect = GetClientRect(hwnd);
        for (int i = 0; i < 21; i++)
            if (rects[i][1]) {
                game_over = true;
                int cnt = 0;
                for (int r = 0; r < M; r++) {
                    for (int j = 0; j < N; j++)
                        if (rects[j][r]) {
                            cnt++;
                            break;
                        }
                }

                Statistics stat = {static_cast<unsigned int>((clock() - start_time - pause_time) / CLOCKS_PER_SEC),
                                   static_cast<unsigned int>(cnt)};
                statistics.push_back(stat);
                make_pause();
                DrawText(hdc, L"Game Over", -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
                return 0;
            }
        ClearBackgroud(hdc, rect);

        int W = GetRectWidth(rect), H = GetRectHeigth(rect);
        int size_of_kletka = std::min(W / N, H / M);
        int otstup_x = (W - size_of_kletka * N) / 2, otstup_y = (H - size_of_kletka * M) / 2;
        SetRect(&field, otstup_x, otstup_y, rect.right - otstup_x, rect.bottom - otstup_y);
        print_kletki(hdc, field, size_of_kletka);
        time_t cur_time = clock();
        static int cnt = 0;
        cnt++;

        if (cur_time - last_call_time >= 2 * CLOCKS_PER_SEC) {
            cnt = 0;
            POINT cursor;
            GetCursorPos(&cursor);
            ScreenToClient(hwnd, &cursor);
            cursor.y = field.bottom - size_of_kletka / 2 - cursor.y;
            cursor.x = (rect.right) / 2 - cursor.x;

            double l = sqrt(cursor.x * cursor.x + cursor.y * cursor.y);
//            std::cout << cursor.x << ' ' << cursor.y;
            std::pair<double, double> cursor_ort(1.0 * cursor.x / l, 1.0 * cursor.y / l);
            if (cursor_ort.second <= 0)
                cursor_ort = std::make_pair(0, 1);
            Round round;
            round.init_round(field, size_of_kletka / 2, cursor_ort);
            last_call_time = cur_time;
            rounds.push_back(round);
        }

        for (auto &round:rounds) {
            round.move(field);
            round.print_round(hdc);
        }


        return 0;
    }

    static void restart_game() {
        rounds.clear();

        if (!game_over) {
            int cnt = 0;
            for (int r = 0; r < M; r++) {
                for (int j = 0; j < N; j++)
                    if (rects[j][r]) {
                        cnt++;
                        break;
                    }
            }


            Statistics stat = {static_cast<unsigned int>((clock() - start_time) / CLOCKS_PER_SEC),
                               static_cast<unsigned int>(cnt)};
            statistics.push_back(stat);
        }
        for (int i = 0; i < 21; i++)
            for (int j = 0; j < 21; j++)
                rects[i][j] = false;
        start_time = clock();
        pause = false;
//        pause_time=0;

//        я верю, что остановка - тоже часть пути, так что я убираю этот костыль
        game_over = false;
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


    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
    //////////////////////////////////////////  menu handlers  /////////////////////////////////////////////////////////

    static void settings_menu_handler(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        call_dialog(hwnd);
    }

    static void statisticks_menu_handler(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        make_pause();
        call_stat_dialog(hwnd);
        resume();
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
        make_pause();
        init_dialog_message_map();
        HCreateDialog(hinst, hwnd, dialog_proc);
        dialog_loop();
    }

    // "on_dialog" functions
    static void on_dialog_initialise(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _hdialog = hwnd;

        std::wstring edit_width, edit_height;
        std::wstringstream ss;
        ss << N << L" " << M;
        ss >> edit_width >> edit_height;

        InitialiseDialogAndControlls(hinst, hwnd)
                .init_dialog()
                .CreateStaticBox({20, 20}, 100, 20, 0, L"N")
                .CreateEditBox({140, 20}, id_dialog_edit_width, 100, 20, ES_NUMBER, edit_width.c_str())

                .CreateStaticBox({20, 60}, 100, 20, 0, L"M")
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

        if (window_size.x >= 0 && window_size.x <= 20 &&
            window_size.y >= 0 && window_size.y <= 20) {
            N = window_size.x;
            M = window_size.y;
            restart_game();
        } else
            MessageBox(hwnd, L"Invalid data", L"Ошибка", MB_ICONERROR | MB_OK);
        DestroyWindow(hwnd);
    }


    static void on_dialog_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        resume();
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


    static void init_stat_dialog_message_map() {

        stat_dialog_message_map
                .AddHandler(WM_INITDIALOG, on_stat_dialog_initialise)
                .AddHandler(WM_DISPLAYCHANGE, on_stat_dialog_update)
                .AddHandler(WM_PAINT, on_stat_dialog_paint)
                .AddCommandHandler(id_stat_ok, on_stat_dialog_close)
                .AddHandler(WM_CLOSE, on_stat_dialog_close)
                .AddHandler(WM_DESTROY, on_stat_dialog_destroy);
    }

    static void stat_dialog_loop() {
        MSG message;
        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_h_stat_dialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }

    }

    static void call_stat_dialog(HWND hwnd) {
        make_pause();
        init_stat_dialog_message_map();
        HCreateDialog(hinst, hwnd, stat_dialog_proc);
        stat_dialog_loop();
    }

    // "on__stat_dialog" functions
    static void on_stat_dialog_initialise(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _h_stat_dialog = hwnd;

        InitialiseDialogAndControlls(hinst, hwnd)
                .init_dialog({0, 0}, 500, 400, L"Statistics")
                .CreatePushButton({430, 350}, id_stat_ok, 60, 20, 0, L"Ok");
    }

    static void on_stat_dialog_update(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_stat_dialog_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }


    static void on_stat_dialog_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto hdc = DoubleBuffering(hwnd);
        unsigned int sum_time = 0, max_level = 0;
        for (auto i:statistics) {
            max_level = std::max(max_level, i.count);
            sum_time += i.time;
        }
        auto rect = GetClientRect(hwnd);
        rect.bottom -= 40;
        double mx = 1.0 * (GetRectWidth(rect) - 40) / sum_time;
        double my = 1.0 * GetRectHeigth(rect) / max_level;
        double cur_left_pos = 0;
        for (auto i:statistics) {
            auto brush = CreateSolidBrush(GetRandColor());
            brush = (HBRUSH) SelectObject(hdc, brush);
            Rectangle(hdc, cur_left_pos, rect.bottom - i.count * my - 20, cur_left_pos + mx * i.time, rect.bottom - 20);
            cur_left_pos += mx * i.time;
            DeleteObject(SelectObject(hdc, brush));
        }
    }


    static void on_stat_dialog_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        resume();
        PostQuitMessage(0);
    }



/*
    static void on_dialog_(HWND hwnd,WPARAM wparam,LPARAM lparam) {}
*/
    // end of "on_dialog" functions

    static INT_PTR CALLBACK stat_dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        stat_dialog_message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

};

MessageMap Program::message_map;
DialogMessageMap Program::dialog_message_map;
DialogMessageMap Program::stat_dialog_message_map;

HINSTANCE Program::hinst;
MenuClass Program::menu_class;
HWND Program::_hdialog;
HWND Program::_h_stat_dialog;

POINT Program::min_window_size, Program::max_window_size, Program::cur_size;
std::vector<Program::Round> Program::rounds;
int Program::count = 0;
bool Program::rects[21][21] = {0};
RECT Program::field;
int Program::N = 5, Program::M = 10;
bool Program::pause = false, Program::game_over = false;
std::vector<Program::Statistics> Program::statistics;
time_t Program::start_time, Program::pause_time = 0, Program::start_pause;


void Program::print_kletki(HDC hdc, RECT rect, int kletka_size) {
    for (int i = 0; i <= N; i++) {
        MoveToEx(hdc, rect.left + i * kletka_size, rect.top, nullptr);
        LineTo(hdc, rect.left + i * kletka_size, rect.bottom);
    }
    for (int i = 0; i <= M; i++) {
        MoveToEx(hdc, rect.left, rect.top + i * kletka_size, nullptr);
        LineTo(hdc, rect.right, rect.top + i * kletka_size);
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}