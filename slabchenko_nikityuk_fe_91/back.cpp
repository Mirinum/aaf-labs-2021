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

table::table(string n, vector<string> c, vector<bool> ind, unsigned int c_n){
	name = n;
	if(find(ind.begin(), ind.end(), true) == ind.end()){
		throw DBexception("At least one column has to be indexed");
	}
	for(int i = 0; i < c.size(); i++){
		for(int j = i+1; j < c.size(); j++){
			if(c[i] == c[j]){
				throw DBexception("Error, repeated columns");
			}
		}
	}
	for(int i = 0; i < c_n; i++){
		columns.insert(pair<int, string>(i, c[i]));
		if(ind[i]){
			if(main_index == "") main_index = c[i];
			indexes.insert(pair<string,map<string,vector<node*>>>(c[i], map<string,vector<node*>>()));
		}
	}
	cout << "Created table " << name << " with columns as such: " << endl << "|";
	for(int i = 0; i < columns.size(); i++){
		cout << setw(30) << columns[i];
		indexes.find(columns[i]) != indexes.end() ?  cout << "(ind) | " : cout << " | " ;
	}
	cout << endl;
}

void table::insert(vector<string> d){
	if(d.size() != columns.size()) throw DBexception("Invalid quantity of parameters!");
	node* new_node = data.push_back(d);
	for(int i = 0; i < columns.size(); i++){
		if(indexes.count(columns[i]) == 1){
			if(indexes[columns[i]].find(d[i]) == indexes[columns[i]].end())
				indexes.at(columns[i]).insert(pair<string, vector<node*>>(d[i], vector<node*>(1, new_node)));
			else
				indexes.at(columns[i])[d[i]].push_back(new_node);
		}
	}
	cout << "Inserted 1 row into " << name << endl;
}

