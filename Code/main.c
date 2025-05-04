/* Membros: 
    - Caio Oliveira Godinho - NUSP: 
    - Karine Cerqueira Nascimento - NUSP: 13718404
    - Leonardo Gueno Rissetto - NUSP: 13676482
    - Thiago Kashivagui Gonçalves - NUSP: 13676579
*/

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
    return char1->frequency != char2->frequency ? char1->frequency - char2->frequency : char1->ascii - char2->ascii;
}

char *process_line(const char *line)
{
    CharFrequency charFreq[96];
    int frequencies[128] = {0};
    int count = 0;
    int length = strlen(line);

    #pragma omp parallel reduction(+:frequencies[:128])
    {
        #pragma omp for schedule(static)
        for (int i = 0; i < length; i++)
        {
            if (line[i] >= 32 && line[i] < 128)
            {
                frequencies[(int)line[i]]++;
            }
        }
    }

    for (int i = 32; i < 128; i++)
    {
        if (frequencies[i] > 0)
        {
            charFreq[count].ascii = i;
            charFreq[count].frequency = frequencies[i];
            count++;
        }
    }

    qsort(charFreq, count, sizeof(CharFrequency), compare);

    char *buffer = (char *)malloc(sizeof(char) * (count * 10 + 1));
    buffer[0] = '\0';
    
    for (int i = 0; i < count; i++)
    {
        char temp[10];
        sprintf(temp, "%d %d\n", charFreq[i].ascii, charFreq[i].frequency);
        strcat(buffer, temp);
    }

    return buffer;
}

int main()
{
    char buffer[MAX_LINE_LENGTH];
    int num_lines = 0;

    int capacity = 1000;
    char **output_lines = malloc(sizeof(char *) * capacity);

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
                    free(line);

                    #pragma omp critical
                    {
                        if (current_index >= capacity)
                        {
                            capacity *= 2;
                            output_lines = realloc(output_lines, capacity * sizeof(char *));
                        }
                        output_lines[current_index] = output;
                    }
                }
            }
            #pragma omp taskwait
        }
    }

    for (int i = 0; i < num_lines; i++)
    {
        printf("%s", output_lines[i]);
        if (i != num_lines - 1)
            printf("\n");
        free(output_lines[i]);
    }

    free(output_lines);
    return 0;
}