//
//  PairSolution.hpp
//  pcicTree
//
//  Created by David Galvao on 02/02/23.
//

#ifndef PairSolution_hpp
#define PairSolution_hpp

#include <stack>
#include <vector>
#include <set>
#include <string>

#include "Instance.hpp"
#include "Util.hpp"

#define TIME_LIMIT 60000

struct Candidate {
    int intersectionSize;
    std::string intersectionStr;
    int i, j;
    
    Candidate(int i, int j, int intersectionSize, std::string intersectionStr) : i(i), j(j), intersectionSize(intersectionSize), intersectionStr(intersectionStr) { }
    
    bool operator >(const Candidate &b) const {
        return intersectionSize > b.intersectionSize;
    }
};

class PairSolution {
private:
    Instance &instance;
    
    std::unordered_map<std::string, bool> intersections;
    std::vector<Candidate> candidates;
    std::stack<Candidate *> bestSolution;
    std::chrono::steady_clock::time_point start;
    int bestObj, nClusters;
    int **theoreticalObj;
    const bool restrictIntersection;
    
    void computeTheoreticalObj();
    void createCandidates(std::set<int> participants = {});
    void solveRoot();
    int solveNode(std::vector<bool> &, std::stack<Candidate *> &, const int, const int, int);
    
public:
    PairSolution(Instance &, bool);
    PairSolution(Instance &, std::set<int>, bool);
    
    std::vector<Cluster *> getSolution();
    inline int getObj() { return bestObj; }
};
#endif /* PairSolution_hpp */
