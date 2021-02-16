#ifndef GRAPH_H
#define GRAPH_H

#include<string>
#include<vector>
#include <map>
#include "node.h"
#include "preferences.h"
#include "ports.h"
#include "functions.h"
#include <boost/thread/thread.hpp> 


/*
 * Describes a selection of nodes. This graph does not, in fact, have any edges.
 */
class Ports;
class Node;
class Graph
{
	public:
		// class constructor
		Graph(string consensus_filename,  string server_descriptor_db_filename = "");
		// class destructor
		~Graph();
		void addNode(string name, string IP, string published, bool guard, bool exit, bool badexit, bool fast, bool valid, bool stable, bool running, int bandwidth, string policy, sqlite3* database_handle);
		Node* getNode(int i);
		void print();
		map<string,int>* getWeights();
		const unsigned int size();
		int getBestSupport();
		int getTotalSupportedExitBandwidth();
		bool related(int i, int j) const{return  myRelatedArray[i][j];}
		vector<vector<bool> > myRelatedArray;
		void lockSet(unsigned int i){this->graph_mutex[i]->lock();}
		bool trylockSet(unsigned int i){return this->graph_mutex[i]->try_lock();}
		void unlockSet(unsigned int i){this->graph_mutex[i]->unlock();};
		unsigned int getNumberEntryNodes(Ports* ports, Preferences* prefs);
		unsigned int getNumberExitNodes(Ports* ports, Preferences* prefs);
		unsigned int getFractionSizeEntry(double fraction, Ports* ports, Preferences* prefs);
		unsigned int getFractionSizeExit(double fraction, Ports* ports, Preferences* prefs);
		unsigned int getFractionSize(double fraction, Ports* ports, Preferences* prefs);
	private:
		vector<Node> nodes;
		map<string,int>* weights;
		FamilyFingerprintMap *family_fingerprint_map;
		vector<boost::timed_mutex*> graph_mutex;
		int bestSupport;
		int totalSupportedExitBandwidth;
		void assignFamilies();
		FamilyFingerprintMap *constructFamilyFingerprintMapAndReadMoreInfo(const char *server_descriptor_db_filename);
};
		

#endif // GRAPH_H
