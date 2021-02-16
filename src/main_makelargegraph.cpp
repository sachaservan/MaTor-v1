

#include <cstdlib>
// #include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>



using namespace std;
#include "main_makelargegraph.h"
#include "node.h"
#include "nodeweight.h"
#include "graph.h"
#include "weightedgraph.h"
#include "pathselection.h"
#include "preferences.h"
#include "ports.h"
#include "ps_tor.h"
#include "ps_distributor.h"
#include "anoa.h"
#include "senderanonymity.h"
#include "recipientanonymity.h"
#include "functions.h"

#include <boost/thread/thread.hpp> 
#include <boost/bind.hpp>


int main_makelargegraph(unsigned int argc, char *argv[])
{
	string lgnum=""; // Used for largegraph only
	double proz = 0.5;
	string epsilon = "1.2";
	string yearMonth = "2014-02";
	unsigned int maxD = 30;
	unsigned int hour_interval = 3;
	bool doItWithoutEpsilon = false;
	int sysResult;
	for(unsigned int i=1; i < argc; i++)
	{
		string a = argv[i];
		writeParameterString(a,"epsilon=",epsilon);
		writeParameterString(a,"YM=",yearMonth);
		writeParameter<unsigned int>(a,"maxD=",maxD);
		writeParameter<unsigned int>(a,"hours=",hour_interval);
		writeParameter<double>(a,"proz=",proz);
		writeParameter<bool>(a,"noeps=",doItWithoutEpsilon);
	}
	// constructs a graph of the anonymity over several days.
	if (! doItWithoutEpsilon)
       	{				
		sysResult = system(string("rm largegraph-Tor-Epsilon-" + yearMonth + "-SA").c_str());
		sysResult = system(string("rm largegraph-Tor-Epsilon-" + yearMonth + "-RA").c_str());
		sysResult = system(string("rm largegraph-Distributor-Epsilon-" + yearMonth + "-SA").c_str());
		sysResult = system(string("rm largegraph-Distributor-Epsilon-" + yearMonth + "-RA").c_str());
	}

	sysResult = system(string("rm largegraph-Tor-" + yearMonth + "-SA").c_str());
	sysResult = system(string("rm largegraph-Tor-" + yearMonth + "-RA").c_str());
	sysResult = system(string("rm largegraph-Distributor-" + yearMonth + "-SA").c_str());
	sysResult = system(string("rm largegraph-Distributor-" + yearMonth + "-RA").c_str());

	for(unsigned int day=1; day < maxD+1; day++)
	{
		stringstream daystream;
		if(day<10)
			daystream << "0";
		daystream << day;
		for(unsigned int hour = 0; hour < 24; hour+=hour_interval)
		{
			stringstream hourstream;
			if(hour<10)
				hourstream << "0";
			hourstream << hour;
			stringstream command1;
			stringstream command2;
			lgnum = yearMonth + "-" + daystream.str() + "-" + hourstream.str();
			string folder = "../data/consensuses-" + yearMonth + "/";

			if (! doItWithoutEpsilon)
			{
				command1 << "./mator largegraph if=" << folder <<
					daystream.str() << "/" << yearMonth << "-" << daystream.str() << "-" << hourstream.str();
				command1 << "-00-00-consensus ps=Tor epsilon=" << epsilon
					 << " proz=" << proz << "of=Tor-Epsilon-" << yearMonth << " lgnum="
					 << lgnum << " >> anoa-" << yearMonth << ".log";
				sysResult = system(command1.str().c_str());
				command2 << "./mator largegraph if=" << folder <<
					daystream.str() << "/" << yearMonth << "-" << daystream.str() << "-" << hourstream.str();
				command2 << "-00-00-consensus ps=Distributor epsilon=" << epsilon << " proz=" << proz << "of=Distributor-Epsilon-" << yearMonth << " lgnum=" << lgnum << " >> anoa-" << yearMonth << ".log";
				sysResult = system(command2.str().c_str());
			}

			stringstream command3;
			stringstream command4;
		
			command3  << "./mator largegraph if=" << folder <<
				daystream.str() << "/" << yearMonth << "-" << daystream.str() << "-" << hourstream.str();
			command3 << "-00-00-consensus ps=Tor epsilon=1 proz=" << proz << " of=Tor-" << yearMonth << " lgnum=" << lgnum << " >> anoa-" << yearMonth << ".log";
			sysResult = system(command3.str().c_str());
			command4  << "./mator largegraph if=" << folder <<
				daystream.str() << "/" << yearMonth << "-" << daystream.str() << "-" << hourstream.str();
			command4 << "-00-00-consensus ps=Distributor epsilon=1 proz=" << proz << " of=Distributor-" << yearMonth << " lgnum=" << lgnum << " >> anoa-" << yearMonth << ".log";
			sysResult = system(command4.str().c_str());
		}
	}
	sysResult = system("echo Finished");
	return sysResult;
}

