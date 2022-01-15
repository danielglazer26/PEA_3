#include <iostream>
#include "Header/GeneticAlgorithm.h"

int main() {
    auto *gens = new GeneticAlgorithm();
    gens->startAlgorithm(0.40, 100, 10, 5);
    return 0;
}
