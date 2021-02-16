#include "node.h" // class's header file
#include <iostream>
#include <sstream>
#include "functions.h"
#include <boost/algorithm/string.hpp>

// class constructor
Node::Node(string myname, string myIP, string mypublished, bool myguard, bool myexit,bool mybadexit, bool myfast, bool myvalid, bool mystable, bool myrunning, int mybandwidth, string mypolicy, sqlite3* database_handle)
{
	this->name = myname;
	this->IP = myIP;
	this->published = mypublished;
	this->guard=myguard;
	this->is_my_guard=false;
	this->exit=myexit;
	this->badexit = mybadexit;
	this->fast=myfast;
	this->valid=myvalid;
	this->stable=mystable;
	this->running=myrunning;
	this->bandwidth=mybandwidth;
	this->location = this->IPtoGPS(myIP);
	this->family = NULL;
	this->IPfamily = new vector<Node*>();
	this->family_of = new vector<Node*>();
	this->fingerprint = "";
	vector<string> tmp = tokenize(this->IP,'.');
	this->subnet = tmp[0] + "." + tmp[1];
	this->country = "";

	this->getAdditionalNodeInfo(database_handle);

	// Parse the policy for this node.
	if (mypolicy!="")
	{
		vector<string> p = tokenize(mypolicy,',');
		// We consider a policy to be an access policy
		if(p[0] == "accept" || p[0]=="reject")
			this->policyIsAcceptPolicy = (p[0] == "accept");
		else
		{
			cout << "My policy is neither access nor reject policy. I am: "; 
			this->print(false);
			assert(false);
		}
		// Read all the IP's and IP ranges and put them into the policy vector
		for(unsigned int i=1; i < p.size(); i++)
		{
			this->policy.push_back(p[i]);
		}
	}
}

// class destructor
Node::~Node()
{
	// insert your code here
}

int Node::getLocation() const
{
	return this->location;
}
string Node::getName() const
{
	return this->name;
}
string Node::getIP() const
{
	return this->IP;
}

string Node::getPublished() const
{
	return this->published;
}

int Node::getBandwidth() const 
{	return this->bandwidth;}

bool Node::isGuard() const
{	return this->guard;}

bool Node::isExit() const
{	return this->exit;}

bool Node::isBadExit() const
{	return this->badexit;}

bool Node::isFast() const
{	return this->fast;}

bool Node::isValid() const
{	return this->valid;}

bool Node::isStable() const
{	return this->stable;}

string Node::getID() const
{
	return this->getName() + "@" + this->getIP() + " " + this->getPublished();
}

int Node::IPtoGPS(string IP)
{
	return 5;
}

bool Node::isFamily(const Node* n) const
{
	if(this->family==NULL)
		return false;
	for(vector<Node*>::iterator i = this->family->begin(); i != this->family->end(); ++i)
	{
		if (*i == n)
			return true;
	}
	return false;
}
bool Node::isSubnet(const Node* n) const
{
	return (this->subnet == n->subnet);
}


void Node::setFingerprint(string myfingerprint)
{
	this->fingerprint = myfingerprint;
}

string Node::getFingerprint() const
{
	return this->fingerprint;
}

vector<Node*>* Node::getFamily() const
{
	assert(false);
	return this->family;
}

vector<Node*>* Node::getFamilyOf() const
{
	return this->family_of;
}

void Node::addFamilyMember(Node* member)
{
	if(this->family==NULL)
		this->family = new vector<Node*>();
	family->push_back(member);
}

void Node::addFamilyOfMember(Node *member)
{
	family_of->push_back(member);
}

void Node::addIPFamilyMember(Node* member)
{
	IPfamily->push_back(member);
}

void Node::setBelievedFamily(vector<string> mybelieved_family)
{
	this->believed_family = mybelieved_family;
}

vector<string> Node::getBelievedFamily() const
{
	return this->believed_family;
}



