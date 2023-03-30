//
//  main.cpp
//  pcicTree
//
//  Created by David Galvao on 19/10/22.
//

#include <iostream>
#include <ostream>
#include <fstream>
#include <chrono>
#include <regex>

#include "TreeSolution.hpp"
#include "PairSolution.hpp"

void printCluster(std::ostream &os, TreeNode * cluster, int nClusters, int &i, int &aux) {
    os << "\t\t{\n\t\t\t\"id\": " << i++ << ", \n\t\t\t\"Elements\": [";
    aux = 0;
    for(int element : cluster->cluster) {
        if(aux < cluster->cluster.size() - 1) {
            os << element << ", ";
        } else {
            os << element << "], \n";
        }
        aux++;
    }
    os << "\t\t\t\"Intersection\": [";
    aux = 0;
    for(int label : cluster->intersection) {
        if(aux < cluster->intersection.size() - 1) {
            os << label << ", ";
        } else {
            os << label << "], \n\t\t\t";
        }
        aux++;
    }
    os << "\"p_ind\": " << cluster->independentProb << ", \n\t\t\t";
    os << "\"p_priori\": " << cluster->prioriProb << ", \n\t\t\t";
    os << "\"Clique\": " << cluster->clique << ", \n\t\t\t";
    os << "\"normClique\": " << cluster->normClique << ", \n\t\t\t";
    os << "\"Lift\": " << cluster->lift << ", \n\t\t\t";
    os << "\"Leverage\": " << cluster->leverage << "\n\t\t}";
    if(i < nClusters) {
        os << ",\n";
    }
}

double printClusters(std::ostream &os, std::vector<TreeNode *> &clusters) {
    double obj = 0;
    int i = 0, aux = 0;
    os << "{\n\t\"Clusters\": [\n";
    for(TreeNode * cluster : clusters) {
        obj += cluster->intersection.size();
        printCluster(os, cluster, (int) clusters.size(), i, aux);
        delete cluster;
    }
    os << "]\n}\n";
    
    return obj;
}

void printPartition(std::ostream &os, std::vector<Cluster *> &clusters, bool ignoreEmptyIntersection=false) {
    int i = 0, aux = 0, nClusters = (int) clusters.size();
    os << "{\n\t\"Clusters\": [\n";
    for(Cluster * cluster : clusters) {
        if(ignoreEmptyIntersection && cluster->intersection.empty()) continue;
        os << "\t\t{\n\t\t\t\"id\": " << i++ << ", \n\t\t\t\"Elements\": [";
        aux = 0;
        for(int element : cluster->cluster) {
            if(aux < cluster->cluster.size() - 1) {
                os << element << ", ";
            } else {
                os << element << "], \n";
            }
            aux++;
        }
        os << "\t\t\t\"Intersection\": [";
        aux = 0;
        if(cluster->intersection.empty()) {
            os << " ] \n\t\t\t";
        } else {
            for(int label : cluster->intersection) {
                if(aux < cluster->intersection.size() - 1) {
                    os << label << ", ";
                } else {
                    os << label << "] \n\t\t}";
                }
                aux++;
            }
        }
        if(i < nClusters) {
            os << ",\n";
        }
        delete cluster;
    }
    os << "]\n}\n";
}

void printPartitions(std::ostream &os, std::vector<std::vector<Cluster *>> &partitions) {
    os << "{\"Partitions\": [\n";
    for(int l = 0; l < partitions.size(); l++) {
        printPartition(os, partitions[l], true);
        if(l < partitions.size() - 1) {
            os << ",";
        }
    }
    os << "]}\n";
}

int main(int argc, char ** argv) {
    if(argc < 3) {
        std::cout << "Use ./<executable name> <input file name> <problem type, 0 for cover & 1 for partition & 2 for tender partition> <output file name (optional)>\n";
        return -1;
    }
    std::ofstream output;
    if(argc >= 4) {
        output.open(argv[3]);
        if(!output) {
            std::cout << "Error while handling output file: " << argv[3] << "\n";
            exit(-1);
        }
    }
    // Leitura da instância
    char *fileName = argv[1];
    Instance instance(fileName, std::regex_search(fileName, std::regex("N_\\d+_L_\\d+_d_\\d+\\.\\d+\\.txt")));
    // Tipo de problema
    int problemType = atoi(argv[2]);
    // Resolver o problema
    auto start = std::chrono::steady_clock::now();
    double obj;
    switch (problemType) {
        case 0: { // Cobertura
            // Resolver a instância pelo algoritmo da árvore
            std::vector<TreeNode *> cover = treeSolve(instance);
            // Imprimir o resultado
            obj = printClusters(argc >= 4 ? output : std::cout, cover);
            break;
        } case 1: { // Partição
            PairSolution solver(instance, false);
            std::vector<Cluster *> partition = solver.getSolution();
            obj = solver.getObj();
            printPartition(argc >= 4 ? output : std::cout, partition);
            break;
        } case 11: { // Partição com restrições de mesma interseção
            PairSolution solver(instance, true);
            std::vector<Cluster *> partition = solver.getSolution();
            obj = solver.getObj();
            printPartition(argc >= 4 ? output : std::cout, partition);
            break;
        } case 2: { // Partição por licitação
            obj = 0;
            std::vector<std::vector<Cluster *>> partitions;
            for(int l = 0; l < instance.L; l++) {
                std::cout << "Tender " << l << "/" << instance.L << "\n";
                std::set<int> participants;
                for(int i = 0; i < instance.N; i++) {
                    if(instance.matrix[i][l]) {
                        participants.insert(i);
                    }
                }
                PairSolution solver(instance, participants, false);
                partitions.push_back(solver.getSolution());
                obj += solver.getObj();
            }
            printPartitions(argc >= 4 ? output : std::cout, partitions);
            break;
        } case 21: { // Partição por licitação com restrições de mesma interseção
            obj = 0;
            std::vector<std::vector<Cluster *>> partitions;
            for(int l = 0; l < instance.L; l++) {
                std::cout << "Tender " << l << "/" << instance.L << "\n";
                std::set<int> participants;
                for(int i = 0; i < instance.N; i++) {
                    if(instance.matrix[i][l]) {
                        participants.insert(i);
                    }
                }
                PairSolution solver(instance, participants, true);
                partitions.push_back(solver.getSolution());
                obj += solver.getObj();
            }
            printPartitions(argc >= 4 ? output : std::cout, partitions);
            break;
        } default: {
            std::cout << "Use 0 for cover & 1 for partition & 2 for tender partition\n";
            if(argc >= 4) {
                output.close();
            }
            exit(-1);
        }
    }
    auto end = std::chrono::steady_clock::now();

    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    std::cout << "Objective: " << obj << "\n";
    // Fechar o arquivo de entrada, caso aplicável
    if(argc >= 4) {
        output.close();
    }
    return 0;
}
