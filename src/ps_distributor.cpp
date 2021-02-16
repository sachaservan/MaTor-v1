#include "ps_distributor.h" // class's header file
#include <iostream>
#include "weightedgraph.h"
#include "ps_tor.h"

// class constructor

class compBW
{
    public:
	compBW(WeightedGraph* x){this->wg=x;}
	WeightedGraph* wg;
    bool operator() (unsigned int i, unsigned int j) const //Funktionsobjekt
    {
		double bwI =0;
		if((this->wg->getNode(i)->exitWeight)>0)
			bwI = this->wg->getNode(i)->getNode()->getBandwidth();
		double bwJ =0;
		if((this->wg->getNode(j)->exitWeight)>0)
			bwJ = this->wg->getNode(j)->getNode()->getBandwidth();
		return (bwI < bwJ);
    }
};

class compBWEntry
{
    public:
	compBWEntry(WeightedGraph* x, unsigned int max_exit){this->wg=x; this->max_exit_bandwidth = max_exit;}
	WeightedGraph* wg;
	unsigned int max_exit_bandwidth;
    bool operator() (unsigned int i, unsigned int j) const //Funktionsobjekt
    {
		double bwI =0;
		if(this->wg->getNode(i)->entryWeight>0)
			bwI = this->wg->getNode(i)->getNode()->getBandwidth();
		if(wg->getNode(i)->exitWeight>0)
			bwI-=this->max_exit_bandwidth;
		if(bwI < 0)
			bwI=0;
		double bwJ =0;
		if(this->wg->getNode(j)->entryWeight>0)
			bwJ = this->wg->getNode(j)->getNode()->getBandwidth();
		if(wg->getNode(j)->exitWeight>0)
			bwJ-=this->max_exit_bandwidth;
		if(bwJ < 0)
			bwJ=0;
		return (bwI < bwJ);
    }
};


PSDistributor::PSDistributor(Ports* ports, Graph* g, Preferences* prefs, int bwpercentage) : PathSelection(ports, g, prefs)
{

	// Compute maxbandwidth
	// Initialize the normal path selection algorithm
	Preferences* myPrefsWithoutGuards = new Preferences(prefs);
	myPrefsWithoutGuards->setUseGuards(false);
	PSTor* psTor = new PSTor(ports,g,myPrefsWithoutGuards);
	// Compute the weights that the normal path selection would give to the nodes
	WeightedGraph* wTmp = new WeightedGraph(ports, g, psTor, myPrefsWithoutGuards, true);

	// Sort all nodes by their exit bandwidth
	compBW c(wTmp);
	vector<unsigned int> allnodes; 
	for(unsigned int i=0; i < g->size(); i++)
	{
		allnodes.push_back(i);
	}
	std::sort(allnodes.begin(),allnodes.end(),c);

	double tmpBW = 0;
	double maxbandwidth = 0;

	bool foundLimit = false;
	double goalExit = (wTmp->getSumExit()*bwpercentage)/100;
	if(VERBOSE)cout << "My goal for the exit bandwidth is " << goalExit << endl;
	for(unsigned int i=0; i < g->size() && !foundLimit; i++)
	{
		unsigned int n = g->size() - i;
		double bw = 0;
		if(wTmp->getNode(allnodes[i])->exitWeight >0)
			bw = wTmp->getNode(allnodes[i])->getNode()->getBandwidth();
		if(tmpBW + n*bw < goalExit)
		{
			tmpBW+=bw;
			//cout << tmpBW << "  ";
		}else{
			maxbandwidth = (goalExit-tmpBW) / n;
			if(maxbandwidth < wTmp->getNode(allnodes[g->size()-1])->exitWeight)
				foundLimit=true;
		}
	}


	this->max_exit_bandwidth = (int)maxbandwidth;

	compBWEntry cEn(wTmp, this->max_exit_bandwidth);

	// Searching an optimal entry bandwidth
	// Sort the nodes by their (remaining) entry bandwidth:
	std::sort(allnodes.begin(),allnodes.end(),cEn);
	tmpBW = 0;
	maxbandwidth = 0;
	foundLimit=false;
	
	double goalEntry = (wTmp->getSumEntry()*bwpercentage)/100;
	if(VERBOSE)cout << "My goal for the entry bandwidth is " << goalEntry << endl;
	for(unsigned int i=0; i < g->size() && !foundLimit; i++)
	{
		unsigned int n = g->size() - i;
		double bw = 0;
		if(wTmp->getNode(allnodes[i])->entryWeight > 0)
			bw = wTmp->getNode(allnodes[i])->getNode()->getBandwidth();
		if(wTmp->getNode(allnodes[i])->exitWeight>0)
			bw-=this->max_exit_bandwidth;
		if(bw < 0)
			bw = 0;
		if(tmpBW + n*bw < goalEntry)
		{
			//maxbandwidth = wTmp->getNode(allnodes[i])->exitWeight;
			tmpBW+=bw;
		}else{
			maxbandwidth = (goalEntry-tmpBW) / n;
			int maxBW = wTmp->getNode(allnodes[g->size()-1])->getNode()->getBandwidth();

			if(wTmp->getNode(allnodes[g->size()-1])->exitWeight > 0)
				maxBW-=this->max_exit_bandwidth;
			if(maxbandwidth < maxBW)
				foundLimit=true;
		}
	}
	
	this->max_entry_bandwidth = (int)maxbandwidth;;

	this->myconn = ports;
	this->myprefs = prefs;
	this->mygraph = g;

	this->entrySum=0;
	this->middleSum=0;
	this->exitSum=0;
	const Node* target;
	
	for(unsigned int i=0; i < g->size(); i++)
	{
		target = g->getNode(i);
		this->entrySum+=getEntryWeight(target,ports,g,prefs);
		this->middleSum+=getMiddleWeight(target,ports,g,prefs);
		this->exitSum+=getExitWeight(target,ports,g,prefs);
	}
	if(prefs->useGuards())
		assert(this->entrySum>0 && "Maybe you specified to use guards but no possible guards were found");
	else if(this->entrySum < 0)
	{
		cout << "DistribuTor failure, no entry nodes available." << endl;
		
		// try to invoke ps-distributor-inv
	}
		
	assert(this->middleSum>0);
	assert(this->exitSum>0);

	if(this->entrySum < goalEntry * 0.99 && !prefs->useGuards())
	{
		cout << "Could not preserve entry bandwidth! Can guarantee " << this->entrySum << " but need " << goalEntry << "which is " << 100*this->entrySum / goalEntry  << "%" << endl;
	}

}



