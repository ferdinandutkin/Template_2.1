#define UNICODE

#include <windows.h>
#include <cmath>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    UINT delta_t = 20;
    const int R = 10;
    const int beg_speed = 3;
}

enum IDS {
    id_timer = 1000
};

class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static bool need_to_move;
    static double accel;

    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_RBUTTONDOWN, on_right_button_down)
                .AddHandler(WM_RBUTTONUP, on_right_button_up)
                .AddHandler(WM_TIMER, on_timer)
                .AddHandler(WM_ERASEBKGND, on_erase_backgrownd);
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

private:
    static void on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        SetTimer(hwnd, id_timer, consts::delta_t, nullptr);
    }

    static void on_right_button_down(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto rect = GetClientRect(hwnd);
        auto rect_in_screen_metrics = rect_to_screen(hwnd, rect);
        ClipCursor(&rect_in_screen_metrics);
        need_to_move = false;
    }

    static void on_right_button_up(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        ClipCursor(nullptr);
        need_to_move = true;
    }

    static POINT MakePoint(double x, double y);

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (!need_to_move)
            return 0;
        static double speed = consts::beg_speed;
        static int direction = 1;
        const static int rotate_speed = 3;
        static int prev_x = static_cast<int>(consts::R - speed), prev_angle = 0;
        static HBRUSH brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        const int R = consts::R;
        RECT united_rect, client_rect = GetClientRect(hwnd);
        int cur_x = static_cast<int>(prev_x + speed), cur_y = f(cur_x) + client_rect.bottom / 2;
        auto ellipse_rect = SetRect(cur_x - R, cur_y - R, cur_x + R, cur_y + R);
        UnionRect(&united_rect, &ellipse_rect, &client_rect);
        if (!EqualRect(&client_rect, &united_rect)) {
            direction *= -1;
//            speed=consts::beg_speed;
            speed *= direction;
            DeleteObject(brush);
            brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
//            prev_x = cur_x;
            return 0;
        }
        PAINTSTRUCT ps;
        auto old_hdc = BeginPaint(hwnd, &ps);
        auto hdc = CreateCompatibleDC(ps.hdc);
        auto old_bitmap = SelectBufferBitmap(ps.hdc, hdc, client_rect);

        FillRect(hdc, &client_rect, WHITE_BRUSH);
        auto old_brush = (HBRUSH) SelectObject(hdc, brush);

        Ellipse(hdc, ellipse_rect.left, ellipse_rect.top, ellipse_rect.right, ellipse_rect.bottom);
        MoveToEx(hdc, cur_x, cur_y, nullptr);
        int cur_angle = (prev_angle + rotate_speed) % 360;
        POINT R_pick_pos_after_rotating = MakePoint(cur_x + R * cos(cur_angle * M_PI / 180),
                                                    cur_y + R * sin(cur_angle * M_PI / 180));
        LineTo(hdc, R_pick_pos_after_rotating.x, R_pick_pos_after_rotating.y);

        SelectObject(hdc, old_brush);

        CopyContextBits(ps.hdc, hdc, client_rect);
        ReleaseOldObj(hdc, old_bitmap);
        ReleaseDC(hwnd, old_hdc);
        EndPaint(hwnd, &ps);

        prev_angle = cur_angle;
        prev_x = cur_x;
        speed += (speed ? speed / abs(speed) : 1) * accel;
        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }

    static void on_timer(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_erase_backgrownd(HWND hwnd, WPARAM wparam, LPARAM lparam) {

    }


    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

    static RECT GetClientRect(HWND hwnd) {
        RECT r;
        ::GetClientRect(hwnd, &r);
        return r;
    }

    static RECT rect_to_screen(HWND hwnd, RECT rect) {
        POINT positionLeftTop = {rect.left, rect.top};
        ClientToScreen(hwnd, &positionLeftTop);
        POINT positionRightBottom = {rect.right, rect.bottom};
        ClientToScreen(hwnd, &positionRightBottom);
        rect = SetRect(positionLeftTop.x, positionLeftTop.y, positionRightBottom.x, positionRightBottom.y);
        return rect;
    }

    static RECT SetRect(int left, int top, int right, int bottom) {
        RECT r;
        ::SetRect(&r, left, top, right, bottom);
        return r;
    }

    ////////////////////////////////// for 2 buf ///////////////////////////////////////////////////////////////////////
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
    ////////////////////////////////// for 2 buf end ///////////////////////////////////////////////////////////////////

    static int f(int x) {
        return static_cast<int>(100 * sin(1.0 * x / 100));
    }
};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
bool Program::need_to_move = true;
double Program::accel = 0.01;


POINT Program::MakePoint(double x, double y) {
    POINT pt = {static_cast<LONG>(x), static_cast<LONG>(y)};
    return pt;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}