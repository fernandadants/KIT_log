#include "Data.h"
#include <vector>
#include <random>
#include <iostream>

using namespace std;

// implementando a estrutura para criar uma solução do TSP
typedef struct Solution
{
    vector<int> sequencia;
    double valorObj;
} Solution;

// iterando sobre os vértices da solução e mostrar resultados
void showSolution(Solution *s)
{
    for (int i = 0; i < s->sequencia.size() - 1; i++)
    {
        cout << s->sequencia[i] << "-> ";
        cout << s->sequencia.back() << endl;
    }
}

// estrutura necessária para realizar a inserção de novos vértices na solução
typedef struct InsertionInfo
{
    int noInserido;     // novo vértice 'k' que será inserido
    int arestaRemovida; // aresta {i, j} que será removida após a inserção do novo 'k'
    double custo;       // delta (ou custo) ao inserir o vértice 'k' na aresta (i, j)
};

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

vector<int> escolher3NosAleatorios(Data &d){
    vector<int> nos(3);
    unsigned int n = d.getDimension();

    for(int i=0; i<3; i++){
        unsigned int no_aleatorio =  rand() % n;

        if(i==0){
            nos[i] = no_aleatorio;
        }else if (no_aleatorio == nos[i-1]){
            unsigned int no_aleatorio =  rand() % n;
            nos[i] = no_aleatorio;
        }

    }

    return nos;
}

// faz a construção de uma solução razoável para o problema através do método de inserção mais barata
Solution Construcao(Data &d)
{
    Solution s;
    s.sequencia = {1, 1};
    s.sequencia = escolher3NosAleatorios(d);
    //vector<int> CL = noRestantes();

    //while (!CL.empty())
    //{
        //vector<InsertionInfo> custoInsercao = calcularCustoInsercao(s, CL, data);
        //ordenarCrescente(custoInsercao);
    //}
}

int main(int argc, char **argv)
{

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();

    cout << "Dimension: " << n << endl;
    cout << "DistanceMatrix: " << endl;
    data.printMatrixDist();

    cout << "Exemplo de Solucao s = ";
    double cost = 0.0;
    for (size_t i = 1; i < n; i++)
    {
        cout << i << " -> ";
        cost += data.getDistance(i, i + 1);
    }
    cost += data.getDistance(n, 1);
    cout << n << " -> " << 1 << endl;
    cout << "Custo de S: " << cost << endl;

    return 0;
}
