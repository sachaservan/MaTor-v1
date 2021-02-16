#include "recipientanonymity.h"
#include <iostream>
#include <assert.h>
#include "range.hpp"


RecipientAnonymity::RecipientAnonymity(Graph* g, WeightedGraph* wg1, WeightedGraph* wg2, double eps)
{
	const unsigned int EVIL_NODES = 5;

	this->active_instances=0;
	this->gSize = g->size();
	this->epsilon=eps;
	this->deltaFromSeeingEntryNodes = 0;
	
	//this->instancesMiddle = new double[USED_THREADS * this->gSize];
	//this->instancesThisEntrySeesMiddle1 = new double[USED_THREADS * this->gSize];
	//this->instancesThisEntrySeesMiddle2 = new double[USED_THREADS * this->gSize];
	//this->instancesEntryNum = new unsigned int[USED_THREADS];
    
    if(VERBOSE)cout << endl << "Precomputing Family Weights for approximating the impact of nodes..." << endl;

	this->usedAsEntry1 = new double[this->gSize];
	this->usedAsEntry2 = new double[this->gSize];
    if(VERBOSE)cout << "Nodes: " << g->size() << std::endl;

    for(unsigned int i=0; i < g->size(); i++)
	{
		this->deltas.push_back(0);
		this->deltasEntry.push_back(0);
		this->deltasMiddle.push_back(0);
		this->deltasExit.push_back(0);
		this->usedAsEntry1[i]=0;
		this->usedAsEntry2[i]=0;
	}
    
    PathSelection* ps = wg1->getPS(); // We'll use this one for family weights, right?
	ps->computeFamilyWeights(wg1);
	//wg1->getPS()->computeFamilyWeights(wg1);
    //wg2->getPS()->computeFamilyWeights();

    if(VERBOSE)cout << endl << "Computing Recipient Anonymity for epsilon = " << this->epsilon << " ..." << endl;

	if(VERBOSE)cout << "Computing probability that an exit node is compromized and that entry nodes are used ..." << endl;
	double exitProb1=0;
	double exitProb2=0;
	Node* exit;
	Node* entry;
	NodeWeight* exitW1;
	NodeWeight* middleW1;
	NodeWeight* entryW1;
	NodeWeight* exitW2;
	NodeWeight* middleW2;
	NodeWeight* entryW2;
    if(VERBOSE)cout << "Computing: " << std::flush;
	for(unsigned int exit_num=0; exit_num < g->size(); exit_num++) // For all exit nodes
	{
        if(exit_num % 1000 == 0)
        {
            if(VERBOSE)cout << exit_num << "... " << std::flush;
        }
		exit = g->getNode(exit_num);
		exitW1 = wg1->getNode(exit_num);
		exitW2 = wg1->getNode(exit_num);
		if(wg1->getNode(exit_num)->exitWeight == 0)
			continue;
		exitProb1 = wg1->getPS()->exitProb(exit, exitW1, wg1);
		exitProb2 = wg2->getPS()->exitProb(exit, exitW2, wg2);
		this->deltasExit[exit_num]+=exitProb1; // The exit nodes can contribute to delta
		for(unsigned int entry_num=0; entry_num < g->size(); entry_num++) // For all entry nodes
		{
			entry = g->getNode(entry_num);
			entryW1 = wg1->getNode(entry_num);
			entryW2 = wg2->getNode(entry_num);
			if(entryW1->entryWeight > 0 && wg1->getPS()->entryExitAllowed(entry,exit,wg1->getPrefs()))
				this->usedAsEntry1[entry_num]+=exitProb1 * wg1->getPS()->entryProb(entry, entryW1, exit, exitW1, wg1); 
			if(entryW2->entryWeight > 0 && wg2->getPS()->entryExitAllowed(entry,exit,wg2->getPrefs()))
				this->usedAsEntry2[entry_num]+=exitProb2 * wg2->getPS()->entryProb(entry, entryW2, exit, exitW2, wg2); 
		}
	}
    if(VERBOSE)cout << std::endl;

	for(unsigned int i=0; i < g->size(); i++)
	{
		this->deltas[i] = this->deltasExit[i];
	}
    if(VERBOSE)cout << endl << "Delta from exits only (for " << EVIL_NODES << " compromised nodes):" << this->computeDelta(EVIL_NODES,g) << endl;
	if(VERBOSE)cout << "Computing probability that entry nodes see certain middle nodes ... " << endl;
	double W1;
	double W2;

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
		if(entryW1->entryWeight == 0)
			continue;
		for(unsigned int middle_num=0; middle_num < g->size(); middle_num++) // For all middle nodes
		{
			//middleW1 = wg1->getNode(middle_num);
			//middleW2 = wg2->getNode(middle_num);

			W1 = ps->miPMaxEx(entry_num, middle_num, wg1);
			W2 = ps->miPMinEx(entry_num, middle_num, wg2);

			if(W2 == 0 || W1 / W2 > this->epsilon)
			{
				if(W1 > 1)
					W1 = 1;
				double W = (W1-W2) * ps->enPMaxEx(entry_num, wg1);
				tmp += W;
			}
		}
		if(this->usedAsEntry2[entry_num] == 0 || (this->usedAsEntry1[entry_num] / this->usedAsEntry2[entry_num]) > this->epsilon)
		{
			double W = (this->usedAsEntry1[entry_num] - this->usedAsEntry2[entry_num]);
			this->deltaFromSeeingEntryNodes+= W;
			if(VERBOSE) cout << "Adding W : " << W << " ( " << (this->usedAsEntry1[entry_num] / this->usedAsEntry2[entry_num]) << " ) to " << this->deltaFromSeeingEntryNodes << endl;
			if(VERBOSE) cout << " " << this->deltaFromSeeingEntryNodes << endl;
			// this->deltasEntry[entry_num]+= W; // Node is distinguishing anyway!
		}
		if(tmp > this->usedAsEntry1[entry_num])
			this->deltasEntry[entry_num]+=this->usedAsEntry1[entry_num];
		else
			this->deltasEntry[entry_num]+=tmp;
	}
	
    if(VERBOSE)cout << std::endl;
	for(unsigned int i=0; i < g->size(); i++)
	{
		this->deltas[i] = this->deltasEntry[i];
	}
    if(VERBOSE)cout << endl << "Delta from seeing entry nodes:" << this->deltaFromSeeingEntryNodes << endl;
	if(VERBOSE)cout << "Delta from entry nodes (for " << EVIL_NODES << " compromised nodes):" << this->computeDelta(EVIL_NODES,g) << endl;
	if(VERBOSE)cout << "Computing probability that middle nodes see certain exit nodes ... " << endl;

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
				this->deltasMiddle[middle_num] += exitProb1*(W1-W2);
			}
		}
	}

	for(unsigned int i=0; i < g->size(); i++)
	{
		this->deltas[i] = this->deltasMiddle[i];
	}
    if(VERBOSE)cout << endl << "Delta from middle nodes(for " << EVIL_NODES << " compromised nodes):" << this->computeDelta(EVIL_NODES,g) << endl;


    double delta = 0;
	for(unsigned int i=0; i < g->size(); i++)
	{
		this->deltas[i] = this->deltasExit[i] + this->deltasEntry[i] * (1-this->deltasExit[i]) + this->deltasMiddle[i] * (1-this->deltasExit[i]) * (1-this->deltasEntry[i]);
        delta += this->deltas[i];
	}

    if(VERBOSE)cout << endl << "Sum of all deltas (since we take an approximation, this will probably be significantly larger than 1): " << delta << endl;
	
    if(VERBOSE)cout << "RecipientAnonymity() DONE" << std::endl;
}
RecipientAnonymity::~RecipientAnonymity()
{

}