// checks whether the node can be used as entry node of a circuit.
bool Node::possibleEntry(Ports* ports, Preferences* prefs) const 
// This function checks whether the node can be used as an entry node for the given circuit. This will depend on the preferences.
{
	if(prefs->fastCircuit() && this->fast==false)
		return false;
	if(prefs->requireStable() && this->stable==false)
		return false;
	if(prefs->useGuards())
	{
		if(this->is_my_guard==false)
			return false;
	}
	else
	{	if(this->guard==false)
			return false;
	}
	if(!prefs->allowNonValidEntry() && this->valid==false)
		return false;
	
	return true;
}
bool Node::possibleMiddle(Ports* ports, Preferences* prefs) const 
{
	if(prefs->fastCircuit() && this->fast==false)
		return false;
	if(prefs->requireStable() && this->stable==false)
		return false;
	// This function checks whether the node can be used as a middle node for the given circuit.
	if(!prefs->allowNonValidMiddle() && this->valid==false)
		return false;

	return true;
}
bool Node::possibleExit(Ports* ports, Preferences* prefs) const 
{
	// This function checks whether the node can be used as an exit node for the given circuit. This will depend on preferences as well as some connection stuff and ports.
	if(prefs->fastCircuit() && this->fast==false)
		return false;
	if(prefs->requireStable() && this->stable==false)
		return false;
	if(!prefs->allowNonValidExit() && this->valid==false)
		return false;
	if(prefs->exitNodes() && this->exit==false)
		return false;
	if(this->badexit)
		return false;
	if(this->computeSupport(ports) < ports->getBestSupport())
	{
		//cout << "This node does not support the connection:";
		return false;
	}
	// In their code they use, e.g.: compare_tor_addr_to_node_policy(addrp, conn->socks_request->port,exit);
	// to find out whether the policy fits
	// this is in connection_edge.c, line 2801 ff.
	return true;
}

bool Node::related(const Node*  n) const
// returns true if the nodes are related (in the same subnet or in the same family)
{
	return (this->subnet == n->subnet) || (this->family!= NULL && n->family!=NULL && this->isFamily(n));
}
//Computes the number of ports that are supported by the node.
int Node::computeSupport(Ports* conn) const
{
	int support=0;
	for(unsigned int i=0; i < conn->size(); i++)
		if(this->supportsPort(conn->getPort(i)))
			support++;
	return support;
}

bool Node::supportsPort(int port) const
{
//Accept only if policy is accept policy and we have found the port or if policy = reject policy and did not find the port.	
	if(this->policy.size() ==0)
		return true; // No policy => supported.

	for(unsigned int i=0; i < this->policy.size(); i++)
	{
		if(policy[i].find("-")!=std::string::npos) // Port range
		{
			int low; int high;
			vector<string> tmp = tokenize(policy[i],'-');
			if(tmp.size()!=2)
			{
				cout << "The policy {" << policy[i] << "} of this node is weird:";
				this->print();
				assert(false);
			}
			istringstream(tmp[0]) >> low;
			istringstream(tmp[1]) >> high;
			if(low <= port && port <= high)
			{
				return this->policyIsAcceptPolicy; // We found it, so we return true iff this is an accept policy
			}
		}else{
			int p;
			istringstream(policy[i]) >> p;
			if(port == p)
				return this->policyIsAcceptPolicy; // We found it, so we return true iff this is an accept policy
		}
	}
	
	return (!this->policyIsAcceptPolicy); // We did not find it, so we return true iff this is an reject policy
}

long int Node::combined_middle_bandwidth(const Node* other, long int other_family_sum, const Node* sender, const Node* receiver, Preferences* prefs) const
{
	//if(this->)
	long int combined = other_family_sum;
	for(unsigned int i=0; this->family!=NULL && i < this->family->size(); i++)
	{
		if(!other->related((*this->family)[i]))
			combined+=(*this->family)[i]->getBandwidth();
	}
	for(unsigned int i=0; i < this->IPfamily->size(); i++)
	{
		if(!other->related((*this->IPfamily)[i]))
			combined+=(*this->IPfamily)[i]->getBandwidth();
	}
	return combined;
}

void Node::print(bool recursive) const
{
	cout << "Node \"" << this->name << "\"@" << this->IP << " [Guard=" << this->guard << ", Exit=" << this->exit << ", Bandwidth=" << this->bandwidth << ", subnet=" << this->subnet;
	if(this->policy.size()>0 && VERBOSE)
	{
		cout << ", policy = {accept=" << this->policyIsAcceptPolicy;

		for(unsigned int i=0; i < this->policy.size(); i++)
			cout << ", <" << this->policy[i] << ">";
	cout << "}";
	}
	cout << "]"<< endl;
	if(this->family!=NULL && recursive)
	{
		for(unsigned int i=0; i< this->family->size(); i++)
		{
			cout << "Family member: ";
			(*this->family)[i]->print(false);
		}
	}
	if(recursive)
	{
		for(unsigned int i=0; i< this->IPfamily->size(); i++)
		{
			cout << "In same subnet: ";
			(*this->IPfamily)[i]->print(false);
		}
	}
}

void Node::getAdditionalNodeInfo(sqlite3 *database_handle)
{
	// TBA
}

string Node::getCountry() const
{
	return this->country;
}

void Node::setCountry(string _)
{
	this->country = _;
}
