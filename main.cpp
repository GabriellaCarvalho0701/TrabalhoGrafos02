#include <dirent.h>
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

    int no, noDest;
    Graph *newGraph;

    switch (option) {
        case 1:
            cout << "Algoritmo Guloso" << endl;
            graph->guloso(limiteClusters, 0);
            break;
        case 2:
            cout << "Algoritmo Guloso Randomizado Adaptativo" << endl;
            graph->guloso(limiteClusters, 1);
            break;
        case 3:
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
    auto *graph = new Graph(argc, argv);
    vector<pair<int, int>> limiteClusters = graph->leituraArquivo();

    processOperationChoice(graph, limiteClusters);

    return 0;
}
