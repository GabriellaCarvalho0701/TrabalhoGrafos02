#include "Graph.h"

#include <math.h>
#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Timer/Timer.h"

using std::cout;
using std::endl;
using std::fstream;

using std::cin;
using std::greater;
using std::list;
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::queue;
using std::string;
using std::vector;

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;

Graph::Graph(size_t argc, char **argv) : nodesTotal(0), edgesTotal(0), firstNode(nullptr), inferiorLimit(-1), upperLimit(-1), currentLimit(0) {
    if (argc != 4) {
        std::cerr
            << "[ERRO] parametros do programa incorretos\nEsperado: ./execGrupoX <arquivo_entrada> <arquivo_saida> <Tipo_Instancia>\n";
        exit(-1);
    }
    pathArquivoEntrada = static_cast<string>(argv[1]);
    pathArquivoSaida = static_cast<string>(argv[2]);
    tipoInstancia = std::stoi(argv[3]);
}

Graph::Graph(int inferiorLimit, int upperLimit) : tipoInstancia(-1) {
    this->totalBeneficio = 0;
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

void Graph::setFirstNode(Node *node) {
    firstNode = node;
}

Node *Graph::getFirstNode() {
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
Node *Graph::getNodeIfExist(int id) {
    Node *node = this->firstNode;

    if (node == nullptr) {
        return nullptr;
    }

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
Node *Graph::createNodeIfDoesntExist(int id, float weight) {
    Node *node = this->getNodeIfExist(id);

    if (node == nullptr) {
        Node *newNode = new Node(id, weight, this);
        Node *oldFirstNode = firstNode;
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
Edge *Graph::createEdge(Node *nodeHead, Node *nodeTail, float weight) {
    Edge *newEdge = new Edge(nodeHead, nodeTail, weight, this);
    this->vectorOfEdges.emplace_back(newEdge);

    Edge *nodeFirstEdge = nodeHead->getFirstEdge();

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

long inline tamanhoArquivo(fstream &arq) {
    arq.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = arq.gcount();
    arq.clear();
    arq.seekg(0, std::ios_base::beg);

    return length;
}

/*
 * Função para a leitura de arquivo
 *@params:
 *
 *@return: um vetor com os limites inferior e superior de cada cluster que deve ter na solução
 ****************************************************************/
vector<pair<int, int>> Graph::leituraArquivo() {
    fstream arquivoEntrada(static_cast<string>(this->pathArquivoEntrada), std::ios::in);
    if (!arquivoEntrada.is_open()) {
        std::cerr << "\n\t[ERRO] arquivo nao pode ser aberto lerArquivo";
        exit(10);
    }

    auto bufferSize = tamanhoArquivo(arquivoEntrada);
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    arquivoEntrada.read(buffer.get(), bufferSize);
    arquivoEntrada.close();
    std::stringstream fileIn(buffer.get());
    vector<pair<int, int>> limiteClusters;

    if (this->tipoInstancia == 1)  // RanReal e Sparse
    {
        limiteClusters = this->leituraRanRealeSparse(fileIn);
    } else if (this->tipoInstancia == 2)  // Handover
    {
        limiteClusters = this->leituraHandover(fileIn);
    } else {
        std::cerr << "\n[ERRO] tipo de instancia nao reconhecido\n(1)RanReal ou Sparse\n(2)Handover\n";
        exit(-1);
    }

    criaArestas();
    for (int i = 0; i < this->nodesTotal; ++i) {
        this->matrizDistancia[i][i] = 0.0f;
    }
    return limiteClusters;
}

/*
 * Função para a leitura de arquivo do tipo RanReal ou Sparse
 *@params: std::stringstream &fileIn: passa o arquivo onde estão os dados
 *
 *@return: um vetor com os limites inferior e superior de cada cluster que deve ter na solução
 ****************************************************************/
vector<pair<int, int>> Graph::leituraRanRealeSparse(std::stringstream &fileIn) {
    string primeiraLinha;
    getline(fileIn, primeiraLinha);
    vector<pair<int, int>> limitClusters;

    limitClusters = processaPrimeiraLinhaRanRealSparse(primeiraLinha);  // resgata informacoes basicas do grafo

    // resize matriz distancia
    this->matrizDistancia.resize(this->getCounterOfNodes(), vector<float>(this->getCounterOfNodes(), 0.0f));

    // processo restante das arestas
    string linha;
    int verticeFonte = 0, verticeAlvo = 0;
    float beneficio = 0.0f;
    while (getline(fileIn, linha, '\n') && !fileIn.eof()) {
        if (linha.empty()) {
            break;
        }

        std::stringstream linhaStream(linha);
        linhaStream >> verticeFonte;
        linhaStream >> verticeAlvo;
        linhaStream >> beneficio;
        matrizDistancia[verticeFonte][verticeAlvo] = beneficio;
        matrizDistancia[verticeAlvo][verticeFonte] = beneficio;
    }

    // imprimeMatrizParaDebug(this->matrizDistancia);
    return limitClusters;
}

/*
 * Função para a leitura da primeira linha do arquivo do tipo RanReal ou Sparse
 *@params: std::stringstream &fileIn: passa o arquivo onde estão os dados
 *
 *@return: um vetor com os limites inferior e superior de cada cluster que deve ter na solução
 ****************************************************************/
vector<pair<int, int>> Graph::processaPrimeiraLinhaRanRealSparse(const string &linha) {
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
    this->clusterType = tokens[2];  //  ds ou ss

    // get limites dos clusters
    vector<pair<int, int>> limitClusters;  // limitClusters[i] = (limiteInferior, limiteSuperior)
    for (int i = 0; i < quantidadeClusters; i++) {
        int clusterId = stoi(tokens[3 + i * 2]);
        int clusterLimite = stoi(tokens[4 + i * 2]);
        limitClusters.emplace_back(clusterId, clusterLimite);
    }

    // get node weights
    for (int i = 0; i < nodesTotalOriginal; i++) {
        float nodeWeight = stof(tokens[4 + quantidadeClusters * 2 + i]);
        this->createNodeIfDoesntExist(i, nodeWeight);
        this->vertices.emplace_back(i, nodeWeight);
    }

    return limitClusters;
}

/*
 * Função para a leitura do arquivo do tipo Handover
 *@params: std::stringstream &fileIn: passa o arquivo onde estão os dados
 *
 *@return: um vetor com os limites inferior e superior de cada cluster que deve ter na solução
 ****************************************************************/
vector<pair<int, int>> Graph::leituraHandover(std::stringstream &fileIn) {
    string linha;
    getline(fileIn, linha, '\n');
    int quantidadeNos = stoi(linha);

    getline(fileIn, linha, '\n');
    this->quantidadeClusters = stoi(linha);

    getline(fileIn, linha, '\n');
    this->upperLimit = stof(linha);  // todos tem os mesmos limites
    this->inferiorLimit = 0.0f;      // sempre 0.0f

    vector<pair<int, int>> limitClusters(this->quantidadeClusters, {this->inferiorLimit,
                                                                    this->upperLimit});  // limitClusters[i] = (limiteInferior, limiteSuperior)

    for (int i = 0; i < quantidadeNos; ++i) {
        getline(fileIn, linha, '\n');
        this->vertices.emplace_back(i, stof(linha));
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

    return limitClusters;
}

/*
 * Função para a criação de todas as arestas do grafo base
 *@params:
 *
 *@return:
 ****************************************************************/
void Graph::criaArestas() {
    for (int i = 0; i < this->getCounterOfNodes(); i++) {
        for (int j = 0; j < this->getCounterOfNodes(); j++) {
            if (matrizDistancia[i][j] != 0) {
                this->createEdge(this->getNodeIfExist(i), this->getNodeIfExist(j), this->matrizDistancia[i][j]);
            }
        }
    }
}

/*
 * Função para inicializar a lista de nós candidatos
 *@params:
 *
 *@return:
 ****************************************************************/
void Graph::inicializaListaDeCandidatos() {
    this->listaDeCandidatos = new list<int>;
    for (int i = 0; i < this->getCounterOfNodes(); i++) {
        listaDeCandidatos->push_back(i);
    }
}

/*
 * Função para retornar um número aleatório entre os valores máximo e mínimo passados
 *@params:  float min: valor mínimo que o número aleatório deve ter
            float max: valor máximo que o número aleatório deve ter
 *
 *@return: retorna um float com o número aleatório
 ****************************************************************/
float returnRandomFloat(float min, float max) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

/*
 * Função para retornar um ponteiro para um nó candidato com o id sorteado aleatoriamente
 *@params:  float min: valor mínimo que o id do nó deve ter
            float max: valor máximo que o id do nó deve ter
 *
 *@return: retorna um ponteiro para o nó com o id do número aleatório retornado
 ****************************************************************/
Node *Graph::retornaNoValidoDosCandidatos(float min, float max) {
    int idRandom = returnRandomFloat(min, max);

    // find the node with at idRandom
    int i = 0;
    for (auto it = this->listaDeCandidatos->begin(); it != this->listaDeCandidatos->end(); ++it) {
        if (i == idRandom) {
            auto node = this->getNodeIfExist(*it);
            this->listaDeCandidatos->erase(it);
            return node;
        }
        i++;
    }
}

/*
 * Função que implementa o algoritmo guloso com sua eurística
 *@params:  vector<pair<int, int>> limitClusters: vetor com os pares de limite inferior e superior de cada cluster
            float *result: beneficio total que a solução fornece
 *
 *@return: retorna um vetor de grafos, onde cada grafo representa o cluster
 ****************************************************************/
vector<Graph *> Graph::guloso(vector<pair<int, int>> limitClusters, float *result) {
    vector<Graph *> solucao;  // vetor de grafos, onde cada grafo representa o cluster
    *result = 0.0f;
    vector<bool> nosVisitados(getCounterOfNodes(), false);                                                             // vetor que marca os nós que já foram incluídos em algum cluster
    int contNosVisitados = 0;                                                                                          // contador que marca quantos nós que já foram incluídos em algum cluster
    vector<vector<bool>> arestasVisitadas(this->getCounterOfNodes(), vector<bool>(this->getCounterOfNodes(), false));  // vetor que marca a aresta cujo seu beneficio já foi somado ao beneficio total da solução

    float resultBeneficio = 0.0f, beneficio = 0.0f;

    // criando os clusters da solucao
    for (int i = 0; i < this->quantidadeClusters; i++) {
        pair<int, int> limitCluster = limitClusters.at(i);
        Graph *cluster = new Graph(limitCluster.first, limitCluster.second);
        solucao.push_back(cluster);
    }

    inicializaListaDeCandidatos();

    // sorteando aleatoriamente um nó para iniciar cada cluster
    for (int i = 0; i < this->quantidadeClusters; i++) {
        auto node = this->getNodeIfExist(i);

        int position = node->getId();

        if (node == nullptr || nosVisitados[position] == true) {
            i--;
            continue;
        }

        nosVisitados[position] = true;
        contNosVisitados++;

        Graph *cluster = solucao[i];
        cluster->createNodeIfDoesntExist(position, node->getWeight());
        cluster->setLimit(node->getWeight());
    }

    // adicionando os demais nós aos clusters respeitando os limites
    priority_queue<pair<float, pair<int, int>>> listaCandidatos;

    // while que garante que os clusters tenham o limite inferior
    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph *cluster = solucao[i];
        int idAux = cluster->getFirstNode()->getId();

        while (cluster->getLimit() < cluster->inferiorLimit || listaCandidatos.empty()) {
            // ordenando a lista de candidatos da aresta de maior beneficio para a de menor de acordo com o nó que entra no cluster
            for (int j = 0; j < getCounterOfNodes(); j++) {
                if (!nosVisitados[j]) {
                    listaCandidatos.push(make_pair(matrizDistancia[idAux][j], make_pair(idAux, j)));
                }
            }

            // selecionando a aresta de melhor beneficio, com seus nós origem e destino
            pair<float, pair<int, int>> candidato = listaCandidatos.top();
            float distancia = candidato.first;
            pair<int, int> parDeNo = candidato.second;
            listaCandidatos.pop();

            Node *noGrafo = cluster->getNodeIfExist(parDeNo.first);
            Node *noExterno = getNodeIfExist(parDeNo.second);

            if (noGrafo == nullptr) {
                noGrafo = cluster->getNodeIfExist(parDeNo.second);
                noExterno = getNodeIfExist(parDeNo.first);
            }

            // verifica se o nó pode entrar na solução do cluster sem ferir seus limites
            if (
                cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit &&
                nosVisitados[noExterno->getId()] == false) {
                cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                cluster->totalBeneficio += distancia;
                arestasVisitadas[noExterno->getId()][noGrafo->getId()] = true;
                arestasVisitadas[noGrafo->getId()][noExterno->getId()] = true;

                Node *noCluster = cluster->getFirstNode();
                while (noCluster != nullptr) {
                    if (arestasVisitadas[noExterno->getId()][noCluster->getId()] == false &&
                        arestasVisitadas[noCluster->getId()][noExterno->getId()] == false) {
                        cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                        resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                    }
                    noCluster = noCluster->getNextNode();
                }

                resultBeneficio += distancia;

                cluster->setLimit(noExterno->getWeight());
                nosVisitados[noExterno->getId()] = true;
                contNosVisitados++;
                idAux = noExterno->getId();
            }
        }
    }

    // atualiza a lista de candidatos com os nós que ainda não estão nos clusters
    for (int i = 0; i < getCounterOfNodes(); i++) {
        for (int j = 0; j < getCounterOfNodes(); j++) {
            if (nosVisitados[i] == false || nosVisitados[j] == false) {
                listaCandidatos.push(make_pair(matrizDistancia[i][j], make_pair(i, j)));
            }
        }
    }

    // while que garante que todos os nós estão nos cluters e garante que os clusters tenham até o limite superior
    while (contNosVisitados < this->getCounterOfNodes() && !listaCandidatos.empty()) {
        pair<float, pair<int, int>> candidato = listaCandidatos.top();
        float distancia = candidato.first;
        pair<int, int> parDeNo = candidato.second;
        listaCandidatos.pop();

        // verifica se os dois nós já não estão na solução
        if (
            !(nosVisitados[parDeNo.first] == true && nosVisitados[parDeNo.second] == true) &&
            !(nosVisitados[parDeNo.first] == false && nosVisitados[parDeNo.second] == false)) {
            for (int i = 0; i < this->quantidadeClusters; i++) {
                Graph *cluster = solucao[i];

                Node *noGrafo = cluster->getNodeIfExist(parDeNo.first);
                Node *noExterno = getNodeIfExist(parDeNo.second);

                if (noGrafo == nullptr) {
                    noGrafo = cluster->getNodeIfExist(parDeNo.second);
                    noExterno = getNodeIfExist(parDeNo.first);
                }

                // verifica se o nó pode entrar na solução do cluster
                if (
                    cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit &&
                    nosVisitados[noExterno->getId()] == false) {
                    cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                    cluster->totalBeneficio += distancia;
                    resultBeneficio += distancia;

                    arestasVisitadas[parDeNo.first][parDeNo.second] = true;
                    arestasVisitadas[parDeNo.second][parDeNo.first] = true;

                    Node *noCluster = cluster->getFirstNode();
                    while (noCluster != nullptr) {
                        if (arestasVisitadas[noExterno->getId()][noCluster->getId()] == false &&
                            arestasVisitadas[noCluster->getId()][noExterno->getId()] == false) {
                            cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                            resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                        }

                        noCluster = noCluster->getNextNode();
                    }

                    cluster->setLimit(noExterno->getWeight());
                    nosVisitados[noExterno->getId()] = true;
                    contNosVisitados++;
                }
            }
        }
    }

    // imprimeCluster(solucao, 1, resultBeneficio);

    *result = resultBeneficio;
    delete this->listaDeCandidatos;
    return solucao;
}

/*
 * Função que implementa o algoritmo guloso randomizado com sua eurística
 *@params:  vector<pair<int, int>> limitClusters: vetor com os pares de limite inferior e superior de cada cluster
            float *result: beneficio total que a solução fornece
 *          float alfa: valor de alfa passado
 *
 *@return: retorna um vetor de grafos, onde cada grafo representa o cluster
 ****************************************************************/
vector<Graph *> Graph::gulosoRandomizado(vector<pair<int, int>> limitClusters, float *result, float alfa) {
    vector<Graph *> solucao;        // vetor de grafos, onde cada grafo representa o cluster
    vector<Node *> nosFracassados;  // aqueles que fracassaram em entrar em algum momento no cluster
    *result = 0.0f;
    vector<bool> nosVisitados(getCounterOfNodes(), false);                                                             // vetor que marca os nós que já foram incluídos em algum cluster
    int contNosVisitados = 0;                                                                                          // contador que marca quantos nós que já foram incluídos em algum cluster
    vector<vector<bool>> arestasVisitadas(this->getCounterOfNodes(), vector<bool>(this->getCounterOfNodes(), false));  // vetor que marca a aresta cujo seu beneficio já foi somado ao beneficio total da solução

    float resultBeneficio = 0.0f, beneficio = 0.0f;

    // criando os clusters da solucao
    for (int i = 0; i < this->quantidadeClusters; i++) {
        pair<int, int> limitCluster = limitClusters.at(i);
        Graph *cluster = new Graph(limitCluster.first, limitCluster.second);
        solucao.push_back(cluster);
    }

    inicializaListaDeCandidatos();

    // sorteando aleatoriamente um nó para iniciar cada cluster
    for (int i = 0; i < this->quantidadeClusters; i++) {
        auto node = retornaNoValidoDosCandidatos(0.0f, alfa * (this->getCounterOfNodes() - 1));

        int position = node->getId();
        if (node == nullptr || nosVisitados[position] == true) {
            i--;
            continue;
        }
        nosVisitados[position] = true;
        contNosVisitados++;

        Graph *cluster = solucao[i];
        cluster->createNodeIfDoesntExist(position, node->getWeight());
        cluster->setLimit(node->getWeight());
    }

    // while que garante que todos os nós estejam nas soluções
    while (contNosVisitados < this->getCounterOfNodes()) {
        // for que garante que todos os clusters sejam povoados
        for (int i = 0; i < this->quantidadeClusters && contNosVisitados < this->getCounterOfNodes(); i++) {
            Graph *cluster = solucao[i];
            vector<pair<float, pair<int, int>>> listaCandidatos;

            int clusterNoId = cluster->getFirstNode()->getId();

            for (int j = 0; j < this->getCounterOfNodes(); j++) {
                if (!nosVisitados[j]) {
                    listaCandidatos.push_back(make_pair(matrizDistancia[clusterNoId][j], make_pair(clusterNoId, j)));
                }
            }

            sort(listaCandidatos.begin(), listaCandidatos.end(), greater<>());

            // escolhe uma posição aleatória entre o 0 e o alfa
            int position = returnRandomFloat(0.0f, alfa * (listaCandidatos.size() - 1));

            // seleciona o candidato de acordo com a posição anterior
            pair<float, pair<int, int>> candidate = listaCandidatos[position];
            float benefit = candidate.first;
            pair<int, int> nodePair = candidate.second;

            // remove o elemento da lista
            listaCandidatos.erase(listaCandidatos.begin() + position);

            Node *graphNode = cluster->getNodeIfExist(nodePair.first);
            Node *noExterno = getNodeIfExist(nodePair.second);

            if (graphNode == nullptr) {
                graphNode = cluster->getNodeIfExist(nodePair.second);
                noExterno = getNodeIfExist(nodePair.first);
            }

            // verifica se o nó está apto a entrar no cluster
            if (
                cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit &&
                nosVisitados[noExterno->getId()] == false) {
                cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                cluster->setLimit(noExterno->getWeight());

                cluster->totalBeneficio += benefit;
                resultBeneficio += benefit;

                arestasVisitadas[nodePair.first][nodePair.second] = true;
                arestasVisitadas[nodePair.second][nodePair.first] = true;

                Node *noCluster = cluster->getFirstNode();
                while (noCluster != nullptr) {
                    cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                    resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                    noCluster = noCluster->getNextNode();
                }

                nosVisitados[noExterno->getId()] = true;
                contNosVisitados++;
            } else {
                int contFracassos = 0;
                for (auto cluster : solucao) {
                    if (
                        cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit &&
                        nosVisitados[noExterno->getId()] == false) {
                        cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                        cluster->setLimit(noExterno->getWeight());

                        cluster->totalBeneficio += benefit;
                        resultBeneficio += benefit;

                        arestasVisitadas[nodePair.first][nodePair.second] = true;
                        arestasVisitadas[nodePair.second][nodePair.first] = true;

                        Node *noCluster = cluster->getFirstNode();
                        while (noCluster != nullptr) {
                            cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                            resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                            noCluster = noCluster->getNextNode();
                        }

                        nosVisitados[noExterno->getId()] = true;
                        contNosVisitados++;
                    } else {
                        contFracassos++;
                    }
                }
                if (contFracassos == this->quantidadeClusters) {
                    delete this->listaDeCandidatos;
                    return vector<Graph *>();
                }
            }
            if (listaCandidatos.empty()) {
                break;
            }
        }
    }
    delete this->listaDeCandidatos;

    *result = resultBeneficio;

    bool fracassou = false;
    int contador = 0;
    for (int i = 0; i < nosFracassados.size(); ++i) {
        for (auto cluster : solucao) {
            if (cluster->getNodeIfExist(nosFracassados[i]->getId()) == nullptr) {
                ++contador;
            }
        }
        if (contador == solucao.size()) {
            fracassou = true;
            break;
        }
    }

    if (fracassou) {
        cout << "";
    }
    return (fracassou) ? vector<Graph *>() : solucao;

    return solucao;
}

/*
 * Função para chamar o algoritmo guloso
 *@params:  vector<pair<int, int>> limitClusters: vetor com os pares de limite inferior e superior de cada cluster
 *
 *@return: imprime no arquivo de saída a solução
 ****************************************************************/
void Graph::algGuloso(vector<pair<int, int>> limitClusters) {
    Timer *timer = new Timer("Tempo de Execucao: ");

    float result = 0;

    vector<Graph *> sol = guloso(limitClusters, &result);

    cout << std::setprecision(2) << std::fixed;
    delete timer;
    cout << "Beneficio da Solucao: " << result << endl;
    if (result > 0) {
        cout << "Conseguiu alguma solucao viavel" << endl;
    } else {
        cout << "Nao conseguiu nenhuma solucao viavel" << endl;
    }

    // imprimeCluster(sol, 1, result);
    output(sol, result);
}

/*
 * Função para chamar o algoritmo guloso randomizado adaptativo
 *@params:  vector<pair<int, int>> limitClusters: vetor com os pares de limite inferior e superior de cada cluster
 *
 *@return: imprime no arquivo de saída a solução
 ****************************************************************/
void Graph::algGulosoRandAdapt(vector<pair<int, int>> limitClusters) {
    Timer *timer = new Timer("Tempo de Execucao: ");

    float maior = 0;
    float result, alfa;
    int numIt = 500;

    cout << "Escolha um alfa: " << endl;
    std::cin >> alfa;

    // cout << "Escolha quantas interações: " << endl;
    // cin >> numIt;

    vector<Graph *> sol, melhorSol;

    cout << "Alfa de randomizacao: " << alfa << endl;

    for (int i = 0; i < numIt; i++) {
        sol = gulosoRandomizado(limitClusters, &result, alfa);
        if (result > maior) {
            maior = result;
            melhorSol = sol;
        }
    }

    // imprimeCluster(sol, 1, maior);

    cout << std::setprecision(2) << std::fixed;
    delete timer;
    cout << "Beneficio da Melhor Solucao: " << maior << endl;
    if (maior > 0) {
        cout << "Conseguiu alguma solucao viavel" << endl;
    } else {
        cout << "Nao conseguiu nenhuma solucao viavel" << endl;
    }

    output(melhorSol, maior);
}

// estrutura auxiliar para o algoritmo guloso reativo
struct media {
    float soma;
    float numSolucoes;
    float media;
};

/*
 * Função para inicializar os vetores do algoritmo guloso reativo
 *@params:  vector<media> &medias: vetor de médias usado no algoritmo guloso reativo
 *          vector<float> &prob: vetor de probabilidades usado no algoritmo guloso reativo
 *          int numAlfas: número de alfas estipulado
 * Obs: usamos o alfa do slide como 2
 *@return:
 ****************************************************************/
void inicializaVetores(vector<float> &prob, vector<media> &medias, int numAlfas) {
    media aux{0, 0, 1};
    auto auxNumAlfas = numAlfas;

    float auxProb = 1.0f / static_cast<float>(auxNumAlfas);

    for (int i = 0; i < numAlfas; i++) {
        prob.push_back(auxProb);
        medias.push_back(aux);
    }
}

/*
 * Função para atualizar o vetor de probabilidades a cada fim de bloco estipulado no algoritmo guloso reativo
 *@params:  vector<media> &medias: vetor de médias usado no algoritmo guloso reativo
 *          vector<float> &prob: vetor de probabilidades usado no algoritmo guloso reativo
 *          vector<float> &solBest: vetor com as melhores soluções encontradas
 *          vector<float> &q: vetor auxiliar para o calculo da probabilidade
 * Obs: usamos o alfa do slide como 2
 *@return:
 ****************************************************************/
void atualizaProbabilidades(vector<media> &medias, vector<float> &prob, vector<float> &solBest, vector<float> &q) {
    float somaQ = 0;
    float melhorSolucao = *(max_element(solBest.begin(), solBest.end()));

    for (int i = 0; i < medias.size(); i++) {
        q[i] = pow((melhorSolucao / medias[i].media), 2);
        somaQ += q[i];
    }

    for (int i = 0; i < medias.size(); i++) {
        prob[i] = q[i] / somaQ;
    }
}

/*
 * Função para escolher o alfa a cada iteração do algoritmo guloso reativo
 *@params:  vector<float> &prob: vetor de probabilidades usado no algoritmo guloso reativo
 *          vector<float> &alfas: vetor de alfas estipulado no algoritmo guloso reativo
 *
 *@return: retorna o float alfa de acordo com a probabilidade
 ****************************************************************/
float escolheAlfa(vector<float> &prob, vector<float> &alfas) {
    float soma = 0;
    int r = rand() % 101;

    for (int i = 0; i < prob.size(); i++) {
        soma += (prob[i] * 100);
        if (soma >= r) {
            return alfas[i];
        }
    }

    return alfas[alfas.size() - 1];
}

/*
 * Função para atualizar o vetor de médias a cada iteração do algoritmo guloso reativo
 *@params:  vector<media> &medias: vetor de médias usado no algoritmo guloso reativo
 *          float solucao: maior beneficio encontrado pelo algoritmo
 *          vector<float> &alfas: vetor de alfas estipulado no algoritmo guloso reativo
 *          float alfa: alfa escolhido naquela interação
 *
 *@return:
 ****************************************************************/
void atualizaMedias(vector<media> &medias, float solucao, vector<float> &alfas, float alfa) {
    int aux = 0;
    float auxSolucao;

    auxSolucao = solucao;

    for (int i = 0; i < alfas.size(); i++) {
        if (alfa == alfas[i]) {
            aux = i;
            break;
        }
    }

    medias[aux].soma = medias[aux].soma + auxSolucao;
    medias[aux].numSolucoes++;
    medias[aux].media = medias[aux].soma / medias[aux].numSolucoes;
}

/*
 * Função para chamar o algoritmo guloso randomizado reativo
 *@params:  vector<pair<int, int>> limitClusters: vetor com os pares de limite inferior e superior de cada cluster
 *
 *@return: imprime no arquivo de saída a solução
 ****************************************************************/
void Graph::algGulosoReativo(vector<pair<int, int>> limitClusters) {
    Timer *timer = new Timer("Tempo de Execucao: ");

    vector<float> alfas{0.05f, 0.10f, 0.15f, 0.30f, 0.50f}, solBest, probabilidade, q;
    vector<media> medias;
    vector<Graph *> sol, melhorSol;
    int numIt = 2500;
    int numBloco = 50;
    float solucao, maiorBeneficio = 0;

    //    cout << "Escolha quantas interações: " << endl;
    //    cin >> numIt;
    //    cout << "Escolha o tamanho do bloco: " << endl;
    //    cin >> numBloco;

    for (int i = 0; i < alfas.size(); i++) {
        q.push_back(0.00f);
        solBest.push_back(0.00f);
    }

    inicializaVetores(probabilidade, medias, alfas.size());

    for (int i = 1; i <= numIt; i++) {
        if (i % numBloco == 0) {
            atualizaProbabilidades(medias, probabilidade, solBest, q);
        }

        float alfaEscolhido = escolheAlfa(probabilidade, alfas);

        float result, semente;

        cout << "Semente de randomizacao: " << alfaEscolhido << endl;

        sol = gulosoRandomizado(limitClusters, &result, alfaEscolhido);
        if (sol.empty()) {
            cout << "Nao conseguiu nenhuma solucao viavell" << endl;
        } else {
            cout << "Beneficio: " << result << endl
                 << endl;
        }
        if (result > maiorBeneficio) {
            maiorBeneficio = result;
            melhorSol = sol;
            solBest[alfaEscolhido] = maiorBeneficio;
        } else {
            for (auto &s : sol) {
                delete s;
            }
        }

        atualizaMedias(medias, maiorBeneficio, alfas, alfaEscolhido);
    }

    float auxSolBest = 0;
    for (int i = 0; i < solBest.size(); i++) {
        if (solBest[i] > auxSolBest) {
            auxSolBest = solBest[i];
        }
    }

    cout << std::setprecision(2) << std::fixed;
    delete timer;
    cout << "Beneficio da Melhor Solucao: " << auxSolBest << endl;
    if (auxSolBest > 0) {
        cout << "Conseguiu alguma solucao viavel" << endl;
    } else {
        cout << "Nao conseguiu nenhuma solucao viavel" << endl;
    }

    output(melhorSol, auxSolBest);
    // imprimeCluster(melhorSol, 1, auxSolBest);
}

/*
 * Função para imprimir os clusters em tela
 *@params:  vector<Graph *> solucao: vetor de grafos, onde cada grafo representa o cluster
 *          int option: opção para chamar o printNodes desejado
 *          float resultBeneficio: beneficio total da solução
 *
 *@return: imprime no arquivo de saída a solução
 ****************************************************************/
void Graph::imprimeCluster(vector<Graph *> solucao, int option, float resultBeneficio) {
    float totalBeneficio = 0.0;

    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph *cluster = solucao[i];

        if (option == 2) {
            cout << "===============IMPRIME CLUSTER " << i + 1 << " ===================" << endl;
            cout << "Beneficio " << cluster->totalBeneficio << endl;
            totalBeneficio += cluster->totalBeneficio;
        }

        if (option == 1) {
            cluster->printNodes();
        } else if (option == 2) {
            cluster->printNodes2();
        }

        cout << endl;
    }

    if (option == 2) {
        cout << std::setprecision(2) << std::fixed;
    }
    cout << "\n\nBeneficio final: " << totalBeneficio << endl;
}

/*
 * Função escreve todos os nós da solução no terminal
 *@params:
 *
 *@return:
 ****************************************************************/
void Graph::printNodes2() {
    Node *node = firstNode;
    int cont = 0;

    cout << "Limite | " << this->inferiorLimit << " <= " << this->getLimit() << " <= " << this->upperLimit << ""
         << endl;

    while (node != nullptr) {
        cout << node->getId() << ",";
        node = node->getNextNode();
        cont++;
    }

    // cout << "Fim" << endl;
}

/*
 * Função escreve todos os nós da solução no terminal
 *@params:
 *
 *@return:
 ****************************************************************/
void Graph::printNodes() {
    Node *node = firstNode;
    int cont = 0;

    /*cout << this->inferiorLimit << " <=  ";
    cout << getLimit() << " <= ";
    cout << this->upperLimit << " ? ";

    if (this->inferiorLimit <= getLimit() && getLimit() >= this->inferiorLimit) {
        cout << "Sim." << endl;
    } else {
        cout << "Nao." << endl;
    }*/

    while (node != nullptr) {
        cout << node->getId() << ",";
        node = node->getNextNode();
        cont++;
    }
}

/*
 * Função escreve o grafo no arquivo passado.
 *
 *@params: vector<Graph *> solucao: vetor contendo o clusters com as soluções
 *         float qualidadeSolucao: beneficio da solução
 *@return:
 ****************************************************************/
void Graph::output(vector<Graph *> solucao, float qualidadeSolucao) {
    string outputFileName = static_cast<string>(this->pathArquivoSaida);
    FILE *outfile = fopen(outputFileName.c_str(), "w+");

    string text = string("Solução da Clusterização Capacitada: \n\n");
    fwrite(text.c_str(), 1, text.size(), outfile);

    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph *cluster = solucao[i];

        text = string("===============IMPRIME CLUSTER ") + std::to_string(i + 1) + string(" ===================") +
               "\n";
        fwrite(text.c_str(), 1, text.size(), outfile);

        text = string("Beneficio do Cluster: ") + std::to_string(cluster->totalBeneficio) + "\n";
        fwrite(text.c_str(), 1, text.size(), outfile);

        Node *node = cluster->getFirstNode();

        text = string("Limite | ") + std::to_string(cluster->inferiorLimit) + string(" <= ") +
               std::to_string(cluster->getLimit()) + string(" <= ") + std::to_string(cluster->upperLimit) + ": \n";
        fwrite(text.c_str(), 1, text.size(), outfile);

        text = string("Nós do Cluster:  ");
        fwrite(text.c_str(), 1, text.size(), outfile);

        while (node != nullptr) {
            text = std::to_string(node->getId()) + string(",");
            fwrite(text.c_str(), 1, text.size(), outfile);
            node = node->getNextNode();
        }

        text = string("\n\n");
        fwrite(text.c_str(), 1, text.size(), outfile);
    }
    text = string("Qualidade da Solução: ") + std::to_string(qualidadeSolucao) + " \n\n";
    fwrite(text.c_str(), 1, text.size(), outfile);

    cout << "O arquivo " << outputFileName << " foi gerado com sucesso.";

    for (auto &s : solucao) {
        delete s;
    }
}
