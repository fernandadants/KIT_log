# KIT - LOG 

## TSP e Framework ILS

### Inicializar o leitor de instâncias
1. Executando arquivo makefile
    > make

1. Executando o arquivo tsp e escolhendo a instância desejada da pasta ./intances como parâmetro
    >./tsp ./instances/att48.tsp
    - Saída:
    ~~~~cmd
    Dimension: 48
    DistanceMatrix: 
    0 1495 381 2012 1157 ... 
    Exemplo de Solucao s = 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> 11 -> 12 -> 13 -> 14 -> 15 -> 16 -> 17 -> 18 -> 19 -> 20 -> 21 -> 22 -> 23 -> 24 -> 25 -> 26 -> 27 -> 28 -> 29 -> 30 -> 31 -> 32 -> 33 -> 34 -> 35 -> 36 -> 37 -> 38 -> 39 -> 40 -> 41 -> 42 -> 43 -> 44 -> 45 -> 46 -> 47 -> 48 -> 1
    Custo de S: 49840

### Resultado das intâncias:

| Instância | Tempo   | Custo  |
| --------- | ------- | ------ |
| a280      | 95739ms | 2658   |
| att48     | 567ms   | 10628  |
| bayg29    | 126ms   | 1610   |
| bays29    | 126ms   | 2020   |
| berlin52  | 779ms   | 7542   |
| bier127   | 12109ms | 121924 |
| brazil58  | 1054ms  | 25395  |
| burma14   | 13ms    | 3323   |
| eil51     | 676ms   | 428    |
| eil76     | 2217ms  | 552    |
| fri26     | 99ms    | 937    |
| gr96      | 4826ms  | 55750  |
| rat99     | 5045ms  | 1225   |

