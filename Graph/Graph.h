#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <list>
#include <queue>
#include <vector>

//#include "../Edge/Edge.h"
//#include "../Node/node.h"

using std::list;
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::string;
using std::vector;

class Node;
class Edge;
class Graph;

class Edge {
   public:
    Edge(Node *headNode, Node *tailNode, float weight, Graph *graph);
    ~Edge() {
    }

    void setNextEdge(Edge *nextEdge);
    Edge *getNextEdge();

    int getId();
    Node *getTailNode();
    Node *getHeadNode();

    int getWeight();
    // int edgeCost(Node *nodeHead, Node *tailNode);

   private:
    int id;
    float weight;
    Node *tailNode;
    Node *headNode;
    Edge *nextEdge;
};

class Node {
    /*
    Id: não pode repetir -> a partir do contador incrementado
    Pk: a partir da entrada do txt -> sem repetir
    */
   public:
    Node(int id, float weight, Graph *graph);
    ~Node() {
        delete this->nextNode;
    }

    // void addNode(Node* node, Edge* edge, Graph* graph);
    int getPkId();
    int getId();

    Node *getNextNode();
    void setNextNode(Node *node);

    Edge *getFirstEdge();
    void setFirstEdge(Edge *firstEdge);

    void incrementDegreeOut();
    void incrementDegreeIn();

    int getGrauIn();
    int getGrauOut();

    int getWeight();
    vector<Edge *> getAdjacentsEdges();

   private:
    int pk;  // Primary Key = ID Único
    int id;
    int weight;
    int degreeIn;
    int degreeOut;
    Node *nextNode;
    Edge *firstEdge;
};

class Graph {
   public:
    Graph(size_t argc, char **argv);

    Graph(int quantityNodes);

    Graph(int inferiorLimit, int upperLimit);

    ~Graph() {
        for (auto &edge : this->vectorOfEdges) {
            delete edge;
        }
        delete this->firstNode;
    }

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

    vector<Graph *> guloso(vector<pair<int, int>> limiteClusters, float *result);
    vector<Graph *> gulosoRandomizado(vector<pair<int, int>> limitClusters, float *result, float alfa);
    void algGulosoRandAdapt(vector<pair<int, int>> limitClusters);
    void algGuloso(vector<pair<int, int>> limitClusters);
    void algGulosoReativo(vector<pair<int, int>> limitClusters);

    void imprimeCluster(vector<Graph *> solucao, int option, float result);
    void output(vector<Graph *> solucao, float qualidadeSolucao);
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

    Node *retornaNoValidoDosCandidatos(float min, float max);

    std::list<int> *listaDeCandidatos;
    string pathArquivoEntrada;
    string pathArquivoSaida;

    int tipoInstancia;
    int quantidadeClusters;
    string clusterType;
};

#endif