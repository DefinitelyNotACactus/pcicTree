//
//  Instance.hpp
//  pcicTree
//
//  Created by David Galvao on 11/11/22.
//

#ifndef Instance_hpp
#define Instance_hpp

#include <vector>
#include <set>
#include <string>

class Instance {
public:
    int N, L;
    std::vector<std::set<int>> vertices;
    std::set<int> **verticesIntersections;
    bool **matrix;
    
    Instance(char *, bool);
    ~Instance();
    
    inline bool isInVertex(const int &vertex, const int &label) { return matrix[vertex][label]; }

private:
    void readInstance(char *);
    void readInstanceLegacy(char *);
};
#endif /* Instance_hpp */
