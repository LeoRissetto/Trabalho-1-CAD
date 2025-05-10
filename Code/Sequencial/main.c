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

#define MAX_LINE 1001   // Define o tamanho máximo de uma linha de entrada
#define ASCII_RANGE 128 // Intervalo de caracteres ASCII considerados (0 a 127)

// Struct para armazenar um caractere ASCII e sua frequência de ocorrência
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

int main()
{
    char line[MAX_LINE];            // Vetor para armazenar a linha lida da entrada
    double wtime = omp_get_wtime(); // Inicia a contagem do tempo de execução

    // Arrays estáticos para evitar alocação dinâmica repetida
    static int freq[ASCII_RANGE];    // Frequência dos caracteres ASCII
    static CharFreq cf[ASCII_RANGE]; // Vetor de estruturas para armazenar os caracteres e suas frequências

    // Lê cada linha da entrada padrão
    while (fgets(line, MAX_LINE, stdin))
    {
        // Zera o vetor de frequências
        memset(freq, 0, sizeof(freq));

        // Obtém o comprimento da linha lida
        const int len = strlen(line);

        // Conta as frequências dos caracteres, ignorando quebras de linha
        for (int i = 0; i < len; i++)
        {
            const unsigned char c = line[i];
            if (c != '\n' && c != '\r')
            {
                freq[c]++;
            }
        }

        // Monta o vetor de CharFreq apenas com os caracteres com frequência > 0
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

        // Ordena os caracteres conforme a função de comparação definida
        qsort(cf, count, sizeof(CharFreq), compare);

        // Impressão comentada: imprime o código ASCII e a frequência de cada caractere
        /*for (int i = 0; i < count; i++)
        {
            printf("%d %d\n", cf[i].ascii, cf[i].freq);
        }*/
    }

    // Calcula e imprime o tempo total de execução
    wtime = omp_get_wtime() - wtime;
    printf("Tempo de execução: %f segundos\n", wtime);

    return 0;
}