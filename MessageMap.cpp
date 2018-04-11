//
// Created by serko on 19.03.2018.
//

#include "MessageMap.h"

void WinApi::CommandMap::AddHandler(int command, void (*handler)(HWND, WORD, WORD, LPARAM)) {
    _commandMap.insert(std::make_pair(command, handler));
}

bool WinApi::CommandMap::ProcessCommand(HWND hWindow, WPARAM wParam, LPARAM lParam) {
    auto loWord = LOWORD(wParam);
    auto iterator = _commandMap.find(loWord);
    if (iterator != _commandMap.end()) {
        auto hiWord = HIWORD(wParam);
        iterator->second(hWindow, loWord, hiWord, lParam);
        return true;
    }

    return false;
}

WinApi::MessageMapBase &WinApi::MessageMapBase::AddHandler(UINT message, void (*handler)(HWND, WPARAM, LPARAM)) {
    _messageMap.insert(std::make_pair(message, handler));
    return *this;
}

WinApi::MessageMapBase &WinApi::MessageMapBase::AddHandler(UINT message, LRESULT (*handler)(HWND, WPARAM, LPARAM)) {
    _messageWithResultMap.insert(std::make_pair(message, handler));
    return *this;
}

WinApi::MessageMapBase &
WinApi::MessageMapBase::AddCommandHandler(int command, void (*handler)(HWND, WORD, WORD, LPARAM)) {
    _commandMap.AddHandler(command, handler);
    return *this;
}

LRESULT WinApi::MessageMapBase::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto lResultIterator = _messageWithResultMap.find(message);
    if (lResultIterator != _messageWithResultMap.end()) {
        auto result = lResultIterator->second(hWnd, wParam, lParam);
        if (message == WM_COMMAND && _commandMap.ProcessCommand(hWnd, wParam, lParam)) {
            return GetProcessedValue();
        }
        return result;
    }

    auto voidIterator = _messageMap.find(message);
    if (voidIterator != _messageMap.end()) {
        voidIterator->second(hWnd, wParam, lParam);
        if (message == WM_COMMAND && _commandMap.ProcessCommand(hWnd, wParam, lParam)) {
            return GetProcessedValue();
        }
        return GetProcessedValue();
    }
    if (message == WM_COMMAND && _commandMap.ProcessCommand(hWnd, wParam, lParam)) {
        return GetProcessedValue();
    }
    return GetDefaultValue(hWnd, message, wParam, lParam);
}

LRESULT WinApi::MessageMap::GetProcessedValue() {
    return 0;
}

LRESULT WinApi::MessageMap::GetDefaultValue(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WinApi::DialogMessageMap::GetProcessedValue() {
    return TRUE;
}

LRESULT WinApi::DialogMessageMap::GetDefaultValue(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return FALSE;
}
