#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *process_line(const char *line)
{
    CharFrequency charFreq[96];
    int frequencies[128] = {0};

    int count = 0;

    #pragma omp parallel for reduction(+ : frequencies[ : 128])
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] >= 32 && line[i] < 128)
        {
            frequencies[(int)line[i]]++;
        }
    }

    #pragma omp parallel for
    for (int i = 32; i < 128; i++)
    {
        if (frequencies[i] > 0)
        {
            int local_index;

            #pragma omp atomic capture
            local_index = count++;

            charFreq[local_index].ascii = i;
            charFreq[local_index].frequency = frequencies[i];
        }
    }

    qsort(charFreq, count, sizeof(CharFrequency), compare);

    char *output = (char *)malloc(count * 20 + 1);
    if (output == NULL)
    {
        return NULL;
    }
    output[0] = '\0';

    for (int i = 0; i < count; i++)
    {
        char temp[20];
        sprintf(temp, "%d %d\n", charFreq[i].ascii, charFreq[i].frequency);
        strcat(output, temp);
    }

    return output;
}

int main()
{
    char **line = NULL;
    int line_count = 0;

    char buffer[MAX_LINE_LENGTH];

    for (; fgets(buffer, MAX_LINE_LENGTH, stdin) != NULL; line_count++)
    {
        char **temp = realloc(line, (line_count + 1) * sizeof(char *));
        line = temp;
        line[line_count] = strdup(buffer);
    }

    char *output[line_count];

    #pragma omp parallel for
    for (int i = 0; i < line_count; i++)
    {
        output[i] = process_line(line[i]);
        free(line[i]);
    }

    for (int i = 0; i < line_count; i++)
    {
        if (i < line_count - 1)
        {
            printf("%s\n", output[i]);
        }
        else
        {
            printf("%s", output[i]);
        }
        free(output[i]);
    }

    free(line);
    return 0;
}