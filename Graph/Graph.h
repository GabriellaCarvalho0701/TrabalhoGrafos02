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

    Graph(int quantityNodes, int inferiorLimit, int upperLimit);

    ~Graph() = default;



    void addCounterOfNodes();
    int getCounterOfNodes();

    void addCounterOfEdges();
    int getCounterOfEdges();

    void setFirstNode(Node *node);
    Node *getFirstNode();

    bool isEdgeWeighted();
    bool isNodeWeighted();
    bool getDirected();

    int *getAllAdjacents(int id, int *cont);
    Node *getNodeIfExist(int id);
    Node *getNodePkId(int id);

    Node *createNodeIfDoesntExist(int id, int peso);
    Edge *createEdge(Node *nodeHead, Node *tailNode, int weight);

    void outputGraph(string outputFileName);
    void outputNodes(string outputFileName, Node *nodes[], int cont, string textStart);
    void outputGraphSetOfNodes(string outputFileName, std::queue<int> nodes);
    void outputEdgeInducedSubgraph(string outputFileName, vector<Edge *> &searchTree, vector<Edge *> &returnEdges);
    void printListAdjacents(int id);
    void printNodes();

    void floyd(int idNodeOrig, int idNodeDest);
    void dijkstra(int idNodeOrig, int idNodeDest);
    int edgeCost(Node *nodeHead, Node *tailNode);

    int *depthSearch(Node *node);
    void auxDepthSearch(Node *node, int visitedNodes[], int *counter);
    void treeDeepthSearch(Node *node, string outputFileName);
    void auxTreeDeepthSearch(Node *node, vector<Node *> &visitedNodes, vector<Edge *> &mainTreeEdge, vector<Edge *> &returnTreeEdge);

    vector<Edge *> edgesMergeSort(vector<Edge *> &edgesToBeMerged);

    bool isNodeInGraph(Node *searchedNode);
    Edge *insertEdge(Edge *edge);
    Node *insertNode(Node *node);

    void setDistanceMatrix();
    void setDistanceMatrix(int nodeHead, int nodeTail, int weight);

    void leituraArquivo();

private:
    Node *firstNode;
    int nodesTotal;
    int edgesTotal;
    int inferiorLimit;
    int upperLimit;
    bool hasWeightedEdges;
    bool isDirected;
    bool hasWeightedNodes;
    vector<Edge *> vectorOfEdges;
    float **distanceMatrix;

    // por daniel, para leitura do arquivo
    int quantidadeClusters;
    string clusterType;

    const string pathArquivoEntrada;
    const string pathArquivoSaida;
    const int tipoInstancia;

    // std::vector<std::pair<int,float>> vertices; // nao entendi pq stenio amazena dessa forma

    void processaPrimeiraLinhaRanRealSparse(const string &linha);
    void leituraRanRealeSparse(std::stringstream &fileIn);
    void leituraHandover(std::stringstream &fileIn);
};

#endif