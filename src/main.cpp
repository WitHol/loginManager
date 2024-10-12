#include "header.h"

int main()
{
    if(getuid() != 0)
    {
        std::cout << "Witdm not running as root!" << std::endl;
        return -1;
    }

    TUI tui;
    while(true)
    {
        tui.loop();

        if(tui.close) break;
    }

    endwin();

    pid_t pid;

    Authenticator authenticator;
    authenticator.login("withol", "wit.hol2008", &pid);
    authenticator.logout();

    return 0;
}