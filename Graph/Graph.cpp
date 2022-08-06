#include "Graph.h"

#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>

using std::cout;
using std::endl;
using std::fstream;

using std::greater;
using std::list;
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::queue;
using std::vector;

using std::string;

Graph::Graph(size_t argc, char **argv)
        : nodesTotal(0), edgesTotal(0), firstNode(nullptr), isDirected(false), hasWeightedEdges(true), hasWeightedNodes(true), inferiorLimit(-1), upperLimit(-1), distanceMatrix(nullptr),
          pathArquivoEntrada(static_cast<string>(argv[1])), pathArquivoSaida(static_cast<string>(argv[2])), tipoInstancia(static_cast<int>(atoi(argv[3])))
{
    if (argc != 4)
    {
        std::cerr << "[ERRO] parametros do programa incorretos\nEsperado: ./execGrupoX_ <arquivo_entrada> <arquivo_saida> <Tipo_Instancia>\n";
        exit(-1);
    }
}

Graph::Graph(int quantityNodes, int inferiorLimit, int upperLimit) :tipoInstancia(-1) // necessario
{
    this->firstNode = nullptr;
    this->nodesTotal = 0;
    this->edgesTotal = 0;
    this->isDirected = 0;
    this->hasWeightedEdges = 1;
    this->hasWeightedNodes = 1;
    this->inferiorLimit = inferiorLimit;
    this->upperLimit = upperLimit;

    setDistanceMatrix();
}

Graph::Graph(int quantityNodes) :tipoInstancia(-1)
{
    this->firstNode = nullptr;
    this->nodesTotal = 0;
    this->edgesTotal = 0;
    this->isDirected = 0;
    this->hasWeightedEdges = 1;
    this->hasWeightedNodes = 1;

    setDistanceMatrix();
}

void Graph::addCounterOfNodes() {
    nodesTotal++;
}

int Graph::getCounterOfNodes() {
    return nodesTotal;
}

void Graph::addCounterOfEdges() {
    edgesTotal++;
}

int Graph::getCounterOfEdges() {
    return edgesTotal;
}

void Graph::setFirstNode(Node* node) {
    firstNode = node;
}

Node* Graph::getFirstNode() {
    return firstNode;
}

bool Graph::isEdgeWeighted() {
    return hasWeightedEdges;
}

bool Graph::isNodeWeighted() {
    return hasWeightedNodes;
}

bool Graph::getDirected()
{
    return isDirected;
}

void Graph::setDistanceMatrix() {
    this->distanceMatrix = new float*[getCounterOfNodes()];

    for (int i = 0; i < getCounterOfNodes(); i++) {
        distanceMatrix[i] = new float[getCounterOfNodes()];
        for (int j = 0; j < getCounterOfNodes(); j++) {
            distanceMatrix[i][j] = -1;
        }
    }
}

void Graph::setDistanceMatrix(int nodeHead, int nodeTail, int weight) {
    distanceMatrix[nodeHead][nodeTail] = weight;
    distanceMatrix[nodeTail][nodeHead] = weight;
}

/*
 * Função que encontra um nó já existente no grafo através de seu id ou retorna null,
 caso o id não corresponda a um nó existente
 *@params: int id: identificador do vértice a ser encontrado no grafo
 *
 *@return: um ponteiro para o objeto nó já previamente existente ou nullptr
 ****************************************************************/
Node* Graph::getNodeIfExist(int id) {
    Node* node = this->firstNode;

    if (node == nullptr) return nullptr;

    while (node != nullptr) {
        if (node->getId() == id) {
            return node;
        }

        node = node->getNextNode();
    }

    return nullptr;
}

bool Graph::isNodeInGraph(Node* searchedNode) {
    Node* node = this->firstNode;

    if (node == nullptr) return false;

    while (node != nullptr) {
        if (node == searchedNode) {
            return true;
        }

        node = node->getNextNode();
    }

    return false;
}

/*
 * Função que cria o nó caso ele ainda não exista no grafo
 *@params: int id: identificador do vértice a ser criado
 *         int weight: peso do vértice
 *@return: um ponteiro para o objeto nó criado ou já previamente existente
 ****************************************************************/
Node* Graph::createNodeIfDoesntExist(int id, int weight) {
    Node* node = this->getNodeIfExist(id);

    if (node == nullptr) {
        Node* newNode = new Node(id, weight, this);
        Node* oldFirstNode = firstNode;
        newNode->setNextNode(oldFirstNode);
        setFirstNode(newNode);

        return newNode;
    }

    return node;
}

