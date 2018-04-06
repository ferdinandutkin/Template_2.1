#define UNICODE

#include <windows.h>
#include <algorithm>
#include <commctrl.h>
#include <dshow.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include "MessageMap.h"
#include "WindowClass.h"
#include "Window.h"
#include "DlgStruct.h"


namespace consts {
    LPCTSTR main_class_name = L"sdighewirusbgvbszdiv";
    LPCTSTR title = L"Main Window";
    const MyData md = {{WS_CAPTION | WS_VISIBLE | DS_CENTER, 0, 1, 10, 10, 100, 100}, 0, 0, L"Test",
                       {WS_CHILD | WS_BORDER, 0, 1, 1, 50, 50, 1234}, 0xFFFF, 0x0083, L"Test", 0
    };
    const std::vector<std::wstring> operations[] = {{L"+", L"-", L"*", L"/", L"%"},
                                                    {L"&", L"|", L"^",},//L"&", L"˅", L"⊕"
                                                    {L"∧", L"˅", L"⊕", L"⇒", L"⇔"}};
    const std::wstring operation_names[] = {L"Арифметические", L"Битовые", L"Логические"};
    const int number_of_operations = 3;
}

enum Control_ids {
    id_operand1 = 102,
    id_operand2,
    id_rezult,
    id_radio_box,
    id_check_box,
    id_combo_box,
    id_button,
    id_slider
};

class Program {
    static WinApi::DialogMessageMap message_map;

    static struct State {
        State() {
            calc_in_real_time = false;
            operation_set = 0;
            precision = 0;
        }

        bool calc_in_real_time;
        int operation_set;
        //! 0 for arithmetic
        //! 1 for logical
        int precision;
    } state;

    static HINSTANCE hinst;
    static HWND _hDialog;

    static HFONT _font;
    static HWND operand1, operand2, result, radio_box, check_box, combo_box, button, slider;
    static bool button_pressed;
    static std::vector<std::pair<int, bool> > operation_ids_set;


    static void init_operation_set() {
        for (int i = 200; i < 200 + consts::number_of_operations; i++)
            operation_ids_set.emplace_back(std::make_pair(i, true));
        operation_ids_set[1].second = false;
        operation_ids_set[2].second = false;
    }

