#include "back.hpp"
#include "outputtools.hpp"
#include <climits>
#include <boost/foreach.hpp>

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
	int ind = index;
	while(index != 0){
		index--;
		if(temp->next == nullptr) throw DBexception("Invalid index! No "+to_string(ind)+" index");
		temp = temp->next;
	}
	return *temp;
}

table::table(string n, vector<string> c, vector<bool> ind, unsigned int c_n){
	name = n;
	if(find(ind.begin(), ind.end(), true) == ind.end()){
		throw DBexception("At least one columns has to be indexed");
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
	for (map<int, string>::iterator i = columns.begin(); i != columns.end(); ++i)
    	colnames[i->second] = i->first;
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

		//==============================================
		//
		//			CONTAMINATION ZONE
		//
		//==============================================

		vector<pair<string, string>> functions;
		for(int i = 0; i < source.size(); i++){
			regex r("^\\s*(count|count_distinct|max_len|avg_len)\\s*\\(\\s*([a-zA-Z]\\w+)\\s*\\)\\s*$", regex_constants::icase);
			smatch m;
			if(regex_search(source[i], m, r)){
				for(int j = 0; j < group_by.size(); j++)
					if(m[2] == group_by[j])
						throw DBexception("You can't use agg functions on column that is used for grouping");
				string func = m[1];
				transform(func.begin(), func.end(), func.begin(),[](unsigned char c){ return tolower(c); });
				functions.push_back(make_pair(func, m[2]));
				cout << setw(0) << "\033[33;1m" << setw(30) << func+"("+m[2].str()+")" << setw(0) << "\033[0m";
			}
			else{
				if(find(group_by.begin(), group_by.end(), source[i]) == group_by.end())
					throw DBexception("Syntax error, `from` statement should have the same coulumns as the `group_by` statement");
				else{
					cout << setw(0) << "\033[33;1m" << setw(30) << source[i] << setw(0) << "\033[0m";
					functions.push_back(make_pair("print", source[i]));
				}
			}


			
		}
		cout << endl;

		map <vector<string>, vector<int>> aggregated;

		for(int i = 0; i < data.size(); i++){
			vector<string> temp;
			for(string agg_param : group_by){
				temp.push_back(data[i][colnames[agg_param]]);
			}
			aggregated[temp].push_back(i);
		}

		/*############################

		REMOVING ANY LINES THAT DO NOT MATCH THE CONDITION

		#############################*/


		regex r("\\s*(((['\"`])[\\w\\s]*\\3)|\\w+)\\s*(=|!=|<|>|<=|>=)\\s*(((['\"`])[\\w\\s]*\\7)|\\w+)\\s*");
		smatch m;
		regex_search(condition, m, r);
		if(m[0]==""){}
		else if (m[0] == condition) {
			string operand1 = m[1].str();
			string operand2 = m[5].str();
			string op = m[4].str();
			int column1 = 0;
			int column2 = 0;
			if(operand1[0] != '"' && operand1[0] != '`' && operand1[0] != '\''){
				if(colnames.find(operand1) == colnames.end()) throw DBexception("Incorrect columns name");
				column1 = colnames.at(operand1);
			}
			if(operand2[0] != '"' && operand2[0] != '`' && operand2[0] != '\''){
				if(colnames.find(operand2) == colnames.end()) throw DBexception("Incorrect columns name");
				column2 = colnames.at(operand2);
			}

			for (auto& x : aggregated){
				vector<int>& agg = x.second;
				for(int i = 0; i < agg.size(); i++){

					node& curr = data[agg[i]];

					if(op == "="){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 != op2) {
								agg[i] = -1;
							}
					}
					else if(op == "!="){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 == op2) {
								agg[i] = -1;
							}
					}
					else if(op == "<"){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 >= op2) {
								agg[i] = -1;
							}
					}
					else if(op == ">"){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 <= op2) {
								agg[i] = -1;
							}
					}
					else if(op == "<="){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 > op2) {
								agg[i] = -1;
							}
					}
					else if(op == ">="){
							string op1="";
							if(operand1[0] == '"' || operand1[0] == '`' || operand1[0] == '\''){
								op1 = operand1;
								op1.erase(0, 1);
								op1.erase(op1.size() - 1);
							}
							else{
								op1 = curr[column1];
							}
							string op2="";
							if(operand2[0] == '"' || operand2[0] == '`' || operand2[0] == '\''){
								op2 = operand2;
								op2.erase(0, 1);
								op2.erase(op2.size() - 1);
							}
							else{
								op2 = curr[column2];
							}
							if(op1 < op2) {
								agg[i] = -1;
							}
					}
				}
			}
		}
		else{
			throw DBexception("Invalid condition");
		}


		/*############################

		PRINTING LEFTOVERS

		#############################*/

