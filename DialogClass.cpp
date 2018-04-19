//
// Created by serko on 19.04.2018.
//

#include "DialogClass.h"

namespace WinApi {

    HCreateDialog::HCreateDialog(HINSTANCE hinst, HWND parent, DLGPROC proc) {
        _hdialog = CreateDialogIndirect(hinst, nullptr, parent, proc);
    }

    HCreateDialog::operator HWND() {
        return _hdialog;
    }
}