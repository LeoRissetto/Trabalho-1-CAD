#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_LINE_LENGTH 1000

typedef struct
{
    int ascii;
    int frequency;
} CharFrequency;

int compare(const void *a, const void *b)
{
    CharFrequency *char1 = (CharFrequency *)a;
    CharFrequency *char2 = (CharFrequency *)b;
    return char1->frequency != char2->frequency ? char1->frequency - char2->frequency : char2->ascii - char1->ascii;
}

int main()
{
    char line[MAX_LINE_LENGTH + 1];
    int frequencies[128] = {0};
    CharFrequency result[96];
    int result_count = 0;
    int pos;

    while (fgets(line, MAX_LINE_LENGTH + 1, stdin) != NULL)
    {
        #pragma omp parallel for
        for (int i = 0; i < 128; i++)
        {
            frequencies[i] = 0;
        }
        result_count = 0;

        #pragma omp parallel for reduction(+ : frequencies[ : 128])
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (line[i] >= 32 && line[i] < 128)
            {
                frequencies[(int)line[i]]++;
            }
        }

        #pragma omp parallel for reduction(+:result_count)
        for (int i = 32; i < 128; i++)
        {
            if (frequencies[i] > 0)
            {
                #pragma omp atomic capture
                pos = result_count++;
                result[pos].ascii = i;
                result[pos].frequency = frequencies[i];
            }
        }

        qsort(result, result_count, sizeof(CharFrequency), compare);

        #pragma omp parallel for
        for (int i = 0; i < result_count; i++)
        {
            printf("%d %d\n", result[i].ascii, result[i].frequency);
        }
        printf("\n");
    }

    return 0;
}