//		map <vector<string>, vector<int>> aggregated;
		int pos = -1; // piece of shit
		for (auto const& group : aggregated)
		{
			bool printed = false;
			for(int j = 0; j < functions.size(); j++){
				if(functions[j].first == "print"){
					vector<string> temp;
					for(int i = 0; i < group.second.size(); i++)
						if(group.second[i] != pos){
							cout << setw(30) << data[group.second[i]][colnames[functions[j].second]];
							printed = true;
							break;}
				}
				else if(functions[j].first == "count"){
					int counted = count_if(group.second.begin(), group.second.end(), [](int i){return i != -1;});
					if(counted){
						cout << setw(30) << counted;
						printed = true;}
				}
				else if(functions[j].first == "count_distinct"){
					vector <string> temp;
					for(int i = 0; i < group.second.size(); i++){
						if(group.second[i] != pos)
							temp.push_back(data[group.second[i]][colnames[functions[j].second]]);
					}
					sort(temp.begin(), temp.end());
					int counted = unique(temp.begin(), temp.end()) - temp.begin();
					if(counted){
						cout << setw(30) << counted;
						printed = true;}
				}
				else if(functions[j].first == "max_len"){
					vector <string> temp;
					for(int i = 0; i < group.second.size(); i++){
						if(group.second[i] != pos)
							temp.push_back(data[group.second[i]][colnames[functions[j].second]]);
					}
					auto longest = max_element(temp.begin(), temp.end(), [](const auto& a, const auto& b) {return a.size() < b.size();});

					int counted = longest->length();
					if(counted && printed){
						cout << setw(30) << counted;
						printed = true;}
				}
				else if(functions[j].first == "avg_len"){
					vector <string> temp;
					for(int i = 0; i < group.second.size(); i++){
						if(group.second[i] != pos)
							temp.push_back(data[group.second[i]][colnames[functions[j].second]]);
					}
					float average = 0;
					for(int i = 0; i < temp.size(); i++)
						average+=temp[i].length();
					average /= temp.size();
					if(average && printed){
						cout << setw(30) << average;
						printed = true;}
				}
			}
			if(printed) cout << endl;
		}

		return;
	}

		//==============================================
		//
		//			CONTAMINATION ZONE
		//
		//==============================================

	if(condition == ""){
		if(indexes[main_index].size() == 0){
			cout << "Table is empty" << endl;
			return;
		}
		if(source.size() == 1 && source[0] == "*"){
			for(int i = 0; i < columns.size(); i++){
				cout << setw(0) << "\033[33;1m" << setw(30) << columns[i] << setw(0) << "\033[0m";
			}
			cout << endl;

			// HIGH QUARANTINE
			for(int i = 0; i < data.size(); i++){
				for(int j = 0; j < columns.size(); j++){
					cout << setw(30) << data[i][j];
				}
				cout << endl;
			}
			// HIGH QUARANTINE
			return;
		}
		for(int i = 0; i < source.size(); i++) if(colnames.find(source[i]) != colnames.end()) cout << setw(0) << "\033[33;1m" << setw(30) << source[i] << setw(0) << "\033[0m";
		cout << endl;
			
		for (int i = 0; i < data.size(); i++){
			for(int j = 0; j < source.size(); j++){
				if(colnames.find(source[j]) != colnames.end()) 
					cout << setw(30) << data[i][colnames.at(source[j])];
			}
			cout << endl;
		}

		//==============================================
		//
		//			CONTAMINATION ZONE
		//
		//==============================================
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
			if(operand1[0] != '"' && operand1[0] != '`' && operand1[0] != '\''){
				if(colnames.find(operand1) == colnames.end()) throw DBexception("Incorrect columns name");
				column1 = colnames.at(operand1);
			}
			if(operand2[0] != '"' && operand2[0] != '`' && operand2[0] != '\''){
				if(colnames.find(operand2) == colnames.end()) throw DBexception("Incorrect columns name");
				column2 = colnames.at(operand2);
			}
			if(indexes.find(operand1) != indexes.end() || indexes.find(operand2) != indexes.end()){
				cout << "INDEXED SEARCH" << endl;
				map<string, vector<node*>>* temp;
				string comp, comparation;
				if(indexes.find(operand1) != indexes.end())
					{temp = &indexes[operand1]; comp = operand2;}
				else
					{temp = &indexes[operand2]; comp = operand1; 
						if(comp[0] == '"' || comp[0] == '`' || comp[0] == '\''){
							if(op[0]=='<') op[0]='>';
							else if (op[0]=='>') op[0]='<';
						}
					}
				map<string, vector<node*>>& index = *temp;

				vector<node*> results;
					pair<string, vector<node*>> i;
					if(op == "="){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] == data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							if(index.find(comparation) != index.end())
								results.insert(results.end(), index.at(comparation).begin(), index.at(comparation).end());
						}
					}
					else if(op == "!="){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] != data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							for (auto const& x : index){
								if(x.first != comparation)
									results.insert(results.end(), x.second.begin(), x.second.end());
							}
						}
					}
					else if(op == "<"){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] < data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							for (auto const& x : index){
								if(x.first < comparation)
									results.insert(results.end(), x.second.begin(), x.second.end());
							}
						}
					}
					else if(op == "<="){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] <= data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							for (auto const& x : index){
								if(x.first <= comparation)
									results.insert(results.end(), x.second.begin(), x.second.end());
							}
						}
					}
					else if(op == ">"){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] > data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							for (auto const& x : index){
								if(x.first > comparation)
									results.insert(results.end(), x.second.begin(), x.second.end());
							}
						}
					}
					else if(op == ">="){
						if(comp[0] != '"' && comp[0] != '`' && comp[0] != '\''){
							for(int i = 0; i < data.size(); i++){
								if(data[i][column1] >= data[i][column2]) results.push_back(&(data[i]));
							}
						}
						else{
							comparation = comp;
							comparation.erase(0, 1);
							comparation.erase(comparation.size() - 1);
							for (auto const& x : index){
								if(x.first >= comparation)
									results.insert(results.end(), x.second.begin(), x.second.end());
							}
						}
					}
					else{
						throw DBexception("Invalid condition");
					}

				if(source.size() == 1 && source[0] == "*"){
					for(int i = 0; i < columns.size(); i++) cout << setw(0) << "\033[33;1m" << setw(30) << columns[i] << setw(0) << "\033[0m";
					cout << endl;
					for (int i = 0; i < results.size(); i++){
						for(int j = 0; j < columns.size(); j++){
							cout << setw(30) << (*results[i])[j];
						}
						cout << endl;
					}
				}
				else{
					for(int i = 0; i < source.size(); i++) if( colnames.find(source[i]) != colnames.end() )cout << setw(0) << "\033[33;1m" << setw(30) << source[i] << setw(0) << "\033[0m";
					cout << endl;
						
					for (int i = 0; i < results.size(); i++){
						for(int j = 0; j < source.size(); j++){	
							if(colnames.find(source[j]) != colnames.end())
								cout << setw(30) << (*results[i])[colnames.at(source[j])];
						}
						cout << endl;
					}
				}
			}
			else{
				

				cout << endl;
				if(source[0] != "*"){
					for(int i = 0; i < source.size(); i++) cout << setw(0) << "\033[33;1m" << setw(30) << source[i] << setw(0) << "\033[0m";
					cout << endl;
				}
				else{
					for(int i = 0; i < columns.size(); i++) cout << setw(0) << "\033[33;1m" << setw(30) << columns[i] << setw(0) << "\033[0m";
					cout << endl;
				}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
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
							if(source[0] != "*"){
								for(int j = 0; j < source.size(); j++)
									cout << setw(30) << data[i][colnames[source[j]]];
								cout << endl;
							}
							else{
								for(int j = 0; j < columns.size(); j++)
									cout << setw(30) << data[i][j];
								cout << endl;
							}
						}
					}
				}
			}
		}
		else{
			throw DBexception("Invalid condition");
		}
	}
	/*else{
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
				column1 = colnames[operand1];
			if(operand2[1] != '"' || operand2[1] != '`' || operand2[1] != '\'')
				column2 = colnames[operand2];
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
	}*/
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
							if(indexes.count(columns[j]) == 1){
								vector<node*>& temp = indexes.at(columns[j]).at(data[i][j]);
								auto k = find(begin(temp), end(temp), &(data[i]));
								if(temp.erase(k) == temp.end()) cout << "ERASED SHITFUCK" << endl;
							}
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