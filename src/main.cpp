#include "Data.h"
#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>

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
    int noInserido; // vértice k que será inserido
    int arestaRemovida; // aresta {i, j} que será removida aṕos a inserção do novo k
    double custo; // custo ao inserir o vértice 'k' na aresta (i, j)

    // estrutura necessária para fazer a comparação de custo
    inline bool operator<(const InsertionInfo &right){
        return custo < right.custo;
    }
};

// iterando sobre os vértices da solução e mostrar os resultados obtidos
void showSolution(Solution &s){
    for(int i=0; i < s.sequencia.size(); i++)
    {
        cout << s.sequencia[i] << " ";
        //cout << s.sequencia.back() << endl;
    }
    cout << endl;
}

vector<InsertionInfo> calcularCustoInsercao(Solution &s, vector<int> &CL, Data &d){
    
    // o tamanho do vetor de custoInsercao será do tamanho de sequencia * o tamanho da lista de candidados
    const int tam = (s.sequencia.size() - 1) * CL.size();

    vector<InsertionInfo> custoInsercao(tam);

    int l = 0;
    // iterando na solução
    for (int a = 0; a < s.sequencia.size()-1; a++)
    {
        int i = s.sequencia[a];
        int j = s.sequencia[a+1];

        // inserindo todos os vertices da lista de candidados e calculando custo
        for (auto k: CL){

            custoInsercao[l].custo = d.getDistance(i, k) + d.getDistance(k, j) - d.getDistance(i, j);
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }
    return custoInsercao;
}
void escolher3aleatorios(Solution &s, vector <int> &CL){
    
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

    //finalizando sequencia com o próprio 1
    s.sequencia.push_back(1);
}

void CustoSolucao(Solution &s, Data &d){
    double custo = 0;

    for (int i=0; i < s.sequencia.size()-1; i++){
        custo += d.getDistance(s.sequencia[i], s.sequencia[i+1]);
    }
    
    s.custo = custo;
}

// faz a construção de uma solução razoável para o problema através do método de inserção mais barata
Solution Construcao(Data &data)
{
    int tam = data.getDimension() - 1;

    // escrevendo lista de candidatos e começando com tam-1 devido a solução já começar com o 1.
    vector<int> CL(tam);

    //inicializando valores da lista de candidatos e começando a partir do 2
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

    while(!CL.empty())
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

    return s;
}

// Estrutura de vizinhaça Swap - N1
bool Swap(Solution &s, Data &d){

    // Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i=1; i < s.sequencia.size()-1; i++){

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_next = s.sequencia[i+1];
        int vi_prev = s.sequencia[i-1];
        
        for (int j = i + 1; j < s.sequencia.size()-1; j++){

            // Inicializando cálculo da variação
            double delta = 0;

            // Vértice j e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            // Tratando o caso em que vi e vj são consecutivos
            if (vi_next == vj){
                delta += 2 * d.getDistance(vi, vj);
                cout << "vi == vj " << delta << endl;
            }

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vi, vi_next) + d.getDistance(vj_prev, vj) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vj) + d.getDistance(vj, vi_next) + d.getDistance(vj_prev, vi) + d.getDistance(vi, vj_next));

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    // Se o melhor delta for menor que 0, aderir à troca.
    if (bestDelta < 0){
        std::swap(s.sequencia[best_i], s.sequencia[best_j]);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

int main(int argc, char **argv)
{
    
    auto data = Data(argc, argv[1]);
    data.read();
    unsigned int n = data.getDimension();

    srand(time(NULL));

    Solution s_construct = Construcao(data);
    CustoSolucao(s_construct, data);

    cout << "Construção: ";
    showSolution(s_construct);
    cout << "Custo: " << s_construct.custo << endl;

    cout << "Swap: " ;
    Swap(s_construct, data);
    showSolution(s_construct);
    cout << "Custo Swap-neighbor: " << s_construct.custo << endl;

    CustoSolucao(s_construct, data);
    cout << "Custo Calculado: " << s_construct.custo << endl;

    return 0;
}
