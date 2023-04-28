//
//  Output.hpp
//  pcicTree
//
//  Created by David Galvao on 13/04/23.
//

#ifndef Output_hpp
#define Output_hpp

#include <iostream>
#include <ostream>
#include <fstream>

#include "Instance.hpp"
#include "TreeSolution.hpp"
#include "Util.hpp"
#include "Metrics.hpp"

void printCluster(std::ostream &, TreeNode *, int, int &, int &);
double printClusters(std::ostream &, std::vector<TreeNode *> &);
void printPartition(std::ostream &, std::vector<Cluster *> &, bool=false);
void printPartitions(std::ostream &, std::vector<std::vector<Cluster *>> &);
void printMetrics(std::ostream &, Instance &, std::vector<Cluster *> &);
void printMatrix(std::ostream &, Instance &);

#endif /* Output_hpp */
