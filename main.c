#include <stdio.h>
#include <assert.h>

#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_CAP 1024
char buffer[BUFFER_CAP];

int handle(struct pollfd stream)
{
    if (stream.revents) {
        ssize_t n = read(stream.fd, buffer, BUFFER_CAP);
        if (n == -1) {
            assert(errno == EAGAIN);
        } else if (n) {
            fwrite(buffer, n, 1, stdout);
            fflush(stdout);
        } else {
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    int cout[2];
    int cerr[2];
    assert(pipe(cout) != -1);
    assert(pipe(cerr) != -1);

    pid_t pid = fork();
    assert(pid != -1);

    if (pid == 0) {
        close(cout[0]);
        close(cerr[0]);

        assert(dup2(cout[1], STDOUT_FILENO) != -1);
        assert(dup2(cerr[1], STDERR_FILENO) != -1);

        close(cout[1]);
        close(cerr[1]);

        char *argv[] = {"./test", NULL};
        execvp(*argv, argv);

        assert(0 && "unreachable");
    }

    close(cout[1]);
    close(cerr[1]);

    fcntl(cout[0], F_SETFL, fcntl(cout[0], F_GETFL) | O_NONBLOCK);
    fcntl(cerr[0], F_SETFL, fcntl(cerr[0], F_GETFL) | O_NONBLOCK);

    struct pollfd stream[2] = {
        {cout[0], POLLIN, 0},
        {cerr[0], POLLIN, 0},
    };

    while (1) {
        assert(poll(stream, 2, -1) != -1);
        if (handle(stream[0]) == 0) {
            break;
        }

        if (handle(stream[1]) == 0) {
            break;
        }
    }

    return 0;
}
