// Here we define a few general functions.

#include "functions.h" // header file
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

std::vector<std::string> tokenize(const std::string& str, char delim) 
{
    std::vector<std::string> tokens;
    std::stringstream mySstream(str);
    std::string temp;

    while(getline(mySstream, temp, delim))
        tokens.push_back(temp);

    return tokens;
} 

bool findToken(vector<string>& myTokens, string searchItem)
{
	for(size_t i = 0; i < myTokens.size(); ++i)
		if(myTokens[i]==searchItem)
			return true;
	return false;
}

void writeParameterString(string text, string parameter_name, string &goal)
{
	string c = checkParameter(text, parameter_name);
	if(c != "")
	{
		goal = c;
		cout << "Reading " << parameter_name << goal << endl;
	}
}


string checkParameter(string text, string parameter_name)
{
	size_t found = text.find(parameter_name);
	if(found!=std::string::npos)
	{
		return text.substr(found+parameter_name.length(),text.length()-found-parameter_name.length());
	}
	return "";
}

QueryResultP execute_sqlite_query (sqlite3 *db_handle, string query)
{
	sqlite3_stmt *statement = 0;
	if(VERBOSE)
	{
		std::cout << "Preparing statement for: ";
		if(query.length() < 450)
			std::cout << query << std::endl;
		else
			std::cout << query.substr(0, 150) << " ... " << query.substr(query.length() - 150) << std::endl;
	}
	//query = "SELECT 1 FROM familyview;";
	//std::cout << "trimming to: " << query << std::endl;
	int res_prepare = sqlite3_prepare(db_handle, query.c_str(), -1, &statement, NULL);
	string s;
	Row row;
	QueryResultP result = new QueryResult();
	if ( res_prepare == SQLITE_OK ) 
	{
		int ctotal = sqlite3_column_count(statement);
		int res_step = sqlite3_step(statement);
		
		while ( res_step != SQLITE_DONE && res_step !=SQLITE_ERROR )         
		{
			if ( res_step == SQLITE_ROW ) 
			{
				//row.erase(row.begin(), row.end());
				row.clear();
				for ( int i = 0; i < ctotal; i++ ) 
				{
					const char* str = (const char*)sqlite3_column_text(statement, i);
					if(str)
						s = string(str);
					else
						s = "";
					// cout << "Column: " << i << ". Entry: " << s << endl;
					row.push_back(s);
				}
				result->push_back(row);
				res_step = sqlite3_step(statement);
			}
		}
		if( res_step != SQLITE_DONE )
		{
			cout << "SQL error" << s << endl;
			// sqlite3_free(error_message);
		}
	} else if ( res_prepare != SQLITE_OK )
	{
		cout << "SQL error in query preparation." << endl;
		cout << "Code " << res_prepare << " (" << db_handle << ") " << sqlite3_errmsg(db_handle) << endl;
		// sqlite3_free(error_message);
	}
	if(VERBOSE) cout << "Returning..." << endl;
	return result;
}

