#define UNICODE
// format of Questions.txt:
// type(0 for alternative, 1 for multichoise)
// title
// amount of answers
// strings(one string for each answer)
// number of correct answer for alternative/amount of correct answers for multychoise
// numbers of correct answers for multychoise

#include <windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <ntdef.h>
#include <cmath>
#include <numeric>
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
    id_prev_button = 1112, id_next_button, id_close_button
};

using namespace WinApi;


class Question {
protected:
    static int cur_number;
    int number;
    std::wstring title;
    std::vector<std::wstring> answers;
    InitialiseDialogAndControlls &dlg;
    std::vector<bool> user_answer;

    void read_answers(std::wistream &fin) {
        unsigned int amount_of_answers;
        fin >> amount_of_answers;
        if (!fin) {
            MessageBox(NULL, L"invalid data, please check if the question is correct", L"Error", MB_ICONERROR | MB_OK);
            exit(0);
        }
        answers.resize(amount_of_answers);
        for (auto &i:answers) {
            fin >> i;
        }
    }

public:
    explicit Question(InitialiseDialogAndControlls &dlg) : number(cur_number++), dlg(dlg) {}

    virtual void read(std::wistream &fin) = 0;

    virtual void paint(HWND hwnd, HDC hdc) = 0;

    virtual bool check(HWND hwnd) = 0;

    void clear() {
        dlg.DeleteControls();
    }
};

class AlternativeQuestion : public Question {
    int correct_answer{};
public:
    explicit AlternativeQuestion(std::wistream &fin, InitialiseDialogAndControlls &dlg) : Question(dlg) { read(fin); }

    void read(std::wistream &fin) override {
        std::getline(fin, title);
        std::getline(fin, title);
        read_answers(fin);
        fin >> correct_answer;
        user_answer.resize(answers.size(), false);
        if (!fin || correct_answer > answers.size()) {
            MessageBox(NULL, L"Error", L"empty or invalid file", MB_ICONERROR | MB_OK);
            exit(0);
        }
    }

    void paint(HWND hwnd, HDC hdc) override {
        auto rect = GetClientRect(hwnd);
        rect.top += 20;
        rect.right -= 40;
        rect.bottom -= 20 + 5 * 20 + 5 * 10;
        rect.left += 20;
        RECT text_rect = rect;
        DrawText(hdc, title.c_str(), -1, &text_rect, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);
        DrawText(hdc, title.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);
        rect.top += GetRectHeigth(text_rect) + 20;
        dlg.DeleteControls();
        auto cur_top_pos = rect.top;
        for (int i = 0; i < answers.size(); i++) {
            dlg._CreateButton({20, cur_top_pos}, i + 1000, GetRectWidth(rect), 20,
                              static_cast<unsigned long>(BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0)),
                              answers[i]);
            SendMessage(GetDlgItem(hwnd, i + 1000), BM_SETCHECK, user_answer[i] ? BST_CHECKED : BST_UNCHECKED, 0);
            cur_top_pos += 30;
        }
        dlg.CreatePushButton({rect.left, cur_top_pos}, id_prev_button, 100, 20, 0, L"Назад");
        dlg.CreatePushButton({rect.right - 100, cur_top_pos}, id_next_button, 100, 20, 0, L"Вперед");
    }

    bool check(HWND hwnd) override {
        user_answer.resize(answers.size(), false);
        for (int i = 0; i < answers.size(); i++) {
            user_answer[i] = SendMessage(GetDlgItem(hwnd, 1000 + i), BM_GETCHECK, 0, 0);
        }
        return (bool) SendMessage(GetDlgItem(hwnd, 999 + correct_answer), BM_GETCHECK, 0, 0);
    }

};

class MultiChoiseQuestion : public Question {
    std::vector<bool> correct_answers{};

public:
    explicit MultiChoiseQuestion(std::wistream &fin, InitialiseDialogAndControlls &dlg) : Question(dlg) { read(fin); }

