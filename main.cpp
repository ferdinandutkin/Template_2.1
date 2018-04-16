#define UNICODE

#include <windows.h>
#include <cmath>
#include <commctrl.h>
#include <sstream>
#include <iostream>
#include <dshow.h>
#include <vector>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"
#include "DlgStruct.h"

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    UINT delta_t = 20;
    const int R = 10;
    const int beg_speed = 3;
    const MyData md = {{WS_CAPTION | WS_VISIBLE | DS_CENTER, 0, 1, 10, 10, 100, 100}, 0, 0, L"Test",
                       {WS_CHILD | WS_BORDER, 0, 1, 1, 50, 50, 1234}, 0xFFFF, 0x0083, L"Test", 0
    };
}
COLORREF dColors[16] = {0};
enum IDS {
    id_timer = 1000,
    id_settings_menu,
    id_start_menu,
    id_stop_menu,
    id_about_menu,
    id_exit_menu,
    id_height,
    id_chooce_color_button,
    id_speed,
    id_accel,
    id_ok,
    id_cancel
};

class Program {
    static WinApi::MessageMap message_map;
    static WinApi::DialogMessageMap dlg_message_map;
    static HINSTANCE hinst;
    static bool need_to_move;
    static double accel;
    static COLORREF color;
    static double speed;
    static HMENU menu, object_menu;
    static HWND _hDialog;
    static int R;
    static COLORREF buf_color;
    static std::vector<COLORREF> custom_colors;

    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CREATE, on_create)
                .AddHandler(WM_RBUTTONDOWN, on_right_button_down)
                .AddHandler(WM_RBUTTONUP, on_right_button_up)
                .AddHandler(WM_TIMER, on_timer)
                .AddHandler(WM_ERASEBKGND, on_erase_backgrownd)
                .AddCommandHandler(id_start_menu, start_menu_handler)
                .AddCommandHandler(id_stop_menu, stop_menu_handler)
                .AddCommandHandler(id_settings_menu, settings_menu_handler)
                .AddCommandHandler(id_about_menu, about_menu_handler)
                .AddCommandHandler(id_exit_menu, exit_menu_handler);
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
    static void create_and_set_menu(HWND hwnd) {
        menu = CreateMenu();

        object_menu = CreatePopupMenu();
        AppendMenu(object_menu, MF_STRING, id_settings_menu, L"&Настройка");
        AppendMenu(object_menu, MF_STRING | MF_DISABLED, id_start_menu, L"&Старт");
        AppendMenu(object_menu, MF_STRING, id_stop_menu, L"&Стоп");

        AppendMenu(menu, MF_POPUP | MF_STRING, (UINT_PTR) object_menu, L"&Объект");

        AppendMenu(menu, MF_STRING, id_about_menu, L"&Справка");

        AppendMenu(menu, MF_STRING, id_exit_menu, L"&Выход");


        SetMenu(hwnd, menu);
    }

    static void on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        SetTimer(hwnd, id_timer, consts::delta_t, nullptr);

        create_and_set_menu(hwnd);
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

        static int direction = 1;
        const static int rotate_speed = 3;
        static int prev_x = static_cast<int>(consts::R - speed), prev_angle = 0;
        HBRUSH brush = CreateSolidBrush(color);
        RECT united_rect, client_rect = GetClientRect(hwnd);
        int cur_x = static_cast<int>(prev_x + speed), cur_y = f(cur_x) + client_rect.bottom / 2;
        auto ellipse_rect = SetRect(cur_x - R, cur_y - R, cur_x + R, cur_y + R);
        UnionRect(&united_rect, &ellipse_rect, &client_rect);
        if (!EqualRect(&client_rect, &united_rect)) {
            direction *= -1;
//            speed=consts::beg_speed;
            speed *= direction;
//            DeleteObject(brush);

            color = RGB(rand() % 256, rand() % 256, rand() % 256);

//            brush = CreateSolidBrush(color);
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

        DeleteObject(SelectObject(hdc, old_brush));

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


    static void start_menu_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        EnableMenuItem(object_menu, id_start_menu, MF_DISABLED);
        EnableMenuItem(object_menu, id_stop_menu, MF_ENABLED);
        need_to_move = true;
    }

    static void stop_menu_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        EnableMenuItem(object_menu, id_start_menu, MF_ENABLED);
        EnableMenuItem(object_menu, id_stop_menu, MF_DISABLED);
        need_to_move = false;
    }

    static void settings_menu_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        need_to_move = false;
        init_dlg_message_map();
        _hDialog = CreateDialogIndirect(hinst, &consts::md.mHeader, hwnd, settings_dialog_proc);
        MSG message;
        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_hDialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        need_to_move = true;
    }

    static void about_menu_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        need_to_move = false;

        CreateDialogIndirect(hinst, &consts::md.mHeader, hwnd, about_dialog_proc);
        MSG message;

        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_hDialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        need_to_move = true;
    }

    static void exit_menu_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void init_dlg_message_map() {
        dlg_message_map.AddHandler(WM_INITDIALOG, on_init_dialog)
                .AddHandler(WM_DESTROY, on_destroy_dlg)
                .AddHandler(WM_CLOSE, on_close)
                .AddHandler(WM_DISPLAYCHANGE, on_update_dialog)
                .AddCommandHandler(id_ok, ok_button_handler)
                .AddCommandHandler(id_cancel, cancel_button_handler)
                .AddCommandHandler(id_chooce_color_button, choose_color_button_handler);
    }


    static LRESULT on_init_dialog(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _hDialog = hwnd;
        init_dialog(hwnd);
        init_controls(hwnd);
        return true;
    }

    static void init_dialog(HWND hwnd, LPCTSTR name = consts::title) {
        SetWindowLong(hwnd, GWL_STYLE, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU);
        SetWindowPos(hwnd, nullptr, 0, 0, 500, 250, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowText(hwnd, name);
    }

    static void on_update_dialog(HWND hwnd, WPARAM wParam, LPARAM lParam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void init_controls(HWND hwnd) {
        CreateControl(WC_STATIC, SS_CENTER, 5, 5, 100, 20, 0, L"Высота:");
        std::wstring str;
        std::wstringstream ss;
        ss << R * 2;
        ss >> str;
        CreateEditBox(120, 5, id_height, 100, 20, ES_NUMBER, str.c_str());

        CreateControl(WC_STATIC, SS_CENTER, 5, 35, 100, 20, 0, L"Скорость:");
        str.clear();
        std::wstringstream ss1;
        ss1 << speed;
        ss1 >> str;
        CreateEditBox(120, 35, id_speed, 100, 20, 0, str.c_str());

        CreateControl(WC_STATIC, SS_CENTER, 5, 65, 100, 20, 0, L"Ускорение:");
        str.clear();
        std::wstringstream ss2;
        ss2 << accel;
        ss2 >> str;
        CreateEditBox(120, 65, id_accel, 100, 20, 0, str.c_str());

        CreateControl(WC_STATIC, SS_CENTER, 5, 95, 100, 20, 0, L"Цвет:");
        CreateControl(WC_BUTTON, BS_PUSHBUTTON | BS_CENTER, 120, 95, 70, 20, id_chooce_color_button, L"Выбрать");

        CreateControl(WC_BUTTON, BS_PUSHBUTTON | BS_CENTER | WS_BORDER, 300, 200, 50, 20, id_ok, L"Ok");
        CreateControl(WC_BUTTON, BS_PUSHBUTTON | BS_CENTER | WS_BORDER, 400, 200, 50, 20, id_cancel, L"Cancel");

        buf_color = color;
    }

    static void on_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void on_destroy_dlg(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }

    static HWND CreateControl(const wchar_t *className, long style, int x, int y, int width, int height, int id,
                              LPCWSTR name = L"") {
        return CreateWindow(
                className, name,
                WS_CHILD | WS_VISIBLE | style,
                x, y,
                width, height,
                _hDialog,
                (HMENU) id,
                hinst,
                nullptr);
    }    //!style|WS_VISIBLE|WS_CHILD

    static HWND
    CreateEditBox(int x, int y, int id, int width = 100, int height = 20, long style = 0, LPCWSTR str = L"") {
        auto new_edit_box = CreateControl(WC_EDIT, WS_TABSTOP | WS_BORDER | ES_CENTER | style, x, y, width, height,
                                          id, str);
        Edit_SetCueBannerText(new_edit_box, L"введите число");
        return new_edit_box;
    }

    static void ok_button_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        wchar_t str[100];
        Edit_GetText(GetDlgItem(hwnd, id_height), str, 100);
        std::wstringstream ss;
        ss << str;
        ss >> R;
        R /= 2;

        Edit_GetText(GetDlgItem(hwnd, id_speed), str, 100);
        ss.clear();
        ss << str;
        ss >> speed;


        Edit_GetText(GetDlgItem(hwnd, id_accel), str, 100);
        ss.clear();
        ss << str;
        ss >> accel;


        color = buf_color;

        DestroyWindow(hwnd);
    }

    static void cancel_button_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void choose_color_button_handler(HWND hwnd, WORD loword, WORD hiword, LPARAM lparam) {
        CHOOSECOLOR cc;
        cc.Flags = CC_RGBINIT | CC_FULLOPEN;
        cc.hInstance = nullptr;
        cc.hwndOwner = hwnd;
        cc.lCustData = 0L;
        cc.lpCustColors = dColors;
        cc.lpfnHook = nullptr;
        cc.lpTemplateName = (LPTSTR) nullptr;
        cc.lStructSize = sizeof(cc);
        cc.rgbResult = buf_color;

        if (ChooseColor(&cc)) {
            buf_color = (COLORREF) cc.rgbResult;

        }
    }

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

    static INT_PTR CALLBACK settings_dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return dlg_message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

    static INT_PTR CALLBACK about_dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_INITDIALOG: {
                init_dialog(hwnd, L"Справка");
                break;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                auto hdc = BeginPaint(hwnd, &ps);
                auto rect = GetClientRect(hwnd);
                SetBkMode(hdc, TRANSPARENT);
                DrawText(hdc,
                         L"Шарик летает по синусоиде, в меню \"&Настройки\" можно задать скорость, ускорение и выбрать"
                         " цвет;\n Чтобы остановить, нажмите правую кнопку мыши, или выберите пункт меню \"&Стоп\";\n"
                         " Чтобы продолжить - отпустите кнопку мыши, или выберите \"&Старт\".", -1, &rect,
                         DT_CENTER | DT_WORDBREAK);

                ReleaseDC(hwnd, hdc);
                EndPaint(hwnd, &ps);
                break;
            }
            case WM_CLOSE:
                DestroyWindow(hwnd);
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                return 0;
        }
        return 1;
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
COLORREF Program::color = RGB(rand() % 256, rand() % 256, rand() % 256);
double Program::speed = consts::beg_speed;
HMENU Program::menu, Program::object_menu;
WinApi::DialogMessageMap Program::dlg_message_map;
HWND Program::_hDialog;
int Program::R = consts::R;
COLORREF Program::buf_color;
std::vector<COLORREF> Program::custom_colors(10);


POINT Program::MakePoint(double x, double y) {
    POINT pt = {static_cast<LONG>(x), static_cast<LONG>(y)};
    return pt;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}