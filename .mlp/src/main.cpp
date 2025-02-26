#include "Data.h"
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

vector<InsertionInfo> calcularCustos(int r, vector<int> &CL, Data &d)
{

    // o tamanho do vetor de custoInsercao será do tamanho da lista de candidados
    const int tam =  CL.size();

    vector<InsertionInfo> custoInsercao(tam);

    // inserindo todos os vertices da lista de candidados e calculando custo
    for (int i = 0; i < tam; i++){
        custoInsercao[i].custo = d.getDistance(r, CL[i]);
        custoInsercao[i].noInserido = CL[i];
    }
    
    return custoInsercao;
}

void showSolution(Solution &s)
{
    for (int i = 0; i < s.sequencia.size(); i++)
    {
        cout << s.sequencia[i];
        // cout << s.sequencia.back() << endl;
        if (i != s.sequencia.size() - 1)
        {
            cout << " > ";
        }
    }
    cout << endl;
}

void custoSolucao(Solution &s, Data &d)
{
    double custo = 0;
    double acumulado = 0;

    for (int i = 0; i < s.sequencia.size() - 1; i++)
    {
        acumulado += d.getDistance(s.sequencia[i], s.sequencia[i + 1]);
        custo += acumulado;
    }

    s.custo = custo;
}

void UpdateAllSubseq(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d){
    // s = solução corrente
    // subseq_matrix = vector<vector<Subsequence>>(n, vector<Subsequence>(n));

    // n = numero de nós da instância
    int n = s.sequencia.size();

    // Inicializando a matriz de subsequências
    for (int i = 0; i < n; i++){
        int v = s.sequencia[i];
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequencia[i];
        subseq_matrix[i][i].last = s.sequencia[i];
    }

    for (int i = 0; i < n; i++){
        for (int j = i + 1; j < n ; j++){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j], d);
            
        }
    }
    
    // Subsequencias invertidas
    // (necessárias para o 2-opt)
    for (int i = n - 1; i >= 0; i--){
        for (int j = i - 1; j >= 0; j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j], d);
        }
    }
            
}

Solution construcao(Data &data)
{
    int tam = data.getDimension() - 1;

    // escrevendo lista de candidatos e começando com tam-1 devido a solução já começar com o 1.
    vector<int> CL(tam);

    // inicializando valores da lista de candidatos e começando a partir do 2
    for (int i = 0, k = 2; i < tam; i++, k++)
    {
        CL[i] = k;
    }

    // iniciando processo de formação da solução
    Solution s;
    s.sequencia = {1};

    // definindo custo inicial como 0
    s.custo = 0;

    int r = 1;

    while (!CL.empty())
    {
        // criando vetor que irá calcular o custo de inserção do novo nó
        vector<InsertionInfo> custoInsercao = calcularCustos(r, CL, data);

        // ordenando em ordem crescente
        sort(custoInsercao.begin(), custoInsercao.end());

        // escolhendo um numero aleatório
        double alpha = 0.25;

        int indice_alpha = ceil(alpha * (custoInsercao.size()));

        // selecionando um vertice aleatorio no vetor, do inicio até o numero alpha
        unsigned int selecionado = rand() % indice_alpha;

        int no_selecionado = custoInsercao[selecionado].noInserido;

        // inserindo nó na solução
        s.sequencia.push_back(no_selecionado);

        // Atribuindo o nó selecionado ao r
        r = no_selecionado;

        auto indice_selecionado = find(CL.begin(), CL.end(), no_selecionado);

        // deletando nó da lista de candidados
        CL.erase(indice_selecionado);
    }

    // terminando sequencia com 1 novamente
    s.sequencia.push_back(1);

    custoSolucao(s, data);

    return s;
}

// Estrutura de vizinhança Two-Opt
bool two_Opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d)
{   
    int best_i, best_j;
    int n = subseq_matrix.size();
    bool best_delta = false;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 2; ++i)
    {
        for (int j = i + 1; j < s.sequencia.size() - 1; ++j)
        {
            //cout << i << " " << j << " ";
            //subseq_matrix[0][i-1].mostrar();
            //subseq_matrix[j][i].mostrar();

            Subsequence sigma_1 = Subsequence:: Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i], d);
            Subsequence sigma_2 = Subsequence:: Concatenate(sigma_1, subseq_matrix[j+1][n-1], d);

            //sigma_2.mostrar();


            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (sigma_2.C < s.custo)
            {
                best_delta = true;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (best_delta)
    {
        reverse(s.sequencia.begin() + best_i, s.sequencia.begin() + best_j + 1);
        UpdateAllSubseq(s, subseq_matrix, d);
        return true;
    }

    return false;
}

int main(int argc, char **argv)
{
    auto data = Data(argc, argv[1]);
    data.read();
    srand(time(NULL));

    int n = data.getDimension() + 1;

    Solution s = construcao(data);
    vector<vector<Subsequence>> subseq_matrix(n, vector<Subsequence>(n));
    showSolution(s);
    cout << s.custo << endl;

    UpdateAllSubseq(s, subseq_matrix, data);

    two_Opt(s, subseq_matrix, data);


    return 0;
}