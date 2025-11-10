#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cidades.h"

// Função auxiliar para liberar memória da lista encadeada
void liberarEstrada(Estrada *estrada) {
    if (estrada == NULL) return;
    
    Cidade *atual = estrada->Inicio;
    while (atual != NULL) {
        Cidade *temp = atual;
        atual = atual->Proximo;
        free(temp);
    }
    free(estrada);
}

// Função auxiliar para ordenar as cidades por posição (bubble sort)
void ordenarCidades(Cidade **cidades, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (cidades[j]->Posicao > cidades[j + 1]->Posicao) {
                Cidade *temp = cidades[j];
                cidades[j] = cidades[j + 1];
                cidades[j + 1] = temp;
            }
        }
    }
}

// Função auxiliar para calcular a vizinhança de cada cidade
double calcularVizinhancaCidade(int posicao, int posAnterior, int posProxima, int T) {
    double limiteEsquerdo, limiteDireito;
    
    // Calcula limite esquerdo (ponto médio com cidade anterior ou fronteira oeste)
    if (posAnterior == -1) {
        limiteEsquerdo = 0.0;
    } else {
        limiteEsquerdo = (posAnterior + posicao) / 2.0;
    }
    
    // Calcula limite direito (ponto médio com cidade próxima ou fronteira leste)
    if (posProxima == -1) {
        limiteDireito = (double)T;
    } else {
        limiteDireito = (posicao + posProxima) / 2.0;
    }
    
    // Retorna o tamanho da vizinhança
    return limiteDireito - limiteEsquerdo;
}

Estrada *getEstrada(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return NULL;
    }
    
    int T, N;
    
    // Lê comprimento total da estrada
    if (fscanf(arquivo, "%d", &T) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    // Lê número de cidades
    if (fscanf(arquivo, "%d", &N) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    // Valida restrições: 3 ≤ T ≤ 10^6 e 2 ≤ N ≤ 10^4
    if (T < 3 || T > 1000000 || N < 2 || N > 10000) {
        fclose(arquivo);
        return NULL;
    }
    
    // Aloca estrutura Estrada
    Estrada *estrada = (Estrada *)malloc(sizeof(Estrada));
    if (estrada == NULL) {
        fclose(arquivo);
        return NULL;
    }
    
    estrada->T = T;
    estrada->N = N;
    estrada->Inicio = NULL;
    
    Cidade *anterior = NULL;
    int *posicoes = (int *)malloc(N * sizeof(int));
    if (posicoes == NULL) {
        free(estrada);
        fclose(arquivo);
        return NULL;
    }
    
    // Lê as cidades na ordem do arquivo
    for (int i = 0; i < N; i++) {
        Cidade *novaCidade = (Cidade *)malloc(sizeof(Cidade));
        if (novaCidade == NULL) {
            liberarEstrada(estrada);
            free(posicoes);
            fclose(arquivo);
            return NULL;
        }
        
        int posicao;
        char nome[256];
        
        // Lê posição e nome da cidade
        if (fscanf(arquivo, "%d %255[^\n]", &posicao, nome) != 2) {
            free(novaCidade);
            liberarEstrada(estrada);
            free(posicoes);
            fclose(arquivo);
            return NULL;
        }
        
        // Valida restrição: 0 < Xi < T (cidade não pode estar nas fronteiras)
        if (posicao <= 0 || posicao >= T) {
            free(novaCidade);
            liberarEstrada(estrada);
            free(posicoes);
            fclose(arquivo);
            return NULL;
        }
        
        // Valida restrição: Xi ≠ Xj (todas as posições devem ser distintas)
        for (int j = 0; j < i; j++) {
            if (posicoes[j] == posicao) {
                free(novaCidade);
                liberarEstrada(estrada);
                free(posicoes);
                fclose(arquivo);
                return NULL;
            }
        }
        
        // Armazena a posição para validação futura
        posicoes[i] = posicao;
        
        // Preenche os dados da cidade
        novaCidade->Posicao = posicao;
        strcpy(novaCidade->Nome, nome);
        novaCidade->Proximo = NULL;
        
        // Adiciona na lista encadeada (mantém ordem de leitura)
        if (estrada->Inicio == NULL) {
            estrada->Inicio = novaCidade;
        } else {
            anterior->Proximo = novaCidade;
        }
        anterior = novaCidade;
    }
    
    free(posicoes);
    fclose(arquivo);
    return estrada;
}

double calcularMenorVizinhanca(const char *nomeArquivo) {
    // Carrega a estrada do arquivo
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return -1.0;
    }
    
    // Cria array de ponteiros para as cidades
    Cidade **cidades = (Cidade **)malloc(estrada->N * sizeof(Cidade *));
    if (cidades == NULL) {
        liberarEstrada(estrada);
        return -1.0;
    }
    
    // Preenche o array com as cidades da lista encadeada
    Cidade *atual = estrada->Inicio;
    for (int i = 0; i < estrada->N; i++) {
        cidades[i] = atual;
        atual = atual->Proximo;
    }
    
    // Ordena as cidades por posição (necessário para calcular vizinhanças)
    ordenarCidades(cidades, estrada->N);
    
    // Calcula a menor vizinhança entre todas as cidades
    double menorVizinhanca = -1.0;
    
    for (int i = 0; i < estrada->N; i++) {
        // Define posição anterior e próxima (-1 indica fronteira)
        int posAnterior = (i > 0) ? cidades[i - 1]->Posicao : -1;
        int posProxima = (i < estrada->N - 1) ? cidades[i + 1]->Posicao : -1;
        
        // Calcula vizinhança desta cidade
        double vizinhanca = calcularVizinhancaCidade(
            cidades[i]->Posicao, 
            posAnterior, 
            posProxima, 
            estrada->T
        );
        
        // Atualiza a menor vizinhança encontrada
        if (menorVizinhanca < 0 || vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
        }
    }
    
    free(cidades);
    liberarEstrada(estrada);
    
    return menorVizinhanca;
}

