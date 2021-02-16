#include "ports.h" // class's header file
#include "functions.h" // class's header file
#include <iostream>

// class constructor
Ports::Ports()
{
	// insert your code here
}

// class destructor
Ports::~Ports()
{
	// insert your code here
}

void Ports::addPort(int port)
{
	this->ports.push_back(port);
}

unsigned int Ports::size() const
{
	return this->ports.size();
}

int Ports::getPort(unsigned int index) const
{
	assert(index < this->ports.size());
	return this->ports[index];
}

void Ports::computeExitSupport(Graph* g)
{
	// Compute how many exit nodes "best" support the ports

	//Debug:
    if(VERBOSE)cout << endl << "Computing exit support of the graph" << endl;

	this->bestSupport=0;
	for(unsigned int i=0; i < g->size(); i++)
	{
		if(g->getNode(i)->computeSupport(this) > this->bestSupport)
			this->bestSupport = g->getNode(i)->computeSupport(this);
	}
	//Debug:
	if(VERBOSE)cout << "Best support is: " << this->bestSupport << endl;

	int total=0;
	for(unsigned int i=0; i < g->size(); i++)
	{
		if(g->getNode(i)->computeSupport(this) == this->bestSupport)
			total+= g->getNode(i)->getBandwidth();
	}
	this->totalSupportedExitBandwidth = total;
	
}

int Ports::getBestSupport() const
{
	return this->bestSupport;
}