    void read(std::wistream &fin) override {
        std::getline(fin, title);
        std::getline(fin, title);
        read_answers(fin);
        unsigned int amount_of_correct_answers;
        fin >> amount_of_correct_answers;
        if (amount_of_correct_answers > answers.size()) {
            MessageBox(NULL, L"Error", L"invalid file", MB_ICONERROR | MB_OK);
            exit(0);
        }
        correct_answers.resize(answers.size());
        for (int i = 0; i < amount_of_correct_answers; i++) {
            int tmp;
            fin >> tmp;
            if (tmp > correct_answers.size()) {
                MessageBox(NULL, L"Error", L"invalid file", MB_ICONERROR | MB_OK);
                exit(0);
            }

            correct_answers[--tmp] = true;
        }
        user_answer.resize(answers.size(), false);
        if (!fin) {
            MessageBox(NULL, L"Error", L"invalid file", MB_ICONERROR | MB_OK);
            exit(0);
        }
    }

    void paint(HWND hwnd, HDC hdc) override {
        auto rect = GetClientRect(hwnd);
        rect.top += 20;
        rect.right -= 40;
        rect.bottom -= 20 + 5 * 20 + 5 * 10;
        rect.left += 20;
        RECT text_rect = rect;
        DrawText(hdc, title.c_str(), -1, &text_rect, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);
        DrawText(hdc, title.c_str(), -1, &rect, DT_CENTER | DT_WORDBREAK);
        rect.top += GetRectHeigth(text_rect) + 20;
        dlg.DeleteControls();
        auto cur_top_pos = rect.top;
        for (int i = 0; i < answers.size(); i++) {
            dlg._CreateButton({20, cur_top_pos}, i + 1000, GetRectWidth(rect), 20,
                              static_cast<unsigned long>(BS_AUTOCHECKBOX), answers[i]);
            SendMessage(GetDlgItem(hwnd, i + 1000), BM_SETCHECK, user_answer[i] ? BST_CHECKED : BST_UNCHECKED, 0);
            cur_top_pos += 30;
        }
        dlg.CreatePushButton({rect.left, cur_top_pos}, id_prev_button, 100, 20, 0, L"Назад");
        dlg.CreatePushButton({rect.right - 100, cur_top_pos}, id_next_button, 100, 20, 0, L"Вперед");
    }

    bool check(HWND hwnd) override {
        for (int i = 0; i < answers.size(); i++) {
            user_answer[i] = SendMessage(GetDlgItem(hwnd, 1000 + i), BM_GETCHECK, 0, 0);
        }
        for (auto i:correct_answers)
            if (!SendMessage(GetDlgItem(hwnd, i + 1000), BM_GETCHECK, 0, 0))
                return false;
        for (int i = 0; i < answers.size(); i++)
            if (SendMessage(GetDlgItem(hwnd, i + 1000), BM_GETCHECK, 0, 0) ^ correct_answers[i])
                return false;
        return true;
    }


};

int Question::cur_number = 1;

class Program {
    static WinApi::DialogMessageMap message_map;
    static HINSTANCE hinst;
    static int question_number;
    static std::vector<Question *> questions;
    static std::vector<bool> answers;


    static void init_message_map() {
        message_map.AddHandler(WM_PAINT, on_paint)
                .AddHandler(WM_DESTROY, on_destroy)
                .AddHandler(WM_CLOSE, on_close)
                .AddHandler(WM_INITDIALOG, on_initialise)
                .AddCommandHandler(id_next_button, ch_next)
                .AddCommandHandler(id_prev_button, ch_prev)
                .AddCommandHandler(id_close_button, on_close);
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
        HCreateDialog(hinst, nullptr, wnd_proc);
        return main_loop();
    }

private:
    static void paint_last_question(HWND hwnd, HDC hdc);

