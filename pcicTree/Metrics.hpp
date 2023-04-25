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

double distanceFunction(const Instance &, const int, const std::set<int> &);
double distanceFunctionJaccard(Instance &, const int, const int);
double averageSilhouetteIntersectionIndex(const Instance &, const std::vector<Cluster *> &);

double silhouetteWidth(Instance &, const std::vector<Cluster *> &);
double categoricalUtility(const Instance &, const std::vector<Cluster *> &);
double entropy(const Instance &, const std::vector<Cluster *> &);

double kModesObjective(const Instance &, const std::vector<Cluster *> &);
double icpObjective(const Instance &, const std::vector<Cluster *> &);

#endif /* Metrics_hpp */
