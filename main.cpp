#define USE_MATH_DEFINES

#include "Template.h"
#include <iostream>
#include <cmath>
#include <windowsx.h>

#define get_host_by_addr(x) gethostbyaddr((char*)&x,sizeof(x),AF_INET)
struct MyData {
    DLGTEMPLATE mHeader;

#pragma pack(2)
    WORD mNoMenu; // 0x0000 -- no menu
    WORD mStdClass; // 0x0000 -- standard dialog class
    wchar_t mTitle[5]; // title: "Test"

#pragma pack(4)
    DLGITEMTEMPLATE mItem;

#pragma pack(2)
    WORD mFfff; // 0xFFFF -- next is standard class ID
    WORD mListBoxClassId; // 0x0083 -- class ID for listbox
    wchar_t mText[5]; // text (not used for listbox)
    WORD mNoData; // 0

#pragma pack(4)

};

namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    const MyData md = {{WS_CAPTION | WS_VISIBLE | DS_CENTER, 0, 1, 10, 10, 100, 100}, 0, 0, L"Test",
                       {WS_CHILD | WS_BORDER, 0, 1, 1, 50, 50, 1234}, 0xFFFF, 0x0083, L"Test", 0
    };
    const short port = 1010;
    const unsigned int UM_SOCKET_NOTIFICATION = WM_USER + 101;
}


class Program {
    static WinApi::MessageMap message_map;
    static HINSTANCE hinst;
    static WinApi::MenuClass menu_class;
    static HWND _hdialog, msg_list, edit, send_button;
    static SOCKET g_ServerSocket;
    static hostent *serv;

    enum id {
        msg_list_id = 102,
        edit_id,
        send_button_id
    };

    static void init_message_map() {
        message_map
                .AddHandler(WM_INITDIALOG, on_create)
                .AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddCommandHandler(id::send_button_id, ch_send_press)
                .AddHandler(consts::UM_SOCKET_NOTIFICATION, HandleSocketEvent);
    }

    static void DeleteGdiObjects() {
    }

    static int main_loop() {
        MSG message;
        BOOL result;
        while ((result = GetMessage(&message, nullptr, 0, 0))) {
            if (!IsDialogMessageW(_hdialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }

        DeleteGdiObjects();

        return (int) message.wParam;
    }

public:
    static int main(HINSTANCE hInstance, int nCmdShow) {
        hinst = hInstance;
        init_message_map();

        auto Dlg = CreateDialogIndirectParam(hinst, &consts::md.mHeader, nullptr, wnd_proc, 0);
        ShowWindow(Dlg, nCmdShow);
        return main_loop();
    }

    static void init_dialog(HWND hwnd) {
        _hdialog = hwnd;
        SetWindowLong(hwnd, GWL_STYLE, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU);
        SetWindowPos(_hdialog, nullptr, 0, 0, 500, 500,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

        SetWindowText(_hdialog, consts::title);
    }

    static void AddMsg(const std::wstring &msg) {
        ListBox_AddString(msg_list, msg.c_str());
    }

    static void AddMsg(const std::string &msg) {
        ((int) (DWORD) SendMessageA((msg_list), LB_ADDSTRING, (LPARAM) 0, (LPARAM) (LPCTSTR) (msg.c_str())));
    }

    static void init_controls(HWND hwnd) {
        msg_list = CreateControl(WC_LISTBOX, 0, 10, 10, WinApi::GetRectWidth(WinApi::GetClientRect(hwnd)) - 20,
                                 WinApi::GetRectHeigth(WinApi::GetClientRect(hwnd)) - 100, id::msg_list_id);

        edit = CreateEditBox(10, WinApi::GetClientRect(hwnd).bottom - 80, id::edit_id);
        send_button = CreateControl(WC_BUTTON, WS_TILED | WS_BORDER, 120, WinApi::GetClientRect(hwnd).bottom - 80, 50,
                                    20, id::send_button_id, L"send");

    }

    static LRESULT on_create(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        init_dialog(hwnd);
        init_controls(hwnd);
        menu_class
                .CreateMenuClass(hwnd, message_map)
                .SetMenu();

        WSADATA wsadata;
        int retcode = WSAStartup(MAKEWORD(2, 2), &wsadata);

        if (retcode != 0) {
            return false;
        }
        AddMsg(L"Winsock initialized.");
        AddMsg(L"please send server name or ip");


        return true;
    }


    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {

        auto hdc = WinApi::DoubleBuffering(hwnd);
        auto rect = WinApi::GetClientRect(hwnd);
        WinApi::ClearBackgroud(hdc, rect);

        return 0;
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        WSACleanup();
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

    static void ch_send_press(HWND hwnd, WPARAM wparam, LPARAM lParam) {
        if (!serv) {
            char buf[100];
            GetWindowTextA(edit, buf, 100);
            std::string addr_str(buf);
            auto i_addr = inet_addr(addr_str.c_str());
            serv = (i_addr == INADDR_NONE ? gethostbyname(addr_str.c_str()) : get_host_by_addr(i_addr));
            if (!serv)
                AddMsg(L"try again");
            else
                AddMsg(L"OK");
            g_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            sockaddr_in sa;
            memset(&sa, 0, sizeof(sa));
            sa.sin_family = AF_INET;
            sa.sin_port = consts::port;
            sa.sin_addr.s_addr = *(u_long *) (serv->h_addr);
            connect(g_ServerSocket, (sockaddr *) &sa, sizeof(sa));
            WSAAsyncSelect(g_ServerSocket, hwnd, consts::UM_SOCKET_NOTIFICATION,
                           FD_READ);
            listen(g_ServerSocket, 5);
        } else {
            char buf[100];
            GetWindowTextA(edit, buf, 100);
            std::string msg(buf);
            send(g_ServerSocket, msg.c_str(), msg.length(), 0);
        }

    }

    static LRESULT HandleSocketEvent(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        auto s = (SOCKET) wparam;
        auto event = WSAGETSELECTEVENT(lparam);
        if (event == FD_READ) {
            char buffer[1024];
            memset(buffer, 0, 1024);

            unsigned long cmd = 0;

            int bytesread = recv(s, buffer, 1024, 0);
            AddMsg(std::string(buffer));
        }
    }

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

    static INT_PTR CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }

};

WinApi::MessageMap Program::message_map;
HINSTANCE Program::hinst;
WinApi::MenuClass Program::menu_class;
HWND Program::_hdialog, Program::msg_list, Program::edit, Program::send_button;
SOCKET Program::g_ServerSocket = 0;
hostent *Program::serv = nullptr;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}