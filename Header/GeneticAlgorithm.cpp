#include "GeneticAlgorithm.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

GeneticAlgorithm::GeneticAlgorithm() {
    matrixWeights = new Matrix();
    matrix = matrixWeights->getMatrixWeights();
}
/**
 *
 * @param probability - prawdopodobieństwo - wystąpienia mutacji
 * @param populationSize - rozmiar populacji w jednej generacji
 * @param populationCopyNumber - liczba osobników populacji przenoszona do kolejnej epoki
 * @param generationNumber - liczba generacji
 * @param selectionType - wybór selekcji:
 *          0 - selekcja koła ruletki
 *          1 - selekcja turniejowa
 * @param crossoverType - wybór typu krzyżowania:
 *          0 - Partially Mapped Crossover
 *          1 - Order Crossover Operator
 */
void GeneticAlgorithm::startAlgorithm(double probability, int populationSize, int populationCopyNumber,
                                      int generationNumber,  int selectionType, int crossoverType) {
    //srand(time(nullptr));
    srand(100);
    mt19937 gen(rand());


    generateRandomParents(gen, populationCopyNumber);
    mainLoop(gen, probability, populationSize, populationCopyNumber, generationNumber, selectionType, crossoverType);
}

void GeneticAlgorithm::mainLoop(mt19937 &engine, double probability, int populationSize, int populationCopyNumber,
                                int generationNumber, int selectionType, int crossoverType) {


    float sum;
    pair<int, int> parents;
    auto pointer1 = population.begin();
    auto pointer2 = population.begin();


    for (int i = 0; i < generationNumber; i++) {

        for (int j = 0; j < populationSize; j++) {
            vector<unsigned> child1(matrixWeights->getSize(), 0);
            vector<unsigned> child2(matrixWeights->getSize(), 0);

            sum = 0;


            switch (selectionType) {
                case 0: {
                    sort(population.begin(), population.end(), cmp);
                    vector<float> fitnessValueTable(population.size(), 0);
                    countFitnessValue(fitnessValueTable, sum);
                    parents = rouletteWheelSelection(engine, fitnessValueTable, sum);
                    break;
                }
                case 1:
                    parents = tournamentSelection(engine);
                    break;
            }

            pointer1 = population.begin() + parents.first;
            pointer2 = population.begin() + parents.second;

            switch (crossoverType) {
                case 0:
                    makePartiallyMappedCrossover(
                            pointer1->second,
                            pointer2->second,
                            child1,
                            child2,
                            engine);
                    break;
                case 1:
                    makeOrderCrossoverOperator(pointer1->second,
                                               pointer2->second,
                                               child1,
                                               child2,
                                               engine);
                    break;
            }

            checkMutation(engine, child1, probability);
            checkMutation(engine, child2, probability);
        }
        sort(population.begin(), population.end(), cmp);
        copyPopulation(populationCopyNumber);

    }
    showPath(globalPath);
    showPRD();

}

void GeneticAlgorithm::countFitnessValue(vector<float> &fitness, float &sum) {

    for (int i = 0; i < population.size(); i++) {
        fitness.at(i) = ((float) finalCost / (population.begin() + i)->first);
    }

    for (int i = 0; i < population.size(); i++) {
        sum += fitness.at(i);
    }
}

// wybieranie rodziców do krzyżowania na podstawie selekcji turniejowej
pair<int, int> GeneticAlgorithm::tournamentSelection(mt19937 &engine) {
    uniform_int_distribution<> randomParent(0, static_cast<int>(population.size()) - 1);

    int a, b;
    a = randomParent(engine);
    b = randomParent(engine);

    pair<int, int> parents;

    if (population.at(a).first > population.at(b).first)
        parents.first = b;
    else
        parents.first = a;

    a = randomParent(engine);
    b = randomParent(engine);


    if (population.at(a).first > population.at(b).first)
        parents.second = b;
    else
        parents.second = a;

    return parents;

}

// wybieranie rodziców do krzyżowania na podstawie selekcji koła ruletki
pair<int, int> GeneticAlgorithm::rouletteWheelSelection(mt19937 &engine, vector<float> &fitness, float &sum) {


    uniform_real_distribution<float> randomNumber(0, sum);
    float r = randomNumber(engine);
    float r2 = randomNumber(engine);

    if (r2 < r)
        swap(r2, r);


    pair<int, int> parents(-1, 0);
    float sum2 = 0;

    for (int i = 0; i < population.size(); i++) {
        sum2 += fitness.at(i);
        if (r <= sum2 && parents.first == -1)
            parents.first = i;
        else if (r2 <= sum2) {
            parents.second = i;
            return parents;
        }
    }

    return parents;
}

