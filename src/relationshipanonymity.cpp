#include "recipientanonymity.h"
#include "relationshipanonymity.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <vector>
#include <deque>
#include <set>

// double minW = 0; // can be raised to filter out some negligible values // moved to helper.hpp


RelationshipAnonymity::RelationshipAnonymity(Graph* g, WeightedGraph* wg1, WeightedGraph* wg2, double eps) : wg1(wg1), wg2(wg2), epsilon(eps), weights(g->size())
{
	int x = 0;
	size_t size = wg1->size();
	const Node *exit, *entry;
	const NodeWeight *exitW, *entryW;
	double enP, exP;
	
	
	std::deque<std::pair<int, int> > edges;
		
	for(int j = 0; j < size; ++j)
		if(exitW = wg1->getNode(j), exit = exitW->getNode(), (exP = wg1->getPS()->exitProb(exit, exitW, wg1)) > minW)
			for(int i = 0; i < size; ++i)
				if(entryW = wg1->getNode(i), entry = entryW->getNode(), (enP = wg1->getPS()->entryProb(entry, entryW, exit, exitW, wg1)) > minW)
					if(wg1->getPS()->entryExitAllowed(entry, exit, wg1->getPrefs())) // just checking here
					{
						//EnPlus.insert(i);
						//ExPlus.insert(j);
						weights[i][j] += enP * exP;
						++x;
					}
	std::cout << x << " possible connections overall" << std::endl;
	
	std::vector<double> usedAsEntry1(g->size());
	std::vector<double> deltasMiddle(g->size());
	const NodeWeight *exitW1, *entryW1, *middleW1;
	const NodeWeight *exitW2, *entryW2, *middleW2;
	double exitProb1, exitProb2;
	PathSelection *ps = wg1->getPS();
	double W1, W2;
	
	if(VERBOSE)cout << "Computing: " << std::flush;
	for(unsigned int exit_num=0; exit_num < g->size(); exit_num++) // For all exit nodes
	{
        if(exit_num % 1000 == 0)
        {
            if(VERBOSE)cout << exit_num << "... " << std::flush;
        }
		exit = g->getNode(exit_num);
		exitW1 = wg1->getNode(exit_num);
		if(wg1->getNode(exit_num)->exitWeight == 0)
			continue;
		exitW2 = wg2->getNode(exit_num);

		exitProb1 = wg1->getPS()->exitProb(exit, exitW1, wg1);
		exitProb2 = wg1->getPS()->exitProb(exit, exitW2, wg2);
			
		for(unsigned int middle_num=0; middle_num < g->size(); middle_num++) // For all middle nodes
		{
			middleW1 = wg1->getNode(middle_num);
			middleW2 = wg2->getNode(middle_num);

			W1 = ps->miPMaxEn(middle_num, exit_num, wg1);
			W2 = ps->miPMinEn(middle_num, exit_num, wg2);

			if(exitProb2*W2 == 0 || (exitProb1*W1) / (exitProb2*W2) > this->epsilon)
			{
				if(W1 > 1)
					W1 = 1;
				deltasMiddle[middle_num] += exitProb1*(W1-W2);
			}
		}
		
		for(unsigned int entry_num=0; entry_num < g->size(); entry_num++) // For all entry nodes
		{
			entry = g->getNode(entry_num);
			entryW1 = wg1->getNode(entry_num);
			if(entryW1->entryWeight > 0 && wg1->getPS()->entryExitAllowed(entry,exit,wg1->getPrefs()))
				usedAsEntry1[entry_num] +=exitProb1 * wg1->getPS()->entryProb(entry, entryW1, exit, exitW1, wg1); 
		}
	}
    if(VERBOSE)cout << std::endl;
    
    if(VERBOSE)cout << "Computing probability that entry nodes see certain middle nodes ... " << endl;
	
    if(VERBOSE)cout << "Computing: " << std::flush;
	for(unsigned int entry_num=0; entry_num < g->size(); entry_num++) // For all entry nodes
	{
        if(entry_num % 1000 == 0)
        {
            if(VERBOSE)cout << entry_num << "... " << std::flush;
        }

		double tmp = 0;
		entry = g->getNode(entry_num);
		entryW1 = wg1->getNode(entry_num);
		entryW2 = wg2->getNode(entry_num);
		if(entryW1->entryWeight == 0 || usedAsEntry1[entry_num] == 0)
			continue;
			
		enP = ps->enPMaxEx(entry_num, wg1);
		
		for(unsigned int middle_num=0; middle_num < g->size(); middle_num++) // For all middle nodes
		{
			if(entry_num == middle_num) continue; // this shouldn't/cannot happen
			
			W1 = ps->miPMaxEx(entry_num, middle_num, wg1);
			W2 = ps->miPMinEx(entry_num, middle_num, wg2);

			if(W2 == 0 || W1 / W2 > this->epsilon)
			{
				if(W1 > 1)
					W1 = 1;
				double W = (W1-W2) * enP;
				tmp += W;
			}
			this->addEnMiEdge(entry_num, middle_num, enP * deltasMiddle[middle_num]);
		}
		if(tmp > usedAsEntry1[entry_num])
			this->addNewEntryWeight(entry_num, usedAsEntry1[entry_num]);
		else
			this->addNewEntryWeight(entry_num, tmp);
	}
	
	// TODO: Try adding the max-edge amplification here
	
	for(edgemap<double>::iterator it = weights.begin(); it != weights.end(); ++it)
		EdgeWeights.insert(*it);
	std::cout << "Copied/sorted weights..." << std::endl;

	if(VERBOSE) std::cout << "accumulation of EdgeWeights: " << std::accumulate(EdgeWeights.begin(), EdgeWeights.end(), 0.0) << std::endl;
}

RelationshipAnonymity::~RelationshipAnonymity()
{
}

void RelationshipAnonymity::outputGuarantees(const unsigned int k, Graph* g) { } // k is the number of bad nodes

double RelationshipAnonymity::computeDelta(const unsigned int K, Graph* g) // k is the number of bad nodes
{
	if(VERBOSE) std::cout << "Computing upper bound for Relationship Anonimity" << std::endl;
		
	int X;
	
	X = K*(K-1) / 2;
	
	if(VERBOSE) std::cout << "... compromising at most " << X << " edges" << std::endl;
			
	if (X > EdgeWeights.size())
		X = EdgeWeights.size();

	std::multiset<double>::reverse_iterator last = EdgeWeights.rbegin();
	for (int i=0; i<X; ++i) ++last;
	
	double Result = std::accumulate(EdgeWeights.rbegin(), last, 0.0);
	
	if(VERBOSE) std::cout << "... adding delta from " << K << " most likely nodes" << std::endl;
	
	last = entryWeights.rbegin();
	for (int i=0; i<K && last != entryWeights.rend(); ++i) ++last;
	Result = std::accumulate(entryWeights.rbegin(), last, Result);
	
	return Result;
} 


void RelationshipAnonymity::print()
{ 
	cout << "RelationshipAnonymity" << endl;
}

void RelationshipAnonymity::addEnMiEdge(int entry, int middle, double weight)
{
	weights[entry][middle] += weight;
}

void RelationshipAnonymity::addNewEntryWeight(int entry, double weight)
{
	if(weight == 0) return;
	entryWeights.insert(weight);
	entryWeightsM[entry] = weight;
}
