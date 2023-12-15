#include "Data.h"
#include <vector>
#include <random>
#include <iostream>
#include <time.h>
#include <algorithm>

using namespace std;

// estrutura para criar uma solução do TSP
struct Solution
{
    vector<int> sequencia;
    double valorObj;
};

// estrutura necessária para realizar a inserção de novos vértices na solução
struct InsertionInfo
{
    int noInserido;     // novo vértice 'k' que será inserido
    int arestaRemovida; // aresta {i, j} que será removida após a inserção do novo 'k'
    double custo;       // delta (ou custo) ao inserir o vértice 'k' na aresta (i, j)
};

// iterando sobre os vértices da solução e mostrar resultados
void showSolution(Solution &s)
{
    for (int i = 0; i < s.sequencia.size() - 1; i++)
    {
        cout << s.sequencia[i] << "-> ";
        cout << s.sequencia.back() << endl;
    }
}

// calcula o cusco da inserção de um novo vértice 'k' na solução
vector<InsertionInfo> calcularCustoInsercao(Solution &s, vector<int> &CL, Data &d)
{
    vector<InsertionInfo> custoInsercao((s.sequencia.size() - 1) * CL.size());

    int l = 0;
    for (int a = 0; a < s.sequencia.size() - 1; a++)
    {
        int i = s.sequencia[a];
        int j = s.sequencia[a + 1];
        for (auto k : CL)
        {
            custoInsercao[l].custo = d.getDistance(i, k) + d.getDistance(j, k) - d.getDistance(i, j);
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = a;
            l++;
        }
    }
    return custoInsercao;
}

void ordenarPorCusto(vector<InsertionInfo>& info) {
    int tam = info.size();

    for (int i = 0; i < tam - 1; ++i) {

        // encontra o índice do menor custo
        int indiceMenor = i;

        // fazendo iteracao dos proximos numeros
        for (int j = i + 1; j < tam; j++) {
            
            // se o custo do proximo numero for menor que o atual
            if (info[j].custo < info[indiceMenor].custo) {
                indiceMenor = j;
            }
        }

        // troca o elemento atual com o menor encontrado
        InsertionInfo temp = info[indiceMenor];
        info[indiceMenor] = info[i];
        info[i] = temp;
    }
}

// faz a construção de uma solução razoável para o problema através do método de inserção mais barata
Solution Construcao(Data &data)
{
    // inicializando arestas da solução
    int tam = data.getDimension();

    // escrevendo lista de candidatos
    vector<int> CL(tam);

    // inicializando valores com a lista de candidatos
    for (int i = 0; i < CL.size(); i++)
    {
        CL[i] = i + 1;
    }

    // iniciando processo de formação da solução
    Solution s;

    // começando a partir do vertice 1 e finalizando nele mesmo
    s.sequencia = {1, 1};

    // escolhendo três nós aleatorios para serem inseridos na solução
    for (int i = 0; i < 3; i++)
    {
        // gerar o indice do nó que será selecionado (entre a posição 1 e tam);
        unsigned int indice_no_aleatorio = rand() % tam + 1;
        unsigned int no_aleatorio = CL[indice_no_aleatorio];

        // inserindo nó na solução
        s.sequencia.insert(s.sequencia.begin() + i + 1, no_aleatorio);

        // excluindo nó da lista de candidatos
        CL.erase(CL.begin() + indice_no_aleatorio);
    }

    // enquanto a lista de candidatos não estiver vazia
    while (!CL.empty())
    {

        // criando vetor que irá calcular o custo de inserção do novo nó
        vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL, data);

        // ordenando em ordem crescente
        ordenarPorCusto(custoInsercao);

        // escolhendo um numero aleatório entre os disponíveis no vetor
        double alpha = rand() % tam;

        // selecionando um vértice aleatório entre o inicio do vetor até o numero alpha
        int selecionado = rand() % ((int)ceil(alpha * custoInsercao.size()));

        //
        int no_selecionado = custoInsercao[selecionado].noInserido;
        int aresta = custoInsercao[selecionado].arestaRemovida;
        s.sequencia.insert(s.sequencia.begin() + aresta + 1, no_selecionado);

        auto indice_selecionado = find(CL.begin(), CL.end(), no_selecionado);
        CL.erase(CL.begin() + indice_selecionado[0]);
    }

    return s;
}

int main(int argc, char **argv)
{

    auto data = Data(argc, argv[1]);
    data.read();
    unsigned int n = data.getDimension();

    cout << "Exemplo de Solucao s = ";
    double cost = 0.0;
    for (unsigned int i = 1; i < n; i++)
    {
        // cout << i << " -> ";
        cost += data.getDistance(i, i + 1);
    }
    cost += data.getDistance(n, 1);
    // cout << n << " -> " << 1 << endl;
    cout << "Custo de S: " << cost << endl;

    srand(time(NULL));

    Solution construct = Construcao(data);
    
    return 0;
}
