#ifndef BACKEND_H
#define BACKEND_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include <regex>
#include <utility>
#include <iterator>
#include <boost/algorithm/string.hpp>

using namespace std;

class leafNode{
	map<string, vector <string>> data;
public:
	leafNode(vector<string> cols);
	int getSpace();
	void insert(map<string,string> row);
};

class branchNode{
	int top, bottom, quantity;
	vector<leafNode> leaves;
public:
	branchNode(int b, int t, vector<string> cols);
	int getSpace();
	void insert(map<string,string> row);
};

class table{
	string name="unnamed";
	vector <string> columns;
	vector <bool> indexes;
	vector <branchNode> branches;
public:
	table(string n, string c [], bool ind [], unsigned int c_n);
	void insert(map<string,string> row);
};




class DBexception : public exception{
	const char* errorMSG;
public:
	DBexception(string s){errorMSG = s.c_str();}
	const char * what () const throw ()
    {
    	return errorMSG;
    }
};

#endif