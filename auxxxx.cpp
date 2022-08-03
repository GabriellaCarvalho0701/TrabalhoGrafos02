// Já cria o grafo lendo do arquivo de entrada

/*Grafo::Grafo(string path, int instanceType)
{
    ofstream outFile;
    ifstream inFile;
    string inFileName;
    string outFileName;


    inFile.open(path);

    if (inFile)
    {
        //cout << "\tLendo arquivo instancia" << endl;
        //cout << "\tPath (caminho): " << path << endl;

        // get length of file:
        inFile.seekg(0, std::ifstream::end);
        auto length = static_cast<unsigned int>(inFile.tellg());
        inFile.seekg(0, std::ifstream::beg);

        //criando e inicializando buffer
        //buffer contem o arquivo inteiro
        auto *buffer = new char[length];
        inFile.read(buffer, length);
        inFile.close();



        if (instanceType == 1)
            instanceTypeOneRead(buffer);//instanceTypeOneRead(inFile);
        else if (instanceType == 2)
            instanceTypeTwoRead(buffer);
        else
        {
            cout << "Tipo de instancia informado: " << instanceType << endl;
            cout << "Digite o tipo de instancia correntamente: 1 ou 2" << endl;
            exit(1);
        }

        criaTodasArestas();

        for(int i = 0; i < ordem; ++i)
            matrizDistancia[i][i] = 0;

        delete[] buffer;
        return;
    }
    cout << "\tFalha na leitura do arquivo!" << endl;
    cout << "\tO caminho tentado foi: \"" << path << "\"." << endl;
    cout << "\tVERIFIQUE se digitou o nome do diretorio path corretamente." << endl;
    exit(0);
}

void Grafo::instanceTypeOneRead(char *buffer)
{
    stringstream stream;

    //cout << "\tLendo Instancia\n";
    char *p = strtok(buffer, "\n");

    stream << p;

    string temp;

    stream >> this->ordem >> this->groups >> temp >> this->lowerLimit >> this->upperLimit;

    criaMatrizPeso();

    for (int i = 0; i < (groups * 2) - 1; ++i) stream >> temp;

    int contVertices = 0;
    while (stream >> temp && temp != "\n")
    {
        this->vertices.emplace_back(contVertices, stof(temp));
        contVertices++;
    }

    int vert1, vert2;
    float peso;
    while (p != nullptr)
    {
        p = strtok(nullptr, "\n");
        stream.clear();
        stream << p;
        stream >> vert1 >> vert2 >> peso;
        matrizDistancia[vert1][vert2] = peso;
        matrizDistancia[vert2][vert1] = peso;
    }
}

void Grafo::instanceTypeTwoRead(char *buffer)
{
    stringstream stream;

    //cout << "\tLendo Instancia\n";

    char *p = strtok(buffer, "\n");
    this->ordem = static_cast<unsigned int>(stoi(p));

    p = strtok(nullptr, "\n");
    this->groups = static_cast<unsigned int>(stoi(p));

    p = strtok(nullptr, "\n");
    this->upperLimit = stof(p);
    this->lowerLimit = 0;

    //cout << "\tLendo Instancia: ordem =  " << this->ordem << " Total de grupos = " <<  this->groups << " Capacidade = " << this->upperLimit << endl;

    criaMatrizPeso();

    for (int i = 0; i < this->ordem; ++i)
    {
        p = strtok(nullptr, "\n");
        this->vertices.emplace_back(i, stof(p));
    }

    p = strtok(nullptr, "\n");
    stream << p;

    for (int i = 0; i < this->ordem; ++i)
    {
        for (int j = 0; j < this->ordem; ++j)
        {
            stream >> matrizDistancia[i][j];
        }
    }
}

void Grafo::criaMatrizPeso()
{
    matrizDistancia = new float *[this->ordem];

    for (int i = 0; i < this->ordem; i++)
    {
        matrizDistancia[i] = new float[this->ordem];
        for (int j = 0; j < this->ordem; j++)
        {
            matrizDistancia[i][j] = 0;
        }
    }
}

void Grafo::criaTodasArestas()
{
    for (int i = 0; i < ordem; ++i)
    {
        for (int j = 0; j < ordem; ++j)
        {
            if (matrizDistancia[i][j] != 0)
            {
                vertices[i].adicionaAresta(vertices[j], matrizDistancia[i][j]);
            }
        }
    }
}*/