// sprawdzenie na podstawie prawdopodobieństwa czy może zajść mutacja
void GeneticAlgorithm::checkMutation(std::mt19937 engine, vector<unsigned int> &child, double probability) {
    uniform_real_distribution<> generateProbability(0.0, 1.0);

    population.emplace_back(calculateCost(child), child);

    auto pointerLast = population.end() - 1;
    if (generateProbability(engine) < probability)
        makeMutation(pointerLast.base());


    if (finalCost > pointerLast->first) {
        finalCost = pointerLast->first;
        globalPath = pointerLast->second;
    }
}

// generowanie pierwszych rodziców o liczbie populationCopyNumber
void GeneticAlgorithm::generateRandomParents(std::mt19937 engine, int populationCopyNumber) {
    vector<unsigned int> vertexes;

    for (unsigned int i = 0; i < matrixWeights->getSize(); i++) vertexes.push_back(i);

    for (unsigned int i = 0; i < populationCopyNumber; i++) {
        shuffle(vertexes.begin(), vertexes.end(), engine);
        pair<int, vector<unsigned int>> p = {this->calculateCost(vertexes), vertexes};
        if (p.first < finalCost) {
            finalCost = p.first;
            globalPath = p.second;
        }
        population.push_back(p);
    }

}

// metoda krzyżowania - Partially Mapped Crossover
void
GeneticAlgorithm::makePartiallyMappedCrossover(const vector<unsigned int> &parent1, const vector<unsigned int> &parent2,
                                               std::vector<unsigned int> &child1, std::vector<unsigned int> &child2,
                                               std::mt19937 engine) {
    uniform_int_distribution<> cutPoints(1, matrixWeights->getSize() - 1);


    int a = 0, b = 0;

    //losowanie punktów przecięcia
    while (a == b || a > b) {
        a = cutPoints(engine);
        b = cutPoints(engine);
    }

    //kopiowanie obszarów przecięcia
    for (int i = a; i <= b; i++) {
        child1.at(i) = parent2.at(i);
        child2.at(i) = parent1.at(i);
    }

    unsigned int firstNoCopied1 = a;
    unsigned int firstNoCopied2 = a;
    for (int i = 0; i < parent1.size(); i++) {
        if (i < a || i > b) {
            copyCutPoints(parent1, &child1, a, b, i, firstNoCopied1);
            copyCutPoints(parent2, &child2, a, b, i, firstNoCopied2);
        }
    }

    completeRestPoints(firstNoCopied1, parent2, &child1, a, b);
    completeRestPoints(firstNoCopied2, parent1, &child2, a, b);

}

// metoda krzyżowania - Order Crossover Operator
void GeneticAlgorithm::makeOrderCrossoverOperator(const std::vector<unsigned int> &parent1,
                                                  const std::vector<unsigned int> &parent2,
                                                  std::vector<unsigned int> &child1, std::vector<unsigned int> &child2,
                                                  std::mt19937 engine) {

    uniform_int_distribution<> cutPoints(1, static_cast<int>(parent1.size()) - 2);


    int a = 0, b = 0;

    //losowanie punktów przecięcia
    while (a == b) {
        a = cutPoints(engine);
        b = cutPoints(engine);
    }
    if (a > b)
        swap(a, b);

    //kopiowanie obszarów przecięcia
    for (int i = a; i <= b; i++) {
        child1.at(i) = parent1.at(i);
        child2.at(i) = parent2.at(i);
    }

    copySequenceFromOrderCrossoverOperator(parent2, child1, a, b);
    copySequenceFromOrderCrossoverOperator(parent1, child2, a, b);


}

// kopiowanie pozostałych wierzchołków sekwencji, które nie były w wyciętym obszarze
void GeneticAlgorithm::copySequenceFromOrderCrossoverOperator(const vector<unsigned int> &parent,
                                                              vector<unsigned int> &child, const int &a, const int &b) {
    int j = b + 1;
    auto v = parent.begin() + b + 1;

    do {

        if (find(child.begin() + a, child.begin() + b + 1, *v) == child.begin() + b + 1) {
            child.at(j) = *v;
            if (v != parent.end() - 1)
                v++;
            else
                v = parent.begin();
        } else {
            if (v != parent.end() - 1)
                v++;
            else
                v = parent.begin();
            continue;
        }

        if (j < child.size() - 1)
            j++;
        else
            j = 0;

    } while (j != a);

}

