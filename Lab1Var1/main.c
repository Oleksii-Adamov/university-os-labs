#include <stdio.h>
#include <trialfuncs.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define MAX_SOFT_FAIL_TRIES 5
#define FUNC_RETURN_TYPE TYPENAME_imin
#define USER_DIALOG_DURATION_MSEC 5000
#define READ_BUFF_FROM_CONSOLE_SIZE 256
#define USER_INTERACTION_LATENCY_MSEC 100

struct ProcessInfo {
    // process id
    int id;
    // read and write end of pipe
    int read_fd;
    int write_fd;
};

enum Func {
    FUNC_F,
    FUNC_G
};

// function to start computation of function f or g in different process
struct ProcessInfo start_process(int x, enum Func func) {
    struct ProcessInfo process_info;
    // creating pipe
    int fd[2];
    if (pipe(fd) == -1) {
        printf("Error: Cannot open a pipe\n");
        perror("");
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
        FUNC_RETURN_TYPE result;
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

// receive status and value of computation from pipe, and change variables according to outcome
void receive(struct pollfd* pfd, FUNC_RETURN_TYPE* ret_val, enum Func func, struct ProcessInfo process_info, int x,
        bool* computed, bool* fail, enum _compfunc_status* status, unsigned int* remaining_soft_fail_tries) {
    read((*pfd).fd, status, sizeof(enum _compfunc_status));
    read((*pfd).fd, ret_val, sizeof(FUNC_RETURN_TYPE));
    if (*status == COMPFUNC_HARD_FAIL) {
        *fail = true;
    }
    else if (*status == COMPFUNC_SOFT_FAIL) {
        if (*remaining_soft_fail_tries > 0) {
            (*remaining_soft_fail_tries)--;
            close(process_info.read_fd);
            process_info = start_process(x, func);
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

void print_func_res_info(enum _compfunc_status status, FUNC_RETURN_TYPE val, unsigned int soft_fail_tries) {
    if (status == COMPFUNC_SUCCESS) {
        printf("%d", val);
    }
    else if (status == COMPFUNC_STATUS_MAX) {
        printf("%s", "didn't finish");
    }
    else {
        printf("%s", symbolic_status(status));
    }
    if (status == COMPFUNC_SOFT_FAIL) {
        printf(" (%d/%d attempts)", MAX_SOFT_FAIL_TRIES - soft_fail_tries, MAX_SOFT_FAIL_TRIES);
    }
}

// global variables for communication between main and user interaction threads
bool gl_user_dialog_active = false;
bool gl_interrupted_by_user = false;
pthread_rwlock_t gl_interrupted_by_user_rwlock;

bool is_interrupted_by_user() {
    // locking read lock
    pthread_rwlock_rdlock(&gl_interrupted_by_user_rwlock);
    bool ret = gl_interrupted_by_user;
    // unlocking read lock
    pthread_rwlock_unlock(&gl_interrupted_by_user_rwlock);
    return ret;
}

void* user_interaction() {
    char buf[READ_BUFF_FROM_CONSOLE_SIZE];
    printf("To cancel computation press Enter\n");
    // polling console input (stdin)
    struct pollfd pfd;
    pfd.fd = 0;
    pfd.events = POLLIN;
    while (!is_interrupted_by_user()) {
        int poll_ret;
        if (gl_user_dialog_active) {
            poll_ret = poll(&pfd, 1, USER_DIALOG_DURATION_MSEC);
        }
        else {
            poll_ret = poll(&pfd, 1, -1);
        }
        if (poll_ret == -1) {
            printf("Error: Poll error\n");
            exit(2);
        }
        if (poll_ret == 0) {
            gl_user_dialog_active = false;
            printf("action is not confirmed within %d seconds proceeding...\n", USER_DIALOG_DURATION_MSEC / 1000);
        }
        if (poll_ret > 0) {
            read(pfd.fd, buf, sizeof(char) * READ_BUFF_FROM_CONSOLE_SIZE);
            // locking write lock
            pthread_rwlock_wrlock(&gl_interrupted_by_user_rwlock);
            if (gl_user_dialog_active && buf[0] == 'y' && buf[1] == '\n') {
                gl_user_dialog_active = false;
                gl_interrupted_by_user = true;
            }
            else if (gl_user_dialog_active && buf[0] == 'n' && buf[1] == '\n') {
                gl_user_dialog_active = false;
                printf("proceeding...\n");
            }
            else if (!gl_user_dialog_active && buf[0] == '\n') {
                gl_user_dialog_active = true;
                printf("Please confirm that computation should be stopped y(es, stop)/n(ot yet)\n");
            }
            else {
                printf("Wrong command\n");
            }
            // unlocking write lock
            pthread_rwlock_unlock(&gl_interrupted_by_user_rwlock);
        }
    }
    return NULL;
}

// asking user to enter x, checking if x is integer and in bound of int, if not - ask again
void query_x(int* x) {
    bool input_not_number = true;
    while (input_not_number) {
        input_not_number = false;
        printf("x = ");
        char buf[READ_BUFF_FROM_CONSOLE_SIZE];
        scanf("%s", buf);
        // atoi return 0 on error, so needed to check 0 input manually
        if (strcmp(buf, "0\0") == 0) {
            *x = 0;
        }
        else {
            *x = atoi(buf);
            if (*x == 0) {
                printf("x must be an integer (type int)\n");
                input_not_number = true;
            }
        }
    }
}

int main() {
    // loop for different x entered by user
    bool user_wants_to_continue = true;
    while (user_wants_to_continue) {
        // variables initialization
        gl_user_dialog_active = false;
        gl_interrupted_by_user = false;
        bool f_computed = false, g_computed = false, fail = false, is_interrupted = false;
        enum _compfunc_status f_status = COMPFUNC_STATUS_MAX, g_status = COMPFUNC_STATUS_MAX;
        unsigned int f_soft_fail_tries = MAX_SOFT_FAIL_TRIES, g_soft_fail_tries = MAX_SOFT_FAIL_TRIES;
        FUNC_RETURN_TYPE f, g;

        // input
        int x;
        query_x(&x);

        // mutex initialization
        pthread_rwlock_init(&gl_interrupted_by_user_rwlock, NULL);

        // creating thread for user interaction (cancellation)
        pthread_t user_interaction_thread;
        if (pthread_create(&user_interaction_thread, NULL, &user_interaction, NULL) != 0) {
            printf("Error: Cannot create a thread\n");
            exit(3);
        }

        // staring processes for computing f and g
        struct ProcessInfo f_process_info = start_process(x, FUNC_F);
        struct ProcessInfo g_process_info = start_process(x, FUNC_G);

        // polling setup
        struct pollfd pfds[2];
        pfds[0].fd = f_process_info.read_fd;
        pfds[1].fd = g_process_info.read_fd;
        for (int i = 0; i < 2; i++) {
            pfds[i].events = POLLIN;
        }

        // polling computation processes with timeout to check if user wished to cancel
        while (!(f_computed && g_computed) && !fail && !is_interrupted_by_user()) {
            int poll_ret = poll(pfds, 2, USER_INTERACTION_LATENCY_MSEC);
            if (poll_ret == -1) {
                printf("Error: Poll error\n");
                exit(2);
            }
            if (poll_ret > 0) {
                // message from f process
                if (pfds[0].revents & POLLIN) {
                    receive(&pfds[0], &f, FUNC_F, f_process_info, x, &f_computed, &fail, &f_status, &f_soft_fail_tries);
                }
                // message from g process
                if (pfds[1].revents & POLLIN) {
                    receive(&pfds[1], &g, FUNC_G, g_process_info, x, &g_computed, &fail, &g_status, &g_soft_fail_tries);
                }
            }
        }

        // terminating processes
        if (kill(f_process_info.id, SIGTERM) != 0) {
            printf("Error: Cannot kill process f\n");
            exit(5);
        }
        if (kill(g_process_info.id, SIGTERM) != 0) {
            printf("Error: Cannot kill process g\n");
            exit(5);
        }

        if ((f_computed && g_computed) || fail) {
            // locking, because thread cannot terminate while this lock is locked
            pthread_rwlock_wrlock(&gl_interrupted_by_user_rwlock);
            if (!gl_interrupted_by_user) {
                if (gl_user_dialog_active) {
                    printf("overriden by system\n");
                }
                // canceling user interaction thread
                if (pthread_cancel(user_interaction_thread) != 0) {
                    printf("Error: Cannot cancel thread\n");
                    exit(4);
                }
                if (pthread_join(user_interaction_thread, NULL) != 0) {
                    printf("Error: Cannot join thread\n");
                    exit(4);
                }
            }
            pthread_rwlock_unlock(&gl_interrupted_by_user_rwlock);

            // printing result
            printf("Result: ");
            if (fail) {
                printf("Fail ( f: ");
                print_func_res_info(f_status, f, f_soft_fail_tries);
                printf(", g: ");
                print_func_res_info(g_status, g, g_soft_fail_tries);
                printf(" )\n\n");
            } else {
                FUNC_RETURN_TYPE result = (f < g) ? f : g;
                printf("%d\n\n", result);
            }
        } else {
            printf("Canceled by user\n");
            printf("f: ");
            print_func_res_info(f_status, f, f_soft_fail_tries);
            printf("\ng: ");
            print_func_res_info(g_status, g, g_soft_fail_tries);
            printf("\n\n");
        }

        // destroying mutex
        pthread_rwlock_destroy(&gl_interrupted_by_user_rwlock);

        // asking user if he wishes to continue
        printf("Do you wish to continue? y(es)/n(o)\n");
        bool user_dialog_active = true;
        while (user_dialog_active) {
            char buf[READ_BUFF_FROM_CONSOLE_SIZE];
            read(0, buf, sizeof(char) * READ_BUFF_FROM_CONSOLE_SIZE);
            if (buf[0] == 'y' && buf[1] == '\n') {
                user_dialog_active = false;
            }
            else if (buf[0] == 'n' && buf[1] == '\n') {
                user_dialog_active = false;
                user_wants_to_continue = false;
            }
            else {
                printf("Wrong command\n");
            }
        }
    }

    return 0;
}
