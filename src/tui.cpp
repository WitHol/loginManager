#include "header.h"

#define CTRL(c) ((c) & 037)

TUI::TUI()
{
    setup();
    write();
}

TUI::~TUI()
{
    endwin();
}

void TUI::loop()
{
    processInput();
    write();
}

void TUI::processInput()
{
    int input = getch();

    if(false);
    else if(input == KEY_UP) current = &login;
    else if(input == KEY_DOWN) current = &password;
    else if(input == KEY_RIGHT) /* TODO */;
    else if(input == KEY_LEFT) /* TODO */;
    else if(input == CTRL('j') && current == &login) current = &password;
    else if(input == CTRL('j') && current == &password) close = true;
    else if(input == KEY_BACKSPACE){ if((*current).length()) (*current).pop_back(); }
    else (*current).push_back(input);
}

void TUI::setup()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    refresh();

    win = newwin(height+2, width+2, (LINES-height-2)/2, (COLS-width-2)/2);
    box(win, 0, 0);
    wrefresh(win);

    win = newwin(height, width, (LINES-height)/2, (COLS-width)/2);

    current = &login;
}

void TUI::write()
{
    wmove(win, 3, 0);

    wprintw(win, initLogin.c_str());
    wprintw(win, login.c_str());
    wprintw(win, std::string(width-initLogin.length()-login.length(), ' ').c_str());

    wmove(win, 4, 0);
    wprintw(win, initPassword.c_str());
    wprintw(win, password.c_str());
    wprintw(win, std::string(width-initPassword.length()-password.length(), ' ').c_str());

    if(current == &login) wmove(win, 3, initLogin.length() + login.length());
    else wmove(win, 4, initPassword.length() + password.length());

    wrefresh(win);
}
