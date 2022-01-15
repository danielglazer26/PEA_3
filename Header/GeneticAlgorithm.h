#ifndef ETAP3_GENETICALGORITHM_H
#define ETAP3_GENETICALGORITHM_H

#include <vector>
#include <random>
#include <ctime>
#include "Matrix.h"

class GeneticAlgorithm {

private:


    Matrix *matrixWeights;

    std::vector<std::pair<int, std::vector<unsigned int>>> population;

    static bool cmp(std::pair<int, std::vector<unsigned int>> &a,
                    std::pair<int, std::vector<unsigned int>> &b) {
        return a.first < b.first;
    }
    int **matrix;

    int finalCost = INT_MAX;
    std::vector<unsigned int> globalPath;

    int swapNeighbors(std::vector<unsigned int> *path, int i, int j);

public:
    GeneticAlgorithm();

    void startAlgorithm(double probability, int populationSize, int populationCopyNumber,
                        int generationNumber);

    int calculateCost(std::vector<unsigned int> path);

    static void showPath(std::vector<unsigned int> path);

    void generateRandomParents(std::mt19937 engine, int populationCopyNumber);

    void makePartiallyMappedCrossover(std::vector<unsigned int> parent1, std::vector<unsigned int> parent2,
                                      std::vector<unsigned int> &child1, std::vector<unsigned int> &child2,
                                      std::mt19937 engine);

    static void
    copyCutPoints(const std::vector<unsigned int> &parent, std::vector<unsigned int> *child, int a, int b, int i,
                  unsigned int &firstNoCopied);

    static void
    completeRestPoints(unsigned int firstNoCopied, const std::vector<unsigned int> &parent,
                       std::vector<unsigned int> *child, int a, int b);

    int tournamentSelection();

    void mainLoop(std::mt19937 engine, double probability, int populationSize, int populationCopyNumber,
                  int generationNumber);

    void showPRD();

    void makeMutation(std::pair<int, std::vector<unsigned int>> *path);

    void checkMutation(std::mt19937 engine, std::vector<unsigned int> &child, double probability);

    void copyPopulation(int number);

    void countFitnessValue(std::vector<float> &fitness);
};


#endif //ETAP3_GENETICALGORITHM_H
