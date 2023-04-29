//
//  Output.cpp
//  pcicTree
//
//  Created by David Galvao on 13/04/23.
//

#include "Output.hpp"

void printCluster(std::ostream &os, TreeNode * cluster, int nClusters, int &i, int &aux) {
    os << "\t\t{\n\t\t\t\"id\": " << i++ << ", \n\t\t\t\"Elements\": [";
    aux = 0;
    for(int element : cluster->elements) {
        if(aux < cluster->elements.size() - 1) {
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
//        delete cluster;
    }
    os << "]\n}\n";
    
    return obj;
}

void printPartition(std::ostream &os, Instance &instance, std::vector<Cluster *> &clusters, bool ignoreEmptyIntersection) {
    int i = 0, aux = 0;
    os << "{\n\t\"Clusters\": [\n";
    for(Cluster * cluster : clusters) {
        if(ignoreEmptyIntersection && cluster->intersection.empty()) continue;
        os << "\t\t{\n\t\t\t\"id\": " << i++ << ", \n\t\t\t\"Elements\": [";
        aux = 0;
        for(int element : cluster->elements) {
            if(aux < cluster->elements.size() - 1) {
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
                    os << label << "], \n\t\t\t";
                }
                aux++;
            }
        }
        os << "\"P_cond\": " << cluster->intersection.size() / (double) instance.numResourcesUsed << ", \n\t\t}";
        if(i < clusters.size()) {
            os << ",\n";
        }
//        delete cluster;
    }
    os << "]\n}\n";
}

void printPartitions(std::ostream &os, Instance &instance, std::vector<std::vector<Cluster *>> &partitions) {
    os << "{\"Partitions\": [\n";
    for(int l = 0; l < partitions.size(); l++) {
        printPartition(os, instance, partitions[l], true);
        if(l < partitions.size() - 1) {
            os << ",";
        }
    }
    os << "]}\n";
}

void printMetrics(std::ostream &os, Instance &instance, std::vector<Cluster *> &partition) {
    os << "Silhouette Intersection: " << averageSilhouetteIntersectionIndex(instance, partition) << "\n";
    os << "Silhouette Width: " << silhouetteWidth(instance, partition) << "\n";
    os << "Categorical Utility: " << categoricalUtility(instance, partition) << "\n";
    os << "Entropy: " << entropy(instance, partition) << "\n";
    os << "Number of clusters: " << partition.size() << "\n";
    os << "K-modes Objective: " << kModesObjective(instance, partition) << "\n";
}

void printMatrix(std::ostream &os, Instance &instance) {
    os << instance.N << " " << instance.L << "\n";
    for(int i = 0; i < instance.N; i++) {
        for(int j = 0; j < instance.L - 1; j++) {
            os << instance.matrix[i][j] << " ";
        }
        os << instance.matrix[i][instance.L - 1] << "\n";
    }
}
