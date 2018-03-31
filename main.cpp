#define UNICODE

#include <windows.h>
#include <iostream>
#include <ctime>
#include <sstream>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    const char flags[] = {2, 3, 4, 1};//2 and 4 for 2 state
    const time_t time_between_signals = 5000;
}

namespace Timers {
    unsigned int timer1 = 12345;
    unsigned int timer2 = 12346;
}

class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static char flag;
    static clock_t start_time, cur_time;

    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_TIMER, on_timer);
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

    static void print_traficlight(HDC hdc, RECT rect) {
        int h = GetRectHeigth(rect), w = GetRectWidth(rect);
        int r = std::min((h - 30) / 6, (w - 20) / 2);
        if (r <= 0)
            return;

        auto brush = CreateSolidBrush(flag == 1 ? RGB(255, 0, 0) : RGB(100, 50, 50));
        auto old_brush = SelectObject(hdc, brush);
        Ellipse(hdc, (w / 2 - r), (h / 2 - 3 * r) / 2, (w / 2 + r), (h / 2 + r) / 2);

        brush = CreateSolidBrush(flag & 1 ? RGB(100, 100, 50) : RGB(255, 255, 0));
        ReleaseOldObj(hdc, brush);
        Ellipse(hdc, (w / 2 - r), (h / 2 - r), (w / 2 + r), (h / 2 + r));
        brush = CreateSolidBrush(flag != 3 ? RGB(50, 100, 50) : RGB(0, 255, 0));
        ReleaseOldObj(hdc, brush);
        Ellipse(hdc, (w / 2 - r), (3 * h / 2 - r) / 2, (w / 2 + r), (3 * h / 2 + 3 * r) / 2);
        ReleaseOldObj(hdc, old_brush);

        clock_t seconds_remain = consts::time_between_signals / 1000 - (cur_time - start_time) / CLOCKS_PER_SEC;
        if (seconds_remain <= 0)
            seconds_remain = 1;
        std::wstringstream ss;
        std::wstring str;
        ss << seconds_remain;
        ss >> str;
        auto font = CreateFont(170, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
        auto old_font = SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, str.c_str(), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        SetBkMode(hdc, OPAQUE);
        ReleaseOldObj(hdc, old_font);
    }

public:


    static int main(HINSTANCE hInstance, int nCmdShow) {
        hinst = hInstance;
        init_message_map();

        WinApi::WindowClass(hinst, consts::main_class_name, wnd_proc).Register();
        WinApi::Window().create_and_show(hinst, consts::main_class_name, consts::title, nCmdShow);
        return main_loop();
    }

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PAINTSTRUCT ps;
        auto rect = GetClientRect(hwnd);
        BeginPaint(hwnd, &ps);

        auto cur_hdc = CreateCompatibleDC(ps.hdc);
        auto old_bitmap = SelectBufferBitmap(ps.hdc, cur_hdc, rect);

        print_traficlight(cur_hdc, rect);

        CopyContextBits(ps.hdc, cur_hdc, rect);
        ReleaseOldObj(cur_hdc, old_bitmap);

        EndPaint(hwnd, &ps);
        return 0;
    }


    static void on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        SetTimer(hwnd, Timers::timer1, consts::time_between_signals, nullptr);
        SetTimer(hwnd, Timers::timer2, 100, TimerProc);
        start_time = clock();
    }

    static void CALLBACK TimerProc(HWND hWindow, UINT message, UINT_PTR idEvent, DWORD time) {
        if (idEvent != Timers::timer2) return;

        cur_time = clock();
        auto rect = GetClientRect(hWindow);
        InvalidateRect(hWindow, &rect, 0);
    }

    static void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (LOWORD(wparam) == Timers::timer1) {
            flag = consts::flags[flag - 1];
            auto rect = GetClientRect(hwnd);
            start_time = cur_time;
            InvalidateRect(hwnd, &rect, 0);
        }
    }


    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        KillTimer(hwnd, Timers::timer1);
        KillTimer(hwnd, Timers::timer2);

        PostQuitMessage(0);
    }

/*
    static void on_(HWND hwnd, WPARAM wparam, LPARAM lparam) {

    }
*/
    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
char Program::flag = 1;
clock_t Program::start_time, Program::cur_time;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}