// kopiowanie punktów cięcia
void GeneticAlgorithm::copyCutPoints(const vector<unsigned int> &parent, vector<unsigned int> *child,
                                     int a, int b, int i, unsigned int &firstNoCopied) {

    unsigned int vertex = (parent.begin() + i).operator*();
    if (find(child->begin() + a, child->begin() + b + 1, vertex) == child->begin() + b + 1)
        (child->begin() + i).operator*() = vertex;
    else if (firstNoCopied == a)
        firstNoCopied = i;

}

// uzupełnianie wektora o pozostałe wierzchołki, które nie zostały przekopiowane w obszarze kopiowania
void GeneticAlgorithm::completeRestPoints(const unsigned int firstNoCopied, const vector<unsigned int> &parent,
                                          vector<unsigned int> *child,
                                          int a, int b) {
    for (unsigned int i = firstNoCopied; i < child->size(); i++) {
        if (i < a || i > b) {
            for (int j = 0; j < parent.size(); j++) {
                if (j < a || j > b) {
                    if (find(child->begin(), child->end(), parent.at(j)) == child->end()) {
                        (child->begin() + i).operator*() = parent.at(j);
                        break;
                    }
                }
            }
        }
    }
}

// wyznaczanie najlepszej mutacji
void GeneticAlgorithm::makeMutation(pair<int, vector<unsigned int>> *path) {
    int bestI, bestJ, bestSwap = INT_MAX;
    int swapValue;

    for (int i = 1; i < path->second.size() - 3; i++) {
        for (int j = i + 1; j < path->second.size() - 2; j++) {
            swapValue = swapNeighbors(&path->second, i, j);
            if (swapValue < bestSwap) {
                bestI = i;
                bestJ = j;
                bestSwap = swapValue;
            }
        }
    }
    swap(path->second.at(bestI), path->second.at(bestJ));
    path->first += bestSwap;
}

// podliczanie zmiany kosztów po zamianie wierzchołków
int GeneticAlgorithm::swapNeighbors(vector<unsigned int> *path, int i, int j) {
    int subtractOldEdges = 0;
    int addNewEdges = 0;

    if (j - i == 1) {
        subtractOldEdges += matrix[path->at(i - 1)][path->at(i)];
        subtractOldEdges += matrix[path->at(i)][path->at(j)];
        subtractOldEdges += matrix[path->at(j)][path->at(j + 1)];

        addNewEdges += matrix[path->at(i - 1)][path->at(j)];
        addNewEdges += matrix[path->at(j)][path->at(i)];
        addNewEdges += matrix[path->at(i)][path->at(j + 1)];

    } else {
        subtractOldEdges += matrix[path->at(i - 1)][path->at(i)];
        subtractOldEdges += matrix[path->at(i)][path->at(i + 1)];
        subtractOldEdges += matrix[path->at(j - 1)][path->at(j)];
        subtractOldEdges += matrix[path->at(j)][path->at(j + 1)];

        addNewEdges += matrix[path->at(i - 1)][path->at(j)];
        addNewEdges += matrix[path->at(j)][path->at(i + 1)];
        addNewEdges += matrix[path->at(j - 1)][path->at(i)];
        addNewEdges += matrix[path->at(i)][path->at(j + 1)];
    }

    return addNewEdges - subtractOldEdges;
}

// wyświetlanie i obliczanie PRD
void GeneticAlgorithm::showPRD() {
    std::cout << finalCost
              << "   "
              << 100 * (((float) (finalCost - matrixWeights->getOptimum()))
                        / (float) matrixWeights->getOptimum())
              << "% \n";
}

// wyświetlanie ścieżki
void GeneticAlgorithm::showPath(vector<unsigned int> path) {

    for (unsigned int i: path)
        cout << i << "->";

    cout << path.front() << "\n";
}

// obliczanie kosztu ścieżki
int GeneticAlgorithm::calculateCost(vector<unsigned int> path) {

    int cost = 0;
    auto i = path.begin();
    while (i != (path.end() - 1)) {
        cost += matrix[i.operator*()][(i + 1).operator*()];
        i.operator++();
    }
    cost += matrix[path.back()][path.front()];
    return cost;
}

// kopiowanie najlepszych jednostek z populacji do nowej generacji
void GeneticAlgorithm::copyPopulation(int number) {
    std::vector<std::pair<int, std::vector<unsigned int>>> p2;
    for (int i = 0; i < number; i++) p2.push_back(population.at(i));
    population = p2;
}