void table::select(vector<string> source, string condition, vector<string> group_by){

	if(group_by.size()){
		for(int i = 0; i < source.size(); i++)
			if(find(group_by.begin(), group_by.end(), source[i]) == group_by.end())
				throw DBexception("Syntax error, `from` statement should have the same coulumns as the `group_by` statement");
			else
				cout << setw(30) << source[i];
		cout << endl;

		map <string, vector<node*>> aggregated;

		//TODO DO IT

		return;
	}

	vector<bool> printable;
	if(source[0] == "*"){
		printable = vector<bool>(columns.size(), true);
	}
	else{
		printable = vector<bool>(columns.size(), false);
		for(int i = 0; i < columns.size(); i++){
			for(string s : source){
				if(s == columns[i]) printable[i] = true;
			}
		}
	}

	if(condition == ""){
		if(indexes[main_index].size() == 0){
			cout << "Table is empty" << endl;
			return;
		}
		for(int i = 0; i < columns.size(); i++) if(printable[i]) cout << setw(0) << "\033[33;1m" << setw(30) << columns[i] << setw(0) << "\033[0m";
		cout << endl;
			
		for (auto const& row : indexes[main_index]){
			for(int j = 0; j < row.second.size(); j++){
				for(int i = 0; i < columns.size(); i++)
					if(printable[i]) cout << setw(30) << (*row.second[j])[i];
				cout << endl;
			}
		}
	}
	else{
		regex r("\\s*(((['\"`])[\\w\\s]*\\3)|\\w+)\\s*(=|!=|<|>|<=|>=)\\s*(((['\"`])[\\w\\s]*\\7)|\\w+)\\s*");
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
			for(int i = 0; i < columns.size(); i++) if(printable[i]) cout << setw(0) << "\033[33;1m" << setw(30) << columns[i] << setw(0) << "\033[0m";
			cout << endl;
			if(op == "="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 == op2) {

						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
			else if(op == "!="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 != op2) {
						
						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
			else if(op == "<"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 < op2) {
						
						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
			else if(op == ">"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 > op2) {
						
						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
			else if(op == ">="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 >= op2) {
						
						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
			else if(op == "<="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 <= op2) {
						
						for(int j = 0; j < columns.size(); j++)
							if(printable[j]) cout << setw(30) << data[i][j];
						cout << endl;
					}
				}
			}
		}
		else{
			throw DBexception("Invalid condition");
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
		for(; data.size() != 0; ){
			data[0].remove();
			deleted++;
		}
	}
	else{
		regex r("\\s*(((['\"`])[\\w\\s]*\\3)|\\w+)\\s*(=|!=|<|>|<=|>=)\\s*(((['\"`])[\\w\\s]*\\7)|\\w+)\\s*");
		smatch m;
		regex_search(condition, m, r);
		if (m[0] == condition) {
			string operand1 = m[1].str();
			string operand2 = m[5].str();
			string op = m[4].str();
			int column1 = 0;
			int column2 = 0;
			if(operand1[0] != '"' && operand1[0] != '`' && operand1[0] != '\'')
				for(int j = 0; j < columns.size(); j++) 
					if(columns[j] == operand1)
						column1 = j;
			if(operand2[0] != '"' && operand2[0] != '`' && operand2[0] != '\'')
				for(int j = 0; j < columns.size(); j++) 
					if(columns[j] == operand2)
						column2 = j;
			if(op == "="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 == op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1)
								indexes.at(columns[j]).erase(data[i][j]);
						}
						data[i].remove(); 
						deleted++;
						i--;
					}
				}
			}
			else if(op == "!="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 != op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1){
									indexes.at(columns[j]).erase(data[i][j]);
							}
						}
						data[i].remove(); 
						deleted++;
						i--; // если так не сделать, оно инкрементируется, и прыгает через ставшую на место удалённой ноду
					}
				}
			}
			else if(op == "<"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 < op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1){
									indexes.at(columns[j]).erase(data[i][j]);
							}
						}
						data[i].remove(); 
						deleted++;
						i--; // если так не сделать, оно инкрементируется, и прыгает через ставшую на место удалённой ноду
					}
				}
			}
			else if(op == ">"){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 > op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1){
									indexes.at(columns[j]).erase(data[i][j]);
							}
						}
						data[i].remove(); 
						deleted++;
						i--; // если так не сделать, оно инкрементируется, и прыгает через ставшую на место удалённой ноду
					}
				}
			}
			else if(op == ">="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 >= op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1){
									indexes.at(columns[j]).erase(data[i][j]);
							}
						}
						data[i].remove(); 
						deleted++;
						i--; // если так не сделать, оно инкрементируется, и прыгает через ставшую на место удалённой ноду
					}
				}
			}
			else if(op == "<="){
				for(int i = 0; i < data.size(); i++){
					string op1="";
					if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
						op1 = operand1;
						op1.erase(0, 1);
						op1.erase(op1.size() - 1);
					}
					else{
						op1 = data[i][column1];
					}
					string op2="";
					if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
						op2 = operand2;
						op2.erase(0, 1);
						op2.erase(op2.size() - 1);
					}
					else{
						op2 = data[i][column2];
					}
					if(op1 <= op2) {
						for(int j = 0; j < columns.size(); j++){
							if(indexes.count(columns[j]) == 1){
									indexes.at(columns[j]).erase(data[i][j]);
							}
						}
						data[i].remove(); 
						deleted++;
						i--; // если так не сделать, оно инкрементируется, и прыгает через ставшую на место удалённой ноду
					}
				}
			}
		}
		else throw DBexception("Invalid condition");
	}
	cout << "Deleted " << deleted << " rows from table " << name << endl;
}

void node::remove(){
	if(this->prev != nullptr) this->prev->next = this->next;
	if(this->next != nullptr) this->next->prev = this->prev;
	if(parent->root == this) parent->root = this->next;
	parent->s--;
	delete this;
}

string node::operator[](int index){
	return data[index];
}