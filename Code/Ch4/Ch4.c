#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int Figure2_3(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    Figure2_3(argc, argv);
}

int Figure2_3(int argc, char *argv[])
{
    int* p = (int*)malloc(sizeof(int));
    assert(p != NULL);
    printf("(%d) address pointed to by p: %p\n", getpid(), p);
    *p = 0;

    int counter = 10;
    while (counter > 0)
    {
        *p = *p + 1;
        printf("(%d) p: %d\n", getpid(), *p);
        --counter;
    }
    return 0;
}
