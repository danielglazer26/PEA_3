#include <iostream>
#include "Header/GeneticAlgorithm.h"

int main() {
    auto *gens = new GeneticAlgorithm();
    gens->startAlgorithm(1,
                         30,
                         5,
                         500,
                         0,
                         1);
    return 0;
}
