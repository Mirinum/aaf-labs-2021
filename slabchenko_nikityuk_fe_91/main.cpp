#include "back.hpp"
#include "outputtools.hpp"

using namespace std;

void remove_extra_whitespaces(const string &input, string &output)
{
	output.clear();
	unique_copy (input.begin(), input.end(), back_insert_iterator<string>(output), [](char a,char b){ return isspace(a) && isspace(b);});
}

static inline string & ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(),
            not1(ptr_fun<int, int>(isspace))));
    return s;
}

static inline string & rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(),
            not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

static inline string & trim(string &s) {
    return ltrim(rtrim(s));
}

int main(){
	try{
		string tablename;
		vector <string> columns;
		vector <bool> indexes;
		while(true){
			cout << "> ";
			string command;
			getline(cin,command,';');
			string stripped_command="";
			stripped_command = trim(command);
			replace( stripped_command.begin(), stripped_command.end(), '\n', ' ');
			cout << "Entered command: " << stripped_command << endl;
			if(stripped_command == ".EXIT") return 0;
			if (regex_match (stripped_command, regex("(CREATE|create)\\s+[a-zA-Z][a-zA-Z0-9]*\\s*\\(\\s*\\w+(\\s+(indexed|INDEXED))?\\s*(,\\s*\\w+(\\s+(indexed|INDEXED))?\\s*)*\\)") )){
				//smatch match;
				//regex_search(stripped_command.begin(), stripped_command.end(), match, "create\\s([a-zA-Z][a-zA-Z0-9]).*");
				columns.clear();
				indexes.clear();
				tablename = "";
				remove_extra_whitespaces(command, stripped_command);
				vector <string> tokens;
				boost::split(tokens, stripped_command, boost::is_any_of("\t\n ()"));
				cout << "Created table named " << tokens[1] << endl << "Structure of the table: " << endl;
				tablename = tokens[1];
				smatch reg_match;
				regex re ("\\(([^\\(\\)]*)\\)");
				regex_search(stripped_command, reg_match, re);
				tokens.clear();
				boost::split(tokens, reg_match.str(1), boost::is_any_of(", \t\n"));
				for(int i = 0; i < tokens.size(); i++){
					if(tokens[i] != "indexed" && tokens[i] != "INDEXED" && tokens[i] != ""){
						columns.push_back(tokens[i]);
						indexes.push_back(false);
						if(tokens[i] != tokens.back()){
							if(tokens[i+1] == "indexed" || tokens[i+1] == "INDEXED"){
								indexes.back() = true;
							}
						}
					}
				}
				for(int i = 0; i < columns.size(); i++){
					cout << columns[i];
					if(indexes[i]) cout << " (indexed)";
					cout << endl;
				}
			}
			else if(regex_match (stripped_command, regex("(INSERT|insert)\\s+(INTO\\s+|into\\s+)?[a-zA-Z][a-zA-Z0-9]*\\s*\\(\\s*['`\"][^'\"`]+['`\"]\\s*(,\\s*['`\"][^'\"`]+['`\"]\\s*){"+to_string(columns.size()-1)+"}\\)") )){
				vector<string> insertion_values;
				string insertion_table;
				smatch name_match, values_match;
				regex name_regex ("((insert|INSERT)(\\s+into|\\s+INTO)?)\\s+(\\w+)");
				regex values_regex ("['\"`]([^'\"`]*?)['\"`]");
				regex_search(stripped_command, name_match, name_regex);
				while (regex_search(stripped_command, values_match, values_regex)) {
					insertion_values.push_back(values_match[0]);
					stripped_command = values_match.suffix();
				}
				cout << "Inserted into table named " <<  name_match[4] << endl;
				cout << "Inserted values: " << endl;
				for(int i = 0; i < insertion_values.size(); i++){
					cout << columns[i] << " inserted as " << insertion_values[i] << endl;
				}
			}
			else if(regex_match (stripped_command, regex("(SELECT|select)\\s+((\\w+\\s*(,\\s*\\w+\\s*)*)|\\*\\s*)(,\\s*(COUNT\\(\\w+\\)|COUNT_DISTINCT\\(\\w+\\)|MAX_LEN\\(\\w+\\)|AVG_LEN\\(\\w+\\)))*\\s+((from|FROM)\\s+[a-zA-Z][a-zA-Z0-9]*\\s*)(\\s*(where|WHERE)\\s+((\\w+)|(['\"`]\\w+['\"`]))\\s*[=!<>]{1,2}\\s*((\\w+)|(['\"`]\\w+['\"`])))?(\\s*(GROUP_BY|group_by)\\s+\\w+\\s*(,\\s*\\w+\\s*)*)?"))){
				cout << "Selecting "; 
				regex selection_regex("(SELECT|select)\\s+(.+)\\s+(FROM|from)");
				smatch selection_match;
				regex_search(stripped_command, selection_match, selection_regex);
				string select_parameter = selection_match.str(2);
				cout << select_parameter;
				regex name_regex ("from\\s+([a-zA-Z][a-zA-Z0-9]*)|FROM\\s+([a-zA-Z][a-zA-Z0-9]*)");
				smatch name_match;
				regex_search(stripped_command, name_match, name_regex);
				string table_name = name_match.str(0).substr(5);
				cout << " from table " << table_name << endl;

			}
			else if(regex_match (stripped_command, regex("(DELETE|delete)(\\s+from|\\s+FROM)?\\s+[a-zA-Z][a-zA-Z0-9]*(\\s+(where|WHERE)\\s+((\\w+)|(['\"`]\\w+['\"`]))\\s*[=!<>]{1,2}\\s*((\\w+)|(['\"`]\\w+['\"`]))?)?"))){
				cout << "Deletion command" << endl;
			}
			else{
				cout << "Command wasn't understood" << endl;
			}
			cin.ignore();
		}
	}
	catch(exception& error){
		otools::colored_out("Internal error\n", red);
		cout << error.what() << endl;
	}
}