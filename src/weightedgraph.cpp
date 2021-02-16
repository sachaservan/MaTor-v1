#include <iostream>
#include <algorithm>
#include "weightedgraph.h" // class's header file
#include "anoa.h"
#include "senderanonymity.h"
// class constructor
WeightedGraph::WeightedGraph(Ports* ports, Graph* g, PathSelection* p, Preferences* prefs, bool simple)
{
    if(VERBOSE)cout << std::endl;
	// Also compute the sums of the bandwidths
	this->sumEntry=0;
	this->sumMiddle=0;
	this->sumExit=0;
	this->mygraph = g;
	this->myprefs = prefs;
	this->myps = p;
	this->numberOfExits	=0;

	for(unsigned int i=0; i < g->size(); i++)
	{
		this->nodes.push_back(NodeWeight(g->getNode(i),i));
		this->nodes[i].entryWeight = p->getEntryWeight (g->getNode(i),ports,g,prefs);
		this->sumEntry += this->nodes[i].entryWeight;
		this->nodes[i].middleWeight = p->getMiddleWeight(g->getNode(i),ports,g,prefs);
		this->sumMiddle+= this->nodes[i].middleWeight;
		this->nodes[i].exitWeight = p->getExitWeight  (g->getNode(i),ports,g,prefs);
		this->sumExit  += this->nodes[i].exitWeight;
		if(this->nodes[i].exitWeight > 0)
		{
			this->numberOfExits++;
		}
	}
	this->exitIndexes = new unsigned int [this->numberOfExits];
	this->reversedExitIndexes = new unsigned int [g->size()];
	if(VERBOSE)cout << "Sums:: Entry: " << this->sumEntry << " Middle: " << this->sumMiddle << " Exit: " << this->sumExit << endl;
    if(VERBOSE)cout << "Computing related weights for each node: " << nodes.size() <<  endl;
	// DEBUG
    if(VERBOSE)cout << "Checking:  " << std::flush;
	for(unsigned int i=0; i < this->nodes.size(); i++)
	{
        if(i % 1000 == 0)
        {
            if(VERBOSE)cout << i << "... " << std::flush;
        }
		for(unsigned int j=0; j < this->nodes.size(); j++)
		{
			assert(this->related(i,j) == this->related(j,i));
		}
	}
    if(VERBOSE)cout << std::endl;
    if(VERBOSE)cout << "Computing: " << std::flush;
	unsigned int exit_index_num=0;
	for(unsigned int i=0; i < this->nodes.size(); i++)
	{
        if(i % 1000 == 0)
        {
            if(VERBOSE)cout << i << "... "<< std::flush;
        }

		if(this->nodes[i].exitWeight>0)
		{
			this->exitIndexes[exit_index_num] = i;
			this->reversedExitIndexes[i]=exit_index_num;
			exit_index_num++;
		}
		this->nodes[i].related_entry_bandwidth=0;
		this->nodes[i].related_middle_bandwidth=0;

		
		assert(this->mygraph->getNode(i) == this->nodes[i].getNode());
		assert(this->mygraph->myRelatedArray[i][i]);
		assert(this->mygraph->related(i,i));
		assert(this->related(i,i));

		// We compute the weight of all nodes that are related to node i.
		for(unsigned int j=0; j < this->nodes.size(); j++)
		{
			if(this->related(i,j))
			{
				assert(this->related(j,i));
				this->nodes[i].related_entry_bandwidth+=this->nodes[j].entryWeight;
				this->nodes[i].related_middle_bandwidth+=this->nodes[j].middleWeight;
			}
		}
	}
    if(VERBOSE)cout << std::endl;
    if(VERBOSE)cout << "done." << endl;
	cout.flush();
}

// class destructor
WeightedGraph::~WeightedGraph()
{

}

double WeightedGraph::getSumEntry()
{
	return this->sumEntry;
}
double WeightedGraph::getSumMiddle()
{
	return this->sumMiddle;
}
double WeightedGraph::getSumExit()
{
	return this->sumExit;
}
NodeWeight* WeightedGraph::getNode(const unsigned int i)
{
	return &(this->nodes[i]);
}

unsigned int WeightedGraph::size()const
{
	return this->nodes.size();
}


