
#ifndef ANOA_H
#define ANOA_H

const int ENTRY_NODE = 1;
const int MIDDLE_NODE = 2;
const int EXIT_NODE = 3;

#include "nodeweight.h"
#include "graph.h"
/*
 * This class is used to specify how compromising  nodes influences the 
 * anonymity guarantee. E.g., for SENDER ANONYMITY, 
 * comprimizing the entry node results in a distinguishing event.
 */
class AnoA
{
	public:
		// class constructor
		AnoA(){};
		// class destructor
		virtual ~AnoA(){}
		virtual void outputGuarantees(const unsigned int k, Graph* g){}; // k is the number of bad nodes
		virtual double computeDelta(const unsigned int k, Graph* g){return 0;}; // k is the number of bad nodes
		virtual void print(){};
	private:
};

#endif // ANOA_H
