//
//  PairSolution.cpp
//  pcicTree
//
//  Created by David Galvao on 02/02/23.
//
#include <cmath>
#include <iostream>

#include "PairSolution.hpp"

PairSolution::PairSolution(Instance &instance, bool restrictIntersection) : restrictIntersection(restrictIntersection), nClusters(ceil((double) instance.N / 2)), instance(instance), bestObj(0), theoreticalObj(new int*[nClusters]) {
    createCandidates();
    computeTheoreticalObj();
    solveRoot();
}

PairSolution::PairSolution(Instance &instance, std::set<int> participants, bool restrictIntersection) : restrictIntersection(restrictIntersection), nClusters(ceil((double) participants.size() / 2)), instance(instance), bestObj(0), theoreticalObj(new int*[nClusters]) {
    if(nClusters > 0) {
        createCandidates(participants);
        computeTheoreticalObj();
        solveRoot();
    }
}

// Obter os clusters candidatos da partição
void PairSolution::createCandidates(std::set<int> participants) {
    // Listar todos os cluster mínimos (candidatos)
    for(int i = 0; i < instance.N; i++) {
        if(!participants.empty() && participants.find(i) == participants.end()) continue;
        for(int j = i + 1; j < instance.N; j++) {
            if(!participants.empty() && participants.find(j) == participants.end()) continue;
            std::set<int> intersection = getIntersection(instance, i, j);
            std::string intersectionStr = intersectionString(intersection);
            if(intersection.size() > 0) {
                candidates.push_back(Candidate(i, j, (int) intersection.size(), intersectionStr));
                intersections[intersectionStr] = false;
            }
        }
    }
    // Ordenar de forma decrescente, pelo tamanho da interseção
    std::sort(candidates.begin(), candidates.end(), std::greater<Candidate>());
}

void PairSolution::computeTheoreticalObj() {
    // Pegar o valor teórico da função objetivo para quando se faltam "left" clusters a se adicioanar
    for(int left = nClusters; left > 1; left--) {
        theoreticalObj[nClusters - left] = new int[candidates.size()];
        int cumSum = 0;
        for(int i = 0; i < candidates.size(); i++) {
            cumSum = 0;
            // Pegar o valor dos "left" proximos clusters somados
            for(int j = 0; i + j < candidates.size() && j < left; j++) {
                cumSum += candidates[i + j].intersectionSize;
            }
//            if(i == 0) {
//                for(int j = 0; i + j < candidates.size() && j < left; j++) {
//                    cumSum += candidates[i + j].intersectionSize;
//                }
//            } else {
//                cumSum -= candidates[i - 1].intersectionSize;
//                if(i + left < candidates.size()) {
//                    cumSum += candidates[i + left].intersectionSize;
//                }
//            }
            theoreticalObj[nClusters - left][i] = cumSum;
        }
    }
    // Pegar o valor teórico para quando se falta 1 cluster a adicionar
    theoreticalObj[nClusters - 1] = new int[candidates.size()];
    for(int i = 0; i < candidates.size(); i++) {
        theoreticalObj[nClusters - 1][i] = candidates[i].intersectionSize;
    }
}

// Buscar pela solução
void PairSolution::solveRoot() {
    for(int i = 0;  i < candidates.size(); i++) {
        std::stack<Candidate *> currentSolution;
        std::vector<bool> inPartition(instance.N, false);
        std::cout << "(" << i << "/" << candidates.size() << ") Best solution: " << bestObj << " Theoretical best: " << theoreticalObj[0][i] << "\n";
        std::vector<std::string> intersections;
        if(theoreticalObj[0][i] > bestObj) {
            ticks = 0;
//        if(candidates[i].intersectionSize * nClusters > bestObj) {
            currentSolution.push(&candidates[i]);
            inPartition[candidates[i].i] = true;
            inPartition[candidates[i].j] = true;
            intersections.push_back(candidates[i].intersectionStr);
            if(candidates[i].intersectionSize > bestObj) {
                bestSolution = currentSolution;
                bestObj = candidates[i].intersectionSize;
                std::cout << "New best solution: " << bestObj << "\n";
            }
            int status = solveNode(inPartition, currentSolution, i, candidates[i].intersectionSize, 1);
            switch (status) {
                case -1:
                    std::cout << "Maximum operations without improvement reached\n";
                    break;
                case 0:
                    std::cout << "No improvement possible\n";
                    break;
                case 1:
                    std::cout << "Completed subtree " << i << "\n";
                    break;
            }
        } else {
            break;
        }
    }
}