/*
 * Função que cria uma aresta no grafo
 *@params: Node *nodeHead: objeto nó de onde a aresta parte (vértice cabeça)
 *         Node *nodeTail: objeto nó onde a aresta chega (vértice cauda)
 *         int weight: peso da aresta
 *@return: um ponteiro para o objeto aresta criado
 ****************************************************************/
Edge* Graph::createEdge(Node* nodeHead, Node* nodeTail, int weight) {
    Edge* newEdge = new Edge(nodeHead, nodeTail, weight, this);
    this->vectorOfEdges.emplace_back(newEdge);

    Edge* nodeFirstEdge = nodeHead->getFirstEdge();

    nodeHead->incrementDegreeOut();
    nodeTail->incrementDegreeIn();

    if (nodeFirstEdge == nullptr) {
        nodeHead->setFirstEdge(newEdge);
    } else {
        newEdge->setNextEdge(nodeFirstEdge);
        nodeHead->setFirstEdge(newEdge);
    }

    return newEdge;
}

/*
 * Função verifica se um Edge já esta no grafo. Verifica também se a versão inversa
 * da aresta esta no grafo, uma vez que por definição de enunciado do problemas isso
 * nao pode existir
 *@params: vector<Edge*> insertedEdges: Vetor de arestas onde será testado
 *         Edge* edge: Aresta que se deseja verificar
 *@return: bool isInserted: Se a aresta esta no vetor de arestas
 ****************************************************************/
bool edgeIsInserted(vector<Edge*> insertedEdges, Edge* edge) {
    bool isInserted = false;
    for (int i = 0; i < insertedEdges.size() && !isInserted; i++) {
        if (
            insertedEdges[i] == edge ||
            (insertedEdges[i]->getTailNode() == edge->getHeadNode() && insertedEdges[i]->getHeadNode() == edge->getTailNode())) {
            isInserted = true;
        }
    }

    return isInserted;
}

/*
 * Função escreve o grafo no arquivo passado. Obs.: Retira as arestas duplicadas
 * dos grafos nao direcionados
 *@params: string outputFileName: Nome do arquivo que será registrado o grafo
 *         bool isWeightedGraph: Informa se trata-se de um grafo ponderado
 *@return:
 ****************************************************************/
void Graph::outputGraph(string outputFileName) {
    Node* node = this->firstNode;

    FILE* outfile = fopen(outputFileName.c_str(), "w+");

    string title;
    if (!this->getDirected()) {
        title = "graph { \n";
    } else {
        title = "digraph { \n";
    }

    fwrite(title.c_str(), 1, title.size(), outfile);

    vector<Edge*> insertedEdges;

    while (node != nullptr) {
        Edge* edge = node->getFirstEdge();

        while (edge != nullptr) {
            // Id da Aresta (Linha do arquivo em que a aresta é criada)
            string nodeBase = std::to_string(node->getId());
            string nodeLinked = std::to_string((edge->getTailNode())->getId());

            string dotNotation = "";

            if (this->isEdgeWeighted()) {
                string weight = std::to_string(edge->getWeight());
                if (!this->getDirected()) {
                    if (!edgeIsInserted(insertedEdges, edge)) {
                        dotNotation = string(nodeBase) + "--" + string(nodeLinked) + " [weight=\"" + string(weight) + "\"] [label=\"" + string(weight) + "\"];\n";
                        insertedEdges.emplace_back(edge);
                    }
                } else {
                    dotNotation = string(nodeBase) + "->" + string(nodeLinked) + " [weight=\"" + string(weight) + "\"] [label=\"" + string(weight) + "\"];\n";
                }
            } else {
                if (!this->getDirected()) {
                    if (!edgeIsInserted(insertedEdges, edge)) {
                        dotNotation = string(nodeBase) + "--" + string(nodeLinked) + ";\n";
                        insertedEdges.emplace_back(edge);
                    }
                } else {
                    dotNotation = string(nodeBase) + "->" + string(nodeLinked) + ";\n";
                }
            }

            fwrite(dotNotation.c_str(), 1, dotNotation.size(), outfile);

            edge = edge->getNextEdge();
        }

        node = node->getNextNode();
    };

    string end = "}";

    fwrite(end.c_str(), 1, end.size(), outfile);

    cout << "O arquivo " << outputFileName << " foi gerado com sucesso. Para visualizar encerre a execucao";
}

/*
 * Função escreve todos os nós do grafo no terminal
 *@params:
 *
 *@return:
 ****************************************************************/
void Graph::printNodes() {
    Node* node = firstNode;
    int cont = 0;

    while (node != nullptr) {
        cout << cont << " - Node " << node->getId() << " Pk Id: " << node->getPkId() << endl;
        node = node->getNextNode();
        cont++;
    }

    cout << "Fim";
}