    static void init_message_map() {
        message_map.AddHandler(WM_INITDIALOG, on_init_dialog)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CLOSE, on_close)
                .AddHandler(WM_DISPLAYCHANGE, on_update_dialog)
                .AddHandler(WM_CTLCOLOREDIT, on_check_and_count)
                .AddCommandHandler(id_combo_box, CBN_SELCHANGE, ch_combo_change)
                .AddCommandHandlersSet(operation_ids_set, BN_CLICKED, chs_change_operations)
                .AddCommandHandler(id_button, BN_CLICKED, ch_button_click)
                .AddCommandHandler(id_check_box, BN_CLICKED, ch_check_click)
                .AddHandler(WM_HSCROLL, on_slider_move);
    }

    static void DeleteGdiObjects() {
    }

    static int main_loop() {
        MSG message;
        BOOL result;
        while ((result = GetMessage(&message, nullptr, 0, 0))) {
            if (!IsDialogMessageW(_hDialog, &message)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }

        DeleteGdiObjects();

        return (int) message.wParam;
    }

    static void init_dialog(HWND hwnd) {
        SetWindowLong(hwnd, GWL_STYLE, DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU);
        SetWindowPos(_hDialog, nullptr, 0, 0, 500, 250 + 20 * (consts::number_of_operations - 3),
                     SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        _font = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
        SetWindowFont(_hDialog, _font, 0);
        SetWindowText(_hDialog, consts::title);
    }


    static void init_controls(HWND hwnd) {
        operand1 = CreateEditBox(30, 30, id_operand1);
        combo_box = CreateComboBox(140, 30, id_combo_box);
        operand2 = CreateEditBox(185, 30, id_operand2);
        button = CreateControl(WC_BUTTON, WS_TILED | WS_BORDER | WS_DISABLED, 290, 30, 30, 20, id_button, L"=");
        result = CreateEditBox(330, 30, id_rezult, 100, 20, ES_READONLY, L"Ошибка");
        radio_box = CreateRadioBoxForOperations(30, 110, id_radio_box);
        check_box = CreateControl(WC_BUTTON, WS_TABSTOP | BS_AUTOCHECKBOX, 240, 130, 150, 20, id_check_box,
                                  L"Считать сразу");
        slider = CreateSlider(0, 3, id_slider);
    }

public:
    static int main(HINSTANCE hInstance, int nCmdShow) {
        hinst = hInstance;
        init_operation_set();
        init_message_map();

        auto Dlg = CreateDialogIndirectParam(hinst, &consts::md.mHeader, nullptr, DialogProc, 0);

        ShowWindow(Dlg, nCmdShow);
        return main_loop();
    }

    //Different on functions
    static LRESULT on_init_dialog(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        _hDialog = hwnd;
        init_dialog(hwnd);
        init_controls(hwnd);
        return true;
    }

    static void on_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DeleteObject(_font);
        PostQuitMessage(0);
    }

    static void on_update_dialog(HWND hwnd, WPARAM wParam, LPARAM lParam) {
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_check_and_count(HWND hwnd, WPARAM wParam, LPARAM lParam) {
        auto h_control = (HWND) lParam;
        static double oper1, oper2;
        static double state1 = 0, state2 = 0;

        int ctrl_id = GetDlgCtrlID(h_control);

        if (!(state1 = is_valid_data(operand1, id_operand1, oper1)) && ctrl_id == id_operand1) {
            SetBkColor((HDC) wParam, RGB(200, 0, 0));
        }
        if (!(state2 = is_valid_data(operand2, id_operand2, oper2)) && ctrl_id == id_operand2) {
            SetBkColor((HDC) wParam, RGB(200, 0, 0));
        }

        if (state1 && state2)
            if (state.calc_in_real_time || button_pressed) {
                std::wstring res = operation(oper1, oper2);

                SetWindowText(result, res.c_str());
                button_pressed = false;
            } else {
                Button_Enable(button, true);
            }
        else {
            Button_Enable(button, false);
            SetWindowText(result, L"Ошибка");
        }
    }

    static void on_slider_move(HWND hwnd, WPARAM wParam, LPARAM lParam) {
        auto id = GetDlgCtrlID((HWND) lParam);
        if (id == id_slider) {
            state.precision = SendMessage(slider, TBM_GETPOS, 0, 0);
            upd_edits();
        }
    }


    //Different ch_functions
    static void ch_combo_change(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        upd_edits();
        if (HiWord == CBN_SELCHANGE) {
            int ItemIndex = SendMessage((HWND) lParam, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

        }
    }

/*    static void ch_radio_change_to_logical(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        if (!state.operation_set) {
            SendMessage(slider, TBM_SETPOS, TRUE, 0);
            state.precision = 0;
            EnableWindow(slider, 0);

            state.operation_set = 1;
            upd_combo_box();
            upd_edits();
        }
    }

    static void ch_radio_change_to_arithmetic(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        if (state.operation_set) {
            EnableWindow(slider, 1);
            state.operation_set = 0;
            upd_combo_box();
            upd_edits();
        }
    }*/

    static void chs_change_operations(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        if (state.operation_set == LoWord - operation_ids_set.front().first) {
            return;
        }
        if (std::binary_search(operation_ids_set.begin(), operation_ids_set.end(), std::make_pair((int) LoWord, true)))
            EnableWindow(slider, 1);
        else {
            SendMessage(slider, TBM_SETPOS, TRUE, 0);
            state.precision = 0;
            EnableWindow(slider, 0);
        }

        state.operation_set = LoWord - operation_ids_set.front().first;

        upd_combo_box();
        upd_edits();
    }

    static void ch_button_click(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        button_pressed = true;
        upd_edits();
    }

    static void ch_check_click(HWND hwnd, WORD LoWord, WORD HiWord, LPARAM lParam) {
        state.calc_in_real_time = (SendMessage(check_box, BM_GETCHECK, 0, 0) == BST_CHECKED);
        Button_Enable(button, false);
        upd_edits();
    }


    static bool is_valid_data(HWND hctrl, int ctrl_id, double &a) {
        wchar_t str[100];
        Edit_GetText(hctrl, str, 100);
        if (wcslen(str) >= 19)
            return false;
        std::wstringstream ss(str);
        if (str[wcslen(str) - 1] == '.')
            return false;
        ss >> a;
        if (!ss)
            return false;
        std::wstring s;
        std::getline(ss, s);
        int Operator = ComboBox_GetCurSel(combo_box);
        if (state.operation_set == 0 && (Operator == 3 || Operator == 4) && ctrl_id == id_operand2 && a == 0)
            return false;
        if ((state.operation_set == 1 || Operator == 4) && (int) a != a)
            return false;
        if (state.operation_set == 2 && a != 0 && a != 1)
            return false;
        return s.empty();


    }

    static std::wstring operation(double op1, double op2) {
        int Operator = ComboBox_GetCurSel(combo_box);
        switch (state.operation_set) {
            case 0: {
                double res = 0;
                switch (Operator) {
                    case 0:
                        res = op1 + op2;
                        break;
                    case 1:
                        res = op1 - op2;
                        break;
                    case 2:
                        res = op1 * op2;
                        break;
                    case 3:
                        res = op1 / op2;
                        break;
                    case 4:
                        res = (unsigned int) op1 % (unsigned int) op2;
                        break;
                    default:
                        break;
                }
                std::wstringstream ss;
                ss << std::fixed << std::setprecision(state.precision) << res;
                std::wstring str;
                ss >> str;
                return str;
            }
            case 1: {
                unsigned int res = 0;
                switch (Operator) {
                    case 0:
                        res = (((unsigned int) op1) & ((unsigned int) op2));
                        break;
                    case 1:
                        res = (((unsigned int) op1) | ((unsigned int) op2));
                        break;
                    case 2:
                        res = (((unsigned int) op1) ^ ((unsigned int) op2));
                        break;
                    default:
                        break;
                }
                std::wstringstream ss;
                ss << res;
                std::wstring str;
                ss >> str;
                return str;
            }
            case 2: {
                bool res = false;
                switch (Operator) {
                    case 0:
                        res = (((bool) op1) && ((bool) op2));
                        break;
                    case 1:
                        res = (((bool) op1) || ((bool) op2));
                        break;
                    case 2:
                        res = (((bool) op1) ^ ((bool) op2));
                        break;
                    case 3:
                        res = ((!((unsigned int) op1)) || ((unsigned int) op2));
                        break;
                    case 4:
                        res = !(((bool) op1) ^ ((bool) op2));
                        break;
                    default:
                        break;
                }
                std::wstringstream ss;
                ss << std::fixed << std::boolalpha << res;
                std::wstring str;
                ss >> str;
                return str;
            }
        }

    }

    static void upd_combo_box();

    static void upd_edits();

    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }


private:
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

    static HWND CreateRadioBoxForOperations(int x, int y, Control_ids id) {
        auto new_radio_box = CreateControl(WC_BUTTON, BS_GROUPBOX, x, y, 200,
                                           90 + (consts::number_of_operations - 3) * 20, id, L"Выберите операции");
        auto btn1 = CreateControl(WC_BUTTON, BS_AUTORADIOBUTTON | WS_GROUP, x + 10, y + 20, 180, 20,
                                  operation_ids_set[0].first, consts::operation_names[0].c_str());
        for (int i = 1; i < consts::number_of_operations; i++)
            auto btn = CreateControl(WC_BUTTON, BS_AUTORADIOBUTTON, x + 10, y + 20 + 20 * i, 180, 20,
                                     operation_ids_set[i].first, consts::operation_names[i].c_str());
        SendMessage(btn1, BM_SETCHECK, BST_CHECKED, 1);
        return new_radio_box;
    }

    static HWND CreateComboBox(int x, int y, Control_ids id, int width = 40, int height = 100) {

        auto combo_box = CreateControl(WC_COMBOBOX, CBS_DROPDOWN | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP, x, y,
                                       width, height,
                                       id);
        SetWindowFont(combo_box, _font, 1);

        insert_strings(combo_box, consts::operations[0]);

        SendMessage(combo_box, CB_SETCURSEL, (WPARAM) 0, (LPARAM) 0);
        return combo_box;
    }

    static HWND CreateSlider(int min_val, int max_val, Control_ids id);

    static void insert_strings(HWND combo_box, const std::vector<std::wstring> &pString);
};

WinApi::DialogMessageMap Program::message_map;
HINSTANCE Program::hinst;
HWND Program::_hDialog;
HWND Program::operand1, Program::operand2, Program::result, Program::radio_box, Program::check_box, Program::combo_box, Program::button, Program::slider;
bool  Program::button_pressed = false;
Program::State Program::state;
HFONT Program::_font;
std::vector<std::pair<int, bool> > Program::operation_ids_set;

void Program::upd_combo_box() {
    SendMessage(combo_box, CB_RESETCONTENT, 0, 0);
    insert_strings(combo_box, consts::operations[state.operation_set]);
    SendMessage(combo_box, CB_SETCURSEL, 0, 0);
}

void Program::insert_strings(HWND combo_box, const std::vector<std::wstring> &pString) {
    for (const auto &i : pString)
        SendMessage(combo_box, CB_ADDSTRING, 0, (LPARAM) (i.c_str()));
}

void Program::upd_edits() {
    InvalidateRect(operand1, nullptr, 1);
    InvalidateRect(operand2, nullptr, 1);
}

HWND Program::CreateSlider(int min_val, int max_val, Control_ids id) {
    auto slider = CreateControl(TRACKBAR_CLASS, WS_TABSTOP | TBS_AUTOTICKS | TBS_HORZ, 240, 160, 150, 32, id);
    SendMessage(slider, TBM_SETRANGE, TRUE, MAKELONG(min_val, max_val));
    return slider;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}