char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    // Carrega a estrada do arquivo
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return NULL;
    }
    
    // Cria array de ponteiros para as cidades
    Cidade **cidades = (Cidade **)malloc(estrada->N * sizeof(Cidade *));
    if (cidades == NULL) {
        liberarEstrada(estrada);
        return NULL;
    }
    
    // Preenche o array com as cidades da lista encadeada
    Cidade *atual = estrada->Inicio;
    for (int i = 0; i < estrada->N; i++) {
        cidades[i] = atual;
        atual = atual->Proximo;
    }
    
    // Ordena as cidades por posição
    ordenarCidades(cidades, estrada->N);
    
    // Encontra a cidade com menor vizinhança
    double menorVizinhanca = -1.0;
    int indiceMenor = 0;
    
    for (int i = 0; i < estrada->N; i++) {
        // Define posição anterior e próxima (-1 indica fronteira)
        int posAnterior = (i > 0) ? cidades[i - 1]->Posicao : -1;
        int posProxima = (i < estrada->N - 1) ? cidades[i + 1]->Posicao : -1;
        
        // Calcula vizinhança desta cidade
        double vizinhanca = calcularVizinhancaCidade(
            cidades[i]->Posicao, 
            posAnterior, 
            posProxima, 
            estrada->T
        );
        
        // Atualiza se encontrou uma vizinhança menor
        if (menorVizinhanca < 0 || vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
            indiceMenor = i;
        }
    }
    
    // Aloca memória para o nome da cidade (deve ser liberado pelo chamador)
    char *nomeCidade = (char *)malloc(256 * sizeof(char));
    if (nomeCidade == NULL) {
        free(cidades);
        liberarEstrada(estrada);
        return NULL;
    }
    
    // Copia o nome da cidade com menor vizinhança
    strcpy(nomeCidade, cidades[indiceMenor]->Nome);
    
    free(cidades);
    liberarEstrada(estrada);
    
    return nomeCidade;
}