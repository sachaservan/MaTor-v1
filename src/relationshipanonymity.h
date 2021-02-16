
#ifndef RELATIONSHIPANONYMITY_H
#define RELATIONSHIPANONYMITY_H

#include "anoa.h"
#include "nodeweight.h"
#include "functions.h"
#include "weightedgraph.h"
#include <boost/thread/thread.hpp>

#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>
#include <deque>
#include <set>

#include "edgemap.hpp"

/*
 * This class is used to specify how compromising  nodes influences the 
 * anonymity guarantee. E.g., for RELATIONSHIP ANONYMITY, 
 * compromising both entry and exit node results in a distinguishing event,
 * but it's not the only one out there.
 */
class RelationshipAnonymity : public AnoA
{
	public:
		// class constructor
		RelationshipAnonymity(Graph* g, WeightedGraph* wg1, WeightedGraph* wg2, double eps);
		// class destructor
		~RelationshipAnonymity();
		double computeDelta(const unsigned int k, Graph* g); // k is the number of bad nodes
		void outputGuarantees(const unsigned int k, Graph* g); // k is the number of bad nodes
		void print();

	private:
		void addEnMiEdge(int entry, int middle, double weight);
		void addNewEntryWeight(int entry, double weight);

		double epsilon;
	
		edgemap<double> weights;
		std::multiset<double> EdgeWeights;
		
		WeightedGraph *wg1, *wg2;
		std::multiset<double> entryWeights;
		std::map<int, double> entryWeightsM;
};

#endif // RELATIONSHIPANONYMITY_H
