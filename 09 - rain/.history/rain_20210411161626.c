/**********************
 * By Damian Demasi
 * Date 11/04/2021
 * Requirements:
 *      Create a program with a two-dimensional array with monthly rain values for YEARS years.
 *      The output should be:
 *          - Total rainfall per year.
 *          - Average rain for all the years.
 *          - Average rain per month.
 *      Use floats.
 **********************/

#include <stdio.h>

#define MONTHS 12
#define YEARS 5

int main ()
{
    unsigned int i, j;
    float sum = 0;
    float rainData[YEARS][MONTHS] = {
        {55, 67, 78, 89, 90, 54, 32, 21, 10, 5, 23, 45}, // Year 1
        {56, 68, 79, 90, 91, 55, 33, 22, 11, 6, 24, 46}, // Year 2
        {57, 69, 80, 91, 92, 56, 34, 23, 12, 7, 25, 47}, // Year 3
        {58, 70, 81, 92, 93, 57, 35, 24, 13, 8, 26, 48}, // Year 4
        {59, 71, 82, 93, 94, 58, 36, 25, 14, 9, 27, 49}  // Year 5
    };
    float yearTotal[YEARS] = {0};
    float monthAverage[MONTHS] = {0};

    // Yearly rain
    for (i = 0; i < YEARS; i++)
    {
        for (j = 0; j < MONTHS; j++)
        {
            yearTotal[i] += rainData[i][j];
        }
    }

    // Monthly rain
    for (i = 0; i < MONTHS; i++)
    {
        for (j = 0; j < YEARS; j++)
        {
            monthAverage[i] += rainData[j][i];
        }
        monthAverage[i] = monthAverage[i] / YEARS;
    }
    
    // Yearly total
    printf("Total yearly rain:\n");
    for (i = 0; i < YEARS; i++)
    {
        printf("Year %d     %.2f\n", i, yearTotal[i]);
        
    }
    
    // Annual average
    printf("\nAnnual rain average:\n");
    for (i = 0; i < YEARS; i++)
    {
        sum += yearTotal[i];
    }
    printf("%.2f\n", sum / YEARS);

    // Monthly average:
    printf("\nMonthly rain average:\n");
    printf("Jan\tFeb\tMar\tApr\tMay\tJun\tJul\tAug\tSep\tOct\tNov\tDec\t\n");
    for (i = 0; i < MONTHS; i++)
    {
        printf("%.2f\t", monthAverage[i]);
    }
    printf("\n");

    return 0;
}