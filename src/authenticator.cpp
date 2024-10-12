#include "header.h"

#define SERVICE_NAME "system-login"

#define err(name)                                                               \
    {                                                                           \
        std::cerr << name << ": " << pam_strerror(phandle, result) << "\n";     \
        end(result);                                                            \
        return false;                                                           \
    }                                                                           \

bool Authenticator::login(const char *username, const char *password, pid_t *child_pid) 
{
    const char * data[2] = {username, password};
    struct pam_conv pconv = {conv, data};
    phandle = NULL;
    int result;

    result = pam_start(SERVICE_NAME, username, &pconv, &phandle);
    if(result != PAM_SUCCESS) err("pam_start");

    result = pam_authenticate(phandle, 0);
    if(result != PAM_SUCCESS) err("pam_authenticate");

    result = pam_acct_mgmt(phandle, 0);
    if(result != PAM_SUCCESS) err("pam_acct_mgmt");

    result = pam_setcred(phandle, PAM_ESTABLISH_CRED);
    if(result != PAM_SUCCESS) err("pam_sercred");
    
    result = pam_open_session(phandle, 0);
    if(result != PAM_SUCCESS) err("pam_open_session");
    
    struct passwd *pw = getpwnam(username);
    init_env(pw);

    *child_pid = fork();
    if (*child_pid == 0) 
    {
        setuid(pw->pw_uid);
        chdir(pw->pw_dir);
        char *cmd = "exec /usr/bin/sh --login .xinitrc";
        execl(pw->pw_shell, pw->pw_shell, "-c", cmd);
        printf("Failed to start window manager");
        exit(1);
    }


    delete phandle;
    return true;
}

bool Authenticator::logout(void) {
    int result = pam_close_session(phandle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(phandle, PAM_DELETE_CRED);
        err("pam_close_session");
    }

    result = pam_setcred(phandle, PAM_DELETE_CRED);
    if (result != PAM_SUCCESS) {
        err("pam_setcred");
    }

    end(result);
    return true;
}

int Authenticator::end(int last_result) {
    int result = pam_end(phandle, last_result);
    phandle = 0;
    return result;
}

int Authenticator::conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) 
{
    *resp = (pam_response*)calloc(num_msg, sizeof(struct pam_response));
    if (*resp == NULL) return PAM_BUF_ERR;

    int result = PAM_SUCCESS;
    int i;
    for (i = 0; i < num_msg; i++) {
        char *username, *password;

        switch ((*msg[i]).msg_style) 
        {
        case PAM_PROMPT_ECHO_ON:
            username = ((char **) appdata_ptr)[0];
            (*resp)[i].resp = strdup(username);
            break;

        case PAM_PROMPT_ECHO_OFF:
            password = ((char **) appdata_ptr)[1];
            (*resp)[i].resp = strdup(password);
            break;

        case PAM_ERROR_MSG:
            std::cerr << (*msg[i]).msg << "\n";
            result = PAM_CONV_ERR;
            break;

        case PAM_TEXT_INFO:
            std::cerr << (*msg[i]).msg << "\n";
            break;

        }

        if (result != PAM_SUCCESS) break;
    }

    if (result != PAM_SUCCESS) 
    {
        free(*resp);
        *resp = 0;
    }

    return result;
}

void Authenticator::init_env(struct passwd *pw) 
{
    set_env("HOME", (*pw).pw_dir);
    set_env("PWD", (*pw).pw_dir);
    set_env("SHELL", (*pw).pw_shell);
    set_env("USER", (*pw).pw_name);
    set_env("LOGNAME", (*pw).pw_name);
    set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/bin");
    set_env("MAIL", _PATH_MAILDIR);

    size_t xauthority_len = strlen((*pw).pw_dir) + strlen("/.Xauthority") + 1;
    char *xauthority = (char*)malloc(xauthority_len);
    snprintf(xauthority, xauthority_len, "%s/.Xauthority", (*pw).pw_dir);
    set_env("XAUTHORITY", xauthority);
    free(xauthority);
}

void Authenticator::set_env(char *name, char *value) 
{
    size_t name_value_len = strlen(name) + strlen(value) + 2;
    char *name_value = (char*)malloc(name_value_len);
    snprintf(name_value, name_value_len,  "%s=%s", name, value);
    pam_putenv(phandle, name_value);
    free(name_value);
}
