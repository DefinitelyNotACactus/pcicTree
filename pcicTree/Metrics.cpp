//
//  Metrics.cpp
//  pcicTree
//
//  Created by David Galvao on 28/03/23.
//

#include "Metrics.hpp"

#include <vector>

double distanceFunction(const Instance & instance, int i, const std::set<int> &intersection) {
    double totalDistance = 0;
    bool inIntersection;
    for(int r = 0; r < instance.L; r++) {
        inIntersection = (intersection.find(r) != intersection.end());
        if(instance.matrix[i][r] && !inIntersection) {
            totalDistance++;
        } else if (inIntersection) {
            totalDistance++;
        }
    }
    return totalDistance;
}

double averageSilhouetteIntersectionIndex(const Instance & instance, const std::vector<Cluster *> &partition) {
    double *a = new double[instance.N], *cl = new double[instance.N], s = 0;
//    std::vector<bool> inP(instance.N, false);
    // Compute a
    for(int c = 0; c < partition.size(); c++) {
        for(int element : partition[c]->elements) {
            a[element] = distanceFunction(instance, element, partition[c]->intersection);
//            inP[element] = true;
            cl[element] = c;
        }
    }
    // Compute b
    for(int i = 0; i < instance.N; i++) {
        double minDistance = INFINITY, distance;
        // Get smallest distance between all clusters that i is not contained in
        for(int c = 0; c < partition.size(); c++) {
            if(cl[i] == c) continue;
            distance = distanceFunction(instance, i, partition[c]->intersection);
            if(distance < minDistance) {
                minDistance = distance;
            }
        }
        // Compute s
        s += (a[i] > minDistance)? (minDistance - a[i]) / a[i] : (minDistance - a[i]) / minDistance;
    }
    
    delete [] a;
    delete [] cl;
    
    return s / instance.N;
}

double categoricalUtility(const Instance & instance, const std::vector<Cluster *> &partition) {
    double sumCatUtility = 0, sumCluster, cljq, probCond;
    std::vector<double>probAjq(instance.L, 0);
    // Compute resources probabilities
    for(int i = 0; i < instance.N; i++) {
        for(int j = 0; j < instance.L; j++) {
            probAjq[j] += (instance.matrix[i][j] == true) ? 1 / (double) instance.N : 0;
        }
    }
    // Compute catUtility
    for(int i = 0; i < partition.size(); i++) {
        sumCluster = cljq = 0;
        for(int j = 0; j < instance.L; j++) {
            for(int element : partition[i]->elements) {
                cljq += (instance.matrix[element][j] == true) ? 1 : 0;
            }
            probCond = cljq / partition[i]->elements.size();
            sumCluster += probCond * probCond - probAjq[j] * probAjq[j];
        }
        sumCatUtility += sumCluster * ((double) partition[i]->elements.size() / instance.N);
    }
    return sumCatUtility;
}

double entropy(const Instance & instance, const std::vector<Cluster *> &partition) {
    double sumEntropy = 0, sumCluster, cljq, probCond;
    // Compute entropy
    for(int i = 0; i < partition.size(); i++) {
        sumCluster = cljq = 0;
        for(int j = 0; j < instance.L; j++) {
            for(int element : partition[i]->elements) {
                cljq += (instance.matrix[element][j] == true) ? 1 : 0;
            }
            probCond = cljq / partition[i]->elements.size();
            if(probCond > 0) {
                sumCluster += probCond * log(probCond);
            }
        }
        sumEntropy += sumCluster * partition[i]->elements.size();
    }
    return sumEntropy * -1;
}