    static LRESULT on_paint(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        static int last_question_number = -1;
        if (last_question_number == question_number)
            return 0;
        auto hdc = DoubleBuffering(hwnd);
        ClearBackgroud(hdc, GetClientRect(hwnd));
        SetBkMode(hdc, TRANSPARENT);
        if (question_number >= questions.size())
            paint_last_question(hwnd, hdc);
        else
            questions[question_number]->paint(hwnd, hdc), last_question_number = question_number;
    }


    static LRESULT on_initialise(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        static auto dlg = InitialiseDialogAndControlls(hinst, hwnd).init_dialog();
        std::wifstream fin("Questions.txt");
        while (fin) {
            bool type;
            fin >> type;
            if (!fin)
                break;
            if (type)
                questions.emplace_back(new MultiChoiseQuestion(fin, dlg));
            else
                questions.emplace_back(new AlternativeQuestion(fin, dlg));
            InvalidateRect(hwnd, nullptr, false);
        }
        if (questions.empty()) {
            MessageBox(hwnd, L"Error", L"empty or invalid file", MB_ICONERROR | MB_OK);
            DestroyWindow(hwnd);
        }
        answers.resize(questions.size(), false);
    }

    static void check(HWND hwnd) {
        answers[question_number] = questions[question_number]->check(hwnd);
    }

    static void ch_next(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (HIWORD(wparam) != BN_CLICKED)
            return;
        check(hwnd);
        question_number++;
        InvalidateRect(hwnd, nullptr, false);
    }

    static void ch_prev(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        if (HIWORD(wparam) != BN_CLICKED)
            return;
        check(hwnd);
        if (question_number == 0)
            return;
        question_number--;
        InvalidateRect(hwnd, nullptr, false);
    }

    static void on_destroy(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        PostQuitMessage(0);
    }

    static void on_close(HWND hwnd, WPARAM wparam, LPARAM lparam) {
        DestroyWindow(hwnd);
    }

    static INT_PTR CALLBACK wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        return message_map.ProcessMessage(hwnd, message, wParam, lParam);
    }
};

WinApi::DialogMessageMap Program::message_map;
HINSTANCE Program::hinst;

std::vector<Question *> Program::questions;

std::vector<bool> Program::answers;

int Program::question_number = 0;

void Program::paint_last_question(HWND hwnd, HDC hdc) {

    auto rect = GetClientRect(hwnd);
    questions.front()->clear();
    CreateWindow(WC_BUTTON, L"Finish", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect.right - 140, rect.bottom - 60,
                 100, 20, hwnd, (HMENU) id_close_button, hinst, nullptr);
    int correct_answers = std::accumulate(answers.begin(), answers.end(), 0);
    double score = 1.0 * correct_answers / answers.size();
    auto brush = CreateSolidBrush(GetRandColor());
    brush = (HBRUSH) SelectObject(hdc, brush);
    Rectangle(hdc, 20, rect.bottom - (GetRectHeigth(rect) - 40) * score - 20, 60, rect.bottom - 20);
    SetBkMode(hdc, TRANSPARENT);
    auto str = std::to_wstring((int) (score * 100)) + L"%";
    TextOut(hdc, 20, rect.bottom - (GetRectHeigth(rect) - 40) * score - 20, str.c_str(),
            str.size());
    DeleteObject(SelectObject(hdc, brush));
    brush = CreateSolidBrush(GetRandColor());
    brush = (HBRUSH) SelectObject(hdc, brush);
    Rectangle(hdc, 70, rect.bottom - (GetRectHeigth(rect) - 40) * (1.0 - score) - 20, 110, rect.bottom - 20);
    str = std::to_wstring((int) ((1.0 - score) * 100)) + L"%";
    TextOut(hdc, 70, rect.bottom - (GetRectHeigth(rect) - 40) * (1.0 - score) - 20,
            str.c_str(), str.size());
    DeleteObject(SelectObject(hdc, brush));

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return Program::main(hInstance, nCmdShow);
}