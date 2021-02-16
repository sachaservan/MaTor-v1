
#ifndef PS_REDISTRIBUTED_H
#define PS_REDISTRIBUTED_H

#include "pathselection.h" // inheriting class's header file
#include "preferences.h"
#include "node.h"
#include "weightedgraph.h"
#include "ports.h"

/*
 * A path selection algorithm that distributes the bandwidth differently.
 */
class PSDistributor : public PathSelection
{
	public:
		// class constructor
		PSDistributor();
		PSDistributor(Ports* ports, Graph* g, Preferences* prefs, int bwpercentage = 100);
		// class destructor
		~PSDistributor();
		double getEntryWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const;
		double getMiddleWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const;
		double getExitWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const;
		bool entryExitAllowed(const Node* entry, const Node* exit, Preferences* prefs) const;

		double exitProb(const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const;	
		double entryProb(const Node* entry,const NodeWeight* entryW, const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const;
		double middleProb(const unsigned int& entry_num, const unsigned int& middle_num, const unsigned int& exit_num, WeightedGraph* wg) const;

		void print();
	private:
		double entrySum;
		double middleSum;
		double exitSum;
		long int max_entry_bandwidth;
		long int max_exit_bandwidth;
		Ports* myconn;
		Preferences* myprefs;
		Graph* mygraph;
};

#endif // PS_REDISTRIBUTED_H
