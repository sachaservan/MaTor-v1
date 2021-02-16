// Here we define a few general functions.


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "assert.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#ifdef __APPLE__
	#include <sqlite3.h>
#else
	#include "sqlite3.h"
#endif

const unsigned int USED_THREADS = 20;
const unsigned int SET_SIZE = 500;
const bool VERBOSE = false;

const double minW = 0;

using namespace std;

typedef vector<string> Row;
/* typedef Row *RowP; */
typedef vector<Row> QueryResult;
typedef QueryResult *QueryResultP;
typedef string Hash;
typedef map<Hash,pair<string, Row> > FamilyFingerprintMap;

string checkParameter(string text, string parameter_name);

template <typename T>
void writeParameter(string text, string parameter_name, T& goal)
{
	string c = checkParameter(text, parameter_name);
	if(c != "")
	{
		istringstream(c) >> goal;
		cout << "Reading " << parameter_name << goal << endl;
	}
}

void writeParameterString(string text, string parameter_name, string &goal);
std::vector<std::string> tokenize(const std::string& str, char delim);
bool findToken(vector<string>& myTokens, string searchItem);

QueryResultP execute_sqlite_query (sqlite3 *db_handle, string query);

#endif //FUNCTIONS_H
