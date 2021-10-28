#include "back.hpp"
#include "outputtools.hpp"
#include <climits>

using namespace std;

node* VectorList::push_back(vector<string> d){
	if(root==nullptr){
		root = new node(this);
		root->data = d;
		s++;
		return root;
	}
	else{
		node* temp = root;
		node* temp_prev = nullptr;
		while(temp->next != nullptr){
			temp_prev = temp;
			temp = temp->next;
		}
		temp->next = new node(this);
		temp->next->data = d;
		temp->prev = temp_prev;
		s++;
		return temp->next;
	}
}

node::node(VectorList* p){
	parent = p;
}

node& VectorList::operator[](int index){
	node* temp = root;
	while(index != 0){
		index--;
		if(temp->next == nullptr) throw DBexception("Invalid index!");
		temp = temp->next;
	}
	return *temp;
}

int VectorList::size(){
	return s;
}

table::table(string n, string c [], bool ind [], unsigned int c_n){
	name = n;
	for(int i = 0; i < c_n; i++){
		columns.insert(pair<int, string>(i, c[i]));
		if(ind[i]){
			indexes.insert(pair<string,map<string,node*>>(c[i], map<string,node*>()));
		}
	}
}

void table::insert(vector<string> d){
	node* new_node = data.push_back(d);
	for(int i = 0; i < columns.size(); i++){
		if(indexes.count(columns[i]) == 1){
			indexes.at(columns[i]).insert(pair<string, node*>(d[i], new_node));
		}
	}
}

void table::remove(string condition){
	int deleted = 0;
	if(condition == "") {
		for(int i = 0; i < columns.size(); i++){
			if(indexes.count(columns[i]) == 1){
				indexes.at(columns[i]).clear();
			}
		}
		for(int i = data.size(); i != 0; i--){
			data[i].remove();
			deleted++;
		}
	}
	else{

		regex r("\\s+(((['\"`])[\\w\\s]*\\3)|\\w+)\\s+(=|!=|<|>|<=|>=)\\s+(((['\"`])[\\w\\s]*\\7)|\\w+)\\s+");
		smatch m;
		regex_search(condition, m, r);
		if (m[0] == condition) {
			string operand1 = m[1].str();
			string operand2 = m[5].str();
			string op = m[4].str();
			int column1 = 0;
			int column2 = 0;
			if(operand1[1] != '"' || operand1[1] != '`' || operand1[1] != '\'')
				for(int j = 0; j < columns.size(); j++) 
					if(columns[j] == operand1)
						column1 = j;
			if(operand2[1] != '"' || operand2[1] != '`' || operand2[1] != '\'')
				for(int j = 0; j < columns.size(); j++) 
					if(columns[j] == operand2)
						column2 = j;
			if(op == "="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 == op2) {data[i].remove(); deleted++;}
				}
			}
			else if(op == "!="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 != op2) {data[i].remove(); deleted++;}
				}
			}
			else if(op == "<"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 < op2) {data[i].remove(); deleted++;}
				}
			}
			else if(op == ">"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 > op2) {data[i].remove(); deleted++;}
				}
			}
			else if(op == ">="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 >= op2) {data[i].remove(); deleted++;}
				}
			}
			else if(op == "<="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand1[1] == '"' || operand1[1] == '`' || operand1[1] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 <= op2) {data[i].remove(); deleted++;}
				}
			}
		}
		else throw DBexception("Invalid condition");
	}
	cout << "Deleted " << deleted << "rows from table " << name << endl;
}

void node::remove(){
	this->prev->next = this->next;
	this->next->prev = this->prev;
	parent->s--;
	delete this;
}

string node::operator[](int index){
	return data[index];
}