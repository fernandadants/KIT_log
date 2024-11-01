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
    double valorObj;
    double custo;
};

// estrutura auxiliar para inserção de novos vértices
struct InsertionInfo
{
    int noInserido;     // vértice k que será inserido
    int arestaRemovida; // aresta {i, j} que será removida aṕos a inserção do novo k
    double custo;       // custo ao inserir o vértice 'k' na aresta (i, j)

    // estrutura necessária para fazer a comparação de custo
    inline bool operator<(const InsertionInfo &right)
    {
        return custo < right.custo;
    }
};

// iterando sobre os vértices da solução e mostrar os resultados obtidos
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

void showSequence(vector<int> &s)
{
    for (int i = 0; i < s.size(); i++)
    {
        cout << s[i] << " ";
    }
    cout << endl;
}

vector<InsertionInfo> calcularCustoInsercao(Solution &s, vector<int> &CL, Data &d)
{

    // o tamanho do vetor de custoInsercao será do tamanho de sequencia * o tamanho da lista de candidados
    const int tam = (s.sequencia.size() - 1) * CL.size();

    vector<InsertionInfo> custoInsercao(tam);

    int l = 0;
    // iterando na solução
    for (int a = 0; a < s.sequencia.size() - 1; a++)
    {
        int i = s.sequencia[a];
        int j = s.sequencia[a + 1];

        // inserindo todos os vertices da lista de candidados e calculando custo
        for (auto k : CL)
        {

            custoInsercao[l].custo = d.getDistance(i, k) + d.getDistance(k, j) - d.getDistance(i, j);
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }
    return custoInsercao;
}
void escolher3aleatorios(Solution &s, vector<int> &CL)
{

    // começando solução a partir do 1
    s.sequencia = {1};

    // escolhendo 3 nós aleatórios para serem inseridos na solução
    for (int i = 0; i < 3; i++)
    {
        int tam = CL.size();

        // gerar indice do nó que será selecionado
        unsigned int indice_no_aleatorio = rand() % (tam);
        int no_aleatorio = CL[indice_no_aleatorio];

        // inserindo nó na solução
        s.sequencia.push_back(no_aleatorio);

        // excluindo nó da lista de candidados
        CL.erase(CL.begin() + indice_no_aleatorio);
    }

    // finalizando sequencia com o próprio 1
    s.sequencia.push_back(1);
}

void custoSolucao(Solution &s, Data &d)
{
    double custo = 0;

    for (int i = 0; i < s.sequencia.size() - 1; i++)
    {
        custo += d.getDistance(s.sequencia[i], s.sequencia[i + 1]);
    }

    s.custo = custo;
}

double custoSolucao2(Solution &s, Data &d){

    double custo = 0;

     for (int i = 0; i < s.sequencia.size() - 1; i++)
    {
        custo += d.getDistance(s.sequencia[i], s.sequencia[i + 1]);
    }

    return custo;
}

// faz a construção de uma solução razoável para o problema através do método de inserção mais barata
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

    // definindo valor obj como 0
    s.valorObj = 0;

    // definindo custo inicial como 0
    s.custo = 0;

    // função para escolher 3 nós aleatórios
    escolher3aleatorios(s, CL);

    while (!CL.empty())
    {
        // criando vetor que irá calcular o custo de inserção do novo nó
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL, data);

        // ordenando em ordem crescente
        sort(custoInsercao.begin(), custoInsercao.end());

        // escolhendo um numero aleatório
        double alpha = rand() % 1 + 0.000001;
        int indice_alpha = ceil(alpha * (custoInsercao.size()));

        // selecionando um vertice aleatorio no vetor, do inicio até o numero alpha
        unsigned int selecionado = rand() % indice_alpha;
        int no_selecionado = custoInsercao[selecionado].noInserido;
        int aresta = custoInsercao[selecionado].arestaRemovida;

        // inserindo nó na solução aleatoria
        s.sequencia.insert(s.sequencia.begin() + aresta + 1, no_selecionado);

        // buscando indice do nó que foi selecionado
        auto indice_selecionado = find(CL.begin(), CL.end(), no_selecionado);

        // deletando nó da lista de candidados
        CL.erase(indice_selecionado);
    }

    custoSolucao(s, data);

    return s;
}

