#include <cstdlib>
// #include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>


using namespace std;
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
#include "main_makelargegraph.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

boost::timed_mutex mutex_write_sum;
boost::timed_mutex mutex_write_within_loop;
boost::timed_mutex mutex_alloc;


int main(int argci, char *argv[])
{
	unsigned int argc = (unsigned int) argci;
	time_t starttime;
	time(&starttime);
    bool silent = false;
    bool computegraph=false;
    bool largegraph = false;
    string graphOutputFile = "graph";
    string lgnum=""; // Used for largegraph only
    double proz;
    if(argc>1)
    {
        string a1 = argv[1];
        if(a1=="silent")
            silent=true;
        if(a1=="graph")
        {
            silent=true;
            computegraph=true;
        }
        if(a1=="largegraph")
        {
            silent=true;
            largegraph=true;
        }
        if(a1=="makelargegraph")
        {
            main_makelargegraph(argc,argv);
	    return EXIT_SUCCESS;
        }
    }

    cout << "Reading config file 'config.cfg'" << std::endl;
    ifstream myfile ("config.cfg");
    string line;
    double epsilon = 1.2;
    int bwpercentage = 100;
    unsigned int kmax = 1000;
    string consensusFile = "../../data/2014-02-05-03-00-00-consensus";

    Ports* conn1 = new Ports();
    Ports* conn2 = new Ports();

    Preferences* prefs = new Preferences();

    vector<string> myguards;

    string ps_name = "PSTor";

    if (!myfile.is_open())
    {
        cout << "Could not open config file 'config.cfg'" << endl;
    }else
        while ( getline (myfile,line) )
        {
            vector<string> tokens;
            tokens = tokenize(line,' ');
            if(tokens.size() < 1)
                continue;
            if(tokens[0] == "PathSelection")
            {
                ps_name = tokens[1];
                if(tokens.size()>2)
                    istringstream(tokens[2]) >> bwpercentage;
            }
            if(tokens[0] == "consensusFile")
                consensusFile = tokens[1];
            if(tokens[0] == "maxMultiplicativeFactor")
                istringstream(tokens[1]) >> epsilon;
            if(tokens[0] == "addPortToScenario1")
            {
                int port;
                istringstream(tokens[1]) >> port;
                conn1->addPort(port);
				cout << "Adding port " << port << " to Scenario 1" << endl;
            }
            if(tokens[0] == "addPortToScenario2")
            {
                int port;
                istringstream(tokens[1]) >> port;
                conn2->addPort(port);
				cout << "Adding port " << port << " to Scenario 2" << endl;
            }
            if(tokens[0] == "useGuards" && tokens.size() > 1)
            {
                prefs->setUseGuards(tokens[1] == "true");
            }
            if(tokens[0] == "exitNodes" && tokens.size() > 1)
            {
                prefs->setExitNodes(tokens[1] == "true");
            }
            if(tokens[0] == "allowNonValidEntry" && tokens.size() > 1)
            {
                prefs->setAllowNonValidEntry(tokens[1] == "true");
            }
            if(tokens[0] == "allowNonValidMiddle" && tokens.size() > 1)
            {
                prefs->setAllowNonValidMiddle(tokens[1] == "true");
            }
            if(tokens[0] == "allowNonValidExit" && tokens.size() > 1)
            {
                prefs->setAllowNonValidExit(tokens[1] == "true");
            }
            if(tokens[0] == "fastCircuits" && tokens.size() > 1)
            {
                prefs->setFastCircuit(tokens[1] == "true");
            }
            if(tokens[0] == "stableCircuits" && tokens.size() > 1)
            {
                prefs->setRequireStable(tokens[1] == "true");
            }

            if(tokens[0] == "guard")
            {
                if(tokens.size() < 4)
                    cout << "Not enough arguments for adding a guard. Syntax: guard [nickname@IP] [published date] [published time]";
                myguards.push_back(tokens[1] + " " + tokens[2] + " " + tokens[3]);
            }
        }

    if(argc>1)
    {
        for(unsigned int i=1; i < argc; i++)
        {
            string a = argv[i];

            writeParameterString(a,"ps=",ps_name);
            writeParameter<double>(a,"epsilon=",epsilon);
            writeParameter<unsigned int>(a,"kmax=",kmax);
            writeParameterString(a,"if=",consensusFile);
            writeParameterString(a,"of=",graphOutputFile);
            writeParameterString(a,"lgnum=",lgnum);
            writeParameter<double>(a,"proz=",proz);
        }
    }

	Graph* graph = new Graph(consensusFile);

	for (unsigned int i = 0; i < myguards.size(); i++)
	{
		for (unsigned int j = 0; j < graph->size(); j++)
		{
			if (myguards[i] == graph->getNode(j)->getID())
			{
				graph->getNode(j)->setAsMyGuard();
				if (prefs->useGuards())cout << "setting guard:";
				if (prefs->useGuards())graph->getNode(j)->print(false);

			}
		}
	}

	conn1->computeExitSupport(graph);

	conn2->computeExitSupport(graph);

	PathSelection* ps1;
	PathSelection* ps2;
	PathSelection* psSA;

	Preferences* prefsSA = new Preferences(prefs);
	prefsSA->setUseGuards(false);

	if (ps_name == "Distributor")
	{
		psSA = new PSDistributor(conn1, graph, prefsSA, bwpercentage);
		ps1 = new PSDistributor(conn1, graph, prefs, bwpercentage);
		ps2 = new PSDistributor(conn2, graph, prefs, bwpercentage);
	}
	else
	{
		psSA = new PSTor(conn1, graph, prefsSA);
		ps1 = new PSTor(conn1, graph, prefs);
		ps2 = new PSTor(conn2, graph, prefs);
	}


	//cout << "PATH SELECTION: \"";
	//ps1->print();
	//cout << "\"" << endl;

	WeightedGraph* wSA;
	wSA = new WeightedGraph(conn1, graph, psSA, prefsSA, true);

	WeightedGraph* w1;
	WeightedGraph* w2;
	w1 = new WeightedGraph(conn1, graph, ps1, prefs, true);
	w2 = new WeightedGraph(conn2, graph, ps2, prefs, true);


	time_t mytime;
	time(&mytime);
	//cout << "Time for preparation: " << mytime - starttime << " seconds." << endl;
	time(&starttime);
	//cout << "Computing sender anonymity. " << endl;
	AnoA* SA = new SenderAnonymity(graph, wSA);
	time(&mytime);
	//cout << "Time for sender anonymity: " << mytime - starttime << " seconds." << endl;
	time(&starttime);
	//cout << "Computing recipient anonymity for epsilon = " << epsilon << endl;
	AnoA* RA1 = new RecipientAnonymity(graph, w1, w2, epsilon);
	AnoA* RA2 = new RecipientAnonymity(graph, w2, w1, epsilon);
	time(&mytime);
	//cout << "Time for recipient anonymity: " << mytime - starttime << " seconds." << endl;
	time(&starttime);
	//cout << "Computing relationship anonymity for epsilon = " << epsilon << endl;
	RelationshipAnonymity* RelA1 = new RelationshipAnonymity(graph, w1, w2, epsilon);
	RelationshipAnonymity* RelA2 = new RelationshipAnonymity(graph, w2, w1, epsilon);
	time(&mytime);
	//cout << "Time for relationship anonymity: " << mytime - starttime << " seconds." << endl;

    // ### INTERACTIVE ###

    string input="dummy";

    while(true && !silent)
    {
        cout << "---------------------------------------------------" << endl;
        cout << "INPUT PREPARED. There are " << w1->size() << " many Tor relays." << endl;
        cout << "Please select which analysis you want to perform:" << endl;
        cout << "type 'SA' for Sender anonymity (most likely nodes)" << endl;
        cout << "type 'RA' for Recipient anonymity (most likely nodes)" << endl;
        cout << "type 'RelA' for Relationship anonymity (most likely nodes/edges)" << endl;
        cout << "type 'Stats' for bandwidth statistics" << endl;
        cout << "type 'Policies' statistics on the policies of exit nodes depending on the user's ports" << endl;
        cout << "type 'exit' to quit the program" << endl;
        cout << "Analysis :";
        cin >> input;
        cout << endl << "---------------------------------------------------" << endl;

        if(input=="exit")
            break;
        if(input=="SA")
        {
            unsigned int n;
            cout << "How many corrupted nodes?" << endl;
            cout << " : ";
            cin >> n;
            if(n>graph->size())
                continue;
            SA->outputGuarantees(n,graph);
        }
        if(input=="RA")
        {
            double delta1;
            double delta2;
            double delta=0;
            unsigned int n;
            cout << "How many corrupted nodes?" << endl;
            cout << " : ";
            cin >> n;
            if(n>graph->size())
                continue;
            delta1 = RA1->computeDelta(n,graph);
            delta2 = RA2->computeDelta(n,graph);
            if(delta2 > delta1)
                delta = delta2;
            else
                delta = delta1;
            cout << "delta (for recipient anonymity) = " << delta << endl;

        }
        if(input=="RelA")
        {
			double delta1, delta2, delta;
            unsigned int n;
            cout << "How many corrupted nodes?" << endl;
            cout << " : ";
            cin >> n;
            if(n > graph->size())
                continue;
            delta1 = RelA1->computeDelta(n, graph);
            delta2 = RelA2->computeDelta(n, graph);
            if(delta2 > delta1)
                delta = delta2;
            else
                delta = delta1;
            cout << "delta (for relationship anonymity) = " << delta << " (" << delta1 << "/" << delta2 << ")" << endl;
		}
        if(input=="Stats")
        {
            long int bw_total=0;
            long int bw_none=0;
            long int bwg_guard_only=0;
            long int bwe_exit_only=0;
            long int bwg_guard_and_exit=0;
            long int bwe_guard_and_exit=0;
            long int num_total=0;
            long int num_none=0;
            long int num_guard_only=0;
            long int num_exit_only=0;
            long int num_guard_and_exit=0;
            for(unsigned int i=0; i < graph->size(); i++)
            {
                int bw = graph->getNode(i)->getBandwidth();
                bw_total+=bw;
                num_total++;
                if(graph->getNode(i)->possibleEntry(conn1,prefs))
                {
                    if(graph->getNode(i)->possibleExit(conn1,prefs))
                    {
                        bwg_guard_and_exit+=w1->getNode(i)->entryWeight;
						bwe_guard_and_exit+=w1->getNode(i)->exitWeight;
                        num_guard_and_exit++;
                    }
                    else
                    {
                        bwg_guard_only+=w1->getNode(i)->entryWeight;
                        num_guard_only++;
                    }
                }else{
                    if(graph->getNode(i)->possibleExit(conn1,prefs))
                    {
						bwe_exit_only+=w1->getNode(i)->exitWeight;
                        num_exit_only++;
                    }else{
                        bw_none+=bw;
                        num_none++;
                    }
                }
            }
            cout << "Total bandwidth is:                "<< bw_total << " over " << num_total << " Nodes" << endl;
            cout << "Total bandwidth for 'none' is:     "<< bw_none << " over " << num_none << " Nodes" << endl;
            cout << "Total entry bandwidth for guard only is: "<< bwg_guard_only << " over " << num_guard_only << " Nodes" << endl;
            cout << "Total entry bandwidth for exit&guard is: "<< bwg_guard_and_exit << " over " << num_guard_and_exit << " Nodes" << endl;
            cout << "Total exit bandwidth for exit only is:  "<< bwe_exit_only << " over " << num_exit_only << " Nodes" << endl;
            cout << "Total exit bandwidth for exit&guard is: "<< bwe_guard_and_exit << " over " << num_guard_and_exit << " Nodes" << endl;
        }
        if(input=="Policies")
        {
            int accepters_acceptPolicy=0;
            int accepters_rejectPolicy=0;
            int rejecters_acceptPolicy=0;
            int rejecters_rejectPolicy=0;
            for(unsigned int i=0; i < graph->size(); i++)
            {
                if(graph->getNode(i)->possibleExit(conn1,prefs))
                {
                    if(graph->getNode(i)->policyIsAcceptPolicy)
                        accepters_acceptPolicy++;
                    else
                        accepters_rejectPolicy++;
                }else{
                    if(graph->getNode(i)->policyIsAcceptPolicy)
                    {
                        rejecters_acceptPolicy++;
                    }
                    else
                        rejecters_rejectPolicy++;

                }
            }
            cout << "Accepted by " << accepters_acceptPolicy + accepters_rejectPolicy << " nodes (" << accepters_acceptPolicy << " nodes with accept policy, " << accepters_rejectPolicy << " nodes with reject policy) and rejected by " << rejecters_acceptPolicy + rejecters_rejectPolicy << " (" << rejecters_acceptPolicy << " nodes with accept policy, " << rejecters_rejectPolicy << " nodes with reject policy)" << endl;
        }

    }
    if(computegraph)
    {
        ofstream graphSA;
        string fname = graphOutputFile + "-SA";
        cout << "Writing to file " << graphOutputFile << endl;
        graphSA.open(fname.c_str());
        ofstream graphRA;
        fname = graphOutputFile + "-RA";
        graphRA.open(fname.c_str());
        ofstream graphRelA;
        fname = graphOutputFile + "-RelA";
        graphRelA.open(fname.c_str());

        //if (proz != 0)
        //    kmax = graph->getFractionSize((proz/100), conn1, prefs);
        for(unsigned int k=0; k< kmax; k++)
        {
            double delta1, delta2, delta = 0;
            delta1 = RA1->computeDelta(k,graph);
            delta2 = RA2->computeDelta(k,graph);
            if(delta2 > delta1)
                delta = delta2;
            else
                delta = delta1;
            graphRA << k << "\t" << delta << endl;
            graphSA << k << "\t" << SA->computeDelta(k,graph) << endl;
            delta1 = RelA1->computeDelta(k,graph);
            delta2 = RelA2->computeDelta(k,graph);
            if(delta2 > delta1)
                delta = delta2;
            else
                delta = delta1;
            graphRelA << k << "\t" << delta << endl;
        }
		/*
		ofstream nodesizes_all;
        nodesizes_all.open("graph-nodesizes-all");
		ofstream nodesizes_entry;
        nodesizes_entry.open("graph-nodesizes-entry");
		ofstream nodesizes_exit;
        nodesizes_exit.open("graph-nodesizes-exit");
		vector<double> nodeW_all;
		vector<double> nodeW_entry;
		vector<double> nodeW_exit;
		for(unsigned int i=0; i < graph->size(); i++)
		{
			nodeW_all.push_back(graph->getNode(i)->getBandwidth());
			nodeW_entry.push_back(w1->getNode(i)->entryWeight);
			nodeW_exit.push_back(w1->getNode(i)->exitWeight);
		}
		sort(nodeW_all.begin(), nodeW_all.end());
		reverse(nodeW_all.begin(), nodeW_all.end());
		sort(nodeW_entry.begin(), nodeW_entry.end());
		reverse(nodeW_entry.begin(), nodeW_entry.end());
		sort(nodeW_exit.begin(), nodeW_exit.end());
		reverse(nodeW_exit.begin(), nodeW_exit.end());
		for(unsigned int i=0; i < graph->size(); i++)
		{
			nodesizes_all << i << "\t" << nodeW_all[i] << endl;
			nodesizes_entry << i << "\t" << nodeW_entry[i] << endl;
			nodesizes_exit << i << "\t" << nodeW_exit[i] << endl;
		}
		*/

    }
    if(largegraph)
    {
       /* ofstream graphSA;
        string fname = "largegraph-" + graphOutputFile + "-SA";
        cout << "Writing to file " << fname << endl;
        graphSA.open(fname.c_str(), std::ofstream::app);

        ofstream graphRA;
        fname = "largegraph-" + graphOutputFile + "-RA";
        graphRA.open(fname.c_str(), std::ofstream::app);

        ofstream graphRelA;
        fname = "largegraph-" + graphOutputFile + "-RelA";
        graphRelA.open(fname.c_str(), std::ofstream::app);*/

		ofstream monthFile;
		string fname = graphOutputFile;
		monthFile.open(fname.c_str(), std::ofstream::app);

        if (proz != 0)
            kmax = graph->getFractionSize((proz/100), conn1, prefs);

        //graphSA << lgnum << "\t" << SA->computeDelta(kmax,graph) << "\n";

        double delta1;
        double delta2;
        delta1 = RA1->computeDelta(kmax,graph);
        delta2 = RA2->computeDelta(kmax,graph);
        //graphRA << lgnum << "\t" << (delta2 > delta1 ? delta2 : delta1) << "\n";
		monthFile << SA->computeDelta(kmax, graph) << "," << (delta2 > delta1 ? delta2 : delta1) << ",";

        delta1 = RelA1->computeDelta(kmax,graph);
        delta2 = RelA2->computeDelta(kmax,graph);
        //graphRelA << lgnum << "\t" << (delta2 > delta1 ? delta2 : delta1) << "\n";
		monthFile << (delta2 > delta1 ? delta2 : delta1) << "," << lgnum << "\n";
		monthFile.close();


    }

    #ifdef __WIND64__
    system("PAUSE");
    #elif __WIN32__
    system("PAUSE");
    #elif __APPLE__
    system("echo Finished");
    #endif
    return EXIT_SUCCESS;
}
