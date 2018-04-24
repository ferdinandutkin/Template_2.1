#define UNICODE

#include <windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <ntdef.h>
#include <cmath>
#include <dshow.h>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"
#include "Template.h"

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    const double epselon = 1e-5;
}

enum IDS {
    id_menu_open = 101, id_menu_settings, id_menu_about, id_menu_exit, id_dialog_list, id_dialog_ok, id_dialog_select
};

using namespace WinApi;

class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static char file[500];
    struct Participant {
        std::wstring surname;
        double score_in_percent;
        COLORREF color;
    };
    static std::vector<Participant> participants;
    static HWND _hwnd;


    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CREATE, on_create)
                .AddCommandHandler(id_menu_open, ch_open_menu)
                .AddCommandHandler(id_menu_settings, ch_settings_menu)
                .AddCommandHandler(id_menu_about, ch_about_menu)
                .AddCommandHandler(id_menu_exit, ch_exit_menu);
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
    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto[hdc, ps]=BeginPaint(hwnd);

        auto rect = GetClientRect(hwnd);
        FillRect(hdc, &rect, WHITE_BRUSH);
        auto winner = *std::max_element(participants.begin(), participants.end(),
                                        [](const Participant &p1, const Participant &p2) {
                                            return p1.score_in_percent < p2.score_in_percent;
                                        });

        double mashtab = (rect.bottom - 20) / winner.score_in_percent;
        int x_size = std::min(static_cast<int>((rect.right / 2 - 40) / participants.size() * 3 / 4), 30);
        int cur_left_pos = 20;
        int otstup = std::min(static_cast<int>((rect.right / 2 - 40) / participants.size() / 4), 10);

        for (const auto &i:participants) {

            auto brush = CreateSolidBrush(i.color);
            brush = (HBRUSH) SelectObject(hdc, brush);
            Rectangle(hdc, cur_left_pos, static_cast<int>(rect.bottom - i.score_in_percent * mashtab),
                      (cur_left_pos + x_size), rect.bottom - 20);
            DeleteObject(SelectObject(hdc, brush));
            cur_left_pos += x_size + otstup;

            std::wstring str;
            std::wstringstream ss;
            ss << i.surname << ' ' << i.score_in_percent << '%';
            getline(ss, str);

            auto font = CreateFont(0, 0, 2700, 2700, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe Print");
            font = (HFONT) SelectObject(hdc, font);
            RECT r;
            SetRect(&r, cur_left_pos - x_size, 20, cur_left_pos, r.bottom - 20);
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, cur_left_pos - otstup, static_cast<int>(rect.bottom - 20 - std::max(
                    static_cast<int>(i.score_in_percent * mashtab) - 20, 120)), str.c_str(), str.size());
            DeleteObject(SelectObject(hdc, font));
        }

        int R = std::min((rect.right / 2 - 40) / 2, (rect.bottom - 40) / 2);
        POINT center = {rect.right * 3 / 4, rect.bottom / 2};
        float cur_angle = 0;
        for (const auto &i:participants) {

            BeginPath(hdc);
            MoveToEx(hdc, center.x, center.y, nullptr);
            AngleArc(hdc, center.x, center.y, static_cast<DWORD>(R), cur_angle,
                     static_cast<FLOAT>(i.score_in_percent * 3.6));
            LineTo(hdc, center.x, center.y);
            EndPath(hdc);

            auto brush = CreateSolidBrush(i.color);
            brush = (HBRUSH) SelectObject(hdc, brush);

            StrokeAndFillPath(hdc);

            DeleteObject(SelectObject(hdc, brush));

            std::wstring str;
            std::wstringstream ss;
            ss << i.surname << ' ' << i.score_in_percent << '%';
            getline(ss, str);

            auto font = CreateFont(0, 0, static_cast<int>((cur_angle + i.score_in_percent * 1.8) * 10),
                                   static_cast<int>((cur_angle + i.score_in_percent * 1.8) * 10), 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, L"Segoe Print");
            font = (HFONT) SelectObject(hdc, font);

            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, static_cast<int>(center.x - 18 * sin((cur_angle + i.score_in_percent * 1.8) * M_PI / 180) +
                                          30 * cos((cur_angle + i.score_in_percent * 1.8) * M_PI / 180)),
                    static_cast<int>(center.y - 18 * cos((cur_angle + i.score_in_percent * 1.8) * M_PI / 180) -
                                     30 * sin((cur_angle + i.score_in_percent * 1.8) * M_PI / 180)), str.c_str(),
                    str.size());
            DeleteObject(SelectObject(hdc, font));


            cur_angle += (i.score_in_percent * 3.6);
        }
        EndPaint(hwnd, hdc, ps);
        return 0;
    }

    static RECT GetClientRect(HWND hwnd) {
        RECT r;
        ::GetClientRect(hwnd, &r);
        return r;
    }

    static void EndPaint(HWND hwnd, HDC hdc, PAINTSTRUCT ps) {
        ReleaseDC(hwnd, hdc);
        ::EndPaint(hwnd, &ps);
    }

    static std::pair<HDC, PAINTSTRUCT> BeginPaint(HWND hwnd) {
        PAINTSTRUCT ps;
        auto hdc = ::BeginPaint(hwnd, &ps);
        return std::make_pair(hdc, ps);
    }

    static LRESULT on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _hwnd = hwnd;
        auto main_menu = CreateMenu();
        auto file_menu = CreatePopupMenu();
        AppendMenu(file_menu, MF_STRING, id_menu_open, L"&Открыть");

        AppendMenu(main_menu, MF_POPUP | MF_STRING, (UINT_PTR) file_menu, L"&Файл");
        AppendMenu(main_menu, MF_STRING, id_menu_settings, L"&Настройки");
        AppendMenu(main_menu, MF_STRING, id_menu_about, L"&Справка");
        AppendMenu(main_menu, MF_STRING, id_menu_exit, L"&Выход");
        SetMenu(hwnd, main_menu);
        refresh_data(hwnd);
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }


    static void ch_open_menu(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        OPENFILENAMEA openfilename_struct;
        ZeroMemory(&openfilename_struct, sizeof(openfilename_struct));
        openfilename_struct.lStructSize = sizeof(openfilename_struct);
        openfilename_struct.lStructSize = sizeof(OPENFILENAME);
        openfilename_struct.hwndOwner = hwnd;
        openfilename_struct.lpstrFile = file;
        openfilename_struct.nMaxFile = sizeof(file);
        openfilename_struct.lpstrFilter = "txt\0*.txt\0";
        openfilename_struct.nFilterIndex = 3;
        openfilename_struct.lpstrInitialDir = nullptr;
        openfilename_struct.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        if (GetOpenFileNameA(&openfilename_struct))
            refresh_data(hwnd);
    }

    static void ch_settings_menu(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto _hDialog = HCreateDialog(hinst, hwnd, settings_dialog_proc);

        MSG message;

        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_hDialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        InvalidateRect(hwnd, nullptr, false);
    }

    static void ch_about_menu(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto _hDialog = HCreateDialog(hinst, hwnd, about_dialog_proc);

        MSG message;

        while (GetMessage(&message, nullptr, 0, 0)) {
            if (!IsDialogMessageW(_hDialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    static void ch_exit_menu(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }


    static void refresh_data(HWND hwnd);


    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

    static void init_dialog(HWND hwnd, LPCTSTR name = consts::title) {
        SetWindowLong(hwnd, GWL_STYLE, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU);
        SetWindowPos(hwnd, nullptr, 0, 0, 500, 250, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        SetWindowText(hwnd, name);
    }

    static INT_PTR CALLBACK about_dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_INITDIALOG: {
                init_dialog(hwnd, L"Справка");
                break;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                auto hdc = DoubleBuffering(hwnd);
                auto rect = GetClientRect(hwnd);
                SetBkMode(hdc, TRANSPARENT);
                DrawText(hdc,
                         L"Диаграмы строятся на основе файла \"Data.txt\", или на основе выбранного файла в меню \"Открыть\", можно настроить цвет в меню \"Настройки\"\n\nАвтор: Ковалевский Сергей",
                         -1, &rect,
                         DT_CENTER | DT_WORDBREAK);

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

    static INT_PTR CALLBACK settings_dialog_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_INITDIALOG: {
                InitialiseDialogAndControlls(hinst, hwnd).init_dialog()
                        .CreateComboBox({150, 50}, id_dialog_list, 200, 200)
                        .CreatePushButton({400, 200}, id_dialog_ok, 80, 20, 0, L"Ok")
                        .CreatePushButton({200, 100}, id_dialog_select, 100, 20, 0, L"Выбрать");

                for (const auto &i:participants) {
                    SendMessage(GetDlgItem(hwnd, id_dialog_list), CB_ADDSTRING, 0, (LPARAM) i.surname.c_str());
                }

                ComboBox_SetCurSel(GetDlgItem(hwnd, id_dialog_list), 0);
                break;
            }
            case WM_COMMAND:
                static COLORREF dColors[16] = {0};
                static COLORREF buf_color = 0;
                if (LOWORD(wParam) == id_dialog_select) {
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
                        participants[ComboBox_GetCurSel(GetDlgItem(hwnd, id_dialog_list))].color = cc.rgbResult;
                        InvalidateRect(_hwnd, nullptr, false);
                    }

                } else if (LOWORD(wParam) == id_dialog_ok) {
                    DestroyWindow(hwnd);
                }
                break;

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

};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
char Program::file[500] = "Data.txt";
std::vector<Program::Participant> Program::participants;
HWND Program::_hwnd;

void Program::refresh_data(HWND hwnd) {
    setlocale(LC_ALL, "rus");
    participants.clear();
    std::wifstream fin(file);
    int sum = 0;
    while (true) {
        Participant participant;
        fin >> participant.surname >> participant.score_in_percent;
        if (!fin)
            break;
        sum += participant.score_in_percent;
        participant.color = GetRandColor();
        participants.push_back(participant);
    }
    if (participants.empty() || abs(sum - 100.0) > consts::epselon) {
        MessageBox(hwnd, L"Invalid file", L"Error", MB_OK | MB_ICONERROR);
        DestroyWindow(hwnd);
    }
    InvalidateRect(hwnd, nullptr, false);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}