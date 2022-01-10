#include "GeneticAlgorithm.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;

GeneticAlgorithm::GeneticAlgorithm() {
    matrixWeights = new Matrix();
    matrix = matrixWeights->getMatrixWeights();
}

void GeneticAlgorithm::startAlgorithm() {
    generateRandomParents();
    vector<unsigned> off1(matrixWeights->getSize(), 0);
    vector<unsigned> off2(matrixWeights->getSize(), 0);
    makePartiallyMappedCrossover(population.begin()->second, (population.begin() + 1)->second, off1, off2);
    showPath(population.begin()->second);
    cout << calculateCost(population.begin()->second);
    cout << endl;
    showPath((population.begin() + 1)->second);
    cout << calculateCost((population.begin() + 1)->second);
    cout << endl;
    showPath(off1);
    cout << calculateCost(off1);
    cout << endl;
    showPath(off2);
    cout << calculateCost(off2);
    cout << endl;
    cout << "Koniec";
}

void GeneticAlgorithm::generateRandomParents() {

    vector<unsigned int> vertexes;

    for (unsigned int i = 0; i < matrixWeights->getSize(); i++) {
        vertexes.push_back(i);
    }

    random_device rd;
    mt19937 gen(rd());

    for (unsigned int i = 0; i < matrixWeights->getSize(); i++) {
        shuffle(vertexes.begin(), vertexes.end(), gen);
        pair<int, vector<unsigned int>> p = {this->calculateCost(vertexes), vertexes};
        population.push_back(p);
    }
    sort(population.begin(), population.end(), cmp);
}

void GeneticAlgorithm::makePartiallyMappedCrossover(vector<unsigned int> parent1, vector<unsigned int> parent2,
                                                    vector<unsigned int> &child1, vector<unsigned int> &child2) {
    random_device rd;
    default_random_engine gen(rd());
    uniform_int_distribution<> cutPoints(1, matrixWeights->getSize() - 1);



    int a = 0, b = 0;

    while (a == b || a > b) {
        a = cutPoints(gen);
        b = cutPoints(gen);
    }

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

void GeneticAlgorithm::copyCutPoints(const vector<unsigned int> &parent, vector<unsigned int> *child,
                                     int a, int b, int i, unsigned int &firstNoCopied) {

    unsigned int vertex = (parent.begin() + i).operator*();
    if (find(child->begin() + a, child->begin() + b + 1, vertex) == child->begin() + b + 1)
        (child->begin() + i).operator*() = vertex;
    else if (firstNoCopied == a)
        firstNoCopied = i;

}

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

/*void GeneticAlgorithm::showPRD(int iter) {
    std::cout << iter
              << "   "
              << *finalCost
              << "   "
              << 100 * (((float) (*finalCost - matrixWeights->getOptimum()))
                        / (float) matrixWeights->getOptimum())
              << "% \n";
}*/

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

