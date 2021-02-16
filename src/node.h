#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <map>
#ifdef __APPLE__
	#include <sqlite3.h>
#else
	#include "sqlite3.h"
#endif
#include "preferences.h"
#include "ports.h"
using namespace std;

/*
 * Describes an onion routing relay.
 */
class Ports;
class Node
{
	public:
		// class constructor
		Node(string name, string IP, string published, bool guard, bool exit, bool badexit, bool fast, bool valid, bool stable, bool running, int bandwidth, string mypolicy, sqlite3*);
		// class destructor
		~Node();
		int getDistance(Node*) const;
		int getLocation() const;
		string getPublished() const;
		string getName() const;
		string getIP() const;
		int getBandwidth() const;
		bool isGuard() const;
		bool isMyGuard() const;
		void setAsMyGuard(){this->is_my_guard=true;};
		bool isExit() const;
		bool isBadExit() const;
		bool isFast() const;
		bool isValid() const;
		bool isStable() const;
		bool isRunning() const;
		string getID() const;
		void print(bool recursive=true) const;
		bool isFamily(const Node* n) const;
		bool isSubnet(const Node* n) const;
		bool possibleEntry(Ports* ports, Preferences* prefs) const;
		bool possibleMiddle(Ports* ports, Preferences* prefs) const;
		bool possibleExit(Ports* ports, Preferences* prefs) const;
		int computeSupport(Ports* conn) const;
		bool related(const Node*  n) const; // returns true if the nodes are not related (not in the same subnet, not in the same family)
		void setFingerprint(string fingerprint);
		string getFingerprint() const;
		void addFamilyMember(Node* member);
		void addFamilyOfMember(Node* member);
		void addIPFamilyMember(Node* member);
		vector<Node*>* getFamily() const;
		vector<Node*>* getFamilyOf() const;
		void setBelievedFamily (vector<string> believed_family);
		vector<string> getBelievedFamily() const;
		long int combined_middle_bandwidth(const Node* other, long int other_family_sum, const Node* sender, const Node* receiver, Preferences* prefs) const;
		vector<Node*>* family;
		vector<Node*>* IPfamily;
		string subnet;
		unsigned int myPos;
		bool policyIsAcceptPolicy;
		bool supportsPort(int port) const;
		string getCountry() const;
		void setCountry(string);
	private:
		int location;
		string name;
		string IP;
		string published;
		string fingerprint;
		int bandwidth;
		bool guard;
		bool is_my_guard;
		bool exit;
		bool badexit;
		bool fast;
		bool valid;
		bool stable;
		bool running;
		vector<string> policy;
		int IPtoGPS(string IP);
		vector<Node*>* family_of;
		vector<string> believed_family;
		string country;
		
		void getAdditionalNodeInfo(sqlite3*);
};
#endif // NODE_H
