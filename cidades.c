#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cidades.h"

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

double calcularVizinhancaCidade(int posicao, int posAnterior, int posProxima, int T) {
    double limiteEsquerdo, limiteDireito;
    
    if (posAnterior == -1) {
        limiteEsquerdo = 0.0;
    } else {
        limiteEsquerdo = (posAnterior + posicao) / 2.0;
    }
    
    if (posProxima == -1) {
        limiteDireito = (double)T;
    } else {
        limiteDireito = (posicao + posProxima) / 2.0;
    }
    
    return limiteDireito - limiteEsquerdo;
}

Estrada *getEstrada(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return NULL;
    }
    
    int T, N;
    
    if (fscanf(arquivo, "%d", &T) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    if (fscanf(arquivo, "%d", &N) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    if (T < 3 || T > 1000000 || N < 2 || N > 10000) {
        fclose(arquivo);
        return NULL;
    }
    
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
    
    if (fscanf(arquivo, " %d ", &posicao) != 1) {
        free(novaCidade);
        liberarEstrada(estrada);
        free(posicoes);
        fclose(arquivo);
        return NULL;
    }
  
    if (fgets(nome, 256, arquivo) == NULL) {
        free(novaCidade);
        liberarEstrada(estrada);
        free(posicoes);
        fclose(arquivo);
        return NULL;
    }
    
    nome[strcspn(nome, "\n")] = '\0';
    
    if (posicao <= 0 || posicao >= T) {
        free(novaCidade);
        liberarEstrada(estrada);
        free(posicoes);
        fclose(arquivo);
        return NULL;
    }
   
        for (int j = 0; j < i; j++) {
            if (posicoes[j] == posicao) {
                free(novaCidade);
                liberarEstrada(estrada);
                free(posicoes);
                fclose(arquivo);
                return NULL;
            }
        }
        
        posicoes[i] = posicao;
        
        novaCidade->Posicao = posicao;
        strcpy(novaCidade->Nome, nome);
        novaCidade->Proximo = NULL;
        
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
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return -1.0;
    }
    
    Cidade **cidades = (Cidade **)malloc(estrada->N * sizeof(Cidade *));
    if (cidades == NULL) {
        liberarEstrada(estrada);
        return -1.0;
    }
    
    Cidade *atual = estrada->Inicio;
    for (int i = 0; i < estrada->N; i++) {
        cidades[i] = atual;
        atual = atual->Proximo;
    }
    
    ordenarCidades(cidades, estrada->N);
    
    double menorVizinhanca = -1.0;
    
    for (int i = 0; i < estrada->N; i++) {
        int posAnterior = (i > 0) ? cidades[i - 1]->Posicao : -1;
        int posProxima = (i < estrada->N - 1) ? cidades[i + 1]->Posicao : -1;
        
        double vizinhanca = calcularVizinhancaCidade(
            cidades[i]->Posicao, 
            posAnterior, 
            posProxima, 
            estrada->T
        );
        
        if (menorVizinhanca < 0 || vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
        }
    }
    
    free(cidades);
    liberarEstrada(estrada);
    
    return menorVizinhanca;
}

char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return NULL;
    }
    
    Cidade **cidades = (Cidade **)malloc(estrada->N * sizeof(Cidade *));
    if (cidades == NULL) {
        liberarEstrada(estrada);
        return NULL;
    }
    
    Cidade *atual = estrada->Inicio;
    for (int i = 0; i < estrada->N; i++) {
        cidades[i] = atual;
        atual = atual->Proximo;
    }
    
    ordenarCidades(cidades, estrada->N);
    
    double menorVizinhanca = -1.0;
    int indiceMenor = 0;
    
    for (int i = 0; i < estrada->N; i++) {
        int posAnterior = (i > 0) ? cidades[i - 1]->Posicao : -1;
        int posProxima = (i < estrada->N - 1) ? cidades[i + 1]->Posicao : -1;
        
        double vizinhanca = calcularVizinhancaCidade(
            cidades[i]->Posicao, 
            posAnterior, 
            posProxima, 
            estrada->T
        );
        
        if (menorVizinhanca < 0 || vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
            indiceMenor = i;
        }
    }
    
    char *nomeCidade = (char *)malloc(256 * sizeof(char));
    if (nomeCidade == NULL) {
        free(cidades);
        liberarEstrada(estrada);
        return NULL;
    }
    
    strcpy(nomeCidade, cidades[indiceMenor]->Nome);
    
    free(cidades);
    liberarEstrada(estrada);
    
    return nomeCidade;
}