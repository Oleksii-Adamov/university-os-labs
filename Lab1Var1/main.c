#include <stdio.h>
#include <trialfuncs.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#define MAX_SOFT_FAIL_TRIES 10
#define FUNC_RETURN_TYPE TYPENAME_imin
#define USER_DIALOG_DURATION_MSEC 5000

struct ProcessInfo {
    int id;
    int read_fd;
    int write_fd;
};

enum Func {
    FUNC_F,
    FUNC_G
};

struct ProcessInfo start_process(int x, enum Func func) {
    struct ProcessInfo process_info;
    // creating pipe
    int fd[2];
    if (pipe(fd) == -1) {
        printf("Error: Cannot open a pipe\n");
        exit(2);
    }
    process_info.read_fd = fd[0];
    process_info.write_fd = fd[1];

    // staring process (forking)
    process_info.id = fork();
    if (process_info.id == -1) {
        printf("Error: Cannot create a fork\n");
        exit(1);
    }
    if (process_info.id == 0) { // child process
        // closing reading pipe
        close(process_info.read_fd);
        // calling function f or g
        enum _compfunc_status status;
        TYPENAME_imin result = 0;
        if (func == FUNC_F) {
            status = trial_f_imin(x, &result);
        }
        else if (func == FUNC_G) {
            status = trial_g_imin(x, &result);
        }
        // writing result to pipe
        write(process_info.write_fd, &status, sizeof(enum _compfunc_status));
        write(process_info.write_fd, &result, sizeof(int));
        // closing pipe
        close(process_info.write_fd);
        exit(0);
    }
    else {
        // closing writing pipe from main process
        close(process_info.write_fd);
        return process_info;
    }
}

void receive(struct pollfd* pfd, FUNC_RETURN_TYPE* ret_val, enum Func func, int x, bool* computed, bool* fail, enum _compfunc_status* status,
        unsigned int* remaining_soft_fail_tries) {
    read((*pfd).fd, status, sizeof(enum _compfunc_status));
    read((*pfd).fd, ret_val, sizeof(FUNC_RETURN_TYPE));
    if (*status == COMPFUNC_HARD_FAIL) {
        *fail = true;
    }
    else if (*status == COMPFUNC_SOFT_FAIL) {
        if (*remaining_soft_fail_tries > 0) {
            (*remaining_soft_fail_tries)--;
            struct ProcessInfo process_info = start_process(x, func);
            (*pfd).fd = process_info.read_fd;
        }
        else {
            *fail = true;
        }
    }
    else if (*status == COMPFUNC_SUCCESS) {
        *computed = true;
    }
}

char* print_status_info(enum _compfunc_status status, unsigned int soft_fail_tries) {
    printf("%s", symbolic_status(status));
    if (status == COMPFUNC_SOFT_FAIL) {
        printf(" (%d/%d attempts)", MAX_SOFT_FAIL_TRIES - soft_fail_tries, MAX_SOFT_FAIL_TRIES);
    }
}

int main() {
    srand(3);

    bool f_computed = false, g_computed = false, fail = false, is_interrupted = false;
    enum _compfunc_status f_status = COMPFUNC_STATUS_MAX, g_status = COMPFUNC_STATUS_MAX;
    unsigned int f_soft_fail_tries = MAX_SOFT_FAIL_TRIES, g_soft_fail_tries = MAX_SOFT_FAIL_TRIES;
    FUNC_RETURN_TYPE f, g;
    bool user_dialog_active = false;
    time_t user_dialog_start_sec;

    printf ("x = ");
    int x;
    scanf("%d", &x);

    // staring processes for computing f and g
    struct ProcessInfo f_process_info = start_process(x, FUNC_F);
    struct ProcessInfo g_process_info = start_process(x, FUNC_G);

    // polling setup
    struct pollfd pfds[3];
    // console input (stdin)
    pfds[0].fd = 0;
    pfds[1].fd = f_process_info.read_fd;
    pfds[2].fd = g_process_info.read_fd;
    for (int i = 0; i < 3; i++) {
        pfds[i].events = POLLIN;
    }
    while (!(f_computed && g_computed) && !fail) {
        printf("Started cycle\n");
        int poll_ret;
        if (user_dialog_active) {
            poll_ret = poll(pfds, 3, USER_DIALOG_DURATION_MSEC);
        }
        else {
            poll_ret = poll(pfds, 3, -1);
        }
        if (poll_ret == -1) {
            printf("Error: Poll error\n");
            exit(2);
        }
        if (poll_ret == 0) {
            user_dialog_active = false;
            printf("action is not confirmed within %d seconds proceeding...\n", USER_DIALOG_DURATION_MSEC / 1000);
        }
        if (poll_ret > 0) {
            // message from f process
            if (pfds[1].revents & POLLIN) {
                receive(&pfds[1], &f, FUNC_F, x, &f_computed, &fail, &f_status, &f_soft_fail_tries);
            }
            // message from g process
            if (pfds[2].revents & POLLIN) {
                receive(&pfds[2], &g, FUNC_G, x, &g_computed, &fail, &g_status, &g_soft_fail_tries);
            }
            // input from user
            if (pfds[0].revents & POLLIN) {
                char buf[256];
                read(pfds[0].fd, buf, sizeof(char) * 256);
                if (!user_dialog_active && buf[0] == '\n' && !(f_computed && g_computed)) {
                    user_dialog_active = true;
                    printf("Please confirm that computation should be stopped y(es, stop)/n(ot yet)\n");
                    //user_dialog_start_sec = time(0);
                    //printf("%ld\n", user_dialog_start_sec);
                }
                else if (user_dialog_active && buf[0] == 'y' && buf[1] == '\n') {
                    user_dialog_active = false;
                    is_interrupted = true;
                    break;
                }
                else if (user_dialog_active && buf[0] == 'n' && buf[1] == '\n') {
                    user_dialog_active = false;
                }
            }
        }
        // duration of user dialog expired
//        printf("%ld ,%ld\n", time(0), time(0) - user_dialog_start_sec);
//        if (user_dialog_active && (time(0) - user_dialog_start_sec) >= USER_DIALOG_DURATION) {
//            user_dialog_active = false;
//            printf("action is not confirmed within %d seconds proceeding...\n", USER_DIALOG_DURATION);
//        }
//        printf("Ended cycle\n");
    }
    if (user_dialog_active) {
        printf("overriden by system\n");
    }
    if (!is_interrupted || (f_computed && g_computed) || fail) {
        printf("Result: ");
        if (fail) {
            printf("Fail ( f: ");
            print_status_info(f_status, f_soft_fail_tries);
            printf(", g: ");
            print_status_info(g_status, g_soft_fail_tries);
            printf(" )\n\n");
            //printf("Fail ( f: %s, g: %s\n\n", symbolic_status(f_status), symbolic_status(g_status));
            //printf("Fail ( f: ", symbolic_status(f_status), " g: ", symbolic_status(g_status), " )");
        } else {
            FUNC_RETURN_TYPE result = (f < g) ? f : g;
            printf("%d\n\n", result);
        }
    }
    else {
        printf("Canceled by user\n");
        printf("f: ");
        print_status_info(f_status, f_soft_fail_tries);
        printf("\ng: ");
        print_status_info(g_status, g_soft_fail_tries);
        printf("\n\n");
    }
    return 0;
}
