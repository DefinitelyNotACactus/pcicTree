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

#include "Instance.hpp"
#include "Util.hpp"

#define MAX_TICKS 500000

struct Candidate {
    int intersectionSize;
    int i, j;
    
    Candidate(int i, int j, int intersectionSize) : i(i), j(j), intersectionSize(intersectionSize) { }
    
    bool operator >(const Candidate &b) const {
        return intersectionSize > b.intersectionSize;
    }
};

class PairSolution {
private:
    Instance &instance;
    
    std::vector<Candidate> candidates;
    std::stack<Candidate *> bestSolution;
    int bestObj, nClusters, ticks;
    int **theoreticalObj;
    
    void computeTheoreticalObj();
    void createCandidates(std::set<int> participants = {});
    void solveRoot();
    void solveNode(std::vector<bool> &, std::stack<Candidate *> &, const int, const int, int);
    
public:
    PairSolution(Instance &);
    PairSolution(Instance &, std::set<int>);
    
    std::vector<Cluster *> getSolution();
    inline int getObj() { return bestObj; }
};

std::vector<Cluster *> pairSolve(Instance &);

#endif /* PairSolution_hpp */
