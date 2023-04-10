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
#include <string>
#include <unordered_map>

#include "Instance.hpp"

struct Cluster {
    std::vector<int> elements;
    std::set<int> intersection;
};

std::set<int> getIntersection(Instance &, const std::vector<int> &, const std::set<int> &, const int);
std::set<int> getIntersection(Instance &, const int, const int);
std::set<int> getUnion(Instance &, const int, const int);

inline bool isInIntersections(const std::unordered_map<std::string, std::set<int> *> &intersections, const std::string &candidate) {
    return (intersections.find(candidate) != intersections.end());
}

inline std::string intersectionString(const std::set<int> &intersection) {
    std::string str = "";
    for(int l : intersection) {
        str += std::to_string(l);
        str += "x";
    }
    return str;
}

#endif /* Util_hpp */
