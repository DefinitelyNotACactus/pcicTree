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
    if(argc < 3 || argc > 5) {
        std::cout << "Use ./<executable name> <input file name> <problem type> <input partition if problem type == 3 or 4> <output file name (optional)>\n";
        std::cout << "Supported problems:\n0-Cover\n1-Partition\n2-Tender Partition\n3-Presolved Partition\n4-Convert Output\n5-Print Matrix\n";
        return -1;
    }
    // Leitura da instância
    char *fileName = argv[1];
    Instance instance(fileName, std::regex_search(fileName, std::regex("N_\\d+_L_\\d+_d_\\d+\\.\\d+\\.txt")));
    // Tipo de problema
    int problemType = atoi(argv[2]);
    bool solveProblem = (problemType != 3 && problemType != 4);
    // Definição da saída
    std::ofstream output;
    if((argc == 4 && solveProblem) || (argc == 5 && !solveProblem)) {
        int argvOutput = (problemType == 3 || problemType == 4)? 4 : 3;
        output.open(argv[argvOutput], std::ios_base::app);
        if(!output) {
            std::cout << "Error while handling output file: " << argv[argvOutput] << "\n";
            exit(-1);
        }
        if(problemType != 4) {
            output << "Instance: " << fileName << "\n";
        }
    } else {
        std::cout << "Instance: " << fileName << "\n";
    }
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
        } case 1:
          case 11: { // Partição
            PairSolution solver(instance, problemType == 11);
            std::vector<Cluster *> partition = solver.getSolution();
            obj = solver.getObj();
            printPartition(argc >= 4 ? output : std::cout, instance, partition);
            printMetrics(argc >= 4 ? output : std::cout, instance, partition);
            break;
        } case 2:
          case 21: { // Partição por licitação
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
                PairSolution solver(instance, participants, problemType == 21);
                partitions.push_back(solver.getSolution());
                obj += solver.getObj();
            }
            printPartitions(argc >= 4 ? output : std::cout, instance, partitions);
            break;
        } case 3: { // Partição pré-resolvida
            std::vector<Cluster *> partition = readPartition(instance, argv[3]);
            printMetrics(argc >= 5 ? output : std::cout, instance, partition);
            obj = icpObjective(instance, partition);
            break;
        } case 4: { // Converter saída
            std::vector<Cluster *> partition = readPartition(instance, argv[3]);
            printPartition(argc >= 5 ? output : std::cout, instance, partition);
            obj = icpObjective(instance, partition);
            break;
        } case 5: { // Matrix
            printMatrix(argc >= 4 ? output : std::cout, instance);
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
    if(argc >= 4 && problemType != 4) {
        output << "Objective: " << obj << "\n";
        output << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }
    if (problemType != 5) {
        std::cout << "Objective: " << obj << "\n";
        std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }
    // Fechar o arquivo de entrada, caso aplicável
    if(argc >= 4) {
        output.close();
    }
    return 0;
}
