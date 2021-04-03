#include <stdio.h>

int main() {
    char str[100];
    int i;
    
    printf("Hello, you!\nPlease input your name and age: ");
    scanf("%s %d", str, &i);

    printf("\nYour name is %s, and your age is %d years old.\n", str, i);

    unsigned int *p;
    printf("\nEnter a number: %d", str, );

    &p = 9;
    printf("Address: %d, Value: %p, Pointer: %d\n", &p, p, *p);

    return 0;
}