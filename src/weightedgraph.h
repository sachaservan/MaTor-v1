#ifndef WEIGHTEDGRAPH_H
#define WEIGHTEDGRAPH_H

#include<vector>
#include "nodeweight.h"
#include "graph.h"
#include "pathselection.h"
#include "preferences.h"
#include "anoa.h"
#include "ports.h"

/*
 * A collection of NodeWeights. 
 * Upon initialization the nodeweights are computed
 * depending on the path selection strategy.
 */
class PathSelection;
class WeightedGraph
{
	public:
		// class constructor
		WeightedGraph(Ports* ports, Graph* g, PathSelection* p, Preferences* prefs, bool simple = false);
		// class destructor
		~WeightedGraph();
		double getSumEntry();
		double getSumMiddle();
		double getSumExit();
		NodeWeight* getNode(const unsigned int i);
		unsigned int size()const;
		bool related(int i, int j) const{return this->mygraph->related(i,j);};
		unsigned int exitIndex(unsigned int i) const{return this->exitIndexes[i];};
		unsigned int reversedExitIndex(unsigned int i) const{return this->reversedExitIndexes[i];};
		unsigned int numberOfExits;
		Preferences* getPrefs() {return myprefs;};
		Ports* getPorts() {return myconn;};
		PathSelection* getPS() {return myps;};
		Graph* getGraph() {return mygraph;};
	private:
		vector<NodeWeight> nodes;
		vector<vector<double> > relativesJoinWeight;
		unsigned int* exitIndexes;
		unsigned int* reversedExitIndexes;
		double sumEntry;
		double sumMiddle;
		double sumExit;
		AnoA* anonymity;
		Graph* mygraph;
		Preferences* myprefs;
		Ports* myconn;
		PathSelection* myps;
};

#endif // WEIGHTEDGRAPH_H
