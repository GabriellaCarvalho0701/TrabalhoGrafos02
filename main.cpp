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

/*
 * Lê e instancia o grafo a partir do arquivo escrito
 * @params: FILE *file: arquivo de onde o grafo sera lido
 *          bool isDirected: se o grafo é direcionado
 *          bool hasWeightedEdges: se o grafo tem peso nas arestas
 *          bool hasWeightedNodes: se o grafo tem peso nos nos
 * @return: Graph*: Grafo final instaciado
 ****************************************************************/
/*Graph *graphReadAndInstantiation(FILE *file) {
    // Graph *graph = new Graph(isDirected, hasWeightedEdges, hasWeightedNodes);

    const int lineSize = 50;

    while (!feof(file)) {
        char line[lineSize];
        char *textInLine = fgets(line, lineSize, file);

        int space = 0;

        if (textInLine != nullptr)
            space = string(textInLine).find(" ", 0);

        if (space > 0) {
            string stringHead, stringTail, stringEdgeWeight;
            int intHead, intTail, intEdgeWeight = 0;

            if (hasWeightedEdges) {
                stringHead = string(textInLine).substr(0, space);
                int newSpace = string(textInLine).find(" ", space + 1);
                stringTail = string(textInLine).substr(space, newSpace);
                stringEdgeWeight = string(textInLine).substr(newSpace, string(textInLine).size());

                intHead = stoi(stringHead);
                intTail = stoi(stringTail);
                intEdgeWeight = stoi(stringEdgeWeight);
            } else {
                stringHead = string(textInLine).substr(0, space);
                stringTail = string(textInLine).substr(space, string(textInLine).size());

                intHead = stoi(stringHead);
                intTail = stoi(stringTail);
                intEdgeWeight = 1;
            }

            Node *nodeHead = graph->createNodeIfDoesntExist(intHead, 0);
            Node *nodeTail = graph->createNodeIfDoesntExist(intTail, 0);

            Edge *edge1 = graph->createEdge(nodeHead, nodeTail, intEdgeWeight);
            if (!isDirected)
                Edge *edge2 = graph->createEdge(nodeTail, nodeHead, intEdgeWeight);
        }

        delete textInLine;
    }

    return graph;
}*/

