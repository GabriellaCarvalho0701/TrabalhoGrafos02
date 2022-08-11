#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <sstream>

using std::cin;
using std::cout;
using std::endl;
using std::string;

#include "./Edge/Edge.cpp"
#include "./Graph/Graph.cpp"
#include "./Node/node.cpp"

void processOperationChoice(Graph *graph, vector<pair<int, int>> limiteClusters) {
    int option;
    cout << "\nDigite a funcionalidade que deseja para o grafo inserido: " << endl;

    cout << "(1) Algoritmo Guloso" << endl;
    cout << "(2) Algoritmo Guloso Randomizado Adaptativo" << endl;
    cout << "(3) Algoritmo Guloso Randomizado Reativo" << endl;
    cout << "-> ";
    cin >> option;
    cout << "\n";

    Graph *newGraph;

    switch (option) {
        case 1:
            cout << "Algoritmo Guloso" << endl;
            graph->algGuloso(limiteClusters);
            break;
        case 2:
            cout << "Algoritmo Guloso Randomizado e Adaptativo" << endl;
            graph->algGulosoRandAdapt(limiteClusters);
            break;
        case 3:
            cout << "Algoritmo Guloso Randomizado Reativo" << endl;
            graph->algGulosoReativo(limiteClusters);
            break;
        default:
            processOperationChoice(graph, limiteClusters);
            break;
    }
}

/*
 * Funcao principal, faz as leituras dos argumentos passados na linha de comentado
 * @params: argc: numero de arumentos
 *          argv: argumentos
 * @return:
 ****************************************************************/
int main(int argc, char *argv[]) {
    long int aux = time(nullptr);
    srand(aux);
    auto *graph = new Graph(argc, argv);
    vector<pair<int, int>> limiteClusters = graph->leituraArquivo();

    processOperationChoice(graph, limiteClusters);

    return 0;
}
