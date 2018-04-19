//
// Created by serko on 19.04.2018.
//

#include "MenuClass.h"

int WinApi::MenuClass::next_id = 1000;

WinApi::MenuClass &WinApi::MenuClass::CreateMenuClass(HWND _hwnd, WinApi::MessageMapBase &_message_map) {
    hwnd = _hwnd;
    message_map = &_message_map;
    menu_items[0] = make_menu_item(CreateMenu(), nullptr);
    return *this;
}

WinApi::MenuClass &
WinApi::MenuClass::AddMenuItem(int id, WinApi::MessageMapBase::MessageHandler handler, const std::wstring &name,
                               int parent_id, UINT flags) {
    if (id >= next_id)
        next_id = id + 1;
    if (id == -1 || id == 0)
        id = next_id++;

    auto it = menu_items.find(parent_id);
    if (it == menu_items.end())
        throw (std::out_of_range(parent_id == 0 ? "forget to CreateMenuClass" : "invalid parent id"));
    else
        AppendMenuW(it->second.self_hmenu, flags, static_cast<UINT_PTR>(id), name.c_str());

    menu_items[id] = make_menu_item(nullptr, it->second.self_hmenu);
    if (handler != nullptr)
        message_map->AddCommandHandler(id, handler);
    return *this;
}


WinApi::MenuClass &
WinApi::MenuClass::AddPopupMenuItem(int id, const std::wstring &name, int parent_id, UINT flags) {//! flags+MF_POPUP
    if (id >= next_id)
        next_id = id + 1;
    if (id == -1)
        id = next_id++;

    auto it = menu_items.find(parent_id);
    if (it == menu_items.end())
        throw (std::out_of_range(parent_id == 0 ? "forget to CreateMenuClass" : "invalid parent id"));

    menu_items[id] = make_menu_item(CreatePopupMenu(), it->second.self_hmenu);

    AppendMenuW(menu_items[id].parent_hmenu, MF_POPUP | flags, (UINT_PTR) menu_items[id].self_hmenu, name.c_str());

    return *this;
}

WinApi::MenuClass &WinApi::MenuClass::EnableItem(int id) {
    auto it = menu_items.find(id);
    if (it == menu_items.end())
        throw (std::out_of_range("invalid id"));
    else
        EnableMenuItem(it->second.parent_hmenu, static_cast<UINT>(id), MF_ENABLED);
}

WinApi::MenuClass::MenuItem WinApi::MenuClass::make_menu_item(HMENU self, HMENU parent) {
    MenuItem tmp{self, parent};
    return tmp;
}

WinApi::MenuClass &WinApi::MenuClass::DisableItem(int id) {
    auto it = menu_items.find(id);
    if (it == menu_items.end())
        throw (std::out_of_range("invalid id"));
    else
        EnableMenuItem(it->second.parent_hmenu, static_cast<UINT>(id), MF_DISABLED);
}
