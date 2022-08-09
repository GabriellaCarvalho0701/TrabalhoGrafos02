#include "Graph.h"

#include <time.h>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
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

Graph::Graph(size_t argc, char** argv) : nodesTotal(0), edgesTotal(0), firstNode(nullptr), inferiorLimit(-1), upperLimit(-1), currentLimit(0) {
    if (argc != 4) {
        std::cerr
            << "[ERRO] parametros do programa incorretos\nEsperado: ./execGrupoX <arquivo_entrada> <arquivo_saida> <Tipo_Instancia>\n";
        exit(-1);
    }
    pathArquivoEntrada = static_cast<string>(argv[1]);
    pathArquivoSaida = static_cast<string>(argv[2]);
    tipoInstancia = std::stoi(argv[3]);
}

Graph::Graph(int inferiorLimit, int upperLimit) : tipoInstancia(-1)  // necessario
{
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

long inline tamanhoArquivo(fstream& arq) {
    arq.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = arq.gcount();
    arq.clear();
    arq.seekg( 0, std::ios_base::beg );
    return length;
}

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
    std::stringstream fileIn(buffer.get());  // os dados estao aqui
    vector<pair<int, int>> limiteClusters;

    if (this->tipoInstancia == 1)  // RanReal e Sparse
    {
        limiteClusters = this->leituraRanRealeSparse(fileIn);
    } else if (this->tipoInstancia == 2)  // Handover
    {
        this->leituraHandover(fileIn);
    } else {
        std::cerr << "\n[ERRO] tipo de instancia nao reconhecido\n(1)RanReal ou Sparse\n(2)Handover\n";
        exit(-1);
    }

    criaArestas();
    for (int i = 0; i < this->nodesTotal; ++i)
    {
        this->matrizDistancia[i][i] = 0.0f;
    }
    return limiteClusters;
}

