#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if(argc<=1) {
        printf("You did not feed me arguments, I will die now :( ...");
        exit(1);
    }  //otherwise continue on our merry way....
    int arg1 = atoi(argv[1]);  //argv[0] is the program name
                            //atoi = ascii to int
    printf("Test: %d\n", arg1);

    return 0;
}
