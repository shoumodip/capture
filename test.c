#include <stdio.h>

int main(void)
{
    printf("STDOUT 1\n");
    fprintf(stderr, "STDERR 1\n");
    printf("STDOUT 2\n");
    fprintf(stderr, "STDERR 2\n");
    return 0;
}
