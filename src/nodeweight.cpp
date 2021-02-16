#include "nodeweight.h" // class's header file

// class constructor
NodeWeight::NodeWeight(const Node* n, int mypos)
{
	this->node = n;
	this->used_as_entry=0;
	this->used_as_middle=0;
	this->used_as_exit=0;
	this->quotient_entry=0;
	this->quotient_middle=0;
	this->quotient_exit=0;	
	this->posInGraph=mypos;
	// insert your code here
}

// class destructor
NodeWeight::~NodeWeight()
{
	// insert your code here
}

void NodeWeight::computeQuotients(NodeWeight* n)
{
	this->quotient_entry  = (n->used_as_entry !=0)?(this->used_as_entry / n->used_as_entry):-1;
	this->quotient_middle = (n->used_as_middle !=0)?(this->used_as_middle / n->used_as_middle):-1;
	this->quotient_exit   = (n->used_as_exit !=0)?(this->used_as_exit / n->used_as_exit):-1;
}

const Node* NodeWeight::getNode() const
{
	return this->node;
}
