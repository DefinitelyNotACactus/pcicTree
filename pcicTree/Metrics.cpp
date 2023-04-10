//
//  Metrics.cpp
//  pcicTree
//
//  Created by David Galvao on 28/03/23.
//

#include "Metrics.hpp"

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
