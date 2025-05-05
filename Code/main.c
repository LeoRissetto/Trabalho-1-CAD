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
#define MAX_CHARS 96

typedef struct
{
    int ascii;
    int frequency;
} CharFrequency;

void merge(CharFrequency arr[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    CharFrequency *L = (CharFrequency *)malloc(n1 * sizeof(CharFrequency));
    CharFrequency *R = (CharFrequency *)malloc(n2 * sizeof(CharFrequency));

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2)
    {
        if (L[i].frequency < R[j].frequency ||
            (L[i].frequency == R[j].frequency && L[i].ascii < R[j].ascii))
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void parallel_merge_sort(CharFrequency arr[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        #pragma omp task if (right - left > 1000)
        parallel_merge_sort(arr, left, mid);

        #pragma omp task if (right - left > 1000)
        parallel_merge_sort(arr, mid + 1, right);

        #pragma omp taskwait
        merge(arr, left, mid, right);
    }
}

char *process_line(const char *line)
{
    int frequencies[128] = {0};
    int length = strlen(line);
    CharFrequency charFreq[MAX_CHARS];
    int count = 0;

    #pragma omp parallel for reduction(+ : frequencies[ : 128]) schedule(static)
    for (int i = 0; i < length; i++)
    {
        if (line[i] >= 32 && line[i] < 128)
        {
            frequencies[(int)line[i]]++;
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

    #pragma omp parallel
    {
        #pragma omp single
        parallel_merge_sort(charFreq, 0, count - 1);
    }

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

    omp_set_num_threads(omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp single
        {
            while (fgets(buffer, MAX_LINE_LENGTH, stdin))
            {
                buffer[strcspn(buffer, "\n")] = '\0';

                if (buffer[0] == '\0')
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
        }
    }

    for (int i = 0; i < num_lines; i++)
    {
        if (i)
            printf("\n");
        printf("%s", output_lines[i]);
        free(output_lines[i]);
    }

    free(output_lines);
    return 0;
}