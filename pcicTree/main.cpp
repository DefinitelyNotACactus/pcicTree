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
#include "Metrics.hpp"
#include "Output.hpp"

int main(int argc, char ** argv) {
    if(argc < 3) {
        std::cout << "Use ./<executable name> <input file name> <problem type, 0 for cover & 1 for partition & 2 for tender partition> <output file name (optional)>\n";
        return -1;
    }
    // Leitura da instância
    char *fileName = argv[1];
    Instance instance(fileName, std::regex_search(fileName, std::regex("N_\\d+_L_\\d+_d_\\d+\\.\\d+\\.txt")));
    // Definição da saída
    std::ofstream output;
    if(argc >= 4) {
        output.open(argv[3], std::ios_base::app);
        if(!output) {
            std::cout << "Error while handling output file: " << argv[3] << "\n";
            exit(-1);
        }
        output << "Instance: " << fileName << "\n";
    } else {
        std::cout << "Instance: " << fileName << "\n";
    }
    // Tipo de problema
    int problemType = atoi(argv[2]);
    // Resolver o problema
    auto start = std::chrono::steady_clock::now();
    double obj = 0;
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
//            printPartition(argc >= 4 ? output : std::cout, partition);
            printMetrics(argc >= 4 ? output : std::cout, instance, partition);
            break;
        } case 11: { // Partição com restrições de mesma interseção
            PairSolution solver(instance, true);
            std::vector<Cluster *> partition = solver.getSolution();
            obj = solver.getObj();
//            printPartition(argc >= 4 ? output : std::cout, partition);
            printMetrics(argc >= 4 ? output : std::cout, instance, partition);
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
    if(argc >= 4) {
        output << "Objective: " << obj << "\n";
        output << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }
    std::cout << "Objective: " << obj << "\n";
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    // Fechar o arquivo de entrada, caso aplicável
    if(argc >= 4) {
        output.close();
    }
    return 0;
}
