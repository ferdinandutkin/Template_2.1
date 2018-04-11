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
    const int id_timer = 1002;
    const int radius = 5;
}

enum Menu {
    menu_calc_sum = 101, menu_task_2, menu_task_3, menu_task_4, menu_task_5
};

class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static int a, b;
    static bool need_to_upd, button_pressed;
    static int x0, y0, cur_x, cur_y, n, m;
    static int cur_task;
    static HMENU h_main_menu;
    static const std::vector<void (*)(HWND, HDC)> task_handlers;
    static std::wstring input_str;

    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_COMMAND, on_command)
                .AddCommandHandler(menu_calc_sum, ch_calc_sum)
                .AddCommandHandler(menu_task_2, ch_task_2)
                .AddCommandHandler(menu_task_3, ch_task_3)
                .AddCommandHandler(menu_task_4, ch_task_4)
                .AddCommandHandler(menu_task_5, ch_task_5)
                .AddHandler(WM_CHAR, on_char)
                .AddHandler(WM_KEYDOWN, on_keydown)
                .AddHandler(WM_LBUTTONDOWN, on_mouse_press)
                .AddHandler(WM_LBUTTONUP, on_mouse_stop_press)
                .AddHandler(WM_MOUSEMOVE, on_mouse_move);
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

    static RECT GetClientRect(HWND hwnd) {
        RECT r;
        ::GetClientRect(hwnd, &r);
        return r;
    }

    static void def_task(HWND hwnd, HDC hdc) {
        auto r = GetClientRect(hwnd);
        FillRect(hdc, &r, WHITE_BRUSH);
    }

    static void task1(HWND hwnd, HDC hdc) {
        def_task(hwnd, hdc);
        auto r = GetClientRect(hwnd);
        wchar_t str[100];
        swprintf(str, 100, L"%d + %d = %d", a, b, a + b);
        DrawText(hdc, str, -1, &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
    }

    static void task2(HWND hwnd, HDC hdc) {
        def_task(hwnd, hdc);
        auto r = GetClientRect(hwnd);
        DrawText(hdc, input_str.c_str(), -1, &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
    }

    static void task3(HWND hwnd, HDC hdc) {
        MoveToEx(hdc, x0, y0, nullptr);
        LineTo(hdc, cur_x, cur_y);
    }

    static void task4(HWND hwnd, HDC hdc) {
        auto r = GetClientRect(hwnd);
        if (n == 0 || m == 0) {
            MessageBox(hwnd, L"Invalid n", L"Error", MB_ICONERROR | MB_OK);
            return;
        }
        int step_x = (r.right - 10) / n;
        if (step_x <= 0) {
            MessageBox(hwnd, L"Invalid n", L"Error", MB_ICONERROR | MB_OK);
            return;
        }
        for (int i = 5; i <= r.right - 5; i += step_x) {
            MoveToEx(hdc, i, r.top + 5, nullptr);
            LineTo(hdc, i, r.bottom - 5);
        }
        int step_y = (r.bottom - 10) / m;
        if (step_y <= 0) {
            MessageBox(hwnd, L"Invalid m", L"Error", MB_ICONERROR | MB_OK);
            return;
        }
        for (int i = 5; i <= r.bottom - 5; i += step_y) {
            MoveToEx(hdc, r.left + 5, i, nullptr);
            LineTo(hdc, r.right - 5, i);
        }
        for (int i = 5; i <= r.right - 5; i += step_x)
            for (int j = 5; j <= r.bottom - 5; j += step_y) {
                auto brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
                brush = (HBRUSH) SelectObject(hdc, brush);
                Ellipse(hdc, i - consts::radius, j - consts::radius, i + consts::radius, j + consts::radius);
                DeleteObject(SelectObject(hdc, brush));
            }


    }


    static void task5(HWND hwnd, HDC hdc) {
        SYSTEMTIME cur_time;
        GetSystemTime(&cur_time);
        auto rect = GetClientRect(hwnd);
        int R = std::min(rect.bottom, rect.right) / 2 - 10;
        POINT center = {rect.right / 2, rect.bottom / 2};

        auto brush = CreateSolidBrush(RGB(250, 250, 0));
        brush = (HBRUSH) SelectObject(hdc, brush);
        Ellipse(hdc, center.x - R - 5, center.y - R - 5, center.x + R + 5, center.y + R + 5);
        DeleteObject(SelectObject(hdc, brush));
        auto pen = CreatePen(PS_ENDCAP_ROUND, 2, RGB(0, 0, 0));
        pen = (HPEN) SelectObject(hdc, pen);
        for (int i = 1; i <= 60; i++) {
            int l = (i % 5 == 0) ? 4 : 2;
            if (i % 5 == 0) {
                wchar_t str[3];
                wsprintf(str, L"%d", i / 5);
                SetBkMode(hdc, TRANSPARENT);
                TextOut(hdc, center.x + (R + 5 - l - 7) * sin((i * 6) * M_PI / 180) - 7,
                        center.y - (R + 5 - l - 7) * cos((i * 6) * M_PI / 180) - 7, str, wcslen(str));
            }

            MoveToEx(hdc, center.x + (R + 5 - l) * cos(i * 6 * M_PI / 180),
                     center.y + (R + 5 - l) * sin(i * 6 * M_PI / 180), nullptr);
            LineTo(hdc, center.x + (R + 5 + l) * cos(i * 6 * M_PI / 180),
                   center.y + (R + 5 + l) * sin(i * 6 * M_PI / 180));
        }
        DeleteObject(SelectObject(hdc, pen));

        pen = CreatePen(PS_ENDCAP_ROUND, 2, RGB(0, 0, 0));
        pen = (HPEN) SelectObject(hdc, pen);
        double angle_sec = (cur_time.wSecond * 6 * M_PI) / 180;
        MoveToEx(hdc, center.x, center.y, nullptr);
        LineTo(hdc, static_cast<int>(center.x + R * sin(angle_sec)), static_cast<int>(center.y - R * cos(angle_sec)));
        DeleteObject(SelectObject(hdc, pen));

        pen = CreatePen(PS_ENDCAP_ROUND, 3, RGB(0, 0, 0));
        pen = (HPEN) SelectObject(hdc, pen);
        double angle_min = (cur_time.wMinute * 6 * M_PI) / 180;
        MoveToEx(hdc, center.x, center.y, nullptr);
        LineTo(hdc, static_cast<int>(center.x + (R - std::min(R - 10, 30)) * sin(angle_min)),
               static_cast<int>(center.y - (R - std::min(R - 10, 30)) * cos(angle_min)));
        DeleteObject(SelectObject(hdc, pen));

        pen = CreatePen(PS_ENDCAP_ROUND, 4, RGB(0, 0, 0));
        pen = (HPEN) SelectObject(hdc, pen);
        double angle_hour = ((cur_time.wHour + 3) * 30 * M_PI) / 180;
        MoveToEx(hdc, center.x, center.y, nullptr);
        LineTo(hdc, static_cast<int>(center.x + (R - std::min(R - 15, 50)) * sin(angle_hour)),
               static_cast<int>(center.y - (R - std::min(R - 15, 50)) * cos(angle_hour)));

        DeleteObject(SelectObject(hdc, pen));
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
        h_main_menu = CreateMenu();
        auto h_file_menu = CreatePopupMenu();
        AppendMenu(h_file_menu, MF_STRING, menu_calc_sum, L"Посчитать сумму");
        AppendMenu(h_file_menu, MF_STRING, menu_task_2, L"Ввести строку");
        AppendMenu(h_file_menu, MF_STRING, menu_task_3, L"Рисовать");
        AppendMenu(h_file_menu, MF_STRING, menu_task_4, L"Решетка");
        AppendMenu(h_file_menu, MF_STRING, menu_task_5, L"Часы");

        AppendMenu(h_main_menu, MF_POPUP | MF_STRING, (UINT_PTR) h_file_menu, L"Choose");
        SetMenu(hwnd, h_main_menu);
    }

    static int GetRectWidth(RECT rect) {
        return rect.right - rect.left;
    }

    static int GetRectHeigth(RECT rect) {
        return rect.bottom - rect.top;
    }

    static HGDIOBJ SelectBufferBitmap(HDC deviceContext, HDC memoryContext, RECT clientRect) {
        auto bufferBitmap = CreateCompatibleBitmap(deviceContext, GetRectWidth(clientRect), GetRectHeigth(clientRect));
        auto oldBitmap = SelectObject(memoryContext, bufferBitmap);
        ClearBackgroud(memoryContext, clientRect);
        return oldBitmap;
    }

    static void ClearBackgroud(HDC deviceContext, RECT clientRect) {
        auto backgroudBrush = GetSysColorBrush(COLOR_WINDOW); // CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(deviceContext, &clientRect, backgroudBrush);
        // DeleteObject(backgroudBrush);
    }

    static void CopyContextBits(HDC destination, HDC source, RECT clientRect) {
        BitBlt(
                destination,
                clientRect.left, clientRect.top,
                GetRectWidth(clientRect), GetRectHeigth(clientRect),
                source,
                0, 0,
                SRCCOPY);
    }

    static void ReleaseOldObj(HDC hdc, HGDIOBJ obj) {
        auto buf_obj = SelectObject(hdc, obj);
        DeleteObject(buf_obj);
    }

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (!need_to_upd)
            return 0;
        auto rect = GetClientRect(hwnd);

        PAINTSTRUCT ps;
        auto hdc = BeginPaint(hwnd, &ps);
        auto cur_hdc = CreateCompatibleDC(ps.hdc);
        auto old_bitmap = SelectBufferBitmap(ps.hdc, cur_hdc, rect);

        task_handlers[cur_task](hwnd, cur_hdc);

        CopyContextBits(ps.hdc, cur_hdc, rect);
        ReleaseOldObj(cur_hdc, old_bitmap);
        ReleaseDC(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    static LRESULT on_command(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        KillTimer(hwnd, consts::id_timer);
        return 0;
    }

    static LRESULT on_char(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (cur_task != 2)
            return 1;
        if (iswcntrl((wchar_t) wparam))
            return 1;
        input_str.push_back((wchar_t) wparam);
        InvalidateRect(hwnd, nullptr, 0);
        return 0;
    }

    static LRESULT on_keydown(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (cur_task != 2)
            return 1;
        if (wparam == VK_BACK) {
            if (input_str.empty())
                return 0;
            input_str.pop_back();
            InvalidateRect(hwnd, nullptr, 0);
        }
        return 0;
    }


    static RECT rect_to_screen(HWND hwnd, RECT rect);

    static LRESULT on_mouse_press(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (cur_task != 3)
            return 1;
        button_pressed = true;
        auto r = GetClientRect(hwnd);
        auto screen_rect = rect_to_screen(hwnd, r);
        ClipCursor(&screen_rect);
        x0 = cur_x = LOWORD(lparam);
        y0 = cur_y = HIWORD(lparam);
        InvalidateRect(hwnd, nullptr, 0);
        return 0;
    }

    static LRESULT on_mouse_stop_press(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (cur_task != 3)
            return 1;
        button_pressed = false;
        ClipCursor(nullptr);
        x0 = cur_x;
        y0 = cur_y;
        cur_x = LOWORD(lparam);
        cur_y = HIWORD(lparam);
        InvalidateRect(hwnd, nullptr, 0);
        return 0;
    }

    static LRESULT on_mouse_move(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (cur_task != 3 || !button_pressed)
            return 1;
        x0 = cur_x;
        y0 = cur_y;
        cur_x = LOWORD(lparam);
        cur_y = HIWORD(lparam);
        InvalidateRect(hwnd, nullptr, 0);
        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        KillTimer(hwnd, consts::id_timer);
        DestroyMenu(h_main_menu);
        PostQuitMessage(0);
    }


    static void ch_calc_sum(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        std::cin >> a >> b;
        need_to_upd = true;
        cur_task = 1;
        InvalidateRect(hwnd, nullptr, true);
    }

    static void ch_task_2(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        need_to_upd = true;
        cur_task = 2;
        InvalidateRect(hwnd, nullptr, true);
    }

    static void ch_task_3(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        need_to_upd = true;
        cur_task = 3;
        InvalidateRect(hwnd, nullptr, true);
    }

    static void ch_task_4(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        std::cin >> n >> m;
        need_to_upd = true;
        cur_task = 4;
        InvalidateRect(hwnd, nullptr, true);
    }

    static void ch_task_5(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        need_to_upd = true;
        cur_task = 5;
        SetTimer(hwnd, consts::id_timer, 1000, timer_1);
        InvalidateRect(hwnd, nullptr, true);
    }

    static void CALLBACK timer_1(HWND hwnd, UINT message, UINT_PTR id, DWORD time) {
        InvalidateRect(hwnd, nullptr, 0);
    }

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
int Program::a, Program::b;
bool Program::need_to_upd = false, Program::button_pressed = false;
int Program::cur_task = 0;
const std::vector<void (*)(HWND, HDC)> Program::task_handlers = {Program::def_task, Program::task1, Program::task2,
                                                                 Program::task3, Program::task4, Program::task5};
int Program::x0, Program::y0, Program::cur_x, Program::cur_y, Program::n, Program::m;


RECT Program::rect_to_screen(HWND hwnd, RECT rect) {
    POINT positionLeftTop = {rect.left, rect.top};
    ClientToScreen(hwnd, &positionLeftTop);
    POINT positionRightBottom = {rect.right, rect.bottom};
    ClientToScreen(hwnd, &positionRightBottom);
    SetRect(&rect, positionLeftTop.x, positionLeftTop.y, positionRightBottom.x, positionRightBottom.y);
    return rect;
}


HMENU Program::h_main_menu;
std::wstring Program::input_str;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}