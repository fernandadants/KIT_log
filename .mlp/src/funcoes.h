#pragma once
#include "Data.h"
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>
#include <fstream>

using namespace std;

// estrutura para criar uma solução TSP
struct Solution
{
    vector<int> sequencia;
    double custo;
};

// Estrutura para armazenar informações importantes de uma subsequência
struct Subsequence{

    // T = Duração
    // W = Custo de atraso
    // C = Custo acumulado
    // first, last = primeiro e último vértice da subsequência

    double T, C;
    int W;
    int first, last;
    inline static Subsequence Concatenate(Subsequence &sigma_1, Subsequence &sigma_2, Data &d){

        Subsequence sigma;
        double temp = d.getDistance(sigma_1.last, sigma_2.first);//t[sigma_1.last][sigma_2.first];
        sigma.W = sigma_1.W + sigma_2.W;
        sigma.T = sigma_1.T + temp + sigma_2.T;
        sigma.C = sigma_1.C + sigma_2.W * (sigma_1.T + temp) + sigma_2.C;
        sigma.first = sigma_1.first;
        sigma.last = sigma_2.last;

        return sigma;
    }

    void mostrar(){
        cout << "F: " << first << " L: " << last << " C: " << C << endl;
    }
};

struct InsertionInfo
{
    int noInserido;     // vértice k que será inserido
    double custo;       // custo ao inserir o vértice 'k' na aresta (i, j)

    // estrutura necessária para fazer a comparação de custo
    inline bool operator<(const InsertionInfo &right)
    {
        return custo < right.custo;
    }
};

vector<InsertionInfo> calcularCustos(int r, vector<int> &CL, Data &d);
void showSolution(Solution &s);
void custoSolucao(Solution &s, Data &d);
double custoSolucao2(vector<int> sequencia, Data &d);
void UpdateAllSubseq(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d);
Solution construcao(Data &data);
bool swap(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data);
bool two_Opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d);
bool or_opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data, int bloco);
void BuscaLocal(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d);
Solution perturbacao(Solution s, vector<vector<Subsequence>> &subseq_matrix, Data &d);
Solution ILS(int maxIter, int maxIterIls, Data &d);