/*
 * Função escreve o grafo de menor caminho no arquivo passado de acordo com os nós
 *@params: string outputFileName: Nome do arquivo que será registrado o grafo
 *         bool isWeightedGraph: Informa se trata-se de um grafo ponderado
 *         bool isDirectedGraph: Informa se trata-se de um grafo direcionado
 *         queue<pair<int, int>> nodes: Lista contendo os vértices do caminho mínimo encontrado
 *@return:
 ****************************************************************/

void Graph::outputGraphSetOfNodes(string outputFileName, queue<int> nodes) {
    FILE* outfile = fopen(outputFileName.c_str(), "w+");

    string title;
    if (!getDirected()) {
        title = "graph { \n";
    } else {
        title = "digraph { \n";
    }

    fwrite(title.c_str(), 1, title.size(), outfile);

    int p = nodes.front();
    nodes.pop();

    while (!nodes.empty()) {
        int q = nodes.front();
        nodes.pop();

        string nodeBase = std::to_string(p);
        string nodeLinked = std::to_string(q);

        int costEdge = edgeCost(getNodeIfExist(p), getNodeIfExist(q));
        if (costEdge == -1)
            continue;

        // cout << "p: " << p << " q: " << q << " = " << costEdge << endl;

        string dotNotation = "";

        if (this->isEdgeWeighted()) {
            string weight = std::to_string(costEdge);
            if (!getDirected()) {
                dotNotation = string(nodeBase) + "--" + string(nodeLinked) + " [weight=\"" + string(weight) + "\"] [label=\"" + string(weight) + "\"];\n";
            } else {
                dotNotation = string(nodeBase) + "->" + string(nodeLinked) + " [weight=\"" + string(weight) + "\"] [label=\"" + string(weight) + "\"];\n";
            }
        } else {
            if (!getDirected()) {
                dotNotation = string(nodeBase) + "--" + string(nodeLinked) + ";\n";
            } else {
                dotNotation = string(nodeBase) + "->" + string(nodeLinked) + ";\n";
            }
        }

        fwrite(dotNotation.c_str(), 1, dotNotation.size(), outfile);

        p = q;
    }

    string end = "}";

    fwrite(end.c_str(), 1, end.size(), outfile);

    cout << "O arquivo " << outputFileName << " foi gerado com sucesso.";
}

/*
 * Função escreve o os nós passados em um arquivo
 *@params: string outputFileName: Nome do arquivo que será registrado os nós
 *         Node* nodes[]: conjuntos de nós resposta da operação realizada
 *         int cont: contador de quantos nós há no array nodes
 *         string textStart: texto inicial a ser escrito
 *@return:
 ****************************************************************/
void Graph::outputNodes(string outputFileName, Node* nodes[], int cont, string textStart) {
    FILE* outfile = fopen(outputFileName.c_str(), "w+");

    string text = string(textStart) + std::to_string(nodes[0]->getId()) + ": \n";
    fwrite(text.c_str(), 1, text.size(), outfile);

    for (int i = 1; i < cont; i++) {
        text = std::to_string(nodes[i]->getId()) + string(",");
        fwrite(text.c_str(), 1, text.size(), outfile);
    }

    cout << "O arquivo " << outputFileName << " foi gerado com sucesso.";
}

/*
 * Função que encontra mostra a lista de adjacências de um nó
 *@params: int id: identificador do vértice a ser mostrada a lista de adjacência
 *
 *@return:
 ****************************************************************/
void Graph::printListAdjacents(int id) {
    Node* node = this->getNodeIfExist(id);

    if (node == nullptr) {
        return;
    }

    Edge* edge = node->getFirstEdge();

    cout << " Lista de adjacencia do no " << node->getId() << ": " << endl;
    while (edge != nullptr) {
        cout << "- No: " << edge->getTailNode()->getId() << endl;
        edge = edge->getNextEdge();
    };

    cout << "Fim" << endl;
}

/*
 * Função que encontra um nó já existente no grafo através de seu pkId ou retorna null,
 caso o id não corresponda a um nó existente
 *@params: int id: identificador do vértice a ser encontrado no grafo
 *
 *@return: um ponteiro para o objeto nó já previamente existente ou nullptr
 ****************************************************************/
Node* Graph::getNodePkId(int id) {
    Node* node = firstNode;

    while (node != nullptr) {
        if (node->getPkId() == id) {
            return node;
        }

        node = node->getNextNode();
    }

    return nullptr;
}

