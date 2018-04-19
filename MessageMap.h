//
// Created by serko on 19.03.2018.
//

#ifndef UNTITLED7_COMMANDMAP_H
#define UNTITLED7_COMMANDMAP_H

#include <map>
#include <Windows.h>

namespace WinApi {
    class CommandMap final {
        std::map<int, void (*)(HWND, WPARAM, LPARAM)> _commandMap;

    public:
        void AddHandler(int command, void (*handler)(HWND, WPARAM, LPARAM));

        bool ProcessCommand(HWND hWindow, WPARAM wParam, LPARAM lParam);
    };


    class MessageMapBase {
    public:
        typedef void (*MessageHandler)(HWND, WPARAM, LPARAM);

        typedef LRESULT (*MessageHandlerWithResult)(HWND, WPARAM, LPARAM);

    private:
        std::map<UINT, MessageHandler> _messageMap;
        std::map<UINT, LRESULT(*)(HWND, WPARAM, LPARAM)> _messageWithResultMap;

        CommandMap _commandMap;
    public:
        MessageMapBase &AddHandler(UINT message, void handler(HWND, WPARAM, LPARAM));

        MessageMapBase &AddHandler(UINT message, LRESULT handler(HWND, WPARAM, LPARAM));

        MessageMapBase &AddCommandHandler(int command, void (*handler)(HWND, WPARAM, LPARAM));

        LRESULT ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual LRESULT GetProcessedValue() = 0;

        virtual LRESULT GetDefaultValue(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    };


    class MessageMap final : public MessageMapBase {
    protected:
        LRESULT GetProcessedValue() override;

        LRESULT GetDefaultValue(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    };


    class DialogMessageMap final : public MessageMapBase {
    protected:
        LRESULT GetProcessedValue() override;

        LRESULT GetDefaultValue(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
    };
}


#endif //UNTITLED7_COMMANDMAP_H
