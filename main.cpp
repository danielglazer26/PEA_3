#include <iostream>
#include "Header/GeneticAlgorithm.h"

int main() {
    auto *gens = new GeneticAlgorithm();
    gens->startAlgorithm(0.6,
                         150,
                         15,
                         300,
                         1,
                         1);
    return 0;
}