/*
 * Função que encontra retorna a lista de adjacências de um nó através de um vetor
 que contém os id's dos nós adjacentes
 *@params: int id: identificador do vértice a ser encontrada a lista de adjacência
 *         int *cont: ponteiro para um contador que indicará quantos nós adjacentes o nó dado possui
 *
 *@return: um ponteiro para o vetor de int's (que correspodem aos id's dos nós adjacentes)
 ****************************************************************/
int* Graph::getAllAdjacents(int id, int* cont) {
    Node* node = this->getNodeIfExist(id);
    int* allNodeAdjacents = new int[getCounterOfNodes()];

    if (node == nullptr) {
        return 0;
    }

    Edge* edge = node->getFirstEdge();

    while (edge != nullptr) {
        allNodeAdjacents[*cont] = edge->getTailNode()->getId();
        (*cont)++;
        edge = edge->getNextEdge();
    };

    return allNodeAdjacents;
}

/*
 * Função verifica o custo da aresta entre dois nós passados
 *@params: Node* nodeHead: nó cuja aresta parte
           Node* tailNode: nó cuja aresta chega
 *@return: retorna o valor do custo da aresta (-1 caso não haja aresta)
 ****************************************************************/
int Graph::edgeCost(Node* nodeHead, Node* tailNode) {
    for (Edge* i = nodeHead->getFirstEdge(); i != nullptr; i = i->getNextEdge()) {
        if (i->getTailNode() == tailNode)
            return i->getWeight();
    }

    return -1;
}

/*
 * Função que faz a busca em profundidade e retorna um vetor contendo os id's dos
 vértices visitados a partir de um nó dado
 *@params: Node* node: ponteiro de objeto do tipo nó
 *
 *@return: um ponteiro para o vetor de int's (que correspodem aos id's dos vértices visitados)
 ****************************************************************/
int* Graph::depthSearch(Node* node) {
    int* cont = 0;
    int* visitedNodes = new int[this->getCounterOfNodes()];
    for (int i = 0; i <= this->getCounterOfNodes(); i++) {
        visitedNodes[i] = 0;
    }

    auxDepthSearch(node, visitedNodes, cont);

    return visitedNodes;
}

/*
 * Função que auxiliar da função de busca em profundidade que efetivamente procura
 os nós do grafo em profundidade  a partir de um dado nó
 *@params: Node* node: ponteiro de objeto do tipo nó de onde partirá a busca
 *         int visitedNodes[]: vetor de int's que contém os id's dos vértices visitados
 *         int* cont: ponteiro para um contador que indicará quantos nós foram visitados
 *
 *@return:
 ****************************************************************/
void Graph::auxDepthSearch(Node* node, int visitedNodes[], int* cont) {
    cont++;
    visitedNodes[node->getPkId()] = 1;
    Edge* edge = node->getFirstEdge();

    while (edge != nullptr) {
        if (visitedNodes[edge->getTailNode()->getPkId()] == 0) {
            auxDepthSearch(edge->getTailNode(), visitedNodes, cont);
        }

        edge = edge->getNextEdge();
    };
}

/*
 * Função auxiliar ordernar um vetor de arestas atraves de merge sort
 *@params: vector<Edge *>& vector: vetor de aretas a ser ordenado
           int start: posicao inicial
           int mid: meio do vetor
           int end: final do vetor
 *@return:
 ****************************************************************/
void mergeSortedIntervals(vector<Edge*>& vetor, int start, int mid, int end) {
    vector<Edge*> temp;

    int i, j;
    i = start;
    j = mid + 1;

    while (i <= mid && j <= end) {
        if (vetor[i]->getWeight() <= vetor[j]->getWeight()) {
            temp.push_back(vetor[i]);
            ++i;
        } else {
            temp.push_back(vetor[j]);
            ++j;
        }
    }

    while (i <= mid) {
        temp.push_back(vetor[i]);
        ++i;
    }

    while (j <= end) {
        temp.push_back(vetor[j]);
        ++j;
    }

    for (int i = start; i <= end; ++i)
        vetor[i] = temp[i - start];
}

/*
 * Função para ordernar um vetor de arestas atraves de merge sort
 *@params: vector<Edge *>& vector: vetor de aretas a ser ordenado
           int start: posicao inicial
           int end: final do vetor
 *@return:
 ****************************************************************/
void mergeSort(vector<Edge*>& vetor, int start, int end) {
    if (start < end) {
        int mid = (start + end) / 2;
        mergeSort(vetor, start, mid);
        mergeSort(vetor, mid + 1, end);
        mergeSortedIntervals(vetor, start, mid, end);
    }
}


