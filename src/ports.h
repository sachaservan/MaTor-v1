#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <vector>

#include "node.h"
#include "graph.h"

using namespace std;
/*
 * Describes a list of open ports.
 */
class Node;
class Graph;
class Ports
{
	public:
		// class constructor
		Ports();
		// class destructor
		~Ports();
		void addPort(int port);
		unsigned int size() const;
		int getPort(unsigned int index) const;
		void computeExitSupport(Graph* g);
		int getBestSupport() const;
	private:
		vector<int> ports;
		int bestSupport;
		int totalSupportedExitBandwidth;
};

#endif // CONNECTIONS_H
