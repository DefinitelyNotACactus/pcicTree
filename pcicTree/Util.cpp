//
//  Util.cpp
//  pcicTree
//
//  Created by David Galvao on 02/02/23.
//

#include "Util.hpp"

/* Obtém a união de um par de vértices */
std::set<int> getUnion(Instance &g, const int vertexA, const int vertexB) {
    std::set<int> u = g.vertices[vertexA];
    
    for(int label : g.vertices[vertexB]) {
        u.insert(label);
    }
    
    return u;
}

/* Obtém a intersecção de um par de vértices */
std::set<int> getIntersection(Instance &g, const int vertexA, const int vertexB) {
    if(g.verticesIntersections[vertexA][vertexB].size() > 0) {
        return g.verticesIntersections[vertexA][vertexB];
    }
    std::set<int> intersection;
//    for(int label = 0; label < g.L; label++) {
//        if(g.isInVertex(vertexA, label) && g.isInVertex(vertexB, label)) {
//            intersection.insert(label);
//        }
//    }
    for(int label : g.vertices[vertexA]) {
        if(g.isInVertex(vertexB, label)) {
            intersection.insert(label);
        }
    }
    g.verticesIntersections[vertexA][vertexB] = intersection;
    g.verticesIntersections[vertexB][vertexA] = intersection;
    
    return intersection;
}

/* Obtém a intersecção de um grupo de vértices (cluster) */
std::set<int> getIntersection(Instance &g, const std::vector<int> &cluster, const std::set<int> &intersection, const int vertex) {
    std::set<int> newIntersection = intersection;
    for(int label : intersection) {
        for(int element : cluster) {
            if(!g.isInVertex(element, label) || !g.isInVertex(vertex, label)) {
            //if(!g->intersections[label][element][vertex]) {
                newIntersection.erase(label);
                break;
            }
        }
    }
    return newIntersection;
}
