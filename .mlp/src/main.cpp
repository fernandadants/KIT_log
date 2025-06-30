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

double custoSolucao2(vector<int> sequencia, Data &d)
{
    double custo = 0;
    double acumulado = 0;

    for (int i = 0; i < sequencia.size() - 1; i++)
    {
        acumulado += d.getDistance(sequencia[i], sequencia[i + 1]);
        custo += acumulado;
    }

    return custo;
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

bool swap(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d)
{

    // Definindo o melhor delta até o momento;
    int best_i, best_j;
    double best_custo = s.custo;
    int n = subseq_matrix.size()-1;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 1; i++)
    {

        for (int j = i + 1; j < s.sequencia.size() - 1; j++)
        {
            
            vector<int> seq = s.sequencia;

            Subsequence sigma_1 = Subsequence:: Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j], d);
            Subsequence sigma_2;

            if(i+1 != j)
                sigma_2 = Subsequence:: Concatenate(sigma_1, subseq_matrix[i+1][j-1], d);
            else
                sigma_2 = sigma_1;
            
            Subsequence sigma_3 = Subsequence:: Concatenate(sigma_2, subseq_matrix[i][i], d);
            Subsequence sigma_4 = Subsequence:: Concatenate(sigma_3, subseq_matrix[j+1][n], d);

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (sigma_4.C < best_custo)
            {
                best_custo = sigma_4.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (best_custo < s.custo)
    {
        swap(s.sequencia[best_i], s.sequencia[best_j]);
        s.custo = best_custo;
        UpdateAllSubseq(s, subseq_matrix, d);
        return true;
    }

    return false;
}

// Estrutura de vizinhança Two-Opt
bool two_Opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d)
{   
    int best_i, best_j;
    double best_custo = s.custo;
    int n = subseq_matrix.size()-1;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 2; ++i)
    {
        for (int j = i + 1; j < s.sequencia.size() - 1; ++j)
        {

            Subsequence sigma_1 = Subsequence:: Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i], d);
            Subsequence sigma_2 = Subsequence:: Concatenate(sigma_1, subseq_matrix[j+1][n], d);

            if (sigma_2.C < best_custo)
            {
                best_custo = sigma_2.C;
                best_i = i;
                best_j = j;
            }
        }
    }
    // Se o melhor delta for menor que 0, aderir à troca.
    if (best_custo < s.custo)
    {
        reverse(s.sequencia.begin() + best_i, s.sequencia.begin() + best_j + 1);
        s.custo = best_custo;
        UpdateAllSubseq(s, subseq_matrix, d);
        return true;
    }

    return false;
}

bool or_opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d, int bloco)
{

    // Definindo o melhor custo até o momento;
    int best_i, best_j;
    double best_custo = s.custo;
    int n = subseq_matrix.size()-1;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - bloco; i++)
    {

        // Um exemplo de bloco de 3 elementos:
        // Sequencia: {1, 2, 3, 4, 5, 6, 1}
        // Bloco:        {2, 3, 4}
        // Vértices: {vi, middle, vi_aux}

        for (int j = 1; j < s.sequencia.size()-1; j++)
        {
            if (j == i){
                j += bloco-1;
                continue;
            }

            Subsequence sigma;

            if(j>i){
                Subsequence a = subseq_matrix[0][i-1];
                Subsequence b = subseq_matrix[i+bloco][j];
                Subsequence c = subseq_matrix[i][i+bloco-1];
                Subsequence z = subseq_matrix[j+1][n];

                sigma = Subsequence:: Concatenate(a, b, d);
                sigma = Subsequence:: Concatenate(sigma, c, d);
                sigma = Subsequence:: Concatenate(sigma, z, d);
            }else{

                Subsequence a = subseq_matrix[0][j-1];
                Subsequence b = subseq_matrix[i][i+bloco-1];
                Subsequence c = subseq_matrix[j][i-1];
                Subsequence z = subseq_matrix[i+bloco][n];

                sigma = Subsequence:: Concatenate(a, b, d);
                sigma = Subsequence:: Concatenate(sigma, c, d);
                sigma = Subsequence:: Concatenate(sigma, z, d);
            }

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (sigma.C < best_custo)
            {
                best_custo = sigma.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor melhor custo for menor que o atual, aderir à troca.
    if (best_custo < s.custo)
    {
        cout << "Best i: " << best_i << " Best j: " << best_j << "\n";
        vector<int> sub_seq = vector<int>(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
        s.custo = best_custo;

        if (best_i > best_j)
        {
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
            s.sequencia.insert(s.sequencia.begin() + best_j+1, sub_seq.begin(), sub_seq.end());
        }
        else
        {
            s.sequencia.insert(s.sequencia.begin() + best_j+1, sub_seq.begin(), sub_seq.end());
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
        }
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
    cout << "Custo Solucao " << s.custo << endl;

    UpdateAllSubseq(s, subseq_matrix, data);

    cout << "Or-opt:" << endl;
    or_opt(s, subseq_matrix, data, 2);

    showSolution(s);
    cout << s.custo << endl;

    return 0;
}