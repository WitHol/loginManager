#pragma once

#include <iostream>
#include <curses.h>
#include <string>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdbool.h>
#include <pwd.h>
#include <paths.h>
#include <unistd.h>

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

class Authenticator
{
    public:
    
    bool login(const char * user, const char * password, pid_t * childPid);
    bool logout();

    private:

    pam_handle_t *phandle;

    void init_env(struct passwd *pw);
    void set_env(char *name, char *value);
    int end(int last_result);
    static int conv(int num_msg, const struct pam_message **msg,
            struct pam_response **resp, void *appdata_ptr);
};