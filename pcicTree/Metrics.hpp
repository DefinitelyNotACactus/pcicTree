//
//  Metrics.hpp
//  pcicTree
//
//  Created by David Galvao on 28/03/23.
//

#ifndef Metrics_hpp
#define Metrics_hpp

#include "Instance.hpp"
#include "Metrics.hpp"
#include "Util.hpp"

#include <set>

double distanceFunction(const Instance &, int , const std::set<int> &);
double averageSilhouetteIntersectionIndex(const Instance &, const std::vector<Cluster *> &);

#endif /* Metrics_hpp */
