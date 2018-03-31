#include <windows.h>
#include "MessageMap.h"

class Program{
    static WinApi::MessageMap map;
    static HINSTANCE hinst;
public:
    static int main(HINSTANCE hInstance,int nCmdShow){
        hinst=hInstance;

    }
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return 0;
}