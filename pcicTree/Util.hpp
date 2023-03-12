//
//  Util.hpp
//  pcicTree
//
//  Created by David Galvao on 02/02/23.
//

#ifndef Util_hpp
#define Util_hpp

#include <vector>
#include <set>
#include "Instance.hpp"

struct Cluster {
    std::vector<int> cluster;
    std::set<int> intersection;
};

std::set<int> getIntersection(Instance &, const std::vector<int> &, const std::set<int> &, const int);
std::set<int> getIntersection(Instance &, const int, const int);
std::set<int> getUnion(Instance &, const int, const int);

#endif /* Util_hpp */
