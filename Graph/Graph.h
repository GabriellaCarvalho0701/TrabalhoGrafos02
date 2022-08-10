#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <list>
#include <queue>
#include <vector>

#include "../Edge/Edge.h"
#include "../Node/node.h"

using std::list;
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::string;
using std::vector;

class Node;
class Edge;

class Graph {
   public:
    Graph(size_t argc, char **argv);

    Graph(int quantityNodes);

    Graph(int inferiorLimit, int upperLimit);

    ~Graph() = default;

    void addCounterOfNodes();
    int getCounterOfNodes();

    void addCounterOfEdges();
    int getCounterOfEdges();

    void setLimit(int limit);
    int getLimit();

    void setFirstNode(Node *node);
    Node *getFirstNode();

    int *getAllAdjacents(int id, int *cont);
    Node *getNodeIfExist(int id);
    Node *getNodePkId(int id);

    Node *createNodeIfDoesntExist(int id, float weight);
    Edge *createEdge(Node *nodeHead, Node *nodeTail, float weight);

    float calculateBenefit();

    vector<pair<int, int>> leituraArquivo();

    vector<pair<int, int>> vertices;
    vector<vector<float>> matrizDistancia;
    vector<pair<int, int>> processaPrimeiraLinhaRanRealSparse(const string &linha);

    vector<pair<int, int>> leituraRanRealeSparse(std::stringstream &fileIn);

    vector<pair<int, int>> leituraHandover(std::stringstream &fileIn);

    void criaArestas();

    vector<Graph *> guloso(vector<pair<int, int>> limiteClusters, bool randomizado, float *result, float semente);
    void algGulosoRandAdapt(vector<pair<int, int>> limitClusters);
    void algGuloso(vector<pair<int, int>> limitClusters);
    void algGulosoReativo(vector<pair<int, int>> limitClusters);
    float qualidadeSolucao(float resultadoObtido);

    void imprimeCluster(vector<Graph *> solucao, int option, float result);
    void output(string outputFileName, vector<Graph *> solucao, float qualidadeSolucao);
    void printNodes();
    void printNodes2();

   private:
    Node *firstNode;
    int nodesTotal;
    int edgesTotal;
    float inferiorLimit;
    float upperLimit;
    float currentLimit;
    vector<Edge *> vectorOfEdges;

    float totalBeneficio;

    // por daniel, para leitura do arquivo
    void inicializaListaDeCandidatos();

    Node * retornaNoValidoDosCandidatos(float min, float max);

    std::list<int>* listaDeCandidatos;
    string pathArquivoEntrada;
    string pathArquivoSaida;

    int tipoInstancia;
    int quantidadeClusters;
    string clusterType;
};

#endif