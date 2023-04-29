//
//  Instance.cpp
//  pcicTree
//
//  Created by David Galvao on 11/11/22.
//
#include <fstream>
#include <iostream>

#include "Instance.hpp"

Instance::Instance(char *filename, bool legacy) : numResourcesUsed(0) {
    if(legacy) {
        readInstanceLegacy(filename);
    } else {
        readInstance(filename);
    }
    for(int i = 0; i < L; i++) {
        if(resourceUsed[i]) numResourcesUsed++;
    }
}

Instance::~Instance() {
    for(int i = 0; i < N; i++) {
        delete [] matrix[i];
        delete [] verticesIntersections[i];
    }
    delete [] matrix;
    delete [] verticesIntersections;
}

/* Faz a leitura das instâncias no formato novo */
void Instance::readInstance(char *filename) {
    std::ifstream input(filename, std::ios::in);
    if(!input) {
        std::cout << "Error while opening file: " << filename << "\n";
        exit(-1);
    }
    // Ler o tamanho da instância (N = Empresas, L = Licitações)
    input >> N;
    input >> L;
    // Inicializar a matriz
    matrix = new bool*[N];
    vertices = std::vector<std::set<int>>(N);
    verticesIntersections = new std::set<int>*[N];
    resourceUsed = std::vector<bool>(L, false);
    // Processar os recursos das empresas
    int v, vnl, l;
    for(int i = 0; i < N; i++) {
        input >> v;
        input >> vnl;
        matrix[i] = new bool[L];
        verticesIntersections[i] = new std::set<int>[N];
        for(int j = 0; j < L; j++) {
            matrix[i][j] = false;
        }
        for(int j = 0; j < vnl; j++) {
            input >> l;
            matrix[v][l] = true;
            resourceUsed[l] = true;
            vertices[v].insert(l);
        }
    }
    input.close();
}

/* Faz a leitura das instâncias no formato antigo */
void Instance::readInstanceLegacy(char *filename) {
    std::ifstream input(filename, std::ios::in);
    if(!input) {
        std::cout << "Error while opening file: " << filename << "\n";
        exit(-1);
    }
    // Ler o tamanho da instância (N = Empresas, L = Licitações)
    input >> N;
    input >> L;
    // Inicializar a matriz
    matrix = new bool*[N];
    vertices = std::vector<std::set<int>>(N);
    verticesIntersections = new std::set<int>*[N];
    resourceUsed = std::vector<bool>(L, false);
    // Processar os recursos das empresas
    int aux;
    for(int i = 0; i < N; i++) {
        matrix[i] = new bool[L];
        verticesIntersections[i] = new std::set<int>[N];
        for(int j = 0; j < L; j++) {
            input >> aux;
            matrix[i][j] = (aux == 1);
            if(matrix[i][j]) {
                vertices[i].insert(j);
                resourceUsed[j] = true;
            }
        }
    }
    input.close();
}
