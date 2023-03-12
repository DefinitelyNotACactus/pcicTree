//
//  TreeSolution.hpp
//  pcicTree
//
//  Created by David Galvao on 19/10/22.
//

#ifndef TreeSolution_hpp
#define TreeSolution_hpp

#include <vector>
#include <set>
#include <string>
#include <unordered_map>

#include "Instance.hpp"
#include "Util.hpp"

struct TreeNode : Cluster {
    double independentProb, prioriProb, lift, leverage, clique, normClique;
    bool intersectionLeaf;
    int index;
    std::vector<TreeNode *> childs;
};

std::vector<TreeNode *> treeSolve(Instance &);
TreeNode * buildNode(Instance &, TreeNode *, std::set<int> &, const int, std::vector<TreeNode *> &, std::unordered_map<std::string, std::set<int> *> &);

bool isSubIntersection(const std::vector<std::set<int> *> &, const std::set<int> &);
bool isSameIntersection(const std::set<int> &, const std::set<int> &);
//bool isInIntersections(const std::vector<std::set<int> *> &, const std::set<int> &);
bool isInIntersections(const std::unordered_map<std::string, std::set<int> *> &, const std::string &);

#endif /* TreeSolution_hpp */
