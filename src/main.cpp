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

void custoSolucao(Solution &s, Data &d){
    double custo = 0;

    for (int i=0; i < s.sequencia.size()-1; i++){
        custo += d.getDistance(s.sequencia[i], s.sequencia[i+1]);
    }
    
    s.custo = custo;
}

// faz a construção de uma solução razoável para o problema através do método de inserção mais barata
Solution construcao(Data &data)
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

    custoSolucao(s, data);

    return s;
}

// Estrutura de vizinhaça Swap - N1
bool swap(Solution &s, Data &d){

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
        swap(s.sequencia[best_i], s.sequencia[best_j]);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

// Estrutura de vizinhança Two-Opt
bool two_Opt(Solution &s, Data &d){

    // Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i=1; i < s.sequencia.size()-2; ++i){

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_next = s.sequencia[i+1];
        int vi_prev = s.sequencia[i-1];
        
        for (int j = i + 1; j < s.sequencia.size()-1; ++j){

            // Inicializando cálculo da variação
            double delta = 0;

            // Vértice j e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vj) + d.getDistance(vi, vj_next));

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
        reverse(s.sequencia.begin()+best_i, s.sequencia.begin()+best_j+1);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool reinsertion(Solution &s, Data &d){

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

            // Posição vj e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vi, vi_next) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vi_next) + d.getDistance(vj, vi) + d.getDistance(vi, vj_next));

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
        auto i = s.sequencia[best_i];

        s.sequencia.erase(s.sequencia.begin() + best_i);
        s.sequencia.insert(s.sequencia.begin() + best_j, i);
        
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool or_two_Opt(Solution &s, Data &d){
    
    //Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i=1; i < s.sequencia.size()-1; i++){

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_prev = s.sequencia[i-1];

        int vi_two = s.sequencia[i+1];
        int vi_two_next = s.sequencia[i+2];
        
        for (int j = i + 2; j < s.sequencia.size()-1; j++){

            // Inicializando cálculo da variação
            double delta = 0;

            // Posição vj e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vi_two, vi_two_next) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vi_two_next) + d.getDistance(vj, vi) + d.getDistance(vi_two, vj_next));

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
        int i = s.sequencia[best_i];
        int i_aux = s.sequencia[best_i+1];

        s.sequencia.erase(s.sequencia.begin() + best_i);
        s.sequencia.erase(s.sequencia.begin() + best_i);

        s.sequencia.insert(s.sequencia.begin() + best_j, i);
        s.sequencia.insert(s.sequencia.begin() + best_j+1, i_aux);
        
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool or_three_Opt(Solution &s, Data &d){
    
    //Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i=1; i < s.sequencia.size()-1; i++){

        // Vértice i e seus vizinhos
        int vi = s.sequencia[i];
        int vi_prev = s.sequencia[i-1];
        
        int vi_three = s.sequencia[i+2];
        int vi_three_next = s.sequencia[i+3];
        
        for (int j = i + 3; j < s.sequencia.size()-1; j++){

            // Inicializando cálculo da variação
            double delta = 0;

            // Posição vj e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vi_three, vi_three_next) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vi_three_next) + d.getDistance(vj, vi) + d.getDistance(vi_three, vj_next));

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

        int i = s.sequencia[best_i];
        int i_two = s.sequencia[best_i+1];
        int i_three = s.sequencia[best_i+2];

        s.sequencia.erase(s.sequencia.begin() + best_i);
        s.sequencia.erase(s.sequencia.begin() + best_i);
        s.sequencia.erase(s.sequencia.begin() + best_i);

        s.sequencia.insert(s.sequencia.begin() + best_j, i);
        s.sequencia.insert(s.sequencia.begin() + best_j+1, i_two);
        s.sequencia.insert(s.sequencia.begin() + best_j+2, i_three);
        
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool or_opt(Solution &s, Data &d, int bloco){

    //Definindo o melhor delta até o momento;
    double bestDelta = 0;
    int best_i, best_j;

    // Começando a iterar sob os nós da solução
    for (int i=1; i < s.sequencia.size()-1; i++){

        // Vértice i e a posição do ultimo no bloco
        int vi = s.sequencia[i];
        int vi_prev = s.sequencia[i-1];
        
        int vi_aux = s.sequencia[i+bloco-1];
        int vi_aux_next = s.sequencia[i+bloco];

        // Um exemplo de bloco de 3 elementos:
        // Sequencia: {1, 2, 3, 4, 5, 6, 1}
        // Bloco:        {2, 3, 4}
        // Vértices: {vi, middle, vi_aux}
        
        for (int j = i + bloco; j < s.sequencia.size()-2; j++){

            // Inicializando cálculo da variação
            double delta = 0;

            // Posição vj e seus vizinhos
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            delta += - (d.getDistance(vi_prev, vi) + d.getDistance(vi_aux, vi_aux_next) + d.getDistance(vj, vj_next)) + (d.getDistance(vi_prev, vi_aux_next) + d.getDistance(vj, vi) + d.getDistance(vi_aux, vj_next));
            cout << delta << endl;

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
        rotate(s.sequencia.begin()+best_i, s.sequencia.begin()+best_i+bloco, s.sequencia.begin()+best_j+1);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

void BuscaLocal(Solution &s, Data &d){
    vector <int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while(NL.empty() == false){
        int n = rand() % NL.size();

        switch (NL[n]){
        case 1:
            improved = swap(s, d);
            cout << "SWAP: ";
            showSolution(s);
            break;
        case 2:
            improved = two_Opt(s, d);
            cout << "TWO-OPT: ";
            showSolution(s);
            break;
        case 3:
            improved = reinsertion(s, d);
            cout << "REINSERTION: ";
            showSolution(s);
            break;
        case 4:
            improved = or_two_Opt(s, d);
            cout << "OR TWO: ";   
            showSolution(s); 
            break;
        case 5:
            improved = or_three_Opt(s, d);
            cout << "OR THREE: ";
            showSolution(s);
            break;
        }

        if(improved){
            NL = {1, 2, 3, 4, 5};
        }else{
            NL.erase(NL.begin()+n);
        }
    }
}

int main(int argc, char **argv)
{
    
    auto data = Data(argc, argv[1]);
    data.read();
    unsigned int n = data.getDimension();

    srand(time(NULL));

    /*Solution s = construcao(data);

    BuscaLocal(s_construct, data);

    cout << "Busca Local: ";
    showSolution(s_construct);
    cout << "Custo aCalculado: " << s_construct.custo << endl; */

    Solution s;
    s.sequencia = {1, 2, 3, 4, 5, 6, 1};
    custoSolucao(s, data);

    cout << "Solução 3-or-opt: ";
    or_opt(s, data, 3);
    showSolution(s);
    cout << "Custo Calculado: " << s.custo << endl;

    return 0;
}
