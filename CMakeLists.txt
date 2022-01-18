cmake_minimum_required(VERSION 3.20)
project(Etap3)

set(CMAKE_CXX_STANDARD 23)

add_executable(Etap3 Source/main.cpp Source/Matrix.cpp Header/Matrix.h Source/LoadFromFile.cpp Header/LoadFromFile.h Source/GeneticAlgorithm.cpp Header/GeneticAlgorithm.h)
