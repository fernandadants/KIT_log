#include "funcoes.h"
#include <chrono>

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

    /*int n = data.getDimension() + 1;

    Solution s = construcao(data);
    vector<vector<Subsequence>> subseq_matrix(n, vector<Subsequence>(n));
    UpdateAllSubseq(s, subseq_matrix, data); */

    srand(time(NULL));

    int maxIter = 10;
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

    double sum = 0;

    for (int i=0; i<10;i++){
        s = ILS(maxIter, maxIterIls, data);

        if(s.custo<best.custo){
            best = s;
        }

        sum += s.custo;
    }
    
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    file << data.getInstanceName() <<  ", " << duration/10000.0 << "s, " << best.custo << endl;

    /* cout << "Tempo de execução: " << duration << "ms\n";
    cout << "Dimensao: " << n << endl;
    //Solution s = ILS(maxIter, maxIterIls, data);
    cout << "Exemplo de Solução: " << endl;
    showSolution(s);
    cout << "Custo: " << s.custo << endl;
    cout << "Custo Calculado: " << custoSolucao2(s, data) << endl; */

    showSolution(s);
    cout << "Custo: " << s.custo << endl;
    cout << "Custo Calculado: " << custoSolucao2(s.sequencia, data) << endl;

    file.close();

    return 0;
}