std::streampos inline tamanhoArquivo(fstream &arq)
{
    arq.seekg(0, std::fstream::end);
    std::streampos tam = arq.tellg();
    arq.seekg(0);
    return tam;
}

void Graph::leituraArquivo()
{
    fstream arquivoEntrada(static_cast<string>(this->pathArquivoEntrada), std::ios::in);
    if (!arquivoEntrada.is_open())
    {
        std::cerr << "\n\t[ERRO] arquivo nao pode ser aberto lerArquivo";
        exit(10);
    }

    auto bufferSize = tamanhoArquivo(arquivoEntrada);
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    arquivoEntrada.read(buffer.get(), bufferSize);
    arquivoEntrada.close();
    std::stringstream fileIn(buffer.get()); // os dados estao aqui

    if (this->tipoInstancia == 1) // RanReal e Sparse
    {
        this->leituraRanRealeSparse(fileIn);
    }
    else if (this->tipoInstancia == 2) // Handover
    {
        this->leituraHandover(fileIn);
    }
    else
    {
        std::cerr << "\n[ERRO] tipo de instancia nao reconhecido\n(1)RanReal ou Sparse\n(2)Handover\n";
        exit(-1);
    }
}

void Graph::leituraRanRealeSparse(std::stringstream &fileIn)
{
    string primeiraLinha;
    getline(fileIn, primeiraLinha);

    processaPrimeiraLinhaRanRealSparse(primeiraLinha); // resgata informacoes basicas do grafo

    // processo restante das arestas
    string linha;
    int verticeFonte = 0, verticeAlvo = 0;
    float beneficio = 0;
    while (getline(fileIn, linha, ' ') && linha !="")
    {
//        getline(fileIn, linha, ' ');
        verticeFonte = stoi(linha);

        getline(fileIn, linha, ' ');
        verticeAlvo = stoi(linha);

        getline(fileIn, linha, '\n');
        beneficio = stof(linha);

        // stenio cria uma matriz distancia ??
        this->createEdge(getNodeIfExist(verticeFonte), getNodeIfExist(verticeAlvo), beneficio); // TODO: mudar parametro para float
    }
}


void Graph::processaPrimeiraLinhaRanRealSparse(const string &linha)
{
    // divide a linha em palavras para ser possível separar os dados
    std::stringstream ss(linha);
    string item;
    vector<string> tokens;
    while (getline(ss, item, ' '))
    {
        tokens.push_back(item);
    }
    // informacoes basicas do grafo
    this->nodesTotal = stoi(tokens[0]);
    this->quantidadeClusters = stoi(tokens[1]);
    this->clusterType =  tokens[2]; //  ds ou ss TODO: nao parece ser realmente util, visto que todos sao ds e nao usa-se essa informacao mais?!

    // TODO: visto que stenio nao usa essa informacao e nao parece ser util guardar o limite de cada cluster
    // get limites dos clusters
    vector<pair<int, int>> clustersLimites; //clustersLimites[i] = (limiteInferior, limiteSuperior)
    for (int i = 0; i < quantidadeClusters; i++)
    {
        int clusterId = stoi(tokens[3 + i * 2]);
        int clusterLimite = stoi(tokens[4 + i * 2]);
        clustersLimites.emplace_back(clusterId, clusterLimite);
    }

    // get node weights
    // vector<int> nodeWeights;
    int contVertices = 0;
    for (int i = 0; i < this->nodesTotal; i++)
    {
        int nodeWeight = stoi(tokens[4 + quantidadeClusters * 2 + i]);
        // nodeWeights.push_back(nodeWeight);
        this->createNodeIfDoesntExist(i, nodeWeight); // TODO: mudar parametro para float
        ++contVertices;
    }


}

void Graph::leituraHandover(std::stringstream &fileIn)
{
    string linha;
    getline(fileIn, linha, '\n');
    int quantidadeNos = stoi(linha);

    getline(fileIn, linha, '\n');
    int quantidadeClusters = stoi(linha);

    getline(fileIn, linha, '\n');
    float upperLimit = stof(linha); // todos tem os mesmos limites
    float lowerLimit = 0.0f;

    for (int i = 0; i < quantidadeNos; ++i)
    {
        getline(fileIn, linha);
        float beneficio = stof(linha);
        // inserir no vertice i com tal beneficio
    }

    getline(fileIn, linha, '\n');
    for (int i = 0; i < quantidadeNos; ++i)
    {
        for (int j = 0; j < quantidadeNos; ++j)
        {
            // linha >> grafo->matrizDistancia[i][j];
        }
    }
}

