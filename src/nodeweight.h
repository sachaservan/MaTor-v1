#ifndef NODEWEIGHT_H
#define NODEWEIGHT_H

#include "node.h"
/*
 * Is used for calculations on the nodes 
 * (how often are they used as entry, middle or exit nodes?
 * What is the quotient of this usage in comparison
 * to another scenario?)
 */
class NodeWeight
{
	public:
		// class constructor
		NodeWeight(const Node* n, int mypos);
		// class destructor
		~NodeWeight();
		void computeQuotients(NodeWeight* n);
		const Node* getNode() const;
		double used_as_entry;
		double quotient_entry;
		double used_as_middle;
		double quotient_middle;
		double used_as_exit;
		double quotient_exit;
		double related_entry_bandwidth;
		double related_middle_bandwidth;
		double entryWeight;
		double middleWeight;
		double exitWeight;
		int posInGraph;
		vector<double> I_am_entry_and_see_as_middle;
		vector<double> I_am_middle_and_see_as_exit;
	private:
		const Node* node;
};

#endif // NODEWEIGHT_H
