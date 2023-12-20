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
    
    // começando solução a partir do 1 e finalizando nele mesmo; definindo valor obj como 0
    s.sequencia = {1};
    s.valorObj = 0;

    // escolhendo 3 nós aleatórios para serem inseridos na solução
    for (int i = 0; i < 3; i++)
    {
        // gerar indice do nó que será selecionado  
        unsigned int indice_no_aleatorio = rand() % (tam);
        unsigned int no_aleatorio = CL[indice_no_aleatorio];

        // inserindo nó na solução
        s.sequencia.push_back(no_aleatorio);

        // excluindo nó da lista de candidados
        CL.erase(CL.begin() + indice_no_aleatorio);
    }

    s.sequencia.push_back(1);

    showSolution(s);

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

        s.sequencia.insert(s.sequencia.begin() + aresta + 1, no_selecionado);

        // buscando indice do nó que foi selecionado
        auto indice_selecionado = find(CL.begin(), CL.end(), no_selecionado);

        CL.erase(indice_selecionado);
    }

    return s;
    
}

int main(int argc, char **argv)
{
    
    auto data = Data(argc, argv[1]);
    data.read();
    unsigned int n = data.getDimension();

    srand(time(NULL));

    Solution s_construct = Construcao(data);

    showSolution(s_construct);
    
    return 0;
}
