#ifndef ETAP2_MATRIX_H
#define ETAP2_MATRIX_H


#include "LoadFromFile.h"

class Matrix {
public:
    Matrix() { loadData(); }

    Matrix(std::string fileName) { loadDataName(fileName); }

    Matrix(bool directed, int vNumber) {
        createNewMatrix(directed, vNumber);
    }

    void showMatrixWages();

    int getSize() const;

    int getOptimum() const;

    int **getMatrixWeights() const;

    void deleteMatrixTable(int **matrixW);

private:
    int size;

    int optimum;

    int **matrixWeights = nullptr;

    void createTables();

    void createMatrix(LoadFromFile *loadFromFile);

    void loadData();

    void createNewMatrix(bool directed, int vNumber);

    void loadDataName(std::string fileName);
};


#endif //ETAP2_MATRIX_H
