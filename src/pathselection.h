#ifndef PATHSELECTION_H
#define PATHSELECTION_H

#include <assert.h>
#include "graph.h"
#include "preferences.h"
#include "node.h"
#include "nodeweight.h"
#include "weightedgraph.h"
#include "ports.h"


/*
 * This is a virtual class that describes path selection strategies.
 */
class WeightedGraph;
class PathSelection
{
	public:
		// class constructor
		PathSelection() : fWComputed(false) {}
		PathSelection(Ports* ports, Graph* g, Preferences* prefs) : fWComputed(false) {}
		// class destructor
		virtual ~PathSelection() {};
		virtual double getEntryWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const { 
			assert(false);
			return false;
		}
		virtual double getMiddleWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const { 
			assert(false);
			return false;
		}
		virtual double getExitWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const { 
			assert(false);
			return false;
		}
		virtual bool entryExitAllowed(const Node* entry, const Node* exit, Preferences* prefs) const{
			assert(false);
			return false;
		}

		virtual double exitProb(const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const{
			assert(false); 
			return false;
		}
		virtual double entryProb(const Node* entry,const NodeWeight* entryW, const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const{
			assert(false);
			return false;
		}
		virtual double middleProb(const unsigned int& entry_num, const unsigned int& middle_num, const unsigned int& exit_num, WeightedGraph* wg) const{
			assert(false);
			return false;
		}

		virtual void print(){assert(false);}
		
		void computeFamilyWeights(WeightedGraph* wg);
		double* largestExitFamilyJoin1;
		double* largestOtherExitFamily1;
		double* largestEntryFamilyJoin1;
		
		double miPMaxEx(int entry_num, int middle_num, WeightedGraph* wg);
		double miPMinEx(int entry_num, int middle_num, WeightedGraph* wg);
		double enPMaxEx(int entry_num, WeightedGraph* wg);
		double miPMaxEn(int middle_num, int exit_num, WeightedGraph* wg);
		double miPMinEn(int middle_num, int exit_num, WeightedGraph* wg);
	private:
		bool fWComputed;
};


#endif // PATHSELECTION_H
