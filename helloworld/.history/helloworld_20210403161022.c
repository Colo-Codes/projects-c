#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i;

    printf("The number of arguments is: %d\n", argc);
    for (i = 0; i < strlen(argv); i++)
    {
        printf("This is an argument: %s\n", argv[i]);
    }

    return 0;
}