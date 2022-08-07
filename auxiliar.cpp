/*9
9
5
4
9
3
8
4
5
1
5
1
1
6
8
3
10
5
4
2
9
7
3
5
3
5
2
7
8
4
8
7
1
6
1
6
2
10
5
4
7
9
9
4
5
10
8
8
9
4
8
7
10
6
4
8
1
7
5
8
1
6
10
8
8
1
4
7
8
10
10
8
5
2
5
2
1
8
8
8
8
9
5
6
10
2
4
8
6
1
3
8
3
7
8
10
4
9
1
1
6
3
6
7
4
7
7
5
3
5
1
1
3
9
9
7
2
1
1
1
8
4
9
1
7
9
9
7
2
1
6
4
5
9
2
10
2
3
4
9
3
1
5
4
8
1
10
4
8
1
6
5
1
6
1
10
2
5
5
2
8
2
3
1
10
8
9
3
10
1
1
10
5
4
1
2
4
4
2
4
3
2
1
2
4
6
9
1
4
10
1
10
10
4
1
9
6
3
1
9
7
8
6
9
6
6
8
1
10
9
3
2
7
8
2
3
10
10
1
10
9
9
9
10
1
8
10
6
7
8
9
10
1
3
5
7
2
8
4
3*/

#include <dirent.h>

#include <iostream>

using std::cout;
using std::endl;
using std::string;

/*
 * AS FUNÇÕES CONTIDAS NESSE ARQUIVO SAO EXCLUSIVAMENTE USADAS
 * PARA DEBUGS E TESTES AUTOMATIZADOS DURANTE O DESENVOLVIMENTO.
 *
 ************************************************************/

/*
 * Função que escreve as 20 primeiras linhas do grafo
 *@params: FILE *file: arquivo onde se encontra o grafo
 *         std::string name: nome
 *@return:
 ****************************************************************/
void printArgs(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        cout << i;
        cout << *argv[i] << endl;
    }
}
/*
 * AS FUNÇÕES CONTIDAS NESSE ARQUIVO SAO EXCLUSIVAMENTE USADAS
 * PARA DEBUGS E TESTES AUTOMATIZADOS DURANTE O DESENVOLVIMENTO.
 *
 ************************************************************/

/*
 * Função que escreve as 20 primeiras linhas do grafo
 *@params: FILE *file: arquivo onde se encontra o grafo
 *         std::string name: nome
 *@return:
 ****************************************************************/
void printFileDetails(FILE *file, std::string name) {
    int i = 1;
    cout << "===== " << name << " =====" << endl;
    cout << "PRIMEIRAS 20 LINHAS " << endl;

    while (!feof(file)) {
        char line[100];

        char *result = fgets(line, 100, file);

        if (result && i <= 20)
            cout << "Linha " << i << ": " << line << endl;

        delete result;
        i++;
    }

    cout << " === TOTAL DE LINHAS: " << i << " === \n\n"
         << endl;

    fclose(file);
}

/*
 * Função que le os arquivos de um determinado diretório
 *@params: char *directoryPath: Caminho ate o diretório partindo de /
 *
 *@return:
 ****************************************************************/
void readFilesInDirectory(const char *directoryPath) {
    DIR *dir;
    FILE *file;
    struct dirent *lsdir;

    dir = opendir(directoryPath);

    while ((lsdir = readdir(dir)) != NULL) {
        string path = std::string(directoryPath) + "/" + std::string(lsdir->d_name);
        file = fopen(path.c_str(), "rt");

        if (file == NULL) {
            perror("Erro ao abrir o arquivo");
        } else {
            printFileDetails(file, lsdir->d_name);
        }
    }
}