// Estrutura de vizinhaça Swap - N1
bool swap(Solution &s, Data &d)
{

    // Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 1; i++)
    {

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_next = s.sequencia[i + 1];
        int vi_prev = s.sequencia[i - 1];

        for (int j = i + 1; j < s.sequencia.size() - 1; j++)
        {

            // Inicializando cálculo da variação
            double delta = 0;

            // Vértice j e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j + 1];
            int vj_prev = s.sequencia[j - 1];

            // Tratando o caso em que vi e vj são consecutivos
            if (vi_next == vj)
            {
                delta += 2 * d.getDistance(vi, vj);
            }

            delta += -(d.getDistance(vi_prev, vi) + d.getDistance(vi, vi_next) + d.getDistance(vj_prev, vj) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vj) + d.getDistance(vj, vi_next) + d.getDistance(vj_prev, vi) + d.getDistance(vi, vj_next));

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (bestDelta < 0)
    {
        swap(s.sequencia[best_i], s.sequencia[best_j]);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

// Estrutura de vizinhança Two-Opt
bool two_Opt(Solution &s, Data &d)
{

    // Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 2; ++i)
    {

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_next = s.sequencia[i + 1];
        int vi_prev = s.sequencia[i - 1];

        for (int j = i + 1; j < s.sequencia.size() - 1; ++j)
        {

            // Inicializando cálculo da variação
            double delta = 0;

            // Vértice j e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j + 1];
            int vj_prev = s.sequencia[j - 1];

            delta += -(d.getDistance(vi_prev, vi) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vj) + d.getDistance(vi, vj_next));

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (bestDelta < 0)
    {
        reverse(s.sequencia.begin() + best_i, s.sequencia.begin() + best_j + 1);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool or_opt(Solution &s, Data &d, int bloco)
{

    // Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i = 1; i < s.sequencia.size() - 1; i++)
    {

        // Vértice i e a posição do ultimo no bloco
        int vi = s.sequencia[i];
        int vi_prev = s.sequencia[i - 1];

        int vi_aux = s.sequencia[i + bloco - 1];
        int vi_aux_next = s.sequencia[i + bloco];

        // Um exemplo de bloco de 3 elementos:
        // Sequencia: {1, 2, 3, 4, 5, 6, 1}
        // Bloco:        {2, 3, 4}
        // Vértices: {vi, middle, vi_aux}

        for (int j = i + bloco; j < s.sequencia.size() - 2; j++)
        {

            // Inicializando cálculo da variação
            double delta = 0;

            // Posição vj e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j + 1];
            int vj_prev = s.sequencia[j - 1];

            delta += -(d.getDistance(vi_prev, vi) + d.getDistance(vi_aux, vi_aux_next) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vi_aux_next) + d.getDistance(vj, vi) + d.getDistance(vi_aux, vj_next));

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (delta < bestDelta)
            {
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (bestDelta < 0)
    {
        // cout << "Best i: " << best_i << " Best j: " << best_j << "\n";
        vector<int> sub_seq = vector<int>(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);

        if (best_i > best_j)
        {
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
            s.sequencia.insert(s.sequencia.begin() + best_j + 1, sub_seq.begin(), sub_seq.end());
        }
        else
        {
            s.sequencia.insert(s.sequencia.begin() + best_j + 1, sub_seq.begin(), sub_seq.end());
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
        }

        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

void BuscaLocal(Solution &s, Data &d)
{
    vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while (NL.empty() == false)
    {
        int n = rand() % NL.size();

        switch (NL[n])
        {
        case 1:
            improved = swap(s, d);
            break;
        case 2:
            improved = two_Opt(s, d);
            break;
        case 3:
            improved = or_opt(s, d, 1);
            break;
        case 4:
            improved = or_opt(s, d, 2);
            break;
        case 5:
            improved = or_opt(s, d, 3);
            break;
        }

        if (improved)
        {
            NL = {1, 2, 3, 4, 5};
        }
        else
        {
            NL.erase(NL.begin() + n);
        }
    }
}

Solution perturbacao(Solution &s, Data &d)
{
    int bestdelta = 0;
    int tam = s.sequencia.size();

    int i1, j1, i2, j2;

    // Gerar duas sequencias de números não sobrepostos
    while (true)
    {
        bool isOver = false;

        i1 = rand() % (tam / 10) + 2;     // tamanho do bloco 1
        j1 = rand() % (tam - i1 - 2) + 2; // posição de inicio do bloco 1

        i2 = rand() % (tam / 10) + 2;     // tamanho do bloco 2
        j2 = rand() % (tam - i2 - 2) + 2; // posição de inicio do bloco 2

        // Verificar se os blocos são sobrepostos
        for (int i = 0; i < i1; i++)
        {
            if (s.sequencia[j1 + i] == s.sequencia[j2])
            {
                isOver = true;
            }
        }

        for(int i=0; i < i2; i++){
            if (s.sequencia[j2 + i] == s.sequencia[j1])
            {
                isOver = true;
            }
        }

        // Se não estão sobrepostos, sair do loop
        if (!isOver)
        {
            break;
        }
    }

    if (j1 > j2)
    {
        swap(i1, i2);
        swap(j1, j2);
    }

    // Vértice i1 e suas respectivas vizinhanças
    int vi1 = s.sequencia[j1];
    int vi1_prev = s.sequencia[j1 - 1];

    int vi1_aux = s.sequencia[j1 + i1 - 1];
    int vi1_aux_next = s.sequencia[j1 + i1];

    // Um exemplo de bloco de 3 elementos:
    // Sequencia: {1, 2, 3, 4, 5, 6, 1}
    // Bloco:        {2, 3, 4}
    // Vértices: {vi, middle, vi_aux}

    // Cálculo da variação
    double delta = 0;

    // Posição vj e seus vizinhos
    int vi2 = s.sequencia[j2];
    int vi2_prev = s.sequencia[j2 - 1];

    int vi2_aux = s.sequencia[j2 + i2 - 1];
    int vi2_aux_next = s.sequencia[j2 + i2];

    if (vi1_aux_next == vi2)
    {
        delta += d.getDistance(vi1_aux, vi2) - d.getDistance(vi1_aux, vi1) - d.getDistance(vi2_aux, vi2) + d.getDistance(vi2_aux, vi1);
    }

    delta += -(d.getDistance(vi1_prev, vi1) + d.getDistance(vi1_aux, vi1_aux_next) + d.getDistance(vi2_prev, vi2) + d.getDistance(vi2_aux, vi2_aux_next)) + (d.getDistance(vi1_prev, vi2) + d.getDistance(vi2_aux, vi1_aux_next) + d.getDistance(vi2_prev, vi1) + d.getDistance(vi1_aux, vi2_aux_next));

    // Se o melhor delta for menor que 0, aderir à troca.
    if (delta < 0)
    {   
        double custo_ant = s.custo;
        s.custo = s.custo + delta;

        vector<int> sub_seq1 = vector<int>(s.sequencia.begin() + j1, s.sequencia.begin() + j1 + i1);
        vector<int> sub_seq2 = vector<int>(s.sequencia.begin() + j2, s.sequencia.begin() + j2 + i2); 

        s.sequencia.erase(s.sequencia.begin() + j2, s.sequencia.begin() + j2 + i2);
        s.sequencia.erase(s.sequencia.begin() + j1, s.sequencia.begin() + j1 + i1);
        s.sequencia.insert(s.sequencia.begin() + j1, sub_seq2.begin(), sub_seq2.end());
        s.sequencia.insert(s.sequencia.begin() + j2+i2-i1, sub_seq1.begin(), sub_seq1.end());
        
        return s;
    }

    return s;
}

Solution ILS(int maxIter, int maxIterIls, Data &d)
{

    Solution bestOfAll;
    bestOfAll.custo = INFINITY;

    for (int i = 0; i < maxIter; i++)
    {

        Solution s = construcao(d);
        Solution best = s;
        int iterIls = 0;

        while (iterIls <= maxIterIls)
        {
            BuscaLocal(s, d);

            if (s.custo < best.custo)
            {
                best = s;
                iterIls = 0;
            }

            s = perturbacao(best, d);

            iterIls++;
        }

        if (best.custo < bestOfAll.custo)
        {
            bestOfAll = best;
        }
    }

    return bestOfAll;
}

int main(int argc, char **argv)
{
    ofstream file("./output.txt", ios_base::app);

    if(!file.is_open()){
        cout << "Erro ao abrir arquivo de saída" << endl;
    }

    auto start = chrono::high_resolution_clock::now();

    auto data = Data(argc, argv[1]);
    data.read();
    unsigned int n = data.getDimension();

    srand(time(NULL));

    int maxIter = 50;
    int maxIterIls;

    if (n > 150)
    {
        maxIterIls = n / 2;
    }
    else
    {
        maxIterIls = n;
    }
    
    Solution best, s;
    best.custo = INFINITY;

    for (int i=0; i<10;i++){
        s = ILS(maxIter, maxIterIls, data);
        if (s.custo < best.custo){
            best = s;
        }
    }
    
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    file << data.getInstanceName() <<  ", " << duration << "ms, " << best.custo << endl;

    /* cout << "Tempo de execução: " << duration << "ms\n";
    cout << "Dimensao: " << n << endl;
    //Solution s = ILS(maxIter, maxIterIls, data);
    cout << "Exemplo de Solução: " << endl;
    showSolution(s);
    cout << "Custo: " << s.custo << endl;
    cout << "Custo Calculado: " << custoSolucao2(s, data) << endl; */

    file.close();

    return 0;
}
