#include "ps_tor.h" // class's header file
#include <iostream>


// class constructor
PSTor::PSTor(Ports* ports, Graph* g, Preferences* prefs) : PathSelection(ports, g, prefs)
{
	this->entrySum=0;
	this->middleSum=0;
	this->exitSum=0;

	this->myconn = ports;
	this->myprefs = prefs;
	this->mygraph = g;

	const Node* target;
	
	for(unsigned int i=0; i < g->size(); i++)
	{
		target = g->getNode(i);
		this->entrySum+=this->getEntryWeight(target,ports,g,prefs);
		this->middleSum+=this->getMiddleWeight(target,ports,g,prefs);
		this->exitSum+=this->getExitWeight(target,ports,g,prefs);
	}
	assert(this->entrySum>0 && "Maybe you specified to use guards but no possible guards were found");
	assert(this->middleSum>0);
	assert(this->exitSum>0);
}

// class destructor
PSTor::~PSTor()
{
	// insert your code here
}

double PSTor::exitProb(const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const
{
	return (exitW->exitWeight / (this->exitSum));
}

double PSTor::entryProb(const Node* entry,const NodeWeight* entryW, const Node* exit,const NodeWeight* exitW, WeightedGraph* wg) const
{
	assert(this->entrySum - (exitW->related_entry_bandwidth) >= 1 || entryW->entryWeight == 0);
	if(entryW->entryWeight > 0)
		return (entryW->entryWeight / (this->entrySum - (exitW->related_entry_bandwidth)));
	else 
		return 0;
}
double PSTor::middleProb(const unsigned int& entry_num, const unsigned int& middle_num, const unsigned int& exit_num, WeightedGraph* wg) const
{
	return 0;
}


double PSTor::getEntryWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	double myBW = (double)target->getBandwidth();
	double myWeight;
	if(target->isGuard() && target->isExit())
	{
		myWeight = ((*g->getWeights())["Wgd"]);
	}else if(target->isGuard()){
		myWeight = ((*g->getWeights())["Wgg"]);		
	}else if(target->isExit()){
		myWeight = 0;		
	}else{ // middle
		myWeight = ((*g->getWeights())["Wgm"]);		
	}
	myWeight /= 10000;
	assert(myWeight <= 1);
	if (target->possibleEntry(ports,prefs))
		return myBW * myWeight;
	else
		return 0;
}
double PSTor::getMiddleWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	double myBW = (double)target->getBandwidth();
	double myWeight;
	if(target->isGuard() && target->isExit())
	{
		myWeight = ((*g->getWeights())["Wmd"]);
	}else if(target->isGuard()){
		myWeight = ((*g->getWeights())["Wmg"]);		
	}else if(target->isExit()){
		myWeight = ((*g->getWeights())["Wme"]);
	}else{ // middle
		myWeight = ((*g->getWeights())["Wmm"]);		
	}
	myWeight /= 10000;
	assert(myWeight <= 1);
	if (target->possibleMiddle(ports,prefs))
		return myBW *myWeight;
	else
		return 0;
}
double PSTor::getExitWeight(const Node* target, Ports* ports, Graph* g, Preferences* prefs) const
{
	double myBW = (double)target->getBandwidth();
	double myWeight;
	if(target->isGuard() && target->isExit())
	{
		myWeight = ((*g->getWeights())["Wed"]);
	}else if(target->isGuard()){
		myWeight = ((*g->getWeights())["Weg"]);		
	}else if(target->isExit()){
		myWeight = ((*g->getWeights())["Wee"]);
	}else{ // middle
		myWeight = ((*g->getWeights())["Wem"]);		
	}
	myWeight /= 10000;
	assert(myWeight <= 1);
	if (target->possibleExit(ports,prefs))
		return myBW * myWeight;
	else
		return 0;
}

bool PSTor::entryExitAllowed(const Node* entry, const Node* exit, Preferences* prefs) const
{
	return (!entry->related(exit));
}

void PSTor::print()
{
	cout << "Tor's path selection algorithm";
}
