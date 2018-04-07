//
// Created by serko on 19.03.2018.
//

#include <vector>
#include <iostream>
#include "MessageMap.h"

void WinApi::CommandMap::AddHandler(int id, void (*handler)(HWND, WORD, WORD, LPARAM), int command) {
    _commandMap.insert(std::make_pair(std::make_pair(id, command), handler));
}

bool WinApi::CommandMap::ProcessCommand(HWND hWindow, WPARAM wParam, LPARAM lParam) {
    auto loWord = LOWORD(wParam);
    auto hiWord = HIWORD(wParam);
    auto iterator = _commandMap.find(std::make_pair(loWord, hiWord));
    if (iterator != _commandMap.end()) {
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
WinApi::MessageMapBase::AddCommandHandler(int id, int command, void (*handler)(HWND, WORD, WORD, LPARAM)) {
    _commandMap.AddHandler(id, handler, command);
    return *this;
}

LRESULT WinApi::MessageMapBase::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND && _commandMap.ProcessCommand(hWnd, wParam, lParam)) {
        return GetProcessedValue();
    }

    auto lResultIterator = _messageWithResultMap.find(message);
    if (lResultIterator != _messageWithResultMap.end()) {
        return lResultIterator->second(hWnd, wParam, lParam);
    }

    auto voidIterator = _messageMap.find(message);
    if (voidIterator != _messageMap.end()) {
        voidIterator->second(hWnd, wParam, lParam);
        return GetProcessedValue();
    }

    return GetDefaultValue(hWnd, message, wParam, lParam);
}

WinApi::MessageMapBase &
WinApi::MessageMapBase::AddCommandHandlersSet(const std::vector<std::pair<int, bool> > &ids, int command,
                                              void (*handler)(HWND, WORD, WORD, LPARAM)) {
    for (auto i:ids) {
        AddCommandHandler(i.first, command, handler);
    }
    return *this;
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
//    DefWindowProc(hWnd,message,wParam,lParam);
    return 0;
}