void RecipientAnonymity::outputGuarantees(const unsigned int k, Graph* g) // k is the number of bad nodes
{
	if(VERBOSE)cout << "I am computing the delta for Recipient Anonymity. I have " << deltas.size() << " many deltas" << endl;

	double delta = this->computeDelta(k,g);

	cout << "delta (for recipient anonymity with " << k << "compromised nodes) = " << delta << endl;

	if(VERBOSE)cout << "Number of nodes: " << g->size() << endl;
	delta=0;
	for(unsigned int i=0; i < g->size(); i++)
	{
		delta += deltas[i];
	}
	
	cout << "Sum of all nodes: " << delta << endl;

}

double RecipientAnonymity::computeDelta(const unsigned int k, Graph* g) // k is the number of bad nodes
{
	assert(k <= g->size());
	vector<unsigned int> worst_nodes;
	for(unsigned int j=0; j < k; j++)
	{
		worst_nodes.push_back(j);
	}
	for(unsigned int i=k; i < g->size(); i++)
	{
		unsigned int t = i;
		// Check whether this node is better at least one of the previously found ones.
		for(unsigned int j=0; j < k; j++)
		{
			if(this->deltas[t] > this->deltas[worst_nodes[j]])
			{
				unsigned int tmp = worst_nodes[j];
				worst_nodes[j] = t;
				t = tmp;
			}
		}
	}
	double delta = 0;
	for(unsigned int i=0; i < k; i++)
	{
		delta += this->deltas[worst_nodes[i]];
	}
	if(VERBOSE)cout << "Delta was " << delta << " and I add the delta from seeing entry nodes of " << this->deltaFromSeeingEntryNodes << endl;
	delta += this->deltaFromSeeingEntryNodes;
	if (delta>1)
		return 1;
	else
		return delta;

}

void RecipientAnonymity::print()
{
	cout << "RecipientAnonymity" << endl;
}

