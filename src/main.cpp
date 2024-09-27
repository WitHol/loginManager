#include "header.h"

int main()
{
    TUI tui;

    while(true)
    {
        tui.loop();
    }

    pid_t pid;

    Authenticator authenticator;
    authenticator.login(tui.login.c_str(), tui.password.c_str(), &pid);
    authenticator.logout();

    return 0;
}