vector<pair<int, int>> Graph::leituraRanRealeSparse(std::stringstream& fileIn) {
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

        // this->createEdge(getNodeIfExist(verticeFonte), getNodeIfExist(verticeAlvo), beneficio);
    }

    // imprimeMatrizParaDebug(this->matrizDistancia);
    return limitClusters;
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
    for (int i = 0; i < nodesTotalOriginal; i++)
    {
        float nodeWeight = stof(tokens[4 + quantidadeClusters * 2 + i]);
        this->createNodeIfDoesntExist(i, nodeWeight);
        this->vertices.emplace_back(i, nodeWeight);
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
    // imprimeMatrizParaDebug(matrizDistancia);
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

vector<Graph*> Graph::guloso(vector<pair<int, int>> limitClusters, bool randomizado, float* result, float semente) {
    vector<Graph*> solucao;
    *result = 0;
    bool* nosVisitados = new bool[getCounterOfNodes()];
    int contNosVisitados = 0;
    vector<vector<bool>> arestasVisitadas;

    arestasVisitadas.resize(this->getCounterOfNodes());
    for (int i = 0; i < this->getCounterOfNodes(); i++) {
        arestasVisitadas[i].resize(this->getCounterOfNodes());
        nosVisitados[i] = false;
    }

    float resultBeneficio = 0;

    float beneficio = 0;

    // criando os clusters da solucao
    for (int i = 0; i < this->quantidadeClusters; i++) {
        pair<int, int> limitCluster = limitClusters.at(i);
        Graph* cluster = new Graph(limitCluster.first, limitCluster.second);
        solucao.push_back(cluster);
    }

    // sorteando aleatoriamente um nó para iniciar cada cluster
    for (int i = 0; i < this->quantidadeClusters; i++) {
        int position = i;
        Node* node = nullptr;

        if (randomizado) {
            position = (int)(rand() % ((int)(semente * getCounterOfNodes())));
            cout << "- " << position << ", ";
        }

        node = getNodeIfExist(position);

        if (node == nullptr || nosVisitados[position] == true) {
            i--;
            continue;
        }

        nosVisitados[position] = true;
        contNosVisitados++;

        Graph* cluster = solucao[i];
        cluster->createNodeIfDoesntExist(position, node->getWeight());
        cluster->setLimit(node->getWeight());
        cout << "-- " << cluster->getFirstNode()->getId() << " " << position << endl;
    }

    cout << endl;

    // adicionando os demais nós aos clusters respeitando os limites
    priority_queue<pair<float, pair<int, int>>> listaCandidatos;

    // while que garante que os clusters tenham o limite inferior
    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph* cluster = solucao[i];
        int idAux = cluster->getFirstNode()->getId();

        while (cluster->getLimit() < cluster->inferiorLimit || listaCandidatos.empty()) {
            // ordenando a lista de candidatos da aresta de maior beneficio para a de menor de acordo com o nó que entra no cluster
            for (int j = 0; j < getCounterOfNodes(); j++) {
                if (!nosVisitados[j])
                    listaCandidatos.push(make_pair(matrizDistancia[idAux][j], make_pair(idAux, j)));
            }

            pair<float, pair<int, int>> candidato = listaCandidatos.top();
            float distancia = candidato.first;
            pair<int, int> parDeNo = candidato.second;
            listaCandidatos.pop();

            Node* noGrafo = cluster->getNodeIfExist(parDeNo.first);
            Node* noExterno = getNodeIfExist(parDeNo.second);

            if (noGrafo == nullptr) {
                noGrafo = cluster->getNodeIfExist(parDeNo.second);
                noExterno = getNodeIfExist(parDeNo.first);
            }

            if (
                cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit && nosVisitados[noExterno->getId()] == false) {
                cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                cluster->totalBeneficio += distancia;
                arestasVisitadas[noExterno->getId()][noGrafo->getId()] = true;
                arestasVisitadas[noGrafo->getId()][noExterno->getId()] = true;

                Node* noCluster = cluster->getFirstNode();
                while (noCluster != nullptr) {
                    if (arestasVisitadas[noExterno->getId()][noCluster->getId()] == false && arestasVisitadas[noCluster->getId()][noExterno->getId()] == false) {
                        cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                        resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                        // cout << noExterno->getId() << " " << noCluster->getId() << endl;
                    }
                    noCluster = noCluster->getNextNode();
                }

                resultBeneficio += distancia;
                // cout << resultBeneficio << " " << cluster->getLimit() << endl;

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
            if (nosVisitados[i] == false || nosVisitados[j] == false)
                listaCandidatos.push(make_pair(matrizDistancia[i][j], make_pair(i, j)));
        }
    }

    // while que garante que todos os nós estão nos cluters e garante que os clusters tenham até o limite superior
    while (contNosVisitados < this->getCounterOfNodes() && !listaCandidatos.empty()) {
        pair<float, pair<int, int>> candidato = listaCandidatos.top();
        float distancia = candidato.first;
        pair<int, int> parDeNo = candidato.second;
        listaCandidatos.pop();

        if (
            !(nosVisitados[parDeNo.first] == true && nosVisitados[parDeNo.second] == true) &&
            !(nosVisitados[parDeNo.first] == false && nosVisitados[parDeNo.second] == false)) {
            for (int i = 0; i < this->quantidadeClusters; i++) {
                Graph* cluster = solucao[i];

                Node* noGrafo = cluster->getNodeIfExist(parDeNo.first);
                Node* noExterno = getNodeIfExist(parDeNo.second);

                if (noGrafo == nullptr) {
                    noGrafo = cluster->getNodeIfExist(parDeNo.second);
                    noExterno = getNodeIfExist(parDeNo.first);
                }

                if (
                    cluster->getLimit() + noExterno->getWeight() <= cluster->upperLimit && nosVisitados[noExterno->getId()] == false) {
                    cluster->createNodeIfDoesntExist(noExterno->getId(), noExterno->getWeight());
                    cluster->totalBeneficio += distancia;
                    resultBeneficio += distancia;

                    arestasVisitadas[parDeNo.first][parDeNo.second] = true;
                    arestasVisitadas[parDeNo.second][parDeNo.first] = true;

                    Node* noCluster = cluster->getFirstNode();
                    while (noCluster != nullptr) {
                        if (arestasVisitadas[noExterno->getId()][noCluster->getId()] == false && arestasVisitadas[noCluster->getId()][noExterno->getId()] == false) {
                            cluster->totalBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                            resultBeneficio += matrizDistancia[noExterno->getId()][noCluster->getId()];
                            // cout << noExterno->getId() << " - " << noCluster->getId() << endl;
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

    // cout << "Total nos: Visitados -> " << contNosVisitados << "; Total ->" << getCounterOfNodes() << endl;
    //     imprimeMatrizParaDebug(matrizAux);

    // imprimeCluster(solucao, 1, resultBeneficio);
    //   imprimeCluster(solucao, 2, resultBeneficio);

    *result = resultBeneficio;
    return solucao;
}

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;

void Graph::algGulosoRandAdapt(vector<pair<int, int>> limitClusters) {
    time_t start, end;
    time(&start);

    float maior = 0;
    float result, semente;
    vector<Graph*> sol;

    srand(time(nullptr));
    for (int i = 0; i < rand(); i++)
        semente = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN)));

    cout << "Semente de randomizacao: " << semente << endl;

    for (int i = 0; i < 1; i++) {
        guloso(limitClusters, 1, &result, semente);
        if (result > maior) {
            maior = result;
            cout << sol.size();
            for (int i = 0; i < sol.size(); i++)
                sol[i] = nullptr;
            sol = guloso(limitClusters, 1, &result, semente);
        }
    }

    imprimeCluster(sol, 2, maior);

    time(&end);

    cout << std::setprecision(2) << std::fixed;
    double time = double(end - start);
    cout << "Tempo de Execucao: " << time << " s" << endl;
    cout << "Melhor Solucao: " << maior << endl;
    cout << "Qualidade Solucao: " << qualidadeSolucao(maior) << "%" << endl;
    if (maior > 0) {
        cout << "Conseguiu alguma solucao viavel" << endl;
    } else {
        cout << "Nao conseguiu nenhuma solucao viavel" << endl;
    }

    output("AlgoritmoGulosoRandomizadoAdaptativo.txt", sol, qualidadeSolucao(maior));
}

void Graph::algGuloso(vector<pair<int, int>> limitClusters) {
    time_t start, end;
    time(&start);

    float result = 0;

    vector<Graph*> sol = guloso(limitClusters, 0, &result, 0);

    time(&end);
    double time = double(end - start);
    cout << std::setprecision(2) << std::fixed;
    cout << "Tempo de Execucao: " << time << " s" << endl;
    cout << "Qualidade Solucao: " << qualidadeSolucao(result) << "%" << endl;
    if (result > 0) {
        cout << "Conseguiu alguma solucao viavel" << endl;
    } else {
        cout << "Nao conseguiu nenhuma solucao viavel" << endl;
    }

    // imprimeCluster(sol, 2, result);
    output("AlgoritmoGuloso.txt", sol, qualidadeSolucao(result));
}

void Graph::algGulosoReativo(vector<pair<int, int>> limitClusters) {
    time_t start, end;
    time(&start);

    vector<float> alfas, solBest, medias, probabilidade, q;

    for (int i = 0; i < rand(); i++)
        alfas.push_back(FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN))));

    alfas.push_back(0.05);
    alfas.push_back(0.10);
    alfas.push_back(0.15);
    alfas.push_back(0.30);
    alfas.push_back(0.50);

    // imprimeCluster(sol, 2, result);
}

float Graph::qualidadeSolucao(float resultadoObtido) {
    float resEsperado;

    if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal240_01.txt") {
        resEsperado = 225003.70;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal240_04.txt") {
        resEsperado = 225683.17;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal240_07.txt") {
        resEsperado = 209305.70;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal480_01.txt") {
        resEsperado = 556126.86;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal480_04.txt") {
        resEsperado = 522790.22;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal960_01.30.txt") {
        resEsperado = 1340369.47;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/Sparse82_02.txt") {
        resEsperado = 1306.64;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/RanReal960_01.30.txt") {
        resEsperado = 1340369.47;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/20_10_270001") {
        resEsperado = 2148.00;
    } else if (this->pathArquivoEntrada == "./EntryFiles/InstanciasTurmaGrafos/30_5_270003") {
        resEsperado = 920.00;
    }

    return ((resultadoObtido * 100) / resEsperado);
}

void Graph::imprimeCluster(vector<Graph*> solucao, int option, float resultBeneficio) {
    float totalBeneficio = 0.0;

    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph* cluster = solucao[i];

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

    if (option == 2)
        cout << std::setprecision(2) << std::fixed;
    cout << "\n\nBeneficio final: " << totalBeneficio << endl;
}

void Graph::printNodes2() {
    Node* node = firstNode;
    int cont = 0;

    cout << "Limite | " << this->inferiorLimit << " <= " << this->getLimit() << " <= " << this->upperLimit << "" << endl;

    while (node != nullptr) {
        cout << node->getId() << ",";
        node = node->getNextNode();
        cont++;
    }

    // cout << "Fim" << endl;
}

void Graph::printNodes() {
    Node* node = firstNode;
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
 * Função escreve o os nós passados em um arquivo
 *@params: string outputFileName: Nome do arquivo que será registrado os nós
 *         Node* nodes[]: conjuntos de nós resposta da operação realizada
 *         int cont: contador de quantos nós há no array nodes
 *         string textStart: texto inicial a ser escrito
 *@return:
 ****************************************************************/
void Graph::output(string outputFileName, vector<Graph*> solucao, float qualidadeSolucao) {
    FILE* outfile = fopen(outputFileName.c_str(), "w+");

    string text = string("Solução da Clusterização Capacitada: \n\n");
    fwrite(text.c_str(), 1, text.size(), outfile);

    for (int i = 0; i < this->quantidadeClusters; i++) {
        Graph* cluster = solucao[i];

        text = string("===============IMPRIME CLUSTER ") + std::to_string(i + 1) + string(" ===================") + "\n";
        fwrite(text.c_str(), 1, text.size(), outfile);

        text = string("Beneficio do Cluster: ") + std::to_string(cluster->totalBeneficio) + "\n";
        fwrite(text.c_str(), 1, text.size(), outfile);

        Node* node = cluster->getFirstNode();

        text = string("Limite | ") + std::to_string(cluster->inferiorLimit) + string(" <= ") + std::to_string(cluster->getLimit()) + string(" <= ") + std::to_string(cluster->upperLimit) + ": \n";
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
    text = string("Qualidade da Solução: ") + std::to_string(qualidadeSolucao) + "% \n\n";
    fwrite(text.c_str(), 1, text.size(), outfile);

    cout << "O arquivo " << outputFileName << " foi gerado com sucesso.";
}

// float Graph::calculateBenefit(){
//     Node* node = this->getFirstNode();
//     Edge* edge = node->getFirstEdge();
//     float weight = 0;

//     while (node != nullptr){
//         while (edge != nullptr) {
//             edge = edge->getNextEdge();
//             weight += edge->getWeight();
//         }
//         node = node->getNextNode();
//     }

//     return weight;
// }