void instanceTypeOneRead(FILE *file) {
    const int lineSize = 1000;
    char line[lineSize];
    char *textInLine = fgets(line, lineSize, file);

    int space = 0;

    if (textInLine != nullptr)
        space = string(textInLine).find(" ", 0);

    if (space > 0) {
        string quantityNodes, quantityClusters, typeCluster;
        int intQuantityNodes, intQuantityClusters;

        quantityNodes = string(textInLine).substr(0, space);
        int newSpace = string(textInLine).find(" ", space + 1);
        quantityClusters = string(textInLine).substr(space, newSpace);
        newSpace += 1;
        typeCluster = string(textInLine).substr(newSpace, 2);

        intQuantityNodes = stoi(quantityNodes);
        intQuantityClusters = stoi(quantityClusters);

        // cout << "Nodes: '" << intQuantityNodes << "' Clusters: '" << intQuantityClusters << "' Type: '" << typeCluster << "'" << endl;

        for (int i = 0; i < intQuantityClusters; i++) {
            string inferiorLimit, upperLimit;
            int intInferiorLimit, intUpperLimit;

            space = string(textInLine).find(" ", newSpace + 1) + 1;
            newSpace = string(textInLine).find(" ", space + 1);
            inferiorLimit = string(textInLine).substr(space, newSpace - space);
            // cout << "spaceI: '" << space << "' NewSpaceI: '" << newSpace << "' Inf: '" << inferiorLimit << "'" << endl;

            space = newSpace + 1;
            newSpace = string(textInLine).find(" ", space);
            upperLimit = string(textInLine).substr(space, newSpace - space);
            // cout << i << " spaceU: '" << space << "' NewSpaceU: '" << newSpace << "' Upper: '" << upperLimit << "'" << endl;

            try {
                intInferiorLimit = stoi(inferiorLimit);
                intUpperLimit = stoi(upperLimit);
            } catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
            }

            Graph *graph = new Graph(intQuantityNodes, intInferiorLimit, intUpperLimit);

            newSpace = space;
        }

        newSpace = newSpace + 3;

        Graph *graph = new Graph(intQuantityNodes);

        for (int i = 0; i < intQuantityNodes; i++) {
            string weight;
            int intWeight;

            space = string(textInLine).find(" ", newSpace + 1) + 1;
            newSpace = string(textInLine).find(" ", space + 1);
            weight = string(textInLine).substr(space, newSpace - space);

            try {
                intWeight = stoi(weight);
            } catch (const std::exception &e) {
                std::cerr << e.what() << '\n';
            }

            // cout << i << "-> spaceI: '" << space << "' NewSpaceI: '" << newSpace << "' Weight: '" << intWeight << "'" << endl;

            Node *node = graph->createNodeIfDoesntExist(i, intWeight);

            newSpace = space;
        }

        while (!feof(file)) {
            delete textInLine;

            char line[lineSize];
            char *textInLine = fgets(line, lineSize, file);

            int space = 0;

            if (textInLine != nullptr)
                space = string(textInLine).find(" ", 0);

            cout << space << endl;
            cout << textInLine << endl;
            if (space > 0) {
                string stringHead, stringTail, stringEdgeWeight;
                int intHead, intTail, intEdgeWeight = 0;

                stringHead = string(textInLine).substr(0, space);
                int newSpace = string(textInLine).find(" ", space + 1);
                stringTail = string(textInLine).substr(space, newSpace);
                stringEdgeWeight = string(textInLine).substr(newSpace, string(textInLine).size());

                intHead = stoi(stringHead);
                intTail = stoi(stringTail);
                intEdgeWeight = stoi(stringEdgeWeight);
                // cout << "spaceI: '" << space << "' NewSpaceI: '" << newSpace << "' Inf: '" << intEdgeWeight << "'" << endl;

                Node *nodeHead = graph->getNodeIfExist(intHead);
                Node *nodeTail = graph->getNodeIfExist(intTail);

                graph->setDistanceMatrix(intHead, intTail, intEdgeWeight);
            }
        }
    }

    /*
while (!feof(file)) {
    char line[lineSize];
    char *textInLine = fgets(line, lineSize, file);

    int space = 0;

    if (textInLine != nullptr)
        space = string(textInLine).find(" ", 0);

    if (space > 0) {
        string stringHead, stringTail, stringEdgeWeight;
        int intHead, intTail, intEdgeWeight = 0;

        if (hasWeightedEdges) {
            stringHead = string(textInLine).substr(0, space);
            int newSpace = string(textInLine).find(" ", space + 1);
            stringTail = string(textInLine).substr(space, newSpace);
            stringEdgeWeight = string(textInLine).substr(newSpace, string(textInLine).size());

            intHead = stoi(stringHead);
            intTail = stoi(stringTail);
            intEdgeWeight = stoi(stringEdgeWeight);
        } else {
            stringHead = string(textInLine).substr(0, space);
            stringTail = string(textInLine).substr(space, string(textInLine).size());

            intHead = stoi(stringHead);
            intTail = stoi(stringTail);
            intEdgeWeight = 1;
        }

        Node *nodeHead = graph->createNodeIfDoesntExist(intHead, 0);
        Node *nodeTail = graph->createNodeIfDoesntExist(intTail, 0);

        Edge *edge1 = graph->createEdge(nodeHead, nodeTail, intEdgeWeight);
        if (!isDirected)
            Edge *edge2 = graph->createEdge(nodeTail, nodeHead, intEdgeWeight);
    }

    delete textInLine;
}*/

    fclose(file);
}

/*
 * Exibe os detalhes da entrada por escrito de forma descritiva
 * e permite que o usuario confirme ou nao essa entrada.
 * @params: string fileName: nome do arquivo
 *          string path: caminho ate o arquivo
 *          bool hasWeightedEdge: se o grafo tem peso nas arestas
 *          bool hasWeightedNode: se o grafo tem peso nos nos
 *          bool isDirected: se o grafo é direcionado
 * @return: bool: Se o usuário confirma a entrada
 ****************************************************************/
bool confirmEntry(string fileName, string path, bool hasWeightedEdge, bool hasWeightedNode, bool isDirected) {
    int confirmation;

    cout << "\n\n =========================================== \n\n";
    cout << "---- Confirmacao dos argumentos de entrada ---- \n"
         << endl;
    cout << "-> Caminho do arquivo de Entrada: " << path << endl;
    cout << "-> Nome arquivo de Entrada: " << fileName << endl;

    cout << "-> Grafo DIRECIONADO: ";
    isDirected ? cout << "SIM" << endl : cout << "NAO" << endl;

    cout << "-> Grafo PONDERADOS nas ARESTAS: ";
    hasWeightedEdge == true ? cout << "SIM" << endl : cout << "NAO" << endl;

    cout << "-> Grafo PONDERADOS nos NOS: ";
    hasWeightedNode == true ? cout << "SIM" << endl : cout << "NAO" << endl;

    cout << "\n\n =========================================== \n\n";
    cout << "Voce confirma a entrada de dados ? [1 = SIM | 0 = NAO] " << endl;
    cout << "-> ";
    cin >> confirmation;

    if (confirmation == 1) return true;

    return false;
}

/*
 * Exibe e processa os opções e escolhas
 * @params: char *argv[]: lista de argumentos
 *          bool hasWeightedNode: se o grafo tem peso nos nos
 *          bool hasWeightedEdge: se o grafo tem peso nas arestas
 *          bool isDirected: se o grafo e direcionado
 *          Graph *graph: grafo já instanciado
 * @return:
 ****************************************************************/
void processOperationChoice(char *argv[], bool hasWeightedNode, bool hasWeightedEdge, bool isDirected, Graph *graph) {
    /*int option;
    cout << "\nDigite a funcionalidade que deseja para o grafo inserido: " << endl;

    cout << "(1) Fecho Transitivo Direto" << endl;
    cout << "(2) Fecho Transitivo Indireto" << endl;
    cout << "(3) Coeficiente de Agrupamento Local" << endl;
    cout << "-> ";
    cin >> option;
    cout << "\n";

    int no, noDest;
    Graph *newGraph;

    switch (option) {
        case 1:
            cout << "Fecho Transitivo Direto de qual no?" << endl;
            cin >> no;
            graph->fechoTransitivoDireto(no);
            break;
        case 2:
            cout << "Fecho Transitivo Indireto de qual no?" << endl;
            cin >> no;
            graph->fechoTransitivoIndireto(no);
            break;
        case 3:
            cout << "Coeficiente de Agrupamento Local de qual no?" << endl;
            cin >> no;
            graph->coeficienteDeAgrupamentoLocal(no);
            break;
        default:
            processOperationChoice(argv, hasWeightedEdge, hasWeightedNode, isDirected, graph);
            break;
    }

    option = 0;
    cout << "\n\nDeseja realizar nova operacao ? [1 = SIM | 0 = NAO]" << endl;
    cout << "-> ";
    cin >> option;

    if (option == 1) processOperationChoice(argv, hasWeightedEdge, hasWeightedNode, isDirected, graph);*/
}

/*
 * Funcao principal, faz as leituras dos argumentos passados na linha de comentado
 * @params: argc: numero de arumentos
 *          argv: argumentos
 * @return:
 ****************************************************************/
int main(int argc, char *argv[])
{
    // apaguei monte de coisa aqui no intuito de simplificar
    auto *graph = new Graph(argc, argv);
    graph->leituraArquivo();
    return 0;
}
