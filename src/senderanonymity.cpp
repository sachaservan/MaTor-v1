#include "senderanonymity.h"
#include "nodeweight.h"
#include "node.h"
#include <assert.h>


SenderAnonymity::SenderAnonymity(Graph* g, WeightedGraph* wSA)
{
    if(VERBOSE)cout << endl << "Initializing SenderAnonymity" << endl;
	for(unsigned int i=0; i < wSA->size(); i++)
	{
		deltas.push_back(0);
	}
	const Node* exit;
	const Node* entry;
	NodeWeight* exitW1;
	NodeWeight* entryW1;
	double val1;
	double dummy = 0;

    if(VERBOSE)cout<< "Initializing SenderAnonymity Calculating: " << std::flush;

	for(unsigned int exit_num=0; exit_num < wSA->size(); exit_num++)
	{
        if(exit_num % 1000 == 0)
        {
            if(VERBOSE)cout << exit_num << "... " << std::flush;
        }

		exitW1 = wSA->getNode(exit_num);
		exit = exitW1->getNode();
		double exitProb1 = wSA->getPS()->exitProb(exit,exitW1, wSA);
		for(unsigned int entry_num=0; entry_num < wSA->size(); entry_num++)
		{
			entryW1 = wSA->getNode(entry_num);
			entry = entryW1->getNode();
			if(!wSA->getPS()->entryExitAllowed(entry,exit,wSA->getPrefs()))
				continue;
			val1 = exitProb1 * wSA->getPS()->entryProb(entry,entryW1, exit,exitW1, wSA);
			this->deltas[entry_num]+= val1;
			dummy += val1;
		}
	}
    if(VERBOSE)cout << endl;
    cout << "Initializing SenderAnonymity Complete. Sum over all nodes = " << dummy << endl;
}
SenderAnonymity::~SenderAnonymity()
{

}
void SenderAnonymity::outputGuarantees(const unsigned int k, Graph* g) // k is the number of bad nodes
{
	if(VERBOSE)cout << "I am computing the delta for Sender Anonymity. I have " << deltas.size() << " many deltas" << endl;

	double delta = this->computeDelta(k,g);

	cout << "delta (for sender anonymiy and " << k << " conpromised nodes) = " << delta << endl;

	if(VERBOSE)cout << "Number of nodes: " << g->size() << endl;
	delta=0;
	for(unsigned int i=0; i < g->size(); i++)
	{
		delta += deltas[i];
	}
	
	cout << "Sum of all nodes: " << delta << endl;


}

double SenderAnonymity::computeDelta(const unsigned int k, Graph* g) // k is the number of bad nodes
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
	return delta;
}

void SenderAnonymity::print()
{
	cout << "SenderAnonymity" << endl;;
}

