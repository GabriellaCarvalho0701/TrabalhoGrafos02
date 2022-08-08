//
// Created by Daniel on 23/07/2022.
//

#ifndef DEV_TIMER_H
#define DEV_TIMER_H

#include <string>
#include <chrono>
#include <iostream>
#include <utility>

using namespace std::chrono;

/*
 * Crie um objeto timer dentro de um escopo, ele ira iniciar o cronometro quando o objeto for criado
 * e iterromper o cronometro quando o objeto for destruido.
 * Sua destruicao ira imprimir o tempo de execucao do escopo.
 *
 * */
class Timer
{
private:
    std::string legenda;
    time_point<std::chrono::system_clock> start;
    time_point<std::chrono::system_clock> end;
    duration<double> elapsedSeconds{};

    void startTimer()
    {
        start = std::chrono::system_clock::now();
    }

    void stopTimer()
    {
        end = std::chrono::system_clock::now();
        elapsedSeconds = end - start;
    }

public:
    explicit Timer(std::string legenda)
    {
        this->legenda = std::move(legenda);
        startTimer();
    }

    ~Timer()
    {
        stopTimer();
        std::cout << legenda << ": " << elapsedSeconds.count() << "s\n";
    }
};


#endif //DEV_TIMER_H