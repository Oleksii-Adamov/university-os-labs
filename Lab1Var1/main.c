#include <stdio.h>
#include <trialfuncs.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

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

int main() {
    //while (true) {
    for (int x = 2; x < 100; x++) {
        bool f_computed = false, g_computed = false, fail = false, is_interrupted = false;
        TYPENAME_imin f, g;
        printf ("x = %d\n", x);
        //printf ("x = ");
        //int x;
        //scanf("%d", &x);

        // staring processes for computing f and g
        struct ProcessInfo f_process_info = start_process(x, FUNC_F);
        struct ProcessInfo g_process_info = start_process(x, FUNC_G);

        // polling setup
        struct pollfd pfds[3];
        pfds[0].fd = 0; // stdin
        pfds[1].fd = f_process_info.read_fd;
        pfds[2].fd = g_process_info.read_fd;
        for (int i = 0; i < 3; i++) {
            pfds[i].events = POLLIN;
        }

        while (!(f_computed && g_computed)) {
            int poll_ret = poll(pfds, 3, -1);
            if (poll_ret == -1) {
                printf("Error: Poll error\n");
                exit(2);
            }
            if (poll_ret > 0) {
                if (pfds[1].revents & POLLIN) {
                    enum _compfunc_status status;
                    read(pfds[1].fd, &status, sizeof(enum _compfunc_status));
                    read(pfds[1].fd, &f, sizeof(TYPENAME_imin));
                    if (status == COMPFUNC_HARD_FAIL) {
                        printf("f Hard Fail\n");
                        fail = true;
                        break;
                    }
                    else if (status == COMPFUNC_SOFT_FAIL) {
                        printf("f Soft Fail\n");
                        fail = true;
                        break;
                    }
                    else if (status == COMPFUNC_SUCCESS) {
                        printf("f = %d\n", f);
                        f_computed = true;
                    }
                }
                if (pfds[2].revents & POLLIN) {
                    enum _compfunc_status status;
                    read(pfds[2].fd, &status, sizeof(enum _compfunc_status));
                    read(pfds[2].fd, &g, sizeof(int));
                    if (status == COMPFUNC_HARD_FAIL) {
                        printf("g Hard Fail\n");
                        fail = true;
                        break;
                    }
                    else if (status == COMPFUNC_SOFT_FAIL) {
                        printf("g Soft Fail\n");
                        fail = true;
                        break;
                    }
                    else if (status == COMPFUNC_SUCCESS) {
                        printf("g = %d\n", g);
                        g_computed = true;
                    }
                }
                if (pfds[0].revents & POLLIN) { // input from user
                    char buf[256];
                    read(pfds[0].fd, buf, sizeof(char) * 256);
                    if (buf[0] == '\n' && !(f_computed && g_computed)) {
                        printf("Interrupted by user\n");
                        is_interrupted = true;
                        break;
                    }
                }
            }
        }
        if (!is_interrupted) {
            printf("Result: ");
            if (fail) {
                printf("fail\n\n");
            } else {
                unsigned int result = (f < g) ? f : g;
                printf("%d\n\n", result);
            }
        }
    }
    /*printf("Hello world\n");

    compfunc_status_t status;
    double fresult;
    int iresult;
    printf ("f(0) and g(0): \n");
    status = trial_f_imul(0, &iresult);
    printf ("f_imul(0): %s\n", symbolic_status(status));
    if (status == COMPFUNC_SUCCESS)
        printf ("f_imul(0): %d\n", iresult);
    printf ("f(0): %d\n", trial_f_imul(0, &iresult));
    PROCESS_FUNC(f, imul, 0);
    printf ("g(0): %d\n", trial_g_imul(0, &iresult));
    PROCESS_FUNC(g, imul, 0);
    printf ("g(1): %d\n", trial_g_imul(1, &iresult));
    PROCESS_FUNC(g, imul, 1);
    printf ("f(2): %d\n", trial_f_imul(2, &iresult));
    PROCESS_FUNC(f, imul, 2);
    printf ("g(3): %d\n", trial_g_imul(3, &iresult));
    PROCESS_FUNC(g, imul, 3);

    PROCESS_FUNC(g, or, -1);
    PROCESS_FUNC(g, or, 0);
    PROCESS_FUNC(g, imin, 0);*/
    return 0;
}
