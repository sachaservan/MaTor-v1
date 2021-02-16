
#ifndef SENDERANONYMITY_H
#define SENDERANONYMITY_H

#include "anoa.h"
#include "nodeweight.h"
#include "functions.h"
#include "weightedgraph.h"
#include <boost/thread/thread.hpp>

/*
 * This class is used to specify how compromising  nodes influences the 
 * anonymity guarantee. E.g., for SENDER ANONYMITY, 
 * comprimizing the entry node results in a distinguishing event.
 */
class SenderAnonymity : public AnoA
{
	public:
		// class constructor
		SenderAnonymity(Graph* g, WeightedGraph* wSA);
		// class destructor
		~SenderAnonymity();
		void outputGuarantees(const unsigned int k, Graph* g); // k is the number of bad nodes
		double computeDelta(const unsigned int k, Graph* g); // k is the number of bad nodes
		void print();
	private:
		boost::timed_mutex mutex_instance_handling;
		//vector<double>* instances [USED_THREADS];
		vector<double> deltas;
		unsigned int active_instances;
		

};

#endif // SENDERANONYMITY_H
