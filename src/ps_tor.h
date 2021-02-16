
#ifndef PSTor_H
#define PSTor_H

#include "pathselection.h" // inheriting class's header file
#include "preferences.h"
#include "node.h"
#include "weightedgraph.h"
#include "ports.h"

/*
 * No description
 */
class PSTor : public PathSelection
{
	public:
		// class constructor
		PSTor();
		PSTor(Ports* ports, Graph* g, Preferences* prefs);
		// class destructor
		~PSTor();
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
		Ports* myconn;
		Preferences* myprefs;
		Graph* mygraph;

};

#endif // PSTor_H
