# Introdução - Pesquisa Operacional
### Inicializar o leitor de instâncias
1. Arquivo .Data do diretório .obj estava dando conflito, por isso precisei fazer a exclusão;

1. Limpando todos os arquivos e objetos de dependência;
    > make clean

1. Executando arquivo makefile
    > make

1. Executando o arquivo tsp e passando dados da ./intances como parâmetro
    >./tsp ./instances/att48.tsp
    - Saída:
    ~~~~cmd
    Dimension: 48
    DistanceMatrix: 
    0 1495 381 2012 1157 ... 
    Exemplo de Solucao s = 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> 11 -> 12 -> 13 -> 14 -> 15 -> 16 -> 17 -> 18 -> 19 -> 20 -> 21 -> 22 -> 23 -> 24 -> 25 -> 26 -> 27 -> 28 -> 29 -> 30 -> 31 -> 32 -> 33 -> 34 -> 35 -> 36 -> 37 -> 38 -> 39 -> 40 -> 41 -> 42 -> 43 -> 44 -> 45 -> 46 -> 47 -> 48 -> 1
    Custo de S: 49840