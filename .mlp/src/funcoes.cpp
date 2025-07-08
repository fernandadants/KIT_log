#include "funcoes.h"

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
        double alpha = 0.99;

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

bool swap(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data)
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
            Subsequence sigma;

            Subsequence a = subseq_matrix[0][i-1];
            Subsequence b = subseq_matrix[j][j];
            Subsequence c = subseq_matrix[i+1][j-1];
            Subsequence d = subseq_matrix[i][i];
            Subsequence e = subseq_matrix[j+1][n];

            sigma = Subsequence:: Concatenate(a, b, data);

            if(j-i > 1){
                sigma = Subsequence:: Concatenate(sigma, c, data);
            }

            sigma = Subsequence:: Concatenate(sigma, d, data);
            sigma = Subsequence:: Concatenate(sigma, e, data);

            // Se o delta calculado for melhor do que o que já existe, trocar.
            if (sigma.C < best_custo)
            {
                best_custo = sigma.C;
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
        UpdateAllSubseq(s, subseq_matrix, data);
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

bool or_opt(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data, int bloco)
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

        //Sequencia: {1 2 3 4 5 6 1}
        //              j i
        //           {1 2 3 4 5 6 1}
        //           {1 3 4 2 5 6 1}


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

                sigma = Subsequence:: Concatenate(a, b, data);
                sigma = Subsequence:: Concatenate(sigma, c, data);
                sigma = Subsequence:: Concatenate(sigma, z, data);
            }else{

                Subsequence a = subseq_matrix[0][j-1];
                Subsequence b = subseq_matrix[i][i+bloco-1];
                Subsequence c = subseq_matrix[j][i-1];
                Subsequence z = subseq_matrix[i+bloco][n];

                sigma = Subsequence:: Concatenate(a, b, data);
                sigma = Subsequence:: Concatenate(sigma, c, data);
                sigma = Subsequence:: Concatenate(sigma, z, data);
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
        vector<int> sub_seq = vector<int>(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
        s.custo = best_custo;

        if (best_i > best_j)
        {
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
            s.sequencia.insert(s.sequencia.begin() + best_j, sub_seq.begin(), sub_seq.end());
        }
        else
        {
            s.sequencia.insert(s.sequencia.begin() + best_j+1, sub_seq.begin(), sub_seq.end());
            s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + bloco);
        }
        UpdateAllSubseq(s, subseq_matrix, data);
        return true;
    }

    return false;
}

void BuscaLocal(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &d)
{
    vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while (NL.empty() == false)
    {
        int n = rand() % NL.size();

        switch (NL[n])
        {
        case 1:
            improved = swap(s, subseq_matrix, d);
            break;
        case 2:
            improved = two_Opt(s, subseq_matrix, d);
            break;
        case 3:
            improved = or_opt(s, subseq_matrix, d, 1);
            break;
        case 4:
            improved = or_opt(s, subseq_matrix, d, 2);
            break;
        case 5:
            improved = or_opt(s, subseq_matrix, d, 3);
            break;
        }

        /* int custo_calculado = custoSolucao2(s.sequencia, d);
        if(custo_calculado != s.custo){
            cout << "Erro: " << custo_calculado << " " << s.custo << " n " << n << endl;
        } */

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

Solution perturbacao(Solution s, vector<vector<Subsequence>> &subseq_matrix, Data &d)
{
    int tam = s.sequencia.size();

    int i1, j1, i2, j2; 

    // Gerar duas sequencias de números não sobrepostos
    while (true)
    {
        bool isOver = false;

        i1 = rand() % (tam / 10) + 2;     // tamanho do bloco 1
        j1 = rand() % (tam - i1 - 2) + 1; // posição de inicio do bloco 1

        i2 = rand() % (tam / 10) + 2;     // tamanho do bloco 2
        j2 = rand() % (tam - i2 - 2) + 1; // posição de inicio do bloco 2

        if (j1 > j2)
        {
            swap(i1, i2);
            swap(j1, j2);
        }

        if (j1+i1 < j2){
            break;
        }
    }

    vector<int> sub_seq1 = vector<int>(s.sequencia.begin() + j1, s.sequencia.begin() + j1 + i1);
    vector<int> sub_seq2 = vector<int>(s.sequencia.begin() + j2, s.sequencia.begin() + j2 + i2); 

    s.sequencia.erase(s.sequencia.begin() + j2, s.sequencia.begin() + j2 + i2);
    s.sequencia.erase(s.sequencia.begin() + j1, s.sequencia.begin() + j1 + i1);
    s.sequencia.insert(s.sequencia.begin() + j1, sub_seq2.begin(), sub_seq2.end());
    s.sequencia.insert(s.sequencia.begin() + j2+i2-i1, sub_seq1.begin(), sub_seq1.end());


    UpdateAllSubseq(s, subseq_matrix, d);
    custoSolucao(s, d);
    return s;
}

Solution ILS(int maxIter, int maxIterIls, Data &d)
{

    Solution bestOfAll;
    bestOfAll.custo = INFINITY;
    unsigned int n = d.getDimension() + 1;

    for (int i = 0; i < maxIter; i++)
    {

        Solution s = construcao(d);
        vector<vector<Subsequence>> subseq_matrix(n, vector<Subsequence>(n));
        UpdateAllSubseq(s, subseq_matrix, d);

        Solution best = s;
        int iterIls;

        for (iterIls = 0; iterIls < maxIterIls; iterIls++){

            //Realiza busca local na solução
            BuscaLocal(s, subseq_matrix, d);

            if (s.custo < best.custo)
            {
                best = s;
                iterIls = 0;
            }

            //Perturba a solução da busca local
            s = perturbacao(best, subseq_matrix, d);
            int custo_calculado = custoSolucao2(s.sequencia, d);
            if(custo_calculado =! s.custo){
                cout << "Erro ILS: " << custo_calculado << " " << s.custo << endl;
    }
        }

        if (best.custo < bestOfAll.custo)
        {
            bestOfAll = best;
        }
    }

    return bestOfAll;
}