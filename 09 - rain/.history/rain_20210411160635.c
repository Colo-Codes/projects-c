/**********************
 * By Damian Demasi
 * Date 11/04/2021
 * Requirements:
 *      Create a program with a two-dimensional array with monthly rain values for 5 years.
 *      The output should be:
 *          - Total rainfall per year.
 *          - Average rain for all the years.
 *          - Average rain per month.
 *      Use floats.
 **********************/

#include <stdio.h>

int main ()
{
    unsigned int i, j;
    unsigned int sum = 0;
    float rainData[5][12] = {
        {55, 67, 78, 89, 90, 54, 32, 21, 10, 5, 23, 45}, // Year 1
        {56, 68, 79, 90, 91, 55, 33, 22, 11, 6, 24, 46}, // Year 2
        {57, 69, 80, 91, 92, 56, 34, 23, 12, 7, 25, 47}, // Year 3
        {58, 70, 81, 92, 93, 57, 35, 24, 13, 8, 26, 48}, // Year 4
        {59, 71, 82, 93, 94, 58, 36, 25, 14, 9, 27, 49}  // Year 5
    };
    float yearTotal[5] = {0};
    float monthAverage[12] = {0};

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 12; j++)
        {
            yearTotal[i] += rainData[i][j];
        }
        



    }
    
    // Yearly total
    printf("Total yearly rain:\n");
    for (i = 0; i < 5; i++)
    {
        printf("Year %d     %.2f\n", i, yearTotal[i]);
        
    }
    
    // Annual average
    printf("\nAnnual rain average:\n");
    for (i = 0; i < 5; i++)
    {
        sum += yearTotal[i];
    }
    printf("%.2f\n", sum / 5);



    return 0;
}