//
//  TreeSolution.cpp
//  pcicTree
//
//  Created by David Galvao on 19/10/22.
//

#include "TreeSolution.hpp"

#include <stack>
#include <cmath>
#include <iostream>

bool isSubIntersection(const std::vector<std::set<int> *> &intersections, const std::set<int> &candidate) {
    bool sub;
    for(auto intersection : intersections) {
        if(candidate.size() <= intersection->size()) {
            sub = true;
            for(int element : candidate) {
                if(intersection->find(element) == intersection->end()) {
                    sub = false;
                    break;
                }
            }
            if(sub) return true;
        }
    }
    return false;
}

bool isSameIntersection(const std::set<int> &intersectionA, const std::set<int> &intersectionB) {
    if(intersectionA.size() != intersectionB.size()) return false;
    for(int element : intersectionA) {
        if(intersectionB.find(element) == intersectionB.end()) return false;
    }
    return true;
}

std::vector<TreeNode *> treeSolve(Instance &g) {
    std::vector<std::set<int> *> intersections;
    std::unordered_map<std::string, std::set<int> *> clusterIntersection; // Vetor para indicar quais intersecções estão presentes na árvore
    std::vector<TreeNode *> clusterNodes;
    std::stack<TreeNode *> nodeStack;

    for(int i = 0; i < g.N - 1; i++) {
        std::cout << "Creating root: " << i << " / " << g.N << "\n";
        TreeNode root;
        root.index = i;
        root.cluster.push_back(i);
        // Criar os filhos da raiz
        for(int j = i + 1; j < g.N; j++) {
            std::set<int> intersection = getIntersection(g, i, j); // Pegar a intersecção do par (i, j)
            // Apenas criar um filho se a intersecção do par não é vazia e não é uma subintersecção de outro par
            if(!intersection.empty() && !isSubIntersection(intersections, intersection)) {
                TreeNode *child = buildNode(g, &root, intersection, j, clusterNodes, clusterIntersection);
                root.childs.push_back(child);
                intersections.push_back(&root.childs[root.childs.size() - 1]->intersection);
            }
        }
        for(TreeNode * child : root.childs) nodeStack.push(child);
    }
    // Desalocar os nós que não formam clusters
//    while (!nodeStack.empty()) {
//        TreeNode * top = nodeStack.top();
//        nodeStack.pop();
//        for(TreeNode * child : top->childs) nodeStack.push(child);
//        if(!top->intersectionLeaf && !top->childs.empty()) delete top;
//    }
    // Processar as probabilidades dos clusters
//    std::cout << "Processing clusters probabilities\n";
//    double probI;
    int c = 0;
    for(TreeNode * node : clusterNodes) {
        std::cout << "Computing metrics of cluster: " << ++c << " / " << clusterNodes.size() << "\n";
        // Calcular a probabilidade independente
        node->independentProb = 0;
        for(int i = 0; i < node->cluster.size() - 1; i++) {
            node->independentProb += log(g.vertices[node->cluster[i]].size() / (double) g.L);
//            probI = g.vertices[node->cluster[i]].size() / (double) g.L;
//            probI = log(g.vertices[node->cluster[i]].size() / (double) g.L);
//            for(int j = i + 1; j < node->cluster.size(); j++) {
//                node->independentProb += node->cluster.size() * (probI + log(g.vertices[node->cluster[j]].size() / (double) g.L));
//                node->independentProb += probI + log(g.vertices[node->cluster[j]].size() / (double) g.L);
//                node->independentProb *= pow(probI * g.vertices[node->cluster[j]].size() / (double) g.L, node->cluster.size());
//            }
        }
        node->independentProb = exp(node->independentProb);
        // Calcular a probabilidade a priori
        node->prioriProb = node->intersection.size() / (double) g.L;
//        for(int i = 0; i < node->cluster.size() - 1; i++) {
//            for(int j = i + 1; j < node->cluster.size(); j++) {
//                node->prioriProb += getIntersection(g, node->cluster[i], node->cluster[j]).size();
//                node->prioriProb += g.verticesIntersections[node->cluster[i]][node->cluster[j]].size();
//            }
//        }
//        node->prioriProb /= g.L;
        // Calcular a alavancagem
        if(node->independentProb > 0) {
            node->lift = node->prioriProb / node->independentProb;
        } else {
            node->lift = 0;
        }
        node->leverage = node->prioriProb - node->independentProb;
        // Calcular o clique do cluster
        node->clique = 0;
        int nPairs = 0;
        for(int i = 0; i < node->cluster.size() - 1; i++) {
            for(int j = i + 1; j < node->cluster.size(); j++) {
                nPairs += 1;
                node->clique += getIntersection(g, node->cluster[i], node->cluster[j]).size() / (double) getUnion(g, node->cluster[i], node->cluster[j]).size();
            }
        }
        node->normClique = node->clique / (double) nPairs;
    }
    return clusterNodes;
}

/* Função para criação de um nó */
TreeNode * buildNode(Instance &g, TreeNode *parent, std::set<int> &intersection, const int vertex, std::vector<TreeNode *> &clusters, std::unordered_map<std::string, std::set<int> *> &clusterIntersection) {
//    std::vector<std::set<int> *> intersections;
    TreeNode *node = new TreeNode();
    
    node->index = vertex;
    node->intersection = intersection;
    node->cluster = parent->cluster;
    node->cluster.push_back(vertex);
    
    // Criar os filhos do nó
    node->intersectionLeaf = true;
    for(int i = vertex + 1; i < g.N; i++) {
        std::set<int> childIntersection = getIntersection(g, node->cluster, node->intersection, i); // Pegar a intersecção do cluster
        std::string hashIntersection = intersectionString(childIntersection);
        // Apenas criar um filho se a intersecção do par não é vazia e não é uma subintersecção de outro par
        if(!childIntersection.empty() && !isInIntersections(clusterIntersection, hashIntersection)) {
            // Checar se o filho possui a mesma intersecção, em caso positivo, significa que o nó não é uma folha de intersecção
            if(node->intersectionLeaf && isSameIntersection(intersection, childIntersection)) node->intersectionLeaf = false;
//            std::cout << "Parent: "<< vertex<< " creating child: " << i << " Intersection: {";
//            for(auto l : childIntersection) {
//                std::cout << " " << l << " ";
//            }
//            std::cout << "}\n";
            
            TreeNode *child = buildNode(g, node, childIntersection, i, clusters, clusterIntersection);
            node->childs.push_back(child);
//            intersections.push_back(&node->childs[node->childs.size() - 1]->intersection);
        }
    }
    for(TreeNode * child : node->childs) {
        if(!child->intersectionLeaf && !child->childs.empty()) {
            delete child;
        }
    }
    // Se o nó é uma folha da árvore ou de intersecção, seus elementos formam um cluster
    if(node->intersectionLeaf || node->childs.empty()){
//        std::cout << "Found cluster of intersection: {";
//        for(auto l : node->intersection) {
//            std::cout << " " << l << " ";
//        }
//        std::cout << "} Number of clusters: " << clusters.size() + 1 << "\n";
        std::string hashIntersection = intersectionString(node->intersection);

        clusters.push_back(node);
        clusterIntersection.insert({hashIntersection, &node->intersection});
//        clusterIntersection.push_back(&node->intersection);
    }

    return node;
}
