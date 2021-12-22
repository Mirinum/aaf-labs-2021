#include "back.hpp"
#include "outputtools.hpp"

using namespace std;

int main(){
	vector<table> tables;
	while(true){
		try{
			cout << "> ";
			string command, cmd;
			getline( cin, command, ';' );
			command += ";";
			if(command == ".EXIT;") return 0;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			regex command_check("\\s*(create|insert|select|delete)\\s+.*\\;", regex_constants::icase);
			regex create("\\s*create\\s+([a-zA-Z]\\w*)\\s*\\((.*)\\)\\s*\\;", regex_constants::icase);
			regex insert("\\s*insert\\s+(?:into\\s+)?([a-zA-Z]\\w*)\\s*\\((.*)\\)\\s*\\;", regex_constants::icase);
			regex select("^\\s*select\\s+(.*)\\s+from\\s+([a-zA-Z]\\w*)(?:\\s+where\\s+(.*?))?(?:\\s+group_by\\s+(.*))?\\s*;$", regex_constants::icase);
			regex remove_all("^\\s*delete\\s+(?:from\\s+)?([a-zA-Z]\\w*);$", regex_constants::icase);
			regex remove("^\\s*delete\\s+(?:from\\s+)?([a-zA-Z]\\w*)\\s+where\\s+(.+(?:=|!=|<|>|<=|>=).+);$", regex_constants::icase);
			regex create_params("^\\s*([a-zA-Z]\\w*)(?:(\\s+indexed))?\\s*$", regex_constants::icase);
			regex insert_params("(['\"`])([^'\"`]*)\\1");
			smatch m;
			replace( command.begin(), command.end(), '\n', ' ');
			if(regex_search(command, m, command_check) == 0) {
				throw DBexception("Syntax error!");
			}
			cmd = m[1];
			transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
			if (m[0] != command) {
				throw DBexception("Syntax error!");
			}
			if(cmd == "CREATE"){
				if(regex_search(command, m, create) != 0){
					string name = m[1];
					bool toBreak = false;
					for(int i = 0; i < tables.size(); i++){
						if(tables[i].get_name() == name){
							throw DBexception("Table with this name already exists");
						}
					}
					string col = m[2];
					vector<string> cols = otools::explode(col, ',');
					vector<string> columns;
					vector<bool> indexes;
					for(int i = 0; i < cols.size(); i++){
						if(regex_search(cols[i], m, create_params) != 0){
							string indexing_parameter = m[2];
							//transform(indexing_parameter.begin(), indexing_parameter.end(), indexing_parameter.begin(), ::toupper);
							if(indexing_parameter != "")
								indexes.push_back(1);
							else
								indexes.push_back(0);
							columns.push_back(m[1]);
						}
						else{
							throw DBexception("Syntax error!");
						}
					}
					tables.push_back(table(name, columns, indexes, columns.size()));
				}
				else{
					throw DBexception("Syntax error!");
				}
			}
			if(cmd == "INSERT"){
				if(regex_search(command, m, insert) != 0){
					string name = m[1];
					string par = m[2];
					vector<string> params = otools::explode(par, ',');
					vector<string> parameters;
					bool found = false;
					bool parameters_found = false;
					for(int i = 0; i < tables.size(); i++){
						if(tables[i].get_name() == name){
							found = true;
							for(int j = 0; j < params.size(); j++){
								if(regex_search(params[j], m, insert_params) != 0){
									parameters.push_back(m[2]);
								}
								else throw DBexception("Invalid parameters");
							}
							tables[i].insert(parameters);
						}
					}
					if(!found){
						otools::colored_out("No table with this name\n", red);
						continue;
					}
				}
				else{
					otools::colored_out("Syntax error\n", red);
					continue;
				}
			}
			if(cmd == "DELETE"){
				if(regex_search(command, m, remove_all) != 0){
					for(int i = 0; i < tables.size(); i++){
						if(tables[i].get_name() == m[1]){
							tables[i].remove("'1'='1'");
						}
					}
				}
				else if(regex_search(command, m, remove) != 0){
					for(int i = 0; i < tables.size(); i++){
						if(tables[i].get_name() == m[1]){
							tables[i].remove(m[2]);
						}
					}
				}
				else{
					otools::colored_out("Syntax error\n", red);
					continue;
				}
			}
			if(cmd == "SELECT"){
				if(regex_search(command, m, select) != 0){
					string name = m[2];
					string src = m[1];
					string condition = m[3];
					string grouping = m[4];
					src.erase(remove_if(src.begin(), src.end(), ::isspace), src.end());
					src.erase(remove_if(grouping.begin(), grouping.end(), ::isspace), grouping.end());
					vector<string> source = otools::explode(src, ',');
					vector<string> groups = otools::explode(grouping, ',');
					for(int i = 0; i < tables.size(); i++){
						if(tables[i].get_name() == name){
							tables[i].select(source, condition, groups);
						}
					}
				}
				else{
					otools::colored_out("Syntax error\n", red);
					continue;
				}
			}
		}
		catch(exception& error){
			otools::colored_out("Internal error\n", red);
			cout << error.what() << endl;
		}
	}
}