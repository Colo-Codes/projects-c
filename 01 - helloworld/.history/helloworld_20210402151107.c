#include <stdio.h>

int main() {
    char str[100];
    int i;
    
    printf("Hello, you!\nPlease input your name and age: ");
    scanf("%s %d", str, &i);

    printf("\nYour name is %s, and your age is %i years old.\n0", str, i);
    return 0;
}