#include "header.h"

int main()
{
    TUI tui;

    while(true)
    {
        tui.loop();
    }

    authenticate(tui.login.c_str(), tui.password.c_str());

    return 0;
}