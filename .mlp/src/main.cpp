#include "Data.h"
#include "main.cpp"
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>
#include <chrono>
#include <fstream>

using namespace std;

// estrutura para criar uma solução TSP
struct Solution
{
    vector<int> sequencia;
    double valorObj;
    double custo;
};

// Estrutura para armazenar informações importantes de uma subsequência

struct Subsequence{
    double T, C;
    int W;
    int first, last;
    inline static Subsequence Concatenate(Subsequence &sigma_1, Subsequence &sigma_2){

        Subsequence sigma;
        double temp = t[sigma_1.last][sigma_2.first];
        sigma.W = sigma_1.W + sigma_2.W;
        sigma.T = sigma_1.T + temp + sigma_2.T;
        sigma.C = sigma_1.C + sigma_2.W * (sigma_1.T + temp) + sigma_2.C;
        sigma.first = sigma_1.first;
        sigma.last = sigma_2.last;

        return sigma;
    }
};

void UpdateAllSubseq(Solution &s, vector<vector<Subsequence>> &subseq_matrix){
    // s = solução corrente
    //subseq_matrix = vector<vector<Subsequence>>(n, vector<Subsequence>(n));

    // n = numero de nós da instância
    int n = s.sequencia.size();

    for (int i = 0; i < n; i++){
        int v = s.sequencia[i];
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequencia[i];
        subseq_matrix[i][i].last = s.sequencia[i];
    }

    for (int i = 0; i < n; i++){
        for (int j = i + 1; j < n; j++){ 
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[i][j]);
        }
    }
    
    // Subsequencias invertidas
    // (necessárias para o 2-opt)
    for (int n -1; i >= 0; i--){
        for (int j = i - 1; j >= 0; j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[i][j]);
        }
    }
            
}
