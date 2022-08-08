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
    void printNodes();
    void printNodes2();

    int *getAllAdjacents(int id, int *cont);
    Node *getNodeIfExist(int id);
    Node *getNodePkId(int id);

    Node *createNodeIfDoesntExist(int id, float weight);
    Edge *createEdge(Node *nodeHead, Node *nodeTail, float weight);

    void outputGraph(string outputFileName);
    void printListAdjacents(int id);

    int *depthSearch(Node *node);
    void auxDepthSearch(Node *node, int visitedNodes[], int *counter);
    void treeDeepthSearch(Node *node, string outputFileName);
    void auxTreeDeepthSearch(Node *node, vector<Node *> &visitedNodes, vector<Edge *> &mainTreeEdge, vector<Edge *> &returnTreeEdge);

    vector<Edge *> edgesMergeSort(vector<Edge *> &edgesToBeMerged);

    bool isNodeInGraph(Node *searchedNode);
    Edge *insertEdge(Edge *edge);
    Node *insertNode(Node *node);

    float calculateBenefit();

    vector<pair<int, int>> leituraArquivo();

    
    std::vector<std::pair<int, int>> vertices;  // deducao do codigo do stenio
    std::vector<std::vector<float>> matrizDistancia;
    vector<pair<int, int>> processaPrimeiraLinhaRanRealSparse(const string &linha);

    vector<pair<int, int>> leituraRanRealeSparse(std::stringstream &fileIn);

    void leituraHandover(std::stringstream &fileIn);

    void criaArestas();

    template <typename T>
    void imprimeMatrizParaDebug(const vector<std::vector<T>> &matriz);

    float guloso(vector<pair<int, int>> limiteClusters, bool randomizado);
    void imprimeCluster(vector<Graph *> solucao, int option, float result);

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
    const string pathArquivoEntrada;
    const string pathArquivoSaida;

    const int tipoInstancia;
    int quantidadeClusters;
    string clusterType;

};

#endif