//
// Created by serko on 19.04.2018.
//

#ifndef UNTITLED7_MENUCLASS_H
#define UNTITLED7_MENUCLASS_H

#include <windows.h>
#include <vector>
#include "MessageMap.h"

namespace WinApi {
    class MenuClass {
        HWND hwnd;
        MessageMapBase *message_map;
        struct MenuItem {
            HMENU self_hmenu;
            HMENU parent_hmenu;
        };
        std::map<int, MenuItem> menu_items;

        static MenuItem make_menu_item(HMENU self, HMENU parent);

    public:
        static int next_id;

        MenuClass &CreateMenuClass(HWND _hwnd, MessageMapBase &_message_map);

        MenuClass &
        AddMenuItem(int id, MessageMapBase::MessageHandler handler, const std::wstring &name, int parent_id = 0,
                    UINT flags = MF_STRING);

        MenuClass &AddPopupMenuItem(int id, const std::wstring &name, int parent_id = 0, UINT flags = MF_STRING);

        MenuClass &EnableItem(int id);

        MenuClass &DisableItem(int id);

        void SetMenu() {
            ::SetMenu(hwnd, menu_items[0].self_hmenu);
        }

        void DestroyMenu() {
            ::DestroyMenu(menu_items[0].self_hmenu);
        }
    };
}

#endif //UNTITLED7_MENUCLASS_H
