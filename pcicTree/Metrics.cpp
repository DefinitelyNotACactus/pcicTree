//
//  Metrics.cpp
//  pcicTree
//
//  Created by David Galvao on 28/03/23.
//

#include "Metrics.hpp"

#include <vector>

#define EPSILON 0.00001

double distanceFunction(const Instance & instance, const int i, const std::set<int> &intersection) {
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

double distanceFunctionJaccard(Instance & instance, const int i, const int j) {
    double unionSize = 0;
    std::set<int> intersection = getIntersection(instance, i, j);
    // Compute union size
    for(int r = 0; r < instance.L; r++) {
        if(instance.matrix[i][r] || instance.matrix[j][r]) {
            unionSize++;
        }
    }
    return 1.0 - intersection.size() / unionSize;
}

double silhouetteWidth(Instance & instance, const std::vector<Cluster *> &partition) {
    double *a = new double[instance.N], *cl = new double[instance.N], s = 0, constA;
    // Compute a
    for(int c = 0; c < partition.size(); c++) {
        constA = 1.0 / (partition[c]->elements.size() - 1.0);
        for(int i = 0; i < partition[c]->elements.size(); i++) {
            a[partition[c]->elements[i]] = 0;
            for(int j = 0; j < partition[c]->elements.size(); j++) {
                if(j == i) continue;
                a[partition[c]->elements[i]] += distanceFunctionJaccard(instance, partition[c]->elements[i], partition[c]->elements[j]);
            }
            a[partition[c]->elements[i]] *= constA;
            cl[partition[c]->elements[i]] = c;
        }
    }
    // Compute b
    for(int i = 0; i < instance.N; i++) {
        double minDistance = INFINITY, distance;
        // Get smallest distance between all clusters that i is not contained in
        for(int c = 0; c < partition.size(); c++) {
            if(cl[i] == c) continue;
            distance = 0;
            for(int element : partition[c]->elements) {
                distance += distanceFunctionJaccard(instance, i, element);
            }
            distance *= (double) 1 / partition[c]->elements.size();
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

double averageSilhouetteIntersectionIndex(const Instance & instance, const std::vector<Cluster *> &partition) {
    double *a = new double[instance.N], *cl = new double[instance.N], s = 0;
    // Compute a
    for(int c = 0; c < partition.size(); c++) {
        for(int element : partition[c]->elements) {
            a[element] = distanceFunction(instance, element, partition[c]->intersection);
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
    double sumCatUtility = 0, sumCluster, cljqTrue, cljqFalse, probCondTrue, probCondFalse;
    std::vector<double>probAjqTrue(instance.L, 0);
    std::vector<double>probAjqFalse(instance.L, 0);
    // Compute resources probabilities P({a_j}^q)
    for(int i = 0; i < instance.N; i++) {
        for(int j = 0; j < instance.L; j++) {
            if(instance.matrix[i][j]) {
                probAjqTrue[j] += 1 / (double) instance.N;
            } else {
                probAjqFalse[j] += 1 / (double) instance.N;
            }
        }
    }
    // Compute catUtility
    for(int i = 0; i < partition.size(); i++) {
        sumCluster = 0;
        for(int j = 0; j < instance.L; j++) {
            cljqTrue = cljqFalse = 0;
            for(int element : partition[i]->elements) {
                if(instance.matrix[element][j]) {
                    cljqTrue += 1;
                } else {
                    cljqFalse += 1;
                }
            }
            probCondTrue = cljqTrue / (double) partition[i]->elements.size();
            probCondFalse = cljqFalse / (double) partition[i]->elements.size();

            sumCluster += probCondTrue * probCondTrue - probAjqTrue[j] * probAjqTrue[j];
            sumCluster += probCondFalse * probCondFalse - probAjqFalse[j] * probAjqFalse[j];
        }
        sumCatUtility += sumCluster * ((double) partition[i]->elements.size() / instance.N);
    }
    return sumCatUtility;
}

double entropy(const Instance & instance, const std::vector<Cluster *> &partition) {
    double sumEntropy = 0, sumCluster, cljqTrue, cljqFalse, probCondTrue, probCondFalse;
    // Compute entropy
    for(int i = 0; i < partition.size(); i++) {
        sumCluster = 0;
        for(int j = 0; j < instance.L; j++) {
            cljqTrue = cljqFalse = 0;
            for(int element : partition[i]->elements) {
                if(instance.matrix[element][j]) {
                    cljqTrue += 1;
                } else {
                    cljqFalse += 1;
                }
            }
            probCondTrue = cljqTrue / partition[i]->elements.size();
            sumCluster += (probCondTrue + EPSILON) * log(probCondTrue + EPSILON);
            
            probCondFalse = cljqFalse / partition[i]->elements.size();
            sumCluster += (probCondFalse + EPSILON) * log(probCondFalse + EPSILON);
        }
        sumEntropy += sumCluster * partition[i]->elements.size();
    }
    return sumEntropy * -1;
}

double kModesObjective(const Instance & instance, const std::vector<Cluster *> &partition) {
    double obj = 0;
    for(Cluster * cluster : partition) {
        std::vector<double> clusterCenter(instance.L, 0);
        // Compute cluster center
        for(int element : cluster->elements) {
            for(int r = 0; r < instance.L; r++) {
                clusterCenter[r] += (instance.matrix[element][r] == 1)? 1.0 / (double) cluster->elements.size() : 0;
            }
        }
        // Compute obj for cluster
        for(int element : cluster->elements) {
            for(int r = 0; r < instance.L; r++) {
                if((instance.matrix[element][r] && clusterCenter[r] < 0.5) || (!instance.matrix[element][r] && clusterCenter[r] >= 0.5)) {
                    obj += 1;
                }
            }
        }
    }
    return obj;
}
