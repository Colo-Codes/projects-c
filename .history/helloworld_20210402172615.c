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

    // Definition
    enum primaryColour {red, yellow, blue};
    // Declaration
    enum primaryColour myColour;
    // Assignment
    primaryColour = red;
    

    return 0;
}