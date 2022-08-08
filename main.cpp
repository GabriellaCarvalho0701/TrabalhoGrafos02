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
 * Funcao principal, faz as leituras dos argumentos passados na linha de comentado
 * @params: argc: numero de arumentos
 *          argv: argumentos
 * @return:
 ****************************************************************/
int main(int argc, char *argv[]) {
    auto *graph = new Graph(argc, argv);
    graph->leituraArquivo();
    return 0;
}