// class destructor
PSDistributor::~PSDistributor()
{
	// insert your code here
}


double PSDistributor::exitProb(const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const
{
	return (exitW->exitWeight / (this->exitSum));
}


double PSDistributor::entryProb(const Node* entry,const NodeWeight* entryW, const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const
{
	if(entryW->entryWeight>0)
		return (entryW->entryWeight / (this->entrySum - (exitW->related_entry_bandwidth)));
	else
		return 0;
}
double PSDistributor::middleProb(const unsigned int& entry_num, const unsigned int& middle_num, const unsigned int& exit_num, WeightedGraph* wg) const
{
	return 0; // Not used by our analysis
}


double PSDistributor::getEntryWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	if(target->possibleEntry(ports,prefs))
	{
		double bw =target->getBandwidth();
		if(target->possibleExit(ports,prefs))
			bw-=this->max_exit_bandwidth;

		if(bw < 0)
			bw=0;

		if(bw < this->max_entry_bandwidth)
			return bw;
		else
			return this->max_entry_bandwidth;
	}
	else
		return 0;
}
double PSDistributor::getMiddleWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	double bw =target->getBandwidth();
	if(target->possibleExit(ports,prefs))
		bw-=this->max_exit_bandwidth;
	if(target->possibleEntry(ports,prefs))
		bw-=this->max_entry_bandwidth;

	if(bw < 0)
		bw = 0;

	if (target->possibleMiddle(ports,prefs))
		return bw;
	else
		return 0;
}
double PSDistributor::getExitWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	if (!target->possibleExit(ports,prefs))
		return 0;

	// Count them with at most max_exit_bandwidth
	double bw = (target->getBandwidth() > this->max_exit_bandwidth)? this->max_exit_bandwidth : target->getBandwidth();
	return bw;
}

bool PSDistributor::entryExitAllowed(const Node* entry, const Node* exit, Preferences* prefs) const
{
	return (!entry->related(exit));
}
void PSDistributor::print()
{
	
	cout << "Distributor with max_exit_bandwidth=" << this->max_exit_bandwidth << " and max_entry_bandwidth=" << this->max_entry_bandwidth;
}
