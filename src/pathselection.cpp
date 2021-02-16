#include "pathselection.h"

void ComputeFamilyWeights(Graph* g, WeightedGraph* wg1, unsigned int min, unsigned int max, PathSelection* me)
{
    // if(VERBOSE)cout << "ComputeFamilyWeights() max: " << max << std::endl;
	for(unsigned int node_num=min; node_num < max; node_num++)
	{
        if(node_num % 100 == 0)
        {
            if(VERBOSE)cout << "." << std::flush;
        }
		NodeWeight* Alice = wg1->getNode(node_num);
		int aliceFam = Alice->related_middle_bandwidth;
		me->largestExitFamilyJoin1[node_num]=0;
		me->largestOtherExitFamily1[node_num]=0;
		me->largestEntryFamilyJoin1[node_num]=0;
		for(unsigned int j=0; j < g->size(); j++)
		{
			int exitFamilyJoin=aliceFam;
			int entryFamilyJoin=aliceFam;
			int otherExitFamily=0;
			NodeWeight* Bob = wg1->getNode(j);
			// We want to compute the middle_weight sum of the join of all Relatives of Alice and Bob (including themselves).
			// To do so, we start with the sum of all of Alice's relatives (which includes herself).

			Node* Charly;
			// We start with all of Bob's Family members. This does not include Bob.
			for(unsigned int k=0; Bob->getNode()->family!=NULL && k < Bob->getNode()->family->size(); k++)
			{
				Charly = (*Bob->getNode()->family)[k];
				//assert(this->related(j,Charly->myPos));
				// We count Charly in only if he is not related to Alice. Note that Alice is in her own subnet, so Charly cannot be Alice.
				if(!g->related(node_num,Charly->myPos))
				{
					if(Alice->entryWeight > 0)
					{
						exitFamilyJoin+=wg1->getNode(Charly->myPos)->middleWeight;
						otherExitFamily+=wg1->getNode(Charly->myPos)->entryWeight;
					}
					if(Alice->exitWeight > 0)
						entryFamilyJoin+=wg1->getNode(Charly->myPos)->middleWeight;
				}
			}
			// Furthermore we check all Nodes that are in Bob's subnet. Note that this does include Bob himself.
			for(unsigned int k=0; k < Bob->getNode()->IPfamily->size(); k++)
			{
				Charly = (*Bob->getNode()->IPfamily)[k];
				assert(g->related(j,Charly->myPos));
			
				// We count only those nodes that are not related to Alice and that are not in the Family of Bob (we counted them in already)!
				// We only exclude Bob here if he is already related to Alice (and thus was in the sum from Alice).
				if(!(g->related(node_num,Charly->myPos)) && !Bob->getNode()->isFamily(Charly))
				{
					if(Alice->entryWeight > 0)
					{
						exitFamilyJoin+=wg1->getNode(Charly->myPos)->middleWeight;
						otherExitFamily+=wg1->getNode(Charly->myPos)->entryWeight;
					}
					if(Alice->exitWeight > 0)
						entryFamilyJoin+=wg1->getNode(Charly->myPos)->middleWeight;
				}
			}
			if(exitFamilyJoin > me->largestExitFamilyJoin1[node_num])
				me->largestExitFamilyJoin1[node_num] = exitFamilyJoin;
			if(otherExitFamily > me->largestOtherExitFamily1[node_num])
				me->largestOtherExitFamily1[node_num] = otherExitFamily;
			if(entryFamilyJoin > me->largestEntryFamilyJoin1[node_num])
				me->largestEntryFamilyJoin1[node_num] = entryFamilyJoin;
		}
	}
}

void PathSelection::computeFamilyWeights(WeightedGraph* wg)
{
	if(fWComputed) return;
	Graph* g = wg->getGraph();
	size_t gSize = g->size();
	
	this->largestExitFamilyJoin1 = new double[gSize];
	this->largestOtherExitFamily1 = new double[gSize];
	this->largestEntryFamilyJoin1 = new double[gSize];
	
	if(VERBOSE)cout << "Vectors initialized" << std::endl;
	vector<boost::thread*> mythreads;
	unsigned int min;
	unsigned int max;
	std::cout << g->size()/SET_SIZE << " # of threads" << std::endl;
	for(unsigned int i=0; i < (g->size()/SET_SIZE)+1; i++)
	{
		min = i*SET_SIZE;
		max = (i+1)*SET_SIZE;
		if(max > g->size())
			max = g->size();
		mythreads.push_back(new boost::thread(ComputeFamilyWeights, g, wg, min, max, this));
		if(mythreads.size()>=USED_THREADS) // Defnined functions.h
		{
			for(unsigned int j=0; j < mythreads.size(); j++)
				mythreads[j]->join();
			mythreads.clear();
		}
		// No deleting threads from vector after joining?
	}
	for(unsigned int j=0; j < mythreads.size(); j++)
		mythreads[j]->join();
	//mythreads.clear();
    if(VERBOSE)cout << std::endl << "Threads Finished!" << std::endl;
    
    fWComputed = true;
}

double PathSelection::miPMaxEx(int entry_num, int middle_num, WeightedGraph* wg)
{
	return wg->getNode(middle_num)->middleWeight / (wg->getSumMiddle() - largestExitFamilyJoin1[entry_num]);
}
double PathSelection::miPMinEx(int entry_num, int middle_num, WeightedGraph* wg)
{
	return wg->getNode(middle_num)->middleWeight / (wg->getSumMiddle() - wg->getNode(entry_num)->related_middle_bandwidth);
}
double PathSelection::enPMaxEx(int entry_num, WeightedGraph* wg)
{
	return wg->getNode(entry_num)->entryWeight / (wg->getSumEntry() - largestOtherExitFamily1[entry_num]);
}
double PathSelection::miPMaxEn(int middle_num, int exit_num, WeightedGraph* wg)
{
	return wg->getNode(middle_num)->middleWeight / (wg->getSumMiddle() - largestEntryFamilyJoin1[exit_num]);
}
double PathSelection::miPMinEn(int middle_num, int exit_num, WeightedGraph* wg)
{
	return wg->getNode(middle_num)->middleWeight / (wg->getSumMiddle() - wg->getNode(exit_num)->related_middle_bandwidth);
}
