#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() {

    int this_is_a_number = 0;
    char this_is_a_string[14];
    double this_is_a_double;

    this_is_a_number = 123;
    strcpy(this_is_a_string, "I am a string");
    this_is_a_double = 123.99;

    bool aBoolean = true;

    enum primaryColour {red, yellow, blue};
    

    return 0;
}