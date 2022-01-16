#include <iostream>
#include "Header/GeneticAlgorithm.h"

int main() {
    auto *gens = new GeneticAlgorithm();
    gens->startAlgorithm(0.6, 50, 15, 100);
    return 0;
}
