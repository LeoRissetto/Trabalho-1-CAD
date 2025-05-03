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

char *process_line(const char *line)
{
    CharFrequency charFreq[96];
    int frequencies[128] = {0};

    int count = 0;
    int length = strlen(line);

    #pragma omp parallel for reduction(+ : frequencies[ : 128])
    for (int i = 0; i < length; i++)
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

    char *output = (char *)malloc(count * 9 + 1);
    if (output == NULL)
    {
        return NULL;
    }
    output[0] = '\0';

    for (int i = 0; i < count; i++)
    {
        char temp[10];
        sprintf(temp, "%d %d\n", charFreq[i].ascii, charFreq[i].frequency);
        strcat(output, temp);
    }

    return output;
}

int main()
{
    char buffer[MAX_LINE_LENGTH];
    char **output_lines = NULL;
    int num_lines = 0;

    int aux = 1;
    int size = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            while (fgets(buffer, MAX_LINE_LENGTH, stdin) != NULL)
            {
                buffer[strcspn(buffer, "\n")] = '\0';

                if(buffer[0] == '\0')
                {
                    continue;
                }

                char *line = strdup(buffer);
                int current_index = num_lines++;

                #pragma omp task firstprivate(line, current_index)
                {
                    char *output = process_line(line);

                    #pragma omp critical
                    {
                        if(aux)
                        {
                            size++;
                            output_lines = realloc(output_lines, size * sizeof(char *));
                            aux = 0;
                        }
                        output_lines[current_index] = output;
                    }

                    free(line);
                }
            }
            #pragma omp taskwait
        }
    }

    for (int i = 0; i < num_lines; i++)
    {
        if (i != num_lines - 1)
            printf("%s\n", output_lines[i]);
        else
            printf("%s", output_lines[i]);
        free(output_lines[i]);
    }

    free(output_lines);
    return 0;
}