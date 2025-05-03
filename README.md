# Analisador de Frequência de Caracteres

Um programa em C que analisa texto de entrada para calcular e ordenar frequências de caracteres usando processamento paralelo com OpenMP.

## Membros

- Leonardo Gueno Rissetto - **NUSP: 13676482**
- Thiago Kashivagui Gonçalves - **NUSP: 13676579**
- Karine Cerqueira Nascimento - **NUSP: 13718404**
- Caio Oliveira Godinho - **NUSP:**

## Descrição

Este programa lê texto de entrada do stdin e para cada linha:
1. Conta a frequência de cada caractere ASCII imprimível (32-127)
2. Ordena os caracteres por frequência (crescente) e valor ASCII (decrescente)
3. Gera a saída no formato: `VALOR_ASCII FREQUÊNCIA`

O programa utiliza OpenMP para processamento paralelo, melhorando o desempenho em sistemas multi-core.

## Características

- Processamento paralelo usando OpenMP
- Gerenciamento eficiente de memória
- Processa múltiplas linhas de entrada
- Ordena caracteres por frequência e valor ASCII
- Suporta todos os caracteres ASCII imprimíveis (32-127)

## Requisitos

- Compilador C com suporte a OpenMP (ex: GCC)
- Make (opcional, para compilação)

## Compilação

Para compilar o programa, use:

```bash
gcc -fopenmp main.c -o main
```

## Como Usar

1. Compile o programa como mostrado acima
2. Execute o programa e forneça a entrada através do stdin:

```bash
./main < input.txt
```

Ou forneça a entrada diretamente:

```bash
echo "Olá Mundo" | ./main
```

## Formato de Entrada

O programa aceita qualquer texto de entrada através do stdin. Cada linha será processada independentemente.

## Formato de Saída

Para cada linha de entrada, o programa gera:
- Uma linha por caractere único
- Cada linha contém: `VALOR_ASCII FREQUÊNCIA`
- Os caracteres são ordenados por frequência (crescente) e valor ASCII (decrescente)

Exemplo de saída:
```
120 1
117 1
115 1
111 1
100 1
97 1
116 2
99 2
32 2
101 5

118 1
117 1
108 1
99 1
32 1
111 3

122 1
118 1
116 1
115 1
110 1
105 1
102 1
100 1
99 1
63 1
117 2
113 2
111 3
97 3
101 4
32 6
```
