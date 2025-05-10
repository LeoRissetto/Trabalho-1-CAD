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

#define MAX_LINE 1001   // Tamanho máximo de cada linha de entrada
#define ASCII_RANGE 128 // Quantidade de caracteres ASCII padrão

// Estrutura que representa um caractere e sua frequência
typedef struct
{
    unsigned char ascii; // Código ASCII do caractere
    int freq;            // Frequência de ocorrência
} CharFreq;

// Função de comparação usada para ordenação:
// Ordena primeiro por frequência crescente, depois por código ASCII crescente
int compare(const void *a, const void *b)
{
    const CharFreq *cf1 = (const CharFreq *)a;
    const CharFreq *cf2 = (const CharFreq *)b;

    return cf1->freq != cf2->freq ? cf1->freq - cf2->freq : cf1->ascii - cf2->ascii;
}

// Função que processa uma linha e gera a saída formatada com frequências
void process_line(const char *line, char *output)
{
    int freq[ASCII_RANGE] = {0}; // Frequência de cada caractere ASCII
    CharFreq cf[ASCII_RANGE];    // Vetor com caractere e frequência

    int len = strlen(line); // Calcula o comprimento da linha

    // Conta as frequências, ignorando quebras de linha
    for (int i = 0; i < len; i++)
    {
        char c = line[i];
        if (c != '\n' && c != '\r')
        {
            freq[c]++;
        }
    }

    // Preenche cf[] apenas com caracteres que aparecem
    int count = 0;
    for (int i = 0; i < ASCII_RANGE; i++)
    {
        if (freq[i] > 0)
        {
            cf[count].ascii = i;
            cf[count].freq = freq[i];
            count++;
        }
    }

    // Ordena os elementos com base na frequência e código ASCII
    qsort(cf, count, sizeof(CharFreq), compare);

    // Aloca espaço para a string de saída
    output = malloc(count * 7 + 1); // Estimativa: até 7 bytes por entrada (char + espaço + freq + \n)
    int pos = 0;

    // Formata a saída na string alocada
    for (int i = 0; i < count; i++)
    {
        pos += sprintf(output + pos, "%c %d\n", cf[i].ascii, cf[i].freq);
    }
    output[pos] = '\0'; // Finaliza a string
}

int main()
{
    char line[MAX_LINE]; // Buffer para a linha de entrada
    int num_lines = 0;   // Contador de linhas processadas
    int capacity = 1000; // Capacidade inicial do vetor de resultados

    char **output_lines = malloc(sizeof(char *) * capacity); // Vetor para armazenar saídas de cada linha

    double start_time = omp_get_wtime(); // Marca o início da execução

    // Início da região paralela com número máximo de threads
    #pragma omp parallel num_threads(omp_get_max_threads())
    {
        #pragma omp single
        {
            // Leitura sequencial das linhas
            for (int num_lines = 0; fgets(line, MAX_LINE, stdin); num_lines++)
            {
                size_t len = strcspn(line, "\n"); // Remove a quebra de linha
                line[len] = '\0';

                if (len == 0)
                    continue; // Ignora linhas vazias

                // Cria uma nova tarefa para processar a linha
                #pragma omp task firstprivate(line, num_lines)
                {
                    /* Código comentado que deveria realocar o vetor se necessário
                    #pragma omp critical
                    if (num_lines == capacity)
                    {
                        capacity *= 2;
                        output_lines = realloc(output_lines, sizeof(char *) * capacity);
                    }*/

                    // Chama a função que processa a linha e armazena a saída
                    process_line(line, output_lines[num_lines]);
                }
            }
        }
    }

    // Impressão e liberação de memória (comentada)
    for (int i = 0; i < num_lines; i++)
    {
        /*
        if (i)
            printf("\n");
        printf("%s", output_lines[i]);
        */
        free(output_lines[i]); // Libera memória da linha processada
    }

    double end_time = omp_get_wtime(); // Marca o fim da execução
    printf("\nTempo de execução: %f segundos\n", end_time - start_time);

    free(output_lines); // Libera o vetor de saída
    return 0;
}
