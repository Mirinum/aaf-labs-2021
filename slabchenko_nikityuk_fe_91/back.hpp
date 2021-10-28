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

class VectorList;

class node{
	VectorList* parent = nullptr; 
public:
	node(VectorList* p);
	vector<string> data = vector<string>();
	node* next = nullptr;
	node* prev = nullptr;
	void remove();
	string operator[](int index);
};

class VectorList{    
	friend class node;
	node* root = nullptr;
	int s = 0;
public:
	node* push_back(vector<string> d);
	node& operator[](int index);
	int size();
};

class table{
	string name;
	map<int, string> columns;
	map<string, map<string, node*>> indexes; // indexes[name of the index][value of the indexed column]
	VectorList data;
public:
	table(string n, string c [], bool ind [], unsigned int c_n);
	void insert(vector<string> d);
	void remove(string condition);
	string get_name(){return name;}
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