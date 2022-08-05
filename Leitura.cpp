//
// Created by Daniel on 03/08/2022.
//
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

using namespace std;

streampos inline tamanhoArquivo(fstream &arq)
{
    arq.seekg(0, std::fstream::end);
    streampos tam = arq.tellg();
    arq.seekg(0);
    return tam;
}

void processaPrimeiraLinha(const string &linha)
{
    // divide a linha em palavras para ser possível separar os dados
    stringstream ss(linha);
    string item;
    vector<string> tokens;
    while (getline(ss, item, ' '))
    {
        tokens.push_back(item);
    }
    // pega o basico
    int quantidadeNos = stoi(tokens[0]);
    int quantidadeClusters = stoi(tokens[1]);
    int clusterType = stoi(tokens[2]);

    // get limites dos clusters
    vector<pair<int, int>> clustersLimites; //clustersLimites[i] = (limiteInferior, limiteSuperior)
    for (int i = 0; i < quantidadeClusters; i++)
    {
        int clusterId = stoi(tokens[3 + i * 2]);
        int clusterLimite = stoi(tokens[4 + i * 2]);
        clustersLimites.emplace_back(clusterId, clusterLimite);
    }
    // get node weights
    vector<int> nodeWeights;
    for (int i = 0; i < quantidadeNos; i++)
    {
        int nodeId = stoi(tokens[4 + quantidadeClusters * 2 + i]);
        nodeWeights.push_back(nodeId);
    }

    // TODO: separou TUDO, mas nao faz nada com os dados ainda, os dados se perdem,
    // TODO: pensar em como guardar esses dados e usa-los depois

}

void LeituraRanRealeSparse(const string &path_file)
{
    fstream arquivoEntrada(static_cast<string>(path_file), ios::in);
    if (!arquivoEntrada.is_open())
    {
        cerr << "\n\t[ERRO] arquivo nao pode ser aberto lerArquivo";
        exit(10);
    }

    auto bufferSize = tamanhoArquivo(arquivoEntrada);
    unique_ptr<char[]> buffer(new char[bufferSize]);
    arquivoEntrada.read(buffer.get(), bufferSize);
    arquivoEntrada.close();
    stringstream fileIn(buffer.get()); // os dados estao aqui
    string primeiraLinha;
    getline(fileIn, primeiraLinha);

    processaPrimeiraLinha(primeiraLinha);

    string linha;
    int verticeFonte = 0, verticeAlvo = 0, beneficio = 0;
    while (!fileIn.eof())
    {
        getline(fileIn, linha, ' ');
        verticeFonte = stoi(linha);

        getline(fileIn, linha, ' ');
        verticeAlvo = stoi(linha);

        getline(fileIn, linha, '\n');
        beneficio = stoi(linha);
        // TODO: inserir no grafo
    }
}

void ReadHandover(const string &path_file)
{
    fstream arquivoEntrada(static_cast<string>(path_file), ios::in);
    if (!arquivoEntrada.is_open())
    {
        cerr << "\n\t[ERRO] arquivo nao pode ser aberto lerArquivo";
        exit(10);
    }

    auto bufferSize = tamanhoArquivo(arquivoEntrada);
    unique_ptr<char[]> buffer(new char[bufferSize]);
    arquivoEntrada.read(buffer.get(), bufferSize);
    arquivoEntrada.close();
    stringstream fileIn(buffer.get()); // os dados estao aqui

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