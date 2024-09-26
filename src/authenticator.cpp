#include "header.h"

#define SERVICE_NAME "display_manager"

#define err(name)                                   \
    do {                                            \
        fprintf(stderr, "%s: %s\n", name,           \
                pam_strerror(pam_handle, result));  \
        end(result);                                \
        return false;                               \
    } while (1);



bool Authenticator::login(const char *username, const char *password, pid_t *child_pid) {
    const char *data[2] = {username, password};
    pam_conv pam_conv = { conv, data };

    int result = pam_start(SERVICE_NAME, username, &pam_conv, &pam_handle);
    if (result != PAM_SUCCESS) {
        err("pam_start");
    }

    result = pam_authenticate(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        err("pam_authenticate");
    }

    result = pam_acct_mgmt(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        err("pam_acct_mgmt");
    }

    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);
    if (result != PAM_SUCCESS) {
        err("pam_setcred");
    }

    result = pam_open_session(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        err("pam_open_session");
    }

    struct passwd *pw = getpwnam(username);
    init_env(pw);

    *child_pid = fork();
    if (*child_pid == 0) {
        chdir(pw->pw_dir);
        // We don't use ~/.xinitrc because we should already be in the users home directory
        char *cmd = "exec /bin/bash --login .xinitrc";
        execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
        printf("Failed to start window manager");
        exit(1);
    }

    return true;
}
