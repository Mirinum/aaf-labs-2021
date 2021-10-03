#include "back.hpp"
#include "outputtools.hpp"
#include <climits>

#define BRANCHSIZE 30

using namespace std;

table::table(string n, string c [], bool ind [], unsigned int c_n){
	name = n;
	for(int i = 0; i < c_n; i++){
		columns.push_back(c[i]);
		indexes.push_back(ind[i]);
	}
	branches.push_back(branchNode(1,100,columns));
	otools::colored_out("Created a table "+this->name+" with columns: \n", green );
	for(int i = 0; i < c_n; i++) cout << columns[i] << (indexes[i] ? " (INDEXED) | " : " | ");
	cout << endl;
}

branchNode::branchNode(int b, int t, vector<string> cols){
	bottom = b;
	top = t;
	for(int i = 0; i < 10; i++) leaves.push_back(leafNode(cols));
}

int branchNode::getSpace(){
	return (top - bottom) - quantity;
}

void table::insert(map<string,string> row){
	if(branches.back().getSpace() == 0) branches.push_back(branchNode((branches.size()*100)+1, (branches.size()+1)*100, columns));
	branches.back().insert(row);
}

int leafNode::getSpace(){
	return data.size();
}

void branchNode::insert(map<string,string> row){
	cout << "Inserted values" << endl << "============================================" << endl << "id: " << quantity << endl;
	leaves[quantity/10].insert(row);
	quantity++;
}

void leafNode::insert(map<string,string> row){
	for(map<string, vector <string>>::iterator i = data.begin(); i != data.end(); i++){
		cout << i->first << ": ";
		if(row.count(i->first) == 0){
			i->second.push_back("NULL");
			cout << "NULL" << endl;
		}
		else{
			i->second.push_back(row[i->first]);
			cout << row[i->first] << endl;
		}
	}
}

leafNode::leafNode(vector<string> cols){
	for(string col : cols){
		data.insert(pair<string, vector <string>>(col, vector<string>()));
	}
}