int PairSolution::solveNode(std::vector<bool> &inPartition, std::stack<Candidate *> &currentSolution, const int currentIndex, const int currentObj, int depth) {
    if(ticks > MAX_TICKS) {
//        std::cout << "Maximum operations without improvement reached\n";
        return -1;
    }
    for(int i = currentIndex + 1; i < candidates.size(); i++) {
        if(currentObj + theoreticalObj[currentSolution.size()][i] <= bestObj) {
//        if(currentObj + candidates[i].intersectionSize * (nClusters - (int) currentSolution.size()) <= bestObj) {
            return 0;
        }
//        std::cout << "(" << i << "/" << candidates.size() << ") D: " << depth << " UB: " << bestObj << " LB: " << currentObj + theoreticalObj[0][i] << "\n";
        if(!inPartition[candidates[i].i] && !inPartition[candidates[i].j]) {
            if(restrictIntersection && intersections[candidates[i].intersectionStr]) continue;
            currentSolution.push(&candidates[i]);
            inPartition[candidates[i].i] = true;
            inPartition[candidates[i].j] = true;
            intersections[candidates[i].intersectionStr] = true;
            if(currentSolution.size() < nClusters) {
                ticks++;
                solveNode(inPartition, currentSolution, i, currentObj + candidates[i].intersectionSize, depth++);
            }
            if(currentObj + candidates[i].intersectionSize > bestObj) {
                ticks = 0;
                bestSolution = currentSolution;
                bestObj = currentObj + candidates[i].intersectionSize;
                std::cout << "New best solution: " << bestObj << "\n";
            }
            currentSolution.pop();
            intersections[candidates[i].intersectionStr] = false;
            inPartition[candidates[i].i] = false;
            inPartition[candidates[i].j] = false;
        }
    }
    return 1;
}

std::vector<Cluster *> PairSolution::getSolution() {
    std::vector<Cluster *> solution;
    std::stack<Candidate *> aux = bestSolution;
    std::vector<bool> inPartition(instance.N, false);
    
    while(!aux.empty()) {
        Candidate *top = aux.top();
        inPartition[top->i] = true;
        inPartition[top->j] = true;
        
        Cluster *cluster = new Cluster();
        cluster->elements.push_back(top->i);
        cluster->elements.push_back(top->j);
        cluster->intersection = getIntersection(instance, top->i, top->j);
        
        solution.push_back(cluster);
        aux.pop();
    }
    // Adicionar as sobras à partição
    Cluster *leftover = new Cluster();
    for(int i = 0; i < instance.N; i++) {
        bool inserted = false;
        if(!inPartition[i]) {
            // Procurar adicionar a sobra a um cluster já existente
            for(Cluster * cluster : solution) {
                // Caso a adição não resulte em uma mudança na interseção, adicione o elemento ao cluster
                if(getIntersection(instance, cluster->elements, cluster->intersection, i).size() == cluster->intersection.size()) {
                    cluster->elements.push_back(i);
                    inserted = true;
                    break;
                }
            }
            if(!inserted) {
                leftover->elements.push_back(i);
            }
        }
    }
    if(!leftover->elements.empty()) {
        solution.push_back(leftover);
    } else {
        delete leftover;
    }
    
    return solution;
}

std::vector<Cluster *> pairSolve(Instance &instance) {
    std::vector<Cluster *> clusters;
    std::vector<Candidate> candidates;
    // Listar todos os cluster mínimos (candidatos)
    for(int i = 0; i < instance.N; i++) {
        for(int j = i + 1; j < instance.N; j++) {
            std::set<int> intersection = getIntersection(instance, i, j);
            if(intersection.size() > 0) {
                candidates.push_back(Candidate(i, j, (int) intersection.size(), intersectionString(intersection)));
            }
        }
    }
    std::sort(candidates.begin(), candidates.end(), std::greater<Candidate>());
    // Construir as solução inicial, pegar o cluster com maior interseção
    // seguido pelo segundo maior (cujos membros não apareceram na partição) e assim sucessivamente
    std::vector<bool> inPartition;
    int bestSolutionValue = INFINITY;
    std::vector<Candidate *> bestSolution;
    for(int i = 0; i < instance.N; i++) {
        std::vector<Candidate *> solution;
        if(candidates[i].intersectionSize * round((double) instance.N / 2) < bestSolutionValue) {
            inPartition = std::vector<bool>(instance.N, false);
            inPartition[candidates[i].i] = true;
            inPartition[candidates[i].j] = true;
            
            solution.push_back(&candidates[i]);
        }
    }
    return clusters;
}
