#ifndef ETAP3_GENETICALGORITHM_H
#define ETAP3_GENETICALGORITHM_H


#include <vector>
#include "Matrix.h"

class GeneticAlgorithm {

private:


    std::vector<std::pair<int, std::vector<unsigned int>>> population;

    static bool cmp(std::pair<int, std::vector<unsigned int>> &a,
                    std::pair<int, std::vector<unsigned int>> &b) {
        return a.first < b.first;
    }

    Matrix *matrixWeights;
    int **matrix;

    int swapNeighbors(std::vector<unsigned int> *path, int i, int j);

public:
    GeneticAlgorithm();

    void startAlgorithm();

    int calculateCost(std::vector<unsigned int> path);

    static void showPath(std::vector<unsigned int> path);

    void generateRandomParents();

    void makePartiallyMappedCrossover(std::vector<unsigned> parent1, std::vector<unsigned> parent2,
                                      std::vector<unsigned> &child1, std::vector<unsigned> &child2);

    static void
    copyCutPoints(const std::vector<unsigned int> &parent, std::vector<unsigned int> *child, int a, int b, int i,
                  unsigned int &firstNoCopied);

    static void
    completeRestPoints(unsigned int firstNoCopied, const std::vector<unsigned int> &parent,
                       std::vector<unsigned int> *child, int a, int b);
};


#endif //ETAP3_GENETICALGORITHM_H
