#ifndef RECIPIENTANONYMITY_H
#define RECIPIENTANONYMITY_H

#include "anoa.h"
#include "nodeweight.h"
#include "functions.h"
#include "weightedgraph.h"
#include <boost/thread/thread.hpp>

#include "relationshipanonymity.h"

/*
 * This class is used to specify how compromising  nodes influences the 
 * anonymity guarantee. E.g., for SENDER ANONYMITY, 
 * comprimizing the entry node results in a distinguishing event.
 */
class RecipientAnonymity : public AnoA
{
	public:
		// class constructor
		RecipientAnonymity(Graph* g, WeightedGraph* wg1, WeightedGraph* wg2, double eps);
		// class destructor
		~RecipientAnonymity();
		void outputGuarantees(const unsigned int k, Graph* g); // k is the number of bad nodes
		double computeDelta(const unsigned int k, Graph* g); // k is the number of bad nodes
		void print();
		double* usedAsEntry1;
		double* usedAsEntry2;
	private:
		boost::timed_mutex mutex_instance_handling;
		unsigned int* instancesEntryNum;
		double* instancesMiddle;
		double* instancesThisEntrySeesMiddle1;
		double* instancesThisEntrySeesMiddle2;
		double deltaFromSeeingEntryNodes;
		vector<double> deltas;
		vector<double> deltasExit;
		vector<double> deltasMiddle;
		vector<double> deltasEntry;
		unsigned int active_instances;
		unsigned int gSize;
		double epsilon;
		

};

#endif // RECIPIENTANONYMITY_H
