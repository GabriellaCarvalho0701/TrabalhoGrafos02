#include "Graph.h"

#include <time.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

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

Graph::Graph(size_t argc, char** argv)
    : nodesTotal(0), edgesTotal(0), firstNode(nullptr), inferiorLimit(-1), upperLimit(-1), currentLimit(0), pathArquivoEntrada(static_cast<string>(argv[1])), pathArquivoSaida(static_cast<string>(argv[2])), tipoInstancia(static_cast<int>(atoi(argv[3]))) {
    if (argc != 4) {
        std::cerr << "[ERRO] parametros do programa incorretos\nEsperado: ./execGrupoX_ <arquivo_entrada> <arquivo_saida> <Tipo_Instancia>\n";
        exit(-1);
    }
}

Graph::Graph(int inferiorLimit, int upperLimit) : tipoInstancia(-1)  // necessario
{
    this->firstNode = nullptr;
    this->nodesTotal = 0;
    this->edgesTotal = 0;
    this->inferiorLimit = inferiorLimit;
    this->upperLimit = upperLimit;
    this->currentLimit = 0;
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

void Graph::setLimit(int limit) {
    this->currentLimit += limit;
}

int Graph::getLimit() {
    return currentLimit;
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

/*
 * Função que cria o nó caso ele ainda não exista no grafo
 *@params: int id: identificador do vértice a ser criado
 *         int weight: peso do vértice
 *@return: um ponteiro para o objeto nó criado ou já previamente existente
 ****************************************************************/
Node* Graph::createNodeIfDoesntExist(int id, float weight) {
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
Edge* Graph::createEdge(Node* nodeHead, Node* nodeTail, float weight) {
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
/*
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
}*/

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

std::streampos inline tamanhoArquivo(fstream& arq) {
    arq.seekg(0, std::fstream::end);
    std::streampos tam = arq.tellg();
    arq.seekg(0);
    return tam;
}

void Graph::leituraArquivo() {
    fstream arquivoEntrada(static_cast<string>(this->pathArquivoEntrada), std::ios::in);
    if (!arquivoEntrada.is_open()) {
        std::cerr << "\n\t[ERRO] arquivo nao pode ser aberto lerArquivo";
        exit(10);
    }

    auto bufferSize = tamanhoArquivo(arquivoEntrada);
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    arquivoEntrada.read(buffer.get(), bufferSize);
    arquivoEntrada.close();
    std::stringstream fileIn(buffer.get());  // os dados estao aqui

    if (this->tipoInstancia == 1)  // RanReal e Sparse
    {
        this->leituraRanRealeSparse(fileIn);
    } else if (this->tipoInstancia == 2)  // Handover
    {
        this->leituraHandover(fileIn);
    } else {
        std::cerr << "\n[ERRO] tipo de instancia nao reconhecido\n(1)RanReal ou Sparse\n(2)Handover\n";
        exit(-1);
    }

    criaArestas();
}

void Graph::leituraRanRealeSparse(std::stringstream& fileIn) {
    string primeiraLinha;
    getline(fileIn, primeiraLinha);
    vector<pair<int, int>> limitClusters;

    limitClusters = processaPrimeiraLinhaRanRealSparse(primeiraLinha);  // resgata informacoes basicas do grafo

    // resize matriz distancia
    this->matrizDistancia.resize(this->getCounterOfNodes());
    for (int i = 0; i < this->getCounterOfNodes(); i++) {
        this->matrizDistancia[i].resize(this->getCounterOfNodes());
    }
    // processo restante das arestas
    string linha;
    int verticeFonte = 0, verticeAlvo = 0;
    float beneficio = 0;
    while (getline(fileIn, linha, ' ') && !linha.empty()) {
        verticeFonte = stoi(linha);

        getline(fileIn, linha, ' ');
        verticeAlvo = stoi(linha);

        getline(fileIn, linha, '\n');
        beneficio = stof(linha);

        // apagar linha abaixo? manter apenas a matriz?
        this->createEdge(getNodeIfExist(verticeFonte), getNodeIfExist(verticeAlvo), beneficio);

        this->matrizDistancia[verticeFonte][verticeAlvo] = beneficio;
        this->matrizDistancia[verticeAlvo][verticeFonte] = beneficio;
    }
    // imprimeMatrizParaDebug(this->matrizDistancia);
    guloso(limitClusters);
}

vector<pair<int, int>> Graph::processaPrimeiraLinhaRanRealSparse(const string& linha) {
    // divide a linha em palavras para ser possível separar os dados
    std::stringstream ss(linha);
    string item;
    vector<string> tokens;
    while (getline(ss, item, ' ')) {
        tokens.push_back(item);
    }
    // informacoes basicas do grafo
    auto nodesTotalOriginal = stoi(tokens[0]);
    this->quantidadeClusters = stoi(tokens[1]);
    this->clusterType = tokens[2];  //  ds ou ss TODO: nao parece ser realmente util, visto que todos sao ds e nao usa-se essa informacao mais?!

    // TODO: visto que stenio nao usa essa informacao e nao parece ser util guardar o limite de cada cluster
    // get limites dos clusters
    vector<pair<int, int>> clustersLimites;  // clustersLimites[i] = (limiteInferior, limiteSuperior)
    for (int i = 0; i < quantidadeClusters; i++) {
        int clusterId = stoi(tokens[3 + i * 2]);
        int clusterLimite = stoi(tokens[4 + i * 2]);
        clustersLimites.emplace_back(clusterId, clusterLimite);
    }

    // get node weights
    int contVertices = 0;
    for (int i = 0; i < nodesTotalOriginal; i++) {
        float nodeWeight = stof(tokens[4 + quantidadeClusters * 2 + i]);
        this->createNodeIfDoesntExist(i, nodeWeight);  // remover essa linha?
        this->vertices.emplace_back(contVertices, nodeWeight);
        ++contVertices;
    }

    return clustersLimites;
}

void Graph::leituraHandover(std::stringstream& fileIn) {
    string linha;
    getline(fileIn, linha, '\n');
    int quantidadeNos = stoi(linha);

    getline(fileIn, linha, '\n');
    this->quantidadeClusters = stoi(linha);

    getline(fileIn, linha, '\n');
    this->upperLimit = stof(linha);  // todos tem os mesmos limites
    this->inferiorLimit = 0.0f;      // sempre 0.0f

    for (int i = 0; i < this->quantidadeClusters; ++i) {
        //        this->inserirCluster(this->inferiorLimit,this->upperLimit);
    }

    for (int i = 0; i < quantidadeNos; ++i) {
        getline(fileIn, linha, '\n');
        this->createNodeIfDoesntExist(i, stof(linha));
    }

    this->matrizDistancia.reserve(quantidadeNos);
    for (int i = 0; i < quantidadeNos; ++i) {
        std::vector<float> aux;
        for (int j = 0; j < quantidadeNos; ++j) {
            getline(fileIn, linha, ' ');
            aux.push_back(stof(linha));
        }
        this->matrizDistancia.push_back(aux);
    }
    // imprimeMatrizParaDebug(matrizDistancia);
}

template <typename T>
void Graph::imprimeMatrizParaDebug(const std::vector<std::vector<T>>& matriz) {
    std::unitbuf(cout);
    for (int i = 0; i < matriz.size(); ++i) {
        for (int j = 0; j < matriz[i].size(); ++j) {
            cout << matriz[i][j] << " ";
        }
        cout << endl;
    }
    // restore buf to cout
    std::unitbuf(cout);
}

void Graph::criaArestas() {
    for (int i = 0; i < this->getCounterOfNodes(); i++) {
        for (int j = 0; j < this->getCounterOfNodes(); j++) {
            if (matrizDistancia[i][j] != 0) {
                this->createEdge(this->getNodeIfExist(i), this->getNodeIfExist(j), this->matrizDistancia[i][j]);
            }
        }
    }
}

void Graph::guloso(vector<pair<int, int>> limitClusters) {
    vector<Graph*> solucao;
    bool* nosVisitados = new bool[getCounterOfNodes()];
    int contNosVisitados = 0;

    for (int i = 0; i < getCounterOfNodes(); i++)
        nosVisitados[i] = false;

    // criando os clusters da solucao
    for (int i = 0; i < this->quantidadeClusters; i++) {
        pair<int, int> limitCluster = limitClusters.at(i);
        Graph* cluster = new Graph(limitCluster.first, limitCluster.second);
        solucao.push_back(cluster);
    }

    srand(time(0));

    // sorteando aleatoriamente um nó para iniciar cada cluster
    for (int i = 0; i < this->quantidadeClusters; i++) {
        int idRand = rand() % getCounterOfNodes();
        Node* node = getNodeIfExist(idRand);

        if (node == nullptr || nosVisitados[idRand] == true) {
            i--;
            continue;
        }

        nosVisitados[idRand] = true;
        contNosVisitados++;

        Graph* cluster = solucao[i];
        cluster->createNodeIfDoesntExist(idRand, node->getWeight());
        cluster->setLimit(node->getWeight());
        cout << "- " << idRand << endl;
    }

    // ordenando a matriz de distancia da aresta de maior beneficio para a de menor
    vector<vector<float>> matrizAux = this->matrizDistancia;
    priority_queue<pair<float, pair<int, int>>> listaCandidatos;  // distancia, par(nocabeça, nocauda)

    for (int i = 0; i < getCounterOfNodes(); i++) {
        for (int j = 0; j < getCounterOfNodes(); j++) {
            listaCandidatos.push(make_pair(matrizAux[i][j], make_pair(i, j)));
        }
    }

    /*while (!listaCandidatos.empty()) {
        cout << listaCandidatos.top().first << ": " << listaCandidatos.top().second.first << " , " << listaCandidatos.top().second.second << endl;
        listaCandidatos.pop();
    }*/

    // adicionando os demais nós aos clusters respeitando os limites
    while (contNosVisitados < this->getCounterOfNodes() || !listaCandidatos.empty()) {
        if (contNosVisitados >= this->getCounterOfNodes())
            break;

        pair<float, pair<int, int>> candidato = listaCandidatos.top();
        float distancia = candidato.first;
        pair<int, int> parDeNo = candidato.second;
        listaCandidatos.pop();
        // listaCandidatos.pop();

        if (nosVisitados[parDeNo.first] == true && nosVisitados[parDeNo.second] == true)
            continue;

        if (nosVisitados[parDeNo.first] == false && nosVisitados[parDeNo.second] == false)
            continue;

        for (int i = 0; i < this->quantidadeClusters; i++) {
            Graph* cluster = solucao[i];

            Node* noGrafo = cluster->getNodeIfExist(parDeNo.first);
            Node* noExterno = getNodeIfExist(parDeNo.second);

            if (noGrafo == nullptr) {
                noGrafo = cluster->getNodeIfExist(parDeNo.second);
                noExterno = getNodeIfExist(parDeNo.first);
            }

            if (cluster->getLimit() + noExterno->getWeight() > cluster->upperLimit)
                continue;

            cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
            cluster->setLimit(noExterno->getWeight());
            nosVisitados[noExterno->getId()] = true;
            contNosVisitados++;

            cout << "- " << noExterno->getId() << endl;

            break;
        }
    }
    // cout << "nos: " << contNosVisitados << " " << getCounterOfNodes();
    //  imprimeMatrizParaDebug(matrizAux);

    imprimeCluster(solucao);
}

void Graph::imprimeCluster(vector<Graph*> solucao) {
    cout << "===============IMPRIME CLUSTER ===================" << endl;
    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph* cluster = solucao[i];

        cluster->printNodes();

        cout << endl;
    }
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

    cout << "Limite: " << getLimit() << endl;

    while (node != nullptr) {
        cout << node->getId() << ", ";
        node = node->getNextNode();
        cont++;
    }

    // cout << "Fim" << endl;
}