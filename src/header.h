#pragma once

#include <iostream>
#include <curses.h>
#include <string>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

void authenticate(const char * user, const char * password);

class TUI
{
    public:

    std::string login;
    std::string password;

    TUI();
    ~TUI();
    void loop();

    private:
    
    const std::string    initLogin = "    login: ";
    const std::string initPassword = " password: ";

    const int width = 60;
    const int height = 6;

    WINDOW * win;

    std::string * current;

    void setup();
    void write();
    